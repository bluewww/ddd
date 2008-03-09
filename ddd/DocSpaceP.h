/* $Id$ */
/* DocSpace-Widget (privater Teil Deklaration) */

/*
    Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
    Written by Andreas Zeller <zeller@gnu.org>.

    This file is part of DDD.

    DDD is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    DDD is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with DDD -- see the file COPYING.
    If not, see <http://www.gnu.org/licenses/>.

    DDD is the data display debugger.
    For details, see the DDD World-Wide-Web page, 
    `http://www.gnu.org/software/ddd/',
    or send a mail to the DDD developers <ddd@gnu.org>.
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
