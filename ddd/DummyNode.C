// $Id$
// Implementation Klasse DummyNode

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
// Revision 1.1  1995/05/01 15:47:38  zeller
// Initial revision
//
// Revision 10.9  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.8  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.7  1993/05/22  20:17:01  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.6  1993/04/22  11:07:21  zeller
// Lizenz verbessert
// 
// Revision 10.5  1993/04/21  00:27:57  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.4  1993/04/16  11:44:27  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.3  1992/11/03  19:37:15  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.2  1992/06/01  16:51:18  zeller
// Neu: #pragma implementation
// 
// Revision 10.1  1992/02/18  11:19:57  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:57:31  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:16  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:53:52  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 1.1  1991/05/24  18:54:08  zeller
// Initial revision
// 

char DummyNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "VSLLib.h"
#include "VSLDef.h"
#include "DummyNode.h"

DEFINE_TYPE_INFO_1(DummyNode, VSLNode)

// DummyNode

bool DummyNode::matchesAll = false;
