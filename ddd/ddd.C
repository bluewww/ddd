// $Id$
// DDD main program (and much more)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.


// This file does most of the application work in DDD.  That is:
// - DDD main function
// - Creation of Source and Data Windows
// - Menu bars
// - Handling of separate execution window
// - Command window
// - Command completion
// and more...
//
// (Some day, this file shall be split into several modules. - AZ)


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
#pragma implementation "Queue.h"
#endif

#include "config.h"
// #include "MemCheckD.h"

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

// Motif stuff
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/MainW.h>
#include <Xm/PanedW.h>
#include <Xm/Label.h>
#include <Xm/List.h>      // XmListSelectPos()
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h> // XmCreateWorkArea()
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/CutPaste.h>
#include <Xm/Protocols.h>
#include <Xm/SelectioB.h>
#include <Xm/FileSB.h>
#include <Xm/Frame.h>
#include <X11/Shell.h>
#include <X11/Xlib.h>	  // Event names
#include <X11/Xutil.h>	  // Window manager functions

#ifndef LESSTIF_VERSION
extern "C" {
#define new new_w
#define class class_w
#include <Xm/SashP.h>	  // XmIsSash()
#undef class
#undef new
}
#else
#define XmIsSash(x) false
#endif

#ifdef HAVE_X11_XMU_EDITRES_H
#include <X11/Xmu/Editres.h>
#endif

// Nora stuff
#include "assert.h"
#include "cook.h"
#include "strclass.h"
#include "bool.h"
#include "converters.h"
#include "MakeMenu.h"
#include "MString.h"
#include "HelpCB.h"
#include "ExitCB.h"
#include "DestroyCB.h"
#include "events.h"
#include "Queue.h"
#include "StringA.h"
#include "findParent.h"
#include "longName.h"
#include "hostname.h"
#include "fileSBF.h"
#include "GraphEdit.h"
#include "sigName.h"


// DDD stuff
#include "ddd.h"
#include "comm-manag.h"
#include "disp-read.h"
#include "string-fun.h"
#include "GDBAgent.h"
#include "DataDisp.h"
#include "ArgField.h"
#include "filetype.h"
#include "version.h"
#include "configinfo.h"
#include "xconfig.h"
#include "host.h"
#include "strings.h"
#include "mydialogs.h"
#include "verify.h"


// Standard stuff
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <setjmp.h>

// GNU stuff
extern "C" {
#include <termcap/termcap.h>
}
#include "glob.h"

// Icon stuff
#include "dddlogo.xbm"
#include "dddmask.xbm"

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

//-----------------------------------------------------------------------------
// Forward function decls
//-----------------------------------------------------------------------------
// Callbacks
void gdbModifyCB       (Widget, XtPointer, XtPointer);
void gdbMotionCB       (Widget, XtPointer, XtPointer);
void gdbChangeCB       (Widget, XtPointer, XtPointer);
void gdbBreakArgCmdCB  (Widget, XtPointer, XtPointer);
void gdbClearArgCmdCB  (Widget, XtPointer, XtPointer);
void gdbLineArgCmdCB   (Widget, XtPointer, XtPointer);
void gdbPrintArgCmdCB  (Widget, XtPointer, XtPointer);
void gdbDisplayArgCmdCB(Widget, XtPointer, XtPointer);
void gdbLookupCB       (Widget, XtPointer, XtPointer);
void gdbFindForwardCB  (Widget, XtPointer, XtPointer);
void gdbFindBackwardCB (Widget, XtPointer, XtPointer);

void gdbEditSourceCB   (Widget, XtPointer, XtPointer);
void gdbReloadSourceCB (Widget, XtPointer, XtPointer);

void gdbGoBackCB       (Widget, XtPointer, XtPointer);
void gdbGoForwardCB    (Widget, XtPointer, XtPointer);

void gdbHistoryCB  (Widget, XtPointer, XtPointer);
void gdbNextCB     (Widget, XtPointer, XtPointer);
void gdbPrevCB     (Widget, XtPointer, XtPointer);
void gdbClearCB    (Widget, XtPointer, XtPointer);
void gdbCompleteCB (Widget, XtPointer, XtPointer);
void gdbApplyCB    (Widget, XtPointer, XtPointer);

void gdbCutSelectionCB   (Widget, XtPointer, XtPointer);
void gdbCopySelectionCB  (Widget, XtPointer, XtPointer);
void gdbPasteClipboardCB (Widget, XtPointer, XtPointer);
void gdbClearSelectionCB (Widget, XtPointer, XtPointer);
void gdbDeleteSelectionCB(Widget, XtPointer, XtPointer);
void gdbUpdateEditCB     (Widget, XtPointer, XtPointer);
void gdbUpdateViewCB     (Widget, XtPointer, XtPointer);

void gdbOpenFileCB           (Widget, XtPointer, XtPointer);
void gdbOpenCoreCB           (Widget, XtPointer, XtPointer);
void gdbOpenSourceCB         (Widget, XtPointer, XtPointer);

void gdbOpenCommandWindowCB  (Widget, XtPointer, XtPointer);
void gdbOpenSourceWindowCB   (Widget, XtPointer, XtPointer);
void gdbOpenDataWindowCB     (Widget, XtPointer, XtPointer);
void gdbOpenExecWindowCB     (Widget, XtPointer, XtPointer);

void gdbCloseCommandWindowCB (Widget, XtPointer, XtPointer);
void gdbCloseSourceWindowCB  (Widget, XtPointer, XtPointer);
void gdbCloseDataWindowCB    (Widget, XtPointer, XtPointer);
void gdbCloseExecWindowCB    (Widget, XtPointer, XtPointer);

void gdbRunCB                (Widget, XtPointer, XtPointer);

void DDDExitCB               (Widget, XtPointer, XtPointer);
void DDDCloseCB              (Widget, XtPointer, XtPointer);
void DDDSaveOptionsCB        (Widget, XtPointer, XtPointer);

void DDDWWWPageCB            (Widget, XtPointer, XtPointer);

void graphQuickPrintCB       (Widget, XtPointer, XtPointer);
void graphPrintCB            (Widget, XtPointer, XtPointer);

void graphToggleShowGridCB      (Widget, XtPointer, XtPointer);
void graphToggleShowHintsCB     (Widget, XtPointer, XtPointer);
void graphToggleSnapToGridCB    (Widget, XtPointer, XtPointer);
void graphToggleCompactLayoutCB (Widget, XtPointer, XtPointer);
void graphToggleAutoLayoutCB    (Widget, XtPointer, XtPointer);

void graphAlignCB            (Widget, XtPointer, XtPointer);
void graphRotateCB           (Widget, XtPointer, XtPointer);
void graphLayoutCB           (Widget, XtPointer, XtPointer);

void sourceToggleFindWordsOnlyCB    (Widget, XtPointer, XtPointer);
void sourceToggleCacheSourceFilesCB (Widget, XtPointer, XtPointer);
void sourceToggleCacheMachineCodeCB (Widget, XtPointer, XtPointer);
void sourceToggleDisplayGlyphsCB    (Widget, XtPointer, XtPointer);
void sourceToggleDisassembleCB      (Widget, XtPointer, XtPointer);

void dddToggleGroupIconifyCB       (Widget, XtPointer, XtPointer);
void dddToggleGlobalTabCompletionCB(Widget, XtPointer, XtPointer);
void dddToggleSeparateExecWindowCB (Widget, XtPointer, XtPointer);
void dddToggleSaveOptionsOnExitCB  (Widget, XtPointer, XtPointer);
void dddToggleSaveHistoryOnExitCB  (Widget, XtPointer, XtPointer);
void dddToggleSuppressWarningsCB   (Widget, XtPointer, XtPointer);
void dddSetSeparateWindowsCB       (Widget, XtPointer, XtPointer);
void dddSetKeyboardFocusPolicyCB   (Widget, XtPointer, XtPointer);
void dddSetPannerCB                (Widget, XtPointer, XtPointer);
void dddSetDebuggerCB              (Widget, XtPointer, XtPointer);

void StructureNotifyEH(Widget, XtPointer, XEvent *, Boolean *);


// Handling of help texts
MString gdbDefaultHelp(Widget widget);

// Buttons and state
void set_buttons_from_gdb(string& text);
void set_buttons_from_gdb(Widget w, string& text);
void set_status_from_gdb(const string& text);
void set_selection_from_gdb(string& text);
void set_status(const string& text);
void update_options();

// Callbacks
void gdb_ready_for_questionHP (void *, void *, void *);
void gdb_ready_for_cmdHP      (void *, void *, void *);
void gdb_eofHP                (Agent *, void *, void *);


// Actions
void controlAct           (Widget, XEvent*, String*, Cardinal*);
void prev_historyAct      (Widget, XEvent*, String*, Cardinal*);
void next_historyAct      (Widget, XEvent*, String*, Cardinal*);
void forward_characterAct (Widget, XEvent*, String*, Cardinal*);
void backward_characterAct(Widget, XEvent*, String*, Cardinal*);
void beginning_of_lineAct (Widget, XEvent*, String*, Cardinal*);
void end_of_lineAct       (Widget, XEvent*, String*, Cardinal*);
void set_lineAct          (Widget, XEvent*, String*, Cardinal*);
void complete_commandAct  (Widget, XEvent*, String*, Cardinal*);
void complete_argAct      (Widget, XEvent*, String*, Cardinal*);
void complete_tabAct      (Widget, XEvent*, String*, Cardinal*);
void delete_or_controlAct (Widget, XEvent*, String*, Cardinal*);
void insert_source_argAct (Widget, XEvent*, String*, Cardinal*);
void insert_graph_argAct  (Widget, XEvent*, String*, Cardinal*);
void next_tab_groupAct    (Widget, XEvent*, String*, Cardinal*);
void prev_tab_groupAct    (Widget, XEvent*, String*, Cardinal*);
void get_focusAct         (Widget, XEvent*, String*, Cardinal*);

// Setup
static Boolean ddd_setup_done(XtPointer client_data);

// Cleanup
static void save_options(Widget origin);
static void ddd_cleanup();
static void ddd_signal(int sig);
static void ddd_fatal(int sig);
static void ddd_install_fatal();
static void ddd_install_signal();

// Warning proc
static void ddd_xt_warning(String message);

// Button creator
Widget make_buttons(Widget parent, const string& name, const string& list);

// Sash killer
void untraverse_sashes(Widget paned);
void unmanage_sashes(Widget paned, int ignore = -1);

// Helper for empty source arg field
void source_argHP (void*, void*, void* call_data);

// Execution TTY
void startup_exec_tty();
void startup_exec_tty(string& command, Widget origin = 0);
void kill_exec_tty();
void remove_init_file();

// Controlling TTY
void tty_command(Agent *source, void *client_data, void *call_data);
void tty_eof(Agent *source, void *client_data, void *call_data);

// History
void add_to_history(const string& line);

// Arguments
void add_to_arguments(string line);
void update_arguments();

// Help texts
void show_invocation(DebuggerType type);
void show_version();
void show_configuration();
void show_resources(XrmDatabase database);
void show_manual();

// DDD logos
Pixmap iconlogo(Widget shell);
Pixmap iconmask(Widget shell);
Pixmap versionlogo(Widget shell);

// Window manager
void wm_set_icon(Widget shell, Pixmap icon, Pixmap mask);
void wm_set_icon(Display *display, Window shell, Pixmap icon, Pixmap mask);

void wm_set_name(Widget shell, string title = "", string icon = "");
void wm_set_name(Display *display, Window shell,
		 string title = "", string icon = "");

void wm_set_group_leader(Display *display, Window shell, Window leader_shell);
static void wait_until_mapped(Widget w);

// Options
string options_file();


// Obscure features
void handle_obscure_commands(string& command, Widget origin);


//-----------------------------------------------------------------------------
// Xt Stuff
//-----------------------------------------------------------------------------

// Application resource definitions
static XtResource resources[] = {
    { 
	XtNdebugger,
	XtCDebugger,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger), 
	XtRString, 
	XtPointer("")
    },
    { 
	XtNdebuggerCommand,
	XtCDebuggerCommand,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger_command), 
	XtRString, 
	XtPointer("")
    },
    { 
	XtNtraceDialog, 
	XtCTrace, 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, trace_dialog),
	XmRImmediate, 
	XtPointer(False)
    },
    { 
	XtNtraceShellCommands,
	XtCTrace,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, trace_shell_commands),
	XmRImmediate, 
	XtPointer(False)
    },
    { 
	XtNgdbInitCommands, 
	XtCGDBInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_initial_cmds), 
	XtRString,
	XtPointer(
	    "set height 0\n"
	    "set width 0\n"
	    "set print pretty\n"
	    "set print array\n"
	    "set print repeats 0\n" 
	    "set verbose off\n"
	    "set prompt (gdb) \n")
    },
    { 
	XtNdbxInitCommands, 
	XtCDBXInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_initial_cmds), 
	XtRString,
	XtPointer("")
    },
    { 
	XtNvslPath, 
	XtCVSLPath, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vslPath), 
	XtRString, 
	XtPointer(".")
    },
    { 
	XtNvslLibrary, 
	XtCVSLLibrary, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vslLibrary), 
	XtRString, 
	XtPointer("builtin")
    },
    {
	XtNmaxNameLength,
	XtCMaxNameLength,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_name_length),
	XmRImmediate,
	XtPointer(32)
    },
    {
	XtNindentAmount,
	XtCIndentAmount,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, indent_amount),
	XmRImmediate,
	XtPointer(8)
    },
    {
	XtNcommandButtons,
	XtCCommandButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, command_buttons),
	XtRString,
	XtPointer("Yes:No:Interrupt^C")
    },
    {
	XtNsourceButtons,
	XtCSourceButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, source_buttons),
	XtRString,
	XtPointer("run:next:step:continue:finish:up:down")
    },
    {
	XtNseparateDataWindow,
	XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_data_window),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNseparateSourceWindow,
	XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_source_window),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNseparateExecWindow,
	XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_exec_window),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNtermCommand,
	XtCTermCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_command),
	XtRString,
	XtPointer("xterm -e /bin/sh -c")
    },
    {
	XtNtermType,
	XtCTermType,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_type),
	XtRString,
	XtPointer("xterm")
   },
   {
	XtNuseTTYCommand,
	XtCUseTTYCommand,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_tty_command),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNquestionTimeout,
	XtCQuestionTimeout,
	XtRInt,
	sizeof(int),
	XtOffsetOf(AppData, question_timeout),
	XtRImmediate,
	XtPointer(5)
    },
    {
	XtNsynchronousDebugger,
	XtCSynchronousDebugger,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, synchronous_gdb),
	XtRImmediate,
	XtPointer(False)
    },
    {
        XtNdebuggerHost,
	XtCDebuggerHost,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host),
	XtRString,
	XtPointer("")
    },
    {
        XtNdebuggerHostLogin,
	XtCDebuggerHostLogin,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host_login),
	XtRString,
	XtPointer("")
    },
    {
        XtNrshCommand,
	XtCRshCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rsh_command),
	XtRString,
	XtPointer("rsh")
    },
    {
        XtNlistExecCommand,
	XtCListExecCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_exec_command),
	XtRString,
	XtPointer("file @MASK@ | grep  '.*:.*exec.*' | cut -d: -f1")
    },
    {
        XtNlistSourceCommand,
	XtCListSourceCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_source_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*text.*' | cut -d: -f1")
    },
    {
        XtNlistCoreCommand,
	XtCListCoreCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_core_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*core.*' | cut -d: -f1")
    },
    {
        XtNlistDirCommand,
	XtCListDirCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_dir_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*directory.*' | cut -d: -f1")
    },
    {
        XtNwwwPage,
	XtCWWWPage,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_page),
	XtRString,
	XtPointer("http://www.cs.tu-bs.de/softech/")
    },
    {
        XtNwwwCommand,
	XtCWWWCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_command),
	XtRString,
	XtPointer("netscape @URL@")
    },
    {
        XtNshowInvocation,
	XtCShowInvocation,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_invocation),
	XtRImmediate,
	XtPointer(False)
    },
    {
        XtNshowVersion,
	XtCShowVersion,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_version),
	XtRImmediate,
	XtPointer(False)
    },
    {
        XtNshowConfiguration,
	XtCShowConfiguration,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_configuration),
	XtRImmediate,
	XtPointer(False)
    },
    {
        XtNshowResources,
	XtCShowResources,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_resources),
	XtRImmediate,
	XtPointer(False)
    },
    {
        XtNshowManual,
	XtCShowManual,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_manual),
	XtRImmediate,
	XtPointer(False)
    },
    {
        XtNcheckConfiguration,
	XtCCheckConfiguration,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_configuration),
	XtRImmediate,
	XtPointer(False)
    },
    {
        XtNprintCommand,
	XtCPrintCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, print_command),
	XtRString,
	XtPointer("lp")
    },
    {
        XtNeditCommand,
	XtCEditCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, edit_command),
	XtRString,
	XtPointer("xterm -e vi +@LINE@ @FILE@")
    },
    {
        XtNpannedGraphEditor,
	XtCPannedGraphEditor,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, panned_graph_editor),
	XtRImmediate,
	XtPointer(False)
    },
    {
        XtNfindWordsOnly,
	XtCFindWordsOnly,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_words_only),
	XtRImmediate,
	XtPointer(True)
    },
    {
	XtNgroupIconify,
	XtCGroupIconify,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, group_iconify),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNglobalTabCompletion,
	XtCGlobalTabCompletion,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, global_tab_completion),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNsaveOptionsOnExit,
	XtCSaveOptionsOnExit,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, save_options_on_exit),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNsaveHistoryOnExit,
	XtCSaveHistoryOnExit,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, save_history_on_exit),
	XtRImmediate,
	XtPointer(True)
    },
    {
	XtNcacheSourceFiles,
	XtCCacheSourceFiles,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_source_files),
	XtRImmediate,
	XtPointer(True)
    },
    {
	XtNcacheMachineCode,
	XtCCacheMachineCode,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_machine_code),
	XtRImmediate,
	XtPointer(True)
    },
    {
	XtNsuppressWarnings,
	XtCSuppressWarnings,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, suppress_warnings),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNttyMode,
	XtCTTYMode,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, tty_mode),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNfullNameMode,
	XtCTTYMode,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, full_name_mode),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNdisplayGlyphs,
	XtCDisplayGlyphs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_glyphs),
	XtRImmediate,
	XtPointer(True)
    },
    {
	XtNdisassemble,
	XtCDisassemble,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, disassemble),
	XtRImmediate,
	XtPointer(False)
    },
    {
	XtNdddinitVersion,
	XtCVersion,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dddinit_version),
	XtRString,
	XtPointer(0)
    },
    {
	XtNappDefaultsVersion,
	XtCVersion,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, app_defaults_version),
	XtRString,
	XtPointer(0)
    },
};


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

{ "--namelength",           XtNmaxNameLength,        XrmoptionSepArg, NULL },
{ "-namelength",            XtNmaxNameLength,        XrmoptionSepArg, NULL },

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

{ "--panned-graph-editor",  XtNpannedGraphEditor,    XrmoptionNoArg, S_true },
{ "-panned-graph-editor",   XtNpannedGraphEditor,    XrmoptionNoArg, S_true },

{ "--scrolled-graph-editor", XtNpannedGraphEditor,   XrmoptionNoArg, S_false },
{ "-scrolled-graph-editor", XtNpannedGraphEditor,    XrmoptionNoArg, S_false },

{ "--synchronous-debugger", XtNsynchronousDebugger,  XrmoptionNoArg, S_true },
{ "-synchronous-debugger",  XtNsynchronousDebugger,  XrmoptionNoArg, S_true },

{ "--disassemble",          XtNdisassemble,          XrmoptionNoArg, S_true },
{ "-disassemble",           XtNdisassemble,          XrmoptionNoArg, S_true },

{ "--no-disassemble",       XtNdisassemble,          XrmoptionNoArg, S_false },
{ "-no-disassemble",        XtNdisassemble,          XrmoptionNoArg, S_false },

{ "--no-glyphs",            XtNdisplayGlyphs,        XrmoptionNoArg, S_false },
{ "-no-glyphs",             XtNdisplayGlyphs,        XrmoptionNoArg, S_false },

{ "--glyphs",               XtNdisplayGlyphs,        XrmoptionNoArg, S_true },
{ "-glyphs",                XtNdisplayGlyphs,        XrmoptionNoArg, S_true },

{ "--host",                 XtNdebuggerHost,         XrmoptionSepArg, NULL },
{ "-host",                  XtNdebuggerHost,         XrmoptionSepArg, NULL },

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
    {"ddd-next-tab-group",     next_tab_groupAct},
    {"ddd-prev-tab-group",     prev_tab_groupAct},
    {"ddd-previous-tab-group", prev_tab_groupAct},
    {"ddd-get-focus",          get_focusAct},
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
    { "quit",        MMPush, { DDDExitCB }},
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
    { "interrupt",   MMPush, { gdbCommandCB, "\003" }},
    MMEnd
};

enum DDDWindow { GDBWindow, SourceWindow, DataWindow, ExecWindow };

static MMDesc command_view_menu[] =
{
    { "command",    MMPush, { gdbOpenCommandWindowCB }},
    { "source",     MMPush, { gdbOpenSourceWindowCB }},
    { "data",       MMPush, { gdbOpenDataWindowCB }},
    { "exec",       MMPush, { gdbOpenExecWindowCB }},
    MMEnd
};

static MMDesc source_view_menu[] =
{
    { "command",    MMPush, { gdbOpenCommandWindowCB }},
    { "source",     MMPush, { gdbOpenSourceWindowCB }},
    { "data",       MMPush, { gdbOpenDataWindowCB }},
    { "exec",       MMPush, { gdbOpenExecWindowCB }},
    MMEnd
};

static MMDesc data_view_menu[] =
{
    { "command",    MMPush, { gdbOpenCommandWindowCB }},
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
    { "complete", MMPush, { gdbCompleteCB }},
    { "apply",    MMPush, { gdbApplyCB }},
    MMEnd
};

static Widget stack_w;
static Widget register_w;

