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

#include "config.h"
#include "strclass.h"
#include "bool.h"

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/RadioButton.h>
#include <GUI/CheckButton.h>
#include <GUI/CheckMenuItem.h>
#include <GUI/Scale.h>
#include <GUI/Entry.h>
#endif

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
#if defined(IF_XM)
extern void sourceToggleFindWordsOnlyCB     (Widget, XtPointer, XtPointer);
extern void sourceToggleFindCaseSensitiveCB (Widget, XtPointer, XtPointer);
extern void sourceToggleCacheSourceFilesCB  (Widget, XtPointer, XtPointer);
extern void sourceToggleCacheMachineCodeCB  (Widget, XtPointer, XtPointer);
extern void sourceToggleDisplayLineNumbersCB(Widget, XtPointer, XtPointer);
#else
extern void sourceToggleFindWordsOnlyCB     (GUI::CheckButton *);
extern void sourceToggleFindCaseSensitiveCB (GUI::CheckButton *);
extern void sourceToggleCacheSourceFilesCB  (GUI::CheckButton *);
extern void sourceToggleCacheMachineCodeCB  (GUI::CheckButton *);
extern void sourceToggleDisplayLineNumbersCB(GUI::CheckButton *);
#endif
#if defined(IF_XM)
extern void sourceSetIntRegistersCB         (Widget, XtPointer, XtPointer);
extern void sourceSetAllRegistersCB         (Widget, XtPointer, XtPointer);
extern void sourceSetDisplayGlyphsCB        (Widget, XtPointer, XtPointer);
extern void sourceSetUseSourcePathCB        (Widget, XtPointer, XtPointer);
extern void sourceSetTabWidthCB             (Widget, XtPointer, XtPointer);
extern void sourceSetSourceIndentCB 	    (Widget, XtPointer, XtPointer);
extern void sourceSetCodeIndentCB 	    (Widget, XtPointer, XtPointer);
#else
extern void sourceSetIntRegistersCB         (GUI::RadioButton *);
extern void sourceSetAllRegistersCB         (GUI::RadioButton *);
extern void sourceSetDisplayGlyphsCB        (bool);
extern void sourceSetUseSourcePathCB        (bool);
extern void sourceSetTabWidthCB             (GUI::Scale *);
extern void sourceSetSourceIndentCB 	    (GUI::Scale *);
extern void sourceSetCodeIndentCB 	    (GUI::Scale *);
#endif

#if defined(IF_XM)
extern void graphToggleDetectAliasesCB      (Widget, XtPointer, XtPointer);
extern void graphToggleAlign2dArraysCB      (Widget, XtPointer, XtPointer);
extern void graphToggleShowGridCB           (Widget, XtPointer, XtPointer);
extern void graphToggleShowHintsCB          (Widget, XtPointer, XtPointer);
extern void graphToggleSnapToGridCB         (Widget, XtPointer, XtPointer);
extern void graphToggleCompactLayoutCB      (Widget, XtPointer, XtPointer);
extern void graphToggleAutoLayoutCB         (Widget, XtPointer, XtPointer);
extern void graphToggleAutoCloseCB          (Widget, XtPointer, XtPointer);
extern void graphToggleShowAnnotationsCB    (Widget, XtPointer, XtPointer);
extern void graphToggleShowDependentTitlesCB(Widget, XtPointer, XtPointer);
extern void graphToggleClusterDisplaysCB    (Widget, XtPointer, XtPointer);
extern void graphSetGridSizeCB              (Widget, XtPointer, XtPointer);
extern void graphSetDisplayPlacementCB      (Widget, XtPointer, XtPointer);
#else
extern void graphToggleDetectAliasesCB      (GUI::Bipolar *);
extern void graphToggleAlign2dArraysCB      (GUI::CheckButton *);
extern void graphToggleShowGridCB           (GUI::CheckButton *);
extern void graphToggleShowHintsCB          (GUI::CheckButton *);
extern void graphToggleSnapToGridCB         (GUI::CheckButton *);
extern void graphToggleCompactLayoutCB      (GUI::CheckButton *);
extern void graphToggleAutoLayoutCB         (GUI::CheckButton *);
extern void graphToggleAutoCloseCB          (GUI::CheckButton *);
extern void graphToggleShowAnnotationsCB    (GUI::CheckButton *);
extern void graphToggleShowDependentTitlesCB(GUI::CheckButton *);
extern void graphToggleClusterDisplaysCB    (GUI::CheckButton *);
extern void graphSetGridSizeCB              (GUI::Scale *);
extern void graphSetDisplayPlacementCB      (GUI::RadioButton *w, GUI::Orientation);
#endif

