// $Id$
// NORA Destroy Callbacks

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
// Revision 1.7  1993/05/22  20:09:30  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.6  1993/04/22  11:04:09  zeller
// Lizenz verbessert
// 
// Revision 1.5  1993/04/16  11:41:48  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.4  1993/02/25  15:43:50  zeller
// Neu: UnmanageShellCB, UnmanageThisCB
// 
// Revision 1.3  1992/11/13  00:37:01  zeller
// Fix: Deklarationen jetzt typkorrekt
// 
// Revision 1.2  1992/11/05  11:52:25  zeller
// Umbenennung: CB ans Ende gesetzt (sonst Verwechslung mit Klassen)
// 
// Revision 1.1  1992/02/20  14:20:44  zeller
// Initial revision
// 

char DestroyCB_rcsid[] = "$Id$";

#include "findParent.h"
#include "DestroyCB.h"


// callbacks

// destroy the surrounding shell
void DestroyShellCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget shell = findShellParent(widget);
    XtDestroyWidget(shell);
}

// destroy specific widget
void DestroyThisCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget w = Widget(client_data);
    XtDestroyWidget(w);
}

// unmanage the surrounding shell
void UnmanageShellCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget shell = findShellParent(widget);
    XtUnmanageChild(shell);
}

// unmanage specific widget
void UnmanageThisCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget w = Widget(client_data);
    XtUnmanageChild(w);
}
