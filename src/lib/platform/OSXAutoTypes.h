/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2022 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#pragma once

#include <Carbon/Carbon.h>
#include <dispatch/dispatch.h>
#include <memory>
#include <pthread.h>
#include <type_traits>

using CFDeallocator = decltype(&CFRelease);
using AutoCFArray = std::unique_ptr<const __CFArray, CFDeallocator>;
using AutoCFDictionary = std::unique_ptr<const __CFDictionary, CFDeallocator>;
using AutoTISInputSourceRef = std::unique_ptr<__TISInputSource, CFDeallocator>;

// Text Input Sources (TIS) calls must run on the main thread: on macOS 27 they assert
// and crash the process when the input source list has to be rebuilt on another thread
// (seen when leaving the computer, from pollActiveGroup() on the core thread). Running
// them all on the main thread also serializes them. The core's main thread runs the
// NSApplication loop, which serves the main queue.
template <typename F> void runOnMainThread(F &&f)
{
  if (pthread_main_np()) {
    f();
    return;
  }
  using Fn = std::remove_reference_t<F>;
  dispatch_sync_f(dispatch_get_main_queue(), &f, [](void *context) { (*static_cast<Fn *>(context))(); });
}
