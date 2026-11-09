/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2013 - 2016 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "deskflow/StreamChunker.h"

#include "base/Event.h"
#include "base/IEventQueue.h"
#include "base/Log.h"
#include "deskflow/ClipboardChunk.h"
#include "deskflow/ClipboardCompress.h"

#include <chrono>
#include <optional>
#include <string>

static const size_t g_chunkSize = 512 * 1024; // 512kb

void StreamChunker::sendClipboard(
    const std::string_view &data, size_t size, ClipboardID id, uint32_t sequence, IEventQueue *events,
    void *eventTarget, bool compress
)
{
  const size_t originalSize = size;
  std::optional<std::string> packed;
  std::string_view payload = data.substr(0, size);
  if (compress) {
    const auto begin = std::chrono::steady_clock::now();
    packed = ClipboardCompress::compress(payload);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin);
    if (packed) {
      LOG_DEBUG(
          "compressed clipboard %d from %zu to %zu bytes in %lld ms", id, originalSize, packed->size(),
          static_cast<long long>(ms.count())
      );
      payload = *packed;
      size = packed->size();
    } else {
      LOG_DEBUG("clipboard %d not compressible, sending %zu bytes as is", id, originalSize);
    }
  }

  // send first message (data size)
  std::string dataSize = QString::number(size).toStdString();
  ClipboardChunk *sizeMessage = ClipboardChunk::start(id, sequence, dataSize);

  events->addEvent(Event(EventTypes::ClipboardSending, eventTarget, sizeMessage));

  // send clipboard chunk with a fixed size
  size_t sentLength = 0;
  size_t chunkSize = g_chunkSize;

  while (true) {
    // make sure we don't read too much from the mock data.
    if (sentLength + chunkSize > size) {
      chunkSize = size - sentLength;
    }

    std::string chunk(payload.substr(sentLength, chunkSize));
    ClipboardChunk *dataChunk = ClipboardChunk::data(id, sequence, chunk);

    events->addEvent(Event(EventTypes::ClipboardSending, eventTarget, dataChunk));

    sentLength += chunkSize;
    if (sentLength == size) {
      break;
    }
  }

  // send last message
  ClipboardChunk *end = ClipboardChunk::end(id, sequence);

  events->addEvent(Event(EventTypes::ClipboardSending, eventTarget, end));

  LOG_DEBUG("sent clipboard size=%zu (uncompressed %zu)", sentLength, originalSize);
}
