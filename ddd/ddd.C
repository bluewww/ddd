// $Id$
// DDD main program (and much more)

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
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
// See the GNU Library General Public License for more details.
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
#endif

#ifdef HAVE_X11_XMU_EDITRES_H
#include <X11/Xmu/Editres.h>
#endif

#include <X11/IntrinsicP.h>	// LessTif hacks
extern "C" {
#define new new_w
#define class class_w
#include <Xm/SashP.h>		// XmIsSash(), for LessTif hacks
#undef class
#undef new
}
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
#include "ExitCB.h"
#include "GraphEdit.h"
#include "GDBAgent.h"
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
#include "dbx-lookup.h"
#include "editing.h"
#include "environ.h"
#include "exectty.h"
#include "exit.h"
#include "expired.h"
#include "file.h"
#include "findParent.h"
#include "frame.h"
#include "gdbinit.h"
#include "graph.h"
#include "history.h"
#include "host.h"
#include "logo.h"
#include "longName.h"
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
#include <setjmp.h>
#include <time.h>

#if HAVE_EXCEPTIONS && HAVE_STDEXCEPT
#define string stdstring
#include <stdexcept>
#undef string
#endif // HAVE_EXCEPTIONS && HAVE_STDEXCEPT


//-----------------------------------------------------------------------------
// Forward function decls
//-----------------------------------------------------------------------------

// Callbacks
static void gdb_readyHP        (Agent *, void *, void *);
static void gdb_strangeHP      (Agent *, void *, void *);
static void gdb_panicHP        (Agent *, void *, void *);
static void gdb_echo_detectedHP(Agent *, void *, void *);
static void language_changedHP (Agent *, void *, void *);
static void source_argHP       (void *, void *, void *call_data);

// Setup
static Boolean ddd_setup_done(XtPointer client_data);

// Warning proc
static void ddd_xt_warning(String message);

// Event handling
void process_pending_events();

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
static void gdbUpdateViewsCB     (Widget, XtPointer, XtPointer);

// Preferences
static void make_preferences (Widget parent);
static void dddPopupPreferencesCB (Widget, XtPointer, XtPointer);

// User emergencies (Ctrl-C)
bool process_emergencies();

// Return true if user interaction events are pending
static bool pending_interaction();

// Create status line
static void create_status(Widget parent);

// Status LED
static void blink(bool set);
static void ToggleBlinkCB(Widget, XtPointer client_data, XtPointer call_data);
static void DisableBlinkHP(Agent *, void *, void *);

// Status history
static void PopupStatusHistoryCB(Widget, XtPointer, XtPointer);
static void PopdownStatusHistoryCB(Widget, XtPointer, XtPointer);

#if SHOW_GDB_STATUS
// GDB status
static void ShowGDBStatusCB(Widget w, XtPointer, XtPointer);
#endif

// Argument callback
static void ActivateCB(Widget, XtPointer client_data, XtPointer call_data);

// Drag and drop
static void CheckDragCB(Widget, XtPointer client_data, XtPointer call_data);

// Verify whether buttons are active
static void verify_buttons(MMDesc *items);

// Setup shortcut menu
static void set_shortcut_menu(DataDisp *data_disp, string expressions);

// Register shells of menu ITEMS.
void register_menu_shell(MMDesc *items);

// Fix the size of the status line
static void fix_status_size();

// Decorate new shell
static void decorate_new_shell(Widget w);

// Check if window manager decorates transients
static void start_have_decorated_transients(Widget parent);
static bool have_decorated_transients();

// Set `Settings' title
static void set_settings_title(Widget w);

// Popup DDD logo upon start-up.
static void popup_startup_logo(Widget parent, string color_key);
static void popdown_startup_logo(XtPointer data = 0, XtIntervalId *id = 0);

// Read in database from FILENAME.  Upon version mismatch, ignore some
// resources such as window sizes.
static XrmDatabase GetFileDatabase(char *filename);

// Lock `~/.ddd'
static void lock_ddd(Widget parent);

// Various setups
static void setup_version_info();
static void setup_environment();
static void setup_command_tool(bool iconic);
static void setup_options(int argc, char *argv[],
			  StringArray& saved_options, string& gdb_name,
			  bool& no_windows);
static void setup_tty();
static void setup_version_warnings();


//-----------------------------------------------------------------------------
// Xt Stuff
//-----------------------------------------------------------------------------

static char S_true[]    = "true";
static char S_false[]   = "false";

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

{ "--trace",                XtCTrace,                XrmoptionNoArg,  S_true },
{ "-trace",                 XtCTrace,                XrmoptionNoArg,  S_true },

{ "--trace-dialog",         XtNtraceDialog,          XrmoptionNoArg,  S_true },
{ "-trace-dialog",          XtNtraceDialog,          XrmoptionNoArg,  S_true },

{ "--trace-shell-commands", XtNtraceShellCommands,   XrmoptionNoArg,  S_true },
{ "-trace-shell-commands",  XtNtraceShellCommands,   XrmoptionNoArg,  S_true },

{ "--vsl-library",          XtNvslLibrary,           XrmoptionSepArg, NULL },
{ "-vsl-library",           XtNvslLibrary,           XrmoptionSepArg, NULL },

{ "--vsl-path",             XtNvslPath,              XrmoptionSepArg, NULL },
{ "-vsl-path",              XtNvslPath,              XrmoptionSepArg, NULL },

{ "--separate",             XtCSeparate,             XrmoptionNoArg, S_true },
{ "-separate",              XtCSeparate,             XrmoptionNoArg, S_true },
{ "--separate-windows",     XtCSeparate,             XrmoptionNoArg, S_true },
{ "-separate-windows",      XtCSeparate,             XrmoptionNoArg, S_true },

{ "--separate-source-window",XtNseparateSourceWindow, XrmoptionNoArg, S_true },
{ "-separate-source-window", XtNseparateSourceWindow, XrmoptionNoArg, S_true },

{ "--separate-data-window", XtNseparateDataWindow,   XrmoptionNoArg, S_true },
{ "-separate-data-window",  XtNseparateDataWindow,   XrmoptionNoArg, S_true },

{ "--attach",               XtCSeparate,             XrmoptionNoArg, S_false },
{ "-attach",                XtCSeparate,             XrmoptionNoArg, S_false },
{ "--attach-windows",       XtCSeparate,             XrmoptionNoArg, S_false },
{ "-attach-windows",        XtCSeparate,             XrmoptionNoArg, S_false },

{ "--attach-source-window", XtNseparateSourceWindow, XrmoptionNoArg, S_false },
{ "-attach-source-window",  XtNseparateSourceWindow, XrmoptionNoArg, S_false },

{ "--attach-data-window",   XtNseparateDataWindow,   XrmoptionNoArg, S_false },
{ "-attach-data-window",    XtNseparateDataWindow,   XrmoptionNoArg, S_false },

{ "--exec-window",          XtNseparateExecWindow,   XrmoptionNoArg, S_true },
{ "-exec-window",           XtNseparateExecWindow,   XrmoptionNoArg, S_true },

{ "--no-exec-window",       XtNseparateExecWindow,   XrmoptionNoArg, S_false },
{ "-no-exec-window",        XtNseparateExecWindow,   XrmoptionNoArg, S_false },

{ "--no-source-window",     XtNsourceWindow,         XrmoptionNoArg, S_false },
{ "-no-source-window",      XtNsourceWindow,         XrmoptionNoArg, S_false },

{ "--no-data-window",       XtNdataWindow,           XrmoptionNoArg, S_false },
{ "-no-data-window",        XtNdataWindow,           XrmoptionNoArg, S_false },

{ "--no-debugger-console",  XtNdebuggerConsole,      XrmoptionNoArg, S_false },
{ "-no-debugger-console",   XtNdebuggerConsole,      XrmoptionNoArg, S_false },

{ "--button-tips",          XtNbuttonTips,           XrmoptionNoArg, S_true },
{ "-button-tips",           XtNbuttonTips,           XrmoptionNoArg, S_true },

{ "--no-button-tips",       XtNbuttonTips,           XrmoptionNoArg, S_false },
{ "-no-button-tips",        XtNbuttonTips,           XrmoptionNoArg, S_false },

{ "--value-tips",           XtNvalueTips,            XrmoptionNoArg, S_true },
{ "-value-tips",            XtNvalueTips,            XrmoptionNoArg, S_true },

{ "--no-value-tips",        XtNvalueTips,            XrmoptionNoArg, S_false },
{ "-no-value-tips",         XtNvalueTips,            XrmoptionNoArg, S_false },

{ "--status-at-bottom",     XtNstatusAtBottom,       XrmoptionNoArg, S_true },
{ "-status-at-bottom",      XtNstatusAtBottom,       XrmoptionNoArg, S_true },

{ "--status-at-top",        XtNstatusAtBottom,       XrmoptionNoArg, S_false },
{ "-status-at-top",         XtNstatusAtBottom,       XrmoptionNoArg, S_false },

{ "--panned-graph-editor",  XtNpannedGraphEditor,    XrmoptionNoArg, S_true },
{ "-panned-graph-editor",   XtNpannedGraphEditor,    XrmoptionNoArg, S_true },

{ "--scrolled-graph-editor", XtNpannedGraphEditor,   XrmoptionNoArg, S_false },
{ "-scrolled-graph-editor", XtNpannedGraphEditor,    XrmoptionNoArg, S_false },

{ "--sync-debugger",        XtNsynchronousDebugger,  XrmoptionNoArg, S_true },
{ "-sync-debugger",         XtNsynchronousDebugger,  XrmoptionNoArg, S_true },

{ "--disassemble",          XtNdisassemble,          XrmoptionNoArg, S_true },
{ "-disassemble",           XtNdisassemble,          XrmoptionNoArg, S_true },

{ "--no-disassemble",       XtNdisassemble,          XrmoptionNoArg, S_false },
{ "-no-disassemble",        XtNdisassemble,          XrmoptionNoArg, S_false },

{ "--glyphs",               XtNdisplayGlyphs,        XrmoptionNoArg, S_true },
{ "-glyphs",                XtNdisplayGlyphs,        XrmoptionNoArg, S_true },

{ "--no-glyphs",            XtNdisplayGlyphs,        XrmoptionNoArg, S_false },
{ "-no-glyphs",             XtNdisplayGlyphs,        XrmoptionNoArg, S_false },

{ "--host",                 XtNdebuggerHost,         XrmoptionSepArg, NULL },
{ "-host",                  XtNdebuggerHost,         XrmoptionSepArg, NULL },

{ "--rhost",                XtNdebuggerRHost,        XrmoptionSepArg, NULL },
{ "-rhost",                 XtNdebuggerRHost,        XrmoptionSepArg, NULL },

{ "--login",                XtNdebuggerHostLogin,    XrmoptionSepArg, NULL },
{ "-login",                 XtNdebuggerHostLogin,    XrmoptionSepArg, NULL },
{ "-l",                     XtNdebuggerHostLogin,    XrmoptionSepArg, NULL },

{ "--tty",                  XtNttyMode,              XrmoptionNoArg, S_true },
{ "-tty",                   XtNttyMode,              XrmoptionNoArg, S_true },
{ "-t",                     XtNttyMode,              XrmoptionNoArg, S_true },

{ "--fullname",             XtCTTYMode,              XrmoptionNoArg, S_true },
{ "-fullname",              XtCTTYMode,              XrmoptionNoArg, S_true },
{ "-f",                     XtCTTYMode,              XrmoptionNoArg, S_true },

{ "--version",              XtNshowVersion,          XrmoptionNoArg, S_true },
{ "-version",               XtNshowVersion,          XrmoptionNoArg, S_true },
{ "-v",                     XtNshowVersion,          XrmoptionNoArg, S_true },

{ "--configuration",        XtNshowConfiguration,    XrmoptionNoArg, S_true },
{ "-configuration",         XtNshowConfiguration,    XrmoptionNoArg, S_true },

{ "--manual",               XtNshowManual,           XrmoptionNoArg, S_true },
{ "-manual",                XtNshowManual,           XrmoptionNoArg, S_true },

{ "--license",              XtNshowLicense,          XrmoptionNoArg, S_true },
{ "-license",               XtNshowLicense,          XrmoptionNoArg, S_true },

{ "--news",                 XtNshowNews,             XrmoptionNoArg, S_true },
{ "-news",                  XtNshowNews,             XrmoptionNoArg, S_true },

