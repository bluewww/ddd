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
