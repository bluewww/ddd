// $Id$ -*- C++ -*-
// DDD command-line actions and callbacks

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_editing_h
#define _DDD_editing_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "bool.h"
#include "strclass.h"

// Actions
extern void controlAct           (Widget, XEvent*, String*, Cardinal*);
extern void forward_characterAct (Widget, XEvent*, String*, Cardinal*);
extern void backward_characterAct(Widget, XEvent*, String*, Cardinal*);
extern void beginning_of_lineAct (Widget, XEvent*, String*, Cardinal*);
extern void end_of_lineAct       (Widget, XEvent*, String*, Cardinal*);
extern void set_lineAct          (Widget, XEvent*, String*, Cardinal*);
extern void delete_or_controlAct (Widget, XEvent*, String*, Cardinal*);
extern void insert_source_argAct (Widget, XEvent*, String*, Cardinal*);
extern void insert_graph_argAct  (Widget, XEvent*, String*, Cardinal*);
extern void next_tab_groupAct    (Widget, XEvent*, String*, Cardinal*);
extern void prev_tab_groupAct    (Widget, XEvent*, String*, Cardinal*);
extern void get_focusAct         (Widget, XEvent*, String*, Cardinal*);
extern void isearch_nextAct      (Widget, XEvent*, String*, Cardinal*);
extern void isearch_prevAct      (Widget, XEvent*, String*, Cardinal*);
extern void isearch_exitAct      (Widget, XEvent*, String*, Cardinal*);

// Callbacks
extern void gdbModifyCB          (Widget, XtPointer, XtPointer);
extern void gdbMotionCB          (Widget, XtPointer, XtPointer);
extern void gdbChangeCB          (Widget, XtPointer, XtPointer);

extern void gdbNextCB            (Widget, XtPointer, XtPointer);
extern void gdbPrevCB            (Widget, XtPointer, XtPointer);
extern void gdbISearchNextCB     (Widget, XtPointer, XtPointer);
extern void gdbISearchPrevCB     (Widget, XtPointer, XtPointer);
extern void gdbClearCB           (Widget, XtPointer, XtPointer);
extern void gdbCompleteCB        (Widget, XtPointer, XtPointer);
extern void gdbApplyCB           (Widget, XtPointer, XtPointer);
extern void gdbClearWindowCB     (Widget, XtPointer, XtPointer);

// Return current GDB command line.
extern string current_line();

// True if last input was at GDB prompt.
extern bool gdb_input_at_prompt;

// Clear isearch mode.
// RESET means to reset the history; SHOW means to redisplay.
extern void clear_isearch(bool reset = false, bool show = true);

#endif // _DDD_editing_h
// DON'T ADD ANYTHING BEHIND THIS #endif