{ "--check-configuration",  XtNcheckConfiguration,   XrmoptionNoArg, S_true },
{ "-check-configuration",   XtNcheckConfiguration,   XrmoptionNoArg, S_true },

{ "--lesstif-hacks",        XtNlessTifVersion,       XrmoptionNoArg, "81" },
{ "-lesstif-hacks",         XtNlessTifVersion,       XrmoptionNoArg, "81" },

{ "--no-lesstif-hacks",     XtNlessTifVersion,       XrmoptionNoArg, "1000" },
{ "-no-lesstif-hacks",      XtNlessTifVersion,       XrmoptionNoArg, "1000" },

{ "--lesstif-version",      XtNlessTifVersion,       XrmoptionSepArg, NULL },
{ "-lesstif-version",       XtNlessTifVersion,       XrmoptionSepArg, NULL },

{ "--help",                 XtNshowInvocation,       XrmoptionNoArg, S_true },
{ "-help",                  XtNshowInvocation,       XrmoptionNoArg, S_true },
{ "-h",                     XtNshowInvocation,       XrmoptionNoArg, S_true },
{ "--?",                    XtNshowInvocation,       XrmoptionNoArg, S_true },
{ "-?",                     XtNshowInvocation,       XrmoptionNoArg, S_true },

};

// Actions
static XtActionsRec actions [] = {
    {"gdb-control",            controlAct},
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

struct FileItems {
    enum FileItem { OpenFile, OpenCore, OpenSource, Sep1,
		    OpenSession, SaveSession, Sep2,
		    Attach, Detach, Sep3,
		    Print, PrintAgain, Sep4,
		    CD, Sep5,
		    Make, MakeAgain, Sep6,
		    Close, Restart, Exit
    };
};

#define FILE_MENU \
{ \
    { "open_file",     MMPush, { gdbOpenFileCB }}, \
    { "open_core",     MMPush, { gdbOpenCoreCB }}, \
    { "open_source",   MMPush, { gdbOpenSourceCB }}, \
    MMSep, \
    { "open_session",  MMPush, { OpenSessionCB }}, \
    { "save_session",  MMPush, { SaveSessionAsCB }}, \
    MMSep, \
    { "attach",        MMPush, { gdbOpenProcessCB }}, \
    { "detach",        MMPush, { gdbCommandCB, "detach" }}, \
    MMSep, \
    { "print",         MMPush, { graphPrintCB }}, \
    { "printAgain",    MMPush | MMUnmanaged, \
 	                       { graphQuickPrintCB, XtPointer(1) }}, \
    { "separator",     MMSeparator | MMUnmanaged }, \
    { "cd",            MMPush, { gdbChangeDirectoryCB }}, \
    { "separator",     MMSeparator | MMUnmanaged }, \
    { "make",          MMPush, { gdbMakeCB }}, \
    { "makeAgain",     MMPush | MMUnmanaged, { gdbMakeAgainCB }}, \
    MMSep, \
    { "close",         MMPush, { DDDCloseCB }}, \
    { "restart",       MMPush, { DDDRestartCB }}, \
    { "exit",          MMPush, { DDDExitCB, XtPointer(EXIT_SUCCESS) }}, \
    MMEnd \
}

static MMDesc command_file_menu[] = FILE_MENU;
static MMDesc source_file_menu[]  = FILE_MENU;
static MMDesc data_file_menu[]    = FILE_MENU;


struct ProgramItems {
    enum ProgramItem { Run, RunAgain, Sep1,
		       SeparateExecWindow, Sep2,
		       Step, Stepi, Next, Nexti, Sep3,
		       Cont, Finish, Sep4,
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
    MMSep, \
    { "cont",        MMPush, { gdbCommandCB, "cont" }}, \
    { "finish",      MMPush, { gdbCommandCB, "finish" }}, \
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

enum DDDWindow { ToolWindow, ExecWindow, DummySep,
		 DataWindow, SourceWindow, GDBWindow, CommonWindow };

static MMDesc view_menu[] =
{
    { "tool",    MMPush, { gdbOpenToolWindowCB }},
    { "exec",    MMPush, { gdbOpenExecWindowCB }},
    MMSep,
    { "data",    MMPush, { gdbOpenDataWindowCB }},
    { "source",  MMPush, { gdbOpenSourceWindowCB }},
    { "console", MMPush, { gdbOpenCommandWindowCB }},
    MMEnd
};

static MMDesc views_menu[] = 
{
    { "tool",    MMPush,   { gdbOpenToolWindowCB }},
    { "exec",    MMPush,   { gdbOpenExecWindowCB }},
    MMSep,
    { "data",    MMToggle, { gdbToggleDataWindowCB }},
    { "source",  MMToggle, { gdbToggleSourceWindowCB }},
    { "console", MMToggle, { gdbToggleCommandWindowCB }},
    MMEnd
};

struct EditItems {
    enum EditItem { 
	Cut, Copy, Paste, ClearAll, Delete, Sep1, 
	SelectAll, UnselectAll, Sep2,
	Preferences, Settings, Sep3,
	SaveOptions
    };
};

#define EDIT_MENU(win) \
{ \
    { "cut",         MMPush,  { gdbCutSelectionCB,    XtPointer(win) }}, \
    { "copy",        MMPush,  { gdbCopySelectionCB,   XtPointer(win) }}, \
    { "paste",       MMPush,  { gdbPasteClipboardCB,  XtPointer(win) }}, \
    { "clearAll",    MMPush,  { gdbClearAllCB,        XtPointer(win) }}, \
    { "delete",      MMPush,  { gdbDeleteSelectionCB, XtPointer(win) }}, \
    { "separator",   MMSeparator | MMUnmanaged }, \
    { "selectAll",   MMPush | MMUnmanaged, \
	                      { gdbSelectAllCB,       XtPointer(win) }}, \
    { "unselectAll", MMPush | MMUnmanaged, \
	                      { gdbUnselectAllCB,     XtPointer(win) }}, \
    MMSep, \
    { "preferences", MMPush,  { dddPopupPreferencesCB }}, \
    { "settings",    MMPush,  { dddPopupSettingsCB }}, \
    MMSep, \
    { "saveOptions", MMPush,  { DDDSaveOptionsCB, XtPointer(SAVE_DEFAULT) }}, \
    MMEnd \
};

static MMDesc command_edit_menu[] = EDIT_MENU(GDBWindow);
static MMDesc source_edit_menu[]  = EDIT_MENU(SourceWindow);
static MMDesc data_edit_menu[]    = EDIT_MENU(DataWindow);

static Widget isearch_prev_w;
static Widget isearch_next_w;
static Widget isearch_exit_w;
static Widget complete_w;
static Widget apply_w;

static MMDesc command_menu[] =
{
    { "history",  MMPush, { gdbHistoryCB }},
    MMSep,
    { "prev",     MMPush, { gdbPrevCB }},
    { "next",     MMPush, { gdbNextCB }},
    MMSep,
    { "isearch_prev", MMPush, { gdbISearchPrevCB }, NULL, &isearch_prev_w },
    { "isearch_next", MMPush, { gdbISearchNextCB }, NULL, &isearch_next_w },
    { "isearch_exit", MMPush, { gdbISearchExitCB }, NULL, &isearch_exit_w },
    MMSep,
    { "complete", MMPush, { gdbCompleteCB }, NULL, &complete_w },
    { "apply",    MMPush, { gdbApplyCB },    NULL, &apply_w },
    MMSep,
    { "clear_line",   MMPush, { gdbClearCB }},
    { "clear_window", MMPush, { gdbClearWindowCB }},
    MMSep,
    { "buttons",  MMPush, { dddEditButtonsCB }},
    MMEnd
};

static Widget stack_w;
static Widget registers_w;
static Widget threads_w;
static Widget up_w;
static Widget down_w;

static MMDesc stack_menu[] =
{
    { "stack",      MMPush,  { SourceView::ViewStackFramesCB }, 
      NULL, &stack_w },
    { "registers",  MMPush,  { SourceView::ViewRegistersCB },
      NULL, &registers_w },
    { "threads",    MMPush,  { SourceView::ViewThreadsCB },
      NULL, &threads_w },
    MMSep,
    { "up",         MMPush,  { gdbCommandCB, "up" },
      NULL, &up_w },
    { "down",       MMPush,  { gdbCommandCB, "down" },
      NULL, &down_w },
    MMEnd
};

static Widget find_words_only_w;
static Widget find_case_sensitive_w;
static Widget disassemble_w;

static MMDesc source_menu[] =
{
    { "breakpoints", MMPush, { SourceView::EditBreakpointsCB }},
    MMSep,
    { "findWordsOnly",       MMToggle, { sourceToggleFindWordsOnlyCB }, 
      NULL, &find_words_only_w },
    { "findCaseSensitive",   MMToggle, { sourceToggleFindCaseSensitiveCB }, 
      NULL, &find_case_sensitive_w },
    { "disassemble",         MMToggle,  { sourceToggleDisassembleCB },
      NULL, &disassemble_w },
    MMSep,
    { "edit",       MMPush,  { gdbEditSourceCB }},
    { "reload",     MMPush,  { gdbReloadSourceCB }},
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
static Widget ungrab_mouse_pointer_w;

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
    { "ungrabMousePointer",  MMToggle, { dddToggleUngrabMousePointerCB },
      NULL, &ungrab_mouse_pointer_w },
    MMEnd
};


// Source preferences
static Widget display_line_numbers_w;
static Widget tab_width_w;

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
static MMDesc find_menu[] =
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

static Widget refer_sources_w;
static MMDesc source_preferences_menu[] = 
{
    { "showExecPos",      MMRadioPanel, MMNoCB, glyph_menu },
    { "referSources",     MMRadioPanel, MMNoCB, refer_menu, &refer_sources_w },
    { "find",             MMButtonPanel, MMNoCB, find_menu },
    { "cache",            MMButtonPanel, MMNoCB, cache_menu },
    { "displayLineNumbers", MMToggle, { sourceToggleDisplayLineNumbersCB },
      NULL, &display_line_numbers_w },
    { "tabWidth", MMScale, { sourceSetTabWidthCB }, NULL, &tab_width_w },
    MMEnd
};


// Data preferences
static Widget graph_detect_aliases_w;
static Widget graph_align_2d_arrays_w;
static Widget graph_show_hints_w;
static Widget graph_snap_to_grid_w;
static Widget graph_compact_layout_w;
static Widget graph_auto_layout_w;
static Widget graph_grid_size_w;

static MMDesc data_preferences_menu[] = 
{
    { "detectAliases", MMToggle,  { graphToggleDetectAliasesCB },
      NULL, &graph_detect_aliases_w },
    { "align2dArrays", MMToggle,  { graphToggleAlign2dArraysCB },
      NULL, &graph_align_2d_arrays_w },
    { "showHints",     MMToggle | MMUnmanaged,  { graphToggleShowHintsCB },
      NULL, &graph_show_hints_w },
    { "snapToGrid",    MMToggle,  { graphToggleSnapToGridCB },
      NULL, &graph_snap_to_grid_w },
    { "compactLayout", MMToggle,  { graphToggleCompactLayoutCB },
      NULL, &graph_compact_layout_w },
    { "autoLayout",    MMToggle,  { graphToggleAutoLayoutCB },
      NULL, &graph_auto_layout_w },
    { "gridSize",      MMScale,   { graphSetGridSizeCB },
      NULL, &graph_grid_size_w },
    MMEnd
};


// Startup preferences
static Widget set_separate_windows_w;
static Widget set_attached_windows_w;
static MMDesc window_mode_menu [] = 
{
    { "separate",  MMToggle, { dddSetSeparateWindowsCB, XtPointer(True) },
      NULL, &set_separate_windows_w },
    { "attached",  MMToggle, { dddSetSeparateWindowsCB, XtPointer(False) },
      NULL, &set_attached_windows_w },
    MMEnd
};

static Widget set_status_bottom_w;
static Widget set_status_top_w;
static MMDesc status_position_menu [] = 
{
    { "bottom",  MMToggle, { dddSetStatusAtBottomCB, XtPointer(True) },
      NULL, &set_status_bottom_w },
    { "top",     MMToggle, { dddSetStatusAtBottomCB, XtPointer(False) },
      NULL, &set_status_top_w },
    MMEnd
};

static Widget set_tool_buttons_in_tool_bar_w;
static Widget set_tool_buttons_in_command_tool_w;
static MMDesc tool_buttons_menu [] = 
{
    { "commandTool",  MMToggle, { dddSetToolBarCB, XtPointer(False) },
      NULL, &set_tool_buttons_in_command_tool_w },
    { "sourceWindow", MMToggle, { dddSetToolBarCB, XtPointer(True) },
      NULL, &set_tool_buttons_in_tool_bar_w },
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
static MMDesc debugger_menu [] = 
{
    { "gdb", MMToggle, { dddSetDebuggerCB, XtPointer(GDB) },
      NULL, &set_debugger_gdb_w },
    { "dbx", MMToggle, { dddSetDebuggerCB, XtPointer(DBX) },
      NULL, &set_debugger_dbx_w },
    { "xdb", MMToggle, { dddSetDebuggerCB, XtPointer(XDB) },
      NULL, &set_debugger_xdb_w },
    MMEnd
};

static Widget startup_logo_color_w;
static Widget startup_logo_grey_w;
static Widget startup_logo_grey4_w;
static Widget startup_logo_mono_w;
static Widget startup_logo_none_w;

static MMDesc startup_logo_menu [] = 
{
    { "color", MMToggle, { dddSetStartupLogoCB, XtPointer("c") },
      NULL, &startup_logo_color_w },
    { "grey", MMToggle, { dddSetStartupLogoCB, XtPointer("g") },
      NULL, &startup_logo_grey_w },
    { "grey4", MMToggle, { dddSetStartupLogoCB, XtPointer("g4") },
      NULL, &startup_logo_grey4_w },
    { "mono", MMToggle | MMUnmanaged, { dddSetStartupLogoCB, XtPointer("m") },
      NULL, &startup_logo_mono_w },
    { "none", MMToggle,  { dddSetStartupLogoCB, XtPointer("") },
      NULL, &startup_logo_none_w },
    MMEnd
};

static MMDesc startup_preferences_menu [] =
{
    { "windows",         MMRadioPanel, MMNoCB, window_mode_menu },
    { "statusPosition",  MMRadioPanel, MMNoCB, status_position_menu },
    { "toolButtons",     MMRadioPanel, MMNoCB, tool_buttons_menu },
    { "keyboardFocus",   MMRadioPanel, MMNoCB, keyboard_focus_menu },
    { "dataScrolling",   MMRadioPanel, MMNoCB, data_scrolling_menu },
    { "debugger",        MMRadioPanel, MMNoCB, debugger_menu },
    { "showStartupLogo", MMRadioPanel, MMNoCB, startup_logo_menu },
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
static Widget locals_w         = 0;
static Widget args_w           = 0;
static Widget detect_aliases_w = 0;
static Widget infos_w          = 0;
static Widget align_w          = 0;

static MMDesc data_menu[] = 
{
    { "displays",   MMPush,    { DataDisp::EditDisplaysCB }},
    MMSep,
    { "detectAliases", MMToggle, { graphToggleDetectAliasesCB },
      NULL, &detect_aliases_w },
    MMSep,
    { "info locals", MMToggle,  { graphToggleLocalsCB }, NULL, &locals_w },
    { "info args",   MMToggle,  { graphToggleArgsCB }, NULL, &args_w },
    { "infos",       MMPush,    { dddPopupInfosCB }, NULL, &infos_w },
    MMSep,
    { "align",      MMPush,    { graphAlignCB  }, NULL, &align_w },
    { "rotate",     MMPush,    { graphRotateCB }},
    { "layout",     MMPush,    { graphLayoutCB }},
    MMSep,
    { "selectAll",  MMPush,    { DataDisp::selectAllCB }},
    { "refresh",    MMPush,    { DataDisp::refreshCB }},
    MMEnd
};


// Help
static MMDesc help_menu[] = 
{
    {"whatNext",    MMPush, { WhatNextCB }},
    MMSep,
    {"onContext",   MMPush, { HelpOnContextCB }},
    {"onWindow",    MMPush, { HelpOnWindowCB }},
    {"onHelp",      MMPush, { HelpOnHelpCB }},
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
    { "view",     MMMenu,          MMNoCB, view_menu },
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
    { "view",    MMMenu,          MMNoCB, view_menu },
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
    { "view",    MMMenu,          MMNoCB, view_menu },
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

static MMDesc print_menu[] =
{
    { "printRef",        MMPush, { gdbPrintRefCB } },
    { "whatis",          MMPush, { gdbWhatisCB } },
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

struct BreakItems {
    enum ArgCmd { TempBreak, Enable, ContUntil, SetPC };
};

static MMDesc break_menu[] = 
{
    { "tempBreakAt", MMPush, { gdbTempBreakAtCB }},
    { "enable",      MMPush, { gdbToggleEnableCB }},
    { "contUntil",   MMPush, { gdbContUntilCB }},
    { "setPC",       MMPush, { gdbSetPCCB }},
    MMEnd
};

struct ArgItems {
    enum ArgCmd { Lookup, Break, Print, Display, FindForward, FindBackward };
};

static MMDesc arg_cmd_area[] = 
{
    {"lookup",        MMPush,  { gdbLookupCB       }},
    {"breakAt",       MMPush,  { gdbToggleBreakCB  }, break_menu   },
    {"print",         MMPush,  { gdbPrintCB        }, print_menu   },
    {"display",       MMPush,  { gdbDisplayCB      }, display_menu },
    {"findBackward",  MMPush,  { gdbFindBackwardCB }},
    {"findForward",   MMPush,  { gdbFindForwardCB  }},
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

// Argument command list
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
static Widget tool_bar_w;

// Strings to be ignored in GDB output
string gdb_out_ignore = "";

// Command management
bool private_gdb_output;   // true if output is running
bool private_gdb_input;    // true if input is running

// true, if initial gdb prompt appeared
bool gdb_initialized;

// The Xt Warning handler
static XtErrorHandler ddd_original_xt_warning_handler;

// Resume after fatal errors
bool main_loop_entered = false;
jmp_buf main_loop_env;

// Initial delays
static Delay *init_delay = 0;

// Logo stuff
static string last_shown_startup_logo;

// Events to note for window visibility
const int STRUCTURE_MASK = StructureNotifyMask | VisibilityChangeMask;

// The atom for the delete-window protocol
static Atom WM_DELETE_WINDOW;

//-----------------------------------------------------------------------------
// Set sensitivity
//-----------------------------------------------------------------------------

inline void set_sensitive(Widget w, bool state)
{
    if (w != 0)
	XtSetSensitive(w, state);
}

inline void manage_child(Widget w, bool state)
{
    if (w != 0)
    {
	if (state)
	    XtManageChild(w);
	else
	    XtUnmanageChild(w);
    }
}

//-----------------------------------------------------------------------------
// Message handling
//-----------------------------------------------------------------------------

static MString version_warnings;



//-----------------------------------------------------------------------------
// Transient position
//-----------------------------------------------------------------------------

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
    char *program_name = argc > 0 ? argv[0] : ddd_NAME;

    // Install signal handlers

    // On some systems (notably HP-UX), GDB has trouble finding what
    // function invoked the signal handler.  Hence, if the environment
    // variable DDD_NO_SIGNAL_HANDLERS is set, we do not install
    // signal handlers, causing DDD to report signals immediately.

    if (getenv("DDD_NO_SIGNAL_HANDLERS") == 0)
    {
	ddd_install_signal();	         // Cleanup upon termination
	ddd_install_fatal(program_name); // Fatal error
    }

#ifdef SIGCHLD
    // Setup signals: Restore default action for SIGCHLD signals.
    // Without asynchronous signal handling, DDD still runs well and
    // is less dependent on OS-specific signal handling.
    signal(SIGCHLD, SignalProc(SIG_DFL));
#endif

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

    // Create new session dir if needed
    create_session_dir(app_data.session, messages);

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

    // Check for --version, --help, etc.
    if (app_data.show_version)
	show_version();

    if (app_data.show_invocation)
	show_invocation(type);

    if (app_data.show_configuration)
	show_configuration();

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
	|| app_data.show_manual)
	return EXIT_SUCCESS;

    // From this point on, we'll be running under X.

    // Warn for incompatible `Ddd' and `~/.ddd/init' files
    setup_version_warnings();

    // Global variables: Set LessTif version
    lesstif_version = app_data.lesstif_version;

    // Global variables: Set maximum lengths
    max_value_tip_length              = app_data.max_value_tip_length;
    max_value_doc_length              = app_data.max_value_doc_length;
    DispBox::max_display_title_length = app_data.max_display_title_length;
    SourceView::max_popup_expr_length = app_data.max_popup_expr_length;

    // Global variables: Set bump_displays
    DataDisp::bump_displays = app_data.bump_displays;

    // Global variables: Set delays for button and value tips
    help_button_tip_delay = app_data.button_tip_delay;
    help_value_tip_delay  = app_data.value_tip_delay;
    help_button_doc_delay = app_data.button_doc_delay;
    help_value_doc_delay  = app_data.value_doc_delay;
    help_clear_doc_delay  = app_data.clear_doc_delay;
    help_clear_tip_delay  = app_data.clear_tip_delay;

#ifdef XmNdragStartCallback
    // Setup drag and drop callback
    Widget display_w = verify(XmGetXmDisplay(XtDisplay(toplevel)));
    XtAddCallback(display_w, XmNdragStartCallback,
		  CheckDragCB, NULL);
#else
    (void) CheckDragCB;		// Use it
#endif

    // Show startup logo
    Boolean iconic;
    XtVaGetValues(toplevel, XmNiconic, &iconic, NULL);
    if (!iconic && restart_session() == "")
	popup_startup_logo(toplevel, app_data.show_startup_logo);
    last_shown_startup_logo = app_data.show_startup_logo;

    // Register own converters
    registerOwnConverters();

    // Lock `~/.ddd/'.
    lock_ddd(toplevel);

    // Create GDB interface
    gdb = new_gdb(type, app_data, app_context, argc, argv);
    gdb->trace_dialog(app_data.trace_dialog);
    defineConversionMacro("GDB", gdb->title());

    // Set up GDB handlers
    gdb->addHandler(ReadyForQuestion, gdb_readyHP);
    gdb->addHandler(InputEOF,         gdb_eofHP);
    gdb->addHandler(ErrorEOF,         gdb_eofHP);
    gdb->addHandler(Died,             gdb_diedHP);
    gdb->addHandler(Died,             DisableBlinkHP);
    gdb->addHandler(LanguageChanged,  DataDisp::language_changedHP);
    gdb->addHandler(LanguageChanged,  language_changedHP);
    gdb->addHandler(ReplyRequired,    gdb_selectHP);
    gdb->addHandler(Panic,            gdb_panicHP);
    gdb->addHandler(Strange,          gdb_strangeHP);
    gdb->addHandler(EchoDetected,     gdb_echo_detectedHP);

    // Set default history file (never read, only overwritten)
    set_gdb_history_file(gdb->history_file());

    // Setup shell creation
    Delay::shell_registered = decorate_new_shell;

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

#ifdef HAVE_X11_XMU_EDITRES_H
    XtAddEventHandler(command_shell, EventMask(0), true,
		      XtEventHandler(_XEditResCheckMessages), NULL);
#endif

    // From this point on, we have a true top-level window.

    // Create main window
    Widget main_window = 
	verify(XtVaCreateManagedWidget ("main_window", 
					xmMainWindowWidgetClass,
					command_shell,
					NULL));

    // Re-register own converters to override Motif converters.
    registerOwnConverters();

    // Create menu bar
    MMDesc *menubar = common_menubar;
    if (app_data.separate_data_window && app_data.separate_source_window)
	menubar = command_menubar;

    Widget menubar_w = MMcreateMenuBar (main_window, "menubar", menubar);
    MMaddCallbacks(menubar);
    verify_buttons(menubar);
    register_menu_shell(menubar);

    // Create Paned Window
    Widget paned_work_w = 
	verify(XtVaCreateWidget("paned_work_w",
				xmPanedWindowWidgetClass,
				main_window,
				NULL));

    // Status line
    if (!app_data.separate_source_window && !app_data.status_at_bottom)
	create_status(paned_work_w);

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
#ifdef HAVE_X11_XMU_EDITRES_H
	XtAddEventHandler(data_disp_shell, EventMask(0), true,
			  XtEventHandler(_XEditResCheckMessages), NULL);
#endif

	data_main_window_w = 
	    verify(XtVaCreateManagedWidget("data_main_window",
					   xmMainWindowWidgetClass,
					   data_disp_shell,
					   NULL));

	// Add menu bar
	data_menubar_w = 
	    MMcreateMenuBar (data_main_window_w, "menubar", data_menubar);
	MMaddCallbacks(data_menubar);
	verify_buttons(data_menubar);
	register_menu_shell(data_menubar);

	data_disp_parent = 
	    verify(XtVaCreateManagedWidget ("data_paned_work_w",
					    xmPanedWindowWidgetClass,
					    data_main_window_w,
					    NULL));
    }

    // Create data display
    data_disp = new DataDisp (app_context,
			      data_disp_parent,
			      app_data.vsl_path,
			      app_data.vsl_library,
			      app_data.vsl_defs,
			      app_data.panned_graph_editor);
    set_shortcut_menu(data_disp, app_data.display_shortcuts);

    if (app_data.separate_data_window)
    {
	// More values for main window
	XtVaSetValues (data_main_window_w,
		       XmNmenuBar,    data_menubar_w,
		       XmNworkWindow, data_disp_parent,
		       NULL);
    }

    // Data buttons (optional)
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
#ifdef HAVE_X11_XMU_EDITRES_H
	XtAddEventHandler(source_view_shell, EventMask(0), true,
			  XtEventHandler(_XEditResCheckMessages), NULL);
#endif

	source_main_window_w = 
	    verify(XtVaCreateManagedWidget("source_main_window",
					   xmMainWindowWidgetClass,
					   source_view_shell,
					   NULL));

	// Add menu bar
	source_menubar_w = 
	    MMcreateMenuBar (source_main_window_w, "menubar", source_menubar);
	MMaddCallbacks(source_menubar);
	verify_buttons(source_menubar);
	register_menu_shell(source_menubar);

	source_view_parent = 
	    verify(XtVaCreateManagedWidget ("source_paned_work_w",
					    xmPanedWindowWidgetClass,
					    source_main_window_w,
					    NULL));

	// Status line
	if (!app_data.status_at_bottom)
	    create_status(source_view_parent);
    }

    source_view = new SourceView(app_context, source_view_parent);

    if (app_data.separate_source_window)
    {
	// More values for main window
	XtVaSetValues (source_main_window_w,
		       XmNmenuBar,    source_menubar_w,
		       XmNworkWindow, source_view_parent,
		       NULL);
    }

    // Argument field and commands
    arg_cmd_w = 
	verify(XmCreateRowColumn(source_view_parent, "arg_cmd_w", NULL, 0));

    Widget arg_label = create_arg_label(arg_cmd_w);
    source_arg = new ArgField (arg_cmd_w, "source_arg");
    XtAddCallback(arg_label, XmNactivateCallback, 
		  ClearTextFieldCB, source_arg->widget());

    MMcreateWorkArea(arg_cmd_w, "arg_cmd_area", arg_cmd_area);
    MMaddCallbacks(arg_cmd_area);
    XtManageChild(arg_cmd_w);
    register_menu_shell(arg_cmd_area);

    XtAddCallback(source_arg->widget(), XmNactivateCallback, 
		  ActivateCB, 
		  XtPointer(arg_cmd_area[ArgItems::Lookup].widget));

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(arg_cmd_w, NULL, &size);
    unsigned char unit_type;
    XtVaGetValues(arg_cmd_w, XmNunitType, &unit_type, NULL);
    Dimension new_height = XmConvertUnits(arg_cmd_w, XmVERTICAL, XmPIXELS, 
					  size.height, unit_type);
    XtVaSetValues(arg_cmd_w,
		  XmNpaneMaximum, new_height,
		  XmNpaneMinimum, new_height,
		  NULL);

    // Tool bar (optional)
    tool_bar_w = make_buttons(source_view_parent, "tool_bar", 
			      app_data.tool_buttons);
    if (tool_bar_w != 0)
	XtUnmanageChild(tool_bar_w);

    // Source buttons (optional)
    source_buttons_w = make_buttons(source_view_parent, "source_buttons", 
				    app_data.source_buttons);

    // Status line
    if (app_data.separate_source_window && app_data.status_at_bottom)
	create_status(source_view_parent);


    // GDB window
    gdb_w = verify(XmCreateScrolledText(paned_work_w,
					"gdb_w",
					NULL, 0));
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

#if 0
    // Don't edit the text until the first prompt appears.
    XmTextSetEditable(gdb_w, false);
#endif

    // Console buttons (optional)
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

    // Create preference panels
    make_preferences(paned_work_w);

    // All widgets are created at this point.
    
    // Load history for current session
    load_history(session_history_file(app_data.session));

    // Put saved options back again
    int i;
    for (i = argc + saved_options.size() - 1; i > saved_options.size(); i--)
	argv[i] = argv[i - saved_options.size()];
    for (i = saved_options.size() - 1; i >= 0; i--)
	argv[i + 1] = saved_options[i];
    argc += saved_options.size();
    argv[argc] = 0;

    // Setup environment.
    setup_environment();

    // Setup handlers
    source_arg->addHandler (Changed, source_argHP);
    source_arg->callHandlers();

    // Setup insertion position
    promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, messagePosition);

    // Setup help pixmap
    helpOnVersionPixmapProc = versionlogo;

    // Setup version info
    setup_version_info();

    // Customize `settings' title.
    set_settings_title(command_edit_menu[EditItems::Settings].widget);
    set_settings_title(source_edit_menu[EditItems::Settings].widget);
    set_settings_title(data_edit_menu[EditItems::Settings].widget);

    // The logo is no longer needed now
    popdown_startup_logo();

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
	init_delay = new Delay;
    else
	init_delay = new StatusDelay("Opening session " 
				     + quote(app_data.session));

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

    // Save option states
    save_option_state();

    // Setup option states
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
    else if (!app_data.tty_mode)
    {
	// Popup the command shell only; other shells follow as needed
	initial_popup_shell(command_shell);
    }
    else
    {
	// TTY mode: all shells follow as needed.
    }

    if (app_data.tty_mode)
    {
	// TTY mode: no need for a debugger console
	gdbCloseCommandWindowCB(gdb_w, 0, 0);
    }
    if (app_data.full_name_mode)
    {
	// Full name mode: no need for source window
	gdbCloseSourceWindowCB(gdb_w, 0, 0);
	gdbCloseToolWindowCB(gdb_w, 0, 0);
    }

    // Close windows explicitly requested
    if (!app_data.data_window)
	gdbCloseDataWindowCB(gdb_w, 0, 0);
    if (!app_data.source_window)
    {
	gdbCloseSourceWindowCB(gdb_w, 0, 0);
	gdbCloseToolWindowCB(gdb_w, 0, 0);
    }
    if (!app_data.debugger_console)
	gdbCloseCommandWindowCB(gdb_w, 0, 0);

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
    {
	setup_command_tool(iconic);
    }

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

    // Start debugger
    start_gdb();
    gdb_tty = gdb->slave_tty();

    if (!emptyCommandQueue())
    {
	// GDB startup leaves us with pending graph commands.  We
	// should better initialize the VSL library right now.
	DispBox::init_vsllib();
    }

    // Main Loop
    main_loop_entered = false;
    int sig;
    if ((sig = setjmp(main_loop_env)))
    {
	main_loop_entered = false;
	ddd_show_signal(sig);
	reset_status_lock();
    }

    // Set `main_loop_entered' to true as soon 
    // as DDD becomes idle again.
    XtAppAddWorkProc(app_context, ddd_setup_done, 0);

    bool forever = true;
    while (forever)
	process_next_event();

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
    // Check if GDB is still running
    gdb->running();

    // Check if the command TTY is still open
    tty_running();

    // Check if the separate TTY is still running
    exec_tty_running();

    // Check for emergencies
    process_emergencies();

    // Restart blinker
    blink(!gdb->isReadyWithPrompt());

    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);

    if (app_data.synchronous_gdb && gdb->isBusyOnQuestion())
    {
	// Synchronous mode: wait for GDB to answer question
	XtAppProcessEvent(app_context, XtIMAlternateInput);
    }
    else if (XtAppPending(app_context) & (XtIMXEvent | XtIMTimer))
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
#if HAVE_STDEXCEPT
    // Catch standard exceptions
#if HAVE_TYPEINFO
    // Just get the exception type and diagnostics
    catch (const exception& err)
    {
	ddd_show_exception(typeid(err).name(), err.what());
    }
#else // !HAVE_TYPEINFO
    // I seriously doubt that exception handling works without
    // run-time type identification.  Nonetheless, catch the two major
    // error classes.
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
#endif // HAVE_STDEXCEPT
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
	XtNshowStartupLogo,	              // Startup logo
	XtNshowHints,		              // Show edge hints
	XtNungrabMousePointer,	              // Ungrab pointer settings
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
		read_leading_blanks(version_found);
		strip_final_blanks(version_found);

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

	read_leading_blanks(item);
	strip_final_blanks(item);

	read_leading_blanks(label);
	strip_final_blanks(label);

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

    if (lesstif_version > 79)
	return;

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

    if (sash == 0 || !XtIsRealized(sash))
	return;			// No sash found


    // Simulate a vertical drag of MOVEMENT pixels
    const Dimension movement = max(height, HeightOfScreen(XtScreen(sash)));

    // Press button 1 ...
    XEvent event;
    event.type                = ButtonPress;
    event.xbutton.serial      = 0;
    event.xbutton.display     = XtDisplay(sash);
    event.xbutton.window      = XtWindow(sash);
    event.xbutton.root        = RootWindowOfScreen(XtScreen(sash));
    event.xbutton.subwindow   = None;
    event.xbutton.time        = XtLastTimestampProcessed(XtDisplay(sash));
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
	event.xmotion.root        = RootWindowOfScreen(XtScreen(sash));
	event.xmotion.subwindow   = None;
	event.xmotion.time        = XtLastTimestampProcessed(XtDisplay(sash));
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
    event.xmotion.time        = XtLastTimestampProcessed(XtDisplay(sash));
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
    event.xbutton.time        = XtLastTimestampProcessed(XtDisplay(sash));
    event.xbutton.x           = 0;
    event.xbutton.y           = movement;
    event.xbutton.x_root      = 0;
    event.xbutton.y_root      = movement;
    event.xbutton.state       = Button1Mask;
    event.xbutton.button      = Button1;
    event.xbutton.same_screen = True;
    XtDispatchEvent(&event);
}


//-----------------------------------------------------------------------------
// Locking stuff
//-----------------------------------------------------------------------------

static bool continue_despite_lock = false;
static int lock_dialog_x = -1;
static int lock_dialog_y = -1;

static void ContinueDespiteLockCB(Widget, XtPointer, XtPointer)
{
    continue_despite_lock = true;
}

static void lock_ddd(Widget parent)
{
    LockInfo info;
    bool lock_ok = 
	lock_session_dir(XtDisplay(parent), DEFAULT_SESSION, info);
    if (lock_ok)
	return;

    string lock_file = session_lock_file(DEFAULT_SESSION);
    
    MString msg = rm(DDD_NAME " has detected a ") 
	+ tt(lock_file) + rm(" file.") + cr()
	+ cr()
	+ rm("This may indicate that another user is running "
	     DDD_NAME) + cr()
	+ rm("using your ")
	+ tt(session_state_dir() + "/") + rm(" files.") + cr()
	+ rm(DDD_NAME " appears to be running on host ")
	+ tt(info.hostname) + cr()
	+ rm("under process ID ") + tt(itostring(info.pid)) + cr()
	+ cr()
	+ rm("You may continue to use " DDD_NAME ", but your "
	     "saved " DDD_NAME " state may be") + cr()
	+ rm("overwritten by the other " DDD_NAME " instance.") + cr()
	+ cr()
	+ rm("Otherwise, make sure that you are not running "
	     "another " DDD_NAME ",") + cr()
	+ rm("delete the ") + tt(lock_file) + rm(" file,") + cr()
	+ rm("and restart " DDD_NAME ".");
	
    Arg args[10];
    int arg = 0;
	
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

	if (app_data.initial_session != 0)
	{
	    set_session(app_data.initial_session);
	    app_data.initial_session = 0;
	}

	return True;		// Remove from the list of work procs
    }
    return False;		// Get called again
}

static Boolean ddd_setup_done(XtPointer)
{
    if (app_data.session == DEFAULT_SESSION)
    {
	// Clear delay now
	delete init_delay;
	init_delay = 0;
    }

    if (emptyCommandQueue() && gdb->isReadyWithPrompt())
    {
	// Some WMs have trouble with early decorations.  Just re-decorate.
	decorate_new_shell(command_shell);
	decorate_new_shell(data_disp_shell);
	decorate_new_shell(source_view_shell);

	ddd_check_version();
	install_button_tips();
	fix_status_size();

	// Initialize `views' menu.  LessTif needs this.
	gdbUpdateViewsCB(0, XtPointer(views_menu), 0);

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
#if defined(XmNdragStartCallback) && defined(__linux__)
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
// Option handling
//-----------------------------------------------------------------------------

static void set_toggle(Widget w, Boolean new_state, bool notify = false)
{
    if (w == 0)
	return;

    assert(XmIsToggleButton(w));

    Boolean old_state;
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
    XtVaSetValues(w, 
		  XmNvalue, value,
		  XmNcursorPosition, 0,
		  NULL);
}

// Reflect state in option menus
void update_options()
{
    set_toggle(find_words_only_w, app_data.find_words_only);
    set_toggle(words_only_w, app_data.find_words_only);

    set_toggle(find_case_sensitive_w, app_data.find_case_sensitive);
    set_toggle(case_sensitive_w, app_data.find_case_sensitive);

    set_sensitive(disassemble_w, gdb->type() == GDB);
    set_toggle(disassemble_w, app_data.disassemble);

    set_toggle(command_separate_exec_window_w, app_data.separate_exec_window);
    set_toggle(source_separate_exec_window_w,  app_data.separate_exec_window);
    set_toggle(data_separate_exec_window_w,    app_data.separate_exec_window);

    set_toggle(button_tips_w,            app_data.button_tips); 
    set_toggle(value_tips_w,             app_data.value_tips); 
    set_toggle(button_docs_w,            app_data.button_docs); 
    set_toggle(value_docs_w,             app_data.value_docs); 
    set_toggle(set_global_completion_w,  app_data.global_tab_completion);
    set_toggle(set_console_completion_w, !app_data.global_tab_completion);
    set_toggle(group_iconify_w,          app_data.group_iconify);
    set_toggle(uniconify_when_ready_w,   app_data.uniconify_when_ready);
    set_toggle(ungrab_mouse_pointer_w,   app_data.ungrab_mouse_pointer);
    set_toggle(suppress_warnings_w,      app_data.suppress_warnings);

    set_toggle(display_line_numbers_w,   app_data.display_line_numbers);
    set_toggle(cache_source_files_w,     app_data.cache_source_files);
    set_toggle(cache_machine_code_w,     app_data.cache_machine_code);
    set_toggle(set_display_glyphs_w,     app_data.display_glyphs);
    set_toggle(set_display_text_w,       !app_data.display_glyphs);
    set_toggle(set_refer_path_w,         app_data.use_source_path);
    set_toggle(set_refer_base_w,         !app_data.use_source_path);

    XtVaSetValues(tab_width_w, XmNvalue, app_data.tab_width, NULL);

    set_toggle(led_w,                    app_data.blink_while_busy);

    set_sensitive(cache_machine_code_w, gdb->type() == GDB);
    set_sensitive(set_refer_base_w, gdb->type() != GDB);
    set_sensitive(set_refer_path_w, gdb->type() != GDB);
    set_sensitive(refer_sources_w,  gdb->type() != GDB);


    Boolean show_grid, snap_to_grid, show_hints, auto_layout;
    LayoutMode layout_mode;
    Dimension grid_width, grid_height;

    XtVaGetValues(data_disp->graph_edit, 
 		  XtNshowGrid,   &show_grid,
 		  XtNsnapToGrid, &snap_to_grid,
		  XtNshowHints,  &show_hints,
		  XtNautoLayout, &auto_layout,
		  XtNlayoutMode, &layout_mode,
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  NULL);

    set_toggle(detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_align_2d_arrays_w, app_data.align_2d_arrays);

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

    set_sensitive(graph_snap_to_grid_w, show_grid);
    set_sensitive(align_w, show_grid);

    set_toggle(graph_show_hints_w, show_hints);
    set_toggle(graph_auto_layout_w, auto_layout);
    set_toggle(graph_compact_layout_w, layout_mode == CompactLayoutMode);

    XtVaSetValues(graph_grid_size_w, XmNvalue, show_grid ? grid_width : 0, 
		  NULL);


    unsigned char policy = '\0';
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &policy, NULL);
    set_toggle(set_focus_pointer_w,        policy == XmPOINTER);
    set_toggle(set_focus_explicit_w,       policy == XmEXPLICIT);

    set_toggle(set_scrolling_panner_w,     app_data.panned_graph_editor);
    set_toggle(set_scrolling_scrollbars_w, !app_data.panned_graph_editor);

    set_toggle(set_status_bottom_w,        app_data.status_at_bottom);
    set_toggle(set_status_top_w,           !app_data.status_at_bottom);

    set_toggle(set_tool_buttons_in_tool_bar_w,     app_data.tool_bar);
    set_toggle(set_tool_buttons_in_command_tool_w, !app_data.tool_bar);

    Boolean separate = 
	app_data.separate_data_window || app_data.separate_source_window;
    set_toggle(set_separate_windows_w, separate);
    set_toggle(set_attached_windows_w, !separate);

    DebuggerType type = debugger_type(app_data.debugger);
    set_toggle(set_debugger_gdb_w, type == GDB);
    set_toggle(set_debugger_dbx_w, type == DBX);
    set_toggle(set_debugger_xdb_w, type == XDB);

    string color_key = app_data.show_startup_logo;
    set_toggle(startup_logo_color_w, color_key == "c");
    set_toggle(startup_logo_grey_w,  color_key == "g");
    set_toggle(startup_logo_grey4_w, color_key == "g4");
    set_toggle(startup_logo_mono_w,  color_key == "m");
    set_toggle(startup_logo_none_w,  color_key == "");

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

    source_view->source_indent_amount = app_data.indent_source;
    source_view->code_indent_amount   = app_data.indent_code;
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

    if (app_data.tool_bar && tool_bar_w != 0 && !XtIsManaged(tool_bar_w))
    {
	if (app_data.source_window)
	    XtManageChild(tool_bar_w);
	gdbCloseToolWindowCB(command_shell, 0, 0);
    }
    else if (!app_data.tool_bar && tool_bar_w != 0 && XtIsManaged(tool_bar_w))
    {
	XtUnmanageChild(tool_bar_w);
	if (app_data.source_window)
	    gdbOpenToolWindowCB(command_shell, 0, 0);
    }

    EnableButtonTips(app_data.button_tips);
    EnableButtonDocs(app_data.button_docs);
    EnableTextTips(app_data.value_tips);
    EnableTextDocs(app_data.value_docs);

    if (last_shown_startup_logo != app_data.show_startup_logo)
    {
	popup_startup_logo(gdb_w, app_data.show_startup_logo);

	static XtIntervalId timer = 0;

	if (timer != 0)
	{
	    XtRemoveTimeOut(timer);
	    timer = 0;
	}

	timer = XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 1000, 
				popdown_startup_logo, (void *)&timer);
    }

    set_string(edit_command_w,       app_data.edit_command);
    set_string(get_core_command_w,   app_data.get_core_command);
    set_string(ps_command_w,         app_data.ps_command);
    set_string(term_command_w,       app_data.term_command);
    set_string(uncompress_command_w, app_data.uncompress_command);
    set_string(www_command_w,        app_data.www_command);

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
		  XtNshowGrid,   &initial_show_grid,
		  XtNshowHints,  &initial_show_hints,
		  XtNsnapToGrid, &initial_snap_to_grid,
		  XtNlayoutMode, &initial_layout_mode, 
		  XtNautoLayout, &initial_auto_layout,
		  XtNgridWidth,  &initial_grid_width,
		  XtNgridHeight, &initial_grid_height,
		  NULL);

    XtVaGetValues(command_shell,
		  XmNkeyboardFocusPolicy, &initial_focus_policy, NULL);

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
    notify_set_toggle(ungrab_mouse_pointer_w, 
		      initial_app_data.ungrab_mouse_pointer);
}

