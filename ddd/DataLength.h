// $Id$
// Data/Length struct

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
// Revision 1.1  1995/05/01 15:47:36  zeller
// Initial revision
//
// Revision 1.9  1993/08/27  15:48:07  zeller
// Neu: TypeInfo
//
// Revision 1.8  1993/05/22  20:19:44  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.7  1993/04/22  10:49:17  zeller
// Lizenz verbessert
// 
// Revision 1.6  1993/04/22  10:36:45  zeller
// Lizenz verbessert
// 
// Revision 1.5  1993/04/20  22:56:04  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.4  1993/04/16  11:28:24  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.3  1992/06/02  02:08:32  zeller
// #pragma interface eingefuehrt
// 
// Revision 1.2  1992/06/02  02:06:44  zeller
// #pragma once entfernt
// 
// Revision 1.1  1992/05/14  10:06:26  zeller
// Initial revision
// 

#ifndef _Nora_DataLength_h
#define _Nora_DataLength_h

#ifdef __GNUG__
#pragma interface
#endif


#include <string.h>
#include "TypeInfo.h"

struct DataLength {
    DECLARE_TYPE_INFO

    const char *data;
    int length;

    DataLength(const char *d):
	data(d), length(strlen(d))
    {}

    DataLength(const char *d, int l):
        data(d), length(l)
    {}
};

#endif
