/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

//! Optional compression of marshalled clipboard data
/*!
A compressed payload is the 4 byte magic "DFZ1" followed by the output of
qCompress() (which starts with the 4 byte big endian size of the original).
Marshalled clipboard data starts with the format count, a small number, so it
can never start with the magic. A receiver therefore tells the two apart by
looking at the first 4 bytes. A sender may only compress for a peer that
announced it understands this (see kOptionClipboardCompress).
*/
namespace ClipboardCompress {

//! Compress \p data, nothing if the result would not be smaller
std::optional<std::string> compress(std::string_view data);

//! True if \p data starts with the magic
bool isCompressed(const std::string &data);

//! Replace compressed \p data with the original
/*!
Does nothing and returns true if \p data is not compressed. Returns false and
clears \p data if it is malformed or the original would be larger than
\p maxSize bytes.
*/
bool decompress(std::string &data, size_t maxSize);

} // namespace ClipboardCompress