#if defined(IF_XM)
extern void dddToggleGroupIconifyCB         (Widget, XtPointer, XtPointer);
extern void dddToggleUniconifyWhenReadyCB   (Widget, XtPointer, XtPointer);
extern void dddToggleSeparateExecWindowCB   (Widget, XtPointer, XtPointer);
extern void dddToggleCheckGrabsCB           (Widget, XtPointer, XtPointer);
extern void dddToggleSaveHistoryOnExitCB    (Widget, XtPointer, XtPointer);
extern void dddToggleSuppressWarningsCB     (Widget, XtPointer, XtPointer);
extern void dddToggleWarnIfLockedCB         (Widget, XtPointer, XtPointer);
extern void dddToggleSaveOptionsOnExitCB    (Widget, XtPointer, XtPointer);
#else
extern void dddToggleGroupIconifyCB         (GUI::CheckButton *);
extern void dddToggleUniconifyWhenReadyCB   (GUI::CheckButton *);
extern void dddToggleSeparateExecWindowCB   (GUI::CheckMenuItem *);
extern void dddToggleCheckGrabsCB           (GUI::CheckButton *);
extern void dddToggleSaveHistoryOnExitCB    (GUI::CheckButton *);
extern void dddToggleSuppressWarningsCB     (GUI::CheckButton *);
extern void dddToggleWarnIfLockedCB         (GUI::CheckButton *);
extern void dddToggleSaveOptionsOnExitCB    (GUI::CheckMenuItem *);
#endif


#if defined(IF_XM)

extern void dddToggleButtonTipsCB           (Widget, XtPointer, XtPointer);
extern void dddToggleValueTipsCB            (Widget, XtPointer, XtPointer);
extern void dddToggleButtonDocsCB           (Widget, XtPointer, XtPointer);
extern void dddToggleValueDocsCB            (Widget, XtPointer, XtPointer);

extern void dddToggleButtonCaptionsCB       (Widget, XtPointer, XtPointer);
extern void dddToggleButtonImagesCB         (Widget, XtPointer, XtPointer);
extern void dddToggleFlatButtonsCB          (Widget, XtPointer, XtPointer);
extern void dddToggleColorButtonsCB         (Widget, XtPointer, XtPointer);
extern void dddToggleToolbarsAtBottomCB     (Widget, XtPointer, XtPointer);

extern void dddSetCrashCB                   (Widget, XtPointer, XtPointer);
extern void dddClearMaintenanceCB           (Widget, XtPointer, XtPointer);

extern void dddSetGlobalTabCompletionCB     (Widget, XtPointer, XtPointer);
extern void dddSetSeparateWindowsCB         (Widget, XtPointer, XtPointer);

#else

extern void dddToggleButtonTipsCB           (GUI::CheckButton *);
extern void dddToggleValueTipsCB            (GUI::CheckButton *);
extern void dddToggleButtonDocsCB           (GUI::CheckButton *);
extern void dddToggleValueDocsCB            (GUI::CheckButton *);