static bool general_preferences_changed()
{
    return app_data.button_tips != initial_app_data.button_tips
	|| app_data.button_docs != initial_app_data.button_docs
	|| app_data.value_tips != initial_app_data.value_tips
	|| app_data.value_docs != initial_app_data.value_docs
	|| app_data.global_tab_completion != 
	    initial_app_data.global_tab_completion
	|| app_data.group_iconify != initial_app_data.group_iconify
	|| app_data.uniconify_when_ready != 
	    initial_app_data.uniconify_when_ready
	|| app_data.suppress_warnings != initial_app_data.suppress_warnings
	|| app_data.ungrab_mouse_pointer != 
	    initial_app_data.ungrab_mouse_pointer;
}

static void ResetSourcePreferencesCB(Widget, XtPointer, XtPointer)
{
    notify_set_toggle(set_display_glyphs_w, initial_app_data.display_glyphs);
    notify_set_toggle(set_display_glyphs_w, !initial_app_data.display_glyphs);
    notify_set_toggle(set_refer_path_w, initial_app_data.use_source_path);
    notify_set_toggle(set_refer_base_w, !initial_app_data.use_source_path);
    notify_set_toggle(words_only_w, initial_app_data.find_words_only);
    notify_set_toggle(case_sensitive_w, initial_app_data.find_case_sensitive);
    notify_set_toggle(cache_source_files_w, 
		      initial_app_data.cache_source_files);
    notify_set_toggle(cache_machine_code_w, 
		      initial_app_data.cache_machine_code);
    notify_set_toggle(display_line_numbers_w, 
		      initial_app_data.display_line_numbers);

    if (app_data.tab_width != initial_app_data.tab_width)
    {
	app_data.tab_width = initial_app_data.tab_width;
	update_options();
    }
}

