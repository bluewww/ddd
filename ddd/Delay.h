// $Id$  -*- C++ -*-
// Delay class -- displays an hourglass cursor until destroyed

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_Delay_h
#define _DDD_Delay_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "TypeInfo.h"
#include "VarArray.h"
#include "assert.h"

// _Delay defines a delay for a specific widget only
class _Delay {
public:
    DECLARE_TYPE_INFO

private:
    Widget widget;
    Cursor old_cursor;

    static Cursor current_cursor;
    static Cursor hourglass_cache;

    _Delay(const _Delay&);
    _Delay& operator = (const _Delay&);

protected:
    static void DestroyCB(Widget, XtPointer, XtPointer);
    virtual Cursor hourglass_cursor();

public:
    _Delay(Widget w);
    virtual ~_Delay();
};

typedef VarArray<Widget>   WidgetArray;
typedef VarArray<_Delay *> DelayArray;

// Delay also defines delays for all registered shells
class Delay: public _Delay {
public:
    DECLARE_TYPE_INFO

private:
    static int delay_count;
    static WidgetArray _shells;
    static DelayArray delays;
    static void DestroyCB(Widget, XtPointer, XtPointer);

public:
    Delay(Widget w = 0);
    virtual ~Delay();
    static void register_shell(Widget w);
    static const WidgetArray& shells() { return _shells; }

    static void (*shell_registered)(Widget w);
};

#endif // _DDD_Delay_h
// DON'T ADD ANYTHING BEHIND THIS #endif