static MMDesc stack_menu[] =
{
    { "stack",       MMPush, { SourceView::ViewStackFramesCB }, 
      NULL, &stack_w },
    { "registers",   MMPush,  { SourceView::ViewRegistersCB },
      NULL, &register_w },
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


// Option widgets


// All these widgets come in four times:
// w[0] holds the last created widget.
// w[<DDDOption>] holds the widget for the menu in window <DDDOption>.

enum DDDOption { DummyOptions   = 0, 
		 CommandOptions = 1, 
		 SourceOptions  = 2,
		 DataOptions    = 3 };

static Widget group_iconify_w[4];
static Widget global_tab_completion_w[4];
static Widget separate_exec_window_w[4];
static Widget save_options_on_exit_w[4];
static Widget save_history_on_exit_w[4];
static Widget graph_show_grid_w[4];
static Widget graph_show_hints_w[4];
static Widget graph_snap_to_grid_w[4];
static Widget graph_compact_layout_w[4];
static Widget graph_auto_layout_w[4];
static Widget find_words_only_w[4];
static Widget cache_source_files_w[4];
static Widget cache_machine_code_w[4];
static Widget display_glyphs_w[4];
static Widget disassemble_w[4];
static Widget suppress_warnings_w[4];
static Widget set_focus_pointer_w[4];
static Widget set_focus_explicit_w[4];
static Widget set_scrolling_panner_w[4];
static Widget set_scrolling_scrollbars_w[4];
static Widget set_debugger_gdb_w[4];
static Widget set_debugger_dbx_w[4];
static Widget set_separate_windows_w[4];
static Widget set_attached_windows_w[4];

static MMDesc ddd_options_menu[] = 
{
    { "groupIconify",        MMToggle, { dddToggleGroupIconifyCB }, 
      NULL, group_iconify_w },
    { "globalTabCompletion", MMToggle, { dddToggleGlobalTabCompletionCB }, 
      NULL, global_tab_completion_w },
    { "separateExecWindow",  MMToggle, { dddToggleSeparateExecWindowCB }, 
      NULL, separate_exec_window_w },
    { "suppressWarnings",    MMToggle, { dddToggleSuppressWarningsCB }, 
      NULL, suppress_warnings_w },
    { "saveOptionsOnExit",   MMToggle, { dddToggleSaveOptionsOnExitCB }, 
      NULL, save_options_on_exit_w },
    { "saveHistoryOnExit",   MMToggle, { dddToggleSaveHistoryOnExitCB }, 
      NULL, save_history_on_exit_w },
    MMEnd
};

static MMDesc source_options_menu[] = 
{
    { "findWordsOnly",    MMToggle, { sourceToggleFindWordsOnlyCB }, 
      NULL, find_words_only_w },
    { "disassemble",     MMToggle,  { sourceToggleDisassembleCB },
      NULL, disassemble_w },
    { "displayGlyphs",    MMToggle, { sourceToggleDisplayGlyphsCB }, 
      NULL, display_glyphs_w },
    { "cacheSourceFiles", MMToggle, { sourceToggleCacheSourceFilesCB }, 
      NULL, cache_source_files_w },
    { "cacheMachineCode", MMToggle, { sourceToggleCacheMachineCodeCB }, 
      NULL, cache_machine_code_w },
    MMEnd
};

static MMDesc data_options_menu[] = 
{
    { "showGrid",   MMToggle,  { graphToggleShowGridCB }, 
      NULL, graph_show_grid_w },
    { "showHints",  MMToggle,  { graphToggleShowHintsCB },
      NULL, graph_show_hints_w },
    { "snapToGrid", MMToggle,  { graphToggleSnapToGridCB },
      NULL, graph_snap_to_grid_w },
    { "compactLayout", MMToggle, { graphToggleCompactLayoutCB },
      NULL, graph_compact_layout_w },
    { "autoLayout", MMToggle,  { graphToggleAutoLayoutCB },
      NULL, graph_auto_layout_w },
    MMEnd
};

static MMDesc window_mode_menu [] = 
{
    { "separate",  MMToggle, { dddSetSeparateWindowsCB, XtPointer(True) },
      NULL, set_separate_windows_w },
    { "attached", MMToggle, { dddSetSeparateWindowsCB, XtPointer(False) },
      NULL, set_attached_windows_w },
    MMEnd
};

static MMDesc keyboard_focus_menu [] = 
{
    { "pointer",  MMToggle, { dddSetKeyboardFocusPolicyCB, 
			    XtPointer(XmPOINTER) },
      NULL, set_focus_pointer_w },
    { "explicit", MMToggle, { dddSetKeyboardFocusPolicyCB, 
			    XtPointer(XmEXPLICIT) },
      NULL, set_focus_explicit_w },
    MMEnd
};

static MMDesc data_scrolling_menu [] = 
{
    { "panner", MMToggle,     { dddSetPannerCB, XtPointer(True) },
      NULL, set_scrolling_panner_w },
    { "scrollbars", MMToggle, { dddSetPannerCB, XtPointer(False) },
      NULL, set_scrolling_scrollbars_w },
    MMEnd
};

static MMDesc debugger_menu [] = 
{
    { "gdb", MMToggle, { dddSetDebuggerCB, XtPointer(GDB) },
      NULL, set_debugger_gdb_w },
    { "dbx", MMToggle, { dddSetDebuggerCB, XtPointer(DBX) },
      NULL, set_debugger_dbx_w },
    MMEnd
};

static MMDesc startup_options_menu [] =
{
    { "windows",         MMRadioMenu, MMNoCB, window_mode_menu },
    { "keyboardFocus",   MMRadioMenu, MMNoCB, keyboard_focus_menu },
    { "dataScrolling",   MMRadioMenu, MMNoCB, data_scrolling_menu },
    { "debugger",        MMRadioMenu, MMNoCB, debugger_menu },
    MMEnd
};

static MMDesc options_menu [] =
{
    { "generalOptions", MMMenu, MMNoCB, ddd_options_menu     },
    { "sourceOptions",  MMMenu, MMNoCB, source_options_menu  },
    { "dataOptions",    MMMenu, MMNoCB, data_options_menu    },
    { "startupOptions", MMMenu, MMNoCB, startup_options_menu },
    MMSep,
    { "saveOptions",    MMPush,   { DDDSaveOptionsCB }},
    MMEnd
};

static MMDesc data_menu[] = 
{
    { "displays",   MMPush,    { DataDisp::EditDisplaysCB }},
    MMSep,
    { "align",      MMPush,    { graphAlignCB  }},
    { "rotate",     MMPush,    { graphRotateCB }},
    { "layout",     MMPush,    { graphLayoutCB }},
    MMSep,
    { "selectAll",  MMPush,    { DataDisp::selectAllCB }},
    { "refresh",    MMPush,    { DataDisp::refreshCB }},
    MMEnd
};

static MMDesc help_menu[] = 
{
    {"onContext",   MMPush, { HelpOnContextCB }},
    {"onWindow",    MMPush, { HelpOnWindowCB }},
    {"onHelp",      MMPush, { HelpOnHelpCB }},
    {"onVersion",   MMPush, { HelpOnVersionCB }},
    MMSep,
    {"www",         MMPush, { DDDWWWPageCB }},
    MMSep,
    {"index",       MMPush, { ManualStringHelpCB, XtPointer(ddd_man_page) }},
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
    {"break",         MMPush,  { gdbBreakArgCmdCB   }},
    {"clear",         MMPush,  { gdbClearArgCmdCB   }},
    {"print",         MMPush,  { gdbPrintArgCmdCB   }},
    {"display",       MMPush,  { gdbDisplayArgCmdCB }},
    {"find_backward", MMPush,  { gdbFindBackwardCB  }},
    {"find_forward",  MMPush,  { gdbFindForwardCB   }},
    MMEnd
};



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// All communication with GDB passes through this variable
GDBAgent*     gdb = 0;

// All communication with the command TTY passes through this variable
LiterateAgent* command_tty = 0;

// Application resources
AppData       app_data;

// Data display
DataDisp*     data_disp;

// Source display
SourceView*   source_view;

// Argument field
ArgField*     source_arg;

// Argument command list
Widget arg_cmd_w;

// GDB input/output widget
Widget gdb_w;

// GDB status indicator (only used if separate source window is used)
Widget status_w;

// GDB yes/no widget
static Widget yn_dialog;

// GDB run dialog
static Widget run_dialog;

// GDB selection dialog
Widget gdb_selection_dialog;
Widget gdb_selection_list_w;

// Last output position
XmTextPosition promptPosition;

// Last message position
XmTextPosition messagePosition;

// Application context
XtAppContext app_context;

// Completion delay flag
Delay *completion_delay = 0;

// Buttons
Widget command_buttons_w;
Widget source_buttons_w;

// Shells (only used if separate windows are used)
Widget command_shell;
Widget data_disp_shell;
Widget source_view_shell;

// Flags: shell state
enum WindowState { PoppedUp, PoppedDown, Iconic, Transient };
WindowState command_shell_state     = PoppedDown;
WindowState data_disp_shell_state   = PoppedDown;
WindowState source_view_shell_state = PoppedDown;

// Separate TTY for command execution
// TTY name
string separate_tty_name = "/dev/tty";

// TTY pid (0: not initialized, -1: failed)
pid_t separate_tty_pid   = 0;

// TTY terminal type
string separate_tty_term  = "dumb";

// TTY pipe agent
LiterateAgent *separate_tty_agent = 0;

// TTY window
Window separate_tty_window = 0;

// Current GDB TTY
string gdb_tty = "";

// True if the next line is to be displayed in the status line
bool show_next_line_in_status = false;

// True if the next 'Starting' line is to be displayed in the separate tty
bool show_starting_line_in_tty    = false;

// Strings to be ignored in GDB output
string gdb_out_ignore = "";

// GDB command redirection
string gdb_redirection = "";

// Command management
static bool private_gdb_output;   // true if output is running
static bool private_gdb_input;    // true if input is running
static bool tty_gdb_input;        // true if input comes from command tty
static bool private_gdb_history;  // true if history command was issued
static bool gdb_keyboard_command; // true if last cmd came from GDB window
static Widget gdb_last_origin;    // origin of last command
static bool gdb_question_running; // Is gdb_question running?

// History viewer
static Widget gdb_history_w  = 0;
static Widget gdb_commands_w = 0;

// Host management
string gdb_host = "";		  // non-empty if separate host

// true, if initial gdb prompt appeared
static bool gdb_initialized;

// GDB initialization file (may be remote)
static string gdb_init_file;

// Our invocation name
static char *ddd_invoke_name = ddd_NAME;

// True if options were changed
static bool options_changed = false;
static bool startup_options_changed = false;

// True if last input was at gdb prompt
static bool gdb_input_at_prompt = true;

// History storage
static StringArray gdb_history;

// Index to current history entry
static int gdb_current_history;

// File name to save the history
static string gdb_history_file;

// Size of saved history
static int    gdb_history_size = 100;

// True if the history was just loaded
static bool   gdb_new_history = true;

// Argument storage
static StringArray gdb_arguments;
static Widget gdb_arguments_w;

// True if DDD is about to exit
static bool ddd_is_exiting = false;

// The Xt Warning handler
static XtErrorHandler ddd_original_xt_warning_handler;

// Resume after fatal errors
static bool main_loop_entered = false;
static jmp_buf main_loop_env;

//-----------------------------------------------------------------------------
// Remote commands
//-----------------------------------------------------------------------------

bool remote_gdb()
{
    return gdb_host != "";
}

string sh_quote(string s)
{
    s.gsub('\'', "'\\''");
    return string('\'') + s + '\'';
}

static string _sh_command(string command, bool force_local)
{
    if (force_local || !remote_gdb())
	return "/bin/sh -c " + sh_quote(command);

    string rsh = app_data.rsh_command;
    string login = app_data.debugger_host_login;
    if (login != "")
	rsh += " -l " + login;

    string display = getenv("DISPLAY");
    if (display.contains("unix:", 0) || display.contains(":", 0))
	display = string(fullhostname()) + display.from(":");

    string settings = "DISPLAY='" + display + "'; export DISPLAY; ";
    command = settings + command;

    rsh += " " + gdb_host + " /bin/sh -c " + sh_quote(sh_quote(command));

    return rsh;
}

string sh_command(string command, bool force_local)
{
    string ret = _sh_command(command, force_local);
    if (app_data.trace_shell_commands)
	clog << "+ " << ret << "\n";
    clog.flush();
    return ret;
}


//-----------------------------------------------------------------------------
// DDD main program
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    // As this is a C++ program, execution does not begin here.  At
    // this point, all global data objects already have been properly
    // initialized.

    // This one is required for error messages
    ddd_invoke_name = argc ? argv[0] : ddd_NAME;

    // Install signal handlers

    // On some systems (notably HP-UX), GDB has trouble finding what
    // function invoked the signal handler.  Hence, if the environment
    // variable DDD_NO_SIGNAL_HANDLERS is set, we do not install
    // signal handlers, causing DDD to report signals immediately.

    if (getenv("DDD_NO_SIGNAL_HANDLERS") == 0)
    {
	ddd_install_signal();	// Cleanup upon termination
	ddd_install_fatal();	// Fatal error
    }

#ifdef SIGCHLD
    // Setup signals: Restore default action for SIGCHLD signals.
    // Without asynchronous signal handling, DDD still runs well and
    // is less dependent on OS-specific signal handling.
    signal(SIGCHLD, (void (*)(int))SIG_DFL);
#endif

    // Check if we are to run without windows
    bool no_windows = false;

    // Don't run DDD setuid.  DDD invokes shell commands and even
    // shell scripts, such that all known problems of setuid shell
    // scripts apply.
    if (geteuid() != getuid())
	no_windows = true;

    // Check if the `--nw' option was given
    string gdb_name = "gdb";
    int gdb_option_pos = -1;
    for (int i = 1; i < argc; i++)
    {
	string arg = string(argv[i]);
	if ((arg == "--debugger" || arg == "-debugger") && i < argc - 1)
	{
	    gdb_name = argv[i + 1];
	    gdb_option_pos = i;
	}

	if (arg == "--nw" || arg == "-nw")
	{
	    if (gdb_option_pos >= 0)
	    {
		// Strip `--debugger NAME'
		for (int j = gdb_option_pos; j <= argc - 2; j++)
		    argv[j] = argv[j + 2];
		argc -= 2;
		i    -= 2;
	    }

	    // Strip `--nw'
	    for (int j = i; j <= argc - 1; j++)
		argv[j] = argv[j + 1];
	    argc -= 1;
	    i    -= 1;

	    no_windows = true;
	}
    }

    if (no_windows)
    {
	argv[0] = gdb_name;
	execvp(gdb_name, argv);
	perror(gdb_name);
	return 1;
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

    Widget toplevel = XtAppInitialize(&app_context,
				      DDD_CLASS_NAME,
				      0, 0,
				      &argc, argv,
				      ddd_fallback_resources,
				      args, arg);

    // Merge in ~/.dddinit resources
    XrmDatabase target = XtDatabase(XtDisplay(toplevel));
    XrmMergeDatabases(dddinit, &target);

    // Setup toplevel window
#if defined(HAVE_X11_XMU_EDITRES_H)
    XtAddEventHandler(toplevel, EventMask(0), true,
		      XtEventHandler(_XEditResCheckMessages), NULL);
#endif

    XtAppAddActions(app_context, actions, XtNumber(actions));
    Atom WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW", False);

    // Get application resources
    XtVaGetApplicationResources(toplevel, (XtPointer)&app_data,
				resources, XtNumber(resources),
				NULL);

    // Check the X configuration
    if (app_data.check_configuration)
	return check_x_configuration(toplevel, true);

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

    if (app_data.show_version 
	|| app_data.show_invocation 
	|| app_data.show_configuration
	|| app_data.show_resources
	|| app_data.show_manual)
	return 0;

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

    // Create command shell
    arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    command_shell =
	verify(XtCreatePopupShell("command_shell",
				  topLevelShellWidgetClass,
				  toplevel, args, arg));
    XmAddWMProtocolCallback(command_shell,
			    WM_DELETE_WINDOW, gdbCloseCommandWindowCB, 0);
#if defined(HAVE_X11_XMU_EDITRES_H)
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

    // Register own converters
    // Do this now to override Motif converters.
    registerOwnConverters();

    // Create menu bar
    MMDesc *menubar = combined_menubar;
    if (app_data.separate_data_window && app_data.separate_source_window)
	menubar = command_menubar;

    Widget menubar_w = MMcreateMenuBar (main_window, "menubar", menubar);
    MMaddCallbacks(menubar);

    group_iconify_w[CommandOptions]            = group_iconify_w[0];
    global_tab_completion_w[CommandOptions]    = global_tab_completion_w[0];
    separate_exec_window_w[CommandOptions]     = separate_exec_window_w[0];
    save_options_on_exit_w[CommandOptions]     = save_options_on_exit_w[0];
    save_history_on_exit_w[CommandOptions]     = save_history_on_exit_w[0];
    graph_show_grid_w[CommandOptions]          = graph_show_grid_w[0];
    graph_show_hints_w[CommandOptions]         = graph_show_hints_w[0];
    graph_snap_to_grid_w[CommandOptions]       = graph_snap_to_grid_w[0];
    graph_compact_layout_w[CommandOptions]     = graph_compact_layout_w[0];
    graph_auto_layout_w[CommandOptions]        = graph_auto_layout_w[0];
    find_words_only_w[CommandOptions]          = find_words_only_w[0];
    cache_source_files_w[CommandOptions]       = cache_source_files_w[0];
    cache_machine_code_w[CommandOptions]       = cache_machine_code_w[0];
    display_glyphs_w[CommandOptions]           = display_glyphs_w[0];
    disassemble_w[CommandOptions]              = disassemble_w[0];
    suppress_warnings_w[CommandOptions]        = suppress_warnings_w[0];
    set_focus_pointer_w[CommandOptions]        = set_focus_pointer_w[0];
    set_focus_explicit_w[CommandOptions]       = set_focus_explicit_w[0];
    set_scrolling_panner_w[CommandOptions]     = set_scrolling_panner_w[0];
    set_scrolling_scrollbars_w[CommandOptions] = set_scrolling_scrollbars_w[0];
    set_debugger_gdb_w[CommandOptions]         = set_debugger_gdb_w[0];
    set_debugger_dbx_w[CommandOptions]         = set_debugger_dbx_w[0];
    set_separate_windows_w[CommandOptions]     = set_separate_windows_w[0];
    set_attached_windows_w[CommandOptions]     = set_attached_windows_w[0];


    // Create Paned Window
    Widget paned_work_w = 
	verify(XtVaCreateWidget("paned_work_w",
				xmPanedWindowWidgetClass,
				main_window,
				NULL));

    if (!app_data.separate_source_window)
    {
	status_w = verify(XmCreateLabel(paned_work_w, "status_w", NULL, 0));
	XtManageChild(status_w);

	XtWidgetGeometry size;
	size.request_mode = CWHeight;
	XtQueryGeometry(status_w, NULL, &size);
	XtVaSetValues(status_w,
		      XmNpaneMaximum, size.height,
		      XmNpaneMinimum, size.height,
		      NULL);
    }

    // Graph area
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
#if defined(HAVE_X11_XMU_EDITRES_H)
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

	group_iconify_w[DataOptions]            = group_iconify_w[0];
	global_tab_completion_w[DataOptions]    = global_tab_completion_w[0];
	separate_exec_window_w[DataOptions]     = separate_exec_window_w[0];
	save_options_on_exit_w[DataOptions]     = save_options_on_exit_w[0];
	save_history_on_exit_w[DataOptions]     = save_history_on_exit_w[0];
	graph_show_grid_w[DataOptions]          = graph_show_grid_w[0];
	graph_show_hints_w[DataOptions]         = graph_show_hints_w[0];
	graph_snap_to_grid_w[DataOptions]       = graph_snap_to_grid_w[0];
	graph_compact_layout_w[DataOptions]     = graph_compact_layout_w[0];
	graph_auto_layout_w[DataOptions]        = graph_auto_layout_w[0];
	find_words_only_w[DataOptions]          = find_words_only_w[0];
        cache_source_files_w[DataOptions]       = cache_source_files_w[0];
        cache_machine_code_w[DataOptions]       = cache_machine_code_w[0];
        display_glyphs_w[DataOptions]           = display_glyphs_w[0];
        disassemble_w[DataOptions]              = disassemble_w[0];
	suppress_warnings_w[DataOptions]        = suppress_warnings_w[0];
	set_focus_pointer_w[DataOptions]        = set_focus_pointer_w[0];
	set_focus_explicit_w[DataOptions]       = set_focus_explicit_w[0];
	set_scrolling_panner_w[DataOptions]     = set_scrolling_panner_w[0];
	set_scrolling_scrollbars_w[DataOptions] = 
	    set_scrolling_scrollbars_w[0];
	set_debugger_gdb_w[DataOptions]         = set_debugger_gdb_w[0];
	set_debugger_dbx_w[DataOptions]         = set_debugger_dbx_w[0];
	set_separate_windows_w[DataOptions]     = set_separate_windows_w[0];
	set_attached_windows_w[DataOptions]     = set_attached_windows_w[0];

	data_disp_parent = 
	    verify(XtVaCreateManagedWidget ("data_paned_work_w",
					    xmPanedWindowWidgetClass,
					    data_main_window_w,
					    NULL));
    }
				  
    data_disp = new DataDisp (app_context,
			      data_disp_parent,
			      app_data.vslPath,
			      app_data.vslLibrary,
			      app_data.max_name_length,
			      app_data.panned_graph_editor);

    if (app_data.separate_data_window)
    {
	// More values for main window
	XtVaSetValues (data_main_window_w,
		       XmNmenuBar,    data_menubar_w,
		       XmNworkWindow, data_disp_parent,
		       NULL);
    }

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
#if defined(HAVE_X11_XMU_EDITRES_H)
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

	group_iconify_w[SourceOptions]            = group_iconify_w[0];
	global_tab_completion_w[SourceOptions]    = global_tab_completion_w[0];
	separate_exec_window_w[SourceOptions]     = separate_exec_window_w[0];
	save_options_on_exit_w[SourceOptions]     = save_options_on_exit_w[0];
	save_history_on_exit_w[SourceOptions]     = save_history_on_exit_w[0];
	graph_show_grid_w[SourceOptions]          = graph_show_grid_w[0];
	graph_show_hints_w[SourceOptions]         = graph_show_hints_w[0];
	graph_snap_to_grid_w[SourceOptions]       = graph_snap_to_grid_w[0];
	graph_compact_layout_w[SourceOptions]     = graph_compact_layout_w[0];
	graph_auto_layout_w[SourceOptions]        = graph_auto_layout_w[0];
	find_words_only_w[SourceOptions]          = find_words_only_w[0];
	cache_source_files_w[SourceOptions]       = cache_source_files_w[0];
	cache_machine_code_w[SourceOptions]       = cache_machine_code_w[0];
	display_glyphs_w[SourceOptions]           = display_glyphs_w[0];
	disassemble_w[SourceOptions]              = disassemble_w[0];
	suppress_warnings_w[SourceOptions]        = suppress_warnings_w[0];
	set_focus_pointer_w[SourceOptions]        = set_focus_pointer_w[0];
	set_focus_explicit_w[SourceOptions]       = set_focus_explicit_w[0];
	set_scrolling_panner_w[SourceOptions]     = set_scrolling_panner_w[0];
	set_scrolling_scrollbars_w[SourceOptions] = 
	    set_scrolling_scrollbars_w[0];
	set_debugger_gdb_w[SourceOptions]         = set_debugger_gdb_w[0];
	set_debugger_dbx_w[SourceOptions]         = set_debugger_dbx_w[0];
	set_separate_windows_w[SourceOptions]     = set_separate_windows_w[0];
	set_attached_windows_w[SourceOptions]     = set_attached_windows_w[0];

	source_view_parent = 
	    verify(XtVaCreateManagedWidget ("source_paned_work_w",
					    xmPanedWindowWidgetClass,
					    source_main_window_w,
					    NULL));

	status_w = 
	    verify(XmCreateLabel(source_view_parent, "status_w", NULL, 0));
	XtManageChild(status_w);

	XtWidgetGeometry size;
	size.request_mode = CWHeight;
	XtQueryGeometry(status_w, NULL, &size);
	XtVaSetValues(status_w,
		      XmNpaneMaximum, size.height,
		      XmNpaneMinimum, size.height,
		      NULL);
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

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(arg_cmd_w, NULL, &size);
    XtVaSetValues(arg_cmd_w,
		  XmNpaneMaximum, size.height,
		  XmNpaneMinimum, size.height,
		  NULL);

    // source_area (Befehle mit PushButton an gdb) ----------------------------
    source_buttons_w = make_buttons(source_view_parent, "source_buttons", 
				    app_data.source_buttons);

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

    // Don't edit the text until the first prompt appears.
    XmTextSetEditable(gdb_w, false);

    // source_area (Befehle mit PushButton an gdb) ----------------------------
    command_buttons_w = make_buttons(paned_work_w, "command_buttons", 
				     app_data.command_buttons);

    // Paned Window is done
    XtManageChild (paned_work_w);

    // More values for main window
    XtVaSetValues (main_window,
		   XmNmenuBar,    menubar_w,
		   XmNworkWindow, paned_work_w,
		   NULL);


    // All widgets are created at this point.
    set_status("Welcome to " DDD_NAME " " DDD_VERSION "!");

    // Setup history
    char *home = getenv("HOME");
    if (home == 0)
	gdb_history_file = ".ddd_history";
    else
	gdb_history_file = string(home) + "/.ddd_history";

    // Set host specification
    gdb_host = (app_data.debugger_host ? app_data.debugger_host : "");

    string initial_cmds;
    switch (type)
    {
    case GDB:
	initial_cmds = app_data.gdb_initial_cmds;
	break;
    case DBX:
	initial_cmds = app_data.dbx_initial_cmds;
	break;
    }

    if (initial_cmds == "")
	gdb_init_file = "";
    else
    {
	// Set initial commands
	if (remote_gdb())
	{
	    gdb_init_file = "${TMPDIR-/tmp}/ddd" + itostring(getpid());
	    Agent cat(sh_command("cat > " + gdb_init_file));
	    cat.start();

	    FILE *fp = cat.outputfp();
	    fputs(initial_cmds, fp);
	}
	else
	{
	    gdb_init_file = tmpnam(0);
	    ofstream os(gdb_init_file);
	    os << initial_cmds << "\n";
	}
    }

    // Startup debugger
    string gdb_call = 
	sh_command("exec " + 
		   build_gdb_call(type, 
				  app_data.debugger_command,
				  gdb_init_file,
				  argc, argv));

    gdb = new GDBAgent(app_context, gdb_call, type);

    gdb->set_trace_dialog(app_data.trace_dialog);

    // Setup handlers
    gdb->addBusyHandler(ReadyForQuestion, gdb_ready_for_questionHP);
    gdb->addBusyHandler(ReadyForCmd,      gdb_ready_for_cmdHP);
    gdb->addHandler    (InputEOF,         gdb_eofHP);
    gdb->addHandler    (ErrorEOF,         gdb_eofHP);
    DataDisp::set_handlers();

    source_arg->addHandler (Changed, source_argHP);
    source_arg->callHandlers();

    // Set the terminal type
    static string term_env = string("TERM=") + app_data.term_type;
    putenv(term_env);

    // Setup insertion position
    promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, messagePosition);

    // Setup help pixmap
    helpOnVersionPixmapProc = versionlogo;

    // Setup extra version info
    helpOnVersionExtraText = 
	MString(string(config_info).before("\n\n"), "rm") +
	MString("\n\n"
DDD_NAME " is free software and you are welcome to distribute copies of it\n"
"under certain conditions; type `show copying' to see the conditions.\n"
"There is absolutely no warranty for " DDD_NAME "; "
"type `show warranty' for details.\n"
"\n"
"If you appreciate this software, please send a picture postcard to:\n"
"\n"
"    Technische Universit\344t Braunschweig\n"
"    Abteilung Softwaretechnologie\n"
"    Gau\337stra\337e 17\n"
"    D-38092 Braunschweig\n"
"    GERMANY\n"
"\n"
"Send bug reports to <" ddd_NAME "-bugs@ips.cs.tu-bs.de>\n"
"Send comments and suggestions to <" ddd_NAME "@ips.cs.tu-bs.de>", "rm");

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
    if (source_view_shell)
	unmanage_sashes(source_view_parent, 3);

    untraverse_sashes(data_disp_parent);
    if (data_disp_shell)
	unmanage_sashes(data_disp_parent);

    untraverse_sashes(paned_work_w);
    if (source_view_shell && data_disp_shell)
	unmanage_sashes(paned_work_w);

    // Setup option states
    update_options();

    Boolean iconic;
    XtVaGetValues(toplevel, XmNiconic, &iconic, NULL);
    if (iconic)
    {
	// Startup all shells iconified
	initial_popup_shell(command_shell);
	initial_popup_shell(source_view_shell);
	initial_popup_shell(data_disp_shell);
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

    // Wait for the command shell to be mapped, such that we don't
    // lose debugger output.  This also decreases system load on
    // single-processor machines since DDD is idle when the debugger
    // starts.
    XSync(XtDisplay(command_shell), False);
    wait_until_mapped(command_shell);
    XmUpdateDisplay(command_shell);

    // Setup TTY interface
    if (app_data.tty_mode)
    {
	command_tty = new LiterateAgent(app_context);
	command_tty->addHandler(Input, tty_command);
	command_tty->addHandler(InputEOF, tty_eof);
	command_tty->start();

	string init_msg = XmTextGetString(gdb_w);
	// init_msg.gsub("\344", "ae");
	// init_msg.gsub("\366", "oe");
	// init_msg.gsub("\374", "ue");
	// init_msg.gsub("\337", "ss");
	// init_msg.gsub("\251", "(C)");
	tty_out(init_msg);
    }

    // Start debugger
    start_gdb();
    gdb_tty = gdb->slave_tty();

    // Invoke `ddd_setup_done' as soon as all initializations are done.
    XtAppAddWorkProc(app_context, ddd_setup_done, 0);

    // Main Loop
    int sig;
    if ((sig = setjmp(main_loop_env)))
    {
        // Caught a signal
        if (sig == SIGINT)
	{
	    // Propagate interrupt to GDB
	    gdb_keyboard_command = true;
	    _gdb_command("\003", gdb_w);
	}
	else
	{
	    // Show diagnostic
	    cerr << sigName(sig) << "\n";
	}
	gdb_question_running = false;
    }

    for (;;)
    {
	// Check if GDB is still running
	gdb->running();

	// Check if the command TTY is still open
	if (command_tty)
	    command_tty->running();

	if (app_data.synchronous_gdb && gdb->isBusyOnQuestion())
	{
	    // Synchronous mode: wait for GDB to answer question
	    XtAppProcessEvent(app_context, XtIMAlternateInput);
	}
	else if (XtAppPending(app_context) & XtIMAlternateInput)
	{
	    // Process pending GDB output
	    XtAppProcessEvent(app_context, XtIMAlternateInput);
	}
	else
	{
	    // Process next X event
	    XtAppProcessEvent(app_context, XtIMAll);
	}
    }

    // Never reached...
    return 0;
}


