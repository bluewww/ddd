// $Id$
// DDD main program (and much more)

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char ddd_rcsid[] =
    "$Id$";

// Introduction to DDD
// ===================
//
// :bug: n.  An unwanted and unintended property of a program or
// piece of hardware, esp. one that causes it to malfunction.
// Antonym of {feature}.  Examples: "There's a bug in the editor: 
// it writes things out backwards."  "The system crashed because of  
// a hardware bug."  "Fred is a winner, but he has a few bugs"
// (i.e., Fred is a good guy, but he has a few personality problems).
//
// Historical note: Admiral Grace Hopper (an early computing pioneer  
// better known for inventing {COBOL}) liked to tell a story in  
// which a technician solved a {glitch} in the Harvard Mark II
// machine by pulling an actual insect out from between the contacts
// of one of its relays, and she subsequently promulgated {bug} in
// its hackish sense as a joke about the incident (though, as she was
// careful to admit, she was not there when it happened).  For many
// years the logbook associated with the incident and the actual bug 
// in question (a moth) sat in a display case at the Naval Surface
// Warfare Center (NSWC).  The entire story, with a picture of the 
// logbook and the moth taped into it, is recorded in the "Annals
// of the History of Computing", Vol. 3, No. 3 (July 1981),
// pp. 285--286.
//
// The text of the log entry (from September 9, 1947), reads "1545
// Relay #70 Panel F (moth) in relay.  First actual case of bug being
//  found".  This wording establishes that the term was already
// in use at the time in its current specific sense -- and Hopper
// herself reports that the term `bug' was regularly applied to
// problems in radar electronics during WWII.
//
// Indeed, the use of `bug' to mean an industrial defect was already  
// established in Thomas Edison's time, and a more specific and rather
// modern use can be found in an electrical handbook from 1896 
// ("Hawkin's New Catechism of Electricity", Theo. Audel & Co.)
// which says: "The term `bug' is used to a limited extent to
// designate any fault or trouble in the connections or working of 
// electric apparatus."  It further notes that the term is "said to  
// have originated in quadruplex telegraphy and have been transferred
// to all electric apparatus."
//
// The latter observation may explain a common folk etymology of the 
// term; that it came from telephone company usage, in which "bugs in
// a telephone cable" were blamed for noisy lines.  Though this
// derivation seems to be mistaken, it may well be a distorted memory
// of a joke first current among *telegraph* operators more than
// a century ago!
//
// Actually, use of `bug' in the general sense of a disruptive event
// goes back to Shakespeare!  In the first edition of Samuel Johnson's
// dictionary one meaning of `bug' is "A frightful object; a 
// walking spectre"; this is traced to `bugbear', a Welsh term for
// a variety of mythological monster which (to complete the circle)
// has recently been reintroduced into the popular lexicon through 
// fantasy role-playing games.
//
// In any case, in jargon the word almost never refers to insects.
// Here is a plausible conversation that never actually happened:
//
// "There is a bug in this ant farm!"
//
// "What do you mean?  I don't see any ants in it."
//
// "That's the bug."
//
// A careful discussion of the etymological issues can be found in a
// paper by Fred R. Shapiro, 1987, "Entomology of the Computer Bug: 
// History and Folklore", American Speech 62(4):376-378.
//
// [There has been a widespread myth that the original bug was moved
// to the Smithsonian, and an earlier version of this entry so 
// asserted.  A correspondent who thought to check discovered that the
// bug was not there.  While investigating this in late 1990, your 
// editor discovered that the NSWC still had the bug, but had 
// unsuccessfully tried to get the Smithsonian to accept it -- and
// that the present curator of their History of American Technology
// Museum didn't know this and agreed that it would make a worthwhile
// exhibit.  It was moved to the Smithsonian in mid-1991, but due to 
// space and money constraints has not yet been exhibited.  Thus, the
// process of investigating the original-computer-bug bug fixed it in
// an entirely unexpected way, by making the myth true!  -- ESR]



//-----------------------------------------------------------------------------
// Name conventions used:
// ...CB : Callback function,
// ...DCB: Dialog callback function,
// ...CD : Create a dialog
// ...Act: Action procedure
// ...HP : Handler procedure (see `HandlerL.h' for details)
//-----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
// #include "MemCheckD.h"

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "ddd.h"

// Motif stuff
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/CutPaste.h>
#include <Xm/MainW.h>
#include <Xm/PanedW.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>	// XmCreateWorkArea()
#include <Xm/Protocols.h>
#include <Xm/SelectioB.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/ArrowB.h>
#include <Xm/MwmUtil.h>
#include <X11/Shell.h>

#if XmVersion >= 1002
#include <Xm/Display.h>
#include <Xm/DragDrop.h>
#include <Xm/RepType.h>		// XmRepTypeInstallTearOffModelConverter()
#endif

#if HAVE_X11_XMU_EDITRES_H
#include <X11/Xmu/Editres.h>
#endif

#include <X11/IntrinsicP.h>	// LessTif hacks
#include "Sash.h"
#include "LessTifH.h"

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

// Lots of DDD stuff
#include "AgentM.h"
#include "AppData.h"
#include "ArgField.h"
#include "DataDisp.h"
#include "DestroyCB.h"
#include "DispGraph.h"
#include "DispGraph.h"
#include "DispBox.h"
#include "DispValue.h"
#include "ExitCB.h"
#include "GraphEdit.h"
#include "GDBAgent.h"
#include "HistoryD.h"
#include "LabelH.h"
#include "MakeMenu.h"
#include "SourceView.h"
#include "TimeOut.h"
#include "VSEFlags.h"
#include "WhatNextCB.h"
#include "args.h"
#include "assert.h"
#include "bool.h"
#include "buttons.h"
#include "charsets.h"
#include "cmdtty.h"
#include "comm-manag.h"
#include "Command.h"
#include "complete.h"
#include "configinfo.h"
#include "converters.h"
#include "cook.h"
#include "data.h"
#include "dbx-lookup.h"
#include "disp-read.h"
#include "editing.h"
#include "environ.h"
#include "examine.h"
#include "exectty.h"
#include "exit.h"
#include "expired.h"
#include "file.h"
#include "filetype.h"
#include "findParent.h"
#include "fonts.h"
#include "frame.h"
#include "gdbinit.h"
#include "getrlimit.h"
#include "history.h"
#include "home.h"
#include "host.h"
#include "hostname.h"
#include "logo.h"
#include "logplayer.h"
#include "longName.h"
#include "mainloop.h"
#include "options.h"
#include "post.h"
#include "print.h"
#include "question.h"
#include "regexps.h"
#include "resources.h"
#include "sashes.h"
#include "select.h"
#include "session.h"
#include "settings.h"
#include "shell.h"
#include "shorten.h"
#include "show.h"
#include "source.h"
#include "status.h"
#include "strclass.h"
#include "string-fun.h"
#include "tips.h"
#include "toolbar.h"
#include "ungrab.h"
#include "verify.h"
#include "version.h"
#include "windows.h"
#include "wm.h"
#include "xconfig.h"

// Standard stuff
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <time.h>
#include <signal.h>

#if HAVE_STD_EXCEPTIONS
#define string stdstring	  // Avoid `string' name clash
#define __SGI_STL_INTERNAL_RELOPS // Avoid <stl_relops.h> warning
#include <stdexcept>
#undef string
#endif // HAVE_STD_EXCEPTIONS


//-----------------------------------------------------------------------------
// Forward function decls
//-----------------------------------------------------------------------------

// Callbacks
static void gdb_readyHP        (Agent *, void *, void *);
static void gdb_strangeHP      (Agent *, void *, void *);
static void gdb_recordingHP    (Agent *, void *, void *);
static void gdb_panicHP        (Agent *, void *, void *);
static void gdb_echo_detectedHP(Agent *, void *, void *);
static void language_changedHP (Agent *, void *, void *);
static void report_languageHP  (Agent *, void *, void *);
static void source_argHP       (void *, void *, void *call_data);

// Warning proc
static void ddd_xt_warning(String message);

// Getting ready
static void WhenReady            (Widget, XtPointer, XtPointer);

// Cut and Paste
static void gdbCutSelectionCB    (Widget, XtPointer, XtPointer);
static void gdbCopySelectionCB   (Widget, XtPointer, XtPointer);
static void gdbPasteClipboardCB  (Widget, XtPointer, XtPointer);
static void gdbDeleteSelectionCB (Widget, XtPointer, XtPointer);
static void gdbClearAllCB        (Widget, XtPointer, XtPointer);
static void gdbSelectAllCB       (Widget, XtPointer, XtPointer);
static void gdbUnselectAllCB     (Widget, XtPointer, XtPointer);

// Update menus
static void gdbUpdateEditCB      (Widget, XtPointer, XtPointer);
static void gdbUpdateFileCB      (Widget, XtPointer, XtPointer);
static void gdbUpdateViewCB      (Widget, XtPointer, XtPointer);
static void gdbUpdateViewsCB     (Widget, XtPointer, XtPointer);
static void gdbUpdateAllMenus();

// Preferences
static void make_preferences (Widget parent);
static void dddPopupPreferencesCB (Widget, XtPointer, XtPointer);

// Synchronize `():' fields
static void sync_args(ArgField *a1, ArgField *a2);

// Return true if user interaction events are pending
static bool pending_interaction();

// Create status line
static void create_status(Widget parent);

// Status LED
static void blink(bool set);
static void ToggleBlinkCB(Widget, XtPointer client_data, XtPointer call_data);
static void DisableBlinkHP(Agent *, void *, void *);

// GDB Status
static void ReportStartHP(Agent *, void *, void *);
static void ReportDeathHP(Agent *, void *, void *);

// Status history
static void PopupStatusHistoryCB(Widget, XtPointer, XtPointer);
static void PopdownStatusHistoryCB(Widget, XtPointer, XtPointer);

// Argument callback
static void ActivateCB(Widget, XtPointer client_data, XtPointer call_data);

// Drag and drop
static void CheckDragCB(Widget, XtPointer client_data, XtPointer call_data);

// Verify whether buttons are active
static void verify_buttons(MMDesc *items);

// Setup shortcut menu
static void set_shortcut_menu(DataDisp *data_disp, string expressions);

// Fix the size of the status line
static void fix_status_size();

// Setup new shell
static void setup_new_shell(Widget w);

// Check if window manager decorates transients
static void start_have_decorated_transients(Widget parent);
static bool have_decorated_transients();

// Set `Settings' title
static void set_settings_title(Widget w);

// Set Cut/Copy/Paste bindings for MENU to STYLE
static void set_cut_copy_paste_bindings(MMDesc *menu, BindingStyle style);

// Popup DDD splash screen upon start-up.
static void SetSplashScreenCB(Widget, XtPointer, XtPointer);
static void popup_splash_screen(Widget parent, string color_key);
static void popdown_splash_screen(XtPointer data = 0, XtIntervalId *id = 0);

// Read in database from FILENAME.  Upon version mismatch, ignore some
// resources such as window sizes.
static XrmDatabase GetFileDatabase(char *filename);

// Lock `~/.ddd'
static void lock_ddd(Widget parent);

// Various setups
static void setup_version_info();
static void setup_environment();
static void setup_command_tool();
static void setup_options(int& argc, char *argv[],
			  StringArray& saved_options, string& gdb_name,
			  bool& no_windows);
static void setup_tty();
static void setup_version_warnings();
static void setup_auto_command_prefix();
static void setup_core_limit();
static void setup_options();
static void setup_cut_copy_paste_bindings(XrmDatabase db);

// Help hooks
static void PreHelpOnContext(Widget w, XtPointer, XtPointer);
static void PostHelpOnItem(Widget item);

// Log player stuff
static void check_log(const string& logname, DebuggerType& type);


//-----------------------------------------------------------------------------
// Xt Stuff
//-----------------------------------------------------------------------------

static char ON[]  = "on";
static char OFF[] = "off";

// Options
// Note: we support both the GDB '--OPTION' and the X '-OPTION' convention.
static XrmOptionDescRec options[] = {
{ "--session",              XtNsession,              XrmoptionSepArg, NULL },
{ "-session",               XtNsession,              XrmoptionSepArg, NULL },

{ "-xtsessionID",           XtNsession,              XrmoptionSepArg, NULL },

{ "--debugger",             XtNdebuggerCommand,      XrmoptionSepArg, NULL },
{ "-debugger",              XtNdebuggerCommand,      XrmoptionSepArg, NULL },

{ "--gdb",                  XtNdebugger,             XrmoptionNoArg,  "gdb" },
{ "-gdb",                   XtNdebugger,             XrmoptionNoArg,  "gdb" },

{ "--dbx",                  XtNdebugger,             XrmoptionNoArg,  "dbx" },
{ "-dbx",                   XtNdebugger,             XrmoptionNoArg,  "dbx" },

{ "--xdb",                  XtNdebugger,             XrmoptionNoArg,  "xdb" },
{ "-xdb",                   XtNdebugger,             XrmoptionNoArg,  "xdb" },

{ "--jdb",                  XtNdebugger,             XrmoptionNoArg,  "jdb" },
{ "-jdb",                   XtNdebugger,             XrmoptionNoArg,  "jdb" },

{ "--trace",                XtNtrace,                XrmoptionNoArg,  ON },
{ "-trace",                 XtNtrace,                XrmoptionNoArg,  ON },

{ "--play-log",		    XtNplayLog,              XrmoptionSepArg, NULL },
{ "-play-log",		    XtNplayLog,              XrmoptionSepArg, NULL },

{ "--font",                 XtNdefaultFont,          XrmoptionSepArg, NULL },
{ "-font",                  XtNdefaultFont,          XrmoptionSepArg, NULL },
{ "-fn",                    XtNdefaultFont,          XrmoptionSepArg, NULL },

{ "--fontsize",             XtCFontSize,             XrmoptionSepArg, NULL },
{ "-fontsize",              XtCFontSize,             XrmoptionSepArg, NULL },

{ "--vsl-library",          XtNvslLibrary,           XrmoptionSepArg, NULL },
{ "-vsl-library",           XtNvslLibrary,           XrmoptionSepArg, NULL },

{ "--vsl-path",             XtNvslPath,              XrmoptionSepArg, NULL },
{ "-vsl-path",              XtNvslPath,              XrmoptionSepArg, NULL },

{ "--separate",             XtCSeparate,             XrmoptionNoArg, ON },
{ "-separate",              XtCSeparate,             XrmoptionNoArg, ON },
{ "--separate-windows",     XtCSeparate,             XrmoptionNoArg, ON },
{ "-separate-windows",      XtCSeparate,             XrmoptionNoArg, ON },

{ "--separate-source-window", XtNseparateSourceWindow, XrmoptionNoArg, ON },
{ "-separate-source-window",  XtNseparateSourceWindow, XrmoptionNoArg, ON },

{ "--separate-data-window", XtNseparateDataWindow,   XrmoptionNoArg, ON },
{ "-separate-data-window",  XtNseparateDataWindow,   XrmoptionNoArg, ON },

{ "--attach",               XtCSeparate,             XrmoptionNoArg, OFF },
{ "-attach",                XtCSeparate,             XrmoptionNoArg, OFF },
{ "--attach-windows",       XtCSeparate,             XrmoptionNoArg, OFF },
{ "-attach-windows",        XtCSeparate,             XrmoptionNoArg, OFF },

{ "--attach-source-window", XtNseparateSourceWindow, XrmoptionNoArg, OFF },
{ "-attach-source-window",  XtNseparateSourceWindow, XrmoptionNoArg, OFF },

{ "--attach-data-window",   XtNseparateDataWindow,   XrmoptionNoArg, OFF },
{ "-attach-data-window",    XtNseparateDataWindow,   XrmoptionNoArg, OFF },

{ "--exec-window",          XtNseparateExecWindow,   XrmoptionNoArg, ON },
{ "-exec-window",           XtNseparateExecWindow,   XrmoptionNoArg, ON },

{ "--no-exec-window",       XtNseparateExecWindow,   XrmoptionNoArg, OFF },
{ "-no-exec-window",        XtNseparateExecWindow,   XrmoptionNoArg, OFF },

{ "--source-window",        XtNopenSourceWindow,     XrmoptionNoArg, ON },
{ "-source-window",         XtNopenSourceWindow,     XrmoptionNoArg, ON },

{ "--no-source-window",     XtNopenSourceWindow,     XrmoptionNoArg, OFF },
{ "-no-source-window",      XtNopenSourceWindow,     XrmoptionNoArg, OFF },

{ "--data-window",          XtNopenDataWindow,       XrmoptionNoArg, ON },
{ "-data-window",           XtNopenDataWindow,       XrmoptionNoArg, ON },

{ "--no-data-window",       XtNopenDataWindow,       XrmoptionNoArg, OFF },
{ "-no-data-window",        XtNopenDataWindow,       XrmoptionNoArg, OFF },

{ "--debugger-console",     XtNopenDebuggerConsole,  XrmoptionNoArg, ON },
{ "-debugger-console",      XtNopenDebuggerConsole,  XrmoptionNoArg, ON },

{ "--no-debugger-console",  XtNopenDebuggerConsole,  XrmoptionNoArg, OFF },
{ "-no-debugger-console",   XtNopenDebuggerConsole,  XrmoptionNoArg, OFF },

{ "--button-tips",          XtNbuttonTips,           XrmoptionNoArg, ON },
{ "-button-tips",           XtNbuttonTips,           XrmoptionNoArg, ON },

{ "--no-button-tips",       XtNbuttonTips,           XrmoptionNoArg, OFF },
{ "-no-button-tips",        XtNbuttonTips,           XrmoptionNoArg, OFF },

{ "--value-tips",           XtNvalueTips,            XrmoptionNoArg, ON },
{ "-value-tips",            XtNvalueTips,            XrmoptionNoArg, ON },

{ "--no-value-tips",        XtNvalueTips,            XrmoptionNoArg, OFF },
{ "-no-value-tips",         XtNvalueTips,            XrmoptionNoArg, OFF },

{ "--status-at-bottom",     XtNstatusAtBottom,       XrmoptionNoArg, ON },
{ "-status-at-bottom",      XtNstatusAtBottom,       XrmoptionNoArg, ON },

{ "--status-at-top",        XtNstatusAtBottom,       XrmoptionNoArg, OFF },
{ "-status-at-top",         XtNstatusAtBottom,       XrmoptionNoArg, OFF },

{ "--panned-graph-editor",  XtNpannedGraphEditor,    XrmoptionNoArg, ON },
{ "-panned-graph-editor",   XtNpannedGraphEditor,    XrmoptionNoArg, ON },

{ "--scrolled-graph-editor", XtNpannedGraphEditor,   XrmoptionNoArg, OFF },
{ "-scrolled-graph-editor", XtNpannedGraphEditor,    XrmoptionNoArg, OFF },

{ "--sync-debugger",        XtNsynchronousDebugger,  XrmoptionNoArg, ON },
{ "-sync-debugger",         XtNsynchronousDebugger,  XrmoptionNoArg, ON },

{ "--disassemble",          XtNdisassemble,          XrmoptionNoArg, ON },
{ "-disassemble",           XtNdisassemble,          XrmoptionNoArg, ON },

{ "--no-disassemble",       XtNdisassemble,          XrmoptionNoArg, OFF },
{ "-no-disassemble",        XtNdisassemble,          XrmoptionNoArg, OFF },

{ "--glyphs",               XtNdisplayGlyphs,        XrmoptionNoArg, ON },
{ "-glyphs",                XtNdisplayGlyphs,        XrmoptionNoArg, ON },

{ "--no-glyphs",            XtNdisplayGlyphs,        XrmoptionNoArg, OFF },
{ "-no-glyphs",             XtNdisplayGlyphs,        XrmoptionNoArg, OFF },

{ "--host",                 XtNdebuggerHost,         XrmoptionSepArg, NULL },
{ "-host",                  XtNdebuggerHost,         XrmoptionSepArg, NULL },

{ "--rhost",                XtNdebuggerRHost,        XrmoptionSepArg, NULL },
{ "-rhost",                 XtNdebuggerRHost,        XrmoptionSepArg, NULL },

{ "--login",                XtNdebuggerHostLogin,    XrmoptionSepArg, NULL },
{ "-login",                 XtNdebuggerHostLogin,    XrmoptionSepArg, NULL },
{ "-l",                     XtNdebuggerHostLogin,    XrmoptionSepArg, NULL },

{ "--tty",                  XtNttyMode,              XrmoptionNoArg, ON },
{ "-tty",                   XtNttyMode,              XrmoptionNoArg, ON },
{ "-t",                     XtNttyMode,              XrmoptionNoArg, ON },

{ "--fullname",             XtCTTYMode,              XrmoptionNoArg, ON },
{ "-fullname",              XtCTTYMode,              XrmoptionNoArg, ON },
{ "-f",                     XtCTTYMode,              XrmoptionNoArg, ON },

{ "--version",              XtNshowVersion,          XrmoptionNoArg, ON },
{ "-version",               XtNshowVersion,          XrmoptionNoArg, ON },
{ "-v",                     XtNshowVersion,          XrmoptionNoArg, ON },

{ "--configuration",        XtNshowConfiguration,    XrmoptionNoArg, ON },
{ "-configuration",         XtNshowConfiguration,    XrmoptionNoArg, ON },

{ "--manual",               XtNshowManual,           XrmoptionNoArg, ON },
{ "-manual",                XtNshowManual,           XrmoptionNoArg, ON },

{ "--license",              XtNshowLicense,          XrmoptionNoArg, ON },
{ "-license",               XtNshowLicense,          XrmoptionNoArg, ON },

{ "--news",                 XtNshowNews,             XrmoptionNoArg, ON },
{ "-news",                  XtNshowNews,             XrmoptionNoArg, ON },

{ "--fonts",                XtNshowFonts,            XrmoptionNoArg, ON },
{ "-fonts",                 XtNshowFonts,            XrmoptionNoArg, ON },

{ "--roulette",             XtNroulette,             XrmoptionNoArg, ON },
{ "-roulette",              XtNroulette,             XrmoptionNoArg, ON },

{ "--russian-roulette",     XtNrussianRoulette,      XrmoptionNoArg, ON },
{ "-russian-roulette",      XtNrussianRoulette,      XrmoptionNoArg, ON },

{ "--check-configuration",  XtNcheckConfiguration,   XrmoptionNoArg, ON },
{ "-check-configuration",   XtNcheckConfiguration,   XrmoptionNoArg, ON },

{ "--lesstif-hacks",        XtNlessTifVersion,       XrmoptionNoArg, "999" },
{ "-lesstif-hacks",         XtNlessTifVersion,       XrmoptionNoArg, "999" },

{ "--no-lesstif-hacks",     XtNlessTifVersion,       XrmoptionNoArg, "1000" },
{ "-no-lesstif-hacks",      XtNlessTifVersion,       XrmoptionNoArg, "1000" },

{ "--lesstif-version",      XtNlessTifVersion,       XrmoptionSepArg, NULL },
{ "-lesstif-version",       XtNlessTifVersion,       XrmoptionSepArg, NULL },

{ "--help",                 XtNshowInvocation,       XrmoptionNoArg, ON },
{ "-help",                  XtNshowInvocation,       XrmoptionNoArg, ON },
{ "-h",                     XtNshowInvocation,       XrmoptionNoArg, ON },
{ "--?",                    XtNshowInvocation,       XrmoptionNoArg, ON },
{ "-?",                     XtNshowInvocation,       XrmoptionNoArg, ON },

};

// Actions
static XtActionsRec actions [] = {
    {"gdb-control",            controlAct},
    {"gdb-interrupt",          interruptAct},
    {"gdb-command",            commandAct},
    {"gdb-process",            processAct},
    {"gdb-delete-or-control",  delete_or_controlAct},
    {"gdb-prev-history",       prev_historyAct},
    {"gdb-previous-history",   prev_historyAct},
    {"gdb-next-history",       next_historyAct},
    {"gdb-beginning-of-line",  beginning_of_lineAct},
    {"gdb-end-of-line",        end_of_lineAct},
    {"gdb-forward-character",  forward_characterAct},
    {"gdb-backward-character", backward_characterAct},
    {"gdb-set-line",           set_lineAct},
    {"gdb-complete-command",   complete_commandAct},
    {"gdb-complete-arg",       complete_argAct},
    {"gdb-complete-tab",       complete_tabAct},
    {"gdb-insert-source-arg",  insert_source_argAct},
    {"gdb-insert-graph-arg",   insert_graph_argAct},
    {"gdb-isearch-prev",       isearch_prevAct},
    {"gdb-isearch-next",       isearch_nextAct},
    {"gdb-isearch-exit",       isearch_exitAct},
    {"gdb-popup-menu",         popupAct},
    {"ddd-next-tab-group",     next_tab_groupAct},
    {"ddd-prev-tab-group",     prev_tab_groupAct},
    {"ddd-previous-tab-group", prev_tab_groupAct},
    {"ddd-get-focus",          get_focusAct},
};


//-----------------------------------------------------------------------------
// Menus
//-----------------------------------------------------------------------------

struct RecentItems {
    enum RecentItem { R1, R2, R3, R4, R5, R6, R7, R8, R9 };
};

#define RECENT_MENU \
{ \
    { "r1", MMPush, { gdbOpenRecentCB, XtPointer(1) }}, \
    { "r2", MMPush, { gdbOpenRecentCB, XtPointer(2) }}, \
    { "r3", MMPush, { gdbOpenRecentCB, XtPointer(3) }}, \
    { "r4", MMPush, { gdbOpenRecentCB, XtPointer(4) }}, \
    { "r5", MMPush, { gdbOpenRecentCB, XtPointer(5) }}, \
    { "r6", MMPush, { gdbOpenRecentCB, XtPointer(6) }}, \
    { "r7", MMPush, { gdbOpenRecentCB, XtPointer(7) }}, \
    { "r8", MMPush, { gdbOpenRecentCB, XtPointer(8) }}, \
    { "r9", MMPush, { gdbOpenRecentCB, XtPointer(9) }}, \
    MMEnd \
}

static MMDesc command_recent_menu[] = RECENT_MENU;
static MMDesc source_recent_menu[]  = RECENT_MENU;
static MMDesc data_recent_menu[]    = RECENT_MENU;

struct FileItems {
    enum FileItem { OpenFile, OpenClass, Recent, OpenCore, OpenSource, Sep1,
		    OpenSession, SaveSession, Sep2,
		    Attach, Detach, Sep3,
		    Print, PrintAgain, Sep4,
		    CD, Sep5,
		    Make, MakeAgain, Sep6,
		    Close, Restart, Exit
    };
};

#define FILE_MENU(recent_menu) \
{ \
    { "open_file",     MMPush, { WhenReady, XtPointer(gdbOpenFileCB) }}, \
    { "open_class",    MMPush | MMUnmanaged, { WhenReady, XtPointer(gdbOpenClassCB) }}, \
    { "recent",        MMMenu, MMNoCB, recent_menu }, \
    { "open_core",     MMPush, { WhenReady, XtPointer(gdbOpenCoreCB) }}, \
    { "open_source",   MMPush, { gdbLookupSourceCB }}, \
    MMSep, \
    { "open_session",  MMPush, { WhenReady, XtPointer(OpenSessionCB) }}, \
    { "save_session",  MMPush, { WhenReady, XtPointer(SaveSessionAsCB) }}, \
    MMSep, \
    { "attach",        MMPush, { WhenReady, XtPointer(gdbOpenProcessCB) }}, \
    { "detach",        MMPush, { gdbCommandCB, "detach" }}, \
    MMSep, \
    { "print",         MMPush, { graphPrintCB }}, \
    { "printAgain",    MMPush | MMUnmanaged, \
 	                       { graphQuickPrintCB, XtPointer(1) }}, \
    { "separator",     MMSeparator | MMUnmanaged }, \
    { "cd",            MMPush, { WhenReady, XtPointer(gdbChangeDirectoryCB) }}, \
    { "separator",     MMSeparator | MMUnmanaged }, \
    { "make",          MMPush, { WhenReady, XtPointer(gdbMakeCB) }}, \
    { "makeAgain",     MMPush | MMUnmanaged, { WhenReady, XtPointer(gdbMakeAgainCB) }}, \
    MMSep, \
    { "close",         MMPush, { DDDCloseCB }}, \
    { "restart",       MMPush, { DDDRestartCB }}, \
    { "exit",          MMPush, { DDDExitCB, XtPointer(EXIT_SUCCESS) }}, \
    MMEnd \
}