static bool source_preferences_changed()
{
    return app_data.display_glyphs != initial_app_data.display_glyphs
	|| app_data.display_line_numbers 
	   != initial_app_data.display_line_numbers
	|| app_data.use_source_path != initial_app_data.use_source_path
	|| app_data.find_words_only != initial_app_data.find_words_only
	|| app_data.find_case_sensitive != initial_app_data.find_case_sensitive
	|| app_data.cache_source_files != initial_app_data.cache_source_files
	|| app_data.cache_machine_code != initial_app_data.cache_machine_code
	|| app_data.tab_width != initial_app_data.tab_width;
}

static void ResetDataPreferencesCB(Widget, XtPointer, XtPointer)
{
    notify_set_toggle(detect_aliases_w, initial_app_data.detect_aliases);
    notify_set_toggle(graph_detect_aliases_w, initial_app_data.detect_aliases);
    notify_set_toggle(graph_align_2d_arrays_w, 
		      initial_app_data.align_2d_arrays);
    notify_set_toggle(graph_show_hints_w, initial_show_hints);
    notify_set_toggle(graph_snap_to_grid_w, initial_snap_to_grid);
    notify_set_toggle(graph_compact_layout_w, 
	       initial_layout_mode == CompactLayoutMode);
    notify_set_toggle(graph_auto_layout_w, initial_auto_layout);

    Dimension grid_width, grid_height;
    Boolean show_grid;

    XtVaGetValues(data_disp->graph_edit, 
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  XtNshowGrid,   &show_grid,
		  NULL);

    if (grid_width     != initial_grid_width 
	|| grid_height != initial_grid_height
	|| show_grid   != initial_show_grid)
    {
	XtVaSetValues(data_disp->graph_edit,
		      XtNgridWidth,  grid_width  = initial_grid_width,
		      XtNgridHeight, grid_height = initial_grid_height,
		      XtNshowGrid,   show_grid   = initial_show_grid,
		      NULL);
		      
	update_options();
    }
}

