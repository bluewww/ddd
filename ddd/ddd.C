// $Id$
// DDD main program (and much more)

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de)
// and Andreas Zeller (zeller@ips.cs.tu-bs.de)
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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

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


char ddd_rcsid[] =
    "$Id$";

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
#include <Xm/Frame.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>	// XmCreateWorkArea()
#include <Xm/Protocols.h>
#include <Xm/SelectioB.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <X11/Shell.h>

#ifdef HAVE_X11_XMU_EDITRES_H
#include <X11/Xmu/Editres.h>
#endif

// Lots of DDD stuff
#include "AgentM.h"
#include "AppData.h"
#include "ArgField.h"
#include "DataDisp.h"
#include "ExitCB.h"
#include "GraphEdit.h"
#include "GDBAgent.h"
#include "MakeMenu.h"
#include "SourceView.h"
#include "VSEFlags.h"
#include "editing.h"
#include "args.h"
#include "assert.h"
#include "bool.h"
#include "buttons.h"
#include "cmdtty.h"
#include "comm-manag.h"
#include "commandQ.h"
#include "complete.h"
#include "configinfo.h"
#include "converters.h"
#include "cook.h"
#include "dbx-lookup.h"
#include "editing.h"
#include "exectty.h"
#include "exit.h"
#include "expired.h"
#include "file.h"
#include "findParent.h"
#include "gdbinit.h"
#include "graph.h"
#include "history.h"
#include "logo.h"
#include "options.h"
#include "post.h"
#include "print.h"
#include "question.h"
#include "resources.h"
#include "sashes.h"
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

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

//-----------------------------------------------------------------------------
// Forward function decls
//-----------------------------------------------------------------------------

// Callbacks
void gdb_ready_for_questionHP (void *, void *, void *);
void gdb_ready_for_cmdHP      (void *, void *, void *);
void gdb_eofHP                (Agent *, void *, void *);
void source_argHP (void*, void*, void* call_data);

// Setup
static Boolean ddd_setup_done(XtPointer client_data);

// Warning proc
static void ddd_xt_warning(String message);

// Event handling
static void process_pending_events();

// Cut and Paste
void gdbCutSelectionCB    (Widget, XtPointer, XtPointer);
void gdbCopySelectionCB   (Widget, XtPointer, XtPointer);
void gdbPasteClipboardCB  (Widget, XtPointer, XtPointer);
void gdbClearSelectionCB  (Widget, XtPointer, XtPointer);
void gdbDeleteSelectionCB (Widget, XtPointer, XtPointer);
void gdbUpdateEditCB      (Widget, XtPointer, XtPointer);
void gdbUpdateViewCB      (Widget, XtPointer, XtPointer);

// Preferences
static void make_preferences (Widget parent);
static void dddPopupPreferencesCB (Widget, XtPointer, XtPointer);
static void update_reset_preferences();

// User emergencies (Ctrl-C)
static void check_emergencies();

// Create status line
static void create_status(Widget parent);

// Status LED
static void blink(bool set);

// Callbacks
static void ActivateCB(Widget, XtPointer client_data, XtPointer call_data);


//-----------------------------------------------------------------------------
// Xt Stuff
//-----------------------------------------------------------------------------

static char S_true[]  = "true";
static char S_false[] = "false";

// Options
// Note: we support both the GDB '--OPTION' and the X '-OPTION' convention.
static XrmOptionDescRec options[] = {
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

{ "--resources",            XtNshowResources,        XrmoptionNoArg, S_true },
{ "-resources",             XtNshowResources,        XrmoptionNoArg, S_true },

{ "--manual",               XtNshowManual,           XrmoptionNoArg, S_true },
{ "-manual",                XtNshowManual,           XrmoptionNoArg, S_true },

{ "--license",              XtNshowLicense,          XrmoptionNoArg, S_true },
{ "-license",               XtNshowLicense,          XrmoptionNoArg, S_true },

{ "--check-configuration",  XtNcheckConfiguration,   XrmoptionNoArg, S_true },
{ "-check-configuration",   XtNcheckConfiguration,   XrmoptionNoArg, S_true },

{ "--help",                 XtNshowInvocation,       XrmoptionNoArg, S_true },
{ "-help",                  XtNshowInvocation,       XrmoptionNoArg, S_true },
{ "-h",                     XtNshowInvocation,       XrmoptionNoArg, S_true },
{ "--?",                    XtNshowInvocation,       XrmoptionNoArg, S_true },
{ "-?",                     XtNshowInvocation,       XrmoptionNoArg, S_true },

};

// Actions
XtActionsRec actions [] = {
    {"gdb-control",            controlAct},
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

// Resources
String ddd_fallback_resources[] = {
#include "Ddd.ad.h"
0
};

//-----------------------------------------------------------------------------
// Menus
//-----------------------------------------------------------------------------

static MMDesc file_menu[] = 
{
    { "open_file",   MMPush, { gdbOpenFileCB }},
    { "open_core",   MMPush, { gdbOpenCoreCB }},
    { "open_source", MMPush, { gdbOpenSourceCB }},
    MMSep,
    { "print",      MMPush,  { graphPrintCB }},
    { "quickPrint", MMPush,  { graphQuickPrintCB }},
    MMSep,
    { "close",       MMPush, { DDDCloseCB }},
    { "restart",     MMPush, { DDDRestartCB }},
    { "exit",        MMPush, { DDDExitCB }},
    MMEnd
};

static MMDesc program_menu[] =
{
    { "run",         MMPush, { gdbRunCB }},
    { "run_again",   MMPush, { gdbCommandCB, "run" }},
    MMSep,
    { "step",        MMPush, { gdbCommandCB, "step" }},
    { "stepi",       MMPush, { gdbCommandCB, "stepi" }},
    { "next",        MMPush, { gdbCommandCB, "next" }},
    { "nexti",       MMPush, { gdbCommandCB, "nexti" }},
    MMSep,
    { "cont",        MMPush, { gdbCommandCB, "cont" }},
    { "finish",      MMPush, { gdbCommandCB, "finish" }},
    MMSep,
    { "kill",        MMPush, { gdbCommandCB, "kill" }},
    { "break",       MMPush, { gdbCommandCB, "\003" }},
    { "quit",        MMPush, { gdbCommandCB, "\034" }},
    MMEnd
};

enum DDDWindow { ToolWindow, GDBWindow, SourceWindow, DataWindow, ExecWindow };

static MMDesc command_view_menu[] =
{
    { "tool",       MMPush, { gdbOpenToolWindowCB }},
    { "console",    MMPush, { gdbOpenCommandWindowCB }},
    { "source",     MMPush, { gdbOpenSourceWindowCB }},
    { "data",       MMPush, { gdbOpenDataWindowCB }},
    { "exec",       MMPush, { gdbOpenExecWindowCB }},
    MMEnd
};

static MMDesc source_view_menu[] =
{
    { "tool",       MMPush, { gdbOpenToolWindowCB }},
    { "console",    MMPush, { gdbOpenCommandWindowCB }},
    { "source",     MMPush, { gdbOpenSourceWindowCB }},
    { "data",       MMPush, { gdbOpenDataWindowCB }},
    { "exec",       MMPush, { gdbOpenExecWindowCB }},
    MMEnd
};

static MMDesc data_view_menu[] =
{
    { "tool",       MMPush, { gdbOpenToolWindowCB }},
    { "console",    MMPush, { gdbOpenCommandWindowCB }},
    { "source",     MMPush, { gdbOpenSourceWindowCB }},
    { "data",       MMPush, { gdbOpenDataWindowCB }},
    { "exec",       MMPush, { gdbOpenExecWindowCB }},
    MMEnd
};

struct EditItems {
    enum EditItem { Cut, Copy, Paste, Dummy, Clear, Delete };
};

static MMDesc command_edit_menu[] = 
{
    { "cut",    MMPush, { gdbCutSelectionCB,    XtPointer(GDBWindow) }},
    { "copy",   MMPush, { gdbCopySelectionCB,   XtPointer(GDBWindow) }},
    { "paste",  MMPush, { gdbPasteClipboardCB,  XtPointer(GDBWindow) }},
    MMSep,
    { "clear",  MMPush, { gdbClearSelectionCB,  XtPointer(GDBWindow) }},
    { "delete", MMPush, { gdbDeleteSelectionCB, XtPointer(GDBWindow) }},
    MMEnd
};

static MMDesc source_edit_menu[] = 
{
    { "cut",    MMPush, { gdbCutSelectionCB,    XtPointer(SourceWindow) }},
    { "copy",   MMPush, { gdbCopySelectionCB,   XtPointer(SourceWindow) }},
    { "paste",  MMPush, { gdbPasteClipboardCB,  XtPointer(SourceWindow) }},
    MMSep,
    { "clear",  MMPush, { gdbClearSelectionCB,  XtPointer(SourceWindow) }},
    { "delete", MMPush, { gdbDeleteSelectionCB, XtPointer(SourceWindow) }},
    MMEnd
};

static MMDesc data_edit_menu[] = 
{
    { "cut",    MMPush | MMInsensitive, 
          { gdbCutSelectionCB,    XtPointer(DataWindow) }},
    { "copy",   MMPush, 
          { gdbCopySelectionCB,   XtPointer(DataWindow) }},
    { "paste",  MMPush | MMInsensitive,
          { gdbPasteClipboardCB,  XtPointer(DataWindow) }},
    MMSep,
    { "clear",  MMPush | MMInsensitive, 
	  { gdbClearSelectionCB,  XtPointer(DataWindow) }},
    { "delete", MMPush | MMInsensitive, 
	  { gdbDeleteSelectionCB, XtPointer(DataWindow) }},
    MMEnd
};

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
    { "complete", MMPush, { gdbCompleteCB }},
    { "apply",    MMPush, { gdbApplyCB }},
    MMSep,
    { "clear_line",   MMPush, { gdbClearCB }},
    { "clear_window", MMPush, { gdbClearWindowCB }},
    MMEnd
};