static MMDesc command_file_menu[] = FILE_MENU(command_recent_menu);
static MMDesc source_file_menu[]  = FILE_MENU(source_recent_menu);
static MMDesc data_file_menu[]    = FILE_MENU(data_recent_menu);


struct ProgramItems {
    enum ProgramItem { Run, RunAgain, Sep1,
		       SeparateExecWindow, Sep2,
		       Step, Stepi, Next, Nexti, Until, Finish, Sep3,
		       Cont, Signal0, Sep4,
		       Kill, Break, Quit
    };
};

#define PROGRAM_MENU(w) \
{ \
    { "run",         MMPush, { gdbRunCB }}, \
    { "run_again",   MMPush, { gdbCommandCB, "run" }}, \
    MMSep, \
    { "separateExecWindow",  MMToggle, \
	{ dddToggleSeparateExecWindowCB }, NULL, &(w) }, \
    MMSep, \
    { "step",        MMPush, { gdbCommandCB, "step" }}, \
    { "stepi",       MMPush, { gdbCommandCB, "stepi" }}, \
    { "next",        MMPush, { gdbCommandCB, "next" }}, \
    { "nexti",       MMPush, { gdbCommandCB, "nexti" }}, \
    { "until",       MMPush, { gdbCommandCB, "until" }}, \
    { "finish",      MMPush, { gdbCommandCB, "finish" }}, \
    MMSep, \
    { "cont",        MMPush, { gdbCommandCB, "cont" }}, \
    { "signal0",     MMPush, { gdbCommandCB, "signal 0" }}, \
    MMSep, \
    { "kill",        MMPush, { gdbCommandCB, "kill" }}, \
    { "break",       MMPush, { gdbCommandCB, "\003" }}, \
    { "quit",        MMPush, { gdbCommandCB, "\034" }}, \
    MMEnd \
}

static Widget command_separate_exec_window_w;
static Widget source_separate_exec_window_w;
static Widget data_separate_exec_window_w;

static MMDesc command_program_menu[]
    = PROGRAM_MENU(command_separate_exec_window_w);
static MMDesc source_program_menu[]
    = PROGRAM_MENU(source_separate_exec_window_w);
static MMDesc data_program_menu[]
    = PROGRAM_MENU(data_separate_exec_window_w);

enum DDDWindow { ToolWindow, 
		 ExecWindow, 
		 Sep,
		 GDBWindow, 
		 SourceWindow,
		 DataWindow, 
		 CodeWindow,
		 CommonWindow };

#define VIEW_MENU \
{ \
    { "tool",    MMPush,   { gdbOpenToolWindowCB }}, \
    { "exec",    MMPush,   { gdbOpenExecWindowCB }}, \
    MMSep, \
    { "console", MMPush,   { gdbOpenCommandWindowCB }}, \
    { "source",  MMPush,   { gdbOpenSourceWindowCB }}, \
    { "data",    MMPush,   { gdbOpenDataWindowCB }}, \
    { "code",    MMToggle | MMUnmanaged, { gdbToggleCodeWindowCB }}, \
    MMEnd \
}

static MMDesc command_view_menu[] = VIEW_MENU;
static MMDesc source_view_menu[]  = VIEW_MENU;
static MMDesc data_view_menu[]    = VIEW_MENU;

static MMDesc views_menu[] = 
{
    { "tool",    MMPush,   { gdbOpenToolWindowCB }},
    { "exec",    MMPush,   { gdbOpenExecWindowCB }},
    MMSep,
    { "console", MMToggle, { gdbToggleCommandWindowCB }},
    { "source",  MMToggle, { gdbToggleSourceWindowCB }},
    { "data",    MMToggle, { gdbToggleDataWindowCB }},
    { "code",    MMToggle, { gdbToggleCodeWindowCB }},
    MMEnd
};

struct EditItems {
    enum EditItem { 
	Undo, Redo, Sep1,
	Cut, Copy, Paste, ClearAll, Delete, Sep2, 
	SelectAll, Sep3,
	Preferences, Settings, Sep4,
	SaveOptions
    };
};

#define EDIT_MENU(win) \
{ \
    { "undo",        MMPush | MMUnmanaged }, \
    { "redo",        MMPush | MMUnmanaged }, \
    { "separator",   MMSeparator | MMUnmanaged }, \
    { "cut",         MMPush,  { gdbCutSelectionCB,    XtPointer(win) }}, \
    { "copy",        MMPush,  { gdbCopySelectionCB,   XtPointer(win) }}, \
    { "paste",       MMPush,  { gdbPasteClipboardCB,  XtPointer(win) }}, \
    { "clearAll",    MMPush,  { gdbClearAllCB,        XtPointer(win) }}, \
    { "delete",      MMPush,  { gdbDeleteSelectionCB, XtPointer(win) }}, \
    MMSep, \
    { "selectAll",   MMPush,  { gdbSelectAllCB,       XtPointer(win) }}, \
    MMSep, \
    { "preferences", MMPush,  { dddPopupPreferencesCB }}, \
    { "settings",    MMPush,  { WhenReady, XtPointer(dddPopupSettingsCB) }}, \
    MMSep, \
    { "saveOptions", MMPush,  { DDDSaveOptionsCB, XtPointer(SAVE_DEFAULT) }}, \
    MMEnd \
}

static MMDesc command_edit_menu[] = EDIT_MENU(GDBWindow);
static MMDesc source_edit_menu[]  = EDIT_MENU(SourceWindow);
static MMDesc data_edit_menu[]    = EDIT_MENU(DataWindow);

static Widget complete_w;
static Widget define_w;

static MMDesc command_menu[] =
{
    { "history",  MMPush, { gdbHistoryCB }},
    MMSep,
    { "prev",     MMPush, { gdbPrevCB }},
    { "next",     MMPush, { gdbNextCB }},
    MMSep,
    { "isearch_prev", MMPush, { gdbISearchPrevCB }},
    { "isearch_next", MMPush, { gdbISearchNextCB }},
    { "isearch_exit", MMPush, { gdbISearchExitCB }},
    MMSep,
    { "complete", MMPush, { WhenReady, XtPointer(gdbCompleteCB) }, 
      NULL, &complete_w },
    { "apply",    MMPush, { WhenReady, XtPointer(gdbApplyCB) }},
    MMSep,
    { "clear_line",   MMPush, { gdbClearCB }},
    { "clear_window", MMPush, { gdbClearWindowCB }},
    MMSep,
    { "define",   MMPush, { dddDefineCommandCB }, NULL, &define_w },
    { "buttons",  MMPush, { dddEditButtonsCB }},
    MMEnd
};

static Widget registers_w;
static Widget threads_w;
static Widget signals_w;

static MMDesc stack_menu[] =
{
    { "stack",      MMPush,  { WhenReady, 
			       XtPointer(SourceView::ViewStackFramesCB) }},
    { "registers",  MMPush,  { WhenReady, 
			       XtPointer(SourceView::ViewRegistersCB) },
      NULL, &registers_w },
    { "threads",    MMPush,  { WhenReady,
			       XtPointer(SourceView::ViewThreadsCB) },
      NULL, &threads_w },
    { "signals",    MMPush,  { WhenReady,
			       XtPointer(dddPopupSignalsCB) },
      NULL, &signals_w },
    MMSep,
    { "up",         MMPush,  { gdbCommandCB, "up" }},
    { "down",       MMPush,  { gdbCommandCB, "down" }},
    MMEnd
};

static Widget lookup_w;
static Widget find_forward_w;
static Widget find_backward_w;
static Widget find_words_only_w;
static Widget find_case_sensitive_w;
static Widget disassemble_w;
static Widget edit_source_w;
static Widget reload_source_w;

static MMDesc source_menu[] =
{
    { "breakpoints", MMPush, { SourceView::EditBreakpointsCB }},
    MMSep,
    {"lookup",        MMPush, { gdbLookupCB       },
      NULL, &lookup_w },
    { "findForward",  MMPush, { gdbFindCB, XtPointer(SourceView::forward) },
      NULL, &find_forward_w },
    { "findBackward", MMPush, { gdbFindCB, XtPointer(SourceView::backward) },
      NULL, &find_backward_w },
    MMSep,
    { "findWordsOnly",       MMToggle, { sourceToggleFindWordsOnlyCB }, 
      NULL, &find_words_only_w },
    { "findCaseSensitive",   MMToggle, { sourceToggleFindCaseSensitiveCB }, 
      NULL, &find_case_sensitive_w },
    MMSep,
    { "disassemble",         MMToggle,  { gdbToggleCodeWindowCB },
      NULL, &disassemble_w },
    MMSep,
    { "edit",       MMPush,  { gdbEditSourceCB }, NULL, &edit_source_w },
    { "reload",     MMPush,  { gdbReloadSourceCB }, NULL, &reload_source_w },
    MMSep,
    { "back",       MMPush,  { gdbGoBackCB }},
    { "forward",    MMPush,  { gdbGoForwardCB }},
    MMEnd
};


// Preferences

// General preferences

static Widget button_tips_w;
static Widget button_docs_w;
static MMDesc button_menu [] = 
{
    { "tips",  MMToggle, { dddToggleButtonTipsCB }, NULL, &button_tips_w },
    { "docs",  MMToggle, { dddToggleButtonDocsCB }, NULL, &button_docs_w },
    MMEnd
};

static Widget value_tips_w;
static Widget value_docs_w;
static MMDesc value_menu [] = 
{
    { "tips",  MMToggle, { dddToggleValueTipsCB }, NULL, &value_tips_w },
    { "docs",  MMToggle, { dddToggleValueDocsCB }, NULL, &value_docs_w },
    MMEnd
};

static Widget set_global_completion_w;
static Widget set_console_completion_w;
static MMDesc completion_menu [] = 
{
    { "inAllWindows", MMToggle, 
      { dddSetGlobalTabCompletionCB, XtPointer(True) }, 
      NULL, &set_global_completion_w },
    { "inConsole", MMToggle, { dddSetGlobalTabCompletionCB, XtPointer(False) },
      NULL, &set_console_completion_w },
    MMEnd
};

static Widget group_iconify_w;
static Widget uniconify_when_ready_w;
static Widget suppress_warnings_w;
static Widget check_grabs_w;

static MMDesc general_preferences_menu[] = 
{
    { "buttonHints",         MMButtonPanel, MMNoCB, button_menu },
    { "valueHints",          MMButtonPanel, MMNoCB, value_menu },
    { "tabCompletion",       MMRadioPanel,  MMNoCB, completion_menu },
    { "groupIconify",        MMToggle, { dddToggleGroupIconifyCB },
      NULL, &group_iconify_w },
    { "uniconifyWhenReady",  MMToggle, { dddToggleUniconifyWhenReadyCB },
      NULL, &uniconify_when_ready_w },
    { "suppressWarnings",    MMToggle, { dddToggleSuppressWarningsCB },
      NULL, &suppress_warnings_w },
    { "checkGrabs",          MMToggle, { dddToggleCheckGrabsCB },
      NULL, &check_grabs_w },
    MMEnd
};


// Source preferences
static Widget set_display_glyphs_w;
static Widget set_display_text_w;
static MMDesc glyph_menu[] =
{
    { "asGlyphs", MMToggle, { sourceSetDisplayGlyphsCB, XtPointer(True) },
      NULL, &set_display_glyphs_w },
    { "asText", MMToggle, { sourceSetDisplayGlyphsCB, XtPointer(False) },
      NULL, &set_display_text_w },
    MMEnd
};

static Widget set_tool_buttons_in_toolbar_w;
static Widget set_tool_buttons_in_command_tool_w;
static MMDesc tool_buttons_menu [] = 
{
    { "commandTool",  MMToggle, { dddSetToolBarCB, XtPointer(False) },
      NULL, &set_tool_buttons_in_command_tool_w },
    { "sourceWindow", MMToggle, { dddSetToolBarCB, XtPointer(True) },
      NULL, &set_tool_buttons_in_toolbar_w },
    MMEnd
};

static Widget set_refer_path_w;
static Widget set_refer_base_w;
static MMDesc refer_menu[] =
{
    { "byPath", MMToggle, { sourceSetUseSourcePathCB, XtPointer(True) },
      NULL, &set_refer_path_w },
    { "byBase", MMToggle, { sourceSetUseSourcePathCB, XtPointer(False) },
      NULL, &set_refer_base_w },
    MMEnd
};

static Widget words_only_w;
static Widget case_sensitive_w;
static MMDesc find_preferences_menu[] =
{
    { "wordsOnly", MMToggle, { sourceToggleFindWordsOnlyCB }, 
      NULL, &words_only_w },
    { "caseSensitive", MMToggle, { sourceToggleFindCaseSensitiveCB }, 
      NULL, &case_sensitive_w },
    MMEnd
};

static Widget cache_source_files_w;
static Widget cache_machine_code_w;
static MMDesc cache_menu[] =
{
    { "cacheSource", MMToggle, { sourceToggleCacheSourceFilesCB }, 
      NULL, &cache_source_files_w },
    { "cacheCode", MMToggle, { sourceToggleCacheMachineCodeCB }, 
      NULL, &cache_machine_code_w },
    MMEnd
};

static Widget tab_width_w;
static Widget source_indent_w;
static Widget code_indent_w;
static MMDesc scales_menu[] = 
{
    { "tabWidth", MMScale, { sourceSetTabWidthCB }, NULL, &tab_width_w },
    { "sourceIndent", MMScale, { sourceSetSourceIndentCB }, NULL, 
      &source_indent_w },
    { "codeIndent", MMScale, { sourceSetCodeIndentCB }, NULL, 
      &code_indent_w },
    MMEnd
};


static Widget refer_sources_w;
static MMDesc source_preferences_menu[] = 
{
    { "showExecPos",      MMRadioPanel, MMNoCB, glyph_menu },
    { "toolButtons",      MMRadioPanel,  MMNoCB, tool_buttons_menu },
    { "referSources",     MMRadioPanel, MMNoCB, refer_menu, &refer_sources_w },
    { "find",             MMButtonPanel, MMNoCB, find_preferences_menu },
    { "cache",            MMButtonPanel, MMNoCB, cache_menu },
    { "scales",       	  MMPanel | MMUnmanagedLabel, MMNoCB, scales_menu },
    MMEnd
};


// Data preferences
static Widget graph_show_hints_w;
static Widget graph_show_annotations_w;
static Widget graph_show_dependent_titles_w;

static MMDesc data_appearance_menu[] =
{
    { "hints", MMToggle, { graphToggleShowHintsCB },
      NULL, &graph_show_hints_w },
    { "annotations", MMToggle, { graphToggleShowAnnotationsCB },
      NULL, &graph_show_annotations_w },
    { "dependentTitles", MMToggle, { graphToggleShowDependentTitlesCB },
      NULL, &graph_show_dependent_titles_w },
    MMEnd
};

static Widget graph_detect_aliases_w;
static Widget graph_align_2d_arrays_w;
static Widget graph_auto_close_w;

static MMDesc data_preferences_menu[] = 
{
    { "appearance", MMButtonPanel, MMNoCB, data_appearance_menu },
    { "detectAliases", MMToggle,  { graphToggleDetectAliasesCB },
      NULL, &graph_detect_aliases_w },
    { "align2dArrays", MMToggle,  { graphToggleAlign2dArraysCB },
      NULL, &graph_align_2d_arrays_w },
    { "autoClose", MMToggle,  { graphToggleAutoCloseCB },
      NULL, &graph_auto_close_w },
    MMEnd
};

static Widget graph_compact_layout_w;
static Widget graph_auto_layout_w;
static Widget graph_snap_to_grid_w;
static Widget graph_grid_size_w;

// Layout preferences
static MMDesc layout_preferences_menu[] =
{
    { "compact", MMToggle,  { graphToggleCompactLayoutCB },
      NULL, &graph_compact_layout_w },
    { "auto",    MMToggle,  { graphToggleAutoLayoutCB },
      NULL, &graph_auto_layout_w },
    { "snapToGrid",    MMToggle,  { graphToggleSnapToGridCB },
      NULL, &graph_snap_to_grid_w },
    { "gridSize",      MMScale,   { graphSetGridSizeCB },
      NULL, &graph_grid_size_w },
    MMEnd
};


// Startup preferences
static Widget set_separate_windows_w;
static Widget set_attached_windows_w;
static MMDesc window_mode_menu [] = 
{
    { "attached",  MMToggle, { dddSetSeparateWindowsCB, XtPointer(1) },
      NULL, &set_attached_windows_w },
    { "separate",  MMToggle, { dddSetSeparateWindowsCB, XtPointer(0) },
      NULL, &set_separate_windows_w },
    MMEnd
};

static Widget set_button_images_w;
static Widget set_button_captions_w;
static Widget set_flat_buttons_w;
static Widget set_color_buttons_w;
static MMDesc button_appearance_menu [] = 
{
    { "images",   MMToggle, { dddToggleButtonImagesCB },
      NULL, &set_button_images_w },
    { "captions", MMToggle, { dddToggleButtonCaptionsCB },
      NULL, &set_button_captions_w },
    { "flat", MMToggle, { dddToggleFlatButtonsCB },
      NULL, &set_flat_buttons_w },
    { "color", MMToggle, { dddToggleColorButtonsCB },
      NULL, &set_color_buttons_w },
    MMEnd
};

static Widget set_focus_pointer_w;
static Widget set_focus_explicit_w;
static MMDesc keyboard_focus_menu [] = 
{
    { "pointer",  MMToggle, { dddSetKeyboardFocusPolicyCB, 
			    XtPointer(XmPOINTER) },
      NULL, &set_focus_pointer_w },
    { "explicit", MMToggle, { dddSetKeyboardFocusPolicyCB, 
			    XtPointer(XmEXPLICIT) },
      NULL, &set_focus_explicit_w },
    MMEnd
};

static Widget set_scrolling_panner_w;
static Widget set_scrolling_scrollbars_w;
static MMDesc data_scrolling_menu [] = 
{
    { "panner", MMToggle,     { dddSetPannerCB, XtPointer(True) },
      NULL, &set_scrolling_panner_w },
    { "scrollbars", MMToggle, { dddSetPannerCB, XtPointer(False) },
      NULL, &set_scrolling_scrollbars_w },
    MMEnd
};

static Widget set_debugger_gdb_w;
static Widget set_debugger_dbx_w;
static Widget set_debugger_xdb_w;
static Widget set_debugger_jdb_w;
static MMDesc debugger_menu [] = 
{
    { "gdb", MMToggle, { dddSetDebuggerCB, XtPointer(GDB) },
      NULL, &set_debugger_gdb_w },
    { "dbx", MMToggle, { dddSetDebuggerCB, XtPointer(DBX) },
      NULL, &set_debugger_dbx_w },
    { "xdb", MMToggle, { dddSetDebuggerCB, XtPointer(XDB) },
      NULL, &set_debugger_xdb_w },
    { "jdb", MMToggle, { dddSetDebuggerCB, XtPointer(JDB) },
      NULL, &set_debugger_jdb_w },
    MMEnd
};

static Widget startup_tips_w;
static Widget splash_screen_w;

static MMDesc show_startup_menu [] =
{
    { "splashScreen", MMToggle, { SetSplashScreenCB }, NULL, &splash_screen_w},
    { "startupTips",  MMToggle, { SetStartupTipsCB },  NULL, &startup_tips_w },
    MMEnd
};

static Widget kde_binding_w;
static Widget motif_binding_w;

static MMDesc binding_menu [] =
{
    { "kde",    MMToggle, { dddSetBindingStyleCB, XtPointer(KDEBindings) }, 
      NULL, &kde_binding_w },
    { "motif",  MMToggle, { dddSetBindingStyleCB, XtPointer(MotifBindings) },
      NULL, &motif_binding_w },
    MMEnd
};

static MMDesc startup_preferences_menu [] =
{
    { "windows",         MMRadioPanel,  MMNoCB, window_mode_menu },
    { "bindings",        MMRadioPanel,  MMNoCB, binding_menu },
    { "buttons",         MMButtonPanel, MMNoCB, button_appearance_menu },
    { "keyboardFocus",   MMRadioPanel,  MMNoCB, keyboard_focus_menu },
    { "dataScrolling",   MMRadioPanel,  MMNoCB, data_scrolling_menu },
    { "debugger",        MMRadioPanel,  MMNoCB, debugger_menu },
    { "show",            MMButtonPanel, MMNoCB, show_startup_menu },
    MMEnd
};


static Widget font_names[4];
static Widget font_sizes[4];

#define FONT_MENU(font) \
{ \
    { "name",   MMTextField | MMUnmanagedLabel, \
    { SetFontNameCB, XtPointer(font) }, NULL, \
    &font_names[int(font)] }, \
    { "size",   MMTextField, \
    { SetFontSizeCB, XtPointer(font) }, NULL, \
    &font_sizes[int(font)] }, \
    { "browse", MMPush, { BrowseFontCB, XtPointer(font) }}, \
    MMEnd \
}

static MMDesc default_font_menu        [] = FONT_MENU(DefaultDDDFont);
static MMDesc variable_width_font_menu [] = FONT_MENU(VariableWidthDDDFont);
static MMDesc fixed_width_font_menu    [] = FONT_MENU(FixedWidthDDDFont);

static MMDesc font_preferences_menu [] =
{
    { "default",         MMPanel,  MMNoCB, default_font_menu },
    { "variableWidth",   MMPanel,  MMNoCB, variable_width_font_menu },
    { "fixedWidth",      MMPanel,  MMNoCB, fixed_width_font_menu },
    MMEnd
};


static Widget edit_command_w;
static Widget get_core_command_w;
static Widget ps_command_w;
static Widget term_command_w;
static Widget uncompress_command_w;
static Widget www_command_w;

static MMDesc helpers_preferences_menu [] =
{
    { "edit",       MMTextField, { dddSetEditCommandCB }, 
      NULL, &edit_command_w},
    { "get_core",   MMTextField, { dddSetGetCoreCommandCB }, 
      NULL, &get_core_command_w},
    { "ps",         MMTextField, { dddSetPSCommandCB },
      NULL, &ps_command_w},
    { "term",       MMTextField, { dddSetTermCommandCB },
      NULL, &term_command_w},
    { "uncompress", MMTextField, { dddSetUncompressCommandCB },
      NULL, &uncompress_command_w},
    { "www",        MMTextField, { dddSetWWWCommandCB },
      NULL, &www_command_w},
    MMEnd
};


// Data
static Widget print_w            = 0;
static Widget display_w          = 0;
static Widget examine_w          = 0;
static Widget locals_w           = 0;
static Widget args_w             = 0;
static Widget detect_aliases_w   = 0;
static Widget infos_w            = 0;
static Widget align_w            = 0;
static Widget edit_watchpoints_w = 0;

static MMDesc data_menu[] = 
{
    { "displays",   MMPush,    { DataDisp::EditDisplaysCB }},
    { "watchpoints", MMPush,   { SourceView::EditBreakpointsCB }, 
                                 NULL, &edit_watchpoints_w },
    {"examine",       MMPush,  { gdbExamineCB }, NULL, &examine_w},
    MMSep,
    {"print",         MMPush,  { gdbPrintCB   }, NULL, &print_w },
    {"display",       MMPush,  { gdbDisplayCB }, NULL, &display_w},
    MMSep,
    { "detectAliases", MMToggle, { graphToggleDetectAliasesCB },
      NULL, &detect_aliases_w },
    MMSep,
    { "info locals", MMToggle,  { graphToggleLocalsCB }, NULL, &locals_w },
    { "info args",   MMToggle,  { graphToggleArgsCB }, NULL, &args_w },
    { "infos",       MMPush,    { WhenReady, XtPointer(dddPopupInfosCB) }, 
      NULL, &infos_w },
    MMSep,
    { "align",      MMPush,    { graphAlignCB  }, NULL, &align_w },
    { "rotate",     MMPush,    { graphRotateCB }},
    { "layout",     MMPush,    { graphLayoutCB }},
    MMSep,
    { "refresh",    MMPush,    { DataDisp::refreshCB }},
    MMEnd
};


// Help
static MMDesc help_menu[] = 
{
    {"onHelp",      MMPush, { HelpOnHelpCB }},
    MMSep,
    {"onItem",      MMPush, { HelpOnItemCB }},
    {"onWindow",    MMPush, { HelpOnWindowCB }},
    MMSep,
    {"whatNext",    MMPush, { WhatNextCB }},
    {"tipOfTheDay", MMPush, { TipOfTheDayCB }},
    MMSep,
    {"dddManual",   MMPush, { DDDManualCB }},
    {"news",        MMPush, { DDDNewsCB }},
    {"gdbManual",   MMPush, { GDBManualCB }},
    MMSep,
    {"license",     MMPush, { DDDLicenseCB }},
    {"www",         MMPush, { DDDWWWPageCB }},
    MMSep,
    {"onVersion",   MMPush, { HelpOnVersionCB }},
    MMEnd
};

// Menu Bar for DDD command window
static MMDesc command_menubar[] = 
{
    { "file",     MMMenu,          { gdbUpdateFileCB, command_file_menu }, 
                                   command_file_menu },
    { "edit",     MMMenu,          { gdbUpdateEditCB, XtPointer(GDBWindow) }, 
                                   command_edit_menu },
    { "view",    MMMenu,          { gdbUpdateViewCB, command_view_menu }, 
      				   command_view_menu },
    { "program",  MMMenu,          MMNoCB, command_program_menu },
    { "commands", MMMenu,          MMNoCB, command_menu },
    { "help",     MMMenu | MMHelp, MMNoCB, help_menu },
    MMEnd
};

// Menu Bar for DDD source view
static MMDesc source_menubar[] = 
{
    { "file",    MMMenu,          { gdbUpdateFileCB, source_file_menu }, 
                                  source_file_menu },
    { "edit",    MMMenu,          { gdbUpdateEditCB, XtPointer(SourceWindow) },
                                  source_edit_menu },
    { "view",    MMMenu,          { gdbUpdateViewCB, source_view_menu },
				  source_view_menu },
    { "program", MMMenu,          MMNoCB, source_program_menu },
    { "stack",   MMMenu,          MMNoCB, stack_menu },
    { "source",  MMMenu,          MMNoCB, source_menu },
    { "help",    MMMenu | MMHelp, MMNoCB, help_menu },
    MMEnd
};

// Menu Bar for DDD data window
static MMDesc data_menubar[] = 
{
    { "file",    MMMenu,          { gdbUpdateFileCB, data_file_menu }, 
                                  data_file_menu },
    { "edit",    MMMenu,          { gdbUpdateEditCB, XtPointer(DataWindow) },
                                  data_edit_menu },
    { "view",    MMMenu,          { gdbUpdateViewCB, data_view_menu }, 
      				  data_view_menu },
    { "program", MMMenu,          MMNoCB, data_program_menu },
    { "data",    MMMenu,          MMNoCB, data_menu },
    { "help",    MMMenu | MMHelp, MMNoCB, help_menu },
    MMEnd
};

