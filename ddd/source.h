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

#ifdef IF_MOTIF

#include <X11/Intrinsic.h>

#endif // IF_MOTIF

#include "gtk_wrapper.h"

#include "bool.h"
#include "SourceView.h"

extern void gdbBreakAtCB      (CB_ALIST_1(Widget));
extern void gdbClearAtCB      (CB_ALIST_1(Widget));
extern void gdbToggleBreakCB  (CB_ALIST_1(Widget));
extern void gdbLookupCB       (CB_ALIST_NULL);

extern void gdbTempBreakAtCB  (CB_ALIST_1(Widget));
extern void gdbRegexBreakAtCB (CB_ALIST_1(Widget));
extern void gdbContUntilCB    (CB_ALIST_1(Widget));
extern void gdbSetPCCB        (CB_ALIST_1(Widget));

extern void gdbPrintCB        (CB_ALIST_12(Widget, XtP(bool)));
extern void gdbDisplayCB      (CB_ALIST_1(Widget));
extern void gdbWatchCB        (CB_ALIST_12(Widget, XtP(long)));
extern void gdbUnwatchCB      (CB_ALIST_NULL);
extern void gdbToggleWatchCB  (CB_ALIST_12(Widget, XtP(long)));

extern void gdbPrintRefCB     (CB_ALIST_12(Widget, XtP(bool)));
extern void gdbDispRefCB      (CB_ALIST_1(Widget));
extern void gdbWatchRefCB     (Widget, XtPointer, XtPointer);

extern void gdbWhatisCB       (CB_ALIST_1(Widget));

extern void gdbEditSourceCB   (CB_ALIST_1(Widget));
extern void gdbReloadSourceCB (CB_ALIST_NULL);

extern void gdbUndoCB         (CB_ALIST_NULL);
extern void gdbRedoCB         (CB_ALIST_NULL);

extern bool have_breakpoint_at_arg();
extern bool have_enabled_breakpoint_at_arg();

extern bool have_watchpoint_at_arg();
extern bool have_watchpoint_at_ref_arg();
extern bool have_enabled_watchpoint_at_arg();

extern void gdbEditBreakpointPropertiesCB (CB_ALIST_NULL);
extern void gdbEditWatchpointPropertiesCB (CB_ALIST_NULL);

extern void gdbToggleEnableBreakpointCB (CB_ALIST_1(Widget));
extern void gdbToggleEnableWatchpointCB (CB_ALIST_1(Widget));

extern void gdbFindCB         (CB_ALIST_12(Widget, XtP(SourceView::SearchDirection)));
extern void gdbFindAgainCB    (CB_ALIST_1(Widget));

extern SourceView::SearchDirection current_find_direction();

#endif // _DDD_source_h
// DON'T ADD ANYTHING BEHIND THIS #endif
