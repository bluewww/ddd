// $Id$  -*- C++ -*-
// Delay class -- displays an hourglass cursor until destroyed

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

// $Log$
// Revision 1.1  1995/05/01 15:47:37  zeller
// Initial revision
//
// Revision 1.8  1995/04/24  11:38:21  zeller
// New: shells() returns all registered shells
//
// Revision 1.7  1995/04/06  15:35:28  zeller
// New: place delay cursors on registered shells
//
// Revision 1.6  1993/10/15  19:56:10  zeller
// Neu: TypeInfo
//
// Revision 1.5  1993/05/22  20:09:27  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.4  1993/04/22  11:04:01  zeller
// Lizenz verbessert
// 
// Revision 1.3  1993/04/20  23:02:14  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.2  1993/04/16  11:41:47  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.1  1992/11/05  11:52:59  zeller
// Initial revision
// 

#ifndef _Nora_Delay_h
#define _Nora_Delay_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "TypeInfo.h"
#include "VarArray.h"


// _Delay defines a delay for a specific widget only
class _Delay {
public:
    DECLARE_TYPE_INFO;

private:
    Widget widget;
    Cursor old_cursor;

    static Cursor current_cursor;
    static Cursor hourglass_cache;

protected:
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
    DECLARE_TYPE_INFO;

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
};

#endif // _Nora_Delay_h
// DON'T ADD ANYTHING BEHIND THIS #endif
