/* $Id$ */
/* DocSpace-Widget (Deklaration) */

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
 * Revision 1.1  1995/05/01 15:47:17  zeller
 * Initial revision
 *
 * Revision 10.7  1993/06/29  10:30:34  zeller
 * Log fixed
 *
 * Revision 10.6  1993/05/22  20:14:18  zeller
 * %Q% added by apply-q-flag.sh 1.5
 *
 * Revision 10.5  1993/04/16  11:31:34  zeller
 * Neu: GNU Library General Public License eingefuegt
 * 
 * Revision 10.4  1992/10/28  16:23:26  zeller
 * Anpassung an X11R5: Kein <X11/copyright.h> mehr
 * 
 * Revision 10.3  1992/07/22  10:52:28  zeller
 * Copy-Konstruktoren jetzt von der Form X::X(const& X)
 * 
 * Revision 10.2  1992/02/18  10:56:53  zeller
 * Konfliktfreie Typbezeichner eingefuehrt
 * 
 * Revision 10.1  1992/02/17  12:50:13  zeller
 * Tabstops von 4 auf 8 gesetzt
 * 
 * Revision 9.1  1991/07/08  05:44:46  zeller
 * Installierte Version
 * 
 * Revision 8.1  1991/07/06  14:58:07  zeller
 * Installierte Version
 * 
 * Revision 1.5  1990/08/28  14:21:54  zeller
 * #pragma once entfernt (Sun-CC mag das nicht)
 * 
 * Revision 1.4  1990/08/13  11:22:03  zeller
 * #pragma once eingefuehrt
 * 
 * Revision 1.3  1990/07/25  13:58:50  zeller
 * Quit-Callback eingefuehrt
 * 
 * Revision 1.2  1990/07/25  12:18:10  zeller
 * ID korrigiert
 * 
 * Revision 1.1  1990/07/25  12:01:59  zeller
 * Initial revision
 * 
*/


#ifndef _DocSpace_h
#define _DocSpace_h

/****************************************************************
 *
 * DocSpace widget
 *
 ****************************************************************/

/* Resources:

 Name                Class              RepType         Default Value
 ----                -----              -------         -------------
 background          Background         Pixel           XtDefaultBackground
 border              BorderColor        Pixel           XtDefaultForeground
 borderWidth         BorderWidth        BoxDimension    1
 destroyCallback     Callback           Pointer         NULL
 height              Height             BoxDimension    0
 mappedWhenManaged   MappedWhenManaged  boolean         True
 sensitive           Sensitive          boolean         True
 width               Width              BoxDimension    0
 x                   Position           Position        0
 y                   Position           Position        0
 callback            Callback           Callback        NULL
 exposeCallback      Callback           Callback        NULL

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNdocSpaceResource     "docSpaceResource"
#define XtNexposeCallback       "exposeCallback"
#define XtNquitCallback     "quitCallback"

#define XtCDocSpaceResource "DocSpaceResource"

/* declare specific DocSpaceWidget class and instance datatypes */

typedef struct _DocSpaceClassRec*   DocSpaceWidgetClass;
typedef struct _DocSpaceRec*        DocSpaceWidget;

/* declare the class constant */

extern WidgetClass docSpaceWidgetClass;

#endif /* _DocSpace_h */