static Widget stack_w;
static Widget registers_w;
static Widget threads_w;
static Widget infos_w;

static MMDesc stack_menu[] =
{
    { "stack",      MMPush,  { SourceView::ViewStackFramesCB }, 
      NULL, &stack_w },
    { "registers",  MMPush,  { SourceView::ViewRegistersCB },
      NULL, &registers_w },
    { "threads",    MMPush,  { SourceView::ViewThreadsCB },
      NULL, &threads_w },
    MMSep,
    { "infos",      MMPush,  { dddPopupInfosCB },
      NULL, &infos_w },
    MMSep,
    { "up",         MMPush,  { gdbCommandCB, "up" }},
    { "down",       MMPush,  { gdbCommandCB, "down" }},
    MMEnd
};

static MMDesc source_menu[] =
{
    { "breakpoints", MMPush, { SourceView::EditBreakpointsCB }},
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
static Widget suppress_warnings_w;
static Widget save_history_on_exit_w;
static Widget ungrab_mouse_pointer_w;

static MMDesc general_preferences_menu[] = 
{
    { "buttonHints",         MMButtonPanel, MMNoCB, button_menu },
    { "valueHints",          MMButtonPanel, MMNoCB, value_menu },
    { "tabCompletion",       MMRadioPanel,  MMNoCB, completion_menu },
    { "groupIconify",        MMToggle, { dddToggleGroupIconifyCB },
      NULL, &group_iconify_w },
    { "suppressWarnings",    MMToggle, { dddToggleSuppressWarningsCB },
      NULL, &suppress_warnings_w },
    { "ungrabMousePointer",  MMToggle, { dddToggleUngrabMousePointerCB },
      NULL, &ungrab_mouse_pointer_w },
    { "saveHistoryOnExit",   MMToggle, { dddToggleSaveHistoryOnExitCB },
      NULL, &save_history_on_exit_w },
    MMEnd
};


// Source preferences
static Widget cache_source_files_w;
static Widget cache_machine_code_w;
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


static Widget refer_sources_w;
static MMDesc source_preferences_menu[] = 
{
    { "showExecPos",      MMRadioPanel, MMNoCB, glyph_menu },
    { "referSources",     MMRadioPanel, MMNoCB, refer_menu, &refer_sources_w },
    { "cacheSourceFiles", MMToggle, { sourceToggleCacheSourceFilesCB }, 
      NULL, &cache_source_files_w },
    { "cacheMachineCode", MMToggle, { sourceToggleCacheMachineCodeCB }, 
      NULL, &cache_machine_code_w },
    { "tabWidth", MMScale, { sourceSetTabWidthCB },
      NULL, &tab_width_w },
    MMEnd
};


// Data preferences
static Widget graph_detect_aliases_w;
static Widget graph_show_hints_w;
static Widget graph_snap_to_grid_w;
static Widget graph_compact_layout_w;
static Widget graph_auto_layout_w;
static Widget graph_grid_size_w;

static MMDesc data_preferences_menu[] = 
{
    { "detectAliases", MMToggle | MMInsensitive,
      { graphToggleDetectAliasesCB },
      NULL, &graph_detect_aliases_w },
    { "showHints",     MMToggle,  { graphToggleShowHintsCB },
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

static MMDesc startup_preferences_menu [] =
{
    { "windows",         MMRadioPanel, MMNoCB, window_mode_menu },
    { "statusPosition",  MMRadioPanel, MMNoCB, status_position_menu },
    { "toolButtons",     MMRadioPanel, MMNoCB, tool_buttons_menu },
    { "keyboardFocus",   MMRadioPanel, MMNoCB, keyboard_focus_menu },
    { "dataScrolling",   MMRadioPanel, MMNoCB, data_scrolling_menu },
    { "debugger",        MMRadioPanel, MMNoCB, debugger_menu },
    MMEnd
};


// Options
// All these widgets come in four times:
// w[0] holds the last created widget.
// w[<DDDOption>] holds the widget for the menu in window <DDDOption>.

enum DDDOption { DummyOptions   = 0, 
		 CommandOptions = 1, 
		 SourceOptions  = 2,
		 DataOptions    = 3 };

static Widget separate_exec_window_w[4];
static Widget find_words_only_w[4];
static Widget disassemble_w[4];
static void set_option_widgets(DDDOption opt);

static Widget settings_w;

static MMDesc options_menu [] =
{
    { "preferences",         MMPush,  { dddPopupPreferencesCB }},
    { "settings",            MMPush,  { dddPopupSettingsCB },
      NULL, &settings_w },
    MMSep,
    { "separateExecWindow",  MMToggle, { dddToggleSeparateExecWindowCB }, 
      NULL, separate_exec_window_w },
    { "findWordsOnly",       MMToggle, { sourceToggleFindWordsOnlyCB }, 
      NULL, find_words_only_w },
    { "disassemble",         MMToggle,  { sourceToggleDisassembleCB },
      NULL, disassemble_w },
    MMSep,
    { "saveOptions",         MMPush,   { DDDSaveOptionsCB }},
    MMEnd
};


// Data
static Widget info_locals_w = 0;
static Widget info_args_w   = 0;
static Widget dump_w        = 0;
static Widget l_w           = 0;

static MMDesc data_menu[] = 
{
    { "displays",   MMPush,    { DataDisp::EditDisplaysCB }},
    MMSep,
    { "l",           MMToggle | MMUnmanaged, { graphToggleLocalsCB },
      NULL, &l_w },
    { "dump",        MMToggle | MMUnmanaged, { graphToggleLocalsCB },
      NULL, &dump_w },
    { "info locals", MMToggle | MMUnmanaged, { graphToggleLocalsCB },
      NULL, &info_locals_w },
    { "info args",   MMToggle | MMUnmanaged, { graphToggleArgsCB },
      NULL, &info_args_w },
    MMSep,
    { "align",      MMPush,    { graphAlignCB  }},
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
    {"onVersion",   MMPush, { HelpOnVersionCB }},
    MMSep,
    {"onContext",   MMPush, { HelpOnContextCB }},
    {"onWindow",    MMPush, { HelpOnWindowCB }},
    {"onHelp",      MMPush, { HelpOnHelpCB }},
    MMSep,
    {"index",       MMPush, { DDDManualCB }},
    {"license",     MMPush, { DDDLicenseCB }},
    {"www",         MMPush, { DDDWWWPageCB }},
    MMEnd
};

// Menu Bar for DDD command window
static MMDesc command_menubar[] = 
{
    { "file",     MMMenu,          MMNoCB, file_menu },
    { "edit",     MMMenu,          { gdbUpdateEditCB }, command_edit_menu },
    { "options",  MMMenu,          MMNoCB, options_menu },
    { "view",     MMMenu,          { gdbUpdateViewCB }, command_view_menu },
    { "program",  MMMenu,          MMNoCB, program_menu },
    { "commands", MMMenu,          MMNoCB, command_menu },
    { "help",     MMMenu | MMHelp, MMNoCB, help_menu },
    MMEnd
};

// Menu Bar for DDD source view
static MMDesc source_menubar[] = 
{
    { "file",    MMMenu,           MMNoCB, file_menu },
    { "edit",    MMMenu,           { gdbUpdateEditCB }, source_edit_menu },
    { "options", MMMenu,           MMNoCB, options_menu },
    { "view",    MMMenu,           { gdbUpdateViewCB }, source_view_menu },
    { "program", MMMenu,           MMNoCB, program_menu },
    { "stack",   MMMenu,           MMNoCB, stack_menu },
    { "source",  MMMenu,           MMNoCB, source_menu },
    { "help",    MMMenu | MMHelp,  MMNoCB, help_menu },
    MMEnd
};

// Menu Bar for DDD data window
static MMDesc data_menubar[] = 
{
    { "file",    MMMenu,          MMNoCB, file_menu },
    { "edit",    MMMenu,          { gdbUpdateEditCB }, data_edit_menu },
    { "options", MMMenu,          MMNoCB, options_menu },
    { "view",    MMMenu,          { gdbUpdateViewCB }, data_view_menu },
    { "program", MMMenu,          MMNoCB, program_menu },
    { "data",    MMMenu,          MMNoCB, data_menu },
    { "help",    MMMenu | MMHelp, MMNoCB, help_menu },
    MMEnd
};

// Menu Bar for combined DDD data/command window
static MMDesc combined_menubar[] = 
{
    { "file",       MMMenu,       MMNoCB, file_menu },
    { "edit",       MMMenu,       { gdbUpdateEditCB }, command_edit_menu },
    { "options",    MMMenu,       MMNoCB, options_menu },
    { "view",       MMMenu,       { gdbUpdateViewCB }, command_view_menu },
    { "program",    MMMenu,       MMNoCB, program_menu },
    { "commands",   MMMenu,       MMNoCB, command_menu },
    { "stack",      MMMenu,       MMNoCB, stack_menu },
    { "source",     MMMenu,       MMNoCB, source_menu },
    { "data",       MMMenu,       MMNoCB, data_menu },
    { "help", MMMenu | MMHelp,    MMNoCB, help_menu },
    MMEnd
};


struct ArgItems {
    enum ArgCmd { Lookup, Break, Clear, Print, Display, 
		  FindForward, FindBackward };
};

static MMDesc arg_cmd_area[] = 
{
    {"lookup",        MMPush,  { gdbLookupCB        }},
    {"breakAt",       MMPush,  { gdbBreakArgCmdCB   }},
    {"clearAt",       MMPush,  { gdbClearArgCmdCB   }},
    {"print",         MMPush,  { gdbPrintArgCmdCB   }},
    {"display",       MMPush,  { gdbDisplayArgCmdCB }},
    {"findBackward",  MMPush,  { gdbFindBackwardCB  }},
    {"findForward",   MMPush,  { gdbFindForwardCB   }},
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


//-----------------------------------------------------------------------------
// DDD main program
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    // As this is a C++ program, execution does not begin here.  At
    // this point, all global data objects already have been properly
    // initialized.

    // Save environment for restart.
    register_restart(argv);

    // This one is required for error messages
    char *program_name = argc ? argv[0] : ddd_NAME;

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
	    static regex RXoptions("-[bcdeiIopPrRsx]");
	    if (i < argc - 1 && arg.matches(RXoptions))
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

    // Read ~/.dddinit resources
    XrmDatabase dddinit = XrmGetFileDatabase(options_file());
    if (dddinit == 0)
	dddinit = XrmGetStringDatabase("");

    // Let command-line arguments override ~/.dddinit
    XrmParseCommand(&dddinit, options, XtNumber(options), 
		    DDD_CLASS_NAME, &argc, argv);

    XtAppContext app_context;
    Widget toplevel = XtAppInitialize(&app_context,
				      DDD_CLASS_NAME,
				      0, 0,
				      &argc, argv,
				      ddd_fallback_resources,
				      args, arg);

    // Merge in ~/.dddinit resources
    XrmDatabase target = XtDatabase(XtDisplay(toplevel));
    XrmMergeDatabases(dddinit, &target);

    // Set up VSL resources
    if (VSEFlags::parse_vsl(argc, argv))
    {
	// Show VSL usage...
	cout << VSEFlags::explain(true);
	return EXIT_FAILURE;
    }

    // Setup toplevel window
#ifdef HAVE_X11_XMU_EDITRES_H
    XtAddEventHandler(toplevel, EventMask(0), true,
		      XtEventHandler(_XEditResCheckMessages), NULL);
#endif

    XtAppAddActions(app_context, actions, XtNumber(actions));
    Atom WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW", False);

    // Get and save application resources
    XtVaGetApplicationResources(toplevel, (XtPointer)&app_data,
				ddd_resources, ddd_resources_size,
				NULL);

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

    // Check for --version, --help, etc.
    if (app_data.show_version)
	show_version();

    if (app_data.show_invocation)
	show_invocation(type);

    if (app_data.show_configuration)
	show_configuration();

    if (app_data.show_resources)
	show_resources(XtDatabase(XtDisplay(toplevel)));

    if (app_data.show_manual)
	show_manual();

    if (app_data.show_license)
	show_license();

    if (app_data.show_version 
	|| app_data.show_invocation 
	|| app_data.show_configuration
	|| app_data.show_resources
	|| app_data.show_manual
	|| app_data.show_license)
	return EXIT_SUCCESS;

    // Warn for incompatible `Ddd' and `~/.dddinit' files
    if (app_data.app_defaults_version == 0)
    {
	cerr << XtName(toplevel) 
	     << ": warning: no version information in `" 
	    DDD_CLASS_NAME "' app-defaults-file\n";
    }
    else if (string(app_data.app_defaults_version) != DDD_VERSION)
    {
	cerr << XtName(toplevel) 
	     << ": warning: using `" DDD_CLASS_NAME 
	     << "' app-defaults file for " DDD_NAME " " 
	     << app_data.app_defaults_version 
	     << " (this is " DDD_NAME " " DDD_VERSION ")\n";
    }

    if (app_data.dddinit_version && 
	string(app_data.dddinit_version) != DDD_VERSION)
    {
	cerr << XtName(toplevel) 
	     << ": warning: using `~/.dddinit' file for " << DDD_NAME " " 
	     << app_data.dddinit_version
	     << " (this is " DDD_NAME " " DDD_VERSION ").\n"
	     << "Please save options.\n";
    }

    // Register own converters
    registerOwnConverters();

    // Global variables: Set maximum lengths for `shorten' calls
    max_value_tip_length              = app_data.max_value_tip_length;
    max_value_doc_length              = app_data.max_value_doc_length;
    DispBox::max_display_title_length = app_data.max_display_title_length;
    SourceView::max_popup_expr_length = app_data.max_popup_expr_length;

    // Global variables: Set delays for button and value tips
    help_button_tip_delay = app_data.button_tip_delay;
    help_value_tip_delay  = app_data.value_tip_delay;
    help_button_doc_delay = app_data.button_doc_delay;
    help_value_doc_delay  = app_data.value_doc_delay;
    help_clear_doc_delay  = app_data.clear_doc_delay;
    help_clear_tip_delay  = app_data.clear_tip_delay;


    // Create command shell
    arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    command_shell =
	verify(XtCreatePopupShell("command_shell",
				  topLevelShellWidgetClass,
				  toplevel, args, arg));
    XmAddWMProtocolCallback(command_shell,
			    WM_DELETE_WINDOW, gdbCloseCommandWindowCB, 0);
#ifdef HAVE_X11_XMU_EDITRES_H
    XtAddEventHandler(command_shell, EventMask(0), true,
		      XtEventHandler(_XEditResCheckMessages), NULL);
#endif
    Delay::register_shell(command_shell);


    // Create main window
    Widget main_window = 
	verify(XtVaCreateManagedWidget ("main_window", 
					xmMainWindowWidgetClass,
					command_shell,
					NULL));

    // Re-register own converters to override Motif converters.
    registerOwnConverters();

    // Create menu bar
    MMDesc *menubar = combined_menubar;
    if (app_data.separate_data_window && app_data.separate_source_window)
	menubar = command_menubar;

    Widget menubar_w = MMcreateMenuBar (main_window, "menubar", menubar);
    MMaddCallbacks(menubar);

    set_option_widgets(CommandOptions);

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
	XmAddWMProtocolCallback(data_disp_shell,
				WM_DELETE_WINDOW, gdbCloseDataWindowCB, 0);
#ifdef HAVE_X11_XMU_EDITRES_H
	XtAddEventHandler(data_disp_shell, EventMask(0), true,
			  XtEventHandler(_XEditResCheckMessages), NULL);
#endif
	Delay::register_shell(data_disp_shell);

	data_main_window_w = 
	    verify(XtVaCreateManagedWidget("data_main_window",
					   xmMainWindowWidgetClass,
					   data_disp_shell,
					   NULL));

	// Add menu bar
	data_menubar_w = 
	    MMcreateMenuBar (data_main_window_w, "menubar", data_menubar);
	MMaddCallbacks(data_menubar);

	set_option_widgets(DataOptions);

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
	XmAddWMProtocolCallback(source_view_shell,
				WM_DELETE_WINDOW, gdbCloseSourceWindowCB, 0);
#ifdef HAVE_X11_XMU_EDITRES_H
	XtAddEventHandler(source_view_shell, EventMask(0), true,
			  XtEventHandler(_XEditResCheckMessages), NULL);
#endif
	Delay::register_shell(source_view_shell);

	source_main_window_w = 
	    verify(XtVaCreateManagedWidget("source_main_window",
					   xmMainWindowWidgetClass,
					   source_view_shell,
					   NULL));

	// Add menu bar
	source_menubar_w = 
	    MMcreateMenuBar (source_main_window_w, "menubar", source_menubar);
	MMaddCallbacks(source_menubar);

	set_option_widgets(SourceOptions);

	source_view_parent = 
	    verify(XtVaCreateManagedWidget ("source_paned_work_w",
					    xmPanedWindowWidgetClass,
					    source_main_window_w,
					    NULL));

	// Status line
	if (!app_data.status_at_bottom)
	    create_status(source_view_parent);
    }

    source_view = new SourceView(app_context,
				 source_view_parent,
				 app_data.indent_amount);

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

    XtVaCreateManagedWidget("arg_label",
			    xmLabelWidgetClass,
			    arg_cmd_w,
			    NULL);

    source_arg = new ArgField (arg_cmd_w, "source_arg");
    MMcreateWorkArea(arg_cmd_w, "arg_cmd_area", arg_cmd_area);
    MMaddCallbacks (arg_cmd_area);
    XtManageChild (arg_cmd_w);

    XtAddCallback(source_arg->widget(), XmNactivateCallback, 
		  ActivateCB, 
		  XtPointer(arg_cmd_area[ArgItems::Lookup].widget));

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(arg_cmd_w, NULL, &size);
    unsigned char unit_type;
    XtVaGetValues(arg_cmd_w, XmNunitType, &unit_type, NULL);
    int new_height = XmConvertUnits(arg_cmd_w, XmVERTICAL, XmPIXELS, 
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

    // Setup history
    init_history_file();

    // Put saved options back again
    for (i = argc + saved_options.size() - 1; i > saved_options.size(); i--)
	argv[i] = argv[i - saved_options.size()];
    for (i = saved_options.size() - 1; i >= 0; i--)
	argv[i + 1] = saved_options[i];
    argc += saved_options.size();
    argv[argc] = 0;

    // Create GDB interface
    gdb = new_gdb(type, app_data, app_context, argc, argv);
    gdb->trace_dialog(app_data.trace_dialog);

    // Setup handlers
    gdb->addBusyHandler(ReadyForQuestion, gdb_ready_for_questionHP);
    gdb->addBusyHandler(ReadyForCmd,      gdb_ready_for_cmdHP);
    gdb->addHandler    (InputEOF,         gdb_eofHP);
    gdb->addHandler    (ErrorEOF,         gdb_eofHP);
    gdb->addHandler    (LanguageChanged,  DataDisp::language_changedHP);
    DataDisp::set_handlers();

    source_arg->addHandler (Changed, source_argHP);
    source_arg->callHandlers();

    // Set the type of the execution tty.
    static string term_env("TERM=");

    switch (gdb->type())
    {
    case GDB:
    case DBX:
	// The debugger console has few capabilities.
	// When starting the execution TTY, we set the correct type.
	term_env += "dumb";
	break;

    case XDB:
	// In XDB, we have no means to set the TTY type afterwards;
	// Set the execution TTY type right now.
	term_env += app_data.term_type;
	break;
    }
    putenv(term_env);

    // Environment.
    // Don't let TERMCAP settings override our TERM settings.
    putenv("TERMCAP=");

    // This avoids tons of problems with debuggers that pipe
    // their output through `more', `less', and likewise.
    putenv("PAGER=cat");

    // Setup insertion position
    promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, messagePosition);

    // Setup help pixmap
    helpOnVersionPixmapProc = versionlogo;

    // Setup version info
    helpOnVersionExtraText = MString(string(config_info).through(".\n"), "rm");

    string expires = ddd_expiration_date();
    if (expires != "")
    {
	string expired_msg = "This version ";
	if (ddd_expired())
	    expired_msg += "has expired since " + expires;
	else
	    expired_msg += "expires " + expires;

	helpOnVersionExtraText += MString(expired_msg + ".\n", "rm");
    }

    helpOnVersionExtraText += MString("\n" DDD_NAME " is ", "rm")
	+ MString("free software", "sl")
	+ MString(" and you are welcome to distribute copies of it\n"
"under certain conditions; select `" DDD_NAME " License' in the `Help' menu\n"
"to see the conditions. "
"There is ", "rm") + MString("absolutely no warranty", "sl")
+ MString(" for " DDD_NAME ";\n"
"see the " DDD_NAME " License for details.\n"
"\n"
"If you appreciate this software, please send a picture postcard to:\n"
"\n"
"    Technische Universit\344t Braunschweig\n"
"    Abteilung Softwaretechnologie\n"
"    B\374ltenweg 88\n"
"    D-38092 Braunschweig\n"
"    GERMANY\n"
"\n"
"Send bug reports to <" ddd_NAME "-bugs@ips.cs.tu-bs.de>;\n"
"see the " DDD_NAME " manual for details on reporting bugs.\n"
"Send comments and suggestions to <" ddd_NAME "@ips.cs.tu-bs.de>.", "rm");

    // Customize `settings' and `status displays' titles.
    MString settings_title(gdb->title() + " Settings...");
    XtVaSetValues(settings_w, XmNlabelString, settings_title.xmstring(), NULL);
#if 0
    MString infos_title(gdb->title() + " Status Displays...");
    XtVaSetValues(infos_w, XmNlabelString, infos_title.xmstring(), NULL);
#endif

    // Realize all top-level widgets
    XtRealizeWidget(command_shell);
    wm_set_icon(command_shell, iconlogo(gdb_w), iconmask(gdb_w));

    if (data_disp_shell)
    {
	XtRealizeWidget(data_disp_shell);
	wm_set_icon(data_disp_shell,
		    iconlogo(data_main_window_w),
		    iconmask(data_main_window_w));
	wm_set_group_leader(XtDisplay(data_disp_shell),
			    XtWindow(data_disp_shell),
			    XtWindow(command_shell));
    }

    if (source_view_shell)
    {
	XtRealizeWidget(source_view_shell);
	wm_set_icon(source_view_shell,
		    iconlogo(source_main_window_w),
		    iconmask(source_main_window_w));
	wm_set_group_leader(XtDisplay(source_view_shell),
			    XtWindow(source_view_shell),
			    XtWindow(command_shell));
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

    // Setup info button states
    switch (gdb->type())
    {
    case XDB:
	XtManageChild(l_w);
	register_info_button(l_w);
	break;

    case DBX:
	XtManageChild(dump_w);
	register_info_button(dump_w);
	break;

    case GDB:
	XtManageChild(info_locals_w);
	XtManageChild(info_args_w);
	register_info_button(info_locals_w);
	register_info_button(info_args_w);
    }
    update_infos();

    Boolean iconic;
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
	// TTY mode: all shells follow as needed
    }

    // If some window is iconified, iconify all others as well
    if (command_shell)
	XtAddEventHandler(command_shell, StructureNotifyMask, False,
			  StructureNotifyEH, XtPointer(0));
    if (source_view_shell)
	XtAddEventHandler(source_view_shell, StructureNotifyMask, False,
			  StructureNotifyEH, XtPointer(0));
    if (data_disp_shell)
	XtAddEventHandler(data_disp_shell, StructureNotifyMask, False,
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
	Widget tool_shell_parent = 
	    source_view_shell ? source_view_shell : command_shell;
	arg = 0;
	XtSetArg(args[arg], XmNx, 
		 WidthOfScreen(XtScreen(tool_shell_parent)) + 1);  arg++;
	XtSetArg(args[arg], XmNy,
		 HeightOfScreen(XtScreen(tool_shell_parent)) + 1); arg++;
	tool_shell = 
	    verify(XmCreateDialogShell(tool_shell_parent, 
				       "tool_shell", args, arg));
	Delay::register_shell(tool_shell);
	arg = 0;
	tool_buttons_w = 
	    verify(XmCreateForm(tool_shell, "tool_buttons", args, arg));
	add_buttons(tool_buttons_w, app_data.tool_buttons);

	// We don't manage the buttons right now, because doing so
	// causes the tool shell to pop up.  This is why
	// `tool_buttons_w' is maintained in `windows.C'.

	wm_set_icon(tool_shell,
		    iconlogo(tool_buttons_w),
		    iconmask(tool_buttons_w));
	wm_set_group_leader(XtDisplay(tool_shell),
			    XtWindow(tool_shell),
			    XtWindow(tool_shell_parent));

#if 0
	XtAddEventHandler(tool_shell, StructureNotifyMask, False,
			  StructureNotifyEH, XtPointer(0));
#endif
	
	if (app_data.tool_bar)
	{
	    // The command tool is not needed, as we have a tool bar.
	}
	else if (source_view_shell || iconic || app_data.tty_mode)
	{
	    // We don't need the command tool right now - 
	    // wait for source window to map
	}
	else
	{
	    // OK, raise the command tool
	    initial_popup_shell(tool_shell);
	}
    }

    // Setup TTY interface
    if (app_data.tty_mode)
    {
	init_command_tty();

	string init_msg = XmTextGetString(gdb_w);
	init_msg.gsub("\344", "ae");
	init_msg.gsub("\366", "oe");
	init_msg.gsub("\374", "ue");
	init_msg.gsub("\337", "ss");
	init_msg.gsub("\251", "(C)");
	tty_out(init_msg);
    }

    // Start debugger
    start_gdb();
    gdb_tty = gdb->slave_tty();

    // Main Loop
    main_loop_entered = false;
    int sig;
    if ((sig = setjmp(main_loop_env)))
    {
	main_loop_entered = false;
	ddd_show_signal(sig);
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
    // Check if GDB is still running
    gdb->running();

    // Check if the command TTY is still open
    tty_running();

    // Check if the separate TTY is still running
    exec_tty_running();

    // Check for emergencies
    check_emergencies();

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
}

static void process_pending_events()
{
    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);
    while (XtAppPending(app_context))
	process_next_event();
}


//-----------------------------------------------------------------------------
// Check this version; give warnings if needed (no license, beta expired, etc.)
//-----------------------------------------------------------------------------

static void ddd_check_version()
{
    if (ddd_expired())
    {
	ostrstream msg;
	msg << "This " DDD_NAME " version (" DDD_VERSION ") has expired since "
	    << ddd_expiration_date() << ".\n"
	    << "Please upgrade to the recent " DDD_NAME " version.";

	post_warning(string(msg), "expired_warning");
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
	while (shell && !XmIsVendorShell(shell))
	    shell = XtParent(shell);
	if (shell)
	    InstallButtonTips(shell, true);
    }
}

//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------

static Boolean ddd_setup_done(XtPointer)
{
    ddd_check_version();
    install_button_tips();
    main_loop_entered = true;

    DispBox::init_vsllib(process_pending_events);
    DataDisp::refresh_graph_edit();

    return True;		// Remove from the list of work procs
}


//-----------------------------------------------------------------------------
// Set sensitivity
//-----------------------------------------------------------------------------

inline void set_sensitive(Widget w, bool state)
{
    if (w != 0)
	XtSetSensitive(w, state);
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
// Option handling
//-----------------------------------------------------------------------------

static void set_option_widgets(DDDOption opt)
{
    separate_exec_window_w[opt] = separate_exec_window_w[0];
    find_words_only_w[opt]      = find_words_only_w[0];
    disassemble_w[opt]          = disassemble_w[0];
}

// Reflect state in option menus
void update_options()
{
    for (int i = 1; i < 4; i++)
    {
	if (separate_exec_window_w[i] == 0)
	    continue;		// Shell not realized

	XtVaSetValues(separate_exec_window_w[i],
		      XmNset, app_data.separate_exec_window, NULL);
	XtVaSetValues(disassemble_w[i],
		      XmNset, app_data.disassemble, NULL);
	XtVaSetValues(find_words_only_w[i],
		      XmNset, app_data.find_words_only, NULL);
	set_sensitive(disassemble_w[i], gdb->type() == GDB);
    }

    XtVaSetValues(button_tips_w,
		  XmNset, app_data.button_tips, NULL); 
    XtVaSetValues(value_tips_w,
		  XmNset, app_data.value_tips, NULL); 
    XtVaSetValues(button_docs_w,
		  XmNset, app_data.button_docs, NULL); 
    XtVaSetValues(value_docs_w,
		  XmNset, app_data.value_docs, NULL); 
    XtVaSetValues(set_global_completion_w,
		  XmNset, app_data.global_tab_completion, NULL);
    XtVaSetValues(set_console_completion_w,
		  XmNset, !app_data.global_tab_completion, NULL);
    XtVaSetValues(group_iconify_w,
		  XmNset, app_data.group_iconify, NULL);
    XtVaSetValues(ungrab_mouse_pointer_w,
		  XmNset, app_data.ungrab_mouse_pointer, NULL);
    XtVaSetValues(save_history_on_exit_w,
		  XmNset, app_data.save_history_on_exit, NULL);

    XtVaSetValues(cache_source_files_w,
		  XmNset, app_data.cache_source_files, NULL);
    XtVaSetValues(cache_machine_code_w,
		  XmNset, app_data.cache_machine_code, NULL);
    XtVaSetValues(set_display_glyphs_w,
		  XmNset, app_data.display_glyphs, NULL);
    XtVaSetValues(set_display_text_w,
		  XmNset, !app_data.display_glyphs, NULL);
    XtVaSetValues(set_refer_path_w,
		  XmNset, app_data.use_source_path, NULL);
    XtVaSetValues(set_refer_base_w,
		  XmNset, !app_data.use_source_path, NULL);
    XtVaSetValues(tab_width_w,
		  XmNvalue, app_data.tab_width, NULL);

    XtVaSetValues(suppress_warnings_w,
		  XmNset, app_data.suppress_warnings, NULL);

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

    XtVaSetValues(graph_detect_aliases_w,
		  XmNset, app_data.detect_aliases, NULL);
    XtVaSetValues(graph_snap_to_grid_w, XmNset, snap_to_grid, NULL);
    XtVaSetValues(graph_show_hints_w, XmNset, show_hints, NULL);
    XtVaSetValues(graph_auto_layout_w, XmNset, auto_layout, NULL);
    XtVaSetValues(graph_compact_layout_w, XmNset, 
		  layout_mode == CompactLayoutMode, NULL);
    XtVaSetValues(graph_grid_size_w, XmNvalue, show_grid ? grid_width : 0, 
		  NULL);


    unsigned char policy = '\0';
    XtVaGetValues(command_shell, 
		  XmNkeyboardFocusPolicy, &policy,
		  NULL);
    XtVaSetValues(set_focus_pointer_w,
		  XmNset, policy == XmPOINTER, NULL);
    XtVaSetValues(set_focus_explicit_w,
		  XmNset, policy == XmEXPLICIT, NULL);

    XtVaSetValues(set_scrolling_panner_w,
		  XmNset, app_data.panned_graph_editor, NULL);
    XtVaSetValues(set_scrolling_scrollbars_w,
		  XmNset, !app_data.panned_graph_editor, NULL);

    XtVaSetValues(set_status_bottom_w,
		  XmNset, app_data.status_at_bottom, NULL);
    XtVaSetValues(set_status_top_w,
		  XmNset, !app_data.status_at_bottom, NULL);

    XtVaSetValues(set_tool_buttons_in_tool_bar_w,
		  XmNset, app_data.tool_bar, NULL);
    XtVaSetValues(set_tool_buttons_in_command_tool_w,
		  XmNset, !app_data.tool_bar, NULL);

    Boolean separate = 
	app_data.separate_data_window || app_data.separate_source_window;
    XtVaSetValues(set_separate_windows_w,
		  XmNset, separate, NULL);
    XtVaSetValues(set_attached_windows_w,
		  XmNset, !separate, NULL);

    DebuggerType type = debugger_type(app_data.debugger);
    XtVaSetValues(set_debugger_gdb_w,
		  XmNset, type == GDB, NULL);
    XtVaSetValues(set_debugger_dbx_w,
		  XmNset, type == DBX, NULL);
    XtVaSetValues(set_debugger_xdb_w,
		  XmNset, type == XDB, NULL);

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

    source_view->set_display_glyphs(app_data.display_glyphs);
    source_view->set_disassemble(gdb->type() == GDB && app_data.disassemble);
    source_view->set_all_registers(app_data.all_registers);
    source_view->set_tab_width(app_data.tab_width);

    if (app_data.tool_bar && tool_bar_w != 0 && !XtIsManaged(tool_bar_w))
    {
	XtManageChild(tool_bar_w);
	gdbCloseToolWindowCB(command_shell, 0, 0);
    }
    else if (!app_data.tool_bar && tool_bar_w != 0 && XtIsManaged(tool_bar_w))
    {
	XtUnmanageChild(tool_bar_w);
	gdbOpenToolWindowCB(command_shell, 0, 0);
    }

    EnableButtonTips(app_data.button_tips);
    EnableButtonDocs(app_data.button_docs);
    EnableTextTips(app_data.value_tips);
    EnableTextDocs(app_data.value_docs);

    update_reset_preferences();
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

static void set_toggle(Widget w, Boolean set)
{
    if (XmToggleButtonGetState(w) != set)
	XmToggleButtonSetState(w, set, True);
}

void save_option_state()
{
    initial_app_data = app_data;

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
    set_toggle(button_tips_w, initial_app_data.button_tips);
    set_toggle(button_docs_w, initial_app_data.button_docs);
    set_toggle(value_tips_w, initial_app_data.value_tips);
    set_toggle(value_docs_w, initial_app_data.value_docs);
    set_toggle(set_global_completion_w, 
	       initial_app_data.global_tab_completion);
    set_toggle(set_console_completion_w, 
	       !initial_app_data.global_tab_completion);
    set_toggle(group_iconify_w, initial_app_data.group_iconify);
    set_toggle(suppress_warnings_w, initial_app_data.suppress_warnings);
    set_toggle(ungrab_mouse_pointer_w, initial_app_data.ungrab_mouse_pointer);
    set_toggle(save_history_on_exit_w, initial_app_data.save_history_on_exit);
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
	|| app_data.suppress_warnings != initial_app_data.suppress_warnings
	|| app_data.ungrab_mouse_pointer != 
	    initial_app_data.ungrab_mouse_pointer
	|| app_data.save_history_on_exit != 
	    initial_app_data.save_history_on_exit;
}

static void ResetSourcePreferencesCB(Widget, XtPointer, XtPointer)
{
    set_toggle(set_display_glyphs_w, initial_app_data.display_glyphs);
    set_toggle(set_display_glyphs_w, !initial_app_data.display_glyphs);
    set_toggle(set_refer_path_w, initial_app_data.use_source_path);
    set_toggle(set_refer_base_w, !initial_app_data.use_source_path);
    set_toggle(cache_source_files_w, initial_app_data.cache_source_files);
    set_toggle(cache_machine_code_w, initial_app_data.cache_machine_code);

    if (app_data.tab_width != initial_app_data.tab_width)
    {
	app_data.tab_width = initial_app_data.tab_width;
	update_options();
    }
}

static bool source_preferences_changed()
{
    return app_data.display_glyphs != initial_app_data.display_glyphs
	|| app_data.use_source_path != initial_app_data.use_source_path
	|| app_data.cache_source_files != initial_app_data.cache_source_files
	|| app_data.cache_machine_code != initial_app_data.cache_machine_code
	|| app_data.tab_width != initial_app_data.tab_width;
}

static void ResetDataPreferencesCB(Widget, XtPointer, XtPointer)
{
    set_toggle(graph_detect_aliases_w, initial_app_data.detect_aliases);
    set_toggle(graph_show_hints_w, initial_show_hints);
    set_toggle(graph_snap_to_grid_w, initial_snap_to_grid);
    set_toggle(graph_compact_layout_w, 
	       initial_layout_mode == CompactLayoutMode);
    set_toggle(graph_auto_layout_w, initial_auto_layout);

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

    set_toggle(set_separate_windows_w, separate);
    set_toggle(set_attached_windows_w, !separate);

    set_toggle(set_status_bottom_w, initial_app_data.status_at_bottom);
    set_toggle(set_status_top_w, !initial_app_data.status_at_bottom);

    set_toggle(set_tool_buttons_in_tool_bar_w, initial_app_data.tool_bar);
    set_toggle(set_tool_buttons_in_command_tool_w, !initial_app_data.tool_bar);

    set_toggle(set_focus_pointer_w, initial_focus_policy == XmPOINTER);
    set_toggle(set_focus_explicit_w, initial_focus_policy == XmEXPLICIT);

    set_toggle(set_scrolling_panner_w, 
	       initial_app_data.panned_graph_editor);
    set_toggle(set_scrolling_scrollbars_w, 
	       !initial_app_data.panned_graph_editor);

    DebuggerType type = debugger_type(initial_app_data.debugger);
    set_toggle(set_debugger_gdb_w, type == GDB);
    set_toggle(set_debugger_dbx_w, type == DBX);
    set_toggle(set_debugger_xdb_w, type == XDB);
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
	    != debugger_type(initial_app_data.debugger);
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
}

static void update_reset_preferences()
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
	XtManageChild(panel);
	XtAddCallback(preferences_dialog, XmNhelpCallback,
		      HelpOnThisCB, XtPointer(panel));
	XtAddCallback(reset_preferences_w, XmNactivateCallback,
		      ResetPreferencesCB, XtPointer(panel));
	current_panel = panel;

	update_reset_preferences();
    }
    else
    {
	XtUnmanageChild(panel);
	XtRemoveCallback(preferences_dialog, XmNhelpCallback,
			 HelpOnThisCB, XtPointer(panel));
	XtRemoveCallback(reset_preferences_w, XmNactivateCallback,
			 ResetPreferencesCB, XtPointer(panel));
    }
}

static void add_panel(Widget parent, Widget buttons, 
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
    XmToggleButtonCallbackStruct cbs;
    cbs.set = set;
    ChangePanelCB(button, XtPointer(form), &cbs);
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

    add_panel(change, buttons, "general", general_preferences_menu, 
	      max_width, max_height, true);
    add_panel(change, buttons, "source",  source_preferences_menu,  
	      max_width, max_height, false);
    add_panel(change, buttons, "data",    data_preferences_menu,    
	      max_width, max_height, false);
    add_panel(change, buttons, "startup", startup_preferences_menu, 
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
}

// Popup Preference Panel
static void dddPopupPreferencesCB (Widget, XtPointer, XtPointer)
{
    XtManageChild(preferences_dialog);
    raise_shell(preferences_dialog);
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

    // Give some `dummy' status message.  Some Motif versions limit
    // the size of the status window to the length of the very first
    // message, so we give some huge string at the beginning.
    MString msg("Welcome to " DDD_NAME " " DDD_VERSION "!", "rm");
    msg += MString(replicate(' ', 120), "rm");

    arg = 0;
    XtSetArg(args[arg], XmNlabelString,      msg.xmstring()); arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNresizable,        True); arg++;
    status_w = verify(XmCreateLabel(status_form, "status", args, arg));
    XtManageChild(status_w);

    arg = 0;
    XtSetArg(args[arg], XmNtopAttachment,      XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment,   XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNrightAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNresizable,          True); arg++; 
    led_w = verify(XmCreateToggleButton(status_form, "led", args, arg));
    XtManageChild(led_w);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(status_w, NULL, &size);
    unsigned char unit_type;
    XtVaGetValues(status_w, XmNunitType, &unit_type, NULL);
    int new_height = XmConvertUnits(status_w, XmVERTICAL, XmPIXELS, 
				    size.height, unit_type);
    XtVaSetValues(led_w,
		  XmNheight, new_height,
		  NULL);
    XtVaSetValues(status_form,
		  XmNpaneMaximum, new_height,
		  XmNpaneMinimum, new_height,
		  NULL);

    blink(true);
}


//-----------------------------------------------------------------------------
// Handle Status LED
//-----------------------------------------------------------------------------

static bool blinker_active      = false; // True iff status LED is active
static XtIntervalId blink_timer = 0;     // Timer used for blinking

static void BlinkCB(XtPointer client_data, XtIntervalId *)
{
    Boolean new_state = Boolean(client_data);
    XtVaSetValues(led_w, XmNfillOnSelect, new_state, NULL);
    XFlush(XtDisplay(led_w));

    if ((blinker_active || new_state) && app_data.busy_blink_rate > 0)
    {
	blink_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(led_w),
				      app_data.busy_blink_rate, BlinkCB, 
				      XtPointer(!new_state));
    }
    else
    {
	blink_timer = 0;
    }
}

static void blink(bool set)
{
    blinker_active = set;

    if (blink_timer == 0)
	BlinkCB(XtPointer(set), (XtIntervalId *)0);
}


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

void source_argHP (void *_arg_field, void *, void *)
{
    ArgField *arg_field = (ArgField *)_arg_field;
    string arg = arg_field->get_string();

    bool can_find = (arg != "");

    set_sensitive(arg_cmd_area[ArgItems::FindBackward].widget, can_find);
    set_sensitive(arg_cmd_area[ArgItems::FindForward].widget,  can_find);

    bool can_print = 
	can_find && (arg.contains("::") || !arg.contains(":"));

    set_sensitive(arg_cmd_area[ArgItems::Print].widget, can_print);
    set_sensitive(arg_cmd_area[ArgItems::Display].widget, can_print);
}

//-----------------------------------------------------------------------------
// Handlers
//-----------------------------------------------------------------------------

void gdb_ready_for_questionHP (void*, void*, void* call_data)
{
    bool gdb_ready = bool(call_data);
    if (gdb_ready)
    {
	if (!gdb_initialized)
	{
	    gdb_initialized = true;
	    XmTextSetEditable(gdb_w, true);

	    // Load the default history (for debuggers that don't
	    // provide history); the ``real'' history is read as reply
	    // to the ``show history filename'' command.
	    load_history();
	}

	// Process next pending command as soon as we return
	if (!emptyCommandQueue())
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
			    processCommandQueue, XtPointer(0));

	// If the debugged program has grabbed the pointer, ungrab it
	ungrab_mouse_pointer();

	// Completion is done
	clear_completion_delay();

	// We don't exit and we don't restart
	ddd_is_exiting = ddd_is_restarting = false;
    }

    set_sensitive(stack_w,     gdb_ready);
    set_sensitive(registers_w, gdb_ready && gdb->type() == GDB);
    set_sensitive(threads_w,   gdb_ready && gdb->type() == GDB);
    set_sensitive(infos_w,     gdb_ready && gdb->type() == GDB);
    set_sensitive(settings_w,
		  gdb_ready && (gdb->type() == GDB || gdb->type() == DBX));

    blink(!gdb_ready);
}