// Menu Bar for common DDD data/command window
static MMDesc common_menubar[] = 
{
    { "file",       MMMenu,       { gdbUpdateFileCB, command_file_menu }, 
                                  command_file_menu },
    { "edit",       MMMenu,       { gdbUpdateEditCB, XtPointer(CommonWindow) },
                                  command_edit_menu },
    { "views",      MMMenu,       { gdbUpdateViewsCB, views_menu }, 
                                  views_menu },
    { "program",    MMMenu,       MMNoCB, command_program_menu },
    { "commands",   MMMenu,       MMNoCB, command_menu },
    { "stack",      MMMenu,       MMNoCB, stack_menu },
    { "source",     MMMenu,       MMNoCB, source_menu },
    { "data",       MMMenu,       MMNoCB, data_menu },
    { "help", MMMenu | MMHelp,    MMNoCB, help_menu },
    MMEnd
};

struct PrintItems {
    enum ArgCmd { PrintRef, Whatis };
};

static Widget print_examine_w = 0;

static MMDesc print_menu[] =
{
    { "printRef",        MMPush, { gdbPrintRefCB } },
    { "whatis",          MMPush, { gdbWhatisCB } },
    { "examine",         MMPush, { gdbExamineCB }, NULL, &print_examine_w },
    MMEnd
};

struct DispItems {
    enum ArgCmd { DispRef };
};

static MMDesc display_menu[] =
{
    { "dispRef",        MMPush, { gdbDispRefCB } },
    MMEnd
};

struct WatchItems {
    enum ArgCmd { Properties, Enable, Sep, CWatch, RWatch, AWatch };
};

static MMDesc watch_menu[] =
{
    { "watchProperties",  MMPush, { gdbEditWatchpointPropertiesCB }},
    { "enableWatch",      MMPush, { gdbToggleEnableWatchpointCB }},
    MMSep,
    { "cwatch",       MMPush, { gdbWatchCB, XtPointer(WATCH_CHANGE) } },
    { "rwatch",       MMPush, { gdbWatchCB, XtPointer(WATCH_READ)   } },
    { "awatch",       MMPush, { gdbWatchCB, XtPointer(WATCH_ACCESS) } },

    // It would be nice to have an `unwatch' command here. for
    // recording commands.  Unfortunately, GDB needs a watchpoint
    // number for deleting watchpoints.
#if 0	
    MMSep,
    { "unwatch",      MMPush, { gdbUnwatchCB }},
#endif
    MMEnd
};

struct BreakItems {
    enum ArgCmd { TempBreak, RegexBreak, Properties, Enable, Sep1,
		  ContUntil, SetPC, Sep2, ClearAt2 };
};

static MMDesc break_menu[] = 
{
    { "tempBreakAt",     MMPush, { gdbTempBreakAtCB }},
    { "regexBreakAt",    MMPush, { gdbRegexBreakAtCB }},
    { "breakProperties", MMPush, { gdbEditBreakpointPropertiesCB }},
    { "enableBreak",     MMPush, { gdbToggleEnableBreakpointCB }},
    MMSep,
    { "contUntil",       MMPush, { gdbContUntilCB }},
    { "setPC",           MMPush, { gdbSetPCCB }},
    MMSep,
    { "clearAt2",        MMPush, { gdbClearAtCB }},
    MMEnd
};

struct FindItems {
    enum ArgCmd { FindForward, FindBackward };
};

static MMDesc find_menu[] = 
{
    { "findForward",  MMPush, { gdbFindCB, XtPointer(SourceView::forward) }},
    { "findBackward", MMPush, { gdbFindCB, XtPointer(SourceView::backward) }},
    MMEnd
};

struct ArgItems {
    enum ArgCmd { Lookup, Find, Break, Watch, Print, Display };
};

static MMDesc arg_cmd_area[] = 
{
    {"lookup",        MMPush,  { gdbLookupCB       }},
    {"find",          MMPush | MMInsensitive, { gdbFindAgainCB }, find_menu },
    {"breakAt",       MMPush,  { gdbToggleBreakCB  }, break_menu   },
    {"watch",         MMPush,  { gdbToggleWatchCB, XtPointer(WATCH_CHANGE) }, 
                                 watch_menu },
    {"print",         MMPush,  { gdbPrintCB        }, print_menu   },
    {"display",       MMPush,  { gdbDisplayCB      }, display_menu },
    MMEnd
};


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// All communication with GDB passes through this variable
GDBAgent*     gdb = 0;

// Application resources
AppData       app_data;

// Data display
DataDisp*     data_disp;

// Source display
SourceView*   source_view;

// Argument field
ArgField*     source_arg;

// Argument toolbar
static Widget arg_cmd_w;

// GDB input/output widget
Widget gdb_w;

// GDB status line
Widget status_w;

// GDB activity led
static Widget led_w;

// Last output position
XmTextPosition promptPosition;

// Last message position
XmTextPosition messagePosition;

// Buttons
static Widget console_buttons_w;
static Widget source_buttons_w;
static Widget data_buttons_w;
static Widget command_toolbar_w;

// Strings to be ignored in GDB output
string gdb_out_ignore = "";

// Command management
bool private_gdb_output;   // true if output is running
bool private_gdb_input;    // true if input is running

// true, if initial gdb prompt appeared
bool gdb_initialized;

// The Xt Warning handler
static XtErrorHandler ddd_original_xt_warning_handler;

// Initial delays
static StatusMsg *init_delay = 0;
static Delay *setup_delay = 0;

// Events to note for window visibility
const int STRUCTURE_MASK = StructureNotifyMask | VisibilityChangeMask;

// The atom for the delete-window protocol
static Atom WM_DELETE_WINDOW;

// Logging stuff
static ostrstream devnull;
ostream *_dddlog = &devnull;

// Message handling
static MString version_warnings;


//-----------------------------------------------------------------------------
// DDD main program
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    // As this is a C++ program, execution does not begin here.  At
    // this point, all global data objects already have been properly
    // initialized.

    // Save environment for restart.
    register_argv(argv);
    register_environ();

    // This one is required for error messages
    char *program_name = (char *)(argc > 0 ? argv[0] : ddd_NAME);

    // Install signal handlers

    // On some systems (notably HP-UX), GDB has trouble finding what
    // function invoked the signal handler.  Hence, if the environment
    // variable DDD_NO_SIGNAL_HANDLERS is set, we do not install
    // signal handlers, causing DDD to report signals immediately.

    if (getenv(DDD_NAME "_NO_SIGNAL_HANDLERS") == 0)
    {
	ddd_install_signal();	         // Cleanup upon termination
	ddd_install_fatal(program_name); // Fatal error
    }

    // Check if we are to run without windows
    bool no_windows = false;

    // Don't run DDD setuid.  DDD invokes shell commands and even
    // shell scripts, such that all known problems of setuid shell
    // scripts apply.
    if (geteuid() != getuid())
	no_windows = true;

    // Check for special options:
    // `--nw'   - no windows (GDB)
    // `-L'     - no windows (XDB)
    // `--PLAY' - logplayer mode (DDD)
    // and options that would otherwise be eaten by Xt
    StringArray saved_options;
    string gdb_name = "gdb";
    setup_options(argc, argv, saved_options, gdb_name, no_windows);

    // If we don't want windows, just start GDB.
    if (no_windows)
    {
	argv[0] = gdb_name;
	execvp(gdb_name, argv);
	perror(gdb_name);
	return EXIT_FAILURE;
    }

    // Initialize X toolkit
    Arg args[10];
    int arg = 0;

    // Install X error handlers
    ddd_install_x_fatal();
    ddd_install_x_error();

    ostrstream messages;

    // Set up a `~/.ddd/' directory hierarchy
    create_session_dir(DEFAULT_SESSION, messages);

    // Read ~/.ddd/init resources
    XrmDatabase dddinit = 
	GetFileDatabase(session_state_file(DEFAULT_SESSION));
    if (dddinit == 0)
	dddinit = XrmGetStringDatabase("");

    // Read ~/.ddd/tips resources
    XrmDatabase dddtips =
	GetFileDatabase(session_tips_file());
    if (dddtips != 0)
	XrmMergeDatabases(dddtips, &dddinit);

    // Let command-line arguments override ~/.ddd/init
    XrmParseCommand(&dddinit, options, XtNumber(options), 
		    DDD_CLASS_NAME, &argc, argv);

    String session_id = 0;
    if (session_id == 0)
    {
	// Determine session
	char *session_rtype = 0;
	XrmValue session_value;

	string Nsession   = string(DDD_CLASS_NAME ".") + XtNsession;
	string CSessionID = string(DDD_CLASS_NAME ".") + XtCSessionID;
#if XtSpecificationRelease >= 6
	string NsessionID = string(DDD_CLASS_NAME ".") + XtNsessionID;
#endif

	// Try resource or option
	if (
#if XtSpecificationRelease >= 6
	    XrmGetResource(dddinit, NsessionID, CSessionID,
			   &session_rtype, &session_value) ||
#endif
	    XrmGetResource(dddinit, Nsession, CSessionID,
			   &session_rtype, &session_value))
	{
	    if (session_value.addr != 0)
	    {
		static string id((char *)session_value.addr, 
				 session_value.size);
		session_id = id;
	    }
	}

	if (session_id == 0)
	{
	    // Try `=FILE' hack: if the last or second-to-last arg is
	    // `=FILE', replace it by FILE and use FILE as session id.
	    for (int i = argc - 1; i >= 1 && i >= argc - 2; i--)
	    {
		if (argv[i][0] == '=')
		{
		    // Delete '='
		    int j = 0;
		    while ((argv[i][j] = argv[i][j + 1]))
			j++;
			
		    session_id = (char *)SourceView::basename(argv[i]);
		    session_value.addr = session_id;
		    session_value.size = strlen(session_id) + 1;
		    XrmPutResource(&dddinit, Nsession, XtRString, 
				   &session_value);
		}
	    }
	}
    }

    XrmDatabase session_db = 0;
    if (restart_session() != "")
    {
	// A session is given in $DDD_SESSION: override everything.
	session_db = GetFileDatabase(session_state_file(restart_session()));
	if (session_db != 0)
	    XrmMergeDatabases(session_db, &dddinit);
    }
    else if (session_id != 0)
    {
	// Merge in session resources; these override `~/.ddd/init' as
	// well as the command-line options.
	session_db = GetFileDatabase(session_state_file(session_id));
	if (session_db != 0)
	    XrmMergeDatabases(session_db, &dddinit);
    }

    // Register own converters.  This must be done here to install the
    // String -> Cardinal converter.
    registerOwnConverters();

    // Open X connection and create top-level application shell
    XtAppContext app_context;
    arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;

#if XtSpecificationRelease >= 6
    if (session_id)
    {
	XtSetArg(args[arg], XtNsessionID, session_id); arg++;
    }

    Widget toplevel =
	XtOpenApplication(&app_context, DDD_CLASS_NAME,
			  XrmOptionDescList(0), 0,
			  &argc, argv, ddd_fallback_resources,
			  sessionShellWidgetClass,
			  args, arg);

    XtAddCallback(toplevel, XtNsaveCallback, SaveSmSessionCB, XtPointer(0));
    XtAddCallback(toplevel, XtNdieCallback, ShutdownSmSessionCB, XtPointer(0));
#else
    Widget toplevel = 
	XtAppInitialize(&app_context, DDD_CLASS_NAME,
			XrmOptionDescList(0), 0, 
			&argc, argv, ddd_fallback_resources, args, arg);
#endif
    ddd_install_xt_error(app_context);

    // Merge ~/.ddd/init resources into application shell
    XrmDatabase target = XtDatabase(XtDisplay(toplevel));
    XrmMergeDatabases(dddinit, &target);

    // Setup top-level actions; this must be done before reading
    // application defaults.
    XtAppAddActions(app_context, actions, XtNumber(actions));

    // Register string -> OnOff converter; this must be done before
    // reading application defaults.
    XtSetTypeConverter(XmRString, XtROnOff, CvtStringToOnOff,
		       NULL, 0, XtCacheAll, 
		       XtDestructor(NULL));

    // Get application resources into APP_DATA
    XtVaGetApplicationResources(toplevel, (XtPointer)&app_data,
				ddd_resources, ddd_resources_size,
				NULL);


#if XtSpecificationRelease >= 6
    // Synchronize SESSION_ID and APP_DATA.session
    session_id = 0;
    XtVaGetValues(toplevel, XtNsessionID, &session_id, NULL);
    if (session_id != 0)
	app_data.session = session_id;
#endif

    if (app_data.session == 0)
	app_data.session = DEFAULT_SESSION;
    if (restart_session() != "")
    {
	static string s;
	s = restart_session();
	app_data.session = s;
    }
    session_id = app_data.session;

    // From this point on, APP_DATA is valid.

#ifdef SIGCHLD
    if (app_data.poll_child_status)
    {
	// In POLL_CHILD_STATUS mode, we ignore SIGCHLD signals and
	// poll the child process state explicitly, as in DDD 3.0 and
	// earlier.  Advantage: DDD does not depend on correct
	// asynchronous signal handling.  Disadvantage: somewhat
	// slower due to explicit polling.
	signal(SIGCHLD, SignalProc(SIG_DFL));
    }
#endif

    // Setup label hack
    arg = 0;
    XtCreateWidget("label_hack", xmLabelHackWidgetClass, toplevel, args, arg);

    // Set key bindings
    setup_cut_copy_paste_bindings(XtDatabase(XtDisplay(toplevel)));

    // Define font macros
    setup_fonts(app_data, XtDatabase(XtDisplay(toplevel)));

    // Create new session dir if needed
    create_session_dir(app_data.session, messages);

    // Create a new auto_command_prefix if needed
    setup_auto_command_prefix();

    // Forward messages found so far into cerr
    {
	string msg(messages);
	cerr << msg;
    }

    // Set up VSL resources
    if (VSEFlags::parse_vsl(argc, argv))
    {
	// Show VSL usage...
	cout << VSEFlags::explain(true);
	return EXIT_FAILURE;
    }

    // Check the X configuration
    if (app_data.check_configuration)
    {
	if (check_x_configuration(toplevel, true))
	    return EXIT_FAILURE;
	else
	    return EXIT_SUCCESS;
    }

    // If needed, fix the X configuration silently
    check_x_configuration(toplevel, false);

    // Set up warning handler
    ddd_original_xt_warning_handler =
	XtAppSetWarningHandler(app_context, ddd_xt_warning);

    // Set up debugger defaults
    if (app_data.debugger[0] == '\0')
    {
	if (app_data.debugger_command[0] == '\0')
	    app_data.debugger_command = gdb_name;
	app_data.debugger = app_data.debugger_command;
    }
    DebuggerType type = debugger_type(app_data.debugger);
    if (app_data.debugger_command[0] == '\0')
	app_data.debugger_command = app_data.debugger;

    // Set host specification
    if (app_data.debugger_rhost && app_data.debugger_rhost[0] != '\0')
	gdb_host = app_data.debugger_rhost;
    else if (app_data.debugger_host && app_data.debugger_host[0] != '\0')
	gdb_host = app_data.debugger_host;
    else
	gdb_host = "";

    if (gdb_host.contains('@') && string(app_data.debugger_host_login) == "")
    {
	static string new_login = gdb_host.before('@');
	app_data.debugger_host_login = new_login;
	gdb_host = gdb_host.after('@');
    }

    if (gdb_host == "" && string(app_data.debugger_host_login) != "")
    {
	cerr << argv[0] << ": --login requires --rhost or --host\n";
	return EXIT_FAILURE;
    }

    // Check for `--play-log'
    if (app_data.play_log != NULL)
    {
	// Invoke self with `--PLAY LOGFILE' as inferior
	app_data.debugger_command = argv[0];

	if (!remote_gdb())
	{
	    // Override debugger type from log
	    check_log(app_data.play_log, type);
	}

	// Don't overwrite existing log files
	app_data.trace = true;
    }


    // Check for `--version', `--help', `--news', etc.
    if (app_data.show_version)
	show_version(cout);

    if (app_data.show_invocation)
	show_invocation(type, cout);

    if (app_data.show_configuration)
	show_configuration(cout);

    if (app_data.show_news)
	show(ddd_news);

    if (app_data.show_license)
	show(ddd_license);

    if (app_data.show_manual)
	show(ddd_man);

    if (app_data.show_version 
	|| app_data.show_invocation 
	|| app_data.show_configuration
	|| app_data.show_news
	|| app_data.show_license
	|| app_data.show_manual
	|| app_data.show_fonts)
	return EXIT_SUCCESS;

    // From this point on, we'll be running under X.

    // Create a `~/.ddd/log' file for this session; 
    // log invocation and configuration
    if (app_data.trace)
    {
	_dddlog = &clog;
    }
    else
    {
	static ofstream log(session_log_file());
	_dddlog = &log;
    }
    show_configuration(dddlog);
    dddlog << "$ ";
    int i;
    for (i = 0; saved_argv()[i] != 0; i++)
	dddlog << " " << cook(saved_argv()[i]);
    dddlog << '\n';
    dddlog.flush();

    // Warn for incompatible `Ddd' and `~/.ddd/init' files
    setup_version_warnings();

    // Global variables: Set LessTif version
    lesstif_version = app_data.lesstif_version;

    // Global variables: Set maximum lengths
    max_value_tip_length              = app_data.max_value_tip_length;
    max_value_doc_length              = app_data.max_value_doc_length;
    DispBox::max_display_title_length = app_data.max_display_title_length;
    SourceView::max_popup_expr_length = app_data.max_popup_expr_length;

    // Global variables: Setup data display
    DataDisp::bump_displays           = app_data.bump_displays;
    DispValue::expand_repeated_values = app_data.expand_repeated_values;
    DispGraph::hide_inactive_displays = app_data.hide_inactive_displays;

    // Global variables: Set delays for button and value tips
    help_button_tip_delay = app_data.button_tip_delay;
    help_value_tip_delay  = app_data.value_tip_delay;
    help_button_doc_delay = app_data.button_doc_delay;
    help_value_doc_delay  = app_data.value_doc_delay;
    help_clear_doc_delay  = app_data.clear_doc_delay;
    help_clear_tip_delay  = app_data.clear_tip_delay;

#if XmVersion >= 2000
    // Setup drag and drop callback
    Widget display_w = verify(XmGetXmDisplay(XtDisplay(toplevel)));
    if (XmIsDisplay(display_w))
    {
	XtAddCallback(display_w, XmNdragStartCallback,
		      CheckDragCB, NULL);
    }
#else
    (void) CheckDragCB;		// Use it
#endif

    // Re-register own converters.  Motif may have overridden some of
    // these, so register them again.
    registerOwnConverters();

    // Show splash screen
    Boolean iconic;
    XtVaGetValues(toplevel, XmNiconic, &iconic, NULL);
    if (app_data.splash_screen && !iconic && restart_session() == "")
	popup_splash_screen(toplevel, app_data.splash_screen_color_key);

    // Re-register own converters.  Motif has overridden some of
    // these, so register them again.
    registerOwnConverters();

    // Install special Motif converters
#if XmVersion >= 1002
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Lock `~/.ddd/'.
    lock_ddd(toplevel);

    // Put saved options back again
    for (i = argc + saved_options.size() - 1; i > saved_options.size(); i--)
	argv[i] = argv[i - saved_options.size()];
    for (i = saved_options.size() - 1; i >= 0; i--)
	argv[i + 1] = saved_options[i];
    argc += saved_options.size();
    argv[argc] = 0;

    // Create GDB interface
    gdb = new_gdb(type, app_data, app_context, argc, argv);
    defineConversionMacro("GDB", gdb->title());

    // Set up GDB handlers
    gdb->addHandler(ReadyForQuestion, gdb_readyHP);
    gdb->addHandler(InputEOF,         gdb_eofHP);
    gdb->addHandler(ErrorEOF,         gdb_eofHP);
    gdb->addHandler(Started,          ReportStartHP);
    gdb->addHandler(Died,             gdb_diedHP);
    gdb->addHandler(Died,             DisableBlinkHP);
    gdb->addHandler(Died,             ReportDeathHP);
    gdb->addHandler(LanguageChanged,  DataDisp::language_changedHP);
    gdb->addHandler(LanguageChanged,  language_changedHP);
    gdb->addHandler(LanguageChanged,  report_languageHP);
    gdb->addHandler(ReplyRequired,    gdb_selectHP);
    gdb->addHandler(Panic,            gdb_panicHP);
    gdb->addHandler(Strange,          gdb_strangeHP);
    gdb->addHandler(Recording,        gdb_recordingHP);
    gdb->addHandler(EchoDetected,     gdb_echo_detectedHP);

    // Set default history file (never read, only overwritten)
    set_gdb_history_file(gdb->history_file());

    // Setup shell creation
    Delay::shell_registered = setup_new_shell;

    // Create command shell

    // Use original arguments
    char **original_argv = saved_argv();
    int original_argc = 0;
    while (original_argv[original_argc] != 0)
	original_argc++;

    arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    XtSetArg(args[arg], XmNargc,           original_argc); arg++;
    XtSetArg(args[arg], XmNargv,           original_argv); arg++;

    if (!app_data.separate_source_window && !app_data.separate_data_window)
    {
	// One single window - use command shell as top-level shell
	command_shell = 
	    verify(XtAppCreateShell(NULL, DDD_CLASS_NAME,
				    applicationShellWidgetClass,
				    XtDisplay(toplevel), args, arg));

	// From now on, use the command shell as parent
	toplevel = command_shell;
    }
    else
    {
	// Separate windows - make command shell a popup shell.  
	// The toplevel window is never realized.
	command_shell =
	    verify(XtCreatePopupShell("command_shell",
				      applicationShellWidgetClass,
				      toplevel, args, arg));
    }
    WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(command_shell, WM_DELETE_WINDOW, DDDCloseCB, 0);

    // From this point on, we have a true top-level window.

    // Create main window
    arg = 0;
    Widget main_window = XmCreateMainWindow(command_shell, "main_window",
					    args, arg);
    XtManageChild(main_window);

    // Re-register own converters to override Motif converters.
    registerOwnConverters();

    // Create menu bar
    MMDesc *menubar = common_menubar;
    if (app_data.separate_data_window && app_data.separate_source_window)
	menubar = command_menubar;

    Widget menubar_w = MMcreateMenuBar(main_window, "menubar", menubar);
    MMaddCallbacks(menubar);
    MMaddHelpCallback(menubar, ImmediateHelpCB);
    verify_buttons(menubar);
    register_menu_shell(menubar);

    // Create Paned Window
    arg = 0;
    XtSetArg(args[arg], XmNborderWidth,     0); arg++;
    XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
    Widget paned_work_w = verify(XmCreatePanedWindow(main_window,
						     "paned_work_w",
						     args, arg));
    XtManageChild(paned_work_w);

    // Status line
    if (!app_data.separate_source_window && !app_data.status_at_bottom)
	create_status(paned_work_w);

    // Toolbar label type
    unsigned char label_type = XmSTRING;
    if (app_data.button_captions || app_data.button_images)
	label_type = XmPIXMAP;

    // Common toolbar
    if (!app_data.button_captions && !app_data.button_images)
	app_data.common_toolbar = false;

    Widget arg_label = 0;
    if (!app_data.separate_source_window &&
	!app_data.separate_data_window &&
	app_data.common_toolbar)
    {
	arg_cmd_area[ArgItems::Display].type |= MMUnmanaged;
	arg_cmd_w = create_toolbar(paned_work_w, "common",
				   arg_cmd_area, DataDisp::graph_cmd_area,
				   arg_label, source_arg, XmPIXMAP);

	DataDisp::graph_cmd_w = arg_cmd_w;

	if (command_toolbar_w == 0)
	{
	    command_toolbar_w = make_buttons(paned_work_w, 
					     "command_toolbar", 
					     app_data.tool_buttons);
	}
    }
    
    // Install icons if not already done
    install_icons(command_shell, 
		  app_data.button_color_key,
		  app_data.active_button_color_key);

    // Data window
    Widget data_disp_parent = paned_work_w;
    Widget data_menubar_w = 0;
    Widget data_main_window_w = 0;
    if (app_data.separate_data_window)
    {
	arg = 0;
	XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
	data_disp_shell =
	    verify(XtCreatePopupShell("data_disp_shell",
				      topLevelShellWidgetClass,
				      toplevel, args, arg));
	XmAddWMProtocolCallback(data_disp_shell, WM_DELETE_WINDOW, 
				DDDCloseCB, 0);

	arg = 0;
	data_main_window_w = XmCreateMainWindow(data_disp_shell, 
						"data_main_window",
						args, arg);
	XtManageChild(data_main_window_w);

	// Add menu bar
	data_menubar_w = 
	    MMcreateMenuBar (data_main_window_w, "menubar", data_menubar);
	MMaddCallbacks(data_menubar);
	MMaddHelpCallback(menubar, ImmediateHelpCB);
	verify_buttons(data_menubar);
	register_menu_shell(data_menubar);

	arg = 0;
	XtSetArg(args[arg], XmNborderWidth,     0); arg++;
	XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
	XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
	data_disp_parent = verify(XmCreatePanedWindow(data_main_window_w,
						      "data_paned_work_w",
						      args, arg));
	XtManageChild(data_disp_parent);
    }

    // Create data display
    data_disp = new DataDisp(data_disp_parent, data_buttons_w);

    if (app_data.separate_data_window)
    {
	// More values for main window
	XtVaSetValues (data_main_window_w,
		       XmNmenuBar,    data_menubar_w,
		       XmNworkWindow, data_disp_parent,
		       NULL);
    }

    if (data_buttons_w == 0)
	data_buttons_w = make_buttons(data_disp_parent, "data_buttons", 
				      app_data.data_buttons);

    // Source window
    Widget source_view_parent = paned_work_w;
    Widget source_menubar_w = 0;
    Widget source_main_window_w = 0;
    if (app_data.separate_source_window)
    {
	arg = 0;
	XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
	source_view_shell = 
	    verify(XtCreatePopupShell("source_view_shell",
				      topLevelShellWidgetClass,
				      toplevel, args, arg));
	XmAddWMProtocolCallback(source_view_shell, WM_DELETE_WINDOW, 
				DDDCloseCB, 0);

	arg = 0;
	source_main_window_w = XmCreateMainWindow(source_view_shell,
						  "source_main_window",
						  args, arg);
	XtManageChild(source_main_window_w);

	// Add menu bar
	source_menubar_w = 
	    MMcreateMenuBar (source_main_window_w, "menubar", source_menubar);
	MMaddCallbacks(source_menubar);
	MMaddHelpCallback(menubar, ImmediateHelpCB);
	verify_buttons(source_menubar);
	register_menu_shell(source_menubar);

	// Add source window
	arg = 0;
	XtSetArg(args[arg], XmNborderWidth,     0); arg++;
	XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
	XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
	source_view_parent = 
	    verify(XmCreatePanedWindow(source_main_window_w, 
				       "source_paned_work_w", args, arg));
	XtManageChild(source_view_parent);

	// Status line
	if (!app_data.status_at_bottom)
	    create_status(source_view_parent);
    }

    // Add toolbar
    if (arg_cmd_w == 0 && !app_data.toolbars_at_bottom)
    {
	arg_cmd_w = create_toolbar(source_view_parent, "source",
				   arg_cmd_area, 0, arg_label, source_arg,
				   label_type);
    }

    if (command_toolbar_w == 0 && !app_data.toolbars_at_bottom)
    {
	command_toolbar_w = make_buttons(source_view_parent, 
					 "command_toolbar", 
					 app_data.tool_buttons);
    }

    if (source_buttons_w == 0 && !app_data.toolbars_at_bottom)
    {
	source_buttons_w = make_buttons(source_view_parent, 
					"source_buttons", 
					app_data.source_buttons);
    }

    source_view = new SourceView(source_view_parent);
    source_view->set_max_glyphs(app_data.max_glyphs);
    source_view->cache_glyph_images = app_data.cache_glyph_images;

    if (app_data.separate_source_window)
    {
	// More values for main window
	XtVaSetValues (source_main_window_w,
		       XmNmenuBar,    source_menubar_w,
		       XmNworkWindow, source_view_parent,
		       NULL);
    }

    // Source toolbar
    if (arg_cmd_w == 0)
	arg_cmd_w = create_toolbar(source_view_parent, "source",
				   arg_cmd_area, 0, arg_label, source_arg,
				   label_type);
    XtAddCallback(arg_label, XmNactivateCallback, 
		  ClearTextFieldCB, source_arg->text());

    if (DataDisp::graph_cmd_w == arg_cmd_w)
    {
	// Common toolbar
	XtAddCallback(arg_label, XmNactivateCallback, 
		      DataDisp::SelectionLostCB, XtPointer(0));
    }

    XtAddCallback(source_arg->text(), XmNactivateCallback, 
		  ActivateCB, 
		  XtPointer(arg_cmd_area[ArgItems::Lookup].widget));
    sync_args(source_arg, data_disp->graph_arg);

    if (data_disp->graph_arg != 0)
	XtAddCallback(data_disp->graph_arg->text(), XmNactivateCallback, 
		      ActivateCB, 
		      XtPointer(data_disp->graph_cmd_area[0].widget));

    if (command_toolbar_w == 0)
    {
	command_toolbar_w = make_buttons(source_view_parent, 
					 "command_toolbar", 
					 app_data.tool_buttons);
    }
    if (command_toolbar_w != 0)
	XtUnmanageChild(command_toolbar_w);

    if (source_buttons_w == 0)
	source_buttons_w = make_buttons(source_view_parent, "source_buttons", 
					app_data.source_buttons);

    // Status line
    if (app_data.separate_source_window && app_data.status_at_bottom)
	create_status(source_view_parent);

    // Debugger console
    if (console_buttons_w == 0 && !app_data.toolbars_at_bottom)
	console_buttons_w = make_buttons(paned_work_w, "console_buttons", 
					 app_data.console_buttons);

    arg = 0;
    gdb_w = verify(XmCreateScrolledText(paned_work_w, "gdb_w", args, arg));

    XtAddCallback (gdb_w,
		   XmNmodifyVerifyCallback,
		   gdbModifyCB,
		   NULL);
    XtAddCallback (gdb_w,
		   XmNmotionVerifyCallback,
		   gdbMotionCB,
		   NULL);
    XtAddCallback (gdb_w,
		   XmNvalueChangedCallback,
		   gdbChangeCB,
		   NULL);
    XtManageChild (gdb_w);

    if (!app_data.separate_source_window || !app_data.separate_data_window)
    {
	// Don't resize the debugger console when resizing the main
	// window - resize source and/or data windows instead
	XtVaSetValues(XtParent(gdb_w), XmNskipAdjust, True, NULL);
    }

    // Set up the scrolled window
    XtVaSetValues(XtParent(gdb_w),
		  XmNspacing,         0,
		  XmNborderWidth,     0,
		  XmNshadowThickness, 0,
		  NULL);

    // Give the ScrolledWindow the size specified for the debugger console
    set_scrolled_window_size(gdb_w);

#if 0
    // Don't edit the text until the first prompt appears.
    XmTextSetEditable(gdb_w, false);
#endif

    if (console_buttons_w == 0)
	console_buttons_w = make_buttons(paned_work_w, "console_buttons", 
					 app_data.console_buttons);

    // Status line
    if (app_data.status_at_bottom && !app_data.separate_source_window)
	create_status(source_view_parent);

    // Paned Window is done
    XtManageChild (paned_work_w);

    // More values for main window
    XtVaSetValues (main_window,
		   XmNmenuBar,    menubar_w,
		   XmNworkWindow, paned_work_w,
		   NULL);

    // All main widgets (except shells) are created at this point.
    
    // Load history for current session
    load_history(session_history_file(app_data.session));

    // Tie arg fields to history
    tie_combo_box_to_history(source_arg->text(), arg_history_filter);
    if (data_disp->graph_arg != 0)
	tie_combo_box_to_history(data_disp->graph_arg->text(), 
				 arg_history_filter);

    // Tie `recent files' to history
    tie_menu_to_recent_files(command_recent_menu);
    tie_menu_to_recent_files(source_recent_menu);
    tie_menu_to_recent_files(data_recent_menu);

    // Setup environment.
    setup_environment();

    // Setup handlers
    source_arg->addHandler (Changed, source_argHP);
    source_arg->callHandlers();

    // Setup insertion position
    promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, messagePosition);

    // Setup help hooks
    PreHelpOnContextHook = PreHelpOnContext;
    PostHelpOnItemHook   = PostHelpOnItem;

    // Setup version info
    setup_version_info();

    // Customize `settings' title.
    set_settings_title(command_edit_menu[EditItems::Settings].widget);
    set_settings_title(source_edit_menu[EditItems::Settings].widget);
    set_settings_title(data_edit_menu[EditItems::Settings].widget);

    // Close windows explicitly requested
    if (!app_data.data_window && 
	(!app_data.full_name_mode || !app_data.tty_mode))
    {
	// We don't want the data window (unless in full name mode,
	// where we always open a data window - because otherwise, no
	// window would remain and we'd be gone).
	gdbCloseDataWindowCB(gdb_w, 0, 0);
    }

    if (!app_data.source_window || app_data.full_name_mode)
    {
	// We don't need the source window, since we're invoked by Emacs.
	gdbCloseSourceWindowCB(gdb_w, 0, 0);

	if (!app_data.disassemble)
	    gdbCloseToolWindowCB(gdb_w, 0, 0);
    }

    if (!app_data.disassemble)
    {
	// We don't disassemble.
	gdbCloseCodeWindowCB(gdb_w, 0, 0);
    }

    if ((!app_data.debugger_console || app_data.tty_mode) &&
	(!app_data.separate_source_window || !app_data.separate_data_window))
    {
	// We don't need the debugger console, since we have a TTY.
	gdbCloseCommandWindowCB(gdb_w, 0, 0);
    }

    if (data_disp_shell != 0)
    {
	// Set width of the separate data window
	Dimension max_width = 0;
	get_paned_window_width(paned_work_w, max_width);
	get_paned_window_width(source_view_parent, max_width);
	set_paned_window_size(data_disp_parent, max_width);
	set_main_window_size(data_main_window_w);
    }

    // Popdown the splash screen before it eats up all colors
    popdown_splash_screen();

    // Realize all top-level widgets
    XtRealizeWidget(command_shell);
    Delay::register_shell(command_shell);

    if (data_disp_shell)
    {
	XtRealizeWidget(data_disp_shell);
	Delay::register_shell(data_disp_shell);
    }

    if (source_view_shell)
    {
	XtRealizeWidget(source_view_shell);
	Delay::register_shell(source_view_shell);
    }

    // Create initial delay
    if (app_data.session == DEFAULT_SESSION)
    {
	setup_delay = new Delay;
	init_delay  = new StatusMsg("Starting " + gdb->title());
	unlock_status();	// We still want to see the messages
    }
    else
    {
	init_delay = new StatusDelay("Opening session " 
				     + quote(app_data.session));
    }

    if (app_data.decorate_tool == Auto)
    {
	// Check for decorated transient windows.  We can do this only
	// after the command shell has been realized, because
	// otherwise the init shell might always be decorated.
	start_have_decorated_transients(command_shell);
    }

    // Remove unnecessary sashes
    untraverse_sashes(source_view_parent);

    // The sash in the source view is kept, as it separates source and
    // assembler code windows.