//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------

static Boolean ddd_setup_done(XtPointer)
{
    main_loop_entered = true;
    return True;		// Remove from the list of work procs
}


//-----------------------------------------------------------------------------
// Signal handling
//-----------------------------------------------------------------------------

// Setup signals: Cleanup on termination
void ddd_install_signal()
{
#ifdef SIGHUP
    signal(SIGHUP, ddd_signal);
#endif

#ifdef SIGTERM
    signal(SIGTERM, ddd_signal);
#endif
}

// Setup signals: Issue message on fatal errors
void ddd_install_fatal()
{
    // Make sure strsignal() is initialized properly
    (void)sigName(1);

#ifdef SIGINT
    signal(SIGINT, ddd_fatal);
#endif

#ifdef SIGFPE
    signal(SIGFPE, ddd_fatal);
#endif

#ifdef SIGILL
    signal(SIGILL, ddd_fatal);
#endif

#ifdef SIGSEGV
    signal(SIGSEGV, ddd_fatal);
#endif

#ifdef SIGBUS
    signal(SIGBUS, ddd_fatal);
#endif

#ifdef SIGABRT
    signal(SIGABRT, ddd_fatal);
#endif

#ifdef SIGIOT
    signal(SIGIOT, ddd_fatal);
#endif

#ifdef SIGTRAP
    signal(SIGTRAP, ddd_fatal);
#endif

#ifdef SIGEMT
    signal(SIGEMT, ddd_fatal);
#endif

#ifdef SIGSYS
    signal(SIGSYS, ddd_fatal);
#endif
}

//-----------------------------------------------------------------------------
// Set sensitivity
//-----------------------------------------------------------------------------

inline void set_sensitive(Widget w, bool state)
{
    if (w == 0)
	return;

    if (XtIsSensitive(w) == state)
	return;
    XtSetSensitive(w, state);
}


//-----------------------------------------------------------------------------
// Option handling
//-----------------------------------------------------------------------------

// Reflect state in option menus
void update_options()
{
    Arg args[10];
    int arg = 0;

    for (int i = 1; i < 4; i++)
    {
	if (group_iconify_w[i] == 0)
	    continue;

	XtVaSetValues(group_iconify_w[i],
		      XmNset, app_data.group_iconify, NULL);
	XtVaSetValues(global_tab_completion_w[i],
		      XmNset, app_data.global_tab_completion, NULL);
	XtVaSetValues(separate_exec_window_w[i],
		      XmNset, app_data.separate_exec_window, NULL);
	XtVaSetValues(save_options_on_exit_w[i],
		      XmNset, app_data.save_options_on_exit, NULL);
	XtVaSetValues(save_history_on_exit_w[i],
		      XmNset, app_data.save_history_on_exit, NULL);

	XtVaSetValues(find_words_only_w[i],
		      XmNset, app_data.find_words_only, NULL);
	XtVaSetValues(cache_source_files_w[i],
		      XmNset, app_data.cache_source_files, NULL);
	XtVaSetValues(cache_machine_code_w[i],
		      XmNset, app_data.cache_machine_code, NULL);
	XtVaSetValues(display_glyphs_w[i],
		      XmNset, app_data.display_glyphs, NULL);
	XtVaSetValues(disassemble_w[i],
		      XmNset, app_data.disassemble, NULL);

	XtVaSetValues(suppress_warnings_w[i],
		      XmNset, app_data.suppress_warnings, NULL);

	set_sensitive(disassemble_w[i],        gdb->type() == GDB);
	set_sensitive(cache_machine_code_w[i], gdb->type() == GDB);

	Boolean state;
	arg = 0;
	XtSetArg(args[arg], XtNshowGrid, &state); arg++;
	XtGetValues(data_disp->graph_edit, args, arg);
	arg = 0;
	XtSetArg(args[arg], XmNset, state); arg++;
	XtSetValues(graph_show_grid_w[i], args, arg);

	arg = 0;
	XtSetArg(args[arg], XtNsnapToGrid, &state); arg++;
	XtGetValues(data_disp->graph_edit, args, arg);
	arg = 0;
	XtSetArg(args[arg], XmNset, state); arg++;
	XtSetValues(graph_snap_to_grid_w[i], args, arg);

	arg = 0;
	XtSetArg(args[arg], XtNshowHints, &state); arg++;
	XtGetValues(data_disp->graph_edit, args, arg);
	arg = 0;
	XtSetArg(args[arg], XmNset, state); arg++;
	XtSetValues(graph_show_hints_w[i], args, arg);

	LayoutMode mode;
	arg = 0;
	XtSetArg(args[arg], XtNlayoutMode, &mode); arg++;
	XtGetValues(data_disp->graph_edit, args, arg);
	arg = 0;
	XtSetArg(args[arg], XmNset, mode == CompactLayoutMode); arg++;
	XtSetValues(graph_compact_layout_w[i], args, arg);

	arg = 0;
	XtSetArg(args[arg], XtNautoLayout, &state); arg++;
	XtGetValues(data_disp->graph_edit, args, arg);
	arg = 0;
	XtSetArg(args[arg], XmNset, state); arg++;
	XtSetValues(graph_auto_layout_w[i], args, arg);

	unsigned char policy = '\0';
	XtVaGetValues(command_shell, 
		      XmNkeyboardFocusPolicy, &policy,
		      NULL);
	XtVaSetValues(set_focus_pointer_w[i],
		      XmNset, policy == XmPOINTER, NULL);
	XtVaSetValues(set_focus_explicit_w[i],
		      XmNset, policy == XmEXPLICIT, NULL);

	XtVaSetValues(set_scrolling_panner_w[i],
		      XmNset, app_data.panned_graph_editor, NULL);
	XtVaSetValues(set_scrolling_scrollbars_w[i],
		      XmNset, !app_data.panned_graph_editor, NULL);

	Boolean separate = 
	    app_data.separate_data_window || app_data.separate_source_window;
	XtVaSetValues(set_separate_windows_w[i],
		      XmNset, separate, NULL);
	XtVaSetValues(set_attached_windows_w[i],
		      XmNset, !separate, NULL);

	DebuggerType type = debugger_type(app_data.debugger);
	XtVaSetValues(set_debugger_gdb_w[i],
		      XmNset, type == GDB, NULL);
	XtVaSetValues(set_debugger_dbx_w[i],
		      XmNset, type == DBX, NULL);
    }

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
}


//-----------------------------------------------------------------------------
// DDD logo
//-----------------------------------------------------------------------------

// Return pixmaps suitable for icons on the root window
Pixmap iconlogo(Widget w)
{
    GC gc = DefaultGC(XtDisplay(w), XScreenNumberOfScreen(XtScreen(w)));
    XGCValues gcv;
    XGetGCValues(XtDisplay(w), gc, GCForeground | GCBackground, &gcv);
		      
    int depth = PlanesOfScreen(XtScreen(w));
    Pixmap icon = 
	XCreatePixmapFromBitmapData(XtDisplay(w),
				    RootWindowOfScreen(XtScreen(w)),
				    dddlogo_bits,
				    dddlogo_width, dddlogo_height,
				    gcv.foreground, gcv.background,
				    depth);

    return icon;
}

Pixmap iconmask(Widget w)
{
    return XCreateBitmapFromData(XtDisplay(w),
				 RootWindowOfScreen(XtScreen(w)),
				 dddmask_bits,
				 dddmask_width, dddmask_height);
}

// Return pixmaps suitable for the widget W
Pixmap versionlogo(Widget w)
{
    Pixel foreground, background;

    XtVaGetValues(w,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  NULL);

    int depth = PlanesOfScreen(XtScreen(w));
    Pixmap logo = 
	XCreatePixmapFromBitmapData(XtDisplay(w),
				    XtWindow(w),
				    dddlogo_bits,
				    dddlogo_width, dddlogo_height,
				    foreground, background,
				    depth);

    return logo;
}


//-----------------------------------------------------------------------------
// Window Manager Functions
//-----------------------------------------------------------------------------
    
void wm_set_icon(Display *display, Window shell, Pixmap icon, Pixmap mask)
{
    XWMHints *wm_hints = XAllocWMHints();

    wm_hints->flags       = IconPixmapHint | IconMaskHint;
    wm_hints->icon_pixmap = icon;
    wm_hints->icon_mask   = mask;

    XSetWMHints(display, shell, wm_hints);

    XFree(wm_hints);
}

void wm_set_icon(Widget shell, Pixmap icon, Pixmap mask)
{
    XtVaSetValues(shell,
		  XmNiconPixmap, icon,
#ifdef LESSTIF_VERSION		// LessTif 0.1 does not define XmNiconMask
		  "iconMask", mask,
#else
		  XmNiconMask, mask,
#endif
		  NULL);
    wm_set_icon(XtDisplay(shell), XtWindow(shell), icon, mask);
}

void wm_set_group_leader(Display * /* display */,
			 Window /* shell_window */, Window /* leader_window */)
{
    // Disabled, since (at least in fvwm) it has no effect but to
    // disable generation of individual icons.
#if 0
    XWMHints *wm_hints = XAllocWMHints();

    wm_hints->flags        = WindowGroupHint;
    wm_hints->window_group = leader_window;

    XSetWMHints(display, shell_window, wm_hints);

    XFree(wm_hints);
#endif
}

void wm_set_name(Display *display, Window shell_window,
		 string title, string icon)
{
    strip_final_blanks(title);
    strip_final_blanks(icon);

    if (title != "")
	XStoreName(display, shell_window, (String)title);
    if (icon != "")
	XSetIconName(display, shell_window, (String)icon);
}

void wm_set_name(Widget shell, string title, string icon)
{
    strip_final_blanks(title);
    strip_final_blanks(icon);

    XtVaSetValues(shell,
		  XmNiconName, (char *)icon,
		  XmNtitle,    (char *)title,
		  NULL);
    
    wm_set_name(XtDisplay(shell), XtWindow(shell), title, icon);
}


//-----------------------------------------------------------------------------
// Show version
//-----------------------------------------------------------------------------

void show_version()
{
    cout << DDD_NAME " " DDD_VERSION " (" DDD_HOST "), "
	"Copyright (C) 1995 TU Braunschweig, Germany.\n";
}

//-----------------------------------------------------------------------------
// Show invocation
//-----------------------------------------------------------------------------

void show_invocation(DebuggerType type)
{
    string gdb_version = "";
    string options     = "";
    string base        = "";

    string gdb_help = sh_command(string(app_data.debugger_command) 
				 + " -h");
    switch (type)
    {
    case GDB:
    {
	base = "GDB, the GNU debugger.";

	Agent help(gdb_help);
	help.start();

	FILE *fp = help.inputfp();
	if (fp)
	{
	    enum { Init, Options, Other, Done } state = Init;
	    char buf[BUFSIZ];

	    while (fgets(buf, sizeof(buf), fp))
	    {
		if (buf[0] && buf[strlen(buf) - 1] == '\n')
		    buf[strlen(buf) - 1] = '\0';

		string option;
		switch (state)
		{
		case Init:
		    gdb_version = string(buf) + "\n";
		    state = Other;
		    break;

		case Other:
		    if (string(buf).contains("Options:"))
			state = Options;
		    break;

		case Options:
		    option = buf;
		    if (option == "")
			state = Done;
		    else
			options += option + "\n";
		    break;

		case Done:
		    break;
		}
	    }
	}
	break;

    }
    case DBX:
	base = "DBX, the UNIX debugger.";
	options = "  [DBX options]      Pass option to DBX.\n";
	break;
    }

    show_version();
    cout << gdb_version <<
	"This is DDD, the data display debugger, based on "
	<< base << "\n" << 
	"Usage:\n"
	"    " ddd_NAME " [options]"
	" executable-file [core-file or process-id]]\n"
	"Options:\n"
	<< options <<
	"  --dbx              Invoke DBX as inferior debugger.\n"
	"  --gdb              Invoke GDB as inferior debugger.\n"
	"  --debugger NAME    Invoke inferior debugger as NAME.\n"
	"  --host HOST        Run inferior debugger on HOST.\n"
	"  --login LOGIN      Use LOGIN for connecting to host.\n"
	"  --vsl-library LIB  Load VSL library LIB.\n"
	"  --vsl-path PATH    Look for VSL libraries in PATH.\n"
	"  --trace-dialog     Show inferior debugger I/O on standard error.\n"
	"  --trace-shell      Show shell commands on standard error.\n"
	"  --exec-window      Create a window for"
	" running debugged programs.\n"
	"  --no-exec-window   Do not create a window for"
	" running debugged programs.\n"
	"  --attach-windows   Attach data and source windows to"
	" command window.\n"
	"  --separate-windows Do not attach data and source windows to"
	" command window.\n"
	"  --scrolled-graph   Use Motif scrollbars"
	" for moving the data display.\n"
	"  --panned-graph     Use an Athena panner"
	" for moving the data display.\n"
        "  --tty              Use the tty as additional debugger interface.\n"
	"  --version          Show the DDD version and exit.\n"
	"  --configuration    Show the DDD configuration flags and exit.\n"
	"  --manual           Show the DDD manual and exit.\n"
	"\n"
	"Standard X options are also accepted, such as:\n"
	"  -display DISPLAY    Run on X server DISPLAY.\n"
	"  -geometry GEOMETRY  Specify initial size and location.\n"
	"  -foreground COLOR   Use COLOR as foreground color.\n"
	"  -background COLOR   Use COLOR as background color.\n"
	"  -xrm RESOURCE       Specify a resource name and value.\n"
	"\n"
	"For more information, consult the " DDD_NAME " `Help' menu,"
	" type `help' from\n"
	"within " DDD_NAME ", "
	"or consult the manual pages of " DDD_NAME " and the"
	" inferior debugger.\n";
}


//-----------------------------------------------------------------------------
// Show Configuration
//-----------------------------------------------------------------------------

#define _stringize(x) #x
#define stringize(x) _stringize(x)

void show_configuration()
{    
    show_version();
    cout << 
	"Using X" stringize(X_PROTOCOL) "R" stringize(XlibSpecificationRelease)
	 ", Xt" stringize(X_PROTOCOL) "R" stringize(XtSpecificationRelease)
	 ", Motif " stringize(XmVERSION) "." stringize(XmREVISION) "\n";
    cout << config_info;
}


//-----------------------------------------------------------------------------
// Show Resources
//-----------------------------------------------------------------------------

void show_resources(XrmDatabase db)
{
    string tmpfile = tmpnam(0);
    XrmPutFileDatabase(db, tmpfile);
    
    {
	ifstream is(tmpfile);
	int c;
	while ((c = is.get()) != EOF)
	    cout.put(char(c));
    }

    unlink(tmpfile);
}


//-----------------------------------------------------------------------------
// Show Manual Page
//-----------------------------------------------------------------------------

void show_manual()
{
    FILE *fp = 0;
    if (isatty(fileno(stdout)))
    {
	char *pager = getenv("PAGER");
	if (fp == 0 && pager != 0)
	    fp = popen(pager, "w");
	if (fp == 0)
	    fp = popen("less", "w");
	if (fp == 0)
	    fp = popen("more", "w");
    }
    if (fp == 0)
    {
	fputs(ddd_man_page, stdout);
    }
    else
    {
	fputs(ddd_man_page, fp);
	pclose(fp);
    }
}

//-----------------------------------------------------------------------------
// Shell counter
//-----------------------------------------------------------------------------

int running_shells()
{
    return int(command_shell_state != PoppedDown)
	+ int(source_view_shell_state != PoppedDown)
	+ int(data_disp_shell_state != PoppedDown);
}


//-----------------------------------------------------------------------------
// Opening files
//-----------------------------------------------------------------------------

static Widget find_shell(Widget w = 0)
{
    if (w == 0)
	w = gdb_last_origin;
    if (w == 0)
	return command_shell;

    Widget parent = findTopLevelShellParent(w);
    if (parent == 0)
	return command_shell;

    if (!XtIsRealized(parent))
	return command_shell;

    XWindowAttributes xwa;
    XGetWindowAttributes(XtDisplay(parent), XtWindow(parent), &xwa);
    if (xwa.map_state != IsViewable)
	return command_shell;

    return parent;
}

typedef void (*FileSearchProc)(Widget fs, 
			       XmFileSelectionBoxCallbackStruct *cbs);

static VarArray<Widget> file_filters;
static VarArray<Widget> file_dialogs;

static string current_file_filter = "";

// Make sure that every change in one filter is reflected in all others
static void SyncFiltersCB(Widget dialog, XtPointer, XtPointer)
{
    static bool entered = false;

    if (entered)
	return;

    entered = true;

    // clog << "widget = " << longName(text) << "\n";

    while (dialog != 0 && !XmIsFileSelectionBox(dialog))
	dialog = XtParent(dialog);
	
    // clog << "dialog = " << longName(dialog) << "\n";

    Widget text = XmFileSelectionBoxGetChild(dialog, XmDIALOG_FILTER_TEXT);
    String _current_file_filter = XmTextGetString(text);
    current_file_filter = _current_file_filter;
    XtFree(_current_file_filter);

    for (int i = 0; i < file_filters.size(); i++)
    {
	if (file_dialogs[i] != dialog)
	{
	    // clog << "other dialog = " << longName(file_dialogs[i]) << "\n";
	    XmTextSetString(file_filters[i], current_file_filter);
	}
    }

    entered = false;
}

// Make sure that every new filter call is performed in all other
// dialogs as well
static void FilterAllCB(Widget dialog, XtPointer client_data, 
			XtPointer call_data)
{
    SyncFiltersCB(dialog, client_data, call_data);

    // clog << "widget = " << longName(dialog) << "\n";

    while (dialog != 0 && !XmIsFileSelectionBox(dialog))
	dialog = XtParent(dialog);
	
    // clog << "dialog = " << longName(dialog) << "\n";

    for (int i = 0; i < file_dialogs.size(); i++)
    {
	if (file_dialogs[i] != dialog)
	{
	    // clog << "other dialog = " << longName(file_dialogs[i]) << "\n";
	    XmFileSelectionDoSearch(file_dialogs[i], NULL);
	}
    }
}

// Create a file dialog NAME with DO_SEARCH_FILES and DO_SEARCH_DIRS
// as search procedures for files and directories, respectively, and
// OK_CALLBACK as the procedure called when a file is selected.
Widget file_dialog(Widget w, const string& name,
		   FileSearchProc do_search_files,
		   FileSearchProc do_search_dirs,
		   XtCallbackProc ok_callback)
{
    Delay delay(w);

    Arg args[10];
    int arg = 0;

    string pwd;

    arg = 0;
    if (do_search_files)
    {
	XtSetArg(args[arg], XmNfileSearchProc, do_search_files); arg++;
    }
    if (do_search_dirs)
    {
	XtSetArg(args[arg], XmNdirSearchProc, do_search_dirs); arg++;
    }

    if (remote_gdb())
    {
	pwd = gdb_question("pwd");
	if (pwd == string(-1))
	{
	    post_error("Cannot get current remote directory", "pwd_error", w);
	    pwd = "";
	}
	else
	{
	    switch(gdb->type())
	    {
	    case GDB:
		pwd = pwd.after("directory ");
		pwd = pwd.before(int(pwd.length()) - 2);
		break;

	    case DBX:
		pwd = pwd.before('\n');
		break;
	    }
	}

	if (pwd != "")
	{
	    static MString xmpwd;
	    xmpwd = pwd;
	    XtSetArg(args[arg], XmNdirectory, xmpwd.xmstring()); arg++;
	}
    }

    Widget dialog = 
	verify(XmCreateFileSelectionDialog(w, name, args, arg));
    Delay::register_shell(dialog);
    XtAddCallback(dialog, XmNokCallback,     ok_callback, 0);
    XtAddCallback(dialog, XmNcancelCallback, UnmanageThisCB, 
		  XtPointer(dialog));
    XtAddCallback(dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    Widget filter = XmFileSelectionBoxGetChild(dialog, XmDIALOG_FILTER_TEXT);
    file_filters += filter;
    if (current_file_filter != "")
	XmTextSetString(filter, current_file_filter);
    XtAddCallback(filter, XmNvalueChangedCallback, SyncFiltersCB, 0);

    Widget filter_button = 
	XmFileSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON);
    XtAddCallback(filter_button, XmNactivateCallback, FilterAllCB, 0);

    file_dialogs += dialog;

    return dialog;
}

// Search for remote files and directories, using the command CMD
static void searchRemote(Widget fs,
			 XmFileSelectionBoxCallbackStruct *cbs,
			 String cmd,
			 bool search_dirs)
{
    Delay delay(fs);

    int nitems = 0;
    int size = 256;
    XmStringTable items = 
	XmStringTable(XtMalloc(size * sizeof(XmString)));

    String mask;
    if (!XmStringGetLtoR(cbs->mask, MSTRING_DEFAULT_CHARSET, &mask))
	return;
    String dir;
    if (!XmStringGetLtoR(cbs->dir, MSTRING_DEFAULT_CHARSET, &dir))
	return;

    if (search_dirs)
    {
	string extra_dir = string(dir) + ".";
	items[nitems++] = 
	    XmStringCreateLtoR(extra_dir, MSTRING_DEFAULT_CHARSET);
	extra_dir = string(dir) + "..";
	items[nitems++] = 
	    XmStringCreateLtoR(extra_dir, MSTRING_DEFAULT_CHARSET);
    }

    string command = cmd;
    command.gsub("@MASK@", mask);
    command = sh_command(command);

    Agent search(command);
    search.start();

    FILE *fp = search.inputfp();
    if (fp == 0)
    {
	perror(command);
	return;
    }

    char buf[BUFSIZ];
    while (fgets(buf, sizeof(buf), fp))
    {
	if (buf[0] && buf[strlen(buf) - 1] == '\n')
	    buf[strlen(buf) - 1] = '\0';

	if (nitems >= size)
	{
	    size += 256;
	    items = XmStringTable(XtRealloc((char *)items,
					    size * sizeof(XmString)));
	}
	    
	items[nitems++] = XmStringCreateLtoR(buf, MSTRING_DEFAULT_CHARSET);
    }

    if (search_dirs)
    {
	XtVaSetValues(fs,
		      XmNdirListItems,     items,
		      XmNdirListItemCount, nitems,
		      XmNdirectoryValid,   True,
		      XmNlistUpdated,      True,
		      NULL);
    }
    else
    {
	if (nitems > 0)
	{
	    XtVaSetValues(fs,
			  XmNfileListItems,     items,
			  XmNfileListItemCount, nitems,
			  XmNdirSpec,           items[0],
			  XmNlistUpdated,       True,
			  NULL);
	}
	else
	{
	    XtVaSetValues(fs,
			  XmNfileListItems,     0,
			  XmNfileListItemCount, 0,
			  XmNlistUpdated,       True,
			  NULL);
	}
    }
}