static bool data_preferences_changed()
{
    Boolean show_grid, show_hints, snap_to_grid, auto_layout;
    LayoutMode layout_mode;
    Dimension grid_width, grid_height;

    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowGrid,   &show_grid,
		  XtNshowHints,  &show_hints,
		  XtNsnapToGrid, &snap_to_grid,
		  XtNlayoutMode, &layout_mode, 
		  XtNautoLayout, &auto_layout,
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  NULL);

    return app_data.detect_aliases != initial_app_data.detect_aliases
	|| app_data.align_2d_arrays != initial_app_data.align_2d_arrays
	|| show_grid    != initial_show_grid
	|| show_hints   != initial_show_hints
	|| snap_to_grid != initial_snap_to_grid
	|| layout_mode  != initial_layout_mode 
	|| auto_layout  != initial_auto_layout
	|| grid_width   != initial_grid_width
	|| grid_height  != initial_grid_height;
}

static void ResetStartupPreferencesCB(Widget, XtPointer, XtPointer)
{
    Boolean separate = initial_app_data.separate_data_window 
	|| initial_app_data.separate_source_window;

    notify_set_toggle(set_separate_windows_w, separate);
    notify_set_toggle(set_attached_windows_w, !separate);

    notify_set_toggle(set_status_bottom_w, initial_app_data.status_at_bottom);
    notify_set_toggle(set_status_top_w, !initial_app_data.status_at_bottom);

    notify_set_toggle(set_tool_buttons_in_tool_bar_w, 
		      initial_app_data.tool_bar);
    notify_set_toggle(set_tool_buttons_in_command_tool_w, 
		      !initial_app_data.tool_bar);

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

    string color_key = initial_app_data.show_startup_logo;
    notify_set_toggle(startup_logo_color_w, color_key == "c");
    notify_set_toggle(startup_logo_grey_w,  color_key == "g");
    notify_set_toggle(startup_logo_grey4_w, color_key == "g4");
    notify_set_toggle(startup_logo_mono_w,  color_key == "m");
    notify_set_toggle(startup_logo_none_w,  color_key == "");
}