#if 0
    if (source_view_shell)
       unmanage_sashes(source_view_parent);
#endif

    untraverse_sashes(data_disp_parent);
    if (data_disp_shell)
	unmanage_sashes(data_disp_parent);

    untraverse_sashes(paned_work_w);
    if (source_view_shell && data_disp_shell)
	unmanage_sashes(paned_work_w);

    // Create subshells.  We do this after the main window has been
    // realized, since LessTif won't make the shells transient otherwise.
    source_view->create_shells();
    data_disp->create_shells();
    set_shortcut_menu(data_disp, app_data.display_shortcuts);

    // Create preference panels
    make_preferences(paned_work_w);

    // Save option states
    save_option_state();

    // Setup option states
    setup_options();
    update_options();

    // Setup info buttons

    // Our info routines require that the widgets be named after the
    // info command.  Ugly hack.
    strcpy(XtName(locals_w), gdb->info_locals_command());
    strcpy(XtName(args_w),   gdb->info_args_command());

    if (gdb->info_locals_command() != gdb->info_args_command())
    {
	register_info_button(locals_w);
	register_info_button(args_w);
    }
    else
    {
	register_info_button(locals_w);
	XtUnmanageChild(args_w);
    }
    update_infos();

    // Startup shells
    XtVaGetValues(toplevel, XmNiconic, &iconic, NULL);
    if (iconic)
    {
	// Startup command shell iconified; others follow as needed
	initial_popup_shell(command_shell);
    }
    else if ((!app_data.debugger_console || app_data.tty_mode) && 
	     app_data.separate_source_window &&
	     app_data.separate_data_window)
    {
	// Debugger console is closed: wait for source to pop up
    }
    else if (!app_data.tty_mode)
    {
	// Popup the command shell only; other shells follow as needed
	initial_popup_shell(command_shell);
    }
    else
    {
	// TTY mode: all shells follow as needed.
    }

    // Trace positions and visibility of all DDD windows
    if (command_shell)
	XtAddEventHandler(command_shell, STRUCTURE_MASK, False,
			  StructureNotifyEH, XtPointer(0));
    if (source_view_shell)
	XtAddEventHandler(source_view_shell, STRUCTURE_MASK, False,
			  StructureNotifyEH, XtPointer(0));
    if (data_disp_shell)
	XtAddEventHandler(data_disp_shell, STRUCTURE_MASK, False,
			  StructureNotifyEH, XtPointer(0));

#if 0
    // Wait for the command shell to be mapped, such that we don't
    // lose debugger output.  This also decreases system load on
    // single-processor machines since DDD is idle when the debugger
    // starts.
    wait_until_mapped(command_shell);
#endif

    // Create command tool
    if (app_data.tool_buttons && strlen(app_data.tool_buttons) > 0)
	setup_command_tool();

    // Make sure we see all messages accumulated so far
    {
	string msg(messages);
	while (msg != "")
	{
	    string line = msg.before('\n');
	    set_status(line);
	    msg = msg.after('\n');
	}
    }

    // Setup TTY interface
    setup_tty();

    // Raise core limit if needed (this must be done before starting GDB)
    setup_core_limit();

    // Start debugger
    start_gdb();
    gdb_tty = gdb->slave_tty();

    if (!emptyCommandQueue())
    {
	// GDB startup leaves us with pending graph commands.  We
	// should better initialize the VSL library right now.
	DispBox::init_vsllib();
    }

    // Enter main event loop
    ddd_main_loop();

    return EXIT_SUCCESS;	// Never reached
}



//-----------------------------------------------------------------------------
// Process next X Event
//-----------------------------------------------------------------------------

void process_next_event()
{
#if HAVE_EXCEPTIONS
    try {
#endif

    if (app_data.poll_child_status)
    {
	// Check if GDB is still running
	gdb->running();

	// Check if the command TTY is still open
	tty_running();
    }
    else
    {
#if !ASYNC_CHILD_STATUS_CHANGE
	// Process pending child status changes
	Agent::runningAgents.commit();
#endif
    }

    // Check if the separate TTY is still running
    exec_tty_running();

    // Check for emergencies
    process_emergencies();

    // Update tear-off menus
    gdbUpdateAllMenus();

    // Restart blinker
    blink(gdb->recording() || !gdb->isReadyWithPrompt());

    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);

    if ((XtAppPending(app_context) & XtIMXEvent) != 0)
    {
	XEvent event;

	if (XtAppPeekEvent(app_context, &event))
	{
	    // We have an X event pending.  Check it.
	    switch (event.type)
	    {
	    case MotionNotify:
	    case ButtonPress:
	    case ButtonRelease:
	    case EnterNotify:
	    case LeaveNotify:
	    case KeyPress:
	    case KeyRelease:

		// The pointer cannot be grabbed, since we receive
		// input events.
		check_grabs(false);
		break;

	    default:
		break;
	    }
	}
    }

    if (app_data.synchronous_gdb && gdb->isBusyOnQuestion())
    {
	// Synchronous mode: wait for GDB to answer question
	XtAppProcessEvent(app_context, XtIMAlternateInput);
    }
    else if ((XtAppPending(app_context) & (XtIMXEvent | XtIMTimer)) != 0)
    {
	// Process next X event
	XtAppProcessEvent(app_context, XtIMXEvent | XtIMTimer);
    }
    else
    {
	// Process pending GDB output
	XtAppProcessEvent(app_context, XtIMAll);
    }
#if HAVE_EXCEPTIONS
    }
#if HAVE_STD_EXCEPTIONS
    // Catch standard exceptions.
#if HAVE_TYPEINFO
    // Just get the exception type and diagnostics.
    catch (const exception& err)
    {
	ddd_show_exception(typeid(err).name(), err.what());
    }
#else // !HAVE_TYPEINFO
    // Catch the two major error classes.
    catch (const logic_error& err)
    {
	ddd_show_exception("logic_error", err.what());
    }
    catch (const runtime_error& err)
    {
	ddd_show_exception("runtime_error", err.what());
    }
    catch (const exception& err)
    {
	ddd_show_exception("exception", err.what());
    }
#endif // !HAVE_TYPEINFO
#endif // HAVE_STD_EXCEPTIONS
    catch (...)
    {
	// Handle non-standard C++ exceptions
	ddd_show_exception();
    }
#endif // HAVE_EXCEPTIONS
}

void process_pending_events()
{
    if (command_shell == 0)
	return;

    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);
    while (XtAppPending(app_context))
	process_next_event();
}


//-----------------------------------------------------------------------------
// Check if interaction events (key or mouse) are pending
//-----------------------------------------------------------------------------

static bool pending_interaction()
{
    XEvent event;
    const long mask = KeyPressMask | ButtonMotionMask | ButtonPressMask;
    Bool pending = XCheckMaskEvent(XtDisplay(command_shell), mask, &event);
    if (pending)
	XPutBackEvent(XtDisplay(command_shell), &event);
    return pending;
}

//-----------------------------------------------------------------------------
// Check this version; give warnings if needed (no license, beta expired, etc.)
//-----------------------------------------------------------------------------

