// $Id$ -*- C++ -*-
// Save and edit DDD options

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

#ifndef _DDD_options_h
#define _DDD_options_h

#include "strclass.h"
#include "bool.h"

#ifdef IF_MOTIF

#include <X11/Intrinsic.h>

#endif // IF_MOTIF

#include "gtk_wrapper.h"

#include "BindingS.h" // for BindingType
#include "GDBAgent.h" // for DebuggerType

// Options

// Flags for saving options
#define CREATE_OPTIONS   (1 << 0) // Create simple options file if non-existent
#define SAVE_SESSION     (1 << 1) // Save additional session information
#define SAVE_GEOMETRY    (1 << 2) // Save window geometry
#define SAVE_CORE        (1 << 3) // Always include core file
#define MAY_KILL         (1 << 4) // May we kill to get a core dump?
#define MAY_PTRACE       (1 << 5) // May we use ptrace() to get a core dump?
#define MAY_GCORE        (1 << 6) // May we use gcore to get a core dump?
#define DONT_SAVE        (1 << 7) // Only simulate; don't actually save
#define DONT_RELOAD_CORE (1 << 8) // Do not load generated core file
#define DONT_COPY_CORE   (1 << 9) // Do not copy existing core file
#define MAY_INTERACT     (1 << 10) // Allow user interaction
#define DONT_RELOAD_FILE (1 << 11) // Do not load debuggee

#define SAVE_DEFAULT MAY_INTERACT

// True if saving options means to kill the program
bool saving_options_kills_program(unsigned long flags);

// True if data cannot be saved
bool saving_options_excludes_data(unsigned long flags);

// Save options in current session state file, using FLAGS
bool save_options(unsigned long flags);

// Get restarting commands in RESTART, using FLAGS
bool get_restart_commands(string& restart, unsigned long flags);

// Check options file
void check_options_file();

// Lots and lots of callbacks
extern void sourceToggleFindWordsOnlyCB     (CB_ARG_LIST_TOGGLE(,));
extern void sourceToggleFindCaseSensitiveCB (CB_ARG_LIST_TOGGLE(,));
extern void sourceToggleCacheSourceFilesCB  (CB_ARG_LIST_TOGGLE(,));
extern void sourceToggleCacheMachineCodeCB  (CB_ARG_LIST_TOGGLE(,));
extern void sourceToggleDisplayLineNumbersCB(CB_ARG_LIST_TOGGLE(,));
extern void sourceSetIntRegistersCB         (CB_ARG_LIST_TOGGLE(,));
extern void sourceSetAllRegistersCB         (CB_ARG_LIST_TOGGLE(,));
extern void sourceSetDisplayGlyphsCB        (CB_ARG_LIST_2());
extern void sourceSetUseSourcePathCB        (CB_ARG_LIST_2());
#ifdef IF_MOTIF
extern void sourceSetTabWidthCB             (CB_ARG_LIST_3());
extern void sourceSetSourceIndentCB 	    (CB_ARG_LIST_3());
extern void sourceSetCodeIndentCB 	    (CB_ARG_LIST_3());
#else // NOT IF_MOTIF
extern void sourceSetTabWidthCB             (RANGE_P);
extern void sourceSetSourceIndentCB 	    (RANGE_P);
extern void sourceSetCodeIndentCB 	    (RANGE_P);
#endif // IF_MOTIF

