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