static void ddd_check_version()
{
    static bool checked = false;

    if (checked)
	return;
    checked = true;

    if (restart_session() != "")
	return; // We told the user in the previous DDD instance

    // Tell user once more about version mismatches
    if (!version_warnings.isEmpty())
    {
	Arg args[10];
	int arg = 0;

	XtSetArg(args[arg], XmNmessageString, 
		 version_warnings.xmstring()); arg++;
	Widget warning = 
	    verify(XmCreateWarningDialog(command_shell, "bad_version_warning",
					 args, arg));
	Delay::register_shell(warning);
	XtUnmanageChild(XmMessageBoxGetChild(warning, XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(warning, XmNhelpCallback, ImmediateHelpCB, NULL);

	manage_and_raise(warning);
    }

    if (app_data.dddinit_version == 0 ||
	string(app_data.dddinit_version) != DDD_VERSION)
    {
	// We have no ~/.ddd/init file or an old one: show version info
	HelpOnVersionCB(command_shell, 0, 0);

	// We have no ~/.ddd/init file: create a simple one
	if (app_data.dddinit_version == 0)
	{
	    static string session = app_data.session;
	    app_data.session = DEFAULT_SESSION;
	    save_options(CREATE_OPTIONS);
	    app_data.session = session;
	}
    }

    // Check for expired versions
    if (ddd_expired())
    {
	ostrstream msg;
	msg << "This " DDD_NAME " version (" DDD_VERSION ") has expired since "
	    << ddd_expiration_date() << ".\n"
	    << "Please upgrade to the recent " DDD_NAME " version.";

	post_warning(string(msg), "expired_warning");
    }

    if (app_data.startup_tips)
	TipOfTheDayCB(gdb_w);
}

// Read in database from FILENAME.  Upon version mismatch, ignore some
// resources such as window sizes.
XrmDatabase GetFileDatabase(char *filename)
{
    string version_found = "";

    string tempfile = tmpnam(0);
    ofstream os(tempfile);
    ifstream is(filename);

#if 0
    clog << "Copying " << filename << " to " << tempfile << "\n";
#endif

    // Resources to ignore upon copying
    static char *do_not_copy[] = 
    { 
	XmNwidth, XmNheight,	              // Shell sizes
	XmNcolumns, XmNrows,	              // Text window sizes
	XtNtoolRightOffset, XtNtoolTopOffset, // Command tool offset
	XtNshowHints,		              // Show edge hints
    };

    bool version_mismatch = false;
    while (is)
    {
	char _line[BUFSIZ];
	_line[0] = '\0';
	is.getline(_line, sizeof(_line));
	string line = _line;

	bool copy = true;
	if (line.contains('!', 0))
	{
	    // Comment -- proceed
	}
	else
	{
	    if (line.contains(XtNdddinitVersion ":"))
	    {
		version_found = line.after(":");
		strip_space(version_found);

		if (version_found != DDD_VERSION)
		    version_mismatch = true;
	    }
	    else
	    {
		for (int i = 0; copy && i < int(XtNumber(do_not_copy)); i++)
		{
		    string res = string(".") + do_not_copy[i] + ":";
		    if (line.contains(res) && version_mismatch)
		    {
#if 0
			cerr << "Warning: ignoring " << line 
			     << " in " << filename << "\n";
#endif
			copy = false;
		    }
		}
	    }
	}

	if (copy)
	    os << line << '\n';
    }

    // Flush them all
    os.close();
    is.close();

    if (version_mismatch)
    {
	// Read database from filtered file
	XrmDatabase db = XrmGetFileDatabase(tempfile);
	unlink(tempfile);
	return db;
    }
    else
    {
	// No version mismatch - read from original file
	unlink(tempfile);
	return XrmGetFileDatabase(filename);
    }
}



//-----------------------------------------------------------------------------
// Install button tips
//-----------------------------------------------------------------------------

static void install_button_tips()
{
    const WidgetArray& shells = Delay::shells();
    for (int i = 0; i < shells.size(); i++)
    {
	Widget shell = shells[i];
	while (shell && !XmIsVendorShell(shell) && !XmIsMenuShell(shell))
	    shell = XtParent(shell);
	if (shell)
	    InstallButtonTips(shell);
    }
}

void register_menu_shell(MMDesc *items)
{
    if (items == 0)
	return;

    // Register shell of this menu
    Widget shell = items[0].widget;
    while (shell && !XtIsShell(shell))
	shell = XtParent(shell);
    if (shell)
	Delay::register_shell(shell);

    // Register shells of submenus
    for (int i = 0; items[i].widget != 0; i++)
	register_menu_shell(items[i].items);
}


//-----------------------------------------------------------------------------
// Verify buttons
//-----------------------------------------------------------------------------

static void verify_button(MMDesc *item, XtPointer)
{
    verify_button(item->widget);
}

static void verify_buttons(MMDesc *items)
{
    MMonItems(items, verify_button);
}

//-----------------------------------------------------------------------------
// Create DataDisp shortcut menu
//-----------------------------------------------------------------------------

static void set_shortcut_menu(DataDisp *data_disp, string exprs)
{
    int newlines = exprs.freq('\n') + 1;
    string *items = new string[newlines];
    split(exprs, items, newlines, '\n');

    StringArray items_s;
    StringArray labels_s;
    for (int i = 0; i < newlines; i++)
    {
	string item  = items[i];
	string label = "";

	if (item.contains(app_data.label_delimiter))
	{
	    label = item.after(app_data.label_delimiter);
	    item  = item.before(app_data.label_delimiter);
	}

	strip_space(item);
	strip_space(label);

	if (item == "")
	    continue;

	items_s  += item;
	labels_s += label;
    }

    data_disp->set_shortcut_menu(items_s, labels_s);

    delete[] items;
}

//-----------------------------------------------------------------------------
// Fix size of status line
//-----------------------------------------------------------------------------

static void fix_status_size()
{
    if (!app_data.status_at_bottom)
	return;

    Widget status_form = XtParent(status_w);
    if (!XtIsRealized(status_form))
	return;

    Dimension pane_maximum, height;
    XtVaGetValues(status_form,
		  XmNpaneMaximum, &pane_maximum, 
		  XmNheight, &height,
		  NULL);

    if (height <= pane_maximum)
	return;

    XtVaSetValues(status_form,
		  XmNallowResize, True,
		  XmNheight, pane_maximum,
		  XmNallowResize, False,
		  NULL);

    if (lesstif_version <= 79)
    {
	// For LessTif 0.79 and earlier, simulate a drag of the lowest
	// sash to the bottom.  Ugly LessTif hack.

	// Find the children of the paned window
	Widget paned = XtParent(status_form);
	WidgetList children;
	Cardinal num_children = 0;
    
	XtVaGetValues(paned, 
		      XmNchildren, &children,
		      XmNnumChildren, &num_children,
		      NULL);

	// The sash controlling the status line is the lowest of all
	Widget sash = 0;
	for (Cardinal i = 0; i < num_children; i++)
	{
	    Widget child = children[i];

	    if (XmIsSash(child) && XtIsRealized(child) && XtIsManaged(child))
	    {
		Position sash_y  = 0;
		Position child_y = 1;

		if (sash != 0)
		{
		    XtVaGetValues(sash,  XmNy, &sash_y, NULL);
		    XtVaGetValues(child, XmNy, &child_y, NULL);
		}

		if (child_y > sash_y)
		    sash = child;
	    }
	}

	if (sash != 0 && XtIsRealized(sash))
	{
	    // Simulate a vertical drag of MOVEMENT pixels
	    const Dimension movement = 
		max(height, HeightOfScreen(XtScreen(sash)));

	    // Press button 1 ...
	    XEvent event;
	    event.type                = ButtonPress;
	    event.xbutton.serial      = 0;
	    event.xbutton.display     = XtDisplay(sash);
	    event.xbutton.window      = XtWindow(sash);
	    event.xbutton.root        = RootWindowOfScreen(XtScreen(sash));
	    event.xbutton.subwindow   = None;
	    event.xbutton.time        = 
		XtLastTimestampProcessed(XtDisplay(sash));
	    event.xbutton.x           = 0;
	    event.xbutton.y           = 0;
	    event.xbutton.x_root      = 0;
	    event.xbutton.y_root      = 0;
	    event.xbutton.state       = Button1Mask;
	    event.xbutton.button      = Button1;
	    event.xbutton.same_screen = True;
	    XtDispatchEvent(&event);

	    // ... move down ...
	    for (Dimension y = 0; y < movement; y += 5)
	    {
		event.type                = MotionNotify;
		event.xmotion.serial      = 0;
		event.xmotion.display     = XtDisplay(sash);
		event.xmotion.window      = XtWindow(sash);
		event.xmotion.root        = 
		    RootWindowOfScreen(XtScreen(sash));
		event.xmotion.subwindow   = None;
		event.xmotion.time        = 
		    XtLastTimestampProcessed(XtDisplay(sash));
		event.xmotion.x           = 0;
		event.xmotion.y           = y;
		event.xmotion.x_root      = 0;
		event.xmotion.y_root      = y;
		event.xmotion.state       = Button1Mask;
		event.xmotion.is_hint     = NotifyNormal;
		event.xmotion.same_screen = True;
		XtDispatchEvent(&event);
	    }

	    // ... until target position is reached ...
	    event.type                = MotionNotify;
	    event.xmotion.serial      = 0;
	    event.xmotion.display     = XtDisplay(sash);
	    event.xmotion.window      = XtWindow(sash);
	    event.xmotion.root        = RootWindowOfScreen(XtScreen(sash));
	    event.xmotion.subwindow   = None;
	    event.xmotion.time        = 
		XtLastTimestampProcessed(XtDisplay(sash));
	    event.xmotion.x           = 0;
	    event.xmotion.y           = movement;
	    event.xmotion.x_root      = 0;
	    event.xmotion.y_root      = movement;
	    event.xmotion.state       = Button1Mask;
	    event.xmotion.is_hint     = NotifyNormal;
	    event.xmotion.same_screen = True;
	    XtDispatchEvent(&event);

	    // ... and release button1 again.
	    event.type                = ButtonRelease;
	    event.xbutton.serial      = 0;
	    event.xbutton.display     = XtDisplay(sash);
	    event.xbutton.window      = XtWindow(sash);
	    event.xbutton.root        = RootWindowOfScreen(XtScreen(sash));
	    event.xbutton.subwindow   = None;
	    event.xbutton.time        = 
		XtLastTimestampProcessed(XtDisplay(sash));
	    event.xbutton.x           = 0;
	    event.xbutton.y           = movement;
	    event.xbutton.x_root      = 0;
	    event.xbutton.y_root      = movement;
	    event.xbutton.state       = Button1Mask;
	    event.xbutton.button      = Button1;
	    event.xbutton.same_screen = True;
	    XtDispatchEvent(&event);
	}
    }
}


//---------------------------------------------------------------------------
// Locking stuff
//---------------------------------------------------------------------------

static bool continue_despite_lock = false;
static int lock_dialog_x = -1;
static int lock_dialog_y = -1;

static void ContinueDespiteLockCB(Widget, XtPointer, XtPointer)
{
    continue_despite_lock = true;
}

static void TryLock(XtPointer client_data, XtIntervalId *)
{
    Widget w = (Widget)client_data;

    LockInfo info;
    bool lock_ok = lock_session_dir(XtDisplay(w), DEFAULT_SESSION, info);

    if (lock_ok)
    {
	continue_despite_lock = true;
	return;
    }

    XtAppAddTimeOut(XtWidgetToApplicationContext(w), 500, 
		    TryLock, client_data);
}

#if XmVersion >= 1002
static void KillLockerCB(Widget w, XtPointer client_data, XtPointer)
{
    static int attempts_to_kill = 0;

    LockInfo *info = (LockInfo *)client_data;

    kill(info->pid, SIGHUP);

    if (attempts_to_kill++ == 0)
    {
	// Try locking again until successful
	TryLock(XtPointer(w), 0);
    }
}
#endif

static void lock_ddd(Widget parent)
{
    LockInfo info;
    bool lock_ok = lock_session_dir(XtDisplay(parent), DEFAULT_SESSION, info);
    if (lock_ok)
	return;

    bool on_local_host = (info.hostname == fullhostname());

    string lock_file = session_lock_file(DEFAULT_SESSION);
    
    MString msg = rm(DDD_NAME " has detected a ") 
	+ tt(lock_file) + rm(" file.") + cr()
	+ cr()
	+ rm("This may indicate that another user is running "
	     DDD_NAME) + cr()
	+ rm("using your ")
	+ tt(session_state_dir() + "/") + rm(" files.") + cr()
	+ rm(DDD_NAME " appears to be running under process ID ") 
	+ tt(itostring(info.pid));

    if (!on_local_host)
	msg += cr() + rm("on host ") + tt(info.hostname);

    msg += rm(".") 
	+ cr()
	+ rm("You may continue to use " DDD_NAME ", but your "
	     "saved " DDD_NAME " state may be") + cr()
	+ rm("overwritten by the other " DDD_NAME " instance.") + cr()
	+ cr();

#if XmVersion >= 1002
    if (on_local_host)
	msg += rm("You can kill the other " DDD_NAME " instance now.") + cr();
#endif

    msg += rm("Otherwise, make sure that you are not running "
	      "another " DDD_NAME " and ") + cr()
	+ rm("delete the ") + tt(lock_file) + rm(" file.") + cr()
	+ cr()
	+ rm("To continue despite the other " DDD_NAME " instance, ")
	+ rm("click on ") + bf("Continue") + rm(".") + cr();
#if XmVersion >= 1002
    if (on_local_host)
	msg += rm("To kill the other " DDD_NAME " instance and continue, ")
	    + rm("click on ") + bf("Kill") + rm(".") + cr();
#endif

    msg += rm("To exit this " DDD_NAME " instance, click on ") 
	+ bf("Exit") + rm(".");
	
    Arg args[10];
    int arg = 0;
	
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
    XtSetArg(args[arg], XmNdialogStyle, 
	     XmDIALOG_FULL_APPLICATION_MODAL); arg++;

    string geometry;
    if (lock_dialog_x >= 0 && lock_dialog_y >= 0)
    {
	ostrstream os;
	os << "+" << lock_dialog_x << "+" << lock_dialog_y;
	geometry = string(os);
	XtSetArg(args[arg], XmNgeometry, geometry.chars()); arg++;
	XtSetArg(args[arg], XmNx, lock_dialog_x); arg++;
	XtSetArg(args[arg], XmNy, lock_dialog_y); arg++;
    }

    Widget lock_dialog =
	verify(XmCreateQuestionDialog(parent, "lock_dialog", args, arg));
    Delay::register_shell(lock_dialog);

#if XmVersion >= 1002
    Widget kill = verify(XmCreatePushButton(lock_dialog, "kill", 0, 0));
    XtManageChild(kill);
    XtAddCallback(kill, XmNactivateCallback,
		  KillLockerCB, XtPointer(&info));
    set_sensitive(kill, on_local_host);
#endif

    XtAddCallback(lock_dialog, XmNhelpCallback,
		  ImmediateHelpCB, NULL);
    XtAddCallback(lock_dialog, XmNokCallback,
		  ContinueDespiteLockCB, NULL);
    XtAddCallback(lock_dialog, XmNcancelCallback, ExitCB, 
		  XtPointer(EXIT_FAILURE));

    if (geometry != "")
    {
	Widget shell = lock_dialog;
	while (!XmIsDialogShell(shell))
	    shell = XtParent(shell);

	arg = 0;
	XtSetArg(args[arg], XmNgeometry, geometry.chars()); arg++;
	XtSetArg(args[arg], XmNx, lock_dialog_x); arg++;
	XtSetArg(args[arg], XmNy, lock_dialog_y); arg++;
	XtSetValues(shell, args, arg);
    }

    continue_despite_lock = false;
    manage_and_raise(lock_dialog);

    while (!continue_despite_lock)
	XtAppProcessEvent(XtWidgetToApplicationContext(lock_dialog), XtIMAll);

    XtDestroyWidget(lock_dialog);

    // Try locking once more
    lock_ok = lock_session_dir(XtDisplay(parent), DEFAULT_SESSION, info);
}


//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------

static Boolean session_setup_done(XtPointer)
{
    if (emptyCommandQueue() && gdb->isReadyWithPrompt())
    {
	// Delete initialization delay, if any
	delete init_delay;
	init_delay = 0;
	set_status_mstring(rm("Welcome to " DDD_NAME " " DDD_VERSION "!"));

	if (app_data.initial_session != 0)
	{
	    set_session(app_data.initial_session);
	    app_data.initial_session = 0;
	}

	return True;		// Remove from the list of work procs
    }
    return False;		// Get called again
}

Boolean ddd_setup_done(XtPointer)
{
    // Delete setup delay, if any
    delete setup_delay;
    setup_delay = 0;

    if (emptyCommandQueue() && gdb->isReadyWithPrompt())
    {
	// Some WMs have trouble with early decorations.  Just re-decorate.
	setup_new_shell(command_shell);
	setup_new_shell(data_disp_shell);
	setup_new_shell(source_view_shell);

	ddd_check_version();
	install_button_tips();
	fix_status_size();

	if (running_shells() == 0)
	{
	    // We have no shell (yet).  Be sure to popup at least one shell.
	    if (app_data.source_window)
		gdbOpenSourceWindowCB(gdb_w, 0, 0);
	    else if (app_data.data_window)
		gdbOpenDataWindowCB(gdb_w, 0, 0);
	    else
		gdbOpenCommandWindowCB(gdb_w, 0, 0);
	}

	// Initialize `views' menu.  LessTif needs this.
 	gdbUpdateViewsCB(0, XtPointer(views_menu),        0);

	// Initialize `view' menus.  LessTif needs this.
 	gdbUpdateViewCB(0,  XtPointer(command_view_menu), 0);
 	gdbUpdateViewCB(0,  XtPointer(source_view_menu),  0);
 	gdbUpdateViewCB(0,  XtPointer(data_view_menu),    0);

	main_loop_entered = true;

	DispBox::init_vsllib(process_pending_events);
	DataDisp::refresh_graph_edit();

	if (init_delay != 0 || app_data.initial_session != 0)
	{
	    // Restoring session may still take time
	    XtAppAddWorkProc(XtWidgetToApplicationContext(command_shell),
			     session_setup_done, 0);
	}
	return True;		// Remove from the list of work procs
    }
    else
    {
	return False;		// Keep on processing the command queue
    }
}



//-----------------------------------------------------------------------------
// Activate argument field
//-----------------------------------------------------------------------------

static void ActivateCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
    
    Widget button = Widget(client_data);
    XtCallActionProc(button, "ArmAndActivate", cbs->event, (String *)0, 0);
}

//-----------------------------------------------------------------------------
// Drag and drop
//-----------------------------------------------------------------------------

static void CheckDragCB(Widget, XtPointer, XtPointer call_data)
{
    (void) call_data;		// Use it

    // Some Linux Motif implementations have trouble dragging pixmaps.
    // Disable this, such that we don't get drowned in bug reports.
#if XmVersion >= 2000 && defined(__linux__)
    XmDragStartCallbackStruct *cbs = (XmDragStartCallbackStruct *)call_data;
    Widget w = cbs->widget;
    // clog << "Dragging from " << longName(w) << "\n";

    if (XtIsSubclass(w, xmLabelWidgetClass) 
	|| XtIsSubclass(w, xmLabelGadgetClass))
    {
	unsigned char label_type = XmSTRING;
	XtVaGetValues(w, XmNlabelType, &label_type, NULL);
	if (label_type == XmPIXMAP)
	    cbs->doit = False;
    }
#endif // XmNdragStartCallback
}


//-----------------------------------------------------------------------------
// Context help
//-----------------------------------------------------------------------------

static StatusMsg *context_help_msg = 0;

static void PreHelpOnContext(Widget w, XtPointer, XtPointer)
{
    delete context_help_msg;
    context_help_msg = 
	new StatusMsg("Please click on the item you want information for");

    XFlush(XtDisplay(w));
}

static void PostHelpOnItem(Widget item)
{
    if (item != 0)
    {
	if (context_help_msg != 0)
	    context_help_msg->outcome = longName(item);
    }

    delete context_help_msg;
    context_help_msg = 0;
}


//-----------------------------------------------------------------------------
// Option handling
//-----------------------------------------------------------------------------

static void set_toggle(Widget w, unsigned char new_state, bool notify = false)
{
    if (w == 0)
	return;

    assert(XmIsToggleButton(w));

#if XmVersion < 2000
    Boolean old_state;
#else
    unsigned char old_state;
#endif
    XtVaGetValues(w, XmNset, &old_state, NULL);

    if (old_state != new_state)
    {
	if (notify)
	    XmToggleButtonSetState(w, new_state, True);
	else
	    XtVaSetValues(w, XmNset, new_state, NULL);
    }
}

inline void notify_set_toggle(Widget w, Boolean new_state)
{
    set_toggle(w, new_state, true);
}

inline void set_string(Widget w, String value)
{
    if (w == 0)
	return;

    XtVaSetValues(w, 
		  XmNvalue, value,
		  XmNcursorPosition, 0,
		  NULL);
}

static bool have_cmd(const string& cmd)
{
    return cmd_file(cmd).contains('/', 0);
}

// Reflect state in option menus
void update_options()
{
    set_toggle(find_words_only_w, app_data.find_words_only);
    set_toggle(words_only_w, app_data.find_words_only);

    set_toggle(find_case_sensitive_w, app_data.find_case_sensitive);
    set_toggle(case_sensitive_w, app_data.find_case_sensitive);

    set_toggle(disassemble_w, app_data.disassemble);

    bool separate_exec_window = 
	app_data.separate_exec_window && gdb->has_redirection();
    set_toggle(command_separate_exec_window_w, separate_exec_window);
    set_toggle(source_separate_exec_window_w,  separate_exec_window);
    set_toggle(data_separate_exec_window_w,    separate_exec_window);

    set_toggle(button_tips_w,            app_data.button_tips); 
    set_toggle(value_tips_w,             app_data.value_tips); 
    set_toggle(button_docs_w,            app_data.button_docs); 
    set_toggle(value_docs_w,             app_data.value_docs); 
    set_toggle(set_global_completion_w,  app_data.global_tab_completion);
    set_toggle(set_console_completion_w, !app_data.global_tab_completion);
    set_toggle(group_iconify_w,          app_data.group_iconify);
    set_toggle(uniconify_when_ready_w,   app_data.uniconify_when_ready);
    set_toggle(check_grabs_w,   	 app_data.check_grabs);
    set_toggle(suppress_warnings_w,      app_data.suppress_warnings);

    set_toggle(cache_source_files_w,     app_data.cache_source_files);
    set_toggle(cache_machine_code_w,     app_data.cache_machine_code);
    set_toggle(set_display_glyphs_w,     app_data.display_glyphs);
    set_toggle(set_display_text_w,       !app_data.display_glyphs);
    set_toggle(set_refer_path_w,         app_data.use_source_path);
    set_toggle(set_refer_base_w,         !app_data.use_source_path);

    if (tab_width_w != 0)
    {
	XtVaSetValues(tab_width_w,     XmNvalue, app_data.tab_width, NULL);
	int source_indent = app_data.indent_source;
	if (app_data.display_line_numbers)
	    source_indent += app_data.line_number_width;
	XtVaSetValues(source_indent_w, XmNvalue, source_indent, NULL);
	XtVaSetValues(code_indent_w,   XmNvalue, app_data.indent_code, NULL);
    }

    set_toggle(led_w, app_data.blink_while_busy);

    Boolean show_grid, snap_to_grid, show_hints, auto_layout, show_annotations;
    LayoutMode layout_mode;
    Dimension grid_width, grid_height;

    XtVaGetValues(data_disp->graph_edit, 
 		  XtNshowGrid,   &show_grid,
 		  XtNsnapToGrid, &snap_to_grid,
		  XtNshowHints,  &show_hints,
		  XtNshowAnnotations, &show_annotations,
		  XtNautoLayout, &auto_layout,
		  XtNlayoutMode, &layout_mode,
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  NULL);

    set_toggle(detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_align_2d_arrays_w, app_data.align_2d_arrays);

    if (graph_snap_to_grid_w != 0)
    {
	if (!show_grid && XtIsSensitive(graph_snap_to_grid_w))
	{
	    // Grid has been disabled - disable `snap to grid' as well
	    XtVaSetValues(data_disp->graph_edit, XtNsnapToGrid, False, NULL);
	}
	else if (show_grid && !XtIsSensitive(graph_snap_to_grid_w))
	{
	    // Grid has been re-enabled - restore `snap to grid' setting
	    XtVaSetValues(data_disp->graph_edit, XtNsnapToGrid, 
			  XmToggleButtonGetState(graph_snap_to_grid_w), NULL);
	}
	else
	{
	    set_toggle(graph_snap_to_grid_w, snap_to_grid);
	}
    }

    set_sensitive(graph_snap_to_grid_w, show_grid);
    set_sensitive(align_w, show_grid);

    set_toggle(graph_show_hints_w, show_hints);
    set_toggle(graph_show_annotations_w, show_annotations);
    set_toggle(graph_auto_layout_w, auto_layout);
    set_toggle(graph_compact_layout_w, layout_mode == CompactLayoutMode);
    set_toggle(graph_auto_close_w, app_data.auto_close_data_window);
    set_toggle(graph_show_dependent_titles_w,
	       app_data.show_dependent_display_titles);

    if (graph_grid_size_w != 0)
	XtVaSetValues(graph_grid_size_w, XmNvalue, show_grid ? grid_width : 0, 
		      NULL);


    unsigned char policy = '\0';
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &policy, NULL);
    set_toggle(set_focus_pointer_w,        policy == XmPOINTER);
    set_toggle(set_focus_explicit_w,       policy == XmEXPLICIT);

    set_toggle(set_scrolling_panner_w,     app_data.panned_graph_editor);
    set_toggle(set_scrolling_scrollbars_w, !app_data.panned_graph_editor);

    set_toggle(set_button_images_w,        app_data.button_images);
    set_toggle(set_button_captions_w,      app_data.button_captions);
    set_toggle(set_flat_buttons_w,         app_data.flat_toolbar_buttons);

    string button_color_key        = app_data.button_color_key;
    string active_button_color_key = app_data.active_button_color_key;
#if XmVersion < 2000
    set_toggle(set_color_buttons_w, button_color_key == 'c');
#else
    if (button_color_key == 'c' && active_button_color_key == 'c')
	set_toggle(set_color_buttons_w, XmSET);
    else if (button_color_key == active_button_color_key)
	set_toggle(set_color_buttons_w, XmUNSET);
    else
	set_toggle(set_color_buttons_w, XmINDETERMINATE);
#endif

    set_toggle(set_tool_buttons_in_toolbar_w,      app_data.command_toolbar);
    set_toggle(set_tool_buttons_in_command_tool_w, !app_data.command_toolbar);

    Boolean separate = 
	app_data.separate_data_window || app_data.separate_source_window;
    set_toggle(set_separate_windows_w, separate);
    set_toggle(set_attached_windows_w, !separate);

    DebuggerType type = debugger_type(app_data.debugger);
    set_toggle(set_debugger_gdb_w, type == GDB);
    set_toggle(set_debugger_dbx_w, type == DBX);
    set_toggle(set_debugger_xdb_w, type == XDB);
    set_toggle(set_debugger_jdb_w, type == JDB);

    set_sensitive(set_debugger_gdb_w, have_cmd("gdb"));
    set_sensitive(set_debugger_dbx_w, have_cmd("dbx"));
    set_sensitive(set_debugger_xdb_w, have_cmd("xdb"));
    set_sensitive(set_debugger_jdb_w, have_cmd("jdb"));

    set_toggle(splash_screen_w, app_data.splash_screen);
    set_toggle(startup_tips_w,  app_data.startup_tips);

    if (app_data.cache_source_files != source_view->cache_source_files)
    {
	source_view->cache_source_files = app_data.cache_source_files;
	if (!app_data.cache_source_files)
	    source_view->clear_file_cache();
    }

    if (app_data.cache_machine_code != source_view->cache_machine_code)
    {
	source_view->cache_machine_code = app_data.cache_machine_code;
	if (!app_data.cache_machine_code)
	    source_view->clear_code_cache();
    }

    source_view->set_display_line_numbers(app_data.display_line_numbers);
    source_view->set_display_glyphs(app_data.display_glyphs);
    source_view->set_disassemble(gdb->type() == GDB && app_data.disassemble);
    source_view->set_all_registers(app_data.all_registers);
    source_view->set_tab_width(app_data.tab_width);
    source_view->set_indent(app_data.indent_source, app_data.indent_code);

    source_view->line_indent_amount   = app_data.line_number_width;
    source_view->lines_above_cursor   = app_data.lines_above_cursor;
    source_view->lines_below_cursor   = app_data.lines_below_cursor;

    source_view->max_breakpoint_number = app_data.max_breakpoint_number;
    data_disp->max_display_number      = app_data.max_display_number;

    data_disp->set_detect_aliases(app_data.detect_aliases);

    if (DispBox::align_2d_arrays != app_data.align_2d_arrays)
    {
	DispBox::align_2d_arrays = app_data.align_2d_arrays;
	// data_disp->refresh_display();
    }

    if (app_data.command_toolbar && 
	command_toolbar_w != 0 && !XtIsManaged(command_toolbar_w))
    {
	if (app_data.source_window)
	    XtManageChild(command_toolbar_w);
	gdbCloseToolWindowCB(command_shell, 0, 0);
    }
    else if (!app_data.command_toolbar && 
	     command_toolbar_w != 0 && XtIsManaged(command_toolbar_w))
    {
	XtUnmanageChild(command_toolbar_w);
	if (app_data.source_window)
	    gdbOpenToolWindowCB(command_shell, 0, 0);
    }

    EnableButtonTips(app_data.button_tips);
    EnableButtonDocs(app_data.button_docs);
    EnableTextTips(app_data.value_tips);
    EnableTextDocs(app_data.value_docs);

    set_string(edit_command_w,       app_data.edit_command);
    set_string(get_core_command_w,   app_data.get_core_command);
    set_string(ps_command_w,         app_data.ps_command);
    set_string(term_command_w,       app_data.term_command);
    set_string(uncompress_command_w, app_data.uncompress_command);
    set_string(www_command_w,        app_data.www_command);

    // Set `find' label
    Widget find_label_ref = 0;
    char *icon = 0;
    switch (current_find_direction())
    {
    case SourceView::forward:
	find_label_ref = find_menu[FindItems::FindForward].widget;
	icon = FIND_FORWARD_ICON;
	break;
	
    case SourceView::backward:
	find_label_ref = find_menu[FindItems::FindBackward].widget;
	icon = FIND_BACKWARD_ICON;
	break;
    }
    XmString label;
    XtVaGetValues(find_label_ref, XmNlabelString, &label, NULL);
    MString new_label(label, true);
    XmStringFree(label);

    set_label(arg_cmd_area[ArgItems::Find].widget, new_label, icon);

    // Font stuff
    if (font_names[DefaultDDDFont] != 0)
    {
	XmTextFieldSetString(font_names[DefaultDDDFont], 
			     app_data.default_font);
	XmTextFieldSetString(font_names[VariableWidthDDDFont],
			     app_data.variable_width_font);
	XmTextFieldSetString(font_names[FixedWidthDDDFont],
			     app_data.fixed_width_font);
	string value;
	value = itostring(app_data.default_font_size);
	XmTextFieldSetString(font_sizes[DefaultDDDFont],       (String)value);
	value = itostring(app_data.variable_width_font_size);
	XmTextFieldSetString(font_sizes[VariableWidthDDDFont], (String)value);
	value = itostring(app_data.fixed_width_font_size);
	XmTextFieldSetString(font_sizes[FixedWidthDDDFont],    (String)value);
    }

    // Key Bindings
    BindingStyle style = app_data.cut_copy_paste_bindings;
    set_toggle(kde_binding_w, style == KDEBindings);
    set_toggle(motif_binding_w, style == MotifBindings);
    set_cut_copy_paste_bindings(command_edit_menu, style);
    set_cut_copy_paste_bindings(source_edit_menu,  style);
    set_cut_copy_paste_bindings(data_edit_menu,    style);

    // Check for source toolbar
    Widget arg_cmd_w = XtParent(source_arg->top());
    if (data_disp->graph_cmd_w == arg_cmd_w)
    {
	// Don't close the common toolbar
    }
    else
    {
	if (XtIsManaged(source_view->source_form()) ||
	    XtIsManaged(source_view->code_form()))
	    manage_paned_child(arg_cmd_w);
	else
	    unmanage_paned_child(arg_cmd_w);
    }

    update_reset_preferences();
    fix_status_size();
}


static void set_settings_title(Widget w)
{
    if (w != 0)
    {
	MString settings_title(gdb->title() + " Settings...");
	XtVaSetValues(w, XmNlabelString, settings_title.xmstring(), NULL);
    }
}



//-----------------------------------------------------------------------------
// Preferences
//-----------------------------------------------------------------------------

// Original application resources
static bool          option_state_saved = false;
static AppData       initial_app_data;
static Boolean       initial_show_grid;
static Boolean       initial_show_hints;
static Boolean       initial_show_annotations;
static Boolean       initial_snap_to_grid;
static Boolean       initial_auto_layout;
static Dimension     initial_grid_width;
static Dimension     initial_grid_height;
static LayoutMode    initial_layout_mode;
static unsigned char initial_focus_policy;

static Widget preferences_dialog;
static Widget reset_preferences_w;
static Widget current_panel;

void save_option_state()
{
    initial_app_data = app_data;

    // Make sure the helper command strings are really duplicated
    static string edit_command;
    edit_command = initial_app_data.edit_command;
    initial_app_data.edit_command = edit_command;

    static string get_core_command;
    get_core_command = initial_app_data.get_core_command;
    initial_app_data.get_core_command = get_core_command;

    static string ps_command;
    ps_command = initial_app_data.ps_command;
    initial_app_data.ps_command = ps_command;

    static string term_command;
    term_command = initial_app_data.term_command;
    initial_app_data.term_command = term_command;

    static string uncompress_command;
    uncompress_command = initial_app_data.uncompress_command;
    initial_app_data.uncompress_command = uncompress_command;

    static string www_command;
    www_command = initial_app_data.www_command;
    initial_app_data.www_command = www_command;

    // Fetch data display resources
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowGrid,        &initial_show_grid,
		  XtNshowAnnotations, &initial_show_annotations,
		  XtNshowHints,       &initial_show_hints,
		  XtNsnapToGrid,      &initial_snap_to_grid,
		  XtNlayoutMode,      &initial_layout_mode, 
		  XtNautoLayout,      &initial_auto_layout,
		  XtNgridWidth,       &initial_grid_width,
		  XtNgridHeight,      &initial_grid_height,
		  NULL);

    XtVaGetValues(command_shell,
		  XmNkeyboardFocusPolicy, &initial_focus_policy, NULL);

    Widget paned_work_w = XtParent(XtParent(gdb_w));
    save_preferred_paned_sizes(paned_work_w);

    option_state_saved = true;
    update_reset_preferences();
}

static void ResetGeneralPreferencesCB(Widget, XtPointer, XtPointer)
{
    notify_set_toggle(button_tips_w, initial_app_data.button_tips);
    notify_set_toggle(button_docs_w, initial_app_data.button_docs);
    notify_set_toggle(value_tips_w, initial_app_data.value_tips);
    notify_set_toggle(value_docs_w, initial_app_data.value_docs);
    notify_set_toggle(set_global_completion_w, 
	       initial_app_data.global_tab_completion);
    notify_set_toggle(set_console_completion_w, 
	       !initial_app_data.global_tab_completion);
    notify_set_toggle(group_iconify_w, initial_app_data.group_iconify);
    notify_set_toggle(uniconify_when_ready_w, 
		      initial_app_data.uniconify_when_ready);
    notify_set_toggle(suppress_warnings_w, initial_app_data.suppress_warnings);
    notify_set_toggle(check_grabs_w, 
		      initial_app_data.check_grabs);
}

static bool general_preferences_changed()
{
    if (app_data.button_tips != initial_app_data.button_tips)
	return true;

    if (app_data.button_docs != initial_app_data.button_docs)
	return true;

    if (app_data.value_tips != initial_app_data.value_tips)
	return true;

    if (app_data.value_docs != initial_app_data.value_docs)
	return true;

    if (app_data.global_tab_completion != 
	initial_app_data.global_tab_completion)
	return true;

    if (app_data.group_iconify != initial_app_data.group_iconify)
	return true;

    if (app_data.uniconify_when_ready != initial_app_data.uniconify_when_ready)
	return true;

    if (app_data.suppress_warnings != initial_app_data.suppress_warnings)
	return true;

    if (app_data.check_grabs != initial_app_data.check_grabs)
	return true;

    return false;
}

static void ResetSourcePreferencesCB(Widget, XtPointer, XtPointer)
{
    notify_set_toggle(set_display_glyphs_w, initial_app_data.display_glyphs);
    notify_set_toggle(set_display_glyphs_w, !initial_app_data.display_glyphs);

    notify_set_toggle(set_tool_buttons_in_toolbar_w, 
		      initial_app_data.command_toolbar);
    notify_set_toggle(set_tool_buttons_in_command_tool_w, 
		      !initial_app_data.command_toolbar);

    notify_set_toggle(set_refer_path_w, initial_app_data.use_source_path);
    notify_set_toggle(set_refer_base_w, !initial_app_data.use_source_path);

    notify_set_toggle(words_only_w, initial_app_data.find_words_only);
    notify_set_toggle(case_sensitive_w, initial_app_data.find_case_sensitive);

    notify_set_toggle(cache_source_files_w, 
		      initial_app_data.cache_source_files);
    notify_set_toggle(cache_machine_code_w, 
		      initial_app_data.cache_machine_code);

    if (app_data.tab_width != initial_app_data.tab_width)
    {
	app_data.tab_width = initial_app_data.tab_width;
	update_options();
    }
    if (app_data.indent_source != initial_app_data.indent_source)
    {
	app_data.indent_source = initial_app_data.indent_source;
	update_options();
    }
    if (app_data.indent_code != initial_app_data.indent_code)
    {
	app_data.indent_code = initial_app_data.indent_code;
	update_options();
    }
}

static bool source_preferences_changed()
{
    if (app_data.display_glyphs != initial_app_data.display_glyphs)
	return true;

    if (app_data.command_toolbar != initial_app_data.command_toolbar)
	return true;

    if (app_data.use_source_path != initial_app_data.use_source_path)
	return true;

    if (app_data.find_words_only != initial_app_data.find_words_only)
	return true;

    if (app_data.find_case_sensitive != initial_app_data.find_case_sensitive)
	return true;

    if (app_data.cache_source_files != initial_app_data.cache_source_files)
	return true;

    if (app_data.cache_machine_code != initial_app_data.cache_machine_code)
	return true;

    if (app_data.tab_width != initial_app_data.tab_width)
	return true;

    if (app_data.indent_source != initial_app_data.indent_source)
	return true;

    if (app_data.indent_code != initial_app_data.indent_code)
	return true;

    if (app_data.display_line_numbers != initial_app_data.display_line_numbers)
	return true;

    return false;
}

static void ResetDataPreferencesCB(Widget, XtPointer, XtPointer)
{
    notify_set_toggle(detect_aliases_w, initial_app_data.detect_aliases);
    notify_set_toggle(graph_detect_aliases_w, initial_app_data.detect_aliases);
    notify_set_toggle(graph_align_2d_arrays_w, 
		      initial_app_data.align_2d_arrays);
    notify_set_toggle(graph_show_hints_w, initial_show_hints);
    notify_set_toggle(graph_show_dependent_titles_w, 
		      initial_app_data.show_dependent_display_titles);
    notify_set_toggle(graph_auto_close_w,
		      initial_app_data.auto_close_data_window);

    Boolean show_hints, show_annotations;

    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowHints,       &show_hints,
		  XtNshowAnnotations, &show_annotations,
		  NULL);

    if (show_hints  != initial_show_hints ||
	show_annotations != initial_show_annotations)
    {
	XtVaSetValues(data_disp->graph_edit,
		      XtNshowHints,       initial_show_hints,
		      XtNshowAnnotations, initial_show_annotations,
		      NULL);
		      
	update_options();
    }
}

static bool data_preferences_changed()
{
    Boolean show_hints, show_annotations;
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowHints,       &show_hints,
		  XtNshowAnnotations, &show_annotations,
		  NULL);

    if (app_data.detect_aliases != initial_app_data.detect_aliases)
	return true;

    if (app_data.align_2d_arrays != initial_app_data.align_2d_arrays)
	return true;

    if (app_data.auto_close_data_window != 
	initial_app_data.auto_close_data_window)
	return true;

    if (show_hints   != initial_show_hints)
	return true;

    if (show_annotations != initial_show_annotations)
	return true;

    if (app_data.show_dependent_display_titles != 
	initial_app_data.show_dependent_display_titles)
	return true;

    return false;
}

