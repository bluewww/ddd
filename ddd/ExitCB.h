// $Id$
// Exit Callbacks

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
// Revision 1.1  1995/05/01 15:47:22  zeller
// Initial revision
//
// Revision 1.8  1993/05/22  20:09:37  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.7  1993/04/22  11:04:11  zeller
// Lizenz verbessert
// 
// Revision 1.6  1993/04/20  23:02:16  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.5  1993/04/16  11:41:50  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.4  1992/11/05  11:52:38  zeller
// Umbenennung: CB ans Ende gesetzt (sonst Verwechslung mit Klassen)
// 
// Revision 1.3  1992/11/04  11:53:22  zeller
// Callbacks korrekt deklariert
// 
// Revision 1.2  1992/06/01  17:04:44  zeller
// #pragma once entfernt
// 
// Revision 1.1  1992/02/20  14:20:46  zeller
// Initial revision
// 

#ifndef _Nora_ExitCB_h
#define _Nora_ExitCB_h

// leave program
extern void ExitCB(Widget widget, XtPointer client_data, XtPointer call_data);

// same, but with interaction
extern void ExitAfterConfirmationCB(Widget widget, XtPointer client_data,
				  XtPointer call_data);

#endif
