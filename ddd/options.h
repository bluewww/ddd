// $Id$ -*- C++ -*-
// Save and edit DDD options

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

#ifndef _DDD_options_h
#define _DDD_options_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include <X11/Intrinsic.h>

// Options

// Return option file name
string options_file();

// Save options in FILE.  If CREATE is true, allow creation; if
// SAVE_YOURSELF is true, include additional state information.
// Return true iff successful.

#define OPTIONS_CREATE         1 // Create simple options file if non-existent
#define OPTIONS_SAVE_SESSION   2 // Save extra session information
#define OPTIONS_INTERACT       4 // Allow interaction

#define OPTIONS_DEFAULT (OPTIONS_INTERACT)

bool save_options(string file, unsigned long flags = OPTIONS_DEFAULT);


// Return true iff the startup preferences have changed.
extern bool startup_preferences_changed();

// Lots and lots of callbacks
extern void sourceToggleFindWordsOnlyCB     (Widget, XtPointer, XtPointer);
extern void sourceToggleCacheSourceFilesCB  (Widget, XtPointer, XtPointer);
extern void sourceToggleCacheMachineCodeCB  (Widget, XtPointer, XtPointer);
extern void sourceToggleDisassembleCB       (Widget, XtPointer, XtPointer);
extern void sourceToggleAllRegistersCB      (Widget, XtPointer, XtPointer);
extern void sourceToggleDisplayLineNumbersCB(Widget, XtPointer, XtPointer);
extern void sourceSetDisplayGlyphsCB        (Widget, XtPointer, XtPointer);
extern void sourceSetUseSourcePathCB        (Widget, XtPointer, XtPointer);
extern void sourceSetTabWidthCB             (Widget, XtPointer, XtPointer);

extern void graphToggleDetectAliasesCB      (Widget, XtPointer, XtPointer);
extern void graphToggleShowGridCB           (Widget, XtPointer, XtPointer);
extern void graphToggleShowHintsCB          (Widget, XtPointer, XtPointer);
extern void graphToggleSnapToGridCB         (Widget, XtPointer, XtPointer);
extern void graphToggleCompactLayoutCB      (Widget, XtPointer, XtPointer);
extern void graphToggleAutoLayoutCB         (Widget, XtPointer, XtPointer);
extern void graphSetGridSizeCB              (Widget, XtPointer, XtPointer);

extern void dddToggleGroupIconifyCB         (Widget, XtPointer, XtPointer);
extern void dddToggleSeparateExecWindowCB   (Widget, XtPointer, XtPointer);
extern void dddToggleUngrabMousePointerCB   (Widget, XtPointer, XtPointer);
extern void dddToggleSaveHistoryOnExitCB    (Widget, XtPointer, XtPointer);
extern void dddToggleSuppressWarningsCB     (Widget, XtPointer, XtPointer);

extern void dddToggleButtonTipsCB           (Widget, XtPointer, XtPointer);
extern void dddToggleValueTipsCB            (Widget, XtPointer, XtPointer);
extern void dddToggleButtonDocsCB           (Widget, XtPointer, XtPointer);
extern void dddToggleValueDocsCB            (Widget, XtPointer, XtPointer);

extern void dddSetGlobalTabCompletionCB     (Widget, XtPointer, XtPointer);
extern void dddSetSeparateWindowsCB         (Widget, XtPointer, XtPointer);
extern void dddSetStatusAtBottomCB          (Widget, XtPointer, XtPointer);
extern void dddSetToolBarCB                 (Widget, XtPointer, XtPointer);
extern void dddSetKeyboardFocusPolicyCB     (Widget, XtPointer, XtPointer);
extern void dddSetPannerCB                  (Widget, XtPointer, XtPointer);
extern void dddSetDebuggerCB                (Widget, XtPointer, XtPointer);

extern void DDDSaveOptionsCB                (Widget, XtPointer, XtPointer);

#endif // _DDD_options_h
// DON'T ADD ANYTHING BEHIND THIS #endif
