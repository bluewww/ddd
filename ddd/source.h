// $Id$ -*- C++ -*-
// Source callbacks and actions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_source_h
#define _DDD_source_h

#if defined(IF_MOTIF)

#include <X11/Intrinsic.h>

#endif

#if !defined(IF_XM)

#include <GUI/Widget.h>

#endif

#include "gtk_wrapper.h"

#include "bool.h"
#include "SourceView.h"

#if defined(IF_XM)
extern void gdbBreakAtCB      (Widget, XtPointer, XtPointer);
extern void gdbClearAtCB      (Widget, XtPointer, XtPointer);
extern void gdbToggleBreakCB  (Widget, XtPointer, XtPointer);
extern void gdbLookupCB       (Widget, XtPointer, XtPointer);
#else
extern void gdbBreakAtCB      (GUI::Widget *);
extern void gdbClearAtCB      (GUI::Widget *);
extern void gdbToggleBreakCB  (GUI::Widget *);
extern void gdbLookupCB       (void);
#endif

#if defined(IF_XM)

extern void gdbTempBreakAtCB  (Widget, XtPointer, XtPointer);
extern void gdbRegexBreakAtCB (Widget, XtPointer, XtPointer);
extern void gdbContUntilCB    (Widget, XtPointer, XtPointer);
extern void gdbSetPCCB        (Widget, XtPointer, XtPointer);

extern void gdbPrintCB        (Widget, XtPointer, XtPointer);
extern void gdbDisplayCB      (Widget, XtPointer, XtPointer);
extern void gdbWatchCB        (Widget, XtPointer, XtPointer);

#else

extern void gdbTempBreakAtCB  (GUI::Widget *);
extern void gdbRegexBreakAtCB (GUI::Widget *);
extern void gdbContUntilCB    (GUI::Widget *);
extern void gdbSetPCCB        (GUI::Widget *);

extern void gdbPrintCB        (GUI::Widget *, bool);
extern void gdbDisplayCB      (GUI::Widget *);
extern void gdbWatchCB        (GUI::Widget *, int);

#endif

extern void gdbUnwatchCB      (CB_ALIST_NULL);

#if defined(IF_XM)

extern void gdbToggleWatchCB  (Widget, XtPointer, XtPointer);

extern void gdbPrintRefCB     (Widget, XtPointer, XtPointer);
extern void gdbDispRefCB      (Widget, XtPointer, XtPointer);

#else

extern void gdbToggleWatchCB  (GUI::Widget *, int);

extern void gdbPrintRefCB     (GUI::Widget *, bool);
extern void gdbDispRefCB      (GUI::Widget *);

#endif

extern void gdbWatchRefCB     (Widget, XtPointer, XtPointer);

#if defined(IF_XM)
extern void gdbWhatisCB       (Widget, XtPointer, XtPointer);

extern void gdbEditSourceCB   (Widget, XtPointer, XtPointer);
extern void gdbReloadSourceCB (Widget, XtPointer, XtPointer);
#else
extern void gdbWhatisCB       (GUI::Widget *);

extern void gdbEditSourceCB   (GUI::Widget *);
extern void gdbReloadSourceCB (void);
#endif

#if defined(IF_XM)
extern void gdbUndoCB         (Widget, XtPointer, XtPointer);
extern void gdbRedoCB         (Widget, XtPointer, XtPointer);
#else
extern void gdbUndoCB         (void);
extern void gdbRedoCB         (void);
#endif

extern bool have_breakpoint_at_arg();
extern bool have_enabled_breakpoint_at_arg();

extern bool have_watchpoint_at_arg();
extern bool have_watchpoint_at_ref_arg();
extern bool have_enabled_watchpoint_at_arg();


#if defined(IF_XM)

extern void gdbEditBreakpointPropertiesCB (Widget, XtPointer, XtPointer);
extern void gdbEditWatchpointPropertiesCB (Widget, XtPointer, XtPointer);

extern void gdbToggleEnableBreakpointCB (Widget, XtPointer, XtPointer);
extern void gdbToggleEnableWatchpointCB (Widget, XtPointer, XtPointer);
extern void gdbFindCB         (Widget, XtPointer, XtPointer);
extern void gdbFindAgainCB    (Widget, XtPointer, XtPointer);

#else

extern void gdbEditBreakpointPropertiesCB (void);
extern void gdbEditWatchpointPropertiesCB (void);

extern void gdbToggleEnableBreakpointCB (GUI::Widget *);
extern void gdbToggleEnableWatchpointCB (GUI::Widget *);
extern void gdbFindCB         (GUI::Widget *, SourceView::SearchDirection);
extern void gdbFindAgainCB    (GUI::Widget *);

#endif

extern SourceView::SearchDirection current_find_direction();

#endif // _DDD_source_h
// DON'T ADD ANYTHING BEHIND THIS #endif
