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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_options_h
#define _DDD_options_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include <X11/Intrinsic.h>

// Options

// Save options in FILE.  If CREATE is true, allow creation; if
// SAVE_YOURSELF is true, include additional state information.
// Return true iff successful.

#define CREATE_OPTIONS 1 // Create simple options file if non-existent
#define SAVE_SESSION   2 // Save additional session information
#define SAVE_GEOMETRY  4 // Save window geometry
#define SAVE_CORE      8 // Always include core file
#define MAY_KILL      16 // True if program can be killed
#define DONT_SAVE     32 // Only simulate; don't actually save
#define MAY_INTERACT 128 // Allow interaction

#define SAVE_DEFAULT MAY_INTERACT

bool saving_options_kills_program(unsigned long flags);
bool saving_options_excludes_data(unsigned long flags);

bool save_options(unsigned long flags);


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
extern void dddSetStartupLogoCB             (Widget, XtPointer, XtPointer);

extern void dddSetEditCommandCB             (Widget, XtPointer, XtPointer);
extern void dddSetGetCoreCommandCB          (Widget, XtPointer, XtPointer);
extern void dddSetPSCommandCB               (Widget, XtPointer, XtPointer);
extern void dddSetTermCommandCB             (Widget, XtPointer, XtPointer);
extern void dddSetUncompressCommandCB       (Widget, XtPointer, XtPointer);
extern void dddSetWWWCommandCB              (Widget, XtPointer, XtPointer);

extern void DDDSaveOptionsCB                (Widget, XtPointer, XtPointer);
extern void DDDSaveOptionsAsCB              (Widget, XtPointer, XtPointer);

#endif // _DDD_options_h
// DON'T ADD ANYTHING BEHIND THIS #endif