static void ResetLayoutPreferencesCB(Widget, XtPointer, XtPointer)
{
    notify_set_toggle(graph_compact_layout_w, 
	       initial_layout_mode == CompactLayoutMode);
    notify_set_toggle(graph_auto_layout_w, initial_auto_layout);
    notify_set_toggle(graph_snap_to_grid_w, initial_snap_to_grid);

    Dimension grid_width, grid_height;
    Boolean show_grid;
    XtVaGetValues(data_disp->graph_edit, 
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  XtNshowGrid,   &show_grid,
		  NULL);

    if (grid_width  != initial_grid_width || 
	grid_height != initial_grid_height ||
	show_grid   != initial_show_grid)
    {
	XtVaSetValues(data_disp->graph_edit,
		      XtNgridWidth,  initial_grid_width,
		      XtNgridHeight, initial_grid_height,
		      XtNshowGrid,   initial_show_grid,
		      NULL);
		      
	update_options();
    }
}

static bool layout_preferences_changed()
{
    Boolean show_grid, snap_to_grid, auto_layout;
    LayoutMode layout_mode;
    Dimension grid_width, grid_height;

    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowGrid,   &show_grid,
		  XtNsnapToGrid, &snap_to_grid,
		  XtNlayoutMode, &layout_mode, 
		  XtNautoLayout, &auto_layout,
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  NULL);

    if (show_grid    != initial_show_grid)
	return true;

    if (snap_to_grid != initial_snap_to_grid)
	return true;

    if (layout_mode  != initial_layout_mode )
	return true;

    if (auto_layout  != initial_auto_layout)
	return true;

    if (grid_width   != initial_grid_width)
	return true;

    if (grid_height  != initial_grid_height)
	return true;

    return false;
}

static void ResetStartupPreferencesCB(Widget, XtPointer, XtPointer)
{
    Boolean separate = initial_app_data.separate_data_window 
	|| initial_app_data.separate_source_window;

    notify_set_toggle(set_separate_windows_w, separate);
    notify_set_toggle(set_attached_windows_w, !separate);

    notify_set_toggle(set_button_captions_w, initial_app_data.button_captions);
    notify_set_toggle(set_button_images_w,   initial_app_data.button_images);
    notify_set_toggle(set_flat_buttons_w,    
		      initial_app_data.flat_toolbar_buttons);

    string button_color_key        = initial_app_data.button_color_key;
    string active_button_color_key = initial_app_data.active_button_color_key;
#if XmVersion < 2000
    notify_set_toggle(set_color_buttons_w, button_color_key == 'c');
#else
    if (button_color_key == 'c' && active_button_color_key == 'c')
	notify_set_toggle(set_color_buttons_w, XmSET);
    else if (button_color_key == active_button_color_key)
	notify_set_toggle(set_color_buttons_w, XmUNSET);
    else
	notify_set_toggle(set_color_buttons_w, XmINDETERMINATE);
#endif

    notify_set_toggle(set_focus_pointer_w, 
		      initial_focus_policy == XmPOINTER);
    notify_set_toggle(set_focus_explicit_w,
		      initial_focus_policy == XmEXPLICIT);

    notify_set_toggle(set_scrolling_panner_w, 
	       initial_app_data.panned_graph_editor);
    notify_set_toggle(set_scrolling_scrollbars_w, 
	       !initial_app_data.panned_graph_editor);

    DebuggerType type = debugger_type(initial_app_data.debugger);
    notify_set_toggle(set_debugger_gdb_w, type == GDB);
    notify_set_toggle(set_debugger_dbx_w, type == DBX);
    notify_set_toggle(set_debugger_xdb_w, type == XDB);
    notify_set_toggle(set_debugger_jdb_w, type == JDB);

    BindingStyle style = initial_app_data.cut_copy_paste_bindings;
    notify_set_toggle(kde_binding_w, style == KDEBindings);
    notify_set_toggle(motif_binding_w, style == MotifBindings);

    notify_set_toggle(splash_screen_w, initial_app_data.splash_screen);
    notify_set_toggle(startup_tips_w,  initial_app_data.startup_tips);
}


static bool startup_preferences_changed()
{

    // Christoph L. Spiel <Christoph_Spiel@physik.tu-muenchen.de>
    // reports that his g++-2.7.2.3 chokes on large `||' expressions.
    // Hence, instead of `return A || B || C', we say `if A return
    // true; if B return true; if C return true; return false;'.

    if (app_data.startup_tips != initial_app_data.startup_tips)
	return true;

    if (app_data.cut_copy_paste_bindings !=
	initial_app_data.cut_copy_paste_bindings)
	return true;

    if (app_data.splash_screen != initial_app_data.splash_screen)
	return true;

    Boolean initial_separate = (initial_app_data.separate_data_window ||
				initial_app_data.separate_source_window);
    Boolean separate = (app_data.separate_data_window || 
			app_data.separate_source_window);
    if (separate != initial_separate)
	return true;

    if (app_data.button_images != initial_app_data.button_images)
	return true;

    if (app_data.button_captions != initial_app_data.button_captions)
	return true;

    if (app_data.flat_toolbar_buttons != initial_app_data.flat_toolbar_buttons)
	return true;

    if (string(app_data.button_color_key) !=
	string(initial_app_data.button_color_key))
	return true;

    if (string(app_data.active_button_color_key) !=
	string(initial_app_data.active_button_color_key))
	return true;

    unsigned char focus_policy;
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &focus_policy, NULL);
    if (focus_policy != initial_focus_policy)
	return true;

    if (app_data.panned_graph_editor != initial_app_data.panned_graph_editor)
	return true;

    if (debugger_type(app_data.debugger) !=
	debugger_type(initial_app_data.debugger))
	return true;

    return false;
}

static void ResetFontPreferencesCB(Widget, XtPointer, XtPointer)
{
    set_font(DefaultDDDFont,       initial_app_data.default_font);
    set_font(VariableWidthDDDFont, initial_app_data.variable_width_font);
    set_font(FixedWidthDDDFont,    initial_app_data.fixed_width_font);

    app_data.default_font_size = 
	initial_app_data.default_font_size;
    app_data.variable_width_font_size = 
	initial_app_data.variable_width_font_size;
    app_data.fixed_width_font_size = 
	initial_app_data.fixed_width_font_size;

    update_options();
}


static bool font_preferences_changed()
{
    if (string(app_data.default_font) != string(initial_app_data.default_font))
	return true;

    if (string(app_data.variable_width_font) != 
	string(initial_app_data.variable_width_font))
	return true;

    if (string(app_data.fixed_width_font) != 
	string(initial_app_data.fixed_width_font))
	return true;

    if (app_data.default_font_size != initial_app_data.default_font_size)
	return true;

    if (app_data.variable_width_font_size != 
	initial_app_data.variable_width_font_size)
	return true;

    if (app_data.fixed_width_font_size != 
	initial_app_data.fixed_width_font_size)
	return true;

    return false;
}

static void ResetHelpersPreferencesCB(Widget, XtPointer, XtPointer)
{
    set_string(edit_command_w,       initial_app_data.edit_command);
    set_string(get_core_command_w,   initial_app_data.get_core_command);
    set_string(ps_command_w,         initial_app_data.ps_command);
    set_string(term_command_w,       initial_app_data.term_command);
    set_string(uncompress_command_w, initial_app_data.uncompress_command);
    set_string(www_command_w,        initial_app_data.www_command);
}

static bool helpers_preferences_changed()
{
    if (string(app_data.edit_command) != string(initial_app_data.edit_command))
	return true;

    if (string(app_data.get_core_command) != 
	string(initial_app_data.get_core_command))
	return true;

    if (string(app_data.ps_command) != string(initial_app_data.ps_command))
	return true;

    if (string(app_data.term_command) != string(initial_app_data.term_command))
	return true;

    if (string(app_data.uncompress_command) != 
	string(initial_app_data.uncompress_command))
	return true;

    if (string(app_data.www_command) != string(initial_app_data.www_command))
	return true;

    return false;
}


static void ResetPreferencesCB(Widget w, XtPointer client_data, 
  			       XtPointer call_data)
{
    Widget panel = (Widget)client_data;
    string panel_name = XtName(panel);

    if (panel_name == "general")
	ResetGeneralPreferencesCB(w, client_data, call_data);
    else if (panel_name == "source")
	ResetSourcePreferencesCB(w, client_data, call_data);
    else if (panel_name == "data")
	ResetDataPreferencesCB(w, client_data, call_data);
    else if (panel_name == "layout")
	ResetLayoutPreferencesCB(w, client_data, call_data);
    else if (panel_name == "startup")
	ResetStartupPreferencesCB(w, client_data, call_data);
    else if (panel_name == "fonts")
	ResetFontPreferencesCB(w, client_data, call_data);
    else if (panel_name == "helpers")
	ResetHelpersPreferencesCB(w, client_data, call_data);
}

void update_reset_preferences()
{
    if (current_panel != 0 && reset_preferences_w != 0 && option_state_saved)
    {
	string panel_name = XtName(current_panel);

	bool sensitive = false;

	if (panel_name == "general")
	    sensitive = general_preferences_changed();
	else if (panel_name == "source")
	    sensitive = source_preferences_changed();
	else if (panel_name == "data")
	    sensitive = data_preferences_changed();
	else if (panel_name == "layout")
	    sensitive = layout_preferences_changed();
	else if (panel_name == "startup")
	    sensitive = startup_preferences_changed();
	else if (panel_name == "fonts")
	    sensitive = font_preferences_changed();
	else if (panel_name == "helpers")
	    sensitive = helpers_preferences_changed();

	set_sensitive(reset_preferences_w, sensitive);
    }
}

static void ChangePanelCB(Widget, XtPointer client_data, XtPointer call_data)
{
    Widget panel = (Widget)client_data;
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
    {
	// Manage this child
	XtManageChild(panel);
	XtAddCallback(preferences_dialog, XmNhelpCallback,
		      HelpOnThisCB, XtPointer(panel));
	XtAddCallback(reset_preferences_w, XmNactivateCallback,
		      ResetPreferencesCB, XtPointer(panel));
	current_panel = panel;

	update_reset_preferences();

	// Unmanage all other children
	WidgetList children;
	Cardinal num_children;
	XtVaGetValues(XtParent(panel), 
		      XmNchildren, &children,
		      XmNnumChildren, &num_children,
		      NULL);

	for (Cardinal i = 0; i < num_children; i++)
	{
	    Widget child = children[i];
	    if (child != panel)
	    {
		XtUnmanageChild(child);
		XtRemoveCallback(preferences_dialog, XmNhelpCallback,
				 HelpOnThisCB, XtPointer(child));
		XtRemoveCallback(reset_preferences_w, XmNactivateCallback,
				 ResetPreferencesCB, XtPointer(child));
	    }
	}
    }
}

static Widget add_panel(Widget parent, Widget buttons, 
			String name, MMDesc items[],
			Dimension& max_width, Dimension& max_height,
			bool set = false)
{
    Arg args[10];
    int arg;

    // Add two rows
    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    Widget form = verify(XmCreateRowColumn(parent, name, args, arg));
    XtManageChild(form);

    // Add panel
    Widget panel = MMcreatePanel(form, "panel", items);
    MMadjustPanel(items);
    MMaddCallbacks(items);
    MMaddHelpCallback(items, ImmediateHelpCB);
    XtManageChild(panel);
    register_menu_shell(items);

    // Fetch panel geometry
    XtWidgetGeometry size;
    size.request_mode = CWHeight | CWWidth;
    XtQueryGeometry(form, NULL, &size);
    max_width  = max(max_width,  size.width);
    max_height = max(max_height, size.height);

    // Add button
    arg = 0;
    Widget button = verify(XmCreateToggleButton(buttons, name, args, arg));
    XtManageChild(button);

    XtAddCallback(button, XmNvalueChangedCallback, ChangePanelCB, 
		  XtPointer(form));

    XmToggleButtonSetState(button, Boolean(set), False);

    if (set)
    {
	XmToggleButtonCallbackStruct cbs;
	cbs.set = set;
	ChangePanelCB(button, XtPointer(form), &cbs);
    }

    return button;
}

static void OfferRestartCB(Widget dialog, XtPointer, XtPointer)
{
    if (startup_preferences_changed() || font_preferences_changed())
    {
	static Widget restart_dialog = 0;
	if (restart_dialog == 0)
	{
	    restart_dialog = 
		verify(XmCreateQuestionDialog(find_shell(dialog), 
					      "restart_dialog", 0, 0));
	    Delay::register_shell(restart_dialog);
	    XtAddCallback(restart_dialog, XmNokCallback,
			  DDDRestartCB, 0);
	    XtAddCallback(restart_dialog, XmNhelpCallback,
			  ImmediateHelpCB, 0);
	}
	manage_and_raise(restart_dialog);
    }
}

// Create preferences dialog
static void make_preferences(Widget parent)
{
    Arg args[10];
    int arg;

    arg = 0;
    preferences_dialog = 
	verify(XmCreatePromptDialog(parent, "preferences", args, arg));
    Delay::register_shell(preferences_dialog);
    XtVaSetValues(preferences_dialog, XmNdefaultButton, Widget(0), NULL);
    XtAddCallback(preferences_dialog, XmNunmapCallback, OfferRestartCB, NULL);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(preferences_dialog,
					       XmDIALOG_APPLY_BUTTON));

    // Remove old prompt
    Widget text = XmSelectionBoxGetChild(preferences_dialog, XmDIALOG_TEXT);
    XtUnmanageChild(text);
    Widget old_label = 
	XmSelectionBoxGetChild(preferences_dialog, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(old_label);

    // Use cancel button to reset settings
    reset_preferences_w = 
        XmSelectionBoxGetChild(preferences_dialog, XmDIALOG_CANCEL_BUTTON);
    XtRemoveAllCallbacks(reset_preferences_w, XmNactivateCallback);

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    Widget box =
	verify(XmCreateRowColumn(preferences_dialog, "box", args, arg));
    XtManageChild(box);

    arg = 0;
    Widget buttons =
	verify(XmCreateRadioBox(box, "buttons", args, arg));
    XtManageChild(buttons);

    arg = 0;
    Widget frame = verify(XmCreateFrame(box, "frame", args, arg));
    XtManageChild(frame);

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    Widget change =
	verify(XmCreateRowColumn(frame, "change", args, arg));
    XtManageChild(change);

    Dimension max_width  = 0;
    Dimension max_height = 0;

    Widget general_button =
	add_panel(change, buttons, "general", general_preferences_menu, 
	      max_width, max_height, false);
    add_panel(change, buttons, "source",  source_preferences_menu,  
	      max_width, max_height, false);
    add_panel(change, buttons, "data",    data_preferences_menu,    
	      max_width, max_height, false);
    add_panel(change, buttons, "layout",  layout_preferences_menu,    
	      max_width, max_height, false);
    add_panel(change, buttons, "startup", startup_preferences_menu, 
	      max_width, max_height, false);
    add_panel(change, buttons, "fonts", font_preferences_menu,
	      max_width, max_height, false);
    add_panel(change, buttons, "helpers", helpers_preferences_menu, 
	      max_width, max_height, false);

    XtVaSetValues(change,
		  XmNwidth, max_width,
		  XmNheight, max_height,
		  XmNresizeWidth, False,
		  XmNresizeHeight, False,
		  NULL);

    XmToggleButtonSetState(general_button, True, True);
}

// Popup Preference Panel
static void dddPopupPreferencesCB (Widget, XtPointer, XtPointer)
{
    manage_and_raise(preferences_dialog);
}


//-----------------------------------------------------------------------------
// Create status line
//-----------------------------------------------------------------------------

static void create_status(Widget parent)
{
    Arg args[15];
    int arg = 0;
    XtSetArg(args[arg], XmNresizePolicy, XmRESIZE_ANY); arg++;
    Widget status_form = 
	verify(XmCreateForm(parent, "status_form", args, arg));
    XtManageChild(status_form);

    // Create LED
    arg = 0;
    XtSetArg(args[arg], XmNtopAttachment,      XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment,   XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNrightAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNresizable,          False); arg++;
    XtSetArg(args[arg], XmNfillOnSelect,       True); arg++;
    XtSetArg(args[arg], XmNset,                True); arg++;

    MString spaces("   ");
    if (lesstif_version < 1000)
    {
	XtSetArg(args[arg], XmNlabelString, spaces.xmstring()); arg++;
    }

    led_w = verify(XmCreateToggleButton(status_form, "led", args, arg));
    XtManageChild(led_w);

    XtAddCallback(led_w, XmNvalueChangedCallback, ToggleBlinkCB, XtPointer(0));

    Pixel arrow_foreground;
    XtVaGetValues(status_form, XmNbackground, &arrow_foreground, 0);

    // Create `Get more status messages' button
    arg = 0;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNresizable,        False); arg++;
    XtSetArg(args[arg], XmNshadowThickness,  0); arg++;
    XtSetArg(args[arg], XmNforeground,       arrow_foreground); arg++;
    XtSetArg(args[arg], XmNarrowDirection, 
	     (app_data.status_at_bottom ? XmARROW_UP : XmARROW_DOWN)); arg++;
    Widget arrow_w = 
	verify(XmCreateArrowButton(status_form, "arrow", args, arg));
    XtManageChild(arrow_w);

    // Give some `dummy' status message.  Some Motif versions limit
    // the size of the status window to the length of the very first
    // message, so we give some huge string at the beginning.
    MString short_msg = rm("Hello, world!");
    MString long_msg = short_msg + rm(replicate(' ', 90));

    arg = 0;
    XtSetArg(args[arg], XmNlabelString,      long_msg.xmstring()); arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_WIDGET); arg++;
    XtSetArg(args[arg], XmNleftWidget,       arrow_w); arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET); arg++;
    XtSetArg(args[arg], XmNrightWidget,      led_w); arg++;
    XtSetArg(args[arg], XmNresizable,        False); arg++;
    XtSetArg(args[arg], XmNrecomputeSize,    False); arg++;
    XtSetArg(args[arg], XmNshadowThickness,  0); arg++;
    status_w = verify(XmCreatePushButton(status_form, "status", args, arg));
    XtManageChild(status_w);

    // Initialize status history
    status_history_size = app_data.status_history_size;
    (void) status_history(parent);
    set_status("");
    set_status_mstring(short_msg);

    XtAddCallback(status_w, XmNarmCallback, 
		  PopupStatusHistoryCB, XtPointer(0));
    XtAddCallback(status_w, XmNdisarmCallback, 
		  PopdownStatusHistoryCB, XtPointer(0));

    XtAddCallback(arrow_w, XmNarmCallback, 
		  PopupStatusHistoryCB, XtPointer(0));
    XtAddCallback(arrow_w, XmNdisarmCallback, 
		  PopdownStatusHistoryCB, XtPointer(0));

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(status_w, NULL, &size);

    if (lesstif_version < 1000)
	XtVaSetValues(led_w, XmNindicatorSize, size.height - 4, NULL);
    else
	XtVaSetValues(led_w, XmNindicatorSize, size.height - 1, NULL);

    XtVaSetValues(arrow_w,
		  XmNheight, size.height - 2,
		  XmNwidth,  size.height - 2,
		  NULL);
    XtVaSetValues(status_form,
		  XmNpaneMaximum, size.height,
		  XmNpaneMinimum, size.height,
		  NULL);

    set_toggle(led_w, app_data.blink_while_busy);
    blink(true);
}


//-----------------------------------------------------------------------------
// Handle Status LED
//-----------------------------------------------------------------------------

static bool blinker_active        = false; // True iff status LED is active
static XtIntervalId blink_timer   = 0;     // Timer for blinking

static void BlinkCB(XtPointer client_data, XtIntervalId *id)
{
    (void) id;			// use it
    assert(*id == blink_timer);
    blink_timer = 0;

    static bool have_led_colors = false;
    static Pixel led_select_color;
    static Pixel led_background_color;

    if (!have_led_colors)
    {
	XtVaGetValues(led_w,
		      XmNbackground, &led_background_color,
		      XmNselectColor, &led_select_color,
		      NULL);
	have_led_colors = true;
    }

    bool set = bool(long(client_data));
    if (set)
	XtVaSetValues(led_w, XmNselectColor, led_select_color, NULL);
    else
	XtVaSetValues(led_w, XmNselectColor, led_background_color, NULL);

    XFlush(XtDisplay(led_w));
    XmUpdateDisplay(led_w);

    if ((blinker_active || set) && app_data.busy_blink_rate > 0)
    {
	blink_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(led_w),
				      app_data.busy_blink_rate, BlinkCB,
				      XtPointer(int(!set)));
    }
}

// Enable or disable blinking
static void blink(bool set)
{
    blinker_active = set;

    if (!XmToggleButtonGetState(led_w))
	return;			// Button is not active

    if (blink_timer == 0)
    {
	if (blinker_active)
	{
	    // Restart blink timer
	    BlinkCB(XtPointer(int(true)), &blink_timer);
	}
    }
}

static void DisableBlinkHP(Agent *, void *, void *)
{
    // GDB has died -- disable status LED
    XmToggleButtonSetState(led_w, False, False);
}

static void ToggleBlinkCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;
    string debugger_status_indicator =
	"Debugger status indicator ";

    app_data.blink_while_busy = info->set;

    if (info->set)
	set_status(debugger_status_indicator + "enabled.");
    else
	set_status(debugger_status_indicator + "disabled.");

    // Restart blinker
    blink(blinker_active);
}

//-----------------------------------------------------------------------------
// Handle Status History
//-----------------------------------------------------------------------------

const Dimension y_popup_offset = 5;

static Widget history_shell = 0;
static void PopupStatusHistoryCB(Widget w, XtPointer client_data, 
				 XtPointer call_data)
{
    (void) call_data;		// Use it
    (void) client_data;		// Use it

    history_shell = status_history(w);

    Position shell_x, shell_y;
    XtTranslateCoords(find_shell(status_w), 0, 0, &shell_x, &shell_y);

    Position status_x, status_y;
    XtTranslateCoords(status_w, 0, 0, &status_x, &status_y);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    Position x, y;
    if (app_data.status_at_bottom)
    {
	XtQueryGeometry(history_shell, NULL, &size);

	x = shell_x;
	y = status_y - size.height - y_popup_offset;
    }
    else
    {
	XtQueryGeometry(status_w, NULL, &size);

	x = shell_x;
	y = status_y + size.height + y_popup_offset;
    }

    XtVaSetValues(history_shell, XmNx, x, XmNy, y, XtPointer(0));
    XtPopup(history_shell, XtGrabNone);
}

static void PopdownStatusHistoryCB(Widget, XtPointer, XtPointer)
{
    if (history_shell != 0)
	XtPopdown(history_shell);
}



//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

void update_arg_buttons()
{
    string arg = source_arg->get_string();

    bool can_find = (arg != "") && !is_file_pos(arg) && 
	source_view->have_source();
    set_sensitive(arg_cmd_area[ArgItems::Find].widget, can_find);
    set_sensitive(find_forward_w, can_find);
    set_sensitive(find_backward_w, can_find);

    bool can_print = (arg != "") && !is_file_pos(arg);
    set_sensitive(arg_cmd_area[ArgItems::Print].widget, can_print);
    set_sensitive(arg_cmd_area[ArgItems::Display].widget, can_print);
    set_sensitive(print_w,   can_print);
    set_sensitive(display_w, can_print);

    set_sensitive(edit_source_w,   source_view->have_source());
    set_sensitive(reload_source_w, source_view->have_source());

    bool can_watch = can_print && gdb->has_watch_command();
    set_sensitive(arg_cmd_area[ArgItems::Watch].widget, can_watch);

    bool have_watch = have_watchpoint_at_arg();

    manage_child(watch_menu[WatchItems::Properties].widget, have_watch);
    manage_child(watch_menu[WatchItems::Enable].widget,     have_watch);
    manage_child(watch_menu[WatchItems::Sep].widget,        have_watch);

    set_sensitive(watch_menu[WatchItems::CWatch].widget, 
		  can_watch && 
		  (gdb->has_watch_command() & WATCH_CHANGE) == WATCH_CHANGE);
    set_sensitive(watch_menu[WatchItems::RWatch].widget,
		  can_watch &&
		  (gdb->has_watch_command() & WATCH_READ) == WATCH_READ);
    set_sensitive(watch_menu[WatchItems::AWatch].widget,
		  can_watch &&
		  (gdb->has_watch_command() & WATCH_ACCESS) == WATCH_ACCESS);
    if (have_watch)
    {
	set_label(arg_cmd_area[ArgItems::Watch].widget, 
		  "Unwatch ()", UNWATCH_ICON);
    }
    else
    {
	set_label(arg_cmd_area[ArgItems::Watch].widget, 
		  "Watch ()", WATCH_ICON);
    }

    bool watch_enabled = have_enabled_watchpoint_at_arg();
    if (watch_enabled)
	set_label(watch_menu[WatchItems::Enable].widget, 
		  "Disable Watchpoint on ()");
    else
	set_label(watch_menu[WatchItems::Enable].widget, 
		  "Enable Watchpoint at ()");


    bool have_break = have_breakpoint_at_arg();

    manage_child(break_menu[BreakItems::TempBreak].widget,   !have_break);
    manage_child(break_menu[BreakItems::RegexBreak].widget,  !have_break);
    manage_child(break_menu[BreakItems::ContUntil].widget,   !have_break);
    manage_child(break_menu[BreakItems::Sep2].widget,        !have_break);
    manage_child(break_menu[BreakItems::ClearAt2].widget,    !have_break);

    manage_child(break_menu[BreakItems::Properties].widget,  have_break);
    manage_child(break_menu[BreakItems::Enable].widget,      have_break);

    if (have_break)
    {
	set_label(arg_cmd_area[ArgItems::Break].widget, 
		  "Clear at ()", CLEAR_AT_ICON);
    }
    else
    {
	set_label(arg_cmd_area[ArgItems::Break].widget, 
		  "Break at ()", BREAK_AT_ICON);
    }

    bool break_enabled = have_enabled_breakpoint_at_arg();
    if (break_enabled)
	set_label(break_menu[BreakItems::Enable].widget, 
		  "Disable Breakpoint at ()");
    else
	set_label(break_menu[BreakItems::Enable].widget, 
		  "Enable Breakpoint at ()");

    set_sensitive(break_menu[BreakItems::ClearAt2].widget,
		  gdb->recording() || have_breakpoint_at_arg());
    set_sensitive(break_menu[BreakItems::Enable].widget,
		  gdb->can_enable());
    set_sensitive(break_menu[BreakItems::SetPC].widget,
		  gdb->has_jump_command() || gdb->has_assign_command());
    set_sensitive(break_menu[BreakItems::RegexBreak].widget,
		  gdb->type() == GDB);

    MString print_ref_label("Print " + gdb->dereferenced_expr("()"));
    XtVaSetValues(print_menu[PrintItems::PrintRef].widget,
		  XmNlabelString, print_ref_label.xmstring(),
		  NULL);

    MString disp_ref_label("Display " + gdb->dereferenced_expr("()"));
    XtVaSetValues(display_menu[DispItems::DispRef].widget,
		  XmNlabelString, disp_ref_label.xmstring(),
		  NULL);

}

// Arg changed - re-label buttons
static void source_argHP(void *, void *, void *)
{
    update_arg_buttons();
    data_disp->refresh_args();
}

// Language changed - re-label buttons
static void language_changedHP(Agent *, void *, void *)
{
    update_arg_buttons();
}


