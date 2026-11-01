/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Deskflow Developers
 * SPDX-FileCopyrightText: (C) 2012 - 2016 Synergy App Ltd
 * SPDX-FileCopyrightText: (C) 2003 Chris Schoeneman
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include "deskflow/ClipboardTypes.h"

#include <cstdint>
#include <vector>

class IClipboard;

//! Rectangle of one physical monitor, in the same coordinates as getShape()
struct MonitorRect
{
  int32_t m_x = 0;
  int32_t m_y = 0;
  int32_t m_w = 0;
  int32_t m_h = 0;
};

//! Computer interface
/*!
This interface defines the methods common to all computers.
*/
class IComputer
{
public:
  virtual ~IComputer() = default;
  struct ClipboardInfo
  {
  public:
    ClipboardID m_id;
    uint32_t m_sequenceNumber;
  };

  //! @name accessors
  //@{

  //! Get event target
  /*!
  Returns the target used for events created by this object.
  */
  virtual void *getEventTarget() const = 0;

  //! Get clipboard
  /*!
  Save the contents of the clipboard indicated by \c id and return
  true iff successful.
  */
  virtual bool getClipboard(ClipboardID id, IClipboard *) const = 0;

  //! Get computer shape
  /*!
  Return the position of the upper-left corner of the computer in \c x and
  \c y and the size of the computer in \c width and \c height.
  */
  virtual void getShape(int32_t &x, int32_t &y, int32_t &width, int32_t &height) const = 0;

  //! Get monitor rectangles
  /*!
  Return the rectangle of each physical monitor, in the coordinates used by
  getShape().  The default is an empty list, meaning the computer is treated
  as one solid rectangle.
  */
  virtual const std::vector<MonitorRect> &getMonitorRects() const
  {
    static const std::vector<MonitorRect> none;
    return none;
  }

  //! Get cursor position
  /*!
  Return the current position of the cursor in \c x and \c y.
  */
  virtual void getCursorPos(int32_t &x, int32_t &y) const = 0;

  //@}
};
