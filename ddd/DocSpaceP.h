/* $Id$ */
/* DocSpace-Widget (privater Teil Deklaration) */

/*
    Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
    Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.

    This file is part of the DDD Library.

    The DDD Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    The DDD Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with the DDD Library -- see the file COPYING.LIB.
    If not, write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    DDD is the data display debugger.
    For details, see the DDD World-Wide-Web page, 
    `http://www.cs.tu-bs.de/softech/ddd/',
    or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.
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
