// $Id$
// Data Link

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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	
// $Log$
// Revision 1.1  1995/05/01 15:47:36  zeller
// Initial revision
//
// Revision 10.12  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.11  1994/02/19  20:20:15  zeller
// Neu: Kommentare auf englisch
//
// Revision 10.10  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.9  1993/05/22  20:14:05  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/21  00:10:58  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/20  22:57:44  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/16  11:31:31  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/10/13  20:49:04  zeller
// strclass.h statt String.h
// 
// Revision 10.4  1992/07/22  10:52:26  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.3  1992/06/01  12:53:37  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:34  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:08  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:44:06  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:56  zeller
// Installierte Version
// 
// Revision 1.1  1991/06/10  14:17:18  zeller
// Initial revision
// 

#ifndef _Nora_DataLink_h
#define _Nora_DataLink_h

#include "strclass.h"
#include "bool.h"

struct Data;			 // Opaque type

struct DataLink {
    Data *(*link)(Data *);	 // Duplicate data
    void (*unlink)(Data *);	 // Unlink data
    bool (*selected)(Data *); // Flag: is data selected?
    string (*name)(Data *);	 // Name of data
    string (*info)(Data *);	 // Debugging info of data
};

#endif