void gdb_ready_for_cmdHP (void *, void *, void *)
{
    // Nothing yet...
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

    default:
	{
	    // Issue control character
	    string c;
	    if (ctrl < ' ')
		c = "^" + string('@' + int(ctrl));
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

    gdb_input_at_prompt = false;

    if (promptPosition == 0)
	promptPosition = XmTextGetLastPosition(gdb_w);

    private_gdb_output = true;

    // Don't care for CR if followed by NL
    static regex RXcrlf("\r\r*\n");
    text.gsub(RXcrlf, "\n");

    // Don't care for strings to be ignored
    static string empty;
    if (gdb_out_ignore != "")
	text.gsub(gdb_out_ignore, empty);

    // Pass TEXT to various functions
    set_selection_from_gdb(text);
    set_buttons_from_gdb(console_buttons_w, text);
    set_buttons_from_gdb(source_buttons_w, text);
    set_buttons_from_gdb(data_buttons_w, text);
    set_buttons_from_gdb(tool_bar_w, text);
    set_status_from_gdb(text);
    set_tty_from_gdb(text);

    // Output TEXT on TTY
    tty_out(text);

    // Output TEXT in debugger console
    do {
	char ctrl      = '\0';
	bool have_ctrl = false;

	check_emergencies();

	string block = text;
	int i = index_control(block);
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

void gdbCutSelectionCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    DDDWindow win = DDDWindow(client_data);
    switch (win)
    {
    case ToolWindow:
	// Cannot cut from command tool
	break;

    case GDBWindow:
	XmTextCut(gdb_w, tm);
	break;

    case SourceWindow:
	XmTextFieldCut(source_arg->widget(), tm);
	break;

    case DataWindow:
	// Cannot cut from data window
	break;

    case ExecWindow:
	// Cannot cut from exec window
	break;
    }
}

void gdbCopySelectionCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);
    
    DDDWindow win = DDDWindow(client_data);
    switch (win)
    {
    case ToolWindow:
	// Cannot copy from command tool
	break;

    case GDBWindow:
	XmTextCopy(gdb_w, tm);
	break;

    case SourceWindow:
	XmTextFieldCopy(source_arg->widget(), tm);
	break;

    case DataWindow:
	XmTextFieldCopy(DataDisp::graph_arg->widget(), tm);
	break;

    case ExecWindow:
	// Cannot copy from exec window
	break;
    }
}