// Language changed - report it
static ostream& operator<< (ostream& os, ProgramLanguage lang)
{
    switch (lang)
    {
    case LANGUAGE_C:       os << "c/c++";         break;
    case LANGUAGE_JAVA:    os << "java";          break;
    case LANGUAGE_PASCAL:  os << "pascal/modula"; break;
    case LANGUAGE_ADA:     os << "ada";           break;
    case LANGUAGE_CHILL:   os << "chill";         break;
    case LANGUAGE_FORTRAN: os << "fortran";       break;
    case LANGUAGE_OTHER:   os << "(unknown)";     break;
    }

    return os;
}

static void report_languageHP(Agent *, void *, void *)
{
    ostrstream os;
    os << "Current language: " << gdb->program_language();
    set_status(string(os));
}


void update_user_buttons()
{
    set_buttons(data_buttons_w,    app_data.data_buttons);
    set_buttons(source_buttons_w,  app_data.source_buttons);
    set_buttons(console_buttons_w, app_data.console_buttons);
    set_shortcut_menu(data_disp,   app_data.display_shortcuts);
}


//-----------------------------------------------------------------------------
// Handlers
//-----------------------------------------------------------------------------

static void gdb_readyHP(Agent *, void *, void *call_data)
{
    bool gdb_ready = bool(long(call_data));
    if (gdb_ready)
    {
	if (!gdb_initialized)
	{
	    gdb_initialized = true;
	    XmTextSetEditable(gdb_w, true);
	}

	// Process next pending command as soon as we return
	if (!emptyCommandQueue())
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
			    processCommandQueue, XtPointer(0));

	// Check for mouse pointer grabs
	check_grabs(true);

	// Completion is done
	clear_completion_delay();

	// Selection is done
	if (gdb_selection_dialog != 0)
	    XtUnmanageChild(gdb_selection_dialog);

	// We don't exit and we don't restart
	ddd_is_exiting = ddd_is_restarting = false;

	if (app_data.uniconify_when_ready && userInteractionSeen())
	{
	    // Uniconify the command shell.  If `iconify all windows
	    // at once' is set, this also uniconifies the other windows.
	    uniconify_shell(command_shell);
	}
    }

    // Some stuff that must be executed every other time
    fix_status_size();
}


struct WhenReadyInfo {
    MString message;
    XtCallbackProc proc;
    XtPointer client_data;
    XmPushButtonCallbackStruct cbs;
    XEvent event;

    WhenReadyInfo(MString msg, XtCallbackProc p,
		  XtPointer cl_data,
		  const XmPushButtonCallbackStruct& c)
	: message(msg),
	  proc(p),
	  client_data(cl_data),
	  cbs(c)
    {
	// Copy event
	if (c.event == 0)
	{
	    // This happens with old LessTif versions
	}
	else
	{
	    memcpy(cbs.event, c.event, sizeof(cbs.event));
	    cbs.event = &event;
	}
    }
};

static void DoneCB(const string& /* answer */, void *qu_data)
{
    WhenReadyInfo *info = (WhenReadyInfo *)qu_data;
    set_status_mstring(info->message + rm("done."));

    (*info->proc)(gdb_w, info->client_data, XtPointer(&info->cbs));
    delete info;
}

// Execute command in (XtCallbackProc)CLIENT_DATA as soon as GDB gets ready
static void WhenReady(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    if (cbs == 0)
	return;	    // This happens with old LessTif versions

    XtCallbackProc proc = (XtCallbackProc)client_data;
    XtPointer user_client_data = 0; // No way to pass extra values here

    if (gdb->isReadyWithPrompt())
    {
	// GDB is ready: do command now
	proc(w, user_client_data, call_data);
	return;
    }

    // Execute command as soon as GDB gets ready
    XmString label = 0;
    XtVaGetValues(w, XmNlabelString, &label, NULL);
    MString _action(label, true);
    XmStringFree(label);
    string action = _action.str();
    if (action.contains("...", -1))
	action = action.before("...");

    MString msg = rm(action + ": waiting until " + gdb->title() 
		     + " gets ready...");
    WhenReadyInfo *info = new WhenReadyInfo(msg, proc, user_client_data, *cbs);

    // We don't want to lock the status, hence we use an ordinary
    // `set_status' call instead of the StatusMsg class.
    set_status_mstring(msg);

    Command c(gdb->nop_command(XtName(w)));
    c.origin   = w;
    c.callback = DoneCB;
    c.data     = (void *)info;
    c.verbose  = false;
    c.prompt   = false;
    c.check    = false;
    c.priority = COMMAND_PRIORITY_USER;

    gdb_command(c);
}


//-----------------------------------------------------------------------------
// I/O warnings
//-----------------------------------------------------------------------------

static void gdb_msgHP(Agent *source, void *call_data, char *name)
{
    if (source->pid() == 0)
    {
	// We're the child.  Don't use the parent's X display.
        return;
    }

    string msg = (char *)call_data;
    string path = source->path();
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    if (gdb != 0)
	path = downcase(gdb->title());
    post_error(path + ": " + msg, name);
}

static void gdb_panicHP(Agent *source, void *, void *call_data)
{
    gdb_msgHP(source, call_data, "gdb_io_error");
}

static void gdb_strangeHP(Agent *source, void *, void *call_data)
{
    gdb_msgHP(source, call_data, "gdb_io_warning");
}

static void gdb_echo_detectedHP(Agent *, void *, void *)
{
    static bool tried = false;

    if (tried)
	return;

    // Here is a more subtle warning.
    set_status(gdb->title() + " is running in echo mode.");

    // Attempt to disable echo mode explicitly via stty command.
    gdb_command(gdb->shell_command("stty -echo -onlcr"), 0, 0, 0, 
		false, false, COMMAND_PRIORITY_AGAIN);

    tried = true;
}

static void gdb_recordingHP(Agent *, void *, void *call_data)
{
    static StatusMsg *recording_msg = 0;
    bool recording = bool(long(call_data));

    delete recording_msg;
    recording_msg = 0;

    if (recording && emptyCommandQueue())
	recording_msg = new StatusMsg("Recording commands");

    // Recording state affects toolbar button sensitivity
    update_arg_buttons();
    data_disp->refresh_args();
}



//-----------------------------------------------------------------------------
// Output
//-----------------------------------------------------------------------------

// Return index of first control character; -1 if not found
static int index_control(const string& text)
{
    for (unsigned i = 0; i < text.length(); i++)
    {
	switch (text[i])
	{
	case '\0':		// NUL
	case '\001':		// SOH
	case '\002':		// STX
	case '\003':		// ETX
	case '\004':		// EOT
	case '\005':		// ENQ
	case '\006':		// ACK
	case '\007':		// BEL
	case '\010':		// BS
	case '\011':		// HT
     // case '\012':		// NL
	case '\013':		// VT
	case '\014':		// NP
	case '\015':		// CR
	case '\016':		// SO
	case '\017':		// SI
	case '\020':		// DLE
	case '\021':		// DC1
	case '\022':		// DC2
	case '\023':		// DC3
	case '\024':		// DC4
	case '\025':		// NAK
	case '\026':		// SYN
	case '\027':		// ETB
	case '\030':		// CAN
	case '\031':		// EM
	case '\032':		// SUB
	case '\033':		// ESC
	case '\034':		// FS
	case '\035':		// GS
	case '\036':		// RS
	case '\037':		// US
	case '\177':		// DEL
	    return i;
	}
    }

    return -1;
}

// Process control character
static void gdb_ctrl(char ctrl)
{
    switch (ctrl)
    {
    case '\t':
    case '\r':
    {
	String s = XmTextGetString(gdb_w);
	string message = s;
	XtFree(s);

	XmTextPosition startOfLine = min(promptPosition, message.length());
	while (startOfLine - 1 >= 0 && message[startOfLine - 1] != '\n')
	    startOfLine--;

	switch (ctrl)
	{
	case '\t':
	{
	    // Go to next tab position
	    const int TAB_WIDTH = 8;
	    int column = promptPosition - startOfLine;
	    int spaces = TAB_WIDTH - column % TAB_WIDTH;
	    string spacing = replicate(' ', spaces);
	
	    XmTextInsert(gdb_w, promptPosition, (String)spacing);
	    promptPosition += spacing.length();
	}
	break;
		
	case '\r':
	{
	    // Erase last line
	    XmTextReplace(gdb_w, startOfLine, promptPosition, "");
	    promptPosition = startOfLine;
	}
	break;
	}
	break;
    }

    case '\b':
    {
	// Erase last character
	XmTextReplace(gdb_w, promptPosition - 1, promptPosition, "");
	promptPosition--;
    }
    break;

    case '\n':
    {
	string c = ctrl;
	XmTextInsert(gdb_w, promptPosition, (String)c);
	promptPosition += c.length();

	// Flush output
	XmTextShowPosition(gdb_w, promptPosition);
    }
    break;

    default:
    {
	// Issue control character
	string c;
	if (ctrl < ' ')
	    c = string("^") + char('@' + ctrl);
	else
	    c = "^?";
	XmTextInsert(gdb_w, promptPosition, (String)c);
	promptPosition += c.length();
    }
    break;
    }

    // XmTextShowPosition(gdb_w, promptPosition);
}


// Append TEXT to GDB output
void _gdb_out(const string& txt)
{
    if (txt == "")
	return;
    if (private_gdb_output)
	return;

    string text(txt);
    gdb_input_at_prompt = gdb->ends_with_prompt(text);

    if (promptPosition == 0)
	promptPosition = XmTextGetLastPosition(gdb_w);

    private_gdb_output = true;

    // Don't care for strings to be ignored
    static string empty;
    if (gdb_out_ignore != "")
	text.gsub(gdb_out_ignore, empty);

    // Pass TEXT to various functions
    set_buttons_from_gdb(console_buttons_w, text);
    set_buttons_from_gdb(source_buttons_w, text);
    set_buttons_from_gdb(data_buttons_w, text);
    set_buttons_from_gdb(command_toolbar_w, text);
    set_status_from_gdb(text);
    set_tty_from_gdb(text);

    // Output TEXT on TTY
    tty_out(text);

    // Output TEXT in debugger console
    bool line_buffered = app_data.line_buffered_console;
    static bool cr_pending = false;
    do {
	char ctrl      = '\0';
	bool have_ctrl = false;

	if (pending_interaction())
	{
	    process_emergencies();
	    line_buffered = false;
	}

	string block = text;

	int i = -1;
	if (line_buffered)
	    i = block.index('\n');
	if (i < 0)
	    i = index_control(block);

	if (i >= 0)
	{
	    ctrl      = block[i];
	    have_ctrl = true;
	    block     = block.before(i);
	    text      = text.after(i);
	}
	else
	{
	    // All done
	    text = "";
	}

	if (block.length() > 0)
	{
	    if (cr_pending)
	    {
		gdb_ctrl('\r');
		cr_pending = false;
	    }

	    XmTextInsert(gdb_w, promptPosition, (String)block);
	    promptPosition += block.length();
	    // XmTextShowPosition(gdb_w, promptPosition);
	}

	if (have_ctrl)
	{
	    if (ctrl == '\r')
	    {
		// Don't output CR immediately, but wait for the next
		// character.  If it's a NL, ignore the CR.
		cr_pending = true;
	    }
	    else
	    {
		if (cr_pending)
		{
		    if (ctrl == '\n')
		    {
			// Ignore CR followed by NL
		    }
		    else
		    {
			gdb_ctrl('\r');
		    }
		    cr_pending = false;
		}
		gdb_ctrl(ctrl);
	    }
	}
    } while (text != "");

    XmTextPosition lastPos = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, lastPos);
    XmTextShowPosition(gdb_w, lastPos);

    private_gdb_output = false;
}


void gdb_out(const string& text)
{
    if (private_gdb_input)
	return;

    _gdb_out(text);
}




//-----------------------------------------------------------------------------
// Cut/Copy/Paste
//-----------------------------------------------------------------------------

static DDDWindow ddd_window(XtPointer client_data)
{
    if (source_view_shell == 0 && data_disp_shell == 0)
	return CommonWindow;
    else
	return DDDWindow(long(client_data));
}

static void gdbCutSelectionCB(Widget w, XtPointer client_data, 
			      XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    DDDWindow win = ddd_window(client_data);
    Boolean success = False;
    Widget dest = XmGetDestination(XtDisplay(w));

    // Try data arg
    if (!success && (win == DataWindow || win == CommonWindow))
    {
	if (data_disp->have_selection())
	{
	    success = XmTextCopy(DataDisp::graph_selection_w, tm);
	    if (success)
		DataDisp::deleteCB(w, client_data, call_data);
	}
    }

    // Try destination window
    if (!success && dest != 0 && XmIsText(dest))
	success = XmTextCut(dest, tm);
    if (!success && dest != 0 && XmIsTextField(dest))
	success = XmTextFieldCut(dest, tm);

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow))
	success = XmTextCut(gdb_w, tm);

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextFieldCut(source_arg->text(), tm);

    if (success)
	gdbUnselectAllCB(w, client_data, call_data);
}

static void gdbCopySelectionCB(Widget w, XtPointer client_data, 
			       XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);
    
    DDDWindow win = ddd_window(client_data);
    Boolean success = False;
    Widget dest = XmGetDestination(XtDisplay(w));

    // Try data arg
    if (!success && (win == DataWindow || win == CommonWindow))
	if (data_disp->have_selection())
	{
	    success = XmTextCopy(DataDisp::graph_selection_w, tm);
	}

    // Try destination window
    if (!success && dest != 0 && XmIsText(dest))
	success = XmTextCopy(dest, tm);
    if (!success && dest != 0 && XmIsTextField(dest))
	success = XmTextFieldCopy(dest, tm);

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow))
	success = XmTextCopy(gdb_w, tm);

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextFieldCopy(source_arg->text(), tm);

    // Try source
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextCopy(source_view->source(), tm);

    // Try code
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextCopy(source_view->code(), tm);
}

static void gdbPasteClipboardCB(Widget w, XtPointer client_data, XtPointer)
{
    DDDWindow win = ddd_window(client_data);

    // Try destination window
    bool success = false;
    Widget dest = XmGetDestination(XtDisplay(w));
    if (dest != 0)
    {
	if (dest == DataDisp::graph_selection_w)
	{
	    // Don't paste into graph selection; paste into GDB
	    // console instead
	    dest = gdb_w;
	}

	if (!success && XmIsText(dest))
	    success = XmTextPaste(dest);
	if (!success && XmIsTextField(dest))
	    success = XmTextFieldPaste(dest);
    }
    if (success)
	return;

    switch (win)
    {
    case ToolWindow:
	// Cannot paste into command tool
	break;

    case GDBWindow:
    case CommonWindow:
    case DataWindow:
	XmTextPaste(gdb_w);
	break;

    case SourceWindow:
	XmTextFieldPaste(source_arg->text());
	break;

    default:
	// Cannot paste into exec window
	break;
    }
}

static void gdbUnselectAllCB(Widget w, XtPointer client_data,
			     XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    XmTextClearSelection(gdb_w, tm);
    XmTextFieldClearSelection(source_arg->text(), tm);
    XmTextClearSelection(source_view->source(), tm);
    XmTextClearSelection(source_view->code(), tm);

    if (data_disp->graph_arg != 0)
	XmTextFieldClearSelection(data_disp->graph_arg->text(), tm);
    if (data_disp->graph_selection_w != 0)
	XmTextClearSelection(data_disp->graph_selection_w, tm);

    DataDisp::unselectAllCB(w, client_data, call_data);
}

static void gdbClearAllCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    gdbUnselectAllCB(w, client_data, call_data);
    source_arg->set_string("");
    gdbClearCB(w, client_data, call_data);
}

static void gdbSelectAllCB(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
    DDDWindow win = ddd_window(client_data);

    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    bool success = false;
    Widget dest = XmGetDestination(XtDisplay(w));
    if (dest != 0)
    {
	if (!success && dest == DataDisp::graph_selection_w)
	{
	    DataDisp::selectAllCB(w, client_data, call_data);
	    success = true;
	}
	if (!success && XmIsText(dest))
	{
	    XmTextSetSelection(dest, 0, XmTextGetLastPosition(dest), tm);
	    success = true;
	}
	if (!success && XmIsTextField(dest))
	{
	    XmTextFieldSetSelection(dest, 0, 
				    XmTextFieldGetLastPosition(dest), tm);
	    success = true;
	}
    }
    if (success)
	return;

    switch (win)
    {
    case SourceWindow:
    case CommonWindow:
    {
	Widget w = source_arg->text();
	XmTextFieldSetSelection(w, 0, XmTextFieldGetLastPosition(w), tm);
	break;
    }

    case DataWindow:
	DataDisp::selectAllCB(w, client_data, call_data);
	break;

    case GDBWindow:
	XmTextSetSelection(gdb_w, promptPosition, 
			   XmTextGetLastPosition(gdb_w), tm);
	break;

    default:
	break;
    }
}


static void gdbDeleteSelectionCB(Widget w, XtPointer client_data, 
				 XtPointer call_data)
{
    DDDWindow win = ddd_window(client_data);
    Boolean success = False;
    Widget dest = XmGetDestination(XtDisplay(w));

    // Try data display
    if (!success && dest == DataDisp::graph_selection_w)
    {
	if (data_disp->have_selection())
	{
	    DataDisp::deleteCB(w, client_data, call_data);
	    success = true;
	}
    }

    // Try destination window
    if (!success && dest != 0 && XmIsText(dest))
	success = XmTextRemove(dest);
    if (!success && dest != 0 && XmIsTextField(dest))
	success = XmTextFieldRemove(dest);

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow))
	success = XmTextRemove(gdb_w);

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextFieldRemove(source_arg->text());

    // Try data display
    if (!success && (win == DataWindow || win == CommonWindow))
    {
	if (data_disp->have_selection())
	{
	    DataDisp::deleteCB(w, client_data, call_data);
	    success = true;
	}
    }

    if (success)
	gdbUnselectAllCB(w, client_data, call_data);
}

// Update cut/copy/paste bindings
static void set_cut_copy_paste_bindings(MMDesc *menu, BindingStyle style)
{
    if (menu == 0 || menu[0].widget == 0)
	return;

#ifdef LESSTIF_VERSION
    // This only works in Lesstif.
    switch (style)
    {
    case KDEBindings:
    {
	MString cut("Ctrl+X");
	MString copy("Ctrl+C");
	MString paste("Ctrl+V");

	XtVaSetValues(menu[EditItems::Cut].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>X",
		      XmNacceleratorText, cut.xmstring(),
		      NULL);
	XtVaSetValues(menu[EditItems::Copy].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>C",
		      XmNacceleratorText, copy.xmstring(),
		      NULL);
	XtVaSetValues(menu[EditItems::Paste].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>V",
		      XmNacceleratorText, paste.xmstring(),
		      NULL);
	break;
    }

    case MotifBindings:
    {
	MString cut("Shift+Del");
	MString copy("Ctrl+Ins");
	MString paste("Shift+Ins");

	XtVaSetValues(menu[EditItems::Cut].widget,
		      XmNaccelerator, "~Ctrl Shift<Key>Delete",
		      XmNacceleratorText, cut.xmstring(),
		      NULL);
	XtVaSetValues(menu[EditItems::Copy].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>Insert",
		      XmNacceleratorText, copy.xmstring(),
		      NULL);
	XtVaSetValues(menu[EditItems::Paste].widget,
		      XmNaccelerator, "~Ctrl Shift<Key>Insert",
		      XmNacceleratorText, paste.xmstring(),
		      NULL);
	break;
    }
    }
#else
    (void) style;
#endif
}

static void setup_cut_copy_paste_bindings(XrmDatabase db)
{
#ifdef LESSTIF_VERSION
    // In Lesstif, we can change accelerators at run-time.  See
    // set_cut_copy_paste_bindings(), above.
    (void) db;
#else
    // Stupid OSF/Motif won't change the accelerators once created.
    // Set resources explicitly.
    String resources = 0;
    switch (app_data.cut_copy_paste_bindings)
    {
    case KDEBindings:
	resources =
	    "*editMenu.cut.acceleratorText: Ctrl+X\n"
	    "*editMenu.copy.acceleratorText: Ctrl+C\n"
	    "*editMenu.paste.acceleratorText: Ctrl+V\n"
	    "*editMenu.cut.accelerator: ~Shift Ctrl<Key>X\n"
	    "*editMenu.copy.accelerator: ~Shift Ctrl<Key>C\n"
	    "*editMenu.paste.accelerator: ~Shift Ctrl<Key>V\n";
	break;

    case MotifBindings:
	resources =
	    "*editMenu.cut.acceleratorText: Shift+Del\n"
	    "*editMenu.copy.acceleratorText: Ctrl+Ins\n"
	    "*editMenu.paste.acceleratorText: Shift+Ins\n"
	    "*editMenu.cut.accelerator: ~Ctrl Shift<Key>Delete\n"
	    "*editMenu.copy.accelerator: ~Shift Ctrl<Key>Insert\n"
	    "*editMenu.paste.accelerator: ~Ctrl Shift<Key>Insert\n";
	break;
    }

    XrmDatabase bindings = XrmGetStringDatabase(resources);
    assert(bindings != 0);
    XrmMergeDatabases(bindings, &db);
#endif
}

//-----------------------------------------------------------------------------
// Update menu entries
//-----------------------------------------------------------------------------

static void gdbUpdateEditCB(Widget w, XtPointer client_data, XtPointer)
{
    DDDWindow win = ddd_window(client_data);

    // Fetch menu
    MMDesc *menu = 0;
    switch (win)
    {
    case GDBWindow:
    case CommonWindow:
	menu = command_edit_menu;
	break;

    case SourceWindow:
	menu = source_edit_menu;
	break;

    case DataWindow:
	menu = data_edit_menu;
	break;

    default:
	break;
    }

    if (menu == 0 || menu[0].widget == 0)
	return;

    // Check if we have something to cut
    XmTextPosition start, end;
    bool can_cut = false;
    Widget dest  = XmGetDestination(XtDisplay(w));

    // Try destination window
    if (!can_cut && dest != 0 && XmIsText(dest))
	can_cut = XmTextGetSelectionPosition(dest, &start, &end);
    if (!can_cut && dest != 0 && XmIsTextField(dest))
	can_cut = XmTextFieldGetSelectionPosition(dest, &start, &end);

    // Try debugger console
    if (!can_cut && (win == GDBWindow || win == CommonWindow))
	can_cut = XmTextGetSelectionPosition(gdb_w, &start, &end);

    // Try source arg
    if (!can_cut && (win == SourceWindow || win == CommonWindow))
	can_cut = XmTextFieldGetSelectionPosition(source_arg->text(), 
						  &start, &end);

    // Try data display
    if (!can_cut && (win == DataWindow || win == CommonWindow))
	can_cut = data_disp->have_selection();

    // Check if we have something to copy
    bool can_copy = can_cut;

    // Try source
    if (!can_copy && (win == SourceWindow || win == CommonWindow))
	can_copy = XmTextGetSelectionPosition(source_view->source(),
					      &start, &end);

    // Try code
    if (!can_copy && (win == SourceWindow || win == CommonWindow))
	can_copy = XmTextGetSelectionPosition(source_view->code(),
					      &start, &end);

    // There is always something to paste
    bool can_paste = true;
    set_sensitive(menu[EditItems::Cut].widget,    can_cut);
    set_sensitive(menu[EditItems::Copy].widget,   can_copy);
    set_sensitive(menu[EditItems::Paste].widget,  can_paste);
    set_sensitive(menu[EditItems::Delete].widget, can_cut);
}

static void gdbUpdateFileCB(Widget, XtPointer client_data, XtPointer)
{
    MMDesc *file_menu = (MMDesc *)client_data;
    if (file_menu == 0 || file_menu[0].widget == 0)
	return;

    // Check whether we can print something
    Graph *graph = graphEditGetGraph(data_disp->graph_edit);
    Boolean can_print = (graph->firstNode() != 0);
    set_sensitive(file_menu[FileItems::Print].widget,      can_print);
    set_sensitive(file_menu[FileItems::PrintAgain].widget, can_print);

    // Check whether we can close something
    Boolean can_close = (running_shells() > 1);
    set_sensitive(file_menu[FileItems::Close].widget, can_close);

#if 0
    // If we have only one window, remove the `Close' item
    Boolean one_window = 
	!app_data.separate_source_window && !app_data.separate_data_window;
    manage_child(file_menu[FileItems::Close].widget, !one_window);
#endif
}

static void gdbUpdateViewCB(Widget, XtPointer client_data, XtPointer)
{
    MMDesc *view_menu = (MMDesc *)client_data;
    if (view_menu == 0 || view_menu[0].widget == 0)
	return;

    set_sensitive(view_menu[CodeWindow].widget, gdb->type() == GDB);
    set_sensitive(view_menu[ExecWindow].widget, gdb->has_redirection());
}

static void gdbUpdateViewsCB(Widget w, XtPointer client_data, 
			     XtPointer call_data)
{
    gdbUpdateViewCB(w, client_data, call_data);

    MMDesc *view_menu = (MMDesc *)client_data;
    if (view_menu == 0 || view_menu[0].widget == 0)
	return;

    set_toggle(view_menu[DataWindow].widget,   have_data_window());
    set_toggle(view_menu[SourceWindow].widget, have_source_window());
    set_toggle(view_menu[CodeWindow].widget,   app_data.disassemble);
    set_toggle(view_menu[GDBWindow].widget,    have_command_window());
}

// In case we have tear-off menus, all these menus must be updated at
// all times.
static void gdbUpdateAllMenus()
{
    gdbUpdateEditCB(gdb_w, XtPointer(GDBWindow), NULL);
    gdbUpdateEditCB(gdb_w, XtPointer(SourceWindow), NULL);
    gdbUpdateEditCB(gdb_w, XtPointer(DataWindow), NULL);
    gdbUpdateFileCB(gdb_w, XtPointer(command_file_menu), NULL);
    gdbUpdateFileCB(gdb_w, XtPointer(source_file_menu), NULL);
    gdbUpdateFileCB(gdb_w, XtPointer(data_file_menu), NULL);

    gdbUpdateViewsCB(gdb_w, XtPointer(views_menu), NULL);
}


//-----------------------------------------------------------------------------
// Configure new shell
//-----------------------------------------------------------------------------

static void setup_new_shell(Widget w)
{
    if (w == 0)
	return;

    Widget shell = findShellParent(w);
    if (shell == 0 && XtIsWidget(w) && XtIsShell(w))
	shell = w;
    if (shell == 0)
	return;

#if HAVE_X11_XMU_EDITRES_H
    // Make the shell handle EditRes messages
    XtRemoveEventHandler(shell, EventMask(0), true,
			 XtEventHandler(_XEditResCheckMessages), NULL);
    XtAddEventHandler(shell, EventMask(0), true,
		      XtEventHandler(_XEditResCheckMessages), NULL);
#endif

    // Use DDD logo as icon of the new shell
    if (shell != 0 && XtIsRealized(shell))
	wm_set_icon(shell, iconlogo(w), iconmask(w));
}

//-----------------------------------------------------------------------------
// Misc functions
//-----------------------------------------------------------------------------

// Xt Warning handler
static void ddd_xt_warning(String message)
{
    if (!app_data.suppress_warnings)
    {
	ddd_original_xt_warning_handler(message);

	static bool informed = false;

	if (!informed)
	{
	    cerr << "(Annoyed?  "
		"Try 'Edit | Preferences | General | Suppress X Warnings'!)\n";
	    informed = true;
	}
    }

    // With auto-raised menus, Motif may fail to ungrab the pointer
    // due to the interference with DDD auto-raise.  In this case,
    // Motif issues a warning that it cannot grab the pointer (and
    // leaves the pointer *grabbed*).  Hence, ungrab it explictly.
    if (gdb_initialized)
    {
	XUngrabPointer(XtDisplay(gdb_w), CurrentTime);
	XFlush(XtDisplay(gdb_w));
    }
}




//-----------------------------------------------------------------------------
// Decoration
//-----------------------------------------------------------------------------