static void searchRemoteExecFiles(Widget fs,
				  XmFileSelectionBoxCallbackStruct *cbs)
{
    searchRemote(fs, cbs, app_data.list_exec_command, false);
}

static void searchRemoteCoreFiles(Widget fs,
				  XmFileSelectionBoxCallbackStruct *cbs)
{
    searchRemote(fs, cbs, app_data.list_core_command, false);
}

static void searchRemoteSourceFiles(Widget fs,
				    XmFileSelectionBoxCallbackStruct *cbs)
{
    searchRemote(fs, cbs, app_data.list_source_command, false);
}

static void searchRemoteDirectories(Widget fs,
				    XmFileSelectionBoxCallbackStruct *cbs)
{
    searchRemote(fs, cbs, app_data.list_dir_command, true);
}

static void sort(char *a[], int size)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= size);
    do {
	h /= 3;
	for (int i = h; i < size; i++)
	{
	    char *v = a[i];
	    int j;
	    for (j = i; j >= h && strcmp(a[j - h], v) > 0; j -= h)
		a[j] = a[j - h];
	    if (i != j)
		a[j] = v;
	}
    } while (h != 1);
}

// Search for local files and directories, using the predicate IS_OKAY
static void searchLocal(Widget fs,
			XmFileSelectionBoxCallbackStruct *cbs,
			bool is_okay(const string& file_name))
{
    Delay delay(fs);

    String mask;
    if (!XmStringGetLtoR(cbs->mask, MSTRING_DEFAULT_CHARSET, &mask))
	return;

    char **files = glob_filename(mask);
    if (files == (char **)0)
    {
	cerr << mask << ": glob failed\n";
    }
    else if (files == (char **)-1)
    {
	post_error(string(mask) + ": " + strerror(errno));
    }
    else
    {
	int count;
	for (count = 0; files[count] != 0; count++)
	    ;
	sort(files, count);

	XmStringTable items = 
	    XmStringTable(XtMalloc(count * sizeof(XmString)));

	int nitems = 0;
	for (int i = 0; files[i] != 0; i++)
	{
	    if (is_okay(files[i]))
		items[nitems++] = XmStringCreateLtoR(files[i], 
						     MSTRING_DEFAULT_CHARSET);
	    free(files[i]);
	}
	free(files);

	if (nitems > 0)
	{
	    XtVaSetValues(fs,
			  XmNfileListItems,     items,
			  XmNfileListItemCount, nitems,
			  XmNdirSpec,           items[0],
			  XmNlistUpdated,       True,
			  NULL);

	    XtFree((char *)items);
	    return;
	}
    }

    // Error or nothing found 
    XtVaSetValues(fs,
		  XmNfileListItems,     0,
		  XmNfileListItemCount, 0,
		  XmNlistUpdated,       True,
		  NULL);
}

static void searchLocalExecFiles(Widget fs,
				 XmFileSelectionBoxCallbackStruct *cbs)
{
    searchLocal(fs, cbs, is_exec_file);
}

static void searchLocalCoreFiles(Widget fs,
				 XmFileSelectionBoxCallbackStruct *cbs)
{
    searchLocal(fs, cbs, is_core_file);
}

static void searchLocalSourceFiles(Widget fs,
				   XmFileSelectionBoxCallbackStruct *cbs)
{
    searchLocal(fs, cbs, is_source_file);
}


// Get the file name from the file selection box W
string get_file(Widget w, XtPointer, XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct *cbs = 
	(XmFileSelectionBoxCallbackStruct *)call_data;

    String s;
    if (!XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &s))
	return string(-1);

    string filename = s;
    XtFree(s);

    if (filename == "" || filename[0] != '/')
    {
	String dir;
	if (!XmStringGetLtoR(cbs->dir, MSTRING_DEFAULT_CHARSET, &dir))
	    return string(-1);

	filename = string(dir) + "/" + filename;
	XtFree(dir);
    }

    if (is_directory(filename))
    {
	MString filter(filename);
	XmFileSelectionDoSearch(w, filter);
	return "";
    }

    return filename;
}

// OK pressed in `Open File'
void openFileDone(Widget w, XtPointer client_data, XtPointer call_data)
{
    string filename = get_file(w, client_data, call_data);
    if (filename == "")
	return;

    XtUnmanageChild(w);

    if (filename != string(-1))
    {
	switch(gdb->type())
	{
	case GDB:
	    gdb_command("file " + filename);
	    break;

	case DBX:
	    gdb_command("debug " + filename);
	    break;
	}
    }
}

// OK pressed in `Open Core'
void openCoreDone(Widget w, XtPointer client_data, XtPointer call_data)
{
    string corefile = get_file(w, client_data, call_data);
    if (corefile == "")
	return;

    XtUnmanageChild(w);

    if (corefile != string(-1))
    {
	switch(gdb->type())
	{
	case GDB:
	    gdb_command("core-file " + corefile);
	    break;

	case DBX:
	    string file = gdb_question("debug");
	    if (file == string(-1))
		post_gdb_busy();
	    else if (file.contains("Debugging: ", 0))
	    {
		file = file.after(": ");
		strip_final_blanks(file);
		gdb_command("debug " + file + " " + corefile);
	    }
	    else
		post_gdb_message(file);
	}
    }
}

// OK pressed in `Open Source'
void openSourceDone(Widget w, XtPointer client_data, XtPointer call_data)
{
    string filename = get_file(w, client_data, call_data);
    if (filename == "")
	return;

    XtUnmanageChild(w);

    if (filename != string(-1))
	source_view->read_file(filename);
}


// Create various file dialogs

void gdbOpenFileCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	if (remote_gdb())
	    dialog = file_dialog(w, "exec_files", searchRemoteExecFiles, 
				 searchRemoteDirectories, openFileDone);
	else
	    dialog = file_dialog(w, "exec_files", searchLocalExecFiles, 
				 0, openFileDone);
    }

    XtManageChild(dialog);
}

void gdbOpenCoreCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	if (remote_gdb())
	    dialog = file_dialog(w, "core_files", searchRemoteCoreFiles, 
				 searchRemoteDirectories, openCoreDone);
	else
	    dialog = file_dialog(w, "core_files", searchLocalCoreFiles, 
				 0, openCoreDone);
    }

    XtManageChild(dialog);
}

void gdbOpenSourceCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	if (remote_gdb())
	    dialog = file_dialog(w, "source_files", searchRemoteSourceFiles, 
				 searchRemoteDirectories, openSourceDone);
	else
	    dialog = file_dialog(w, "source_files", searchLocalSourceFiles, 
				 0, openSourceDone);
    }

    XtManageChild(dialog);
}


//-----------------------------------------------------------------------------
// Window management
//-----------------------------------------------------------------------------

void initial_popup_shell(Widget w)
{
    if (w == 0)
	return;

    assert(XtIsTopLevelShell(w));

    Widget toplevel = w;
    while (XtParent(toplevel))
	toplevel = XtParent(toplevel);

    assert(XtIsTopLevelShell(toplevel));

    Boolean iconic;
    XtVaGetValues(toplevel, XmNiconic, &iconic, NULL);
    XtVaSetValues(w, XmNiconic, iconic, NULL);
    WindowState state = iconic ? Iconic : PoppedUp;

    if (w == command_shell)
	command_shell_state        = state;
    else if (w == data_disp_shell)
	data_disp_shell_state      = state;
    else if (w == source_view_shell)
	source_view_shell_state    = state;

    XtPopup(w, XtGrabNone);
}

void popup_shell(Widget w)
{
    if (w == 0)
	return;

    XtPopup(w, XtGrabNone);

    if (w == command_shell)
	command_shell_state        = PoppedUp;
    else if (w == data_disp_shell)
	data_disp_shell_state      = PoppedUp;
    else if (w == source_view_shell)
	source_view_shell_state    = PoppedUp;

    // Deiconify window
    XMapWindow(XtDisplay(w), XtWindow(w));

    // Place window on top
    XWindowChanges changes;
    changes.stack_mode = Above;
    XReconfigureWMWindow(XtDisplay(w), XtWindow(w), 
			 XScreenNumberOfScreen(XtScreen(w)),
			 CWStackMode, &changes);

#if 0
    wait_until_mapped(w);

    // Get focus
    XSetInputFocus(XtDisplay(w), XtWindow(w), RevertToParent, 
		   XtLastTimestampProcessed(XtDisplay(w)));
#endif

    // Try this one
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
}

void popdown_shell(Widget w)
{
    if (w == 0)
	return;

    if (w == command_shell)
	command_shell_state     = PoppedDown;
    else if (w == data_disp_shell)
	data_disp_shell_state   = PoppedDown;
    else if (w == source_view_shell)
	source_view_shell_state = PoppedDown;

    XtPopdown(w);
}

void iconify_shell(Widget w)
{
    if (w == 0)
	return;

    if (w == command_shell)
	command_shell_state     = Iconic;
    else if (w == data_disp_shell)
	data_disp_shell_state   = Iconic;
    else if (w == source_view_shell)
	source_view_shell_state = Iconic;

    XIconifyWindow(XtDisplay(w), XtWindow(w),
		   XScreenNumberOfScreen(XtScreen(w)));
}

void popup_tty(Widget shell)
{
    if (separate_tty_window)
    {
	// Deiconify window
	XMapWindow(XtDisplay(shell), separate_tty_window);

	// Place window on top
	XWindowChanges changes;
	changes.stack_mode = Above;
	XReconfigureWMWindow(XtDisplay(shell), separate_tty_window, 
			     XScreenNumberOfScreen(XtScreen(shell)),
			     CWStackMode, &changes);
    }
}

void iconify_tty(Widget shell)
{
    if (separate_tty_window)
    {
	XIconifyWindow(XtDisplay(shell), separate_tty_window,
		       XScreenNumberOfScreen(XtScreen(shell)));
    }
}


void StructureNotifyEH(Widget w, XtPointer, XEvent *event, Boolean *)
{
    bool synthetic = false;

    if (w == command_shell)
	synthetic = (command_shell_state == Transient);
    else if (w == data_disp_shell)
	synthetic = (data_disp_shell_state == Transient);
    else if (w == source_view_shell)
	synthetic = (source_view_shell_state == Transient);

    // if (synthetic)
    //    clog << "synthetic event: ";

    switch (event->type)
    {
    case MapNotify:
	// clog << XtName(w) << " is mapped\n";

	// Reflect state
	if (w == command_shell && command_shell_state != PoppedUp)
	    command_shell_state = PoppedUp;
	else if (w == data_disp_shell && data_disp_shell_state != PoppedUp)
	    data_disp_shell_state = PoppedUp;
	else if (w == source_view_shell && source_view_shell_state != PoppedUp)
	    source_view_shell_state = PoppedUp;
	else
	    return;

	if (!synthetic && app_data.group_iconify)
	{
	    // Map all other windows as well
	    if (command_shell_state == Iconic)
	    {
		popup_shell(command_shell);
		command_shell_state = Transient;
	    }
	    if (data_disp_shell_state == Iconic)
	    {
		popup_shell(data_disp_shell);
		data_disp_shell_state = Transient;
	    }
	    if (source_view_shell_state == Iconic)
	    {
		popup_shell(source_view_shell);
		source_view_shell_state = Transient;
	    }
	    popup_tty(command_shell);
	}
	break;

    case UnmapNotify:
	// clog << XtName(w) << " is unmapped\n";

	// Reflect state
	if (w == command_shell
	    && command_shell_state != Iconic
	    && command_shell_state != PoppedDown)
	    command_shell_state = Iconic;
	else if (w == data_disp_shell
		 && data_disp_shell_state != Iconic
		 && data_disp_shell_state != PoppedDown)
	    data_disp_shell_state = Iconic;
	else if (w == source_view_shell
		 && source_view_shell_state != Iconic
		 && source_view_shell_state != PoppedDown)
	    source_view_shell_state = Iconic;
	else
	    return;

	if (!synthetic && app_data.group_iconify)
	{
	    // Iconify all other windows as well
	    if (command_shell_state == PoppedUp)
	    {
		iconify_shell(command_shell);
		command_shell_state = Transient;
	    }
	    if (data_disp_shell_state == PoppedUp)
	    {
		iconify_shell(data_disp_shell);
		data_disp_shell_state = Transient;
	    }
	    if (source_view_shell_state == PoppedUp)
	    {
		iconify_shell(source_view_shell);
		source_view_shell_state = Transient;
	    }
	    iconify_tty(command_shell);
	}
	break;

    default:
	// Any other event...
	break;
    }
}


void gdbCloseCommandWindowCB(Widget w, 
			    XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }
    popdown_shell(command_shell);
}

void gdbCloseSourceWindowCB(Widget w, 
			   XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }
    popdown_shell(source_view_shell);
}

void gdbCloseDataWindowCB(Widget w,
			 XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }
    popdown_shell(data_disp_shell);
}

void gdbCloseExecWindowCB(Widget w,
			 XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }
    kill_exec_tty();
}


void gdbOpenCommandWindowCB(Widget, XtPointer, XtPointer)
{
    popup_shell(command_shell);
}

void gdbOpenSourceWindowCB(Widget, XtPointer, XtPointer)
{
    popup_shell(source_view_shell);
}

void gdbOpenDataWindowCB(Widget, XtPointer, XtPointer)
{
    popup_shell(data_disp_shell);
}

void gdbOpenExecWindowCB(Widget, XtPointer, XtPointer)
{
    if (separate_tty_pid == 0)
	startup_exec_tty();
    popup_tty(command_shell);
}


//-----------------------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------------------

static string unctrl(string name)
{
    if (name.length() >= 2)
    {
	if (name[0] == '^')
	    if (name[1] == '?')
		return '\177';
	    else
		return char(toupper(name[1]) - '@');

	if (name[0] == '\\')
	    return uncook(name);
    }

    if (name.length() == 1 && isalpha(name[0]))
	return char(toupper(name[0]) - '@');

    return name;
}

void YnButtonCB(Widget dialog, 
		XtPointer client_data, 
		XtPointer call_data)
{
    _gdb_out(string((char *)client_data) + '\n');
    gdbCommandCB(dialog, client_data, call_data);
    gdb_keyboard_command = true;
}

Widget make_buttons(Widget parent, const string& name, 
		    const string& button_list)
{
    if (button_list == "")
	return 0;

    Widget buttons = verify(XmCreateWorkArea(parent, name, 0, 0));
    if (buttons == 0)
    {
	// Not available in LessTif 0.1
	buttons = verify(XmCreateRowColumn(parent, name, 0, 0));
    }

    int colons = button_list.freq(':') + 1;
    string *commands = new string[colons];
    split(button_list, commands, colons, ':');

    for (int i = 0; i < colons; i++)
    {
	XtCallbackProc callback = gdbCommandCB;

	string name = commands[i];
	string command = name;
	if (name.contains("..."))
	{
	    name = name.before("...");
	    command = name + ' ';
	}
	else if (name.contains('^'))
	{
	    command = unctrl(name.from('^'));
	    name = name.before('^');
	}
	else if (name != "" && iscntrl(name[name.length() - 1]))
	{
	    command = string(name[name.length() - 1]);
	    name = name.before(-1);
	}

	Widget button = verify(XmCreatePushButton(buttons, name, 0, 0));
	XtManageChild(button);

	if (name == "Yes")
	{
	    command = "yes";
	    XtUnmanageChild(button);
	    callback = YnButtonCB;
	}
	else if (name == "No")
	{
	    command = "no";
	    XtUnmanageChild(button);
	    callback = YnButtonCB;
	}
	else if (name == "Prev")
	    callback = gdbPrevCB;
	else if (name == "Next")
	    callback = gdbNextCB;
	else if (name == "Clear")
	    callback = gdbClearCB;
	else if (name == "Complete")
	    callback = gdbCompleteCB;
	else if (name == "Apply")
	    callback = gdbApplyCB;
	else if (name == "Back")
	    callback = gdbGoBackCB;
	else if (name == "Forward")
	    callback = gdbGoForwardCB;
	else if (name == "Edit")
	    callback = gdbEditSourceCB;
	else if (name == "Reload")
	    callback = gdbReloadSourceCB;

	XtAddCallback(button, XmNactivateCallback, callback,
		      (XtPointer)XtNewString(command));
    }
    delete[] commands;
    DefaultHelpText = gdbDefaultHelp;

    XtManageChild(buttons);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(buttons, NULL, &size);
    XtVaSetValues(buttons,
		  XmNpaneMaximum, size.height,
		  XmNpaneMinimum, size.height,
		  NULL);

    return buttons;
}


//-----------------------------------------------------------------------------
// Sashes
//-----------------------------------------------------------------------------


// Destroy all sashes of PANED, except the one numbered IGNORE
void unmanage_sashes(Widget paned, int ignore)
{
    if (!XmIsPanedWindow(paned))
	return;

    WidgetList children;
    int num_children;

    XtVaGetValues(paned,
		  XtNchildren, &children,
		  XtNnumChildren, &num_children,
		  NULL);

    int n = 0;
    for (int i = 0; i < num_children; i++)
	if (XmIsSash(children[i]) && n++ != ignore)
	{
	    XtUnmanageChild(children[i]);
	    XtUnmapWidget(children[i]);
	}
}

// Disable traversal for all sashes of PANED
void untraverse_sashes(Widget paned)
{
    if (!XmIsPanedWindow(paned))
	return;

    WidgetList children;
    int num_children;

    XtVaGetValues(paned,
		  XtNchildren, &children,
		  XtNnumChildren, &num_children,
		  NULL);

    for (int i = 0; i < num_children; i++)
	if (XmIsSash(children[i]))
	    XtVaSetValues(children[i], XmNtraversalOn, False, NULL);
}

//-----------------------------------------------------------------------------
// Separate tty
//-----------------------------------------------------------------------------

// Create a separate tty window; return its name and process id
void launch_separate_tty(string& ttyname, pid_t& pid, string& term,
			 LiterateAgent*& tty_agent, 
			 Window& windowid, Widget origin)
{
    // If we're already running, all is done.
    if (pid > 0 && !remote_gdb() && kill(pid, 0) == 0)
	return;

    if (pid > 0)
	set_status("Restarting execution tty.");
    else
	set_status("Starting execution tty.");

    Delay delay;

    string old_ttyname = ttyname;

    string command = 
	
	// Set up a temporary file in TMP.
	"tmp=${TMPDIR-/tmp}/ddd$$; export tmp; "

	// Be sure to remove it when exiting...
	"trap \"rm -f $tmp\" 0; "

	// ... or being interrupted.
	"trap 'exit 1' 1 2 15; "

	// Now execute the xterm command
	+ string(app_data.term_command) +

	// which saves TTY, PID, TERM, and WINDOWID in TMP and goes to
	// sleep forever.  Signal 2 (SIGINT) is blocked for two
	// reasons: first, we dont want ^C to kill the tty window;
	// second, later invocations will send us SIGINT to find out
	// whether we're still alive.
	" '"
	"echo `tty` $$ $TERM $WINDOWID >$tmp; "
	"trap \"\" 2; "
	"while true; do sleep 3600; done"
	"' "

	// The whole thing is redirected and in the background such
	// that rsh won't wait for us.
	">/dev/null </dev/null 2>&1 & "

	// The main file waits for TMP to be created...
	"while test ! -s $tmp; do sleep 1; done; "

	// ...and sends TMP's contents to stdout, where DDD is waiting.
	"cat $tmp";

    if (pid > 0 && remote_gdb())
    {
	// We're already running.  Don't start a new tty
	// if the old one is still running.
	ostrstream os;
	os << "kill -2 " << pid << " 2>/dev/null"
	   << " || ( " << command << " )";
	command = string(os);
    }

    command = sh_command(command);

    Agent tty(command);
    tty.start();

    FILE *fp = tty.inputfp();
    if (fp != 0)
    {
	char reply[BUFSIZ];
	fgets(reply, BUFSIZ, fp);

	if (strlen(reply) > 2)
	{
	    istrstream is(reply);
	    is >> ttyname >> pid >> term >> windowid;
	}
    }

    // Sanity check
    if (ttyname == "" || ttyname[0] != '/')
	pid = -1;

    if (pid < 0)
	post_error("Could not start execution tty", "tty_exec_error", origin);

    if (pid < 0 || ttyname != old_ttyname)
    {
	// Close old tty stream
	if (tty_agent)
	{
	    delete tty_agent;
	    tty_agent = 0;
	}
    }

    if (pid > 0 && tty_agent == 0)
    {
	// Open new tty stream
	if (remote_gdb())
	{
	    string command = sh_command("cat -u > " + ttyname);

	    tty_agent = new LiterateAgent(app_context, command);
	    tty_agent->start();
	}
	else
	{
	    FILE *ttyfp = fopen(ttyname, "w");
	    tty_agent = new LiterateAgent(app_context, stdin, ttyfp, stderr);
	    tty_agent->start();
	}
    }

    // Set icon and group leader
    if (windowid)
    {
	wm_set_icon(XtDisplay(command_shell), windowid,
		    iconlogo(command_shell), iconmask(command_shell));
	wm_set_group_leader(XtDisplay(command_shell), windowid,
			    XtWindow(command_shell));
    }
}

void get_args(string command, string& base, string& args)
{
    // Find (last) arguments to `run' command
    base = command;
    args = "";

    if (command.contains(rxwhite))
    {
	base = command.before(rxwhite);
	args = command.after(rxwhite);
    }

    if (args == "" && gdb->type() == GDB)
    {
	args = gdb_question("show args");
	if (!args.contains("Arguments", 0))
	    args = "";
	else
	{
	    if (args.contains('"'))
	    {
		args = args.after('"');
		args = args.before('"', -1);
	    }
	}
    }
}


void gdb_set_tty(const string& tty_name,
		 const string& term_type,
		 Widget origin)
{
    if (gdb->type() == GDB)
    {
	if (tty_name != gdb_tty)
	{
	    // Issue `tty' command to perform redirection
	    string tty_cmd = string("tty ") + tty_name;
	    string reply = gdb_question(tty_cmd);

	    if (reply == string(-1))
	    {
		post_error("GDB I/O error: cannot send tty command", 
			   "tty_command_error", origin);
	    }
	    else if (reply != "")
	    {
		post_gdb_message(reply, origin);
	    }
	    else
		gdb_tty = tty_name;
	}

	// Set remote terminal type
	string env_cmd = string("set environment TERM ") + term_type;
	string reply = gdb_question(env_cmd);
	if (reply == string(-1))
	{
	    post_error("GDB I/O error: cannot send tty command", 
		       "tty_command_error", origin);
	}
	else if (reply != "")
	{
	    post_gdb_message(reply, origin);
	}
    }
    else if (gdb->type() == DBX && gdb->version() != DBX1)
    {
	if (tty_name != gdb_tty)
	{
	    // Issue `dbxenv run_io pty' and `dbxenv run_pty' commands
	    // to perform redirection
	    string command = string("dbxenv run_io pty");
	    string reply = gdb_question(command);

	    if (reply == string(-1))
	    {
		post_error("DBX I/O error: cannot send dbxenv run_io command",
			   "tty_command_error", origin);
	    }
	    else if (reply != "")
	    {
		post_gdb_message(reply, origin);
	    }
	    else
	    {
		command = string("dbxenv run_pty ") + tty_name;
		reply = gdb_question(command);

		if (reply == string(-1))
		{
		post_error("DBX I/O error: cannot send dbxenv run_pty command",
			   "tty_command_error", origin);
		}
		else if (reply != "")
		{
		    post_gdb_message(reply, origin);
		}
		else
		    gdb_tty = tty_name;
	    }
	}
	// Set remote terminal type
	string env_cmd = string("setenv TERM ") + term_type;
	string reply = gdb_question(env_cmd);
	if (reply == string(-1))
	{
	    post_error("GDB I/O error: cannot send tty command", 
		       "tty_command_error", origin);
	}
	else if (reply != "")
	{
	    post_gdb_message(reply, origin);
	}
    }
}

    

