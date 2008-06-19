// $Id$  -*- C++ -*-
// Delay class -- displays an hourglass cursor until destroyed

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_Delay_h
#define _DDD_Delay_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#endif

#include "TypeInfo.h"
#include "VarArray.h"
#include "assert.h"

#if defined(IF_XM)
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
#else
// _Delay defines a delay for a specific widget only
class _Delay {
public:
    DECLARE_TYPE_INFO

private:
    GUI::Widget *widget;
    GUI::Cursor *old_cursor;

    static GUI::Cursor *current_cursor;
    static GUI::Cursor *hourglass_cache;

    _Delay(const _Delay&);
    _Delay& operator = (const _Delay&);

protected:
    static void *DestroyCB(void *);
    virtual GUI::Cursor *hourglass_cursor();

public:
    _Delay(GUI::Widget *w);
    virtual ~_Delay();
};
#endif

#if defined(IF_XM)
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

    Delay(const Delay&);
    Delay& operator = (const Delay&);

public:
    Delay(Widget w = 0);
    virtual ~Delay();
    static void register_shell(Widget w);
    static const WidgetArray& shells() { return _shells; }

    static void (*shell_registered)(Widget w);
};
#else
typedef VarArray<GUI::Widget *>   WidgetArray;
typedef VarArray<_Delay *> DelayArray;

// Delay also defines delays for all registered shells
class Delay: public _Delay {
public:
    DECLARE_TYPE_INFO

private:
    static int delay_count;
    static WidgetArray _shells;
    static DelayArray delays;
    static void DestroyCB(GUI::Widget *);


    Delay(const Delay&);
    Delay& operator = (const Delay&);

public:
    Delay(GUI::Widget *w = 0);
    virtual ~Delay();
    static void register_shell(GUI::Widget *w);
    static const WidgetArray& shells() { return _shells; }

    static void (*shell_registered)(GUI::Widget *w);
};
#endif

#endif // _DDD_Delay_h
// DON'T ADD ANYTHING BEHIND THIS #endif