void gdbPasteClipboardCB(Widget, XtPointer client_data, XtPointer)
{
    DDDWindow win = DDDWindow(client_data);
    switch (win)
    {
    case ToolWindow:
	// Cannot paste into command tool
	break;

    case GDBWindow:
	XmTextPaste(gdb_w);
	break;

    case SourceWindow:
	XmTextFieldPaste(source_arg->widget());
	break;

    case DataWindow:
	// Cannot paste into data window
	break;

    case ExecWindow:
	// Cannot paste into exec window
	break;
    }
}

void gdbClearSelectionCB(Widget, XtPointer client_data, XtPointer)
{
    DDDWindow win = DDDWindow(client_data);
    switch (win)
    {
    case ToolWindow:
	// Cannot clear command tool
	break;

    case GDBWindow:
	XmTextReplace(gdb_w, promptPosition, 
		      XmTextGetLastPosition(gdb_w), "");
	break;

    case SourceWindow:
	source_arg->set_string("");
	break;

    case DataWindow:
	DataDisp::graph_arg->set_string("");
	break;

    case ExecWindow:
	// Cannot clear exec window
	break;
    }
}

void gdbDeleteSelectionCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    DDDWindow win = DDDWindow(client_data);
    switch (win)
    {
    case ToolWindow:
	// Cannot delete from command tool
	break;

    case GDBWindow:
	XmTextRemove(gdb_w);
	break;

    case SourceWindow:
	XmTextFieldRemove(source_arg->widget());
	break;

    case DataWindow:
	DataDisp::deleteCB(w, client_data, call_data);
	break;

    case ExecWindow:
	// Cannot delete from exec window
	break;
    }
}