void redirect_process(string& command,
		      const string& tty_name,
		      Widget origin)
{
    if (app_data.use_tty_command
	&& (gdb->type() == GDB 
	    || (gdb->type() == DBX && gdb->version() != DBX1)))
    {
	// Issue `tty' command to perform redirection
	gdb_set_tty(tty_name, app_data.term_type, origin);
	return;
    }

    char *shell_s = getenv("SHELL");
    if (shell_s == 0)
	shell_s = "/bin/sh";
    string shell(shell_s);

    if (gdb->type() == GDB && remote_gdb())
    {
	// Make sure we use /bin/sh on a remote GDB
	shell = "/bin/sh";
	string reply = gdb_question("set environment SHELL " + shell);
    }

    // Append appropriate redirection directives
    string base;
    string args;
    get_args(command, base, args);

    gdb_redirection = "";
    if (!args.contains("<"))
	gdb_redirection = "< " + tty_name;

    if (!args.contains(">"))
    {
	switch (gdb->type())
	{
	case GDB:
	{
	    static regex RXcsh(".*csh$", true);
	    static regex RXrc(".*rc$", true);

	    if (shell.matches(RXcsh))
	    {
		// csh, tcsh
		gdb_redirection += " >&! " + tty_name;
	    }
	    else if (shell.matches(RXrc))
	    {
		// rc (from tim@pipex.net)
		gdb_redirection += " >" + tty_name + " >[2=1]";
	    }
	    else
	    {
		// sh, bsh, ksh, bash, zsh, sh5, ...
		gdb_redirection += " >" + tty_name + " 2>&1";
	    }
	}
	break;

	case DBX:
	    // DBX has its own parsing; it does not allow to redirect the
	    // error channel. *Sigh*.
	    gdb_redirection +=  " > " + tty_name;
	    if (gdb->version() != DBX1)
	    {
		// DBX 3.x uses ksh style redirection
		gdb_redirection += " 2>&1";
	    }
	    break;
	}
    }

    string new_args;
    if (gdb_redirection != "" && !args.contains(gdb_redirection))
    {
	if (args == "")
	    new_args = gdb_redirection;
	else
	    new_args = gdb_redirection + " " + args;
    }

    gdb_out_ignore = gdb_redirection;
    command = base + " " + new_args;
}

void unredirect_process(string& command,
			Widget origin)
{
    if (gdb_redirection != "")
    {
	// Disable output redirection upon next run
	string base;
	string args;
	get_args(command, base, args);
	if (args.contains(gdb_redirection) && gdb->type() == GDB)
	{
	    args = args.before(gdb_redirection);
	    strip_final_blanks(args);
	    string reply = gdb_question("set args " + args);
	    if (reply != "")
		post_gdb_message(reply, origin);
	}
    }

    // Issue `tty' command to perform redirection
    gdb_set_tty(gdb->slave_tty(), "dumb", origin);

    gdb_redirection = "";
    gdb_out_ignore = "";
}


inline void addcap(string& s, const char *cap, char *& b)
{
    const char *str = tgetstr(cap, &b);
    if (str)
	s += str;
}
    
void initialize_tty(LiterateAgent *tty_agent, const string& tty_term)
{
    char buffer[2048];
    string init;

    int success = tgetent(buffer, tty_term);
    if (success > 0)
    {
	char caps[2048];
	char *b = caps;

	addcap(init, "rs", b);	// Reset from strange mode
	addcap(init, "is", b);	// Ordinary reset
	addcap(init, "cl", b);	// Clear screen
    }

    if (init.length() > 0)
    {
	tty_agent->write(init, init.length());
	tty_agent->flush();
    }
}

void set_tty_title(string message, Window tty_window)
{
    string init = "";

    string title = string(DDD_NAME) + ": Execution Window";
    string icon  = title;

    message = message.after(": ");
    static string empty;
    if (gdb_out_ignore != "")
	message.gsub(gdb_out_ignore, empty);

    string program = message;
    if (program.contains(' '))
	program = program.before(' ');

    if (program != "")
    {
	string program_base = program;
	if (program_base.contains('/'))
	    program_base = program_base.after('/', -1);

	title = string(DDD_NAME) + ": " + message;
	icon  = string(DDD_NAME) + ": " + program_base;
    }

    if (tty_window)
	wm_set_name(XtDisplay(command_shell), tty_window,
		    title, icon);
}

void handle_running_commands(string& command, Widget origin)
{
    // Make sure we see control messages such as `Starting program'
    if (is_running_cmd(command, gdb->type()))
	show_next_line_in_status = true;

    if (is_run_cmd(command))
	startup_exec_tty(command, origin);
}

void startup_exec_tty()
{
    string dummy = "";
    startup_exec_tty(dummy);
}

void startup_exec_tty(string& command, Widget origin)
{
    if (app_data.separate_exec_window 
	&& separate_tty_pid >= 0
	&& gdb->isReadyWithPrompt())
    {
	// Launch separate tty if not running
	launch_separate_tty(separate_tty_name, 
			    separate_tty_pid,
			    separate_tty_term,
			    separate_tty_agent,
			    separate_tty_window,
			    origin);

	if (separate_tty_pid < 0)
	    return;

	// Initialize tty
	initialize_tty(separate_tty_agent, separate_tty_term);

	// Set title from `starting program...' message
	show_starting_line_in_tty = true;

	// Tell GDB to redirect process I/O to this tty
	redirect_process(command, separate_tty_name, origin);
    }
    else
    {
	// Close running tty
	kill_exec_tty();

	// Tell GDB not to redirect its process I/O
	unredirect_process(command, origin);
    }
}


void set_tty_from_gdb(const string& text)
{
    if (private_gdb_input)
	return;
    if (!show_starting_line_in_tty)
	return;
    if (!text.contains("Starting program") && !text.contains("Running:"))
	return;

    show_starting_line_in_tty = false;

    if (separate_tty_pid <= 0 || separate_tty_agent == 0)
	return;

    set_tty_title(text, separate_tty_window);
}

//-----------------------------------------------------------------------------
// GDB command management
//-----------------------------------------------------------------------------

void _gdb_command(string command, Widget origin)
{
    set_status("");

    if (command.length() == 1 && iscntrl(command[0]))
	promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);

    handle_running_commands(command, origin);
    handle_obscure_commands(command, origin);

    gdb_keyboard_command = private_gdb_input;
    gdb_last_origin = (gdb_keyboard_command ? gdb_w : origin);
    user_cmdSUC(command, origin);
    messagePosition = XmTextGetLastPosition(gdb_w);
}

void controlAct(Widget w, XEvent*, String *params, Cardinal *num_params)
{
    if (*num_params != 1)
    {
	cerr << "gdb-control: usage: gdb-control(CONTROL-CHARACTER)\n";
	return;
    }

    gdb_keyboard_command = true;
    _gdb_command(unctrl(params[0]), w);
}

void insert_source_argAct   (Widget w, XEvent*, String*, Cardinal*)
{
    string arg = source_arg->get_string();
    if (XmIsText(w)) {
	if (XmTextGetEditable) {
	    XmTextPosition pos = XmTextGetInsertionPosition(w);
	    XmTextReplace(w, pos, pos, (String)arg);
	}
    }
    else if (XmIsTextField(w)) {
	if (XmTextFieldGetEditable) {
	    XmTextPosition pos = XmTextFieldGetInsertionPosition(w);
	    XmTextFieldReplace(w, pos, pos, (String)arg);
	}
    }
}

void insert_graph_argAct (Widget w, XEvent*, String*, Cardinal*)
{
    string arg = DataDisp::graph_arg->get_string();
    if (XmIsText(w)) {
	if (XmTextGetEditable) {
	    XmTextPosition pos = XmTextGetInsertionPosition(w);
	    XmTextReplace(w, pos, pos, (String)arg);
	}
    }
    else if (XmIsTextField(w)) {
	if (XmTextFieldGetEditable) {
	    XmTextPosition pos = XmTextFieldGetInsertionPosition(w);
	    XmTextFieldReplace(w, pos, pos, (String)arg);
	}
    }
}

void next_tab_groupAct (Widget w, XEvent*, String*, Cardinal*)
{
    XmProcessTraversal(w, XmTRAVERSE_NEXT_TAB_GROUP);
}

void prev_tab_groupAct (Widget w, XEvent*, String*, Cardinal*)
{
    XmProcessTraversal(w, XmTRAVERSE_PREV_TAB_GROUP);
}

void get_focusAct (Widget w, XEvent*, String*, Cardinal*)
{
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
}


//-----------------------------------------------------------------------------
// Command history
//-----------------------------------------------------------------------------

string current_line()
{
    String str = XmTextGetString(gdb_w);
    string input(str + promptPosition, 
		 XmTextGetLastPosition(gdb_w) - promptPosition);
    XtFree(str);
    return input;
}

void set_line_from_history()
{
    private_gdb_history = true;

    const string& input = gdb_history[gdb_current_history];
    XmTextReplace(gdb_w, promptPosition,
		  XmTextGetLastPosition(gdb_w), (String)input);
    XmTextSetInsertionPosition(gdb_w, XmTextGetLastPosition(gdb_w));

    if (gdb_history_w)
    {
	int pos = gdb_current_history + 1;

	int top_item      = 0;
	int visible_items = 0;
	XtVaGetValues(gdb_commands_w,
		      XmNtopItemPosition, &top_item,
		      XmNvisibleItemCount, &visible_items,
		      NULL);

	XmListSelectPos(gdb_commands_w, pos, False);
	if (pos == 1)
	    XmListSetPos(gdb_commands_w, pos);
	else if (pos - 1 < top_item)
	    XmListSetPos(gdb_commands_w, pos - 1);
	else if (pos + 1 >= top_item + visible_items)
	    XmListSetBottomPos(gdb_commands_w, pos + 1);
    }

    private_gdb_history = false;
}

void set_history_from_line(const string& line)
{
    while (gdb_history.size() < 1)
	gdb_history += "";
    gdb_history[gdb_history.size() - 1] = line;

    if (gdb_history_w)
    {
	int pos = gdb_history.size();

	// XmListReplaceItemsPos() disturbs the current selection, so
	// save it here
	int *selected;
	int selected_count;
	if (!XmListGetSelectedPos(gdb_commands_w, &selected, &selected_count))
	    selected = 0;

	MString xm_line(line, LIST_CHARSET);
	XmString xms = xm_line.xmstring();
	XmListReplaceItemsPos(gdb_commands_w, &xms, 1, pos);

	if (selected)
	{
	    // Restore old selection
	    for (int i = 0; i < selected_count; i++)
		XmListSelectPos(gdb_commands_w, selected[i], False);
	    XtFree((char *)selected);
	}
    }
}

// Enter LINE in history
void add_to_history(const string& line)
{
    if (!gdb->isReadyWithPrompt())
	return;

    set_history_from_line(line);

    if (gdb_history.size() < 2 || line != gdb_history[gdb_history.size() - 2])
    {
	gdb_history += "";

	if (gdb_history_w)
	{
	    MString xm_line(line, LIST_CHARSET);
	    int pos = gdb_history.size();
	    XmListAddItem(gdb_commands_w, xm_line, pos - 1);
	    XmListSelectPos(gdb_commands_w, 0, False);
	    XmListSetBottomPos(gdb_commands_w, 0);
	}
    }

    gdb_current_history = gdb_history.size();
    set_history_from_line("");

    if (gdb_history_w)
    {
	XmListSelectPos(gdb_commands_w, 0, False);
	XmListSetBottomPos(gdb_commands_w, 0);
    }

    gdb_new_history = false;

    add_to_arguments(line);
    update_arguments();
}

// Load history from history file
static void load_history()
{
    if (gdb_history_file != "")
    {
	Delay d;

	ifstream is(gdb_history_file);
	if (is.bad())
	    return;

	static StringArray empty;
	gdb_history = empty;

	assert(gdb_history.size() == 0);

	while (is)
	{
	    char line[BUFSIZ];
	    line[0] = '\0';

	    is.getline(line, sizeof(line));
	    if (line[0] != '\0')
	    {
		gdb_history += line;
		add_to_arguments(line);
	    }
	}

	gdb_history += "";
	gdb_current_history = gdb_history.size() - 1;
	gdb_new_history = true;

	update_arguments();
    }
}

// Save history into history file
static void save_history(Widget origin)
{
    if (gdb_history_file != "")
    {
	StatusDelay delay("Saving history in " + quote(gdb_history_file));
	ofstream os(gdb_history_file);
	if (os.bad())
	{
	    post_error("Cannot save history in " + quote(gdb_history_file),
		       "history_save_error", origin);
	    return;
	}

	int start = gdb_history.size() - gdb_history_size;
	if (start < 0)
	    start = 0;

	for (int i = start; i < gdb_history.size(); i++)
	    os << gdb_history[i] << "\n";
    }
}

// Set history file name
void process_history_filename(string answer)
{
    answer = answer.after('"');
    answer = answer.before('"');
    gdb_history_file = answer;

    static bool history_initialized = false;
    if (!history_initialized)
    {
	history_initialized = true;
	load_history();
    }
}

// Set history size
void process_history_size(string answer)
{
    answer = answer.from(rxint);
    int ret = get_positive_nr(answer);
    if (ret >= 0)
	gdb_history_size = ret;
}

// Set history save
void process_history_save(string answer)
{
    // A `set save history on' in ~/.gdbinit causes the DDD history to
    // be saved (since otherwise, it would be overwritten by the GDB
    // history, which is cluttered with internal DDD commands).

    if (answer.contains("is on"))
    {
	app_data.save_history_on_exit = true;
	update_options();
    }
}

// History viewer
void SelectHistoryCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    gdb_current_history = cbs->item_position - 1;
    set_line_from_history();
}

void HistoryDestroyedCB(Widget, XtPointer client_data, XtPointer)
{
    Widget old_gdb_history_w = Widget(client_data);
    if (gdb_history_w == old_gdb_history_w)
    {
	gdb_history_w = 0;
	gdb_commands_w = 0;
    }
}

void gdbHistoryCB(Widget w, XtPointer, XtPointer)
{
    if (gdb_history_w)
    {
	XtManageChild(gdb_history_w);
	return;
    }

    Arg args[10];
    int arg;
	
    // Create history viewer
    arg = 0;
    gdb_history_w =
	verify(XmCreateSelectionDialog(w, "history_dialog", args, arg));
    Delay::register_shell(gdb_history_w);

    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_SELECTION_LABEL));
    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_CANCEL_BUTTON));

    gdb_commands_w = XmSelectionBoxGetChild(gdb_history_w, XmDIALOG_LIST);
    XtVaSetValues(gdb_commands_w,
		  XmNselectionPolicy, XmSINGLE_SELECT,
		  NULL);

    Widget apply_w = XmSelectionBoxGetChild(gdb_history_w, 
					    XmDIALOG_APPLY_BUTTON);
    XtVaSetValues(gdb_history_w,
		  XmNdefaultButton, apply_w,
		  NULL);

    XtAddCallback(gdb_commands_w,
		  XmNsingleSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNmultipleSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNextendedSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNbrowseSelectionCallback, SelectHistoryCB, 0);

    XtAddCallback(gdb_history_w, XmNokCallback, DestroyThisCB, gdb_history_w);
    XtAddCallback(gdb_history_w, XmNapplyCallback, gdbApplyCB, 0);
    XtAddCallback(gdb_history_w, XmNhelpCallback,  ImmediateHelpCB, 0);
    XtAddCallback(gdb_history_w, XtNdestroyCallback, 
		  HistoryDestroyedCB, XtPointer(gdb_history_w));

    bool *selected = new bool[gdb_history.size() + 1];
    for (int i = 0; i < gdb_history.size() + 1; i++)
	selected[i] = false;
    selected[gdb_current_history] = true;

    setLabelList(gdb_commands_w, gdb_history.values(), 
		 selected, gdb_history.size());

    delete[] selected;

    set_history_from_line(current_line());
    XmListSelectPos(gdb_commands_w, 0, False);
    XmListSetBottomPos(gdb_commands_w, 0);

    XtManageChild(gdb_history_w);
}


//-----------------------------------------------------------------------------
// Command queue
//-----------------------------------------------------------------------------

// Command queue
struct Command
{
    string command;		// Command text
    Widget origin;		// Origin

    Command(const string& cmd, Widget w = 0)
	: command(cmd), origin(w)
    {}
    Command(const Command& c)
	: command(c.command), origin(c.origin)
    {}
    Command& operator = (const Command& c)
    {
	if (this != &c)
	{
	    command = c.command;
	    origin = c.origin;
	}
	return *this;
    }
    bool operator == (const Command& c)
    {
	return this == &c || command == c.command && origin == c.origin;
    }
};

Queue<Command> commandQueue;

void clearCommandQueue()
{
    while (!commandQueue.isEmpty())
	commandQueue -= commandQueue.first();
}

void gdb_command(const string& cmd, Widget origin)
{
    if (cmd.length() == 1 && iscntrl(cmd[0]) || cmd == "yes" ||	cmd == "no")
    {
	_gdb_command(cmd, origin);
	clearCommandQueue();
	return;
    }

    if (gdb->isReadyWithPrompt() && commandQueue.isEmpty())
    {
	add_to_history(cmd);
	_gdb_command(cmd, origin);
    }
    else
	commandQueue += Command(cmd, origin);
}

void processCommandQueue(XtPointer, XtIntervalId *)
{
    if (!gdb->isReadyWithPrompt())
    {
	// Try again later...
	XtAppAddTimeOut(app_context, 200, processCommandQueue, XtPointer(0));
    }

    if (!commandQueue.isEmpty())
    {
	Command& c = commandQueue.first();
	Command cmd(c);
	commandQueue -= c;

	add_to_history(cmd.command);
	_gdb_command(cmd.command, cmd.origin);
	gdb_keyboard_command = false;
    }
}


//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

void gdbCommandCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    string command = (String)client_data;
    if (command.contains("..."))
    {
	command = command.before("...") + " ";
	String args[1];
	args[0] = command;
	gdbClearCB(w, client_data, call_data);
	XtCallActionProc(gdb_w, "insert-string", cbs->event, args, 1);
    }
    else
    {
	gdb_command(command, w);
    }
}

void gdbBreakArgCmdCB(Widget w, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();
    string pos;

    switch (gdb->type())
    {
    case GDB:
	if (arg != "" && arg[0] == '0')
	    arg = "*" + arg; // Address given
	gdb_command("break " + arg, w);
	break;

    case DBX:
	if (arg.matches(rxint))
	{
	    // Line number given
	    gdb_command("stop at " + arg, w);
	}
	else if (arg.contains(":") && !arg.contains("::"))
	{
	    // Function:Line given
	    pos = arg;
	}
	else
	{
	    // Function name given
	    pos = dbx_lookup(arg);
	}

	if (pos != "")
	{
	    gdb_command("file " + pos.before(":"), w);
	    gdb_command("stop at " + pos.after(":"), w);
	}
	break;
    }
}

void gdbClearArgCmdCB(Widget w, XtPointer, XtPointer)
{
    string pos;
    string arg = source_arg->get_string();
    switch (gdb->type())
    {
    case GDB:
	if (arg != "" && arg[0] == '0')
	    arg = "*" + arg; // Address given
	gdb_command("clear " + arg);
	break;

    case DBX:
	if (arg.matches(rxint))
	{
	    // Line number given
	    gdb_command("clear " + arg);
	}
	else if (arg.contains(":") && !arg.contains("::"))
	{
	    // Function:Line given
	    pos = arg;
	}
	else
	{
	    // Function name given
	    pos = dbx_lookup(arg);
	}

	if (pos != "")
	{
	    gdb_command("file " + pos.before(":"), w);
	    gdb_command("clear " + pos.after(":"), w);
	}
	break;
    }
}

void gdbLineArgCmdCB(Widget w, XtPointer client_data, XtPointer)
{
    string cmd = (String)client_data;
    string arg = source_arg->get_string();

    gdb_command(cmd + arg, w);
}

void gdbPrintArgCmdCB(Widget w, XtPointer, XtPointer)
{
    string cmd = gdb->print_command();
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command(cmd + " " + arg, w);
}

void gdbDisplayArgCmdCB(Widget w, XtPointer, XtPointer)
{
    string cmd = "graph display";
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command(cmd + " " + arg, w);
}

void gdbLookupCB(Widget, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();
    source_view->lookup(arg);
}

void gdbFindForwardCB(Widget, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = 
	(XmPushButtonCallbackStruct *)call_data;

    string s = source_arg->get_string();
    source_view->find(s, SourceView::forward, 
		      app_data.find_words_only, time(cbs->event));
}

void gdbFindBackwardCB(Widget, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = 
	(XmPushButtonCallbackStruct *)call_data;

    string s = source_arg->get_string();
    source_view->find(s, SourceView::backward, 
		      app_data.find_words_only, time(cbs->event));
}

static void gdbDeleteEditAgent(XtPointer client_data, XtIntervalId *)
{
    // Delete agent after use
    Agent *edit_agent = (Agent *)client_data;
    delete edit_agent;
}

static void gdbEditDoneHP(Agent *edit_agent, void *client_data, void *)
{
    // Editor has terminated: reload current source file
    string *pfile = (string *)client_data;
    // post_gdb_message("Editing of " + quote(*pfile) + " done.");
    delete pfile;

    XtAppAddTimeOut(app_context, 0, gdbDeleteEditAgent, 
		    XtPointer(edit_agent));
    gdbReloadSourceCB(gdb_w, 0, 0);
}

void gdbEditSourceCB  (Widget, XtPointer, XtPointer)
{
    string pos = source_view->line_of_cursor(false);
    string file = pos.before(':');
    string line = pos.after(':');

    StatusDelay delay("Invoking editor for " + quote(file));

    string cmd = app_data.edit_command;
    cmd.gsub("@FILE@", file);
    cmd.gsub("@LINE@", line);
    cmd = sh_command(cmd);

    // Invoke an editor in the background
    LiterateAgent *edit_agent = new LiterateAgent(app_context, cmd);
    edit_agent->removeAllHandlers(Died);
    edit_agent->addHandler(InputEOF, gdbEditDoneHP, (void *)new string(file));
    edit_agent->start();
}

void gdbReloadSourceCB  (Widget, XtPointer, XtPointer)
{
    source_view->reload();
}

void gdbGoBackCB  (Widget, XtPointer, XtPointer)
{
    source_view->go_back();
}

void gdbGoForwardCB  (Widget, XtPointer, XtPointer)
{
    source_view->go_forward();
}

void gdbPrevCB  (Widget w, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Cardinal zero = 0;
    prev_historyAct(w, cbs->event, 0, &zero);
}

void gdbNextCB  (Widget w, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Cardinal zero = 0;
    next_historyAct(w, cbs->event, 0, &zero);
}

void gdbClearCB  (Widget w, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    String args[1] = {""};
    Cardinal num_args = 1;
    set_lineAct(w, cbs->event, args, &num_args);
}

void gdbCompleteCB  (Widget w, XtPointer, XtPointer call_data)
{
    if (!gdb->isReadyWithPrompt())
    {
	post_gdb_busy(w);
	return;
    }

    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    Cardinal zero = 0;
    end_of_lineAct(gdb_w, cbs->event, 0, &zero);
    complete_commandAct(gdb_w, cbs->event, 0, &zero);
}

void gdbApplyCB(Widget w, XtPointer, XtPointer call_data)
{
    if (!gdb->isReadyWithPrompt())
    {
	post_gdb_busy(w);
	return;
    }

    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    Cardinal zero = 0;
    end_of_lineAct(gdb_w, cbs->event, 0, &zero);
    XtCallActionProc(gdb_w, "process-return", cbs->event, 0, zero);
}

void gdbCutSelectionCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    DDDWindow win = DDDWindow(client_data);
    switch (win)
    {
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

    Boolean b = (separate_tty_pid > 0);
    set_sensitive(command_view_menu[ExecWindow].widget, b);
    set_sensitive(source_view_menu[ExecWindow].widget,  b);
    set_sensitive(data_view_menu[ExecWindow].widget,    b);
}




//-----------------------------------------------------------------------------
// Run and Argument Dialog
//-----------------------------------------------------------------------------

static bool arguments_updated = false;
static string last_arguments;

// Update list of arguments
void update_arguments()
{
    if (arguments_updated || run_dialog == 0)
	return;

    bool *selected = new bool[gdb_arguments.size()];
    int pos = -1;
    for (int i = 0; i < gdb_arguments.size(); i++)
    {
	if (gdb_arguments[i] == last_arguments)
	    pos = i;
	selected[i] = false;
    }
    if (pos >= 0)
	selected[pos] = true;

    setLabelList(gdb_arguments_w, gdb_arguments.values(),
		 selected, gdb_arguments.size());

    if (pos >= 0)
	XmListSelectPos(gdb_arguments_w, pos + 1, False);

    delete[] selected;

    Widget text_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, last_arguments);

    arguments_updated = true;
}

// Add ARG to the list of arguments
static void add_argument(string arg)
{
    strip_final_blanks(arg);
    while (arg.length() > 0 && isspace(arg[0]))
	arg = arg.after(0);

    last_arguments = arg;

    // Insertion sort
    int i;
    for (i = 0; i < gdb_arguments.size(); i++)
    {
	int cmp = compare(gdb_arguments[i], arg);
	if (cmp == 0)
	    return;		// Already present
	if (cmp > 0)
	    break;
    }

    gdb_arguments += "<dummy>";

    for (int j = gdb_arguments.size() - 1; j > i; j--)
	gdb_arguments[j] = gdb_arguments[j - 1];
    gdb_arguments[i] = arg;

    arguments_updated = false;
}