bool startup_preferences_changed()
{
    Boolean initial_separate = 
	initial_app_data.separate_data_window 
	|| initial_app_data.separate_source_window;
    Boolean separate = 
	app_data.separate_data_window 
	|| app_data.separate_source_window;

    unsigned char focus_policy;
    XtVaGetValues(command_shell,
		  XmNkeyboardFocusPolicy, &focus_policy, NULL);

    return separate != initial_separate
	|| app_data.status_at_bottom != initial_app_data.status_at_bottom
	|| app_data.tool_bar != initial_app_data.tool_bar
	|| focus_policy != initial_focus_policy
	|| app_data.panned_graph_editor != initial_app_data.panned_graph_editor
	|| debugger_type(app_data.debugger)
  	      != debugger_type(initial_app_data.debugger)
	|| string(app_data.show_startup_logo)
	      != string(initial_app_data.show_startup_logo);
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

bool helpers_preferences_changed()
{
    return string(app_data.edit_command) 
	   != string(initial_app_data.edit_command)
	|| string(app_data.get_core_command) 
	   != string(initial_app_data.get_core_command)
	|| string(app_data.ps_command) 
	   != string(initial_app_data.ps_command)
	|| string(app_data.term_command) 
	   != string(initial_app_data.term_command)
	|| string(app_data.uncompress_command) 
	   != string(initial_app_data.uncompress_command)
	|| string(app_data.www_command) 
	   != string(initial_app_data.www_command);
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
    else if (panel_name == "startup")
	ResetStartupPreferencesCB(w, client_data, call_data);
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
	else if (panel_name == "startup")
	    sensitive = startup_preferences_changed();
	else if (panel_name == "helpers")
	    sensitive = helpers_preferences_changed();

	XtSetSensitive(reset_preferences_w, sensitive);
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
    MMaddCallbacks(items);
    XtManageChild(panel);
    register_menu_shell(items);

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
    add_panel(change, buttons, "startup", startup_preferences_menu, 
	      max_width, max_height, false);
    add_panel(change, buttons, "helpers", helpers_preferences_menu, 
	      max_width, max_height, false);

    unsigned char unit_type;
    XtVaGetValues(change, XmNunitType, &unit_type, NULL);
    int new_height = XmConvertUnits(change, XmVERTICAL, XmPIXELS, 
				    max_height, unit_type);
    int new_width  = XmConvertUnits(change, XmHORIZONTAL, XmPIXELS, 
				    max_width, unit_type);

    XtVaSetValues(change,
		  XmNwidth, new_width,
		  XmNheight, new_height,
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
    Arg args[10];
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
	XtSetArg(args[arg], XmNlabelString,        spaces.xmstring()); arg++;
    }

    led_w = verify(XmCreateToggleButton(status_form, "led", args, arg));
    XtManageChild(led_w);

    XtAddCallback(led_w, XmNvalueChangedCallback, ToggleBlinkCB, XtPointer(0));

    // Create `Get more status messages' button
    arg = 0;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNresizable,        False); arg++; 
    XtSetArg(args[arg], XmNarrowDirection, 
	     (app_data.status_at_bottom ? XmARROW_UP : XmARROW_DOWN)); arg++;
    Widget arrow_w = 
	verify(XmCreateArrowButton(status_form, "arrow", args, arg));
    XtManageChild(arrow_w);

    // Give some `dummy' status message.  Some Motif versions limit
    // the size of the status window to the length of the very first
    // message, so we give some huge string at the beginning.
    MString short_msg = rm("Welcome to " DDD_NAME " " DDD_VERSION "!");
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
    unsigned char unit_type;

    size.request_mode = CWHeight;
    XtQueryGeometry(status_w, NULL, &size);
    XtVaGetValues(status_w, XmNunitType, &unit_type, NULL);
    Dimension new_height = XmConvertUnits(status_w, XmVERTICAL, XmPIXELS, 
					  size.height, unit_type);

    if (lesstif_version < 1000)
	XtVaSetValues(led_w, XmNindicatorSize, new_height - 3, NULL);
    else
	XtVaSetValues(led_w, XmNindicatorSize, new_height - 1, NULL);

    XtVaSetValues(arrow_w,
		  XmNheight, new_height - 2,
		  XmNwidth,  new_height - 2,
		  NULL);
    XtVaSetValues(status_form,
		  XmNpaneMaximum, new_height,
		  XmNpaneMinimum, new_height,
		  NULL);

    set_toggle(led_w, app_data.blink_while_busy);
    blink(true);
}


//-----------------------------------------------------------------------------
// Handle Status LED
//-----------------------------------------------------------------------------

static bool blinker_active        = false; // True iff status LED is active
static XtIntervalId blink_timer   = 0;     // Timer for blinking
static time_t blink_timer_called  = 0;     // Time of blink_timer call

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

    bool set = int(client_data);
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
	blink_timer_called = time((time_t *)0);
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
#if 0
    else
    {
	// The blinker hangs up occasionally - that is, BLINK_TIMER != 0
	// holds, but BlinkCB() is never called.  Hence, we check for the
	// time elapsed since we added the BlinkCB() timeout.
	time_t seconds_since_timer_call = 
	    (time((time_t *)0) - blink_timer_called);
	bool timer_should_have_been_called = 
	    seconds_since_timer_call >= app_data.busy_blink_rate / 1000 + 1;

	if (timer_should_have_been_called && blink_timer != 0)
	{
	    // Remove timer and re-activate it again
	    XtRemoveTimeOut(blink_timer);
	    BlinkCB(XtPointer(int(blinker_active)), &blink_timer);
	}
    }
#endif
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

#if SHOW_GDB_STATUS
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    if (cbs->event->xbutton.state & ShiftMask)
    {
	ShowGDBStatusCB(w, client_data, call_data);
	return;
    }
#endif

    history_shell = status_history(w);

    Position shell_x, shell_y;
    XtTranslateCoords(find_shell(status_w), 0, 0, &shell_x, &shell_y);

    Position status_x, status_y;
    XtTranslateCoords(status_w, 0, 0, &status_x, &status_y);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    unsigned char unit_type;

    XtQueryGeometry(status_w, NULL, &size);
    XtVaGetValues(status_w, XmNunitType, &unit_type, NULL);
    Dimension status_height = XmConvertUnits(status_w, XmVERTICAL, XmPIXELS,
					     size.height, unit_type);

    XtQueryGeometry(history_shell, NULL, &size);
    XtVaGetValues(history_shell, XmNunitType, &unit_type, NULL);
    Dimension history_height = XmConvertUnits(history_shell, XmVERTICAL,
					      XmPIXELS,
					      size.height, unit_type);

    Position x, y;
    if (app_data.status_at_bottom)
    {
	x = shell_x;
	y = status_y - history_height - y_popup_offset;
    }
    else
    {
	x = shell_x;
	y = status_y + status_height + y_popup_offset;
    }

    XtVaSetValues(history_shell, XmNx, x, XmNy, y, XtPointer(0));
    XtPopup(history_shell, XtGrabNone);
}

static void PopdownStatusHistoryCB(Widget, XtPointer, XtPointer)
{
    if (history_shell != 0)
	XtPopdown(history_shell);
}


#if SHOW_GDB_STATUS
//-----------------------------------------------------------------------------
// Show status of inferior debugger
//-----------------------------------------------------------------------------

static MString yn(bool b)
{
    return (b ? rm("yes") : rm("no")) + cr();
}

static MString cap(const MString& title, bool b)
{
    return title + bf(": ") + yn(b);
}

static MString has(const string& command, bool b)
{
    return cap(tb(quote(command)) + bf(" command"), b);
}

static MString cmd(const string& title, const string& cmd)
{
    MString s = bf(title + " command: ");
    if (cmd == "")
	return s + rm("-/-") + cr();
    else
	return s + tt(cmd) + cr();
}

static MString expr(const string& title, const string& expr)
{
    return bf(title + " expression: ") + tt(expr) + cr();
}

static MString option(const string& command, const string& opt, bool b)
{
    return cap(tb(quote(command)) + bf(" wants ") + tb(quote(opt)), b);
}

static void ShowGDBStatusCB(Widget w, XtPointer client_data, XtPointer)
{
    static Widget info = 0;
    if (info == 0)
    {
	info = verify(XmCreateInformationDialog(find_shell(w), 
						"gdb_status_dialog",
						NULL, 0));
	Delay::register_shell(info);
	XtAddCallback(info, XmNhelpCallback,   ImmediateHelpCB, NULL);

	Widget cancel = XmMessageBoxGetChild(info, XmDIALOG_CANCEL_BUTTON);
	XtRemoveAllCallbacks(cancel, XmNactivateCallback);
	XtAddCallback(cancel, XmNactivateCallback, ShowGDBStatusCB, 
		      client_data);
    }

    MString status;

    status += cr() + sl("GENERAL DEBUGGER INFORMATION") + cr();
    status += bf("Debugger: ") + tt(gdb->path()) + cr();
    status += bf("Process ID: ") + tt(itostring(gdb->pid())) + cr();
    status += bf("Master TTY: ") + tt(gdb->master_tty()) + cr();
    status += bf("Slave TTY: ") + tt(gdb->slave_tty()) + cr();
    status += bf("Current state: ");
    if (gdb->isReadyWithPrompt())
	status += rm("ready") + cr();
    else
	status += rm("busy") + cr();

    status += cr() + sl("DEBUGGER CAPABILITIES") + cr();
    status += bf("Debugger type: ") + rm(gdb->title()) + cr();
    status += has("clear",   gdb->has_clear_command());
    status += has("display", gdb->has_display_command());
    status += has("edit",    gdb->has_setenv_command());
    status += has("frame",   gdb->has_frame_command());
    status += has("output",  gdb->has_output_command());
    status += has("pwd",     gdb->has_pwd_command());
    status += has("run_io",  gdb->has_run_io_command());
    status += has("setenv",  gdb->has_setenv_command());
    status += has("when",    gdb->has_when_command());

    status += cr() + sl("DEBUGGER COMMANDS") + cr();
    status += cmd("Args",        gdb->info_args_command());
    status += cmd("Assign",      gdb->assign_command("VAR", "EXPR"));
    status += cmd("Disassemble", gdb->disassemble_command("deadbeef"));
    status += cmd("Display",     gdb->display_command());
    status += cmd("Echo",        gdb->echo_command("TEXT"));
    status += cmd("Frame",       gdb->frame_command());
    status += cmd("Locals",      gdb->info_locals_command());
    status += cmd("Pwd",         gdb->pwd_command());
    status += cmd("Whatis",      gdb->whatis_command("EXPR"));
    status += cmd("Where",       gdb->where_command());

    status += cr() + sl("EXPRESSIONS") + cr();
    status += bf("Current language class: ");
    switch (gdb->program_language())
    {
    case LANGUAGE_C:       status += rm("C"); break;
    case LANGUAGE_PASCAL:  status += rm("Pascal"); break;
    case LANGUAGE_CHILL:   status += rm("Chill"); break;
    case LANGUAGE_FORTRAN: status += rm("FORTRAN"); break;
    case LANGUAGE_OTHER:   status += rm("-/-"); break;
    }
    status += cr();
    status += expr("Address",     gdb->address_expr("EXPR"));
    status += expr("Dereference", gdb->dereferenced_expr("EXPR"));

    status += cr() + sl("OPTIONS") + cr();
    status += option("print", "-r", gdb->has_print_r_option());
    status += option("where", "-h", gdb->has_where_h_option());

    status += cr() + sl("SYNTAX") + cr();
    status += cap(bf("Named values"), gdb->has_named_values());
    status += cap(bf("Semicolon after ") + tb(quote("when")),
		  gdb->has_when_semicolon());
    status += cap(bf("Stderr redirection via ") + tb(quote(">&")),
		  gdb->has_err_redirection());
    status += cap(tb(quote("delete")) + bf(" wants comma-separated args"), 
		  gdb->has_delete_comma());

    XtVaSetValues(info,
		  XmNmessageString, status.xmstring(),
		  NULL);

    XtManageChild(info);
}
#endif // SHOW_GDB_STATUS

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

