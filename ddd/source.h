// $Id$ -*- C++ -*-
// Source callbacks and actions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_source_h
#define _DDD_source_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "bool.h"
#include "SourceView.h"

extern void gdbBreakAtCB      (Widget, XtPointer, XtPointer);
extern void gdbClearAtCB      (Widget, XtPointer, XtPointer);
extern void gdbToggleBreakCB  (Widget, XtPointer, XtPointer);
extern void gdbLookupCB       (Widget, XtPointer, XtPointer);

extern void gdbTempBreakAtCB  (Widget, XtPointer, XtPointer);
extern void gdbRegexBreakAtCB (Widget, XtPointer, XtPointer);
extern void gdbContUntilCB    (Widget, XtPointer, XtPointer);
extern void gdbSetPCCB        (Widget, XtPointer, XtPointer);

extern void gdbPrintCB        (Widget, XtPointer, XtPointer);
extern void gdbDisplayCB      (Widget, XtPointer, XtPointer);
extern void gdbWatchCB        (Widget, XtPointer, XtPointer);
extern void gdbUnwatchCB      (Widget, XtPointer, XtPointer);
extern void gdbToggleWatchCB  (Widget, XtPointer, XtPointer);

extern void gdbPrintRefCB     (Widget, XtPointer, XtPointer);
extern void gdbDispRefCB      (Widget, XtPointer, XtPointer);
extern void gdbWatchRefCB     (Widget, XtPointer, XtPointer);

extern void gdbWhatisCB       (Widget, XtPointer, XtPointer);

extern void gdbEditSourceCB   (Widget, XtPointer, XtPointer);
extern void gdbReloadSourceCB (Widget, XtPointer, XtPointer);

extern void gdbUndoCB         (Widget, XtPointer, XtPointer);
extern void gdbRedoCB         (Widget, XtPointer, XtPointer);

extern bool have_breakpoint_at_arg();
extern bool have_enabled_breakpoint_at_arg();

extern bool have_watchpoint_at_arg();
extern bool have_watchpoint_at_ref_arg();
extern bool have_enabled_watchpoint_at_arg();

extern void gdbEditBreakpointPropertiesCB (Widget, XtPointer, XtPointer);
extern void gdbEditWatchpointPropertiesCB (Widget, XtPointer, XtPointer);

extern void gdbToggleEnableBreakpointCB (Widget, XtPointer, XtPointer);
extern void gdbToggleEnableWatchpointCB (Widget, XtPointer, XtPointer);

extern void gdbFindCB         (Widget, XtPointer, XtPointer);
extern void gdbFindAgainCB    (Widget, XtPointer, XtPointer);

extern SourceView::SearchDirection current_find_direction();

#endif // _DDD_source_h
// DON'T ADD ANYTHING BEHIND THIS #endif