// If LINE is an argument-setting command, add it to the list of arguments
void add_to_arguments(string line)
{
    if (is_set_args_cmd(line))
    {
	string args = line.after("args");
	args = args.after(rxwhite);
	add_argument(args);
    }
    else if (is_run_cmd(line))
    {
	string args = line.after(rxwhite);
	add_argument(args);
    }
}

// Run program with given arguments
void gdbRunDCB(Widget, XtPointer, XtPointer)
{
    Widget text = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    String _args = XmTextGetString(text);
    string args(_args);
    XtFree(_args);

    if (args != "")
	args = " " + args;

    if (gdb->type() == GDB)
    {
	if (args == "")
	    gdb_command("set args", run_dialog);
	gdb_command("run" + args, run_dialog);
    }
    else if (gdb->type() == DBX)
    {
	gdb_command("rerun" + args, run_dialog);
    }
}

// Set program arguments from list
void SelectArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position - 1;
    const string& args = gdb_arguments[pos];
    
    Widget text_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, args);
}

// Create `Run' dialog
void gdbRunCB(Widget w, XtPointer, XtPointer)
{
    if (run_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	run_dialog = 
	    verify(XmCreateSelectionDialog(w, "run_dialog", args, arg));

	Delay::register_shell(run_dialog);
	XtAddCallback(run_dialog, XmNokCallback, gdbRunDCB, 0);
	XtAddCallback(run_dialog, XmNapplyCallback, gdbRunDCB, 0);
	XtAddCallback(run_dialog, XmNhelpCallback,  ImmediateHelpCB, 0);

#if 0
	Widget apply_w = XmSelectionBoxGetChild(run_dialog, 
						XmDIALOG_APPLY_BUTTON);
	XtVaSetValues(run_dialog,
		      XmNdefaultButton, apply_w,
		      NULL);
#endif

	gdb_arguments_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_LIST);
	XtAddCallback(gdb_arguments_w, XmNsingleSelectionCallback,
		      SelectArgsCB, 0);
	XtAddCallback(gdb_arguments_w, XmNmultipleSelectionCallback,
		      SelectArgsCB, 0);
	XtAddCallback(gdb_arguments_w, XmNextendedSelectionCallback,
		      SelectArgsCB, 0);
	XtAddCallback(gdb_arguments_w, XmNbrowseSelectionCallback,
		      SelectArgsCB, 0);
    }

    update_arguments();
    XtManageChild(run_dialog);
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

	    // Kill initialization file
	    remove_init_file();

	    // Load the default history (for debuggers that don't
	    // provide history); the ``real'' history is read as reply
	    // to the ``show history filename'' command.
	    load_history();
	}

	if (!commandQueue.isEmpty())
	    XtAppAddTimeOut(app_context, 0, processCommandQueue, XtPointer(0));

	if (completion_delay)
	    delete completion_delay;
	completion_delay = 0;
    }

    set_sensitive(stack_w,    gdb_ready);
    set_sensitive(register_w, gdb_ready && gdb->type() == GDB);
}

void gdb_ready_for_cmdHP (void *, void *, void *)
{
    // Nothing yet...
}


//-----------------------------------------------------------------------------
// Synchronized questions
//-----------------------------------------------------------------------------

struct GDBReply {
    string answer;    // The answer text (string(-1) if timeout)
    bool received;    // true iff we found an answer
};

static void gdb_reply_timeout(XtPointer client_data, XtIntervalId *)
{
    GDBReply *reply = (GDBReply *)client_data;
    reply->answer   = string(-1);
    reply->received = true;
}

static void gdb_reply(const string& complete_answer, void *qu_data)
{
    GDBReply *reply = (GDBReply *)qu_data;
    reply->answer   = complete_answer;
    reply->received = true;
}

string gdb_question(const string& command, int timeout)
{
    if (gdb_question_running)
	return string(-1);
    gdb_question_running = true;

    static GDBReply reply;
    reply.received = false;

    bool ok = gdb->send_question(command, gdb_reply, (void *)&reply);
    if (!ok)
	return string(-1);	// GDB not ready

    if (timeout == 0)
	timeout = app_data.question_timeout;

    XtIntervalId timer = 0;
    if (timeout > 0)
    {
	timer = XtAppAddTimeOut(app_context, timeout * 1000,
				gdb_reply_timeout, (void *)&reply);
    }

    while (!reply.received && gdb->running())
	XtAppProcessEvent(app_context, XtIMTimer | XtIMAlternateInput);

    if (reply.answer != string(-1))
    {
	if (timer && timeout > 0)
	    XtRemoveTimeOut(timer);
    }

    gdb_question_running = false;

    return reply.answer;
}

// ***************************************************************************
// Lookup location of FUNC using DBX

static Assoc<string, string> pos_cache;

string dbx_lookup(const string& func_name)
{
    if (pos_cache.has(func_name))
	return pos_cache[func_name];

    string reply = gdb_question("func " + func_name);
    if (reply == string(-1))
    {
	post_gdb_busy();
	return "";
    }
    if (reply != "")
    {
	post_gdb_message(reply);
	return "";
    }

    string file    = gdb_question("file");
    strip_final_blanks(file);
    string listing = gdb_question("list");

    // DBX lists 10 lines; the current line is the 5th one.
    string pos = file + ":" + itostring(atoi(listing) + 5);

    pos_cache[func_name] = pos;
    return pos;
}

void clear_dbx_lookup_cache()
{
    static Assoc<string, string> empty;
    pos_cache = empty;
}

//-----------------------------------------------------------------------------
// Default help texts (especially buttons)
//-----------------------------------------------------------------------------

MString gdbDefaultHelp(Widget widget)
{
    string name = XtName(widget);

    string help = gdb_question("help " + name);
    if (help == string(-1))
    {
	help = "No help available now.\n"
	    "Please try again when the debugger is ready.";
    }
    strip_final_blanks(help);

    return MString(name + "\n", "bf") +	MString(help, "rm");
}

//-----------------------------------------------------------------------------
// Editing commands
//-----------------------------------------------------------------------------

void prev_historyAct(Widget, XEvent*, String*, Cardinal*)
{
    if (gdb_current_history == 0)
	return;

    while (gdb_current_history >= gdb_history.size())
	gdb_current_history--;
    gdb_current_history--;
    set_line_from_history();
}

void next_historyAct(Widget, XEvent*, String*, Cardinal*)
{
    if (gdb_current_history >= gdb_history.size() - 1)
	return;

    gdb_current_history++;
    set_line_from_history();
}

void beginning_of_lineAct(Widget, XEvent*, String*, Cardinal*)
{
    XmTextSetInsertionPosition(gdb_w, promptPosition);
}

void end_of_lineAct(Widget, XEvent*, String*, Cardinal*)
{
    XmTextSetInsertionPosition(gdb_w, XmTextGetLastPosition(gdb_w));
}

void forward_characterAct(Widget w, XEvent *e, 
			  String *args, Cardinal *num_args)
{
    XtCallActionProc(w, "forward-character", e, args, *num_args);
}

void backward_characterAct(Widget, XEvent*, String*, Cardinal*)
{
    XmTextPosition pos = XmTextGetInsertionPosition(gdb_w);
    if (pos > promptPosition)
	XmTextSetInsertionPosition(gdb_w, pos - 1);
}

void set_lineAct(Widget, XEvent*, String* params, Cardinal* num_params)
{
    string input = "";
    if (num_params && *num_params > 0)
	input = params[0];
    XmTextReplace(gdb_w, promptPosition, 
		  XmTextGetLastPosition(gdb_w), (String)input);
}

void delete_or_controlAct(Widget w, XEvent *e, 
			  String *args, Cardinal *num_args)
{
    string input = current_line();
    strip_final_newlines(input);
    if (input == "")
	XtCallActionProc(w, "gdb-control", e, args, *num_args);
    else
	XtCallActionProc(w, "delete-next-character", e, args, *num_args);
}


//-----------------------------------------------------------------------------
// Line Completion
//-----------------------------------------------------------------------------

// Sort A
static void sort(string a[], int size)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= size);
    do {
	h /= 3;
	for (int i = h; i < size; i++)
	{
	    string v = a[i];
	    int j;
	    for (j = i; j >= h && a[j - h] > v; j -= h)
		a[j] = a[j - h];
	    if (i != j)
		a[j] = v;
	}
    } while (h != 1);
}

// Remove adjacent duplicates in A
static void uniq(string a[], int& size)
{
    int j = 1;
    for (int i = 1; i < size; i++)
	if (a[i] != a[j - 1])
	    a[j++] = a[i];
    
    size = j;
}

// Info passed to reply functions
struct CompletionInfo {
    Widget widget;		// Widget
    XEvent *event;		// Event 
    string input;		// Current input
};

void complete_reply(const string& complete_answer, void *qu_data);

// Set completion
void set_completion(const CompletionInfo& info, string completion)
{
    // Set input to common prefix
    if (info.widget == gdb_w)
    {
	private_gdb_output = true;

	XmTextReplace(gdb_w, promptPosition,
		      XmTextGetLastPosition(gdb_w), 
		      (String)completion);

	private_gdb_output = false;
    }
    else
    {
	if (XmIsTextField(info.widget))
	{
	    XmTextFieldSetString(info.widget, (String)completion);
	}
	else if (XmIsText(info.widget))
	{
	    XmTextSetString(info.widget, (String)completion);
	}
    }
}

// Same, but for single possible completion
string complete_single_completion(string completion)
{
    // Only one possible expansion: Add final single quote if
    // necessary and add final space as well.

    if (completion.freq('\'') % 2 != 0)
	completion += '\'';
    completion += ' ';

    return completion;
}


// All completions are done
void completion_done(const CompletionInfo& info)
{
    if (XmIsTextField(info.widget))
    {
	XmTextPosition last_pos = 
	    XmTextFieldGetLastPosition(info.widget);
	XmTextFieldSetInsertionPosition(info.widget, last_pos);
	XmTextFieldShowPosition(info.widget, last_pos);
	XmTextFieldSetEditable(info.widget, true);
    }
    else if (XmIsText(info.widget))
    {
	XmTextPosition last_pos = 
	    XmTextGetLastPosition(info.widget);
	XmTextSetInsertionPosition(info.widget, last_pos);
	XmTextShowPosition(info.widget, last_pos);
	XmTextSetEditable(info.widget, true);
    }

    XmTextSetEditable(gdb_w, true);

    if (completion_delay)
	delete completion_delay;
    completion_delay = 0;
}

static string *completions  = 0;
static int completions_size = 0;

// Send completion question
void complete(Widget w, XEvent *e, string input, string cmd)
{
    static CompletionInfo info;
    info.widget = w;
    info.event  = e;
    info.input  = input;

    // Compare with last completions
    static Widget last_completion_w     = 0;
    static int    last_completion_index = -1;
    static string last_completion       = string(-1);
    
    if (completions_size > 0 
	&& completions
	&& completions[0] != ""
	&& w == last_completion_w)
    {
	// Check if this is to be the next completion
	string next_completion = "";

	if (input == last_completion)
	{
	    // We have already shown possible completions:
	    // Expand to first completion
	    last_completion_index = 0;
	    next_completion = complete_single_completion(completions[0]);
	}
	else if (completions_size > 1
		 && last_completion_index >= 0 
		 && input == complete_single_completion(
		     completions[last_completion_index]))
	{
	    if (last_completion_index < completions_size - 1)
	    {
		// Show next completion
		next_completion = complete_single_completion(
		    completions[++last_completion_index]);
	    }
	    else
	    {
		// All completions shown; re-start with initial input
		last_completion_index = -1;
		next_completion = last_completion;
	    }
	}

	if (next_completion != "")
	{
	    set_completion(info, next_completion);
	    completion_done(info);
	    return;
	}
    }

    // Start a new completion session

    // Clear old completions
    last_completion_w     = w;
    last_completion       = input;
    last_completion_index = -1;

    if (completions)
    {
	delete[] completions;
	completions = 0;
    }
    completions_size = 0;

    // Issue diagnostic if completion doesn't work right now
    if (!gdb->isReadyWithPrompt())
    {
	post_gdb_busy(w);
	return;
    }

    // Go and ask GDB for completions.
    string complete_cmd = "complete " + cmd;

    if (XmIsTextField(w))
	XmTextFieldSetEditable(w, false);
    else if (XmIsText(w))
	XmTextSetEditable(w, false);
    XmTextSetEditable(gdb_w, false);
    
    gdb->send_question(complete_cmd, complete_reply, (void *)&info);

    completion_delay = new Delay;
}


// Handle possible completions
void complete_reply(const string& complete_answer, void *qu_data)
{
    const CompletionInfo& info = *((CompletionInfo *)qu_data);

    bool from_gdb_w = (info.widget == gdb_w);

    string input = info.input;
    if (input != "" && isspace(input[0]))
	input = input.after(rxwhite);

    assert(completions == 0);

    int lines = complete_answer.freq('\n') + 1;
    completions      = new string[lines];
    completions_size = split(complete_answer, completions, lines, '\n');
    sort(completions, completions_size);
    uniq(completions, completions_size);

    if (!from_gdb_w)
    {
	// Strip initial `break' command
	for (int i = 0; i < completions_size; i++)
	    completions[i] = completions[i].after(' ');
    }

    if (completions_size == 0 || completions[0] == "")
    {
	// No completion (sigh)
	XtCallActionProc(gdb_w, "beep", info.event, 0, 0);
    }
    else if (completions[0].index("Undefined command:") == 0)
    {
	// GDB versions earlier than 4.13 do not support
	// the `complete' command.

	string message;
	for (int i = 0; i < completions_size; i++)
	    message += completions[i] + '\n';
	post_error(message, "no_completion_error", info.widget);
    }
    else
    {
	// Find common prefix
	string common_pfx = completions[0];
	int i;
	for (i = 1; i < completions_size; i++)
	    common_pfx = common_prefix(common_pfx, completions[i]);

	if (completions_size > 1 && input == common_pfx)
	{
	    // We're already as far as we can get
	    if (from_gdb_w)
	    {
		// Show possible expansions in command window.
		int skip = common_pfx.index(rxwhite, -1) + 1;
	    
		string insertion;
		if (from_gdb_w)
		    insertion += input;
		insertion += "\n";
		for (i = 0; i < completions_size; i++)
		{
		    insertion += completions[i].from(skip);
		    insertion += "\n";
		}
		gdb_out(insertion);
		gdb_out(gdb->default_prompt());

		XmTextSetInsertionPosition(gdb_w, 
					   XmTextGetLastPosition(gdb_w));
	    }
	}
	else
	{
	    string completion = common_pfx;
	    if (completions_size == 1)
		completion = complete_single_completion(completion);
	    set_completion(info, completion);
	}
    }

    completion_done(info);
}

// Complete current GDB command
void complete_commandAct(Widget w, XEvent *e, String* args, Cardinal* num_args)
{
    if (gdb->type() != GDB
	|| w != gdb_w
	|| XmTextGetInsertionPosition(w) != XmTextGetLastPosition(w))
    {
	if (XmIsText(w))
	    XtCallActionProc(w, "process-tab", e, args, *num_args);
	else if (XmIsPrimitive(w))
	    XtCallActionProc(w, "PrimitiveNextTabGroup", e, args, *num_args);
	return;
    }

    // Insert single quote if necessary
    string input = current_line();
    if (is_break_cmd(input))
    {
	int last_space = input.index(rxwhite, -1);
	if (last_space >= 0)
	{
	    string last_word = input.after(last_space);
	    if (last_word.length() > 0 && last_word[0] != '\'')
	    {
		for (int i = 0; i < int(last_word.length()); i++)
		{
		    char c = last_word[i];
		    if (!isalnum(c) && c != '_')
		    {
			input(last_space + 1, 0) = '\'';
			break;
		    }
		}
	    }
	}
    }

    complete(w, e, input, input);
}


// Complete GDB argument
static void _complete_argAct(Widget w, 
			     XEvent *e, 
			     String* args, 
			     Cardinal* num_args,
			     bool tab)
{
    if ((tab && !app_data.global_tab_completion) || gdb->type() != GDB)
    {
	if (XmIsText(w))
	    XtCallActionProc(w, "process-tab", e, args, *num_args);
	else if (XmIsPrimitive(w))
	    XtCallActionProc(w, "PrimitiveNextTabGroup", e, args, *num_args);
	return;
    }

    // The command to use as prefix for completions
    string base = "print";
    if (*num_args >= 1)
	base = args[0];

    // Insert single quote if necessary
    String _input = 0;
    if (XmIsTextField(w))
	_input = XmTextFieldGetString(w);
    else if (XmIsText(w))
	_input = XmTextGetString(w);

    if (_input == 0)
	return;

    string input(_input);
    XtFree(_input);

    if (is_break_cmd(base))
    {
	if (input.length() > 0 && input[0] != '\'')
	{
	    for (int i = 0; i < int(input.length()); i++)
	    {
		char c = input[i];
		if (!isalnum(c) && c != '_')
		{
		    input(0, 0) = '\'';
		    break;
		}
	    }
	}
    }

    complete(w, e, input, base + " " + input);
}

void complete_argAct(Widget w, XEvent *e, String* args, Cardinal* num_args)
{
    _complete_argAct(w, e, args, num_args, false);
}

void complete_tabAct(Widget w, XEvent *e, String* args, Cardinal* num_args)
{
    _complete_argAct(w, e, args, num_args, true);
}

//-----------------------------------------------------------------------------
// Text Callbacks
//-----------------------------------------------------------------------------

void move_to_end_of_line(XtPointer, XtIntervalId *)
{
    XmTextPosition lastPos = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, lastPos);
    XmTextShowPosition(gdb_w, lastPos);
}

// Veto changes before the current input line
void gdbModifyCB(Widget gdb_w, XtPointer, XtPointer call_data)
{
    if (private_gdb_output)
	return;

    XmTextVerifyCallbackStruct *change = 
	(XmTextVerifyCallbackStruct *)call_data;

    if (change->startPos < promptPosition && change->text->length == 0)
    {
	// Attempt to delete text before prompt
#ifdef LESSTIF_VERSION
	// This works with LessTif only;
	// using Motif, it causes a core dump on Solaris.  - AZ
	change->doit = false;
#else
	// Make it a no-op
	change->startPos = change->endPos = change->newInsert = 
	    change->currInsert;
#endif
	return;
    }

    // Make sure newlines are always inserted at the end of the line
    if (change->startPos == change->endPos &&
	(change->startPos < promptPosition || 
	 (change->text->length == 1 && change->text->ptr[0] == '\n')))
    {
	// Add any text at end of text window
	XmTextPosition lastPos = XmTextGetLastPosition(gdb_w);
	change->newInsert = change->startPos = change->endPos = lastPos;
	
	XtAppAddTimeOut(app_context, 0, move_to_end_of_line, XtPointer(0));
    }
}

// Veto key-based cursor movements before current line
void gdbMotionCB(Widget, XtPointer, XtPointer call_data)
{
    if (private_gdb_output)
	return;

    XmTextVerifyCallbackStruct *change = 
	(XmTextVerifyCallbackStruct *)call_data;

    if (change->newInsert >= promptPosition)
	return;

    // We are before the current prompt: don't change the cursor
    // position if a key was pressed.
    if (change->event != NULL && 
	(change->event->type == KeyPress || change->event->type == KeyRelease))
    {
#ifdef LESSTIF_VERSION
	// This works with LessTif only;
	// using Motif, it causes a core dump on Solaris.  - AZ
	change->doit = false;
#else
	// Make it a no-op.
	change->newInsert = change->currInsert;
#endif
    }
}

// Send completed lines to GDB
void gdbChangeCB(Widget w, XtPointer, XtPointer)
{
    if (private_gdb_output)
	return;

    if (gdb->isReadyWithPrompt())
	gdb_input_at_prompt = true;

    string input = current_line();
	
    int newlines = input.freq('\n');
    string *lines = new string[newlines + 1];
    split(input, lines, newlines, '\n');

    private_gdb_input = true;

    if (newlines == 0)
    {
	if (!private_gdb_history)
	    set_history_from_line(input);
    }
    else
    {
	// Process entered lines
	promptPosition = XmTextGetLastPosition(w);
	for (int i = 0; i < newlines; i++)
	{
	    string cmd = lines[i];
	    tty_out(cmd + "\n");

	    if (gdb_input_at_prompt)
	    {
		if (cmd.matches(rxwhite) || cmd == "")
		{
		    // Empty line: repeat last command
		    if (gdb_history.size() >= 2 && !gdb_new_history)
		    {
			cmd = gdb_history[gdb_history.size() - 2];
		    }
		    else
		    {
			// No history yet -- perform a no-op command
			cmd = "echo";
		    }
		}
		else
		{
		    // Add new command to history
		    add_to_history(cmd);
		}
	    }

	    if (gdb_input_at_prompt && !gdb->isReadyWithPrompt())
	    {
		// GDB is busy and the command last typed at the GDB
		// prompt did not cause any output yet (e.g. a new
		// prompt or diagnostic message).  Since sending CMD
		// directly may interfere with internal communication,
		// place CMD in the command queue instead.

		gdb_command(cmd, w);
	    }
	    else
	    {
		// Process anything else right now, clearing the
		// command queue.

		clearCommandQueue();
		_gdb_command(cmd, w);
	    }
	}
    }

    private_gdb_input = false;

    delete[] lines;
}


//-----------------------------------------------------------------------------
// Prompt recognition
//-----------------------------------------------------------------------------

void set_buttons_from_gdb(string& text)
{
    set_buttons_from_gdb(command_buttons_w, text);
    set_buttons_from_gdb(source_buttons_w, text);
}

void set_buttons_from_gdb(Widget buttons, string& text)
{
    if (buttons == 0)
	return;

    static regex rxyn("(y[es]* or n[o]*) *$", true);

    int yn_index = text.index(rxyn);
    bool yn = (yn_index >= 0);

    if (yn && !gdb_keyboard_command)
    {
	// Fetch previous output lines, in case this is a multi-line message.
	String s = XmTextGetString(gdb_w);
	string prompt(s);
	XtFree(s);
	int pos = prompt.index('(', -1);
	if (pos >= 0)
	    pos = prompt.index('\n', pos) + 1;
	if (pos == 0)
	    pos = messagePosition;

	XmTextReplace(gdb_w, pos, XmTextGetLastPosition(gdb_w), "");

	prompt = prompt.from(pos) + text.before(rxyn);
	post_gdb_yn(prompt);
	text = "";
	return;
    }

    static bool last_yn = false;
    if (yn == last_yn)
	return;

    last_yn = yn;

    XtSetSensitive(buttons, false);

    WidgetList children;
    Cardinal num_children;

    XtVaGetValues(buttons,
		  XmNchildren, &children,
		  XmNnumChildren, &num_children,
		  NULL);

    int i;
    for (i = 0; i < int(num_children); i++)
	XtManageChild(children[i]);
    for (i = 0; i < int(num_children); i++)
    {
	
	Widget w = children[i];
	string name = XtName(w);

	if (yn == (name == "Yes" || name == "No"))
	    XtManageChild(w);
	else
	    XtUnmanageChild(w);
    }

    XtSetSensitive(buttons, true);
}

void SelectCB(Widget dialog, XtPointer, XtPointer)
{
    int *numbers = getDisplayNumbers(gdb_selection_list_w);
    string choice = itostring(numbers[0]);
    delete[] numbers;

    // clog << quote(choice) << "\n";

    _gdb_command(choice, dialog);
}

void set_selection_from_gdb(string& text)
{
    static regex rxselect("\\(\n\\|.\\)*\n> ", true);

    if (gdb_keyboard_command || !text.matches(rxselect))
	return;

    // Fetch previous output lines, in case this is a multi-line message.
    String s = XmTextGetString(gdb_w);
    string prompt(s);
    XtFree(s);

    int last_gt = text.index("\n>", -1);
    prompt = prompt.from(int(messagePosition)) + text.before(last_gt);

    XmTextReplace(gdb_w, messagePosition, 
		  XmTextGetLastPosition(gdb_w), "");
    text = "";

    if (gdb_selection_dialog == 0)
    {
	gdb_selection_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(gdb_w),
					   "gdb_selection_dialog", NULL, 0));
	Delay::register_shell(gdb_selection_dialog);

	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog, 
					       XmDIALOG_TEXT));
	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog, 
					       XmDIALOG_SELECTION_LABEL));
	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog, 
					       XmDIALOG_APPLY_BUTTON));

	gdb_selection_list_w = XmSelectionBoxGetChild(gdb_selection_dialog, 
						      XmDIALOG_LIST);
	XtVaSetValues(gdb_selection_list_w,
		      XmNselectionPolicy, XmSINGLE_SELECT,
		      NULL);
	XtAddCallback(gdb_selection_dialog,
		      XmNokCallback, SelectCB, 0);
	XtAddCallback(gdb_selection_dialog,
		      XmNcancelCallback, gdbCommandCB, XtPointer("\003"));
	XtAddCallback(gdb_selection_dialog,
		      XmNhelpCallback, ImmediateHelpCB, 0);
    }

    int count       = prompt.freq('\n') + 1;
    string *choices = new string[count];
    bool *selected  = new bool[count];

    split(prompt, choices, count, '\n');

    for (int i = 0; i < count; i++)
	selected[i] = (get_positive_nr(choices[i]) == 1);

    setLabelList(gdb_selection_list_w, choices, selected, count);

    delete[] choices;
    delete[] selected;

    XtManageChild(gdb_selection_dialog);
}

