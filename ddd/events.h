// $Id$
// Event access

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
// Revision 1.1  1995/05/01 15:47:55  zeller
// Initial revision
//
// Revision 9.8  1993/05/22  20:10:06  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 9.7  1993/04/22  11:03:59  zeller
// Lizenz verbessert
// 
// Revision 9.6  1993/04/21  00:21:49  zeller
// Umbenennung der Header-Dateien
// 
// Revision 9.5  1993/04/20  23:02:19  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 9.4  1993/04/16  11:41:57  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 9.3  1992/06/01  17:04:48  zeller
// #pragma once entfernt
// 
// Revision 9.2  1992/02/26  10:12:26  zeller
// Neue Bezeichner, neue Tabstops
// 
// Revision 9.1  1991/07/08  05:52:49  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:00:48  zeller
// Installierte Version
// 
// Revision 1.1  1991/06/24  20:26:22  zeller
// Initial revision
// 

#ifndef _Nora_events_h
#define _Nora_events_h

#include <X11/Xlib.h>
#include "BoxPoint.h"
#include "BoxSize.h"

BoxPoint point(XEvent *ev); // Location
BoxSize size(XEvent *ev);   // Size
Time time(XEvent *ev);      // Time

#endif
