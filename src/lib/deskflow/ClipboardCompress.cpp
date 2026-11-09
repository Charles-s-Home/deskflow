/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "deskflow/ClipboardCompress.h"

#include "base/Log.h"

#include <QByteArray>

#include <cstdint>
#include <cstring>

namespace {

constexpr std::string_view kMagic = "DFZ1";
// qCompress() level, 1 is fast and still shrinks bitmaps a lot
constexpr int kLevel = 1;
// size of the original that qCompress() puts in front of the data
constexpr size_t kSizeHeader = 4;

} // namespace

namespace ClipboardCompress {

std::optional<std::string> compress(std::string_view data)
{
  if (data.empty()) {
    return std::nullopt;
  }

  const QByteArray packed =
      qCompress(reinterpret_cast<const uchar *>(data.data()), static_cast<qsizetype>(data.size()), kLevel);
  if (packed.isEmpty() || kMagic.size() + static_cast<size_t>(packed.size()) >= data.size()) {
    return std::nullopt;
  }

  std::string out;
  out.reserve(kMagic.size() + static_cast<size_t>(packed.size()));
  out.append(kMagic);
  out.append(packed.constData(), static_cast<size_t>(packed.size()));
  return out;
}

bool isCompressed(const std::string &data)
{
  return data.size() >= kMagic.size() && std::memcmp(data.data(), kMagic.data(), kMagic.size()) == 0;
}

bool decompress(std::string &data, size_t maxSize)
{
  if (!isCompressed(data)) {
    return true;
  }

  auto fail = [&data](const char *why) {
    LOG_ERR("dropping compressed clipboard data, %s", why);
    data.clear();
    data.shrink_to_fit();
    return false;
  };

  if (data.size() < kMagic.size() + kSizeHeader) {
    return fail("truncated");
  }

  // check the claimed size before qUncompress() allocates it
  const auto *p = reinterpret_cast<const uint8_t *>(data.data()) + kMagic.size();
  const size_t expected = (static_cast<size_t>(p[0]) << 24) | (static_cast<size_t>(p[1]) << 16) |
                          (static_cast<size_t>(p[2]) << 8) | static_cast<size_t>(p[3]);
  if (expected == 0 || expected > maxSize) {
    LOG_WARN("not accepting clipboard data, uncompressed size %zu exceeds limit %zu", expected, maxSize);
    return fail("too large");
  }

  const QByteArray original = qUncompress(
      reinterpret_cast<const uchar *>(data.data()) + kMagic.size(), static_cast<qsizetype>(data.size() - kMagic.size())
  );
  if (static_cast<size_t>(original.size()) != expected) {
    return fail("corrupted");
  }

  data.assign(original.constData(), static_cast<size_t>(original.size()));
  return true;
}

} // namespace ClipboardCompress