static void set_label(Widget w, string label)
{
    MString new_label(label);
    XmString old_label;
    XtVaGetValues(w, XmNlabelString, &old_label, NULL);
    if (!XmStringCompare(new_label.xmstring(), old_label))
    {
	XtVaSetValues(w,
		      XmNlabelString, new_label.xmstring(),
		      NULL);
    }
    XmStringFree(old_label);
}

void update_arg_buttons()
{
    string arg = source_arg->get_string();

    bool can_find = (arg != "");

    set_sensitive(arg_cmd_area[ArgItems::FindBackward].widget, can_find);
    set_sensitive(arg_cmd_area[ArgItems::FindForward].widget,  can_find);

    bool can_print = 
	can_find && (arg.contains("::") || !arg.contains(":"));

    set_sensitive(arg_cmd_area[ArgItems::Print].widget, can_print);
    set_sensitive(arg_cmd_area[ArgItems::Display].widget, can_print);

    bool have_break = have_break_at_arg();
    if (have_break)
	set_label(arg_cmd_area[ArgItems::Break].widget, "Clear at ()");
    else
	set_label(arg_cmd_area[ArgItems::Break].widget, "Break at ()");

    manage_child(break_menu[BreakItems::TempBreak].widget, !have_break);
    manage_child(break_menu[BreakItems::Enable].widget,    have_break);
    manage_child(break_menu[BreakItems::ContUntil].widget, !have_break);

    bool break_enabled = break_enabled_at_arg();
    if (break_enabled)
	set_label(break_menu[BreakItems::Enable].widget, 
		  "Disable Breakpoint at ()");
    else
	set_label(break_menu[BreakItems::Enable].widget, 
		  "Enable Breakpoint at ()");
}

static void source_argHP(void *, void *, void *)
{
    update_arg_buttons();
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

// Make DDD menu entries sensitive or insensitive
static void ReadyCB(XtPointer client_data = 0, XtIntervalId *id = 0)
{
    if (id != 0)
    {
	XtIntervalId& timer = *((XtIntervalId *)client_data);
	assert(timer == *id);
	timer = 0;
    }

    bool ready = gdb->isReadyWithPrompt() && emptyCommandQueue();

    set_sensitive(stack_w,        ready);
    set_sensitive(registers_w,    ready && gdb->has_regs_command());
    set_sensitive(threads_w,      ready && gdb->type() == GDB);
    set_sensitive(infos_w,        ready && gdb->type() == GDB);
    set_sensitive(up_w,           ready);
    set_sensitive(down_w,         ready);
    set_sensitive(isearch_prev_w, ready);
    set_sensitive(isearch_next_w, ready);
    set_sensitive(isearch_exit_w, ready);
    set_sensitive(complete_w,     ready && gdb->type() == GDB);
    set_sensitive(apply_w,        ready);

    set_sensitive(command_file_menu[FileItems::OpenFile].widget,       ready);
    set_sensitive(source_file_menu[FileItems::OpenFile].widget,        ready);
    set_sensitive(data_file_menu[FileItems::OpenFile].widget,          ready);

    set_sensitive(command_file_menu[FileItems::OpenCore].widget,       ready);
    set_sensitive(source_file_menu[FileItems::OpenCore].widget,        ready);
    set_sensitive(data_file_menu[FileItems::OpenCore].widget,          ready);

    set_sensitive(command_edit_menu[FileItems::OpenSession].widget,    ready);
    set_sensitive(source_edit_menu[FileItems::OpenSession].widget,     ready);
    set_sensitive(data_edit_menu[FileItems::OpenSession].widget,       ready);

    set_sensitive(command_edit_menu[FileItems::SaveSession].widget,    ready);
    set_sensitive(source_edit_menu[FileItems::SaveSession].widget,     ready);
    set_sensitive(data_edit_menu[FileItems::SaveSession].widget,       ready);

    set_sensitive(command_file_menu[FileItems::Attach].widget,         ready);
    set_sensitive(source_file_menu[FileItems::Attach].widget,          ready);
    set_sensitive(data_file_menu[FileItems::Attach].widget,            ready);

    set_sensitive(command_file_menu[FileItems::Detach].widget,         ready);
    set_sensitive(source_file_menu[FileItems::Detach].widget,          ready);
    set_sensitive(data_file_menu[FileItems::Detach].widget,            ready);

    set_sensitive(command_file_menu[FileItems::Make].widget,           ready);
    set_sensitive(source_file_menu[FileItems::Make].widget,            ready);
    set_sensitive(data_file_menu[FileItems::Make].widget,              ready);

    set_sensitive(command_file_menu[FileItems::MakeAgain].widget,      ready);
    set_sensitive(source_file_menu[FileItems::MakeAgain].widget,       ready);
    set_sensitive(data_file_menu[FileItems::MakeAgain].widget,         ready);

    set_sensitive(command_file_menu[FileItems::CD].widget,             ready);
    set_sensitive(source_file_menu[FileItems::CD].widget,              ready);
    set_sensitive(data_file_menu[FileItems::CD].widget,                ready);

    set_sensitive(command_edit_menu[EditItems::Settings].widget,
		  ready && (gdb->type() == GDB || gdb->type() == DBX));
    set_sensitive(source_edit_menu[EditItems::Settings].widget,
		  ready && (gdb->type() == GDB || gdb->type() == DBX));
    set_sensitive(data_edit_menu[EditItems::Settings].widget,
		  ready && (gdb->type() == GDB || gdb->type() == DBX));

    set_sensitive(command_program_menu[ProgramItems::Run].widget,      ready);
    set_sensitive(source_program_menu[ProgramItems::Run].widget,       ready);
    set_sensitive(data_program_menu[ProgramItems::Run].widget,         ready);

    set_sensitive(command_program_menu[ProgramItems::RunAgain].widget, ready);
    set_sensitive(source_program_menu[ProgramItems::RunAgain].widget,  ready);
    set_sensitive(data_program_menu[ProgramItems::RunAgain].widget,    ready);

    set_sensitive(command_program_menu[ProgramItems::Step].widget,     ready);
    set_sensitive(source_program_menu[ProgramItems::Step].widget,      ready);
    set_sensitive(data_program_menu[ProgramItems::Step].widget,        ready);

    set_sensitive(command_program_menu[ProgramItems::Stepi].widget,    ready);
    set_sensitive(source_program_menu[ProgramItems::Stepi].widget,     ready);
    set_sensitive(data_program_menu[ProgramItems::Stepi].widget,       ready);

    set_sensitive(command_program_menu[ProgramItems::Next].widget,     ready);
    set_sensitive(source_program_menu[ProgramItems::Next].widget,      ready);
    set_sensitive(data_program_menu[ProgramItems::Next].widget,        ready);

    set_sensitive(command_program_menu[ProgramItems::Nexti].widget,    ready);
    set_sensitive(source_program_menu[ProgramItems::Nexti].widget,     ready);
    set_sensitive(data_program_menu[ProgramItems::Nexti].widget,       ready);

    set_sensitive(command_program_menu[ProgramItems::Cont].widget,     ready);
    set_sensitive(source_program_menu[ProgramItems::Cont].widget,      ready);
    set_sensitive(data_program_menu[ProgramItems::Cont].widget,        ready);

    set_sensitive(command_program_menu[ProgramItems::Finish].widget,   ready);
    set_sensitive(source_program_menu[ProgramItems::Finish].widget,    ready);
    set_sensitive(data_program_menu[ProgramItems::Finish].widget,      ready);

    set_sensitive(command_program_menu[ProgramItems::Kill].widget,     ready);
    set_sensitive(source_program_menu[ProgramItems::Kill].widget,      ready);
    set_sensitive(data_program_menu[ProgramItems::Kill].widget,        ready);

    unpost_gdb_busy();
}

static void gdb_readyHP(Agent *, void *, void *call_data)
{
    bool gdb_ready = bool(call_data);
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

	// If the debugged program has grabbed the pointer, ungrab it
	ungrab_mouse_pointer();

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


    // To avoid excessive sensitivity changes, we wait 50ms before
    // enabling sensitivity.
    static XtIntervalId ready_timer = 0;

    if (ready_timer != 0)
    {
	XtRemoveTimeOut(ready_timer);
	ready_timer = 0;
    }

    bool ready = gdb_ready && emptyCommandQueue();
    if (!ready)
    {
	// Get unready right now
	ReadyCB();
    }
    else
    {
	// Get ready in 50ms
	ready_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w),
				      50, ReadyCB, XtPointer(&ready_timer));
    }

    // Some stuff that must be executed every other time
    fix_status_size();
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
#if 0
    if (!remote_gdb())
    {
	static bool warned = false;

	if (!warned)
	{
	    // Echo should not happen with a local DDD.
	    post_warning(gdb->title() + " is running in echo mode.",
			 "gdb_echo_warning");
	    warned = true;
	}
    }
#endif

    // Here is a more subtle warning.
    set_status(gdb->title() + " is running in echo mode.");

    // Disable echo mode explicitly via stty command.
    gdb_command(gdb->shell_command("stty -echo -onlcr"), 0, 0, 0, 
		false, false, COMMAND_PRIORITY_AGAIN);
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
void gdb_ctrl(char ctrl)
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
void _gdb_out(string text)
{
    if (text == "")
	return;
    if (private_gdb_output)
	return;

    gdb_input_at_prompt = gdb->ends_with_prompt(text);

    if (promptPosition == 0)
	promptPosition = XmTextGetLastPosition(gdb_w);

    private_gdb_output = true;

    // Don't care for CR if followed by NL
#if RUNTIME_REGEX
    static regex rxcrlf("\r\r*\n");
#endif
    text.gsub(rxcrlf, "\n");

    // Don't care for strings to be ignored
    static string empty;
    if (gdb_out_ignore != "")
	text.gsub(gdb_out_ignore, empty);

    // Pass TEXT to various functions
    set_buttons_from_gdb(console_buttons_w, text);
    set_buttons_from_gdb(source_buttons_w, text);
    set_buttons_from_gdb(data_buttons_w, text);
    set_buttons_from_gdb(tool_bar_w, text);
    set_status_from_gdb(text);
    set_tty_from_gdb(text);

    // Output TEXT on TTY
    tty_out(text);
    bool line_buffered = app_data.line_buffered_console;

    // Output TEXT in debugger console
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
	    text = "";
	}

	XmTextInsert(gdb_w, promptPosition, (String)block);
	promptPosition += block.length();
	// XmTextShowPosition(gdb_w, promptPosition);

	if (have_ctrl)
	    gdb_ctrl(ctrl);
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
	return DDDWindow(int(client_data));
}