void YnCB(Widget dialog, 
	  XtPointer client_data, 
	  XtPointer call_data)
{
    gdbCommandCB(dialog, client_data, call_data);
    if (yn_dialog)
	XtUnmanageChild(yn_dialog);
}

//-----------------------------------------------------------------------------
// Status recognition
//-----------------------------------------------------------------------------

void set_status_from_gdb(const string& text)
{
    if (private_gdb_input)
	return;

    if (!show_next_line_in_status && !text.matches(gdb->prompt()))
	return;

    // Fetch line before prompt in GDB window
    String s = XmTextGetString(gdb_w);
    string message = s + messagePosition;
    XtFree(s);

    if (message == "" && text.contains('\n'))
	message = text;

    if (show_next_line_in_status && 
	(message == "" || message[message.length() - 1] != '\n'))
	return;

    // Skip prompt and uncomplete lines
    int idx = message.index('\n', -1);
    if (idx >= 0)
	message = message.before(idx);

    strip_final_newlines(message);
    if (message == "" && text.contains('\n'))
	message = text;

    if (show_next_line_in_status)
    {
	messagePosition = XmTextGetLastPosition(gdb_w) + text.length();
	show_next_line_in_status = false;
	message.gsub('\n', ' ');
    }
    else
    {
	// Show first line only
	while (message != "" && message[0] == '\n')
	    message = message.after('\n');
	if (message.contains('\n'))
	    message = message.before('\n');
    }

    strip_final_newlines(message);
    if (message == "")
	return;

    set_status(message);
}

void set_status(const string& message)
{
    if (status_w == 0)
	return;

    string m = message;
    if (m != "" && !m.contains("=") && isascii(m[0]) && islower(m[0]))
	m[0] = toupper(m[0]);

    MString msg(m, "rm");
    XtVaSetValues(status_w,
		  XmNlabelString, msg.xmstring(),
		  NULL);
    XFlush(XtDisplay(status_w));
}

//-----------------------------------------------------------------------------
// Output
//-----------------------------------------------------------------------------

static regex RXcontrol("["
		       "\001"  // SOH
		       "\002"  // STX
		       "\003"  // ETX
		       "\004"  // EOT
		       "\005"  // ENQ
		       "\006"  // ACK
		       "\007"  // BEL
		       "\010"  // BS
		       "\011"  // HT
		       // "\012"  // NL
		       "\013"  // VT
		       "\014"  // NP
		       "\015"  // CR
		       "\016"  // SO
		       "\017"  // SI
		       "\020"  // DLE
		       "\021"  // DC1
		       "\022"  // DC2
		       "\023"  // DC3
		       "\024"  // DC4
		       "\025"  // NAK
		       "\026"  // SYN
		       "\027"  // ETB
		       "\030"  // CAN
		       "\031"  // EM
		       "\032"  // SUB
		       "\033"  // ESC
		       "\034"  // FS
		       "\035"  // GS
		       "\036"  // RS
		       "\037"  // US
		       "\177"  // DEL
		       "]", true);

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

	XmTextPosition startOfLine = promptPosition;
	while (startOfLine - 1 >= 0 && message[startOfLine - 1] != '\n')
	    startOfLine--;

	switch (ctrl)
	{
	case '\t':
	{
	    const int TAB_WIDTH = 8;
	    int column = promptPosition - startOfLine;
	    int spaces = TAB_WIDTH - column % TAB_WIDTH;
	    string spacing = replicate(' ', spaces);
	
	    XmTextInsert(gdb_w, promptPosition, (String)spacing);
	    promptPosition += spacing.length();
	    break;
	}

	case '\r':
	{
	    XmTextReplace(gdb_w, startOfLine, promptPosition, "");
	    promptPosition = startOfLine;
	    break;
	}
	}
	break;
    }

    case '\b':
    {
	XmTextReplace(gdb_w, promptPosition - 1, promptPosition, "");
	promptPosition--;
	break;
    }

    default:
    {
	string c;
	if (ctrl < ' ')
	    c = string("^") + string('@' + int(ctrl));
	else
	    c = "^?";
	XmTextInsert(gdb_w, promptPosition, (String)c);
	promptPosition += c.length();
    }
    }

    XmTextShowPosition(gdb_w, promptPosition);
}
	

// Append TEXT to GDB output
void _gdb_out(string text)
{
    if (text == "")
	return;

    gdb_input_at_prompt = false;

    if (promptPosition == 0)
	promptPosition = XmTextGetLastPosition(gdb_w);

    private_gdb_output = true;

    static string empty;
    if (gdb_out_ignore != "")
	text.gsub(gdb_out_ignore, empty);

    set_selection_from_gdb(text);
    set_buttons_from_gdb(text);
    set_status_from_gdb(text);
    set_tty_from_gdb(text);

    tty_out(text);

    char ctrl;
    do {
	string block = text;
	int i = block.index(RXcontrol);
	ctrl = '\0';
	if (i >= 0)
	{
	    ctrl  = block[i];
	    block = block.before(i);
	    text  = text.after(i);
	}
	else
	    text = "";

	XmTextInsert(gdb_w, promptPosition, (String)block);
	promptPosition += block.length();
	XmTextShowPosition(gdb_w, promptPosition);

	if (ctrl)
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
// Command TTY
//-----------------------------------------------------------------------------

// TTY input received
void tty_command(Agent *, void *, void *call_data)
{
    DataLength *d = (DataLength *)call_data;
    if (promptPosition == 0)
	promptPosition = XmTextGetLastPosition(gdb_w);

    // Simply insert text, invoking all necessary callbacks
    tty_gdb_input = true;
    XmTextInsert(gdb_w, promptPosition, (String)d->data);
    tty_gdb_input = false;
}

// TTY EOF received
void tty_eof(Agent *, void *, void *)
{
    // Kill and exit
    gdb->terminate();
}

// Echo on TTY
void tty_out(const string& text)
{
    if (tty_gdb_input)
	return;

    _tty_out(text);
}

void _tty_out(const string& text)
{
    if (command_tty == 0)
	return;

    command_tty->write((char *)text, text.length());
}

void tty_full_name(const string& pos)
{
    if (app_data.full_name_mode)
    {
	_tty_out("\032\032" + pos + "\n");
    }
    else
    {
	string line = source_view->get_line(pos);
	_tty_out(line + "\n");
    }
}

void prompt()
{
    _gdb_out(gdb->default_prompt());
    if (tty_gdb_input)
	_tty_out(gdb->default_prompt());
}

//-----------------------------------------------------------------------------
// Exiting
//-----------------------------------------------------------------------------

// Some cleanup actions...

void kill_exec_tty()
{
    if (separate_tty_pid > 0)
    {
	if (remote_gdb())
	{
	    ostrstream os;
	    os << "kill -" << SIGHUP << " " << separate_tty_pid 
	       << " >/dev/null </dev/null 2>&1 &";
	    Agent agent(sh_command(string(os)));
	    agent.start();
	    agent.wait();
	}
	else
	    kill(separate_tty_pid, SIGHUP);
    }

    separate_tty_pid    = 0;
    separate_tty_window = 0;
}

void remove_init_file()
{
    if (gdb_init_file != "")
    {
	if (remote_gdb())
	{
	    string rm_init_file = sh_command(
		"rm -f " + gdb_init_file + " >/dev/null </dev/null 2>&1 &");
	    Agent agent(rm_init_file);
	    agent.start();
	    agent.wait();
	}
	else
	{
	    unlink((String)gdb_init_file);
	}
	gdb_init_file = "";
    }
}

static void ddd_cleanup()
{
    if (ddd_is_exiting)
	return;

    ddd_is_exiting = true;

    remove_init_file();
    kill_exec_tty();
    if (command_shell && app_data.save_options_on_exit)
	save_options(command_shell);
    if (command_shell && app_data.save_history_on_exit)
	save_history(command_shell);
    if (gdb)
	gdb->shutdown();
}

// Signal handler: clean up and re-raise signal
static void ddd_signal(int sig)
{
    ddd_cleanup();
    signal(sig, (void (*)(int))SIG_DFL);
    kill(getpid(), sig);
}

// Fatal signal handler: issue error message and re-raise signal
static void ddd_fatal(int sig)
{
    // IF YOU GET HERE WHILE DEBUGGING DDD, READ THIS
    // ----------------------------------------------
    //
    // On some systems, especially HP-UX, the stack frame gets corrupted
    // when a program exits via a signal handler - one cannot determine
    // the place the signal handler was called from, which makes debugging
    // impossible.
    // 
    // You can circumvent this problem by invoking the debugged DDD
    // process with the environment variable DDD_NO_SIGNAL_HANDLERS set.
    // This disables installation of the `ddd_fatal' signal handler
    // and makes it possible for you to determine the problem cause.

    static int fatal_entered = 0;

    if (fatal_entered++ || !main_loop_entered || ddd_is_exiting)
    {
	static const char msg[] =
	    "\nInternal error (%s).\n"
	    "\n"
	    "Congratulations!  You have found a bug in " DDD_NAME ".\n"
	    "\n"
	    "Please send a bug report to `ddd-bugs@ips.cs.tu-bs.de', "
	    "giving a subject like\n"
	    "\n"
	    "    " DDD_NAME " " DDD_VERSION 
	    " (" DDD_HOST ") gets `%s' signal\n"
	    "\n"
	    "To enable us to fix the bug, you should include "
	    "the following information:\n"
	    "  * What you were doing to get this message.  "
	    "Report all the facts.\n"
	    "  * Your " DDD_NAME " configuration.  "
	    "Run %s --configuration' to get it.\n"
	    "  * If a core file was generated in your directory, please run\n"
	    "    `gdb %s core', and type `where' at the `(gdb)' prompt.\n"
	    "See also the section \"Reporting Bugs\" "
	    "in the " DDD_NAME " manual page.\n"
	    "\n"
	    "We thank you for your support.\n\n";

	if (sig != SIGINT)
	{
	    fprintf(stderr, msg, sigName(sig), sigName(sig),
		    ddd_invoke_name, ddd_invoke_name);
	}

	// Re-raise signal.  This should kill us as we return.
	ddd_signal(sig);
	return;
    }

    // Show the message in an error dialog,
    // allowing the user to clean up manually.
    string msg = string("Internal error (") + sigName(sig) + ")";
    if (sig != SIGINT)
    {
	static Widget fatal_dialog = 0;
	if (fatal_dialog)
	    XtDestroyWidget(fatal_dialog);
	fatal_dialog = 
	    verify(XmCreateErrorDialog (find_shell(),
					"fatal_dialog", NULL, 0));
	Delay::register_shell(fatal_dialog);

	XtUnmanageChild (XmMessageBoxGetChild 
			 (fatal_dialog, XmDIALOG_CANCEL_BUTTON));
	XtAddCallback (fatal_dialog, XmNhelpCallback, ImmediateHelpCB, NULL);
	XtAddCallback (fatal_dialog, XmNokCallback,   DDDExitCB,       NULL);

	MString mtext(msg, "rm");
	XtVaSetValues (fatal_dialog,
		       XmNmessageString, mtext.xmstring(),
		       NULL);

	XtManageChild (fatal_dialog);
    }

    // Reinstall fatal error handlers
    ddd_install_fatal();

    // Return to main event loop
    fatal_entered--;
    longjmp(main_loop_env, sig);
}

// Xt Warning handler
static void ddd_xt_warning(String message)
{
    if (!app_data.suppress_warnings)
    {
	ddd_original_xt_warning_handler(message);

	static bool informed = false;

	if (!informed)
	{
	    cerr << "(You can suppress these warnings\n"
		 << "by setting the 'Suppress X Warnings' option"
		 << " in the DDD `Options` menu.)\n";
	    informed = true;
	}
    }
}

// Close callback
void DDDCloseCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }

    Widget shell = findTopLevelShellParent(w);
    popdown_shell(shell);
}

void SaveOptionsAndExitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    ddd_cleanup();

    DDDSaveOptionsCB(w, client_data, call_data);
    ExitCB(w, client_data, call_data);
}

// Exit callback
void _DDDExitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    ddd_cleanup();

    if (startup_options_changed)
    {
	// Startup options are still changed; request confirmation
	if (yn_dialog)
	    XtDestroyWidget(yn_dialog);
	yn_dialog = 
	    verify(XmCreateQuestionDialog(find_shell(w), 
					  "save_options_dialog", NULL, 0));
	Delay::register_shell(yn_dialog);
	XtAddCallback (yn_dialog, XmNokCallback,     SaveOptionsAndExitCB, 0);
	XtAddCallback (yn_dialog, XmNcancelCallback, ExitCB, 0);
	XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);
	XtManageChild(yn_dialog);
    }
    else
	ExitCB(w, client_data, call_data);
}

// Exit after confirmation
void DDDExitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (gdb == 0 || !gdb->running())
    {
	_DDDExitCB(w, client_data, call_data);
	return;
    }

    if (gdb->isReadyWithPrompt())
    {
	gdb_command("quit");
	return;
    }

    // Debugger is still running; request confirmation
    if (yn_dialog)
	XtDestroyWidget(yn_dialog);
    yn_dialog = verify(XmCreateQuestionDialog(find_shell(w),
					      "quit_dialog", NULL, 0));
    Delay::register_shell(yn_dialog);
    XtAddCallback (yn_dialog, XmNokCallback,     _DDDExitCB, 0);
    XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    XtManageChild(yn_dialog);
}

// EOF on input/output detected
void gdb_eofHP(Agent *, void *, void *)
{
    // Kill and exit
    gdb->terminate();
}


//-----------------------------------------------------------------------------
// WWW Page
//-----------------------------------------------------------------------------

void DDDWWWPageCB(Widget, XtPointer, XtPointer)
{
    string url = app_data.www_page;
    string cmd = app_data.www_command;

    StatusDelay delay("Invoking WWW browser for " + quote(url));

    cmd.gsub("@URL@", url);
    cmd += " &";
    cmd = sh_command(cmd, true);
    system(cmd);
}


//-----------------------------------------------------------------------------
// Printing Graphs
//-----------------------------------------------------------------------------

// Convert according to given BoxPrintGC
static int convert(string filename, BoxPrintGC& gc, bool selectedOnly)
{
    StatusDelay delay("Printing graph to " + quote(filename));

    // Get the graph
    Graph *graph = graphEditGetGraph(data_disp->graph_edit);

    if (graph->firstNode() == 0)
    {
	post_error("No displays to print.", "print_empty_graph_error", 
		   data_disp->graph_edit);
	return -1;
    }

    // Get and set the GC
    GraphGC graphGC = graphEditGetGraphGC(data_disp->graph_edit);
    graphGC.printGC = &gc;
    graphGC.printSelectedNodesOnly = selectedOnly;

    ofstream os(filename);
    if (os.bad())
    {
	FILE *fp = fopen(filename, "w");
	post_error("Cannot open " + quote(filename) + ": " + strerror(errno), 
		   "print_failed_error", data_disp->graph_edit);
	if (fp)
	    fclose(fp);
	return -1;
    }

    graph->print(os, graphGC);
    return 0;
}

static void deletePrintAgent(XtPointer client_data, XtIntervalId *)
{
    // Delete agent after use
    Agent *edit_agent = (Agent *)client_data;
    delete edit_agent;
}

static void unlinkPrintFile(XtPointer client_data, XtIntervalId *)
{
    // Delete temp file after use
    string *tempfile = (string *)client_data;
    unlink(*tempfile);
    delete tempfile;
}

static void printDoneHP(Agent *print_agent, void *client_data, void *)
{
    // Printing is done: remove temporary file
    XtAppAddTimeOut(app_context, 0, deletePrintAgent, 
		    XtPointer(print_agent));
    XtAppAddTimeOut(app_context, 0, unlinkPrintFile, 
		    XtPointer(client_data));
}

static void printOutputHP(Agent *, void *, void *call_data)
{
    DataLength *input = (DataLength *)call_data;
    post_warning(string(input->data, input->length), "print_warning");
}

// Print according to given BoxPrintGC
static int print(string command, BoxPrintGC& gc, bool selectedOnly)
{
    string tempfile = tmpnam(0);
    int ret = convert(tempfile, gc, selectedOnly);
    if (ret)
	return ret;

    StatusDelay delay("Printing graph " + quote(tempfile));

    command = command + " " + tempfile;

    LiterateAgent *print_agent = new LiterateAgent(app_context, command);
    print_agent->removeAllHandlers(Died);
    print_agent->addHandler(InputEOF, printDoneHP, 
			   (void *)new string(tempfile));
    print_agent->addHandler(Input, printOutputHP);
    print_agent->addHandler(Error, printOutputHP);
    print_agent->start();

    return 0;
}

// Local state of print dialog
enum PrintType { PRINT_POSTSCRIPT, PRINT_FIG };

static bool            print_selected_only = false;
static bool            print_to_printer    = true;
static BoxPostScriptGC print_postscript_gc;
static BoxFigGC        print_xfig_gc;
static PrintType       print_type = PRINT_POSTSCRIPT;
static Widget          print_dialog = 0;
static Widget          print_command_field   = 0;
static Widget          print_file_name_field = 0;


// Go and print according to local state
void graphQuickPrintCB(Widget w, XtPointer client_data, XtPointer)
{
    if (print_to_printer)
    {
	string command = app_data.print_command;
	if (print_command_field)
	{
	    String c = XmTextFieldGetString(print_command_field);
	    command = c;
	    XtFree(c);
	}
	if (print(command, print_postscript_gc, print_selected_only) == 0)
	{
	    if (print_dialog)
		XtUnmanageChild(print_dialog);
	}
    }
    else
    {
	BoxPrintGC *gc_ptr = 0;
	switch (print_type)
	{
	case PRINT_POSTSCRIPT:
	    gc_ptr = &print_postscript_gc;
	    break;

	case PRINT_FIG:
	    gc_ptr = &print_xfig_gc;
	    break;
	}
	BoxPrintGC& gc = *gc_ptr;

	String file = XmTextFieldGetString(print_file_name_field);
	string f = file;
	XtFree(file);

	strip_final_blanks(f);
	if (f == "")
	    return;

	if (access(f, W_OK) || !is_regular_file(f) || client_data)
	{
	    // File does not exist, is special, or override is on
	    if (convert(f, gc, print_selected_only) == 0)
	    {
		if (print_dialog)
		    XtUnmanageChild(print_dialog);
	    }
	}
	else
	{
	    // File exists - request confirmation
	    if (yn_dialog)
		XtDestroyWidget(yn_dialog);
	    yn_dialog = 
		verify(XmCreateQuestionDialog(find_shell(w),
					      "confirm_overwrite_dialog", 
					      NULL, 0));
	    Delay::register_shell(yn_dialog);
	    XtAddCallback (yn_dialog, XmNokCallback,   graphQuickPrintCB, 
			   (void *)1);
	    XtAddCallback (yn_dialog, XmNhelpCallback, ImmediateHelpCB, 0);

	    string question = "Overwrite existing file " + quote(f) + "?";
	    XmString xmtext = XmStringCreateLtoR (question, "rm");
	    XtVaSetValues (yn_dialog, XmNmessageString, xmtext, NULL);
	    XmStringFree (xmtext);

	    XtManageChild (yn_dialog);
	}
    }
}

static void SetPrintTypeCB(Widget, XtPointer client_data, XtPointer)
{
    print_type = PrintType(client_data);
}

static void SetSensitiveCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	XtSetSensitive(Widget(client_data), True);
}

static void TakeFocusCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	XmProcessTraversal(Widget(client_data), XmTRAVERSE_CURRENT);
}

static void UnsetSensitiveCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	XtSetSensitive(Widget(client_data), False);
}

static void SetPrintSelectedNodesCB(Widget w, XtPointer, XtPointer)
{
    print_selected_only = XmToggleButtonGetState(w);
}

static void SetPrintTargetCB(Widget w, XtPointer, XtPointer)
{
    print_to_printer = XmToggleButtonGetState(w);
}

static void SetGCA4(Widget w, XtPointer, XtPointer)
{
    if (XmToggleButtonGetState(w))
    {
	BoxPostScriptGC a4;

	print_postscript_gc.hsize = a4.hsize;
	print_postscript_gc.vsize = a4.vsize;
    }
}

static void SetGCLetter(Widget w, XtPointer, XtPointer)
{
    BoxPostScriptGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 8 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 11         - gc.voffset * 2;
    }
}

static void SetGCLegal(Widget w, XtPointer, XtPointer)
{
    BoxPostScriptGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 8 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 14         - gc.voffset * 2;
    }
}

static void SetGCExecutive(Widget w, XtPointer, XtPointer)
{
    BoxPostScriptGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 7 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 10         - gc.voffset * 2;
    }
}

static void SetGCOrientation(Widget w, XtPointer, XtPointer)
{
    if (XmToggleButtonGetState(w))
	print_postscript_gc.orientation = BoxPostScriptGC::PORTRAIT;
    else
	print_postscript_gc.orientation = BoxPostScriptGC::LANDSCAPE;
}