static Widget init_label, init_shell;

// Return a transient position on SCREEN (for command tool etc.) in POS_X/POS_Y
static void get_transient_pos(Screen *screen, Position& pos_x, Position& pos_y)
{
    (void) screen;		// Use it
#if 0
    // Use lower right corner.
    pos_x = WidthOfScreen(screen) - 1;
    pos_y = HeightOfScreen(screen) - 1;
#else
    // This loses on some window managers; upper left corner is safer.
    pos_x = 0;
    pos_y = 0;
#endif
}

// Check if window manager decorates transients
static void start_have_decorated_transients(Widget parent)
{
    Position pos_x, pos_y;
    get_transient_pos(XtScreen(parent), pos_x, pos_y);

    ostrstream os;
    os << "+" << pos_x << "+" << pos_y;
    string geometry(os);

    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNgeometry, geometry.chars()); arg++;
    XtSetArg(args[arg], XmNx, pos_x);                   arg++;
    XtSetArg(args[arg], XmNy, pos_y);                   arg++;
    init_shell = verify(XmCreateDialogShell(parent, "init_shell", args, arg));

    arg = 0;
    MString label(DDD_NAME " " DDD_VERSION);
    XtSetArg(args[arg], XmNlabelString, label.xmstring()); arg++;
    init_label = verify(XmCreateLabel(init_shell, ddd_NAME, args, arg));
    XtManageChild(init_label);

    wait_until_mapped(init_label, init_shell);
    XmUpdateDisplay(init_label);
}

static bool have_decorated_transients()
{
    if (init_label == 0 || init_shell == 0)
	start_have_decorated_transients(command_shell);

    XWindowAttributes shell_attributes;
    XGetWindowAttributes(XtDisplay(init_shell), XtWindow(init_shell), 
			 &shell_attributes);

#if 0
    clog << "shell window: " << XtWindow(init_shell)
	 << ", size: " << BoxPoint(shell_attributes.width, 
				       shell_attributes.height) << "\n";
#endif

    // Wait up to 5 seconds until WM has decorated the init shell.
    // Problem: If we have no WM or a non-decorating WM, this delays
    // DDD for 5 seconds; this can be avoided by using an explicit
    // `decorateTool' resource value.
    Window frame_window = 0;
    for (int trial = 1; trial < 5; trial++)
    {
	frame_window = frame(XtDisplay(init_shell), XtWindow(init_shell));
	if (frame_window != XtWindow(init_shell))
	    break;
	XSync(XtDisplay(init_label), False);
	sleep(1);
    }

    XWindowAttributes frame_attributes;
    XGetWindowAttributes(XtDisplay(init_shell), frame_window,
			 &frame_attributes);

#if 0
    clog << "frame window: " << frame_window 
	 << ", size: " << BoxPoint(frame_attributes.width, 
				       frame_attributes.height) << "\n";
#endif

    XtUnmapWidget(init_shell);
    DestroyWhenIdle(init_shell);

    // If the border supplied by the window manager border is more
    // than 5 pixels higher than wider, assume we have some kind of
    // title bar - the shell is decorated.
    int border_height = frame_attributes.height - shell_attributes.height;
    int border_width  = frame_attributes.width  - shell_attributes.width;

    return border_height - border_width > 5;
}


//-----------------------------------------------------------------------------
// Splash Screen
//-----------------------------------------------------------------------------

static Widget splash_shell  = 0;
static Pixmap splash_pixmap;
static _Delay *splash_delay = 0;

static void popdown_splash_screen(XtPointer data, XtIntervalId *id)
{
    (void) id;			// use it

    if (data != 0)
    {
	XtIntervalId *timer = (XtIntervalId *)data;
	assert(*timer == *id);
	*timer = 0;
    }
    
    if (splash_shell != 0)
    {
	XFreePixmap(XtDisplay(splash_shell), splash_pixmap);

	popdown_shell(splash_shell);
	DestroyWhenIdle(splash_shell);
	splash_shell = 0;

	delete splash_delay;
	splash_delay = 0;
    }
}

static void popup_splash_screen(Widget parent, string color_key)
{
    popdown_splash_screen();

    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNallowShellResize, True); arg++;
    XtSetArg(args[arg], XmNborderWidth, 0); arg++;
    splash_shell = verify(XtCreatePopupShell("splash_shell", 
					     overrideShellWidgetClass, 
					     parent, args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNlabelType, XmPIXMAP);   arg++;
    XtSetArg(args[arg], XmNallowResize, True);     arg++;
    XtSetArg(args[arg], XmNborderWidth, 0);        arg++;
    XtSetArg(args[arg], XmNmarginWidth, 0);        arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0);       arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    Widget splash = verify(XmCreateLabel(splash_shell, "splash", args, arg));
    XtManageChild(splash);
    XtRealizeWidget(splash_shell);

    splash_delay = new _Delay(splash_shell);

    splash_pixmap = dddsplash(splash, color_key);
    XtVaSetValues(splash, XmNlabelPixmap, splash_pixmap, NULL);

    Dimension width, height;
    XtVaGetValues(splash_shell, XmNwidth, &width, XmNheight, &height, NULL);

    int x = (WidthOfScreen(XtScreen(splash_shell)) - width) / 2;
    int y = (HeightOfScreen(XtScreen(splash_shell)) - height) / 2;

    XtVaSetValues(splash_shell, XmNx, x, XmNy, y, NULL);

    // Place lock warning on top of startup splash
    lock_dialog_x = x + 20;
    lock_dialog_y = y + 20;

    popup_shell(splash_shell);
    wait_until_mapped(splash, splash_shell);
}

static void SetSplashScreenCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.splash_screen = info->set;

    update_options();
}


//-----------------------------------------------------------------------------
// GDB status
//-----------------------------------------------------------------------------

static StatusMsg *running_msg = 0;

static void ReportStartHP(Agent *agent, void *, void *)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, agent);
    if (gdb == 0)
	return;

    if (running_msg != 0)
	delete running_msg;

    running_msg = new StatusMsg("Running " + gdb->title() + 
				" (pid " + itostring(gdb->pid()) + 
				", tty " + gdb->slave_tty() + ")");
    unlock_status();
}

static void ReportDeathHP(Agent *agent, void *, void *call_data)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, agent);
    if (gdb == 0)
	return;

    char *reason = (char *)call_data;
    if (gdb != 0 && running_msg != 0)
    {
	running_msg->outcome = reason;
	delete running_msg;
	running_msg = 0;
    }
}


//-----------------------------------------------------------------------------
// Emergency
//-----------------------------------------------------------------------------

static Bool is_emergency(Display *, XEvent *event, char *)
{
    switch (event->type)
    {
    case KeyPress:
    {
	char buffer[1024];
	KeySym keysym;

	int len = XLookupString(&event->xkey, buffer, sizeof buffer, 
				&keysym, NULL);
	if (len == 1)
	{
	    char c = buffer[0];
	    if (c== '\003' || c == '\007' || c == '\033' || c == '\034')
	    {
		// Interrupt: ^C, ^G, ^\, or ESC found in queue
		return True;
	    }
	}
    }
    }

    return False;
}

bool process_emergencies()
{
    XEvent event;
    if (XCheckIfEvent(XtDisplay(gdb_w), &event, is_emergency, 0))
    {
	// Emergency: process this event immediately
	XtDispatchEvent(&event);
	return true;
    }

    return false;
}


//----------------------------------------------------------------------------
// Synchronize argument fields
//----------------------------------------------------------------------------

static void SyncArgHP(void *src, void *client_data, void *)
{
    ArgField *source = (ArgField *)src;
    ArgField *target = (ArgField *)client_data;

    target->removeHandler(Changed, SyncArgHP, (void *)source);
    target->set_string(source->get_string());
    target->addHandler(Changed, SyncArgHP, (void *)source);
}

static void sync_args(ArgField *source, ArgField *target)
{
    if (source == 0 || target == 0)
	return;

    target->set_string(source->get_string());

    source->addHandler(Changed, SyncArgHP, (void *)target);
    target->addHandler(Changed, SyncArgHP, (void *)source);
}




//-----------------------------------------------------------------------------
// Log player stuff
//-----------------------------------------------------------------------------

static void check_log(const string& logname, DebuggerType& type)
{
    ifstream log(logname);
    if (log.bad())
    {
	(void) fopen(logname, "r");
	perror(logname);
	exit(EXIT_FAILURE);
    }

    while (log)
    {
	char buffer[1024];
	log.getline(buffer, sizeof buffer);
	string log_line(buffer);

	if (log_line.contains("+  ", 0))
	{
	    type = debugger_type(log_line);
	    return;
	}
    }
}

//-----------------------------------------------------------------------------
// Various setups
//-----------------------------------------------------------------------------

// This text is shown in `About DDD', appended to the value of the
// `helpOnVersionString' resource.
static void setup_version_info()
{
    ostrstream os;
    show_configuration(os);
    string cinfo(os);

    cinfo.gsub("(C)", "\251");

    // Set e-mail address in @tt; the remainder in @rm
    int cinfo_lt = cinfo.index('<', -1);
    int cinfo_gt = cinfo.index('>', -1);
    if (cinfo_lt >= 0 && cinfo_gt >= 0)
    {
	helpOnVersionExtraText = rm(cinfo.before(cinfo_lt));
 	helpOnVersionExtraText += cr();	// place e-mail on separate line
	helpOnVersionExtraText += rm(cinfo(cinfo_lt, 1));
	helpOnVersionExtraText += 
	    tt(cinfo(cinfo_lt + 1, cinfo_gt - cinfo_lt - 1));
	helpOnVersionExtraText += rm(cinfo.from(cinfo_gt));
    }
    else
    {
	helpOnVersionExtraText = rm(cinfo);
    }

    string expires = ddd_expiration_date();
    if (expires != "")
    {
	// Add expiration date
	string expired_msg = DDD_NAME " " DDD_VERSION " ";
	if (ddd_expired())
	    expired_msg += "has expired since " + expires;
	else
	    expired_msg += "expires " + expires;

	helpOnVersionExtraText += rm(expired_msg + ".") + cr();
    }

    helpOnVersionExtraText += cr() 
	+ rm(DDD_NAME " is ") + sl("free software")
	+ rm(" and you are welcome to distribute copies of it") + cr()
	+ rm("under certain conditions; select ")
	+ bf("Help") + rm(" | ") + bf(DDD_NAME " License")
	+ rm(" to see the") + cr()
	+ rm("conditions.  There is ") + sl("absolutely no warranty") 
	+ rm(" for " DDD_NAME "; see the ") + cr()
	+ rm(DDD_NAME " License for details.") + cr();

    helpOnVersionExtraText += cr()
	+ rm("If you find " DDD_NAME " useful, please send "
	     "us a picture postcard:") + cr()
	+ cr()
	+ rm("    Technische Universit\344t Braunschweig") + cr()
	+ rm("    Abteilung Softwaretechnologie") + cr()
	+ rm("    B\374ltenweg 88") + cr()
	+ rm("    D-38092 Braunschweig") + cr()
	+ rm("    GERMANY") + cr();

    string log = session_log_file();
    if (log.contains(gethome(), 0))
	log = "~" + log.after(gethome());

    helpOnVersionExtraText += cr()
	+ rm("Send bug reports to <")
	+ tt(ddd_NAME "-bugs@ips.cs.tu-bs.de") + rm(">.") + cr()
	+ rm("Please include the ") + tt(log) + rm(" file;")
	+ rm(" see the " DDD_NAME " manual for details.") + cr()
	+ rm("Send comments and suggestions to <")
	+ tt(ddd_NAME "@ips.cs.tu-bs.de") + rm(">.") + cr();

    helpOnVersionExtraText += cr()
	+ rm(DDD_NAME " WWW page: ") + tt(app_data.www_page) + cr()
	+ rm(DDD_NAME " discussions: <")
	+ tt(ddd_NAME "-users-request@ips.cs.tu-bs.de") + rm(">") + cr()
	+ rm(DDD_NAME " announcements: <")
	+ tt(ddd_NAME "-announce-request@ips.cs.tu-bs.de") + rm(">");
}

static void setup_environment()
{
    // Set the type of the execution tty.
    switch (gdb->type())
    {
    case GDB:
    case DBX:
	// The debugger console has few capabilities.
	// When starting the execution TTY, we set the correct type.
	put_environment("TERM", "dumb");
	break;

    case XDB:
    case JDB:
	// In XDB and JDB, we have no means to set the TTY type
	// afterwards.  Set the execution TTY type right now.
	put_environment("TERM", app_data.term_type);
	break;
    }

    // Don't let TERMCAP settings override our TERM settings.
    put_environment("TERMCAP", "");

    // This avoids zillions of problems with debuggers that pipe
    // their output through `more', `less', and likewise.
    put_environment("PAGER", "cat");

    // Let the debugger know that we're here
    put_environment(DDD_NAME, ddd_NAME "-" DDD_VERSION "-" DDD_HOST);
}

static void setup_command_tool()
{
    // It is preferable to realize the command tool as a DialogShell,
    // since this will cause it to stay on top of other DDD windows.
    // Unfortunately, some window managers do not decorate transient
    // windows such as DialogShells.  In this case, use a TopLevel
    // shell instead and rely on the DDD auto-raise mechanisms defined
    // in `windows.C'.
    //
    // Nobody ever honors all this work.  -AZ

    bool use_transient_tool_shell = true;
    switch (app_data.decorate_tool)
    {
    case On:
	use_transient_tool_shell = false;
	break;
    case Off:
	use_transient_tool_shell = true;
	break;
    case Auto:
	use_transient_tool_shell = have_decorated_transients();
	break;
    }

    Widget tool_shell_parent = 
	source_view_shell ? source_view_shell : command_shell;

    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    XtSetArg(args[arg], XmNallowShellResize, False);      arg++;
    XtSetArg(args[arg], XmNmwmDecorations,
	     MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU); arg++;
    XtSetArg(args[arg], XmNmwmFunctions, 
	     MWM_FUNC_MOVE | MWM_FUNC_CLOSE); arg++;

    if (use_transient_tool_shell)
    {
	tool_shell = 
	    verify(XmCreateDialogShell(tool_shell_parent, 
				       "tool_shell", args, arg));
    }
    else
    {
	tool_shell = 
	    verify(XtCreateWidget("tool_shell", vendorShellWidgetClass,
				  tool_shell_parent, args, arg));
    }

    XmAddWMProtocolCallback(tool_shell, WM_DELETE_WINDOW, 
			    gdbCloseToolWindowCB, 0);

    arg = 0;
    tool_buttons_w = 
	verify(XmCreateForm(tool_shell, "tool_buttons", args, arg));
    set_buttons(tool_buttons_w, app_data.tool_buttons, false);

    Delay::register_shell(tool_shell);
    XtAddEventHandler(tool_shell, STRUCTURE_MASK, False,
		      StructureNotifyEH, XtPointer(0));

#if XmVersion >= 1002
#define FIXED_COMMAND_TOOL 1
#endif

#if FIXED_COMMAND_TOOL
    // Some FVWM flavors have trouble in finding the `best' window size.
    // Determine `best' size for tool shell.
    XtWidgetGeometry size;
    size.request_mode = CWHeight | CWWidth;
    XtQueryGeometry(tool_buttons_w, NULL, &size);
#endif

    // Set shell geometry
    Position pos_x, pos_y;
    get_transient_pos(XtScreen(tool_shell_parent), pos_x, pos_y);

    ostrstream os;
#if FIXED_COMMAND_TOOL
    os << size.width << "x" << size.height;
#endif
    os << "+" << pos_x << "+" << pos_y;
    string geometry(os);

    XtSetArg(args[arg], XmNgeometry, geometry.chars()); arg++;
    XtSetArg(args[arg], XmNx, pos_x);                   arg++;
    XtSetArg(args[arg], XmNy, pos_y);                   arg++;

#if FIXED_COMMAND_TOOL
    // Some FVWM flavors have trouble in finding the `best' window size.
    XtSetArg(args[arg], XmNmaxWidth,  size.width);      arg++;
    XtSetArg(args[arg], XmNmaxHeight, size.height);     arg++;
    XtSetArg(args[arg], XmNminWidth,  size.width);      arg++;
    XtSetArg(args[arg], XmNminHeight, size.height);     arg++;
#endif

    XtSetValues(tool_shell, args, arg);
}

static void setup_options(int& argc, char *argv[],
			  StringArray& saved_options, string& gdb_name,
			  bool& no_windows)
{
    int gdb_option_pos = -1;
    int gdb_option_offset = 2;
    int i;
    for (i = 1; i < argc; i++)
    {
	string arg = string(argv[i]);

	if (arg == "--")
	    break;		// End of options

	if ((arg == "--debugger" || arg == "-debugger") && i < argc - 1)
	{
	    gdb_name = argv[i + 1];
	    gdb_option_pos    = i;
	    gdb_option_offset = 2;
	}

	if (arg == "--dbx" || arg == "-dbx" 
	    || arg == "--gdb" || arg == "-gdb"
	    || arg == "--xdb" || arg == "-xdb"
	    || arg == "--jdb" || arg == "-jdb")
	{
	    gdb_name = arg.after('-', -1);
	    gdb_option_pos    = i;
	    gdb_option_offset = 1;
	}

	if (arg == "--nw" || arg == "-nw" || arg == "-L")
	{
	    if (gdb_option_pos >= 0)
	    {
		// Strip `--debugger NAME'/`--dbx'/`--gdb', etc.
		for (int j = gdb_option_pos; j <= argc - gdb_option_offset; 
					     j++)
		    argv[j] = argv[j + gdb_option_offset];
		argc -= gdb_option_offset;
		i    -= gdb_option_offset;
	    }

	    // Strip `--nw'/`-L'
	    for (int j = i; j <= argc - 1; j++)
		argv[j] = argv[j + 1];
	    argc -= 1;
	    i    -= 1;

	    no_windows = true;
	}

	if (arg == "--PLAY")
	{
	    // Switch into logplayer mode
	    string logname;
	    if (i < argc - 1)
		logname = argv[i + 1];
	    else
		logname = session_log_file();

	    logplayer(logname);
	}

	if (!no_windows)
	{
	    // Save some one-letter options that would be eaten by Xt:
	    // -b BPS  - Set the line speed to BPS (GDB)
	    // -c FILE - use FILE as a core dump to examine (GDB)
	    // -d DIR  - Add DIR to the path of search files (GDB, XDB)
	    // -e FILE - use FILE as executable file to execute (GDB)
	    // -e FILE - Redirect stderr to FILE (XDB)
	    // -i FILE - Redirect stdin to FILE (XDB)
	    // -I DIR  - Add DIR to the path of search files (DBX)
	    // -o FILE - Redirect stdout to FILE (XDB)
	    // -p FILE - Specify a playback FILE (XDB)
	    // -P PID  - Specify a process id PID (DBX, XDB)
	    // -r FILE - Specify a record FILE (XDB)
	    // -R FILE - Specify a restore state FILE (XDB)
	    // -s FILE - read symbol table (GDB)
	    // -s FILE - execute commands from FILE (DBX)
	    // -x FILE - execute commands from FILE (GDB)

	    // Note: the following options are used by DDD:
	    // -h       - Help
	    // -l LOGIN - Login as LOGIN
	    // -v       - Show version
	    // -?       - Help
	    // -t       - Use TTY mode
	    // -f       - Use fullname mode
#if RUNTIME_REGEX
	    static regex rxoptions("-[bcdeiIopPrRsx]");
#endif
	    if (i < argc - 1 && arg.matches(rxoptions))
	    {
		saved_options += arg;
		saved_options += string(argv[i + 1]);

		for (int j = i; j <= argc - 2; j++)
		    argv[j] = argv[j + 2];
		argc -= 2;
		i    -= 2;
	    }
	}
    }
}

static void setup_tty()
{
    if (app_data.tty_mode)
    {
	init_command_tty();

	// Issue init msg (using 7-bit characters)
	string init_msg = XmTextGetString(gdb_w);
	init_msg.gsub("\344", "ae");
	init_msg.gsub("\366", "oe");
	init_msg.gsub("\374", "ue");
	init_msg.gsub("\337", "ss");
	init_msg.gsub("\251", "(C)");
	tty_out(init_msg);
    }
}

static void setup_version_warnings()
{
    // Check for app-defaults
    if (app_data.app_defaults_version == 0)
    {
	cerr << "Error: No `" DDD_CLASS_NAME "' application defaults file\n"
	    "To resolve this problem, you can:\n"
	    "* install the `" DDD_CLASS_NAME "' file in the X "
	    "application defaults directory, or\n"
	    "* set the `" DDD_CLASS_NAME "' location "
	    "in the XAPPLRESDIR environment variable, or\n"
	    "* recompile " DDD_NAME " with builtin app-defaults file;\n"
	    "  see the `--enable-builtin-app-defaults' option for details.\n";

	exit(EXIT_FAILURE);
    }
    else if (string(app_data.app_defaults_version) != DDD_VERSION)
    {
	cerr << "Warning: using `" DDD_CLASS_NAME "' app-defaults file"
	     << " for " DDD_NAME " " << app_data.app_defaults_version 
	     << " (this is " DDD_NAME " " DDD_VERSION ")\n";

	if (!version_warnings.isEmpty())
	    version_warnings += cr();

	version_warnings += rm("Using `") + tt(DDD_CLASS_NAME)
	    + rm("' app-defaults file for " DDD_NAME " ")
	    + rm(app_data.app_defaults_version)
	    + rm(" (this is " DDD_NAME " " DDD_VERSION ")");
    }

    // Check for ~/.ddd/init
    if (app_data.dddinit_version && 
	string(app_data.dddinit_version) != DDD_VERSION)
    {
	cerr << "Warning: using "
	     << quote(session_state_file(app_data.session))
	     << " file for " DDD_NAME " " << app_data.dddinit_version
	     << "\n(this is " DDD_NAME " " DDD_VERSION ")."
	     << "  Please save options.\n";

	if (!version_warnings.isEmpty())
	    version_warnings += cr();

	version_warnings += rm("Using `")
	    + tt(cook(session_state_file(app_data.session)))
	    + rm("' file for " DDD_NAME " ")
	    + rm(app_data.dddinit_version) + cr()
	    + rm("(this is " DDD_NAME " " DDD_VERSION ").  "
		 "Please save options.");
    }
}

static void setup_auto_command_prefix()
{
    if (app_data.auto_command_prefix == 0)
	app_data.auto_command_prefix = "";
    static string prefix = app_data.auto_command_prefix;

    if (prefix.length() < 3)
    {
	// No prefix or insufficient prefix -- generate a new one
	ostrstream key;
	key << ddd_NAME << "-" << getpid() << "-" 
	    << (long)time((time_t *)0) << ": ";
	prefix = key;
    }

    app_data.auto_command_prefix = prefix;
}

// All options that remain fixed for a session go here.
static void setup_options()
{
    set_sensitive(disassemble_w,        gdb->type() == GDB);
    set_sensitive(code_indent_w,        gdb->type() == GDB);
    set_sensitive(examine_w,            gdb->type() == GDB);
    set_sensitive(print_examine_w,      gdb->type() == GDB);
    set_sensitive(cache_machine_code_w, gdb->type() == GDB);

    set_sensitive(set_refer_base_w, gdb->type() != GDB);
    set_sensitive(set_refer_path_w, gdb->type() != GDB);
    set_sensitive(refer_sources_w,  gdb->type() != GDB);

    set_sensitive(edit_watchpoints_w, gdb->has_watch_command());

    set_sensitive(command_separate_exec_window_w, gdb->has_redirection());
    set_sensitive(source_separate_exec_window_w,  gdb->has_redirection());
    set_sensitive(data_separate_exec_window_w,    gdb->has_redirection());

    bool have_core = gdb->has_core_files();
    set_sensitive(command_file_menu[FileItems::OpenCore].widget,    have_core);
    set_sensitive(source_file_menu[FileItems::OpenCore].widget,     have_core);
    set_sensitive(data_file_menu[FileItems::OpenCore].widget,       have_core);

    bool have_exec = gdb->has_exec_files();
    manage_child(command_file_menu[FileItems::OpenFile].widget,     have_exec);
    manage_child(source_file_menu[FileItems::OpenFile].widget,      have_exec);
    manage_child(data_file_menu[FileItems::OpenFile].widget,        have_exec);

    bool have_classes = gdb->has_classes();
    manage_child(command_file_menu[FileItems::OpenClass].widget, have_classes);
    manage_child(source_file_menu[FileItems::OpenClass].widget,  have_classes);
    manage_child(data_file_menu[FileItems::OpenClass].widget,    have_classes);

    bool have_attach = gdb->has_processes();
    set_sensitive(command_file_menu[FileItems::Attach].widget,    have_attach);
    set_sensitive(source_file_menu[FileItems::Attach].widget,     have_attach);
    set_sensitive(data_file_menu[FileItems::Attach].widget,       have_attach);

    bool have_detach = gdb->has_processes();
    set_sensitive(command_file_menu[FileItems::Detach].widget,    have_detach);
    set_sensitive(source_file_menu[FileItems::Detach].widget,     have_detach);
    set_sensitive(data_file_menu[FileItems::Detach].widget,       have_detach);

    bool have_make = gdb->has_make_command() || gdb->has_shell_command();
    set_sensitive(command_file_menu[FileItems::Make].widget,        have_make);
    set_sensitive(source_file_menu[FileItems::Make].widget,         have_make);
    set_sensitive(data_file_menu[FileItems::Make].widget,           have_make);

    set_sensitive(command_file_menu[FileItems::MakeAgain].widget,   have_make);
    set_sensitive(source_file_menu[FileItems::MakeAgain].widget,    have_make);
    set_sensitive(data_file_menu[FileItems::MakeAgain].widget,      have_make);

    bool have_cd = gdb->has_cd_command();
    set_sensitive(command_file_menu[FileItems::CD].widget,            have_cd);
    set_sensitive(source_file_menu[FileItems::CD].widget,             have_cd);
    set_sensitive(data_file_menu[FileItems::CD].widget,               have_cd);

    bool have_settings = (gdb->type() != XDB);
    set_sensitive(command_edit_menu[EditItems::Settings].widget,have_settings);
    set_sensitive(source_edit_menu[EditItems::Settings].widget, have_settings);
    set_sensitive(data_edit_menu[EditItems::Settings].widget,   have_settings);

    set_sensitive(complete_w,  gdb->type() == GDB);
    set_sensitive(define_w,    gdb->type() == GDB);
    set_sensitive(registers_w, gdb->has_regs_command());
    set_sensitive(threads_w,   gdb->type() == GDB || gdb->type() == JDB);
    set_sensitive(signals_w,   gdb->type() == GDB);
    set_sensitive(infos_w,     gdb->type() == GDB);
}

static void setup_core_limit()
{
#if HAVE_GETRLIMIT && HAVE_SETRLIMIT && defined(RLIMIT_CORE)
    struct rlimit limit;
    limit.rlim_cur = 1;
    limit.rlim_max = 1;

    getrlimit(RLIMIT_CORE, &limit);
    if (limit.rlim_max > limit.rlim_cur && limit.rlim_max > 0)
    {
	string msg;

	if (limit.rlim_cur <= 0)
	    msg = "Enabling core dumps";
	else
	    msg = "Raising core file size limit from "
		+ itostring(limit.rlim_cur) + " to "
		+ itostring(limit.rlim_max) + " bytes";

	StatusDelay delay(msg);

	limit.rlim_cur = limit.rlim_max;
	if (setrlimit(RLIMIT_CORE, &limit))
	    delay.outcome = strerror(errno);
    }
    else if (limit.rlim_cur <= 0)
	set_status("Warning: core dumps are disabled");
#endif // HAVE_GETRLIMIT ...
}