static void gdbCutSelectionCB(Widget w, XtPointer client_data, 
			      XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    DDDWindow win = ddd_window(client_data);
    Boolean success = False;

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow))
	success = XmTextCut(gdb_w, tm);

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextFieldCut(source_arg->widget(), tm);

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

    if (success)
	gdbUnselectAllCB(w, client_data, call_data);
}

static void gdbCopySelectionCB(Widget, XtPointer client_data, 
			       XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);
    
    DDDWindow win = ddd_window(client_data);
    Boolean success = False;

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow))
	success = XmTextCopy(gdb_w, tm);

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextFieldCopy(source_arg->widget(), tm);

    // Try source
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextCopy(source_view->source(), tm);

    // Try code
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextCopy(source_view->code(), tm);

    // Try data arg
    if (!success && (win == DataWindow || win == CommonWindow))
	success = XmTextCopy(DataDisp::graph_selection_w, tm);
}

static void gdbPasteClipboardCB(Widget, XtPointer client_data, XtPointer)
{
    DDDWindow win = ddd_window(client_data);

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
	XmTextFieldPaste(source_arg->widget());
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
    XmTextFieldClearSelection(source_arg->widget(), tm);
    XmTextClearSelection(source_view->source(), tm);
    XmTextClearSelection(source_view->code(), tm);
    XmTextFieldClearSelection(DataDisp::graph_arg->widget(), tm);
    XmTextClearSelection(DataDisp::graph_selection_w, tm);
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

    switch (win)
    {
    case SourceWindow:
    case CommonWindow:
    {
	Widget w = source_arg->widget();
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
    
    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow))
	success = XmTextRemove(gdb_w);

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow))
	success = XmTextFieldRemove(source_arg->widget());

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


static void gdbUpdateEditCB(Widget, XtPointer client_data, XtPointer)
{
    DDDWindow win = ddd_window(client_data);
    XmTextPosition start, end;

    // Check if we have something to cut
    bool can_cut  = false;

    // Try debugger console
    if (!can_cut && (win == GDBWindow || win == CommonWindow))
	can_cut = XmTextGetSelectionPosition(gdb_w, &start, &end);

    // Try source arg
    if (!can_cut && (win == SourceWindow || win == CommonWindow))
	can_cut = XmTextFieldGetSelectionPosition(source_arg->widget(), 
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

    // Check if we have something to paste
    bool can_paste = false;
    switch (win)
    {
    case GDBWindow:
    case CommonWindow:
    case SourceWindow:
	can_paste = true;
	break;

    default:
	break;
    }

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

    if (menu != 0)
    {
	set_sensitive(menu[EditItems::Cut].widget,    can_cut);
	set_sensitive(menu[EditItems::Copy].widget,   can_copy);
	set_sensitive(menu[EditItems::Paste].widget,  can_paste);
	set_sensitive(menu[EditItems::Delete].widget, can_cut);
	set_sensitive(menu[EditItems::UnselectAll].widget, can_copy);
    }
}

static void gdbUpdateFileCB(Widget, XtPointer client_data, XtPointer)
{
    MMDesc *file_menu = (MMDesc *)client_data;

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

void gdbUpdateViewsCB(Widget, XtPointer client_data, XtPointer)
{
    MMDesc *view_menu = (MMDesc *)client_data;

    set_toggle(view_menu[DataWindow].widget,   have_visible_data_window());
    set_toggle(view_menu[SourceWindow].widget, 
	       have_visible_source_window() || have_visible_tool_window());
    set_toggle(view_menu[GDBWindow].widget,    have_visible_command_window());
}

// Language changed - re-label buttons
static void language_changedHP(Agent *source, void *, void *)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    assert(gdb != 0);

    MString print_ref_label("Print " + gdb->dereferenced_expr("()"));
    XtVaSetValues(print_menu[PrintItems::PrintRef].widget,
		  XmNlabelString, print_ref_label.xmstring(),
		  NULL);

    MString disp_ref_label("Display " + gdb->dereferenced_expr("()"));
    XtVaSetValues(display_menu[DispItems::DispRef].widget,
		  XmNlabelString, disp_ref_label.xmstring(),
		  NULL);
}


//-----------------------------------------------------------------------------
// Configure new shell
//-----------------------------------------------------------------------------

static void decorate_new_shell(Widget w)
{
    if (w == 0)
	return;

    Widget shell = findShellParent(w);

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

    // If the window manager frame is more than 5 pixels larger than
    // the shell window, assume the shell is decorated.
    return frame_attributes.height - shell_attributes.height > 5;
}


//-----------------------------------------------------------------------------
// Startup Logo
//-----------------------------------------------------------------------------

static Widget logo_shell  = 0;
static Pixmap logo_pixmap;
static _Delay *logo_delay = 0;

static void popdown_startup_logo(XtPointer data, XtIntervalId *id)
{
    (void) id;			// use it

    if (data != 0)
    {
	XtIntervalId *timer = (XtIntervalId *)data;
	assert(*timer == *id);
	*timer = 0;
    }
    
    if (logo_shell != 0)
    {
	XFreePixmap(XtDisplay(logo_shell), logo_pixmap);

	popdown_shell(logo_shell);
	DestroyWhenIdle(logo_shell);
	logo_shell = 0;

	delete logo_delay;
	logo_delay = 0;
    }
}

static void popup_startup_logo(Widget parent, string color_key)
{
    popdown_startup_logo();

    last_shown_startup_logo = color_key;

    if (color_key == "")
	return;

    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNallowShellResize, True); arg++;
    XtSetArg(args[arg], XmNborderWidth, 0); arg++;
    logo_shell = XtCreatePopupShell("logo_shell", overrideShellWidgetClass, 
				    parent, args, arg);

    arg = 0;
    XtSetArg(args[arg], XmNlabelType, XmPIXMAP); arg++;
    XtSetArg(args[arg], XmNallowResize, True); arg++;
    Widget logo = XmCreateLabel(logo_shell, "logo", args, arg);
    XtManageChild(logo);
    XtRealizeWidget(logo_shell);

    logo_delay = new _Delay(logo_shell);

    logo_pixmap = dddlogo(logo, color_key);
    XtVaSetValues(logo, XmNlabelPixmap, logo_pixmap, NULL);

    Dimension width, height;
    XtVaGetValues(logo_shell, XmNwidth, &width, XmNheight, &height, NULL);

    int x = (WidthOfScreen(XtScreen(logo_shell)) - width) / 2;
    int y = (HeightOfScreen(XtScreen(logo_shell)) - height) / 2;

    XtVaSetValues(logo_shell, XmNx, x, XmNy, y, NULL);

    // Place lock warning on top of startup logo
    lock_dialog_x = x + 20;
    lock_dialog_y = y + 20;

    popup_shell(logo_shell);
    wait_until_mapped(logo, logo_shell);
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

	    int len = XLookupString((XKeyEvent *)event, buffer, sizeof buffer,
				    &keysym, NULL);
	    if (len == 1 && (buffer[0] == '\003' || buffer[0] == '\034'))
	    {
		// Interrupt: ^C or ^\ found in queue
		return True;
	    }
	}
	return False;

    default:
	return False;
    }
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

//-----------------------------------------------------------------------------
// Various setups
//-----------------------------------------------------------------------------

static void setup_version_info()
{
    string cinfo = string(config_info).before("\n");
    while (cinfo.contains(' ', -1))
	cinfo = cinfo.before(int(cinfo.length()) - 1);

    int cinfo_lt = cinfo.index('<');
    int cinfo_gt = cinfo.index('>');
    if (cinfo_lt >= 0 && cinfo_gt >= 0)
    {
	helpOnVersionExtraText = rm(cinfo.through(cinfo_lt));
	helpOnVersionExtraText += cr();
	helpOnVersionExtraText += tt(cinfo(cinfo_lt + 1, 
					   cinfo_gt - cinfo_lt - 1));
	helpOnVersionExtraText += rm(cinfo.from(cinfo_gt));
    }
    else
    {
	helpOnVersionExtraText = rm(cinfo);
    }
    helpOnVersionExtraText += cr();

    string expires = ddd_expiration_date();
    if (expires != "")
    {
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
	+ rm(DDD_NAME " License for details.") + cr()
	+ cr()
	+ sl(DDD_NAME " needs your support!")
	+ rm(" If you have any " DDD_NAME " success stories, ") + cr()
        + rm("please write them down on a picture postcard "
	     "and send them to us:") + cr()
	+ cr()
	+ rm("    Technische Universit\344t Braunschweig") + cr()
	+ rm("    Abteilung Softwaretechnologie") + cr()
	+ rm("    B\374ltenweg 88") + cr()
	+ rm("    D-38092 Braunschweig") + cr()
	+ rm("    GERMANY") + cr()
	+ cr()
	+ rm("Send bug reports to <")
	+ tt(ddd_NAME "-bugs@ips.cs.tu-bs.de") + rm(">;") + cr()
	+ rm("see the " DDD_NAME " manual "
	     "for details on reporting bugs.") + cr()
	+ rm("Send comments and suggestions to <")
	+ tt(ddd_NAME "@ips.cs.tu-bs.de") + rm(">.") + cr()
	+ cr()
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
	// In XDB, we have no means to set the TTY type afterwards;
	// Set the execution TTY type right now.
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

static void setup_command_tool(bool iconic)
{
    Widget tool_shell_parent = 
	source_view_shell ? source_view_shell : command_shell;

    Position pos_x, pos_y;
    get_transient_pos(XtScreen(tool_shell_parent), pos_x, pos_y);

    ostrstream os;
    os << "+" << pos_x << "+" << pos_y;
    string geometry(os);

    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNgeometry, geometry.chars());   arg++;
    XtSetArg(args[arg], XmNx, pos_x);                     arg++;
    XtSetArg(args[arg], XmNy, pos_y);                     arg++;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    XtSetArg(args[arg], XmNallowShellResize, False);      arg++;
    XtSetArg(args[arg], XmNmwmDecorations,
	     MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU); arg++;

    // It is preferable to realize the command tool as a DialogShell,
    // since this will cause it to stay on top of other DDD windows.
    // Unfortunately, some window managers do not decorate transient
    // windows such as DialogShells.  In this case, use a TopLevel
    // shell instead and rely on the DDD auto-raise mechanisms defined
    // in `windows.C'.
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

    if (app_data.tool_bar)
    {
	// The command tool is not needed, as we have a tool bar.
    }
    else if (!app_data.source_window)
    {
	// We have no source window, and thus no command tool.
    }
    else if (source_view_shell || iconic)
    {
	// We don't need the command tool right now - wait for source
	// window to map
    }
    else
    {
	// OK, raise the command tool
	initial_popup_shell(tool_shell);
    }
}

static void setup_options(int argc, char *argv[],
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
	    || arg == "--xdb" || arg == "-xdb")
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
    if (app_data.app_defaults_version == 0)
    {
	cerr << "Warning: no version information in `" 
	     << DDD_CLASS_NAME "' app-defaults-file\n";

	version_warnings += rm("No version information in `")
	    + tt(DDD_CLASS_NAME) + rm("' app-defaults-file") + cr();
    }
    else if (string(app_data.app_defaults_version) != DDD_VERSION)
    {
	cerr << "Warning: using `" DDD_CLASS_NAME "' app-defaults file"
	     << " for " DDD_NAME " " << app_data.app_defaults_version 
	     << " (this is " DDD_NAME " " DDD_VERSION ")\n";

	version_warnings += rm("Using `") + tt(DDD_CLASS_NAME)
	    + rm("' app-defaults file for " DDD_NAME " ")
	    + rm(app_data.app_defaults_version)
	    + rm(" (this is " DDD_NAME " " DDD_VERSION ")") + cr();
    }

    if (app_data.dddinit_version && 
	string(app_data.dddinit_version) != DDD_VERSION)
    {
	cerr << "Warning: using "
	     << quote(session_state_file(app_data.session))
	     << " file for " DDD_NAME " " << app_data.dddinit_version
	     << "\n(this is " DDD_NAME " " DDD_VERSION ")."
	     << "  Please save options.\n";

	version_warnings += rm("Using `")
	    + tt(cook(session_state_file(app_data.session)))
	    + rm("' file for " DDD_NAME " ")
	    + rm(app_data.dddinit_version) + cr()
	    + rm("(this is " DDD_NAME " " DDD_VERSION ").  "
		 "Please save options.");
    }
}
