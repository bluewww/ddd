/* $Id$ */
/* DocSpace-Widget (privater Teil Deklaration) */

/*
    Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
    Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).


    This file is part of the NORA Library.

    The NORA Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    The NORA Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * $Log$
 * Revision 1.1  1995/05/01 15:48:09  zeller
 * Initial revision
 *
 * Revision 10.6  1993/06/29  10:32:19  zeller
 * Log fixed
 *
 * Revision 10.5  1993/05/22  20:14:21  zeller
 * %Q% added by apply-q-flag.sh 1.5
 *
 * Revision 10.4  1993/04/16  11:31:35  zeller
 * Neu: GNU Library General Public License eingefuegt
 * 
 * Revision 10.3  1992/10/28  16:23:26  zeller
 * Anpassung an X11R5: Kein <X11/copyright.h> mehr
 * 
 * Revision 10.2  1992/07/22  10:52:28  zeller
 * Copy-Konstruktoren jetzt von der Form X::X(const& X)
 * 
 * Revision 10.1  1992/02/17  12:50:14  zeller
 * Tabstops von 4 auf 8 gesetzt
 * 
 * Revision 9.1  1991/07/08  05:44:52  zeller
 * Installierte Version
 * 
 * Revision 8.1  1991/07/06  14:58:10  zeller
 * Installierte Version
 * 
 * Revision 1.5  1990/08/28  14:21:56  zeller
 * #pragma once entfernt (Sun-CC mag das nicht)
 * 
 * Revision 1.4  1990/08/13  11:22:05  zeller
 * #pragma once eingefuehrt
 * 
 * Revision 1.3  1990/07/25  13:58:53  zeller
 * Quit-Callback eingefuehrt
 * 
 * Revision 1.2  1990/07/25  12:18:12  zeller
 * ID korrigiert
 * 
 * Revision 1.1  1990/07/25  12:02:00  zeller
 * Initial revision
 * 
*/


#ifndef _DocSpaceP_h
#define _DocSpaceP_h

#include "DocSpace.h"

/* superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRDocSpaceResource "DocSpaceResource"

typedef struct {
    int empty;
} DocSpaceClassPart;

typedef struct _DocSpaceClassRec {
    CoreClassPart   core_class;
    DocSpaceClassPart   docSpace_class;
} DocSpaceClassRec;

extern DocSpaceClassRec docSpaceClassRec;

typedef struct {
    /* resources */
    XtCallbackList expose_callback;
    XtCallbackList input_callback;
    XtCallbackList quit_callback;
    /* private state */
} DocSpacePart;

typedef struct _DocSpaceRec {
    CorePart        core;
    DocSpacePart    docSpace;
} DocSpaceRec;

#endif /* _DocSpaceP_h */