extern void dddToggleButtonCaptionsCB       (GUI::CheckButton *);
extern void dddToggleButtonImagesCB         (GUI::CheckButton *);
extern void dddToggleFlatButtonsCB          (GUI::CheckButton *);
extern void dddToggleColorButtonsCB         (GUI::CheckButton *);
extern void dddToggleToolbarsAtBottomCB     (GUI::CheckButton *);

extern void dddSetCrashCB                   (int);
extern void dddClearMaintenanceCB           (void);

extern void dddSetGlobalTabCompletionCB     (GUI::RadioButton *, int);
extern void dddSetSeparateWindowsCB         (GUI::RadioButton *, int);

#endif

extern void dddSetStatusAtBottomCB          (Widget, XtPointer, XtPointer);
#if defined(IF_XM)
extern void dddSetToolBarCB                 (Widget, XtPointer, XtPointer);
extern void dddSetKeyboardFocusPolicyCB     (Widget, XtPointer, XtPointer);
extern void dddSetPannerCB                  (Widget, XtPointer, XtPointer);
extern void dddSetDebuggerCB                (Widget, XtPointer, XtPointer);
extern void dddToggleAutoDebuggerCB         (Widget, XtPointer, XtPointer);
extern void dddSetCutCopyPasteBindingsCB    (Widget, XtPointer, XtPointer);
extern void dddSetSelectAllBindingsCB       (Widget, XtPointer, XtPointer);

extern void dddSetUndoBufferSizeCB          (Widget, XtPointer, XtPointer);
extern void dddClearUndoBufferCB            (Widget, XtPointer, XtPointer);

extern void dddSetEditCommandCB             (Widget, XtPointer, XtPointer);
extern void dddSetPlotCommandCB             (Widget, XtPointer, XtPointer);
extern void dddSetGetCoreCommandCB          (Widget, XtPointer, XtPointer);
extern void dddSetPSCommandCB               (Widget, XtPointer, XtPointer);
extern void dddSetTermCommandCB             (Widget, XtPointer, XtPointer);
extern void dddSetUncompressCommandCB       (Widget, XtPointer, XtPointer);
extern void dddSetWWWCommandCB              (Widget, XtPointer, XtPointer);
extern void dddSetBuiltinPlotWindowCB       (Widget, XtPointer, XtPointer);

extern void DDDSaveOptionsCB                (Widget, XtPointer, XtPointer);
#else
extern void dddSetToolBarCB                 (GUI::RadioButton *, bool);
extern void dddSetKeyboardFocusPolicyCB     (GUI::Widget *, unsigned char);
extern void dddSetPannerCB                  (GUI::RadioButton *, bool);
extern void dddSetDebuggerCB                (GUI::RadioButton *, DebuggerType);
extern void dddToggleAutoDebuggerCB         (GUI::CheckButton *);
extern void dddSetCutCopyPasteBindingsCB    (GUI::RadioButton *, BindingStyle);
extern void dddSetSelectAllBindingsCB       (GUI::RadioButton *, BindingStyle);

extern void dddSetUndoBufferSizeCB          (GUI::Entry *);
extern void dddClearUndoBufferCB            (void);

extern void dddSetEditCommandCB             (GUI::Entry *);
extern void dddSetPlotCommandCB             (GUI::Entry *);
extern void dddSetGetCoreCommandCB          (GUI::Entry *);
extern void dddSetPSCommandCB               (GUI::Entry *);
extern void dddSetTermCommandCB             (GUI::Entry *);
extern void dddSetUncompressCommandCB       (GUI::Entry *);
extern void dddSetWWWCommandCB              (GUI::Entry *);
extern void dddSetBuiltinPlotWindowCB       (GUI::RadioButton *, bool);

extern void DDDSaveOptionsCB                (GUI::Widget *, unsigned long);
#endif

extern void DDDSaveOptionsAsCB              (Widget, XtPointer, XtPointer);

#endif // _DDD_options_h
// DON'T ADD ANYTHING BEHIND THIS #endif
