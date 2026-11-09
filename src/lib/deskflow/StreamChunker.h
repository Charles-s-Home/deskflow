/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2013 - 2016 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include "deskflow/ClipboardTypes.h"

#include <string_view>

class IEventQueue;

class StreamChunker
{
public:
  //! Send marshalled clipboard \p data in chunks
  /*!
  With \p compress the data is sent compressed if that makes it smaller; only
  pass it for a peer that has agreed to receive compressed data.
  */
  static void sendClipboard(
      const std::string_view &data, size_t size, ClipboardID id, uint32_t sequence, IEventQueue *events,
      void *eventTarget, bool compress = false
  );
};