extern void graphToggleDetectAliasesCB      (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleAlign2dArraysCB      (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleShowGridCB           (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleShowHintsCB          (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleSnapToGridCB         (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleCompactLayoutCB      (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleAutoLayoutCB         (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleAutoCloseCB          (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleShowAnnotationsCB    (CB_ARG_LIST_TOGGLE(,));
extern void graphToggleShowDependentTitlesCB(CB_ARG_LIST_TOGGLE(,));
extern void graphToggleClusterDisplaysCB    (CB_ARG_LIST_TOGGLE(,));
#ifdef IF_MOTIF
extern void graphSetGridSizeCB              (CB_ARG_LIST_3());
extern void graphSetDisplayPlacementCB      (CB_ARG_LIST_23(,));
#else // NOT IF_MOTIF
extern void graphSetGridSizeCB              (RANGE_P);
extern void graphSetDisplayPlacementCB      (TOGGLEBUTTON_P w, XmOrientation);
#endif // IF_MOTIF

#ifdef IF_MOTIF
extern void dddToggleGroupIconifyCB         (CB_ARG_LIST_3());
extern void dddToggleUniconifyWhenReadyCB   (CB_ARG_LIST_3());
extern void dddToggleSeparateExecWindowCB   (CB_ARG_LIST_3());
extern void dddToggleCheckGrabsCB           (CB_ARG_LIST_3());
extern void dddToggleSaveHistoryOnExitCB    (CB_ARG_LIST_3());
extern void dddToggleSuppressWarningsCB     (CB_ARG_LIST_3());
extern void dddToggleWarnIfLockedCB         (CB_ARG_LIST_3());
extern void dddToggleSaveOptionsOnExitCB    (CB_ARG_LIST_3());
#else // NOT IF_MOTIF
extern void dddToggleGroupIconifyCB         (TOGGLEBUTTON_P);
extern void dddToggleUniconifyWhenReadyCB   (TOGGLEBUTTON_P);
extern void dddToggleSeparateExecWindowCB   (TOGGLEBUTTON_P);
extern void dddToggleCheckGrabsCB           (TOGGLEBUTTON_P);
extern void dddToggleSaveHistoryOnExitCB    (TOGGLEBUTTON_P);
extern void dddToggleSuppressWarningsCB     (TOGGLEBUTTON_P);
extern void dddToggleWarnIfLockedCB         (TOGGLEBUTTON_P);
extern void dddToggleSaveOptionsOnExitCB    (TOGGLEBUTTON_P);
#endif // IF_MOTIF

extern void dddToggleButtonTipsCB           (CB_ARG_LIST_TOGGLE(,));
extern void dddToggleValueTipsCB            (CB_ARG_LIST_TOGGLE(,));
extern void dddToggleButtonDocsCB           (CB_ARG_LIST_TOGGLE(,));
extern void dddToggleValueDocsCB            (CB_ARG_LIST_TOGGLE(,));
#ifdef IF_MOTIF
extern void dddToggleButtonCaptionsCB       (CB_ARG_LIST_13(,));
extern void dddToggleButtonImagesCB         (CB_ARG_LIST_13(,));
extern void dddToggleFlatButtonsCB          (CB_ARG_LIST_13(,));
extern void dddToggleColorButtonsCB         (CB_ARG_LIST_13(,));
extern void dddToggleToolbarsAtBottomCB     (CB_ARG_LIST_13(,));
#else // NOT IF_MOTIF
extern void dddToggleButtonCaptionsCB       (TOGGLEBUTTON_P);
extern void dddToggleButtonImagesCB         (TOGGLEBUTTON_P);
extern void dddToggleFlatButtonsCB          (TOGGLEBUTTON_P);
extern void dddToggleColorButtonsCB         (TOGGLEBUTTON_P);
extern void dddToggleToolbarsAtBottomCB     (TOGGLEBUTTON_P);
#endif // IF_MOTIF

extern void dddSetCrashCB                   (CB_ALIST_2(XtP(long)));
extern void dddClearMaintenanceCB           (CB_ALIST_NULL);

extern void dddSetGlobalTabCompletionCB     (CB_ALIST_12(Widget, XtP(long)));
extern void dddSetSeparateWindowsCB         (CB_ARG_LIST_12(,));
extern void dddSetStatusAtBottomCB          (Widget, XtPointer, XtPointer);
extern void dddSetToolBarCB                 (CB_ARG_LIST_12(,));
extern void dddSetKeyboardFocusPolicyCB     (CB_ARG_LIST_12(,));
extern void dddSetPannerCB                  (CB_ARG_LIST_12(,));
#ifdef IF_MOTIF
extern void dddSetDebuggerCB                (CB_ARG_LIST_123(,,));
extern void dddToggleAutoDebuggerCB         (CB_ARG_LIST_13(,));
extern void dddSetCutCopyPasteBindingsCB    (CB_ARG_LIST_23(,));
extern void dddSetSelectAllBindingsCB       (CB_ARG_LIST_23(,));
#else // NOT IF_MOTIF
extern void dddSetDebuggerCB                (TOGGLEBUTTON_P, DebuggerType);
extern void dddToggleAutoDebuggerCB         (TOGGLEBUTTON_P);
extern void dddSetCutCopyPasteBindingsCB    (TOGGLEBUTTON_P, BindingStyle);
extern void dddSetSelectAllBindingsCB       (TOGGLEBUTTON_P, BindingStyle);
#endif // IF_MOTIF

extern void dddSetUndoBufferSizeCB          (CB_ALIST_1(ENTRY_P));
extern void dddClearUndoBufferCB            (CB_ARG_LIST_NULL);

extern void dddSetEditCommandCB             (CB_ALIST_1(ENTRY_P));
extern void dddSetPlotCommandCB             (CB_ALIST_1(ENTRY_P));
extern void dddSetGetCoreCommandCB          (CB_ALIST_1(ENTRY_P));
extern void dddSetPSCommandCB               (CB_ALIST_1(ENTRY_P));
extern void dddSetTermCommandCB             (CB_ALIST_1(ENTRY_P));
extern void dddSetUncompressCommandCB       (CB_ALIST_1(ENTRY_P));
extern void dddSetWWWCommandCB              (CB_ALIST_1(ENTRY_P));
extern void dddSetBuiltinPlotWindowCB       (CB_ALIST_2(XtP(long)));

extern void DDDSaveOptionsCB                (CB_ALIST_12(Widget, XtP(unsigned long)));
extern void DDDSaveOptionsAsCB              (Widget, XtPointer, XtPointer);

#endif // _DDD_options_h
// DON'T ADD ANYTHING BEHIND THIS #endif