void gdbUpdateEditCB(Widget, XtPointer, XtPointer)
{
    // Check whether we can copy something to the clipboard
    XmTextPosition start, end;

    Boolean b = XmTextGetSelectionPosition(gdb_w, &start, &end);
    set_sensitive(command_edit_menu[EditItems::Cut].widget,    b);
    set_sensitive(command_edit_menu[EditItems::Copy].widget,   b);
    set_sensitive(command_edit_menu[EditItems::Delete].widget, b);

    b = XmTextFieldGetSelectionPosition(source_arg->widget(), &start, &end);
    set_sensitive(source_edit_menu[EditItems::Cut].widget,    b);
    set_sensitive(source_edit_menu[EditItems::Copy].widget,   b);
    set_sensitive(source_edit_menu[EditItems::Delete].widget, b);

    b = XmTextFieldGetSelectionPosition(DataDisp::graph_arg->widget(), 
					&start, &end);
    set_sensitive(data_edit_menu[EditItems::Cut].widget,    false);
    set_sensitive(data_edit_menu[EditItems::Copy].widget,   b);
    set_sensitive(data_edit_menu[EditItems::Delete].widget, b);


#if 0				// This doesn't work -- AZ
    // Check whether we can get something from the clipboard
    Display *display = XtDisplay(command_shell);
    int count;
#if XmVersion >= 1002
    unsigned long max_length;
#else
    int max_length;
#endif
    int c = XmClipboardInquireCount(display, 
				    DefaultRootWindow(display),
				    &count, &max_length);
    b = (c == ClipboardSuccess);
#else
    b = true;
#endif

    set_sensitive(command_edit_menu[EditItems::Paste].widget, b);
    set_sensitive(source_edit_menu[EditItems::Paste].widget,  b);
    set_sensitive(data_edit_menu[EditItems::Paste].widget,    false);
}

void gdbUpdateViewCB(Widget, XtPointer, XtPointer)
{
    // Check whether the execution tty is running
    exec_tty_running();
    Boolean b = (exec_tty_pid() > 0);
    set_sensitive(command_view_menu[ExecWindow].widget, b);
    set_sensitive(source_view_menu[ExecWindow].widget,  b);
    set_sensitive(data_view_menu[ExecWindow].widget,    b);

    // Check whether we have a command tool
    b = (tool_shell != 0);
    set_sensitive(command_view_menu[ToolWindow].widget, b);
    set_sensitive(source_view_menu[ToolWindow].widget,  b);
    set_sensitive(data_view_menu[ToolWindow].widget,    b);
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
	    cerr << "(You can suppress these warnings "
		 << "by setting the 'Suppress X Warnings' option\n"
		 << "in the DDD `General Preferences` panel.)\n";
	    informed = true;
	}
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

void check_emergencies()
{
    XEvent event;
    if (XCheckIfEvent(XtDisplay(gdb_w), &event, is_emergency, 0))
    {
	// Emergency: process this event immediately
	XtDispatchEvent(&event);
    }
}