void graphPrintCB(Widget w, XtPointer, XtPointer)
{
    if (print_dialog != 0)
    {
	// Dialog already created -- pop it up again
	XtUnmanageChild(print_dialog);
	XtManageChild(print_dialog);
	return;
    }

    Arg args[10];
    Cardinal num_args;

    print_dialog = 
	verify(XmCreatePromptDialog(find_shell(w), "print", ArgList(0), 0));
    Delay::register_shell(print_dialog);
    XtAddCallback(print_dialog, XmNokCallback,     
		  graphQuickPrintCB, XtPointer(0));
    XtAddCallback(print_dialog, XmNcancelCallback, 
		  UnmanageThisCB, XtPointer(print_dialog));
    XtAddCallback(print_dialog, XmNhelpCallback,   
		  ImmediateHelpCB, XtPointer(0));

    // Remove old prompt
    Widget text = XmSelectionBoxGetChild(print_dialog, 
					 XmDIALOG_TEXT);
    XtUnmanageChild(text);
    Widget label = XmSelectionBoxGetChild(print_dialog, 
					  XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(label);

    // Create form as work area
    Widget options = verify(XmCreateRowColumn(print_dialog, "options", 0, 0));
    XtManageChild(options);

    // Build options
    Widget print_to_option = 
	verify(XmCreateRowColumn(options, "print_to_option", 0, 0));
    Widget print_to = 
	verify(XmCreateLabel(print_to_option, "print_to", 0, 0));
    Widget print_to_field = 
	verify(XmCreateRadioBox(print_to_option, "print_to_field", 0, 0));
    Widget print_to_printer = 
	verify(XmCreateToggleButton(print_to_field, "printer", 0, 0));
    Widget print_to_file = 
	verify(XmCreateToggleButton(print_to_field, "file", 0, 0));
    XtVaSetValues(print_to_field, XmNpacking, XmPACK_TIGHT, 0);
    XtManageChild(print_to_option);
    XtManageChild(print_to);
    XtManageChild(print_to_field);
    XtManageChild(print_to_printer);
    XtManageChild(print_to_file);


    Widget print_command_option = 
	verify(XmCreateRowColumn(options, "print_command_option", 0, 0));
    Widget print_command = 
	verify(XmCreateLabel(print_command_option, "print_command", 0, 0));
    print_command_field = 
	verify(XmCreateTextField(print_command_option, 
				 "print_command_field", 0, 0));
    XtManageChild(print_command_option);
    XtManageChild(print_command);
    XtManageChild(print_command_field);

    string command = string(app_data.print_command) + " ";
    XmTextFieldSetString(print_command_field, command);

    Widget file_name_option = 
	verify(XmCreateRowColumn(options, "file_name_option", 0, 0));
    Widget file_name = 
	verify(XmCreateLabel(file_name_option, "file_name", 0, 0));
    print_file_name_field = 
	verify(XmCreateTextField(file_name_option, "file_name_field", 0, 0));
    Widget file_type_menu =
	verify(XmCreatePulldownMenu(file_name_option, "type", 0, 0));
    Widget postscript = 
	verify(XmCreatePushButton(file_type_menu, "postscript", 0, 0));
    Widget xfig = 
	verify(XmCreatePushButton(file_type_menu, "xfig", 0, 0));
    num_args = 0;
    XtSetArg(args[num_args], XmNsubMenuId, file_type_menu); num_args++;
    Widget file_type = 
	verify(XmCreateOptionMenu(file_name_option, "type", args, num_args));
    XtManageChild(file_name_option);
    XtManageChild(file_name);
    XtManageChild(file_type);
    XtManageChild(print_file_name_field);
    XtManageChild(postscript);
    XtManageChild(xfig);

    XtAddCallback(postscript, XmNactivateCallback, 
		  SetPrintTypeCB, XtPointer(PRINT_POSTSCRIPT));
    XtAddCallback(xfig, XmNactivateCallback, 
		  SetPrintTypeCB, XtPointer(PRINT_FIG));

    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  SetPrintTargetCB, 0);

    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  SetSensitiveCB, XtPointer(print_command_field));
    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  SetSensitiveCB, XtPointer(print_command));
    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(print_file_name_field));
    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(file_type));
    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(file_name));
    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  TakeFocusCB, XtPointer(print_command_field));

    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(print_command_field));
    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(print_command));
    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  SetSensitiveCB, XtPointer(print_file_name_field));
    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  SetSensitiveCB, XtPointer(file_type));
    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  SetSensitiveCB, XtPointer(file_name));
    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  TakeFocusCB, XtPointer(print_file_name_field));

    XmToggleButtonSetState(print_to_printer, True, True);



    Widget print_what_option = 
	verify(XmCreateRowColumn(options, "print_what_option", 0, 0));
    Widget print_what = 
	verify(XmCreateLabel(print_what_option, "print_what", 0, 0));
    Widget print_what_field = 
	verify(XmCreateRadioBox(print_what_option, "print_what_field", 0, 0));
    Widget print_all = 
	verify(XmCreateToggleButton(print_what_field, "all", 0, 0));
    Widget print_selected = 
	verify(XmCreateToggleButton(print_what_field, "selected", 0, 0));
    XtVaSetValues(print_what_field, XmNpacking, XmPACK_TIGHT, 0);
    XtManageChild(print_what_option);
    XtManageChild(print_what);
    XtManageChild(print_what_field);
    XtManageChild(print_all);
    XtManageChild(print_selected);
    XtAddCallback(print_selected, XmNvalueChangedCallback, 
		  SetPrintSelectedNodesCB, 0);

    XmToggleButtonSetState(print_all, True, True);

    Widget print_orientation_option = 
	verify(XmCreateRowColumn(options, "print_orientation_option", 0, 0));
    Widget print_orientation = 
	verify(XmCreateLabel(print_orientation_option,
			     "print_orientation", 0, 0));
    Widget print_orientation_field = 
	verify(XmCreateRadioBox(print_orientation_option,
				"print_orientation_field", 0, 0));
    Widget print_portrait = 
	verify(XmCreateToggleButton(print_orientation_field, 
				    "portrait", 0, 0));
    Widget print_landscape = 
	verify(XmCreateToggleButton(print_orientation_field, 
				    "landscape", 0, 0));
    XtVaSetValues(print_orientation_field, XmNpacking, XmPACK_TIGHT, 0);
    XtManageChild(print_orientation_option);
    XtManageChild(print_orientation);
    XtManageChild(print_orientation_field);
    XtManageChild(print_portrait);
    XtManageChild(print_landscape);
    XtAddCallback(print_portrait, XmNvalueChangedCallback, 
		  SetGCOrientation, 0);

    XmToggleButtonSetState(print_portrait, True, True);

    Widget paper_size_option = 
	verify(XmCreateRowColumn(options, "paper_size_option", 0, 0));
    Widget paper_size = 
	verify(XmCreateLabel(paper_size_option, "paper_size", 0, 0));
    Widget paper_size_field = 
	verify(XmCreateRadioBox(paper_size_option, "paper_size_field", 0, 0));
    Widget a4 = 
	verify(XmCreateToggleButton(paper_size_field, "a4", 0, 0));
    Widget letter = 
	verify(XmCreateToggleButton(paper_size_field, "letter", 0, 0));
    Widget legal = 
	verify(XmCreateToggleButton(paper_size_field, "legal", 0, 0));
    Widget executive = 
	verify(XmCreateToggleButton(paper_size_field, "executive", 0, 0));
    XtManageChild(paper_size_option);
    XtManageChild(paper_size);
    XtManageChild(paper_size_field);
    XtManageChild(a4);
    XtManageChild(letter);
    XtManageChild(legal);
    XtManageChild(executive);

    XtAddCallback(a4,        XmNvalueChangedCallback, SetGCA4,        0);
    XtAddCallback(letter,    XmNvalueChangedCallback, SetGCLetter,    0);
    XtAddCallback(legal,     XmNvalueChangedCallback, SetGCLegal,     0);
    XtAddCallback(executive, XmNvalueChangedCallback, SetGCExecutive, 0);


    XmToggleButtonSetState(a4, True, True);

    XtManageChild(print_dialog);
}


//-----------------------------------------------------------------------------
// Other Graph Functions
//-----------------------------------------------------------------------------

void graphToggleShowGridCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNshowGrid, info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Grid on.");
    else
	set_status("Grid off.");

    update_options();
    options_changed = true;
}

void graphToggleShowHintsCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNshowHints, info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Hints on.");
    else
	set_status("Hints off.");

    update_options();
    options_changed = true;
}


void graphToggleSnapToGridCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNsnapToGrid, info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Snap to grid on.");
    else
	set_status("Snap to grid off.");

    update_options();
    options_changed = true;
}


void graphToggleCompactLayoutCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    LayoutMode mode = RegularLayoutMode;
    if (info->set)
	mode = CompactLayoutMode;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNlayoutMode, mode); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Compact layout enabled.");
    else
	set_status("Regular layout enabled.");

    update_options();
    options_changed = true;
}

void graphToggleAutoLayoutCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNautoLayout, info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Automatic Layout on.");
    else
	set_status("Automatic Layout off.");

    update_options();
    options_changed = true;
}


void graphAlignCB(Widget, XtPointer, XtPointer)
{
    XtCallActionProc(data_disp->graph_edit, 
		     "snap-to-grid", (XEvent *)0, (String *)0, 0);
}

void graphRotateCB(Widget, XtPointer, XtPointer)
{
    String params[1];
    params[0] = "+90";

    XtCallActionProc(data_disp->graph_edit,
		     "rotate", (XEvent *)0, params, 1);
}

void graphLayoutCB(Widget, XtPointer, XtPointer)
{
    XtCallActionProc(data_disp->graph_edit, 
		     "layout", (XEvent *)0, (String *)0, 0);
}


//-----------------------------------------------------------------------------
// Source Options
//-----------------------------------------------------------------------------

void sourceToggleFindWordsOnlyCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.find_words_only = info->set;

    if (info->set)
	set_status("Finding only complete words.");
    else
	set_status("Finding arbitrary occurrences.");

    update_options();
    options_changed = true;
}

void sourceToggleCacheSourceFilesCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.cache_source_files = info->set;

    if (info->set)
	set_status("Caching source texts.");
    else
	set_status("Not caching source texts.");

    update_options();
    options_changed = true;
}

void sourceToggleCacheMachineCodeCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.cache_machine_code = info->set;

    if (info->set)
	set_status("Caching machine code.");
    else
	set_status("Not caching machine code.");

    update_options();
    options_changed = true;
}

void sourceToggleDisplayGlyphsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.display_glyphs = info->set;

    if (info->set)
	set_status("Displaying breakpoints and positions as glyphs.");
    else
	set_status("Displaying breakpoints and positions in the text.");

    update_options();
    options_changed = true;
}

void sourceToggleDisassembleCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.disassemble = info->set;

    if (info->set)
	set_status("Showing machine code.");
    else
	set_status("Not showing machine code.");

    update_options();
    options_changed = true;
}

//-----------------------------------------------------------------------------
// General Options
//-----------------------------------------------------------------------------

void dddToggleGroupIconifyCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.group_iconify = info->set;

    if (info->set)
	set_status(DDD_NAME " windows are iconified as a group.");
    else
	set_status(DDD_NAME " windows are iconified separately.");

    update_options();
    options_changed = true;
}

void dddToggleGlobalTabCompletionCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.global_tab_completion = info->set;

    if (info->set)
	set_status("TAB key completes in all " DDD_NAME " windows.");
    else
	set_status("TAB key completes in " DDD_NAME " command window only.");

    update_options();
    options_changed = true;
}

void dddToggleSeparateExecWindowCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.separate_exec_window = info->set;

    if (info->set)
	set_status("Debugged program will be executed in a "
		   "separate execution window.");
    else
	set_status("Debugged program will be executed in the " 
		   DDD_NAME " command window.");

    update_options();
    options_changed = true;
}

void dddToggleSaveOptionsOnExitCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.save_options_on_exit = info->set;

    if (info->set)
	set_status("Options will be saved when " DDD_NAME " exits.");
    else
	set_status("Options must be saved manually.");

    update_options();
    options_changed = true;
}

void dddToggleSaveHistoryOnExitCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.save_history_on_exit = info->set;

    if (info->set)
	set_status("History will be saved when " DDD_NAME " exits.");
    else
	set_status("History will not be saved.");

    update_options();
    options_changed = true;
}

void dddToggleSuppressWarningsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.suppress_warnings = info->set;

    if (info->set)
	set_status("X Warnings are suppressed.");
    else
	set_status("X Warnings are not suppressed.");

    update_options();
    options_changed = true;
}


//-----------------------------------------------------------------------------
// Startup Options
//-----------------------------------------------------------------------------
static void post_startup_warning(Widget /* w */)
{
#if 0
    post_warning(
	"This change will be effective only after\n"
	"options are saved and " DDD_NAME " is restarted.", 
	"startup_warning", w);
#endif
}

void dddSetSeparateWindowsCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);

    app_data.separate_data_window   = state;
    app_data.separate_source_window = state;

    if (state)
	set_status("Next " DDD_NAME
		   " invocation will start-up with separate windows.");
    else
	set_status("Next " DDD_NAME 
		   " invocation will start-up with one single window.");

    update_options();
    post_startup_warning(w);
    startup_options_changed = options_changed = true;
}

void dddSetKeyboardFocusPolicyCB (Widget w, XtPointer client_data, XtPointer)
{
    unsigned char policy = (unsigned char)int(client_data);

    const WidgetArray& shells = Delay::shells();
    for (int i = 0; i < shells.size(); i++)
    {
	Widget shell = shells[i];
	while (shell && !XmIsVendorShell(shell))
	    shell = XtParent(shell);
	if (shell)
	{
	    XtVaSetValues(shell,
			  XmNkeyboardFocusPolicy, policy,
			  NULL);
	}
    }

    string keyboardFocusPolicy = string("*") + string(XmNkeyboardFocusPolicy);
    XrmDatabase target = XtDatabase(XtDisplay(w));
    switch (policy)
    {
    case XmEXPLICIT:
	XrmPutStringResource(&target, keyboardFocusPolicy,  "EXPLICIT");
	set_status("Setting click-to-type keyboard focus policy.");
	break;

    case XmPOINTER:
	XrmPutStringResource(&target, keyboardFocusPolicy,  "POINTER");
	set_status("Setting pointer-driven keyboard focus policy.");
	break;
    }

    update_options();
    startup_options_changed = options_changed = true;
}

void dddSetPannerCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);
    app_data.panned_graph_editor = state;

    if (state)
	set_status("Next " DDD_NAME
		   " invocation will start-up with a panned graph editor.");
    else
	set_status("Next " DDD_NAME 
		   " invocation will start-up with a scrolled graph editor.");

    update_options();
    post_startup_warning(w);
    startup_options_changed = options_changed = true;
}

void dddSetDebuggerCB (Widget w, XtPointer client_data, XtPointer)
{
    DebuggerType type = DebuggerType(client_data);

    switch (type)
    {
    case DBX:
	app_data.debugger = "dbx";
	set_status("Next " DDD_NAME
		   " invocation will start-up with a DBX debugger.");
	break;

    case GDB:
	app_data.debugger = "gdb";
	set_status("Next " DDD_NAME
		   " invocation will start-up with a GDB debugger.");
	break;
    }

    update_options();
    post_startup_warning(w);
    startup_options_changed = options_changed = true;
}

//-----------------------------------------------------------------------------
// Option management
//-----------------------------------------------------------------------------

inline String bool_value(bool value)
{
    return value ? S_true : S_false;
}

inline string bool_app_value(const string& name, bool value)
{
    return string(DDD_CLASS_NAME) + "*" + name + ": " + bool_value(value);
}

inline string string_app_value(const string& name, const string& value)
{
    return string(DDD_CLASS_NAME) + "*" + name + ": " + value;
}

inline string int_app_value(const string& name, int value)
{
    return string(DDD_CLASS_NAME) + "*" + name + ": " + itostring(value);
}

string widget_value(Widget w, String name)
{
    String value = 0;
    XtVaGetValues(w, 
		  XtVaTypedArg, name, XtRString, &value, sizeof(value),
		  NULL);

    if (value == 0)
	value = (String)XtNewString("");

    return string_app_value(string(XtName(w)) + "." + name, value);
}

string options_file()
{
    char *home = getenv("HOME");
    if (home == 0)
    {
	static int warned = 0;
	if (warned++ == 0)
	    cerr << "Warning: environment variable HOME undefined\n";
	return ".dddinit";
    }

    return string(home) + "/.dddinit";
}

static void save_options(Widget origin)
{
    StatusDelay delay("Saving options in " + quote(options_file()));
    string file = options_file();

    const char delimiter[] = "! DO NOT ADD ANYTHING BELOW THIS LINE";

    // Read the file contents into memory ...
    string dddinit;
    ifstream is(file);
    if (is.bad())
    {
	// File not found: create a new one
	dddinit = 
	    "! DDD initialization file\n"
	    "! Enter your personal DDD resources here.\n"
	    "\n";
    }
    else
    {
	char line[BUFSIZ];
	while (is)
	{
	    line[0] = '\0';
	    is.getline(line, sizeof(line));
	    if (string(line).contains(delimiter, 0))
		break;
	    dddinit += line;
	    dddinit += '\n';
	}
    }

    // ... and write them back again
    ofstream os(file);
    if (os.bad())
    {
	post_error("Cannot save options in " + quote(file),
		   "options_save_error", origin);
	return;
    }

    os << dddinit
       << delimiter
       << " -- " DDD_NAME " WILL OVERWRITE IT\n";

    // The version
    os << string_app_value(XtNdddinitVersion,
			   DDD_VERSION) << "\n";

    // Some settable top-level defaults
    os << bool_app_value(XtNfindWordsOnly,
			 app_data.find_words_only) << "\n";
    os << bool_app_value(XtNcacheSourceFiles,
			 app_data.cache_source_files) << "\n";
    os << bool_app_value(XtNcacheMachineCode,
			 app_data.cache_machine_code) << "\n";
    os << bool_app_value(XtNdisplayGlyphs,
			 app_data.display_glyphs) << "\n";
    os << bool_app_value(XtNdisassemble,
			 app_data.disassemble) << "\n";
    os << bool_app_value(XtNgroupIconify,
			 app_data.group_iconify)   << "\n";
    os << bool_app_value(XtNseparateExecWindow,
			 app_data.separate_exec_window) << "\n";
    os << bool_app_value(XtNseparateSourceWindow,
			 app_data.separate_source_window) << "\n";
    os << bool_app_value(XtNseparateDataWindow,
			 app_data.separate_data_window) << "\n";
    os << bool_app_value(XtNpannedGraphEditor,
			 app_data.panned_graph_editor) << "\n";
    os << bool_app_value(XtNsuppressWarnings,
			 app_data.suppress_warnings) << "\n";
    os << bool_app_value(XtNsaveOptionsOnExit,
			 app_data.save_options_on_exit) << "\n";
    os << bool_app_value(XtNsaveHistoryOnExit,
			 app_data.save_history_on_exit) << "\n";
    os << string_app_value(XtNdebugger,
			   app_data.debugger) << "\n";

    unsigned char policy = '\0';
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &policy, NULL);
    switch (policy)
    {
    case XmPOINTER:
	os << string_app_value(string(XmNkeyboardFocusPolicy), "POINTER") 
	   << "\n";
	break;
    case XmEXPLICIT:
	os << string_app_value(string(XmNkeyboardFocusPolicy), "EXPLICIT")
	   << "\n";
	break;
    }

    // Some settable graph editor defaults
    os << widget_value(data_disp->graph_edit, XtNshowGrid)   << "\n";
    os << widget_value(data_disp->graph_edit, XtNsnapToGrid) << "\n";
    os << widget_value(data_disp->graph_edit, XtNshowHints)  << "\n";
    os << widget_value(data_disp->graph_edit, XtNlayoutMode) << "\n";
    os << widget_value(data_disp->graph_edit, XtNautoLayout) << "\n";

    startup_options_changed = options_changed = false;
}

void DDDSaveOptionsCB (Widget w, XtPointer, XtPointer)
{
    save_options(w);
}


//-----------------------------------------------------------------------------
// Obscure Features
//-----------------------------------------------------------------------------

// Wait until W is mapped
static void wait_until_mapped(Widget w)
{
    Widget shell = find_shell(w);

    if (XtIsRealized(w) && XtIsRealized(shell))
    {
	XWindowAttributes attr;
	while (XGetWindowAttributes(XtDisplay(w), XtWindow(w), &attr)
	       && attr.map_state != IsViewable)
	{
	    if (XGetWindowAttributes(XtDisplay(shell), XtWindow(shell), &attr)
		&& attr.map_state != IsViewable)
		break;		// Shell is withdrawn or iconic

	    // Wait for exposure event
	    XEvent event;
	    XtAppNextEvent(XtWidgetToApplicationContext(w), &event);
	    XtDispatchEvent(&event);
	}
    }

    XmUpdateDisplay(w);
    XSync(XtDisplay(w), false);
}

static bool dungeon_collapsed = false;
extern void meltdown(Display *display);

static void DungeonCollapseCB(XtPointer client_data, XtIntervalId *)
{
    Widget w = Widget(client_data);

    static Widget dungeon_error = 0;
    if (dungeon_error)
	XtDestroyWidget(dungeon_error);

    Widget shell = find_shell(w);
    dungeon_error = 
	verify(XmCreateErrorDialog(shell, "dungeon_collapse_error", NULL, 0));
    Delay::register_shell(dungeon_error);
    XtUnmanageChild(XmMessageBoxGetChild 
		    (dungeon_error, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(dungeon_error, XmNhelpCallback, ImmediateHelpCB, NULL);

    MString mtext("Suddenly, the dungeon collapses.", "rm");
    XtVaSetValues (dungeon_error,
		   XmNmessageString, mtext.xmstring(),
		   NULL);

    XtManageChild(dungeon_error);
    wait_until_mapped(dungeon_error);

    sleep(2);

    meltdown(XtDisplay(w));

    dungeon_collapsed = true;
}

#define CMP(x, y, z) (cmd[x] == cmd[y] + (z))

void handle_obscure_commands(string& cmd, Widget origin)
{
    // The DDD Display Dungeon is a mythical entity where all deleted
    // displays of all DDD instantiations go.  Entering the display
    // dungeon reveals the internals of all programs ever having been
    // debugged with DDD.  Be aware, however, that this is not for the
    // faint-hearted; it requires special magical energy and a
    // supremous power of will to enter the dungeon.  Attempts to
    // force its entry without being sufficiently prepared may cause
    // the dungeon to collapse, taking the adventurer and all of its
    // possessions into debris.                             -- AZ

    if (cmd.length() == 5
	&& CMP(1, 0, 1) && CMP(3, 0, 2)	&& CMP(4, 3, -1) && CMP(2, 1, 1)
	&& cmd[0] + cmd[1] + cmd[2] + cmd[3] + cmd[4] + '<' == 666)
    {
	if (dungeon_collapsed)
	{
	    cmd = "echo Nothing happens.\\n";
	}
	else
	{
	    cmd = "echo The dungeon shakes violently! \\n";

	    static int dungeon_collapsing = 0;
	    if (dungeon_collapsing++ == 0)
	    {
		Widget w = origin ? origin : command_shell;

		XtAppAddTimeOut(XtWidgetToApplicationContext(w), 10000, 
				DungeonCollapseCB, 
				XtPointer(w));
	    }
	}
    }
}

    

//-----------------------------------------------------------------------------
// Dialogs
//-----------------------------------------------------------------------------

void post_gdb_yn(string question, Widget w)
{
    strip_final_blanks(question);
    if (question == "")
	return;

    if (yn_dialog)
	XtDestroyWidget(yn_dialog);
    yn_dialog = verify(XmCreateQuestionDialog(find_shell(w),
					      "yn_dialog", NULL, 0));
    Delay::register_shell(yn_dialog);
    XtAddCallback (yn_dialog, XmNokCallback,     YnCB, (void *)"yes");
    XtAddCallback (yn_dialog, XmNcancelCallback, YnCB, (void *)"no");
    XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    XmString xmtext = XmStringCreateLtoR (question, "rm");
    XtVaSetValues (yn_dialog, XmNmessageString, xmtext, NULL);
    XmStringFree (xmtext);

    XtManageChild (yn_dialog);
}

void post_gdb_busy(Widget w)
{
    if (ddd_is_exiting)
	return;

    static Widget busy_dialog = 0;
    if (busy_dialog)
	XtDestroyWidget(busy_dialog);

    busy_dialog = 
	verify(XmCreateWorkingDialog (find_shell(w), 
				      "busy_dialog", NULL, 0));
    Delay::register_shell(busy_dialog);
    XtUnmanageChild(XmMessageBoxGetChild 
		    (busy_dialog, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(busy_dialog, XmNhelpCallback, ImmediateHelpCB, NULL);
    XtManageChild(busy_dialog);
}

void post_gdb_died(string reason, Widget w)
{
    strip_final_blanks(reason);

    if (gdb_initialized && reason.contains("Exit 0"))
    {
	DDDExitCB(find_shell(w), 0, 0);
	return;
    }

    if (ddd_is_exiting)
    {
	cerr << reason << "\n";
	return;
    }

    string gdb_path = app_data.debugger_command;
    if (gdb_path.contains(" "))
	gdb_path = gdb_path.before(" ");
    _gdb_out("\n" + gdb_path + ": " + reason + "\n");

    static Widget died_dialog = 0;
    if (died_dialog)
	XtDestroyWidget(died_dialog);
    died_dialog = 
	verify(XmCreateErrorDialog (find_shell(w), 
				    "terminated_dialog", NULL, 0));
    Delay::register_shell(died_dialog);

    XtUnmanageChild (XmMessageBoxGetChild 
		     (died_dialog, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback (died_dialog, XmNhelpCallback, ImmediateHelpCB, NULL);
    XtAddCallback (died_dialog, XmNokCallback,   DDDExitCB,       NULL);

    XtManageChild (died_dialog);
}

void post_gdb_message(string text, Widget w)
{
    strip_final_blanks(text);
    if (text == "")
	return;

    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": " << text << "\n";
	return;
    }

#if 0
    if (status_w && !text.contains('\n'))
    {
	set_status(text);
	return;
    }
#endif

    static Widget gdb_message_dialog = 0;
    if (gdb_message_dialog)
	XtDestroyWidget(gdb_message_dialog);
    gdb_message_dialog = 
	verify(XmCreateWarningDialog (find_shell(w), 
				      "gdb_message_dialog", NULL, 0));
    Delay::register_shell(gdb_message_dialog);
    XtUnmanageChild (XmMessageBoxGetChild 
		     (gdb_message_dialog, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback (gdb_message_dialog,
		   XmNhelpCallback,
		   ImmediateHelpCB,
		   NULL);

    MString mtext(text, "rm");
    XtVaSetValues (gdb_message_dialog,
		   XmNmessageString, mtext.xmstring(),
		   NULL);

    XtManageChild (gdb_message_dialog);
}

void post_error (string text, String name, Widget w)
{
    strip_final_blanks(text);
    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": " << text << "\n";
	return;
    }

#if 0
    if (status_w && !text.contains('\n'))
    {
	set_status(text);
	return;
    }
#endif

    if (name == 0)
	name = "ddd_error";

    static Widget ddd_error = 0;
    if (ddd_error)
	XtDestroyWidget(ddd_error);
    ddd_error = 
	verify(XmCreateErrorDialog (find_shell(w), name, NULL, 0));
    Delay::register_shell(ddd_error);
    XtUnmanageChild (XmMessageBoxGetChild 
		     (ddd_error, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback (ddd_error, XmNhelpCallback, ImmediateHelpCB, NULL);

    MString mtext(text, "rm");
    XtVaSetValues (ddd_error,
		   XmNmessageString, mtext.xmstring(),
		   NULL);

    XtManageChild (ddd_error);
}

void post_warning (string text, String name, Widget w)
{
    strip_final_blanks(text);
    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": warning: " << text << "\n";
	return;
    }

#if 0
    if (status_w && !text.contains('\n'))
    {
	set_status(text);
	return;
    }
#endif

    if (name == 0)
	name = "ddd_warning";

    static Widget ddd_warning = 0;
    if (ddd_warning)
	XtDestroyWidget(ddd_warning);
    ddd_warning = 
	verify(XmCreateWarningDialog (find_shell(w), name, NULL, 0));
    Delay::register_shell(ddd_warning);
    XtUnmanageChild (XmMessageBoxGetChild 
		     (ddd_warning, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback (ddd_warning, XmNhelpCallback, ImmediateHelpCB, NULL);

    MString mtext(text, "rm");
    XtVaSetValues (ddd_warning,
		   XmNmessageString, mtext.xmstring(),
		   NULL);

    XtManageChild (ddd_warning);
}
