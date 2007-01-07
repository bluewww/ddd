// $Id$
// DDD main program (and much more)

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 1999-2001 Universitaet Passau, Germany.
// Copyright (C) 2001-2004 Free Software Foundation, Inc.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@gnu.org>.
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

char ddd_rcsid[] =
    "$Id$";

// About Bugs
// ==========
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

#include "config.h"
// #include "MemCheckD.h"

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "ddd.h"

#ifdef IF_MOTIF
// Motif stuff
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/CutPaste.h>
#include <Xm/DrawingA.h>
#include <Xm/MainW.h>
#include <Xm/PanedW.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>	// XmCreateWorkArea()
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

#if HAVE_ATHENA
#include <X11/Xaw/XawInit.h>
#endif

#include <X11/IntrinsicP.h>	// LessTif hacks
#include "Sash.h"
#include "LessTifH.h"

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif
#else // NOT IF_MOTIF
#include <glibmm/main.h>

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/dialog.h>
#include <gtkmm/paned.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/scale.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>

#include "GtkMultiPaned.h"
#endif // IF_MOTIF

// Lots of DDD stuff
#include "AgentM.h"
#include "AppData.h"
#include "ArgField.h"
#include "DataDisp.h"
#include "DeleteWCB.h"
#include "DestroyCB.h"
#include "DispGraph.h"
#include "DispGraph.h"
#include "DispBox.h"
#include "DispValue.h"
#include "ExitCB.h"
#include "GraphEdit.h"
#include "GDBAgent.h"
#include "HistoryD.h"
#ifdef IF_MOTIF
#include "LabelH.h"
#endif // IF_MOTIF
#include "MakeMenu.h"
#include "PlotAgent.h"
#include "SourceView.h"
#ifdef IF_MOTIF
#include "Swallower.h"
#include "TextSetS.h"
#endif // IF_MOTIF
#include "TimeOut.h"
#include "UndoBuffer.h"
#include "VSEFlags.h"
#include "VSLLib.h"
#include "XErrorB.h"
#include "args.h"
#include "assert.h"
#include "basename.h"
#include "bool.h"
#include "buttons.h"
#include "charsets.h"
#include "cmdtty.h"
#include "comm-manag.h"
#include "Command.h"
#include "complete.h"
#ifdef IF_MOTIF
#include "converters.h"
#endif // IF_MOTIF
#include "cook.h"
#include "data.h"
#include "dbx-lookup.h"
#include "deref.h"
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
#ifdef IF_MOTIF
#include "fonts.h"
#endif // IF_MOTIF
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
#include "root.h"
#include "sashes.h"
#include "select.h"
#include "selection.h"
#include "session.h"
#include "settings.h"
#include "shell.h"
#include "shorten.h"
#include "show.h"
#ifdef IF_MOTIF
#include "simpleMenu.h"
#endif // IF_MOTIF
#include "source.h"
#include "status.h"
#include "strclass.h"
#include "string-fun.h"
#include "tabs.h"
#include "tempfile.h"
#include "tictactoe.h"
#ifdef IF_MOTIF
#include "tips.h"
#endif // IF_MOTIF
#include "toolbar.h"
#include "ungrab.h"
#include "verify.h"
#include "version.h"
#include "windows.h"
#include "wm.h"
#include "xconfig.h"

// Standard stuff
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <signal.h>

#if HAVE_LOCALE_H
#include <locale.h>
#endif

#include <limits.h>
#ifndef ARG_MAX
#define ARG_MAX 4096
#endif

#if HAVE_EXCEPTIONS && HAVE_EXCEPTION
#include <exception>
#endif

#if WITH_READLINE
extern "C" {
#include "readline/readline.h"
}
#endif


enum DDDWindow { OtherWindow = -1,
		 ToolWindow  = 0, 
		 ExecWindow, 
		 Sep,
		 GDBWindow, 
		 SourceWindow,
		 DataWindow, 
		 CodeWindow,
		 CommonWindow };

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

#ifdef IF_MOTIF
// Warning proc
static void ddd_xt_warning(String message);
#endif // IF_MOTIF

// Getting ready
#ifdef IF_MOTIF
static void WhenReady            (Widget, XtPointer, XtPointer);
#else // NOT IF_MOTIF
static void WhenReady            (Widget, XtPointer);
#endif // IF_MOTIF

// Cut and Paste
static void gdbCutSelectionCB    (CB_ALIST_12(Widget, XtP(DDDWindow)));
static void gdbCopySelectionCB   (CB_ALIST_12(Widget, XtP(DDDWindow)));
static void gdbSelectAllCB       (CB_ALIST_12(Widget, XtP(DDDWindow)));
static void gdbPasteClipboardCB  (CB_ALIST_12(Widget, XtP(DDDWindow)));
static void gdbDeleteSelectionCB (CB_ALIST_12(Widget, XtP(DDDWindow)));
static void gdbClearAllCB        (CB_ALIST_NULL);
static void gdbUnselectAllCB     (CB_ALIST_NULL);

// Update menus
static void gdbUpdateEditCB      (CB_ALIST_12(Widget, XtP(DDDWindow)));
static void gdbUpdateFileCB      (CB_ARG_LIST_2());
static void gdbUpdateViewCB      (CB_ARG_LIST_2());
static void gdbUpdateViewsCB     (CB_ARG_LIST_2());
static void gdbUpdateAllMenus();

// Preferences
static void make_preferences (Widget parent);
static void dddPopupPreferencesCB (CB_ALIST_NULL);

// Synchronize `():' fields
static void sync_args(ArgField *a1, ArgField *a2);

// Return true if user interaction events are pending
static bool pending_interaction();

// Create status line
static void create_status(CONTAINER_P parent);

// Status LED
static void blink(bool set);
static void ToggleBlinkCB(TOGGLEBUTTON_P w,
#ifdef IF_MOTIF
			  XtPointer client_data,
#endif
			  XtPointer call_data);
static void DisableBlinkHP(Agent *, void *, void *);

// GDB Status
static void ReportStartHP(Agent *, void *, void *);
static void ReportDeathHP(Agent *, void *, void *);

// Status history
#ifdef IF_MOTIF
static void PopupStatusHistoryCB(CB_ALIST_NULL);
static void PopdownStatusHistoryCB(CB_ALIST_NULL);
static void PopdownStatusHistoryEH(Widget, XtPointer, XEvent *, Boolean *);
#else // NOT IF_MOTIF
static bool PopupStatusHistoryCB(GdkEventButton *, Widget);
static bool PopdownStatusHistoryCB(GdkEventButton *);
#endif // IF_MOTIF

// Argument callback
static void ActivateCB(Widget, XtPointer client_data, XtPointer call_data);

// Verify whether buttons are active
static void verify_buttons(const MMDesc *items);

// Setup shortcut menu
static void set_shortcut_menu(DataDisp *data_disp, const string& expressions);
static void set_shortcut_menu(DataDisp *data_disp);

// Fix the size of the status line
static void fix_status_size();

// Setup new shell
static void setup_new_shell(Widget w);

// Setup theme manager
static void setup_theme_manager();

// Set `Settings' title
static void set_settings_title(Widget w);

// Set Cut/Copy/Paste bindings for MENU to STYLE
static void set_cut_copy_paste_bindings(MMDesc *menu, BindingStyle style);

// Set Select All bindings for MENU to STYLE
static void set_select_all_bindings(MMDesc *menu, BindingStyle style);

// Popup DDD splash screen upon start-up.
#ifdef IF_MOTIF
static void SetSplashScreenCB(CB_ARG_LIST_3());
#else // NOT IF_MOTIF
static void SetSplashScreenCB(TOGGLEBUTTON_P);
#endif // IF_MOTIF
static void popup_splash_screen(Widget parent, const string& color_key);
static void popdown_splash_screen(XtPointer data = 0, XtIntervalId *id = 0);

// Read in database from FILENAME.  Upon version mismatch, ignore some
// resources such as window sizes.
static XrmDatabase GetFileDatabase(const string &filename);

// Lock `~/.ddd'
static bool lock_ddd(Widget parent, LockInfo& info);

// Various setups
static void setup_version_info();
static void setup_environment();
static void setup_options(int& argc, const char *argv[],
			  StringArray& saved_options, string& gdb_name,
			  bool& no_windows);
static void setup_command_tty();
static void setup_ddd_version_warnings();
static void setup_motif_version_warnings();
static void setup_auto_command_prefix();
static void setup_core_limit();
static void setup_options();
static void setup_cut_copy_paste_bindings(XrmDatabase db);
static void setup_select_all_bindings(XrmDatabase db);
static void setup_show(XrmDatabase db, const char *app_name, const char *gdb_name);

// Help hooks
static void PreHelpOnContext(Widget w, XtPointer, XtPointer);
static void PostHelpOnItem(Widget item);

// Log player stuff
static void check_log(const string& logname, DebuggerType& type);

#ifdef IF_MOTIF
// Add current selection as argument
static void add_arg_from_selection(Widget toplevel, 
				   int& argc, const char **&argv);
#endif // IF_MOTIF

#if XmVersion < 2000 || defined(LESSTIF_VERSION)
static void toggleOverstrikeAct (Widget, XEvent*, String*, Cardinal*)
{
    // Do nothing.  (Just a dummy.)
}
#endif

static void vsl_echo(const string& msg);

//-----------------------------------------------------------------------------
// Xt Stuff
//-----------------------------------------------------------------------------

// By this point some systems, notably SunOS 4.1.4 with IXI Motif 2.0.0
// under gcc 2.95.2, have defined ON as 1 and OFF as 0, thus rendering
// the static char definitions below invalid.
//
// 08-Feb-00 - Andrew Meredith <andrew@anvil.org>
//
#ifdef ON
#undef ON
#endif

#ifdef OFF
#undef OFF
#endif

static const char * ON  = "on";
static const char * OFF = "off";

#ifdef IF_MOTIF
// Options
// Note: we support both the GDB '--OPTION' and the X '-OPTION' convention.
static XrmOptionDescRec options[] = {
{ XRMOPTSTR("--session"),              XRMOPTSTR(XtNsession),              
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-session"),               XRMOPTSTR(XtNsession),              
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("-xtsessionID"),           XRMOPTSTR(XtNsession),              
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--debugger"),             XRMOPTSTR(XtNdebuggerCommand),      
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-debugger"),              XRMOPTSTR(XtNdebuggerCommand),      
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--automatic-debugger"),   XRMOPTSTR(XtNautoDebugger),         
                                        XrmoptionNoArg,  XPointer(ON) },
{ XRMOPTSTR("-automatic-debugger"),    XRMOPTSTR(XtNautoDebugger),         
                                        XrmoptionNoArg,  XPointer(ON) },

{ XRMOPTSTR("--bash"),                 XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("bash") },
{ XRMOPTSTR("-bash"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("bash") },

{ XRMOPTSTR("--dbg"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("dbg") },
{ XRMOPTSTR("-dbg"),                   XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("dbg") },

{ XRMOPTSTR("--dbx"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("dbx") },
{ XRMOPTSTR("-dbx"),                   XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("dbx") },

{ XRMOPTSTR("--gdb"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("gdb") },
{ XRMOPTSTR("-gdb"),                   XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("gdb") },

{ XRMOPTSTR("--ladebug"),              XRMOPTSTR(XtNdebugger),            
                                        XrmoptionNoArg, XPointer("ladebug") },
{ XRMOPTSTR("-ladebug"), 		    XRMOPTSTR(XtNdebugger),            
                                        XrmoptionNoArg, XPointer("ladebug") },

{ XRMOPTSTR("--jdb"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("jdb") },
{ XRMOPTSTR("-jdb"),                   XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("jdb") },

{ XRMOPTSTR("--perl"),                 XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("perl") },
{ XRMOPTSTR("-perl"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("perl") },

{ XRMOPTSTR("--pydb"),                 XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("pydb") },
{ XRMOPTSTR("-pydb"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("pydb") },

{ XRMOPTSTR("--wdb"),                  XRMOPTSTR(XtNdebugger),            
                                        XrmoptionNoArg, XPointer("wdb") },
{ XRMOPTSTR("-wdb"), 		    XRMOPTSTR(XtNdebugger),            
                                        XrmoptionNoArg, XPointer("wdb") },

{ XRMOPTSTR("--xdb"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("xdb") },
{ XRMOPTSTR("-xdb"),                   XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("xdb") },

{ XRMOPTSTR("--trace"),                XRMOPTSTR(XtNtrace),                
                                        XrmoptionNoArg,  XPointer(ON) },
{ XRMOPTSTR("-trace"),                 XRMOPTSTR(XtNtrace),                
                                        XrmoptionNoArg,  XPointer(ON) },

{ XRMOPTSTR("--play-log"),		   XRMOPTSTR(XtNplayLog),              
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-play-log"),		   XRMOPTSTR(XtNplayLog),              
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--font"),                 XRMOPTSTR(XtNdefaultFont),          
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-font"),                  XRMOPTSTR(XtNdefaultFont),          
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-fn"),                    XRMOPTSTR(XtNdefaultFont),          
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--fontsize"),             XRMOPTSTR(XtCFontSize),             
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-fontsize"),              XRMOPTSTR(XtCFontSize),             
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--vsl-library"),          XRMOPTSTR(XtNvslLibrary),           
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-vsl-library"),           XRMOPTSTR(XtNvslLibrary),           
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--vsl-path"),             XRMOPTSTR(XtNvslPath),              
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-vsl-path"),              XRMOPTSTR(XtNvslPath),              
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--separate"),             XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-separate"),              XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("--separate-windows"),     XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-separate-windows"),      XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--separate-source-window"), XRMOPTSTR(XtNseparateSourceWindow), 
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-separate-source-window"),  XRMOPTSTR(XtNseparateSourceWindow), 
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--separate-data-window"), XRMOPTSTR(XtNseparateDataWindow),   
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-separate-data-window"),  XRMOPTSTR(XtNseparateDataWindow),   
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--attach"),               XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-attach"),                XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("--attach-windows"),       XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-attach-windows"),        XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--attach-source-window"), XRMOPTSTR(XtNseparateSourceWindow), 
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-attach-source-window"),  XRMOPTSTR(XtNseparateSourceWindow), 
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--attach-data-window"),   XRMOPTSTR(XtNseparateDataWindow),   
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-attach-data-window"),    XRMOPTSTR(XtNseparateDataWindow),   
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--exec-window"),          XRMOPTSTR(XtNseparateExecWindow),   
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-exec-window"),           XRMOPTSTR(XtNseparateExecWindow),   
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-exec-window"),       XRMOPTSTR(XtNseparateExecWindow),   
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-exec-window"),        XRMOPTSTR(XtNseparateExecWindow),   
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--source-window"),        XRMOPTSTR(XtNopenSourceWindow),     
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-source-window"),         XRMOPTSTR(XtNopenSourceWindow),     
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-source-window"),     XRMOPTSTR(XtNopenSourceWindow),     
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-source-window"),      XRMOPTSTR(XtNopenSourceWindow),     
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--data-window"),          XRMOPTSTR(XtNopenDataWindow),       
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-data-window"),           XRMOPTSTR(XtNopenDataWindow),       
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-data-window"),       XRMOPTSTR(XtNopenDataWindow),       
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-data-window"),        XRMOPTSTR(XtNopenDataWindow),       
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--debugger-console"),     XRMOPTSTR(XtNopenDebuggerConsole),  
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-debugger-console"),      XRMOPTSTR(XtNopenDebuggerConsole),  
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-debugger-console"),  XRMOPTSTR(XtNopenDebuggerConsole),  
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-debugger-console"),   XRMOPTSTR(XtNopenDebuggerConsole),  
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--button-tips"),          XRMOPTSTR(XtNbuttonTips),           
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-button-tips"),           XRMOPTSTR(XtNbuttonTips),           
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-button-tips"),       XRMOPTSTR(XtNbuttonTips),           
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-button-tips"),        XRMOPTSTR(XtNbuttonTips),           
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--value-tips"),           XRMOPTSTR(XtNvalueTips),            
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-value-tips"),            XRMOPTSTR(XtNvalueTips),            
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-value-tips"),        XRMOPTSTR(XtNvalueTips),            
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-value-tips"),         XRMOPTSTR(XtNvalueTips),            
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--status-at-bottom"),     XRMOPTSTR(XtNstatusAtBottom),       
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-status-at-bottom"),      XRMOPTSTR(XtNstatusAtBottom),       
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--status-at-top"),        XRMOPTSTR(XtNstatusAtBottom),       
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-status-at-top"),         XRMOPTSTR(XtNstatusAtBottom),       
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--toolbars-at-bottom"),   XRMOPTSTR(XtNtoolbarsAtBottom),     
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-toolbars-at-bottom"),    XRMOPTSTR(XtNtoolbarsAtBottom),     
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--toolbars-at-top"),      XRMOPTSTR(XtNtoolbarsAtBottom),     
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-toolbars-at-top"),       XRMOPTSTR(XtNtoolbarsAtBottom),     
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--panned-graph-editor"),  XRMOPTSTR(XtNpannedGraphEditor),    
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-panned-graph-editor"),   XRMOPTSTR(XtNpannedGraphEditor),    
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--scrolled-graph-editor"), XRMOPTSTR(XtNpannedGraphEditor),   
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-scrolled-graph-editor"), XRMOPTSTR(XtNpannedGraphEditor),    
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--sync-debugger"),        XRMOPTSTR(XtNsynchronousDebugger),  
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-sync-debugger"),         XRMOPTSTR(XtNsynchronousDebugger),  
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--disassemble"),          XRMOPTSTR(XtNdisassemble),          
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-disassemble"),           XRMOPTSTR(XtNdisassemble),          
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-disassemble"),       XRMOPTSTR(XtNdisassemble),          
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-disassemble"),        XRMOPTSTR(XtNdisassemble),          
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--glyphs"),               XRMOPTSTR(XtNdisplayGlyphs),        
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-glyphs"),                XRMOPTSTR(XtNdisplayGlyphs),        
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-glyphs"),            XRMOPTSTR(XtNdisplayGlyphs),        
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-glyphs"),             XRMOPTSTR(XtNdisplayGlyphs),        
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--host"),                 XRMOPTSTR(XtNdebuggerHost),         
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-host"),                  XRMOPTSTR(XtNdebuggerHost),         
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--rhost"),                XRMOPTSTR(XtNdebuggerRHost),        
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-rhost"),                 XRMOPTSTR(XtNdebuggerRHost),        
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--login"),                XRMOPTSTR(XtNdebuggerHostLogin),    
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-login"),                 XRMOPTSTR(XtNdebuggerHostLogin),    
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-l"),                     XRMOPTSTR(XtNdebuggerHostLogin),    
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--tty"),                  XRMOPTSTR(XtNttyMode),              
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-tty"),                   XRMOPTSTR(XtNttyMode),              
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-t"),                     XRMOPTSTR(XtNttyMode),              
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--fullname"),             XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("1") },
{ XRMOPTSTR("-fullname"),              XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("1") },
{ XRMOPTSTR("-f"),                     XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("1") },

{ XRMOPTSTR("--annotate"),             XRMOPTSTR(XtNannotate),             
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-annotate"),              XRMOPTSTR(XtNannotate),             
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--annotate=0"),           XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("0") },
{ XRMOPTSTR("-annotate=0"),            XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("0") },

{ XRMOPTSTR("--annotate=1"),           XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("1") },
{ XRMOPTSTR("-annotate=1"),            XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("1") },

{ XRMOPTSTR("--annotate=2"),           XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("2") },
{ XRMOPTSTR("-annotate=2"),            XRMOPTSTR(XtNannotate),             
                                        XrmoptionNoArg, XPointer("2") },

{ XRMOPTSTR("--version"),              XRMOPTSTR(XtNshowVersion),          
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-version"),               XRMOPTSTR(XtNshowVersion),          
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-v"),                     XRMOPTSTR(XtNshowVersion),          
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--configuration"),        XRMOPTSTR(XtNshowConfiguration),    
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-configuration"),         XRMOPTSTR(XtNshowConfiguration),    
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--manual"),               XRMOPTSTR(XtNshowManual),           
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-manual"),                XRMOPTSTR(XtNshowManual),           
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--maintenance"),          XRMOPTSTR(XtNmaintenance),          
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-maintenance"),           XRMOPTSTR(XtNmaintenance),          
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--no-maintenance"),       XRMOPTSTR(XtNmaintenance),          
                                        XrmoptionNoArg, XPointer(OFF) },
{ XRMOPTSTR("-no-maintenance"),        XRMOPTSTR(XtNmaintenance),          
                                        XrmoptionNoArg, XPointer(OFF) },

{ XRMOPTSTR("--license"),              XRMOPTSTR(XtNshowLicense),          
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-license"),               XRMOPTSTR(XtNshowLicense),          
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--news"),                 XRMOPTSTR(XtNshowNews),             
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-news"),                  XRMOPTSTR(XtNshowNews),             
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--fonts"),                XRMOPTSTR(XtNshowFonts),            
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-fonts"),                 XRMOPTSTR(XtNshowFonts),            
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--check-configuration"),  XRMOPTSTR(XtNcheckConfiguration),   
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-check-configuration"),   XRMOPTSTR(XtNcheckConfiguration),   
                                        XrmoptionNoArg, XPointer(ON) },

{ XRMOPTSTR("--lesstif-hacks"),        XRMOPTSTR(XtNlessTifVersion),       
                                        XrmoptionNoArg, XPointer("999") },
{ XRMOPTSTR("-lesstif-hacks"),         XRMOPTSTR(XtNlessTifVersion),       
                                        XrmoptionNoArg, XPointer("999") },

{ XRMOPTSTR("--no-lesstif-hacks"),     XRMOPTSTR(XtNlessTifVersion),       
                                        XrmoptionNoArg, XPointer("1000") },
{ XRMOPTSTR("-no-lesstif-hacks"),      XRMOPTSTR(XtNlessTifVersion),       
                                        XrmoptionNoArg, XPointer("1000") },

{ XRMOPTSTR("--lesstif-version"),      XRMOPTSTR(XtNlessTifVersion),       
                                        XrmoptionSepArg, XPointer(0) },
{ XRMOPTSTR("-lesstif-version"),       XRMOPTSTR(XtNlessTifVersion),       
                                        XrmoptionSepArg, XPointer(0) },

{ XRMOPTSTR("--help"),                 XRMOPTSTR(XtNshowInvocation),       
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-help"),                  XRMOPTSTR(XtNshowInvocation),       
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-h"),                     XRMOPTSTR(XtNshowInvocation),       
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("--?"),                    XRMOPTSTR(XtNshowInvocation),       
                                        XrmoptionNoArg, XPointer(ON) },
{ XRMOPTSTR("-?"),                     XRMOPTSTR(XtNshowInvocation),
                                        XrmoptionNoArg, XPointer(ON) },

};
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Rewrite options using popt.
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Actions
static XtActionsRec actions [] = {
    {XTARECSTR("gdb-control"),            PTR_FUN(controlAct)},
    {XTARECSTR("gdb-interrupt"),          PTR_FUN(interruptAct)},
    {XTARECSTR("gdb-command"),            PTR_FUN(commandAct)},
    {XTARECSTR("gdb-process"),            PTR_FUN(processAct)},
    {XTARECSTR("gdb-delete-or-control"),  PTR_FUN(delete_or_controlAct)},
    {XTARECSTR("gdb-prev-history"),       PTR_FUN(prev_historyAct)},
    {XTARECSTR("gdb-previous-history"),   PTR_FUN(prev_historyAct)},
    {XTARECSTR("gdb-next-history"),       PTR_FUN(next_historyAct)},
    {XTARECSTR("gdb-beginning-of-line"),  PTR_FUN(beginning_of_lineAct)},
    {XTARECSTR("gdb-end-of-line"),        PTR_FUN(end_of_lineAct)},
    {XTARECSTR("gdb-forward-character"),  PTR_FUN(forward_characterAct)},
    {XTARECSTR("gdb-backward-character"), PTR_FUN(backward_characterAct)},
    {XTARECSTR("gdb-set-line"),           PTR_FUN(set_lineAct)},
    {XTARECSTR("gdb-complete-command"),   PTR_FUN(complete_commandAct)},
    {XTARECSTR("gdb-complete-arg"),       PTR_FUN(complete_argAct)},
    {XTARECSTR("gdb-complete-tab"),       PTR_FUN(complete_tabAct)},
    {XTARECSTR("gdb-insert-source-arg"),  PTR_FUN(insert_source_argAct)},
    {XTARECSTR("gdb-insert-graph-arg"),   PTR_FUN(insert_graph_argAct)},
    {XTARECSTR("gdb-isearch-prev"),       PTR_FUN(isearch_prevAct)},
    {XTARECSTR("gdb-isearch-next"),       PTR_FUN(isearch_nextAct)},
    {XTARECSTR("gdb-isearch-exit"),       PTR_FUN(isearch_exitAct)},
    {XTARECSTR("gdb-popup-menu"),         PTR_FUN(popupAct)},
    {XTARECSTR("ddd-next-tab-group"),     PTR_FUN(next_tab_groupAct)},
    {XTARECSTR("ddd-prev-tab-group"),     PTR_FUN(prev_tab_groupAct)},
    {XTARECSTR("ddd-previous-tab-group"), PTR_FUN(prev_tab_groupAct)},
    {XTARECSTR("ddd-get-focus"),          PTR_FUN(get_focusAct)},
    {XTARECSTR("ddd-select-all"),         PTR_FUN(select_allAct)},
#if XmVersion < 2000 || defined(LESSTIF_VERSION)
    {XTARECSTR("toggle-overstrike"),      PTR_FUN(toggleOverstrikeAct)},
#endif
};
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Actions not supported
#endif
#endif // IF_MOTIF

//-----------------------------------------------------------------------------
// Menus
//-----------------------------------------------------------------------------

#define RECENT_MENU \
{ \
    { NM("r1", "r1"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(1) ), 0, 0, 0, 0 }, \
    { NM("r2", "r2"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(2) ), 0, 0, 0, 0 }, \
    { NM("r3", "r3"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(3) ), 0, 0, 0, 0 }, \
    { NM("r4", "r4"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(4) ), 0, 0, 0, 0 }, \
    { NM("r5", "r5"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(5) ), 0, 0, 0, 0 }, \
    { NM("r6", "r6"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(6) ), 0, 0, 0, 0 }, \
    { NM("r7", "r7"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(7) ), 0, 0, 0, 0 }, \
    { NM("r8", "r8"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(8) ), 0, 0, 0, 0 }, \
    { NM("r9", "r9"), MMPush, BIND_1( PTR_FUN(gdbOpenRecentCB), XtPointer(9) ), 0, 0, 0, 0 }, \
    MMEnd					\
}

static MMDesc command_recent_menu[] = RECENT_MENU;
static MMDesc source_recent_menu[]  = RECENT_MENU;
static MMDesc data_recent_menu[]    = RECENT_MENU;

struct FileItems {
    enum FileItem { OpenClass, OpenFile, Recent, OpenCore, OpenSource, Sep1,
		    OpenSession, SaveSession, Sep2,
		    Attach, Detach, Sep3,
		    Print, PrintAgain, Sep4,
		    CD, Sep5,
		    Make, MakeAgain, Sep6,
		    Close, Restart, Exit
    };
};

#ifdef IF_MOTIF

// Auxiliary struct used by WhenReady. A pointer to function can not
// be passed directly in a XtPointer.
struct WhenReadyProc_t {
  XtCallbackProc proc;
};
#define DECL_WR(NAME, PROC)			\
    static WhenReadyProc_t NAME = { PROC }

#else // NOT IF_MOTIF

// Bind everything except the widget argument.  The widget will be
// filled in when we queue the event for processing.
typedef GTK_SLOT_W WhenReadyProc_t;
#define DECL_WR(NAME, PROC)			\
    static WhenReadyProc_t NAME = PROC

#endif // IF_MOTIF

DECL_WR(WR_gdbOpenClassCB, PTR_FUN(gdbOpenClassCB));
DECL_WR(WR_gdbOpenFileCB, PTR_FUN(gdbOpenFileCB));
DECL_WR(WR_gdbOpenCoreCB, PTR_FUN(gdbOpenCoreCB));
DECL_WR(WR_OpenSessionCB, PTR_FUN(OpenSessionCB));
DECL_WR(WR_SaveSessionAsCB, PTR_FUN(SaveSessionAsCB));
DECL_WR(WR_gdbOpenProcessCB, PTR_FUN(gdbOpenProcessCB));
DECL_WR(WR_gdbChangeDirectoryCB, PTR_FUN(gdbChangeDirectoryCB));
DECL_WR(WR_gdbMakeCB, PTR_FUN(gdbMakeCB));
DECL_WR(WR_gdbMakeAgainCB, CB_ARG_HIDE_0(PTR_FUN(gdbMakeAgainCB)));

#define FILE_MENU(recent_menu)						\
{									\
    { NM("open_class", "Open Class..."), MMPush | MMUnmanaged,		\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_gdbOpenClassCB)), 0, 0, 0, 0 }, \
    { NM("open_file", "Open Program..."), MMPush,			\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_gdbOpenFileCB)), 0, 0, 0, 0 }, \
    { NM("recent", "Open Recent"), MMMenu, MMNoCB, recent_menu, 0, 0, 0 }, \
    { NM("open_core", "Open Core Dump..."), MMPush,			\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_gdbOpenCoreCB)), 0, 0, 0, 0}, \
    { NM("open_source", "Open Source..."), MMPush, 			\
      BIND_0(PTR_FUN(gdbLookupSourceCB)), 0, 0, 0, 0 }, 		\
    MMSep,								\
    { NM("open_session", "Open Session..."),  MMPush,			\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_OpenSessionCB)), 0, 0, 0, 0 }, \
    { NM("save_session", "Save Session As..."), MMPush,		\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_SaveSessionAsCB)), 0, 0, 0, 0 },	\
    MMSep,								\
    { NM("attach", "Attach to Process..."), MMPush,			\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_gdbOpenProcessCB)), 0, 0, 0, 0 }, \
    { NM("detach", "Detach Process"), MMPush,				\
      BIND_1(PTR_FUN(gdbCommandCB), XtPointer("detach")), 0, 0, 0, 0 },	\
    MMSep,								\
    { NM("print", "Print Graph..."), MMPush,				\
      BIND_0(PTR_FUN(PrintGraphCB)), 0, 0, 0, 0 },			\
    { NM("printAgain", "Print Again"), MMPush | MMUnmanaged,		\
      BIND_1(PTR_FUN(PrintAgainCB), 1), 0, 0, 0, 0 },			\
    { NM("separator", ""), MMSeparator | MMUnmanaged, MMNoCB, 0, 0, 0, 0 }, \
    { NM("cd", "Change Directory..."), MMPush,				\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_gdbChangeDirectoryCB)), 0, 0, 0, 0 }, \
    MMSep,								\
    { NM("make", "Make..."), MMPush,					\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_gdbMakeCB)), 0, 0, 0, 0 }, \
    { NM("makeAgain", "Make Again"), MMPush | MMUnmanaged,		\
      BIND_1(PTR_FUN(WhenReady), XtPointer(&WR_gdbMakeAgainCB)), 0, 0, 0, 0 }, \
    MMSep,								\
    { NM("close", "Close"), MMPush, BIND_0(PTR_FUN(DDDCloseCB)), 0, 0, 0, 0 }, \
    { NM("restart", "Restart"), MMPush, BIND_0(PTR_FUN(DDDRestartCB)), 0, 0, 0, 0 }, \
    { NM("exit", "Exit"), MMPush,					\
      BIND_1(PTR_FUN(DDDExitCB), EXIT_SUCCESS), 0, 0, 0, 0 },		\
    MMEnd								\
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

#define PROGRAM_MENU(w)							\
{									\
    { NM("run", "Run..."), MMPush, BIND_0(PTR_FUN(gdbRunCB)), 0, 0, 0, 0 },	\
    { NM("run_again", "Run Again"), MMPush,				\
      BIND_1(PTR_FUN(gdbCommandCB), "run"), 0, 0, 0, 0 },		\
    MMSep,								\
    { NM("separateExecWindow", "Run in Execution Window"), MMToggle,	\
      BIND_0(PTR_FUN(dddToggleSeparateExecWindowCB)), 0, (Widget *)&(w), 0, 0 }, \
    MMSep,								\
    { NM("step", "Step"), MMPush,					\
      BIND_1(PTR_FUN(gdbCommandCB), "step"), 0, 0, 0, 0 },		\
    { NM("stepi", "Step Instruction"), MMPush,				\
      BIND_1(PTR_FUN(gdbCommandCB), "stepi"), 0, 0, 0, 0 },		\
    { NM("next", "Next"), MMPush,					\
      BIND_1(PTR_FUN(gdbCommandCB), "next"), 0, 0, 0, 0 },		\
    { NM("nexti", "Next Instruction"), MMPush,				\
      BIND_1(PTR_FUN(gdbCommandCB), "nexti"), 0, 0, 0, 0},		\
    { NM("until", "Until"), MMPush,					\
      BIND_1(PTR_FUN(gdbCommandCB), "until"), 0, 0, 0, 0},		\
    { NM("finish", "Finish"), MMPush,					\
      BIND_1(PTR_FUN(gdbCommandCB), "finish"), 0, 0, 0, 0},		\
    MMSep,								\
    { NM("cont", "Continue"), MMPush,					\
      BIND_1(PTR_FUN(gdbCommandCB), "cont"), 0, 0, 0, 0},		\
    { NM("signal0", "Continue Without Signal"), MMPush,			\
      BIND_1(PTR_FUN(gdbCommandCB), "signal 0"), 0, 0, 0, 0},		\
    MMSep,								\
    { NM("kill", "Kill"), MMPush,					\
      BIND_1(PTR_FUN(gdbCommandCB), "kill"), 0, 0, 0, 0},		\
    { NM("break", "Interrupt"), MMPush,					\
      BIND_1(PTR_FUN(gdbCommandCB), "\003"), 0, 0, 0, 0},		\
    { NM("quit", "Abort"), MMPush,					\
      BIND_1(PTR_FUN(gdbCommandCB), "\034"), 0, 0, 0, 0},		\
    MMEnd								\
}

#ifdef IF_MOTIF
static Widget command_separate_exec_window_w;
static Widget source_separate_exec_window_w;
static Widget data_separate_exec_window_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *command_separate_exec_window_w;
static Gtk::ToggleButton *source_separate_exec_window_w;
static Gtk::ToggleButton *data_separate_exec_window_w;
#endif // IF_MOTIF

static MMDesc command_program_menu[]
    = PROGRAM_MENU(command_separate_exec_window_w);
static MMDesc source_program_menu[]
    = PROGRAM_MENU(source_separate_exec_window_w);
static MMDesc data_program_menu[]
    = PROGRAM_MENU(data_separate_exec_window_w);

#define VIEW_MENU							\
{									\
    { NM("tool", "Command Tool..."), MMPush,				\
      HIDE_0( PTR_FUN(gdbOpenToolWindowCB) ), 0, 0, 0, 0},		\
    { NM("exec", "Execution Window..."), MMPush,			\
      HIDE_0( PTR_FUN(gdbOpenExecWindowCB) ), 0, 0, 0, 0},		\
    MMSep,								\
    { NM("console", "@GDB@ Console"), MMPush,				\
      HIDE_0( PTR_FUN(gdbOpenCommandWindowCB) ), 0, 0, 0, 0},		\
    { NM("source", "Source Window"), MMPush,				\
      HIDE_0( PTR_FUN(gdbOpenSourceWindowCB) ), 0, 0, 0, 0},		\
    { NM("data", "Data Window"),					\
      MMPush,   HIDE_0( PTR_FUN(gdbOpenDataWindowCB) ), 0, 0, 0, 0},	\
    { NM("code", "Machine Code Window"), MMToggle | MMUnmanaged,	\
        BIND_0( PTR_FUN(gdbToggleCodeWindowCB) ), 0, 0, 0, 0},		\
    MMEnd								\
}

static MMDesc command_view_menu[] = VIEW_MENU;
static MMDesc source_view_menu[]  = VIEW_MENU;
static MMDesc data_view_menu[]    = VIEW_MENU;

static MMDesc views_menu[] = 
{
    { NM("tool", "Command Tool..."), MMPush,
      HIDE_0( PTR_FUN(gdbOpenToolWindowCB) ), 0, 0, 0, 0},
    { NM("exec", "Execution Window..."), MMPush,
      HIDE_0( PTR_FUN(gdbOpenExecWindowCB) ), 0, 0, 0, 0},
    MMSep,
    { NM("console", "@GDB@ Console"), MMToggle,
      BIND_0( PTR_FUN(gdbToggleCommandWindowCB) ), 0, 0, 0, 0},
    { NM("source", "Source Window"), MMToggle,
      BIND_0( PTR_FUN(gdbToggleSourceWindowCB) ), 0, 0, 0, 0},
    { NM("data", "Data Window"), MMToggle,
      BIND_0( PTR_FUN(gdbToggleDataWindowCB) ), 0, 0, 0, 0},
    { NM("code", "Machine Code Window"), MMToggle,
      BIND_0( PTR_FUN(gdbToggleCodeWindowCB) ), 0, 0, 0, 0},
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

DECL_WR(WR_dddPopupSettingsCB, CB_ARG_HIDE_0(PTR_FUN(dddPopupSettingsCB)));

#define EDIT_MENU(win, w)						\
{									\
    { NM("undo", "Undo"),                  MMPush,			\
      HIDE_0( PTR_FUN(gdbUndoCB) ), 0, 0, 0, 0},			\
    { NM("redo", "Redo"),                  MMPush,			\
      HIDE_0( PTR_FUN(gdbRedoCB) ), 0, 0, 0, 0},			\
    MMSep,								\
    { NM("cut", "Cut"),                    MMPush,			\
      BIND_1(PTR_FUN(gdbCutSelectionCB),    win), 0, 0, 0, 0},		\
    { NM("copy", "Copy"),                  MMPush,			\
      BIND_1(PTR_FUN(gdbCopySelectionCB),   win), 0, 0, 0, 0},		\
    { NM("paste", "Paste"),                MMPush,			\
      BIND_1(PTR_FUN(gdbPasteClipboardCB),  win), 0, 0, 0, 0},		\
    { NM("clearAll", "Clear"),             MMPush,			\
      HIDE_0( PTR_FUN(gdbClearAllCB) ), 0, 0, 0, 0},			\
    { NM("delete", "Delete"),              MMPush,			\
      BIND_1( PTR_FUN(gdbDeleteSelectionCB), win), 0, 0, 0, 0},		\
    MMSep,								\
    { NM("selectAll", "Select All"),       MMPush,			\
      BIND_1( PTR_FUN(gdbSelectAllCB),       win), 0, 0, 0, 0},		\
    MMSep,								\
    { NM("preferences", "Preferences..."), MMPush,			\
      HIDE_0( PTR_FUN(dddPopupPreferencesCB) ), 0, 0, 0, 0},		\
    { NM("settings", "Settings..."),       MMPush,			\
      BIND_1( PTR_FUN(WhenReady), XtPointer(&WR_dddPopupSettingsCB) ), 0, 0, 0, 0}, \
    MMSep,								\
    { NM("saveOptions", "Save Options"),   MMToggle,			\
      BIND_0( PTR_FUN(dddToggleSaveOptionsOnExitCB) ), 0, (Widget *)&(w), 0, 0}, \
    MMEnd								\
}

#ifdef IF_MOTIF
static Widget command_save_options_w;
static Widget source_save_options_w;
static Widget data_save_options_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *command_save_options_w;
static Gtk::ToggleButton *source_save_options_w;
static Gtk::ToggleButton *data_save_options_w;
#endif // IF_MOTIF

static MMDesc command_edit_menu[] = EDIT_MENU(GDBWindow, 
					      command_save_options_w);
static MMDesc source_edit_menu[]  = EDIT_MENU(SourceWindow,
					      source_save_options_w);
static MMDesc data_edit_menu[]    = EDIT_MENU(DataWindow,
					      data_save_options_w);

static Widget complete_w;
static Widget define_w;

DECL_WR(WR_gdbCompleteCB, PTR_FUN(gdbCompleteCB));
DECL_WR(WR_gdbApplyCB, PTR_FUN(gdbApplyCB));

static MMDesc command_menu[] =
{
    { NM("history", "Command History..."),  MMPush,
      BIND_0( PTR_FUN(gdbHistoryCB) ), 0, 0, 0, 0},
    MMSep,
    { NM("prev", "Previous"),               MMPush,
      BIND_0( PTR_FUN(gdbPrevCB) ), 0, 0, 0, 0},
    { NM("next", "Next"),                   MMPush,
      BIND_0( PTR_FUN(gdbNextCB) ), 0, 0, 0, 0},
    MMSep,
    { NM("isearch_prev", "Find Backward"),  MMPush,
      BIND_0( PTR_FUN(gdbISearchPrevCB) ), 0, 0, 0, 0},
    { NM("isearch_next", "Find Forward"),   MMPush,
      BIND_0( PTR_FUN(gdbISearchNextCB) ), 0, 0, 0, 0},
    { NM("isearch_exit", "Quit Search"),    MMPush,
      BIND_0( PTR_FUN(gdbISearchExitCB) ), 0, 0, 0, 0},
    MMSep,
    { NM("complete", "Complete"),           MMPush,
      BIND_1( PTR_FUN(WhenReady), XtPointer(&WR_gdbCompleteCB) ), 
      0, &complete_w, 0, 0 },
    { NM("apply", "Apply"),                 MMPush,
      BIND_1( PTR_FUN(WhenReady), XtPointer(&WR_gdbApplyCB) ), 0, 0, 0, 0},
    MMSep,
    { NM("clear_line", "Clear Line"),       MMPush,
      HIDE_0( PTR_FUN(gdbClearCB) ), 0, 0, 0, 0},
    { NM("clear_window", "Clear Window"),   MMPush,
      HIDE_0( PTR_FUN(gdbClearWindowCB) ), 0, 0, 0, 0},
    MMSep,
    { NM("define", "Define Command..."),    MMPush,
      BIND_0( PTR_FUN(dddDefineCommandCB) ), 0, &define_w, 0, 0 },
    { NM("buttons", "Edit Buttons..."),     MMPush,
      BIND_0( PTR_FUN(dddEditButtonsCB) ), 0, 0, 0, 0},
    MMEnd
};

static Widget stack_w;
static Widget registers_w;
static Widget threads_w;
static Widget signals_w;

DECL_WR(WR_ViewStackFramesCB, CB_ARG_HIDE_0(PTR_FUN( SourceView::ViewStackFramesCB )));
DECL_WR(WR_ViewRegistersCB, CB_ARG_HIDE_0(PTR_FUN( SourceView::ViewRegistersCB )));
DECL_WR(WR_ViewThreadsCB, CB_ARG_HIDE_0(PTR_FUN( SourceView::ViewThreadsCB )));
DECL_WR(WR_dddPopupSignalsCB, CB_ARG_HIDE_0(PTR_FUN(dddPopupSignalsCB)));
static MMDesc stack_menu[] =
{
    { NM("stack", "Backtrace..."),      MMPush,
      BIND_1( PTR_FUN(WhenReady), XtPointer(&WR_ViewStackFramesCB) ),
      0, &stack_w, 0, 0 },
    { NM("registers", "Registers..."),  MMPush,
      BIND_1( PTR_FUN(WhenReady), XtPointer(&WR_ViewRegistersCB) ),
      0, &registers_w, 0, 0 },
    { NM("threads", "Threads..."),      MMPush,
      BIND_1( PTR_FUN(WhenReady), XtPointer(&WR_ViewThreadsCB) ),
      0, &threads_w, 0, 0 },
    { NM("signals", "Signals..."),      MMPush,
      BIND_1( PTR_FUN(WhenReady), XtPointer(&WR_dddPopupSignalsCB) ),
      0, &signals_w, 0, 0 },
    MMSep,
    { NM("up", "Up"),                   MMPush,
      BIND_1( PTR_FUN(gdbCommandCB), XtPointer("up") ), 0, 0, 0, 0},
    { NM("down", "Down"),               MMPush,
      BIND_1( PTR_FUN(gdbCommandCB), XtPointer("down") ), 0, 0, 0, 0},
    MMEnd
};

#ifdef IF_MOTIF
static Widget lookup_w;
static Widget find_forward_w;
static Widget find_backward_w;
static Widget find_words_only_w;
static Widget find_case_sensitive_w;
static Widget disassemble_w;
static Widget edit_source_w;
static Widget reload_source_w;
static Widget line_numbers1_w;
#else // NOT IF_MOTIF
static Widget lookup_w;
static Widget find_forward_w;
static Widget find_backward_w;
static Gtk::ToggleButton *find_words_only_w;
static Gtk::ToggleButton *find_case_sensitive_w;
static Gtk::ToggleButton *disassemble_w;
static Widget edit_source_w;
static Widget reload_source_w;
static Gtk::ToggleButton *line_numbers1_w;
#endif // IF_MOTIF

static MMDesc source_menu[] =
{
    { NM("breakpoints", "Breakpoints..."), MMPush,
      HIDE_0(PTR_FUN(SourceView::EditBreakpointsCB)), 0, 0, 0, 0},
    MMSep,
    { NM("lookup", "Lookup"),                                    MMPush,
      HIDE_0(PTR_FUN(gdbLookupCB)), 0, (Widget *)&lookup_w, 0, 0 },
    { NM("findForward", "@charset Find@small>> @charset()@rm"),  MMPush,
      BIND_1(PTR_FUN(gdbFindCB), SourceView::forward),
      0, (Widget *)&find_forward_w, 0, 0 },
    { NM("findBackward", "@charset Find@small<< @charset()@rm"), MMPush,
      BIND_1(PTR_FUN(gdbFindCB), SourceView::backward),
      0, (Widget *)&find_backward_w, 0, 0 },
    MMSep,
    { NM("findWordsOnly", "Find Words Only"),                    MMToggle,
      BIND_0(PTR_FUN(sourceToggleFindWordsOnlyCB)), 
      0, (Widget *)&find_words_only_w, 0, 0 },
    { NM("findCaseSensitive", "Find Case Sensitive"),            MMToggle,
      BIND_0(PTR_FUN(sourceToggleFindCaseSensitiveCB)), 
      0, (Widget *)&find_case_sensitive_w, 0, 0 },
    MMSep,
    { NM("lineNumbers", "Display Line Numbers"),                 MMToggle,
      BIND_0(PTR_FUN(sourceToggleDisplayLineNumbersCB)),
      0, (Widget *)&line_numbers1_w, 0, 0 },
    { NM("disassemble", "Display Machine Code"),                 MMToggle,
      BIND_0(PTR_FUN(gdbToggleCodeWindowCB)),
      0, (Widget *)&disassemble_w, 0, 0 },
    MMSep,
    { NM("edit", "Edit Source..."),                              MMPush,  
      BIND_0(PTR_FUN(gdbEditSourceCB)), 0, (Widget *)&edit_source_w, 0, 0 },
    { NM("reload", "Reload Source"),                             MMPush,  
      HIDE_0(PTR_FUN(gdbReloadSourceCB)), 0, (Widget *)&reload_source_w, 0, 0},
    MMEnd
};


// Preferences

// General preferences

#ifdef IF_MOTIF
static Widget button_tips_w;
static Widget button_docs_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *button_tips_w;
static Gtk::ToggleButton *button_docs_w;
#endif // IF_MOTIF

static MMDesc button_menu [] = 
{
    { NM("tips", "as Popup Tips"),       MMToggle,
      BIND_0( PTR_FUN(dddToggleButtonTipsCB) ),
      0, (Widget *)&button_tips_w, 0,0},
    { NM("docs", "in the Status Line"),  MMToggle,
      BIND_0( PTR_FUN(dddToggleButtonDocsCB) ),
      0, (Widget *)&button_docs_w, 0,0},
    MMEnd
};

#ifdef IF_MOTIF
static Widget value_tips_w;
static Widget value_docs_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *value_tips_w;
static Gtk::ToggleButton *value_docs_w;
#endif // IF_MOTIF

static MMDesc value_menu [] = 
{
    { NM("tips", "as Popup Tips"),       MMToggle,
      BIND_0( PTR_FUN(dddToggleValueTipsCB) ),
      0, (Widget *)&value_tips_w, 0, 0 },
    { NM("docs", "in the Status Line"),  MMToggle,
      BIND_0( PTR_FUN(dddToggleValueDocsCB) ),
      0, (Widget *)&value_docs_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget set_global_completion_w;
static Widget set_console_completion_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_global_completion_w;
static Gtk::ToggleButton *set_console_completion_w;
#endif // IF_MOTIF

static MMDesc completion_menu [] = 
{
    { NM("inAllWindows", "in All Windows"),  MMRadio, 
      BIND_1(PTR_FUN(dddSetGlobalTabCompletionCB), True), 
      0, (Widget *)&set_global_completion_w, 0, 0 },
    { NM("inConsole", "in Console Only"),    MMRadio,
      BIND_1(PTR_FUN(dddSetGlobalTabCompletionCB), False),
      0, (Widget *)&set_console_completion_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget max_undo_size_w;
#else // NOT IF_MOTIF
static Gtk::Entry *max_undo_size_w;
#endif // IF_MOTIF

static MMDesc undo_menu [] =
{
    { NM("size", "Undo Buffer Size"),    MMTextField | MMUnmanagedLabel,
      BIND_0(PTR_FUN(dddSetUndoBufferSizeCB)), 
      0, (Widget *)&max_undo_size_w, 0, 0 },
    { NM("kbytes", "kBytes"),            MMLabel,
      MMNoCB, 0, 0, 0, 0 },
    { NM("clear", "Clear Undo Buffer"),  MMPush,
      HIDE_0(PTR_FUN(dddClearUndoBufferCB)), 0, 0, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget group_iconify_w;
static Widget uniconify_when_ready_w;
static Widget suppress_warnings_w;
static Widget warn_if_locked_w;
static Widget check_grabs_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *group_iconify_w;
static Gtk::ToggleButton *uniconify_when_ready_w;
static Gtk::ToggleButton *suppress_warnings_w;
static Gtk::ToggleButton *warn_if_locked_w;
static Gtk::ToggleButton *check_grabs_w;
#endif // IF_MOTIF

static MMDesc general_preferences_menu[] = 
{
    { NM("buttonHints", "Automatic Display of Button Hints"),     MMButtonPanel,
      MMNoCB, button_menu, 0, 0, 0 },
    { NM("valueHints", "Automatic Display of Variable Values"),   MMButtonPanel,
      MMNoCB, value_menu, 0, 0, 0 },
    { NM("tabCompletion", "TAB Key Completes"),                   MMRadioPanel,
      MMNoCB, completion_menu, 0, 0, 0 },
    { NM("groupIconify", "Iconify all Windows at Once"),          MMToggle,
      BIND_0( PTR_FUN(dddToggleGroupIconifyCB) ),
      0, (Widget *)&group_iconify_w, 0, 0 },
    { NM("uniconifyWhenReady", "Uniconify When Ready"),           MMToggle,
      BIND_0( PTR_FUN(dddToggleUniconifyWhenReadyCB) ),
      0, (Widget *)&uniconify_when_ready_w, 0, 0 },
    { NM("suppressWarnings", "Suppress X Warnings"),              MMToggle,
      BIND_0( PTR_FUN(dddToggleSuppressWarningsCB) ),
      0, (Widget *)&suppress_warnings_w, 0, 0 },
    { NM("warnIfLocked", "Warn if Multiple DDD Instances are Running"), MMToggle,
      BIND_0( PTR_FUN(dddToggleWarnIfLockedCB) ), 
      0, (Widget *)&warn_if_locked_w, 0, 0 },
    { NM("checkGrabs", "Continue Automatically when Mouse Pointer is Frozen"), MMToggle,
      BIND_0( PTR_FUN(dddToggleCheckGrabsCB) ),
      0, (Widget *)&check_grabs_w, 0, 0 },
    { NM("undoSize", "Undo Buffer Size"),                         MMPanel,
      MMNoCB, undo_menu, 0, 0, 0 },
    MMEnd
};


// Source preferences

#ifdef IF_MOTIF
static Widget set_display_glyphs_w;
static Widget set_display_text_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_display_glyphs_w;
static Gtk::ToggleButton *set_display_text_w;
#endif // IF_MOTIF

static MMDesc glyph_menu[] =
{
    { NM("asGlyphs", "as Glyphs"),         MMRadio,
      HIDE_0_BIND_1(PTR_FUN(sourceSetDisplayGlyphsCB), XtPointer(True)),
      0, (Widget *)&set_display_glyphs_w, 0, 0 },
    { NM("asText", "as Text Characters"),  MMRadio,
      HIDE_0_BIND_1(PTR_FUN(sourceSetDisplayGlyphsCB), XtPointer(False)),
      0, (Widget *)&set_display_text_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget set_tool_buttons_in_toolbar_w;
static Widget set_tool_buttons_in_command_tool_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_tool_buttons_in_toolbar_w;
static Gtk::ToggleButton *set_tool_buttons_in_command_tool_w;
#endif // IF_MOTIF

static MMDesc tool_buttons_menu [] = 
{
    { NM("commandTool", "Command Tool"),    MMRadio,
      BIND_1( PTR_FUN(dddSetToolBarCB), XtPointer(False) ),
      0, (Widget *)&set_tool_buttons_in_command_tool_w, 0, 0 },
    { NM("sourceWindow", "Source Window"),  MMRadio,
      BIND_1( PTR_FUN(dddSetToolBarCB), XtPointer(True) ),
      0, (Widget *)&set_tool_buttons_in_toolbar_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget set_refer_path_w;
static Widget set_refer_base_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_refer_path_w;
static Gtk::ToggleButton *set_refer_base_w;
#endif // IF_MOTIF

static MMDesc refer_menu[] =
{
    { NM("byPath", "by Path Name"),  MMRadio,
      HIDE_0_BIND_1(PTR_FUN(sourceSetUseSourcePathCB), XtPointer(True)),
      0, (Widget *)&set_refer_path_w, 0, 0 },
    { NM("byBase", "by Base Name"),  MMRadio,
      HIDE_0_BIND_1(PTR_FUN(sourceSetUseSourcePathCB), XtPointer(False)),
      0, (Widget *)&set_refer_base_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget words_only_w;
static Widget case_sensitive_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *words_only_w;
static Gtk::ToggleButton *case_sensitive_w;
#endif // IF_MOTIF

static MMDesc find_preferences_menu[] =
{
    { NM("wordsOnly", "Words Only"),          MMToggle,
      BIND_0( PTR_FUN(sourceToggleFindWordsOnlyCB) ), 
      0, (Widget *)&words_only_w, 0, 0 },
    { NM("caseSensitive", "Case Sensitive"),  MMToggle,
      BIND_0( PTR_FUN(sourceToggleFindCaseSensitiveCB) ), 
      0, (Widget *)&case_sensitive_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget cache_source_files_w;
static Widget cache_machine_code_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *cache_source_files_w;
static Gtk::ToggleButton *cache_machine_code_w;
#endif // IF_MOTIF

static MMDesc cache_menu[] =
{
    { NM("cacheSource", "Soruce Files"),  MMToggle,
      BIND_0( PTR_FUN(sourceToggleCacheSourceFilesCB) ), 
      0, (Widget *)&cache_source_files_w, 0, 0 },
    { NM("cacheCode", "Machine Code"),    MMToggle,
      BIND_0( PTR_FUN(sourceToggleCacheMachineCodeCB) ), 
      0, (Widget *)&cache_machine_code_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget tab_width_w;
static Widget source_indent_w;
static Widget code_indent_w;
#else // NOT IF_MOTIF
static Gtk::Scale *tab_width_w;
static Gtk::Scale *source_indent_w;
static Gtk::Scale *code_indent_w;
#endif // IF_MOTIF

static MMDesc scales_menu[] = 
{
    { NM("tabWidth", "Tab Width"),                   MMScale, 
      BIND_0( PTR_FUN(sourceSetTabWidthCB) ),
      0, (Widget *)&tab_width_w, 0, 0 },
    { NM("sourceIndent", "Source Indentation"),      MMScale, 
      BIND_0( PTR_FUN(sourceSetSourceIndentCB) ),
      0, (Widget *)&source_indent_w, 0, 0 },
    { NM("codeIndent", "Machine Code Indentation"),  MMScale, 
      BIND_0( PTR_FUN(sourceSetCodeIndentCB) ),
      0, (Widget *)&code_indent_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget line_numbers2_w;
static Widget refer_sources_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *line_numbers2_w;
static Widget refer_sources_w;

#endif

static MMDesc source_preferences_menu[] = 
{
    { NM("showExecPos", "Show Position and Breakpoints"),  MMRadioPanel,
      MMNoCB, glyph_menu, 0, 0, 0 },
    { NM("toolButtons", "Tool Buttons Location"),          MMRadioPanel,
      MMNoCB, tool_buttons_menu, 0, 0, 0 },
    { NM("referSources", "Refer to Program Sources"),      MMRadioPanel,
      MMNoCB, refer_menu, &refer_sources_w, 0,0},
    { NM("find", "Find"),                                  MMButtonPanel,
      MMNoCB, find_preferences_menu, 0, 0, 0 },
    { NM("cache", "Cache"),                                MMButtonPanel,
      MMNoCB, cache_menu, 0, 0, 0 },
    { NM("lineNumbers", "Display Source Line Numbers"),    MMToggle,
      BIND_0(PTR_FUN(sourceToggleDisplayLineNumbersCB)), 0, 
      (Widget *)&line_numbers2_w, 0, 0 },
    { NM("scales", "scales"),                              MMPanel | MMUnmanagedLabel, 
      MMNoCB, scales_menu, 0, 0, 0 },
    MMEnd
};


// Data preferences

#ifdef IF_MOTIF
static Widget graph_show_hints_w;
static Widget graph_show_annotations_w;
static Widget graph_show_dependent_titles_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *graph_show_hints_w;
static Gtk::ToggleButton *graph_show_annotations_w;
static Gtk::ToggleButton *graph_show_dependent_titles_w;
#endif // IF_MOTIF

static MMDesc show_menu[] = 
{
    { NM("hints", "Edge Hints"),                              MMToggle,
      BIND_0( PTR_FUN(graphToggleShowHintsCB) ),
      0, (Widget *)&graph_show_hints_w, 0, 0 },
    { NM("annotations", "Edge Annotations"),                  MMToggle,
      BIND_0( PTR_FUN(graphToggleShowAnnotationsCB) ),
      0, (Widget *)&graph_show_annotations_w, 0, 0 },
    { NM("dependentTitles", "Titles of Dependent Displays"),  MMToggle,
      BIND_0( PTR_FUN(graphToggleShowDependentTitlesCB) ),
      0, (Widget *)&graph_show_dependent_titles_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget graph_compact_layout_w;
static Widget graph_auto_layout_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *graph_compact_layout_w;
static Gtk::ToggleButton *graph_auto_layout_w;
#endif // IF_MOTIF

static MMDesc layout_menu[] =
{
    { NM("compact", "Compact"),   MMToggle,
      BIND_0(PTR_FUN(graphToggleCompactLayoutCB)),
      0, (Widget *)&graph_compact_layout_w, 0, 0 },
    { NM("auto", "Automatic"),    MMToggle,
      BIND_0(PTR_FUN(graphToggleAutoLayoutCB)),
      0, (Widget *)&graph_auto_layout_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget graph_top_to_bottom_w;
static Widget graph_left_to_right_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *graph_top_to_bottom_w;
static Gtk::ToggleButton *graph_left_to_right_w;
#endif // IF_MOTIF

static MMDesc direction_menu[] =
{
    { NM("topToBottom", "Top to bottom"),    MMRadio,
      BIND_1(PTR_FUN(graphSetDisplayPlacementCB), XmVERTICAL),
      0, (Widget *)&graph_top_to_bottom_w, 0, 0 },
    { NM("leftToRight", "Left to right"),    MMRadio,
      BIND_1(PTR_FUN(graphSetDisplayPlacementCB), XmHORIZONTAL),
      0, (Widget *)&graph_left_to_right_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget graph_cluster_displays_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *graph_cluster_displays_w;
#endif // IF_MOTIF

static MMDesc placement_menu[] =
{
    { NM("direction", ""),                MMRadioPanel | MMUnmanagedLabel,
      MMNoCB, direction_menu, 0, 0, 0 },
    { NM("clusterDisplays", "Clustered"), MMToggle,
      BIND_0( PTR_FUN(graphToggleClusterDisplaysCB) ),
      0, (Widget *)&graph_cluster_displays_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget graph_snap_to_grid_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *graph_snap_to_grid_w;
#endif // IF_MOTIF

static MMDesc themes_menu[] = 
{
    { NM("snapToGrid", "Auto-Align Displays on Nearest Grid Point"),    MMToggle,
      BIND_0( PTR_FUN(graphToggleSnapToGridCB) ),
      0, (Widget *)&graph_snap_to_grid_w, 0, 0 },
    { NM("themes", "Themes..."),                                        MMPush,
      HIDE_0( PTR_FUN(dddPopupThemesCB) ), 0, 0, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget graph_grid_size_w;
#else // NOT IF_MOTIF
static Gtk::Scale *graph_grid_size_w;
#endif // IF_MOTIF

static MMDesc grid_menu[] =
{
    { NM("gridSize", "Grid Size"),      MMScale,
      BIND_0( PTR_FUN(graphSetGridSizeCB) ),
      0, (Widget *)&graph_grid_size_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget graph_detect_aliases_w;
static Widget graph_align_2d_arrays_w;
static Widget graph_auto_close_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *graph_detect_aliases_w;
static Gtk::ToggleButton *graph_align_2d_arrays_w;
static Gtk::ToggleButton *graph_auto_close_w;
#endif // IF_MOTIF

static MMDesc data_preferences_menu[] = 
{
    { NM("show", "Show"),                     MMPanel,
      MMNoCB, show_menu, 0, 0, 0 },
    { NM("placement", "Placement"),                                    MMPanel,
      MMNoCB, placement_menu, 0, 0, 0 },
    { NM("layout", "Layout"),                                          MMPanel,
      MMNoCB, layout_menu, 0, 0, 0 },
    { NM("detectAliases", "Detect Aliases"),                           MMToggle,
      BIND_0( PTR_FUN(graphToggleDetectAliasesCB) ),
      0, (Widget *)&graph_detect_aliases_w, 0, 0 },
    { NM("align2dArrays", "Display Two-Dimensional Arrays as Tables"), MMToggle,
      BIND_0( PTR_FUN(graphToggleAlign2dArraysCB) ),
      0, (Widget *)&graph_align_2d_arrays_w, 0, 0 },
    { NM("autoClose", "Close Data Window when Deleting last Display"), MMToggle,
      BIND_0( PTR_FUN(graphToggleAutoCloseCB) ),
      0, (Widget *)&graph_auto_close_w, 0, 0 },
    { NM("themes", ""),                                                MMPanel | MMUnmanagedLabel, 
      MMNoCB, themes_menu, 0, 0, 0 },
    { NM("grid", ""),                                                  MMPanel | MMUnmanagedLabel,
      MMNoCB, grid_menu, 0, 0, 0 },
    MMEnd
};


// Startup preferences

#ifdef IF_MOTIF
static Widget set_separate_windows_w;
static Widget set_attached_windows_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_separate_windows_w;
static Gtk::ToggleButton *set_attached_windows_w;
#endif // IF_MOTIF

static MMDesc window_mode_menu [] = 
{
    { NM("attached", "Stacked Windows"),   MMRadio,
      BIND_1( PTR_FUN(dddSetSeparateWindowsCB), XtPointer(1) ),
      0, (Widget *)&set_attached_windows_w, 0, 0 },
    { NM("separate", "Separate Windows"),  MMRadio,
      BIND_1( PTR_FUN(dddSetSeparateWindowsCB), XtPointer(0) ),
      0, (Widget *)&set_separate_windows_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget set_button_images_w;
static Widget set_button_captions_w;
static Widget set_flat_buttons_w;
static Widget set_color_buttons_w;
static Widget set_toolbars_at_bottom_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_button_images_w;
static Gtk::ToggleButton *set_button_captions_w;
static Gtk::ToggleButton *set_flat_buttons_w;
static Gtk::ToggleButton *set_color_buttons_w;
static Gtk::ToggleButton *set_toolbars_at_bottom_w;
#endif // IF_MOTIF

static MMDesc button_appearance_menu [] = 
{
    { NM("images", "Images"),      MMToggle,
      BIND_0( PTR_FUN(dddToggleButtonImagesCB) ),
      0, (Widget *)&set_button_images_w, 0, 0 },
    { NM("captions", "Captions"),  MMToggle,
      BIND_0( PTR_FUN(dddToggleButtonCaptionsCB) ),
      0, (Widget *)&set_button_captions_w, 0, 0 },
    { NM("flat", "Flat"),          MMToggle,
      BIND_0( PTR_FUN(dddToggleFlatButtonsCB) ),
      0, (Widget *)&set_flat_buttons_w, 0, 0 },
    { NM("color", "Color"),        MMToggle,
      BIND_0( PTR_FUN(dddToggleColorButtonsCB) ),
      0, (Widget *)&set_color_buttons_w, 0, 0 },
    { NM("bottom", "Bottom"),      MMToggle,
      BIND_0( PTR_FUN(dddToggleToolbarsAtBottomCB) ),
      0, (Widget *)&set_toolbars_at_bottom_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget set_focus_pointer_w;
static Widget set_focus_explicit_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_focus_pointer_w;
static Gtk::ToggleButton *set_focus_explicit_w;
#endif // IF_MOTIF

static MMDesc keyboard_focus_menu [] = 
{
    { NM("pointer", "Point to Type"),   MMRadio,
      BIND_1( PTR_FUN(dddSetKeyboardFocusPolicyCB), XtPointer(XmPOINTER) ),
      0, (Widget *)&set_focus_pointer_w, 0, 0 },
    { NM("explicit", "Click to Type"),  MMRadio,
      BIND_1( PTR_FUN(dddSetKeyboardFocusPolicyCB), XtPointer(XmEXPLICIT) ),
      0, (Widget *)&set_focus_explicit_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget set_scrolling_panner_w;
static Widget set_scrolling_scrollbars_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_scrolling_panner_w;
static Gtk::ToggleButton *set_scrolling_scrollbars_w;
#endif // IF_MOTIF

static MMDesc data_scrolling_menu [] = 
{
    { NM("panner", "Panner"),          MMRadio,
      BIND_1( PTR_FUN(dddSetPannerCB), XtPointer(true) ),
      0, (Widget *)&set_scrolling_panner_w, 0, 0 },
    { NM("scrollbars", "Scrollbars"),  MMRadio,
      BIND_1( PTR_FUN(dddSetPannerCB), XtPointer(false) ),
      0, (Widget *)&set_scrolling_scrollbars_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget set_debugger_bash_w;
static Widget set_debugger_dbg_w;
static Widget set_debugger_dbx_w;
static Widget set_debugger_gdb_w;
static Widget set_debugger_jdb_w;
static Widget set_debugger_perl_w;
static Widget set_debugger_pydb_w;
static Widget set_debugger_xdb_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *set_debugger_bash_w;
static Gtk::ToggleButton *set_debugger_dbg_w;
static Gtk::ToggleButton *set_debugger_dbx_w;
static Gtk::ToggleButton *set_debugger_gdb_w;
static Gtk::ToggleButton *set_debugger_jdb_w;
static Gtk::ToggleButton *set_debugger_perl_w;
static Gtk::ToggleButton *set_debugger_pydb_w;
static Gtk::ToggleButton *set_debugger_xdb_w;
#endif // IF_MOTIF

static MMDesc debugger_menu [] = 
{
    { NM("bash", "Bash"), MMRadio, BIND_1( PTR_FUN(dddSetDebuggerCB), BASH ),
      0, (Widget *)&set_debugger_bash_w, 0, 0 },
    { NM("dbg", "DBG"), MMRadio, BIND_1( PTR_FUN(dddSetDebuggerCB), DBG ),
      0, (Widget *)&set_debugger_dbg_w, 0, 0 },
    { NM("dbx", "DBX"), MMRadio, BIND_1( PTR_FUN(dddSetDebuggerCB), DBX ),
      0, (Widget *)&set_debugger_dbx_w, 0, 0 },
    { NM("gdb", "GDB"), MMRadio, BIND_1( PTR_FUN(dddSetDebuggerCB), GDB ),
      0, (Widget *)&set_debugger_gdb_w, 0, 0 },
    { NM("jdb", "JDB"), MMRadio, BIND_1( PTR_FUN(dddSetDebuggerCB), JDB ),
      0, (Widget *)&set_debugger_jdb_w, 0, 0 },
    { NM("perl", "Perl"), MMRadio, BIND_1( PTR_FUN(dddSetDebuggerCB), PERL ),
      0, (Widget *)&set_debugger_perl_w, 0, 0 },
    { NM("pydb", "PYDB"), MMRadio, BIND_1( PTR_FUN(dddSetDebuggerCB), PYDB ),
      0, (Widget *)&set_debugger_pydb_w, 0, 0 },
    { NM("xdb", "XDB"), MMRadio, BIND_1( PTR_FUN(dddSetDebuggerCB), XDB ),
      0, (Widget *)&set_debugger_xdb_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget auto_debugger_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *auto_debugger_w;
#endif // IF_MOTIF


static MMDesc auto_debugger_menu [] =
{
    { NM("automatic", "Determine Automatically from Arguments"), MMToggle,
      BIND_0( PTR_FUN(dddToggleAutoDebuggerCB) ), 0, (Widget *)&auto_debugger_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget startup_tips_w;
static Widget splash_screen_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *startup_tips_w;
static Gtk::ToggleButton *splash_screen_w;
#endif // IF_MOTIF

static MMDesc startup_menu [] =
{
    { NM("splashScreen", "DDD Splash Screen"), MMToggle,
      BIND_0( PTR_FUN(SetSplashScreenCB) ), 0, (Widget *)&splash_screen_w, 0, 0 },
#ifdef IF_MOTIF
    { NM("startupTips", "Tip of the Day"),     MMToggle,
      BIND_0( PTR_FUN(SetStartupTipsCB) ), 0, (Widget *)&startup_tips_w, 0, 0 },
#endif // IF_MOTIF
    MMEnd
};

#ifdef IF_MOTIF
static Widget cut_copy_paste_kde_w;
static Widget cut_copy_paste_motif_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *cut_copy_paste_kde_w;
static Gtk::ToggleButton *cut_copy_paste_motif_w;
#endif // IF_MOTIF

static MMDesc cut_copy_paste_menu [] =
{
    { NM("kde", "Copy"),         MMRadio,
      BIND_1( PTR_FUN(dddSetCutCopyPasteBindingsCB), KDEBindings ), 
      0, (Widget *)&cut_copy_paste_kde_w, 0, 0 },
    { NM("motif", "Interrupt"),  MMRadio,
      BIND_1( PTR_FUN(dddSetCutCopyPasteBindingsCB), MotifBindings ),
      0, (Widget *)&cut_copy_paste_motif_w, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget select_all_kde_w;
static Widget select_all_motif_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *select_all_kde_w;
static Gtk::ToggleButton *select_all_motif_w;
#endif // IF_MOTIF

static MMDesc select_all_menu [] =
{
    { NM("kde", "Select All"),           MMRadio,
      BIND_1( PTR_FUN(dddSetSelectAllBindingsCB), KDEBindings ), 
      0, (Widget *)&select_all_kde_w, 0, 0 },
    { NM("motif", "Beginning of Line"),  MMRadio,
      BIND_1( PTR_FUN(dddSetSelectAllBindingsCB), MotifBindings ),
      0, (Widget *)&select_all_motif_w, 0, 0 },
    MMEnd
};

static MMDesc startup_preferences_menu [] =
{
    { NM("windows", "Window Layout"),          MMRadioPanel,
      MMNoCB, window_mode_menu, 0, 0, 0 },
    { NM("cutCopyPaste", "Ctrl+C is"),         MMRadioPanel,
      MMNoCB, cut_copy_paste_menu, 0, 0, 0 },
    { NM("selectAll", "Ctrl+A is"),            MMRadioPanel,
      MMNoCB, select_all_menu, 0, 0, 0 },
    { NM("buttons", "Tool Bar Appearance"),    MMButtonPanel,
      MMNoCB, button_appearance_menu, 0, 0, 0 },
#ifdef IF_MOTIF
    { NM("keyboardFocus", "Keyboard Focus"),   MMRadioPanel,
      MMNoCB, keyboard_focus_menu, 0, 0, 0 },
#endif // IF_MOTIF
    { NM("dataScrolling", "Data Scrolling"),   MMRadioPanel,
      MMNoCB, data_scrolling_menu, 0, 0, 0 },
    { NM("autoDebugger", "Debugger Type"),     MMButtonPanel,
      MMNoCB, auto_debugger_menu, 0, 0, 0 },
    { NM("debugger", ""),                      MMRadioPanel,
      MMNoCB, debugger_menu, 0, 0, 0 },
    { NM("startupWindows", "Startup Windows"), MMButtonPanel,
      MMNoCB, startup_menu, 0, 0, 0 },
    MMEnd
};

#ifdef IF_MOTIF
static Widget font_names[5];
static Widget font_sizes[5];
#else // NOT IF_MOTIF
static Gtk::Entry *font_names[5];
static Gtk::Entry *font_sizes[5];
#endif // IF_MOTIF

#ifdef IF_MOTIF
#define FONT_MENU(font)							\
{									\
    { NM("name", ""),                    MMTextField | MMUnmanagedLabel, \
      BIND_1( PTR_FUN(SetFontNameCB), font ), 0,			\
    (Widget *)&font_names[int(font)], 0, 0 },				\
    { NM("size", "Size"),                MMTextField,			\
      BIND_1( PTR_FUN(SetFontSizeCB), font ), 0,			\
    (Widget *)&font_sizes[int(font)], 0, 0 },				\
    { NM("browse", "Browse..."),         MMPush,			\
      BIND_1( PTR_FUN(BrowseFontCB), font ), 0, 0, 0, 0 },		\
    MMEnd								\
}

static MMDesc default_font_menu        [] = FONT_MENU(DefaultDDDFont);
static MMDesc variable_width_font_menu [] = FONT_MENU(VariableWidthDDDFont);
static MMDesc fixed_width_font_menu    [] = FONT_MENU(FixedWidthDDDFont);
static MMDesc data_font_menu           [] = FONT_MENU(DataDDDFont);
#endif // IF_MOTIF

#ifdef IF_MOTIF
static MMDesc font_preferences_menu [] =
{
    { NM("default", "Default Font"),          MMPanel,
      MMNoCB, default_font_menu, 0, 0, 0 },
    { NM("variableWidth", "Variable Width"),  MMPanel,
      MMNoCB, variable_width_font_menu, 0, 0, 0 },
    { NM("fixedWidth", "Fixed Width"),        MMPanel,
      MMNoCB, fixed_width_font_menu, 0, 0, 0 },
    { NM("data", "Data"),                     MMPanel,
      MMNoCB, data_font_menu, 0, 0, 0 },
    MMEnd
};
#endif // IF_MOTIF

#ifdef IF_MOTIF
static Widget extern_plot_window_w;
static Widget builtin_plot_window_w;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *extern_plot_window_w;
static Gtk::ToggleButton *builtin_plot_window_w;
#endif // IF_MOTIF

static MMDesc plot_window_menu [] =
{
    { NM("extern", "External"), MMRadio,
      HIDE_0_BIND_1(PTR_FUN(dddSetBuiltinPlotWindowCB), false),
      0, (Widget *)&extern_plot_window_w, 0, 0},
    { NM("builtin", "Builtin"), MMRadio,
      HIDE_0_BIND_1(PTR_FUN(dddSetBuiltinPlotWindowCB), true),
      0, (Widget *)&builtin_plot_window_w, 0, 0},
    MMEnd
};

#ifdef IF_MOTIF
static Widget edit_command_w;
static Widget get_core_command_w;
static Widget ps_command_w;
static Widget term_command_w;
static Widget uncompress_command_w;
static Widget www_command_w;
static Widget plot_command_w;
#else // NOT IF_MOTIF
static Gtk::Entry *edit_command_w;
static Gtk::Entry *get_core_command_w;
static Gtk::Entry *ps_command_w;
static Gtk::Entry *term_command_w;
static Gtk::Entry *uncompress_command_w;
static Gtk::Entry *www_command_w;
static Gtk::Entry *plot_command_w;
#endif // IF_MOTIF

static MMDesc helpers_preferences_menu [] =
{
    { NM("edit", "Edit Sources"),        MMTextField,
      BIND_0( PTR_FUN(dddSetEditCommandCB) ), 
      0, (Widget *)&edit_command_w, 0, 0},
    { NM("get_core", "Get Core File"),   MMTextField,
      BIND_0( PTR_FUN(dddSetGetCoreCommandCB) ), 
      0, (Widget *)&get_core_command_w, 0, 0},
    { NM("ps", "List Processes"),        MMTextField,
      BIND_0( PTR_FUN(dddSetPSCommandCB) ),
      0, (Widget *)&ps_command_w, 0, 0},
    { NM("term", "Execution Window"),    MMTextField,
      BIND_0( PTR_FUN(dddSetTermCommandCB) ),
      0, (Widget *)&term_command_w, 0, 0},
    { NM("uncompress", "Uncompress"),    MMTextField,
      BIND_0( PTR_FUN(dddSetUncompressCommandCB) ),
      0, (Widget *)&uncompress_command_w, 0, 0},
    { NM("www", "Web Browser"),          MMTextField,
      BIND_0( PTR_FUN(dddSetWWWCommandCB) ),
      0, (Widget *)&www_command_w, 0, 0},
    { NM("plot", "Plot"),                MMTextField,
      BIND_0( PTR_FUN(dddSetPlotCommandCB) ), 
      0, (Widget *)&plot_command_w, 0, 0},
    { NM("plot_window", "Plot Window"),  MMRadioPanel,
      MMNoCB, plot_window_menu, 0, 0, 0 },
    MMEnd
};


// Data

static Widget print_w            = 0;
static Widget display_w          = 0;
static Widget examine_w          = 0;
static TOGGLEBUTTON_P locals_w   = 0;
static TOGGLEBUTTON_P args_w     = 0;
static TOGGLEBUTTON_P detect_aliases_w   = 0;
static Widget infos_w            = 0;
static Widget align_w            = 0;
static Widget edit_watchpoints_w = 0;

DECL_WR(WR_dddPopupInfosCB, CB_ARG_HIDE_0(PTR_FUN(dddPopupInfosCB)));
static MMDesc data_menu[] = 
{
    { NM("displays", "Displays..."),                MMPush,
      HIDE_0(PTR_FUN(DataDisp::EditDisplaysCB)), 0, 0, 0, 0},
    { NM("themes", "Themes..."),                    MMPush,
      HIDE_0(PTR_FUN(dddPopupThemesCB)), 0, 0, 0, 0},
    { NM("watchpoints", "Watchpoints..."),          MMPush,
      HIDE_0(PTR_FUN(SourceView::EditBreakpointsCB)), 
      0, &edit_watchpoints_w, 0, 0 },
    { NM("examine", "Memory..."),                   MMPush,
      BIND_0(PTR_FUN(gdbExamineCB)), 0, &examine_w, 0, 0},
    MMSep,
    { NM("print", "Print ()"),                      MMPush,  
      BIND_1(PTR_FUN(gdbPrintCB), XtPointer(False)), 0, &print_w, 0, 0 },
    { NM("display", "Display ()"),                  MMPush,
      BIND_0(PTR_FUN(gdbDisplayCB)), 0, &display_w, 0, 0 },
    MMSep,
    { NM("detectAliases", "Detect Aliases"),        MMToggle,
      BIND_0(PTR_FUN(graphToggleDetectAliasesCB)),
      0, (Widget *)&detect_aliases_w, 0, 0 },
    MMSep,
    { NM("info locals", "Display Local Variables"), MMToggle,
      BIND_0(PTR_FUN(graphToggleLocalsCB)), 
      0, (Widget *)&locals_w, 0, 0 },
    { NM("info args", "Display Arguments"),         MMToggle,
      BIND_0(PTR_FUN(graphToggleArgsCB)), 
      0, (Widget *)&args_w, 0, 0 },
    { NM("infos", "Status Displays..."),            MMPush,
      BIND_1(PTR_FUN(WhenReady), &WR_dddPopupInfosCB), 
      0, &infos_w, 0, 0 },
    MMSep,
    { NM("align", "Align on Grid"),                 MMPush,
      HIDE_0(PTR_FUN(graphAlignCB)), 0, &align_w, 0, 0 },
    { NM("rotate", "Rotate Graph"),                 MMPush,
      HIDE_0(PTR_FUN(graphRotateCB)), 0, 0, 0, 0},
    { NM("layout", "Layout Graph"),                 MMPush,
      HIDE_0(PTR_FUN(graphLayoutCB)), 0, 0, 0, 0},
    MMSep,
    { NM("refresh", "Refresh"),                     MMPush,
      BIND_0(PTR_FUN(DataDisp::refreshCB)), 0, 0, 0, 0},
    MMEnd
};


// Maintenance

#ifdef IF_MOTIF
static Widget crash_debug_w     = 0;
static Widget crash_dump_core_w = 0;
static Widget crash_nothing_w   = 0;
#else // NOT IF_MOTIF
static Gtk::ToggleButton *crash_debug_w     = 0;
static Gtk::ToggleButton *crash_dump_core_w = 0;
static Gtk::ToggleButton *crash_nothing_w   = 0;
#endif // IF_MOTIF

static MMDesc crash_menu[] = 
{
    { NM("debug", "Debug DDD"),     MMToggle,
      HIDE_0_BIND_1(PTR_FUN(dddSetCrashCB), 2), 0, (Widget *)&crash_debug_w, 0, 0 },
    { NM("dumpCore", "Dump Core Now"), MMToggle,
      HIDE_0_BIND_1(PTR_FUN(dddSetCrashCB), 1), 0, (Widget *)&crash_dump_core_w, 0, 0 },
    { NM("nothing", "Do Nothing"),     MMToggle,
      HIDE_0_BIND_1(PTR_FUN(dddSetCrashCB), 0), 0, (Widget *)&crash_nothing_w, 0, 0 },
    MMEnd
};

static Widget debug_ddd_w       = 0;
static Widget dump_core_w       = 0;
static Widget valgrindLeakCheck_w = 0;

static MMDesc maintenance_menu[] = 
{
    { NM("debug", "Debug DDD..."),                   MMPush,
      HIDE_0_BIND_1(PTR_FUN(DDDDebugCB), 0), 0, &debug_ddd_w, 0, 0 },
    { NM("dumpCore", "Dump Core Now"),               MMPush,
      HIDE_0(PTR_FUN(DDDDumpCoreCB)), 0, &dump_core_w, 0, 0 },
//  { NM("tictactoe", "Tic Tac Toe..."),             MMPush,
//    HIDE_0(PTR_FUN(TicTacToeCB)), 0, 0, 0, 0 },
    MMSep,
    { NM("crash", "When DDD Crashes"),               MMRadioMenu,
      MMNoCB, crash_menu, 0, 0, 0},
    MMSep,
    { NM("valgrindLeak", "Do Valgrind Leak Check"),  MMPush | MMUnmanaged,
      HIDE_0(PTR_FUN(dddValgrindLeakCheckCB)),
      0, &valgrindLeakCheck_w, 0, 0 },
    MMSep,
    { NM("remove", "Remove Menu"),                   MMPush,
      HIDE_0(PTR_FUN(dddClearMaintenanceCB)), 0, 0, 0, 0 },
    MMEnd
};

static Widget maintenance_w = 0;

// Menu Bar for DDD command window
static MMDesc command_menubar[] = 
{
    { NM("file", "File"),               MMMenu,
      HIDE_0_BIND_1(PTR_FUN(gdbUpdateFileCB), command_file_menu), 
      command_file_menu, 0, 0, 0 },
    { NM("edit", "Edit"),               MMMenu,
      BIND_1(PTR_FUN(gdbUpdateEditCB), GDBWindow),
      command_edit_menu, 0, 0, 0 },
    { NM("view", "View"),               MMMenu,
      HIDE_0_BIND_1(PTR_FUN(gdbUpdateViewCB), command_view_menu), 
      command_view_menu, 0, 0, 0 },
    { NM("program", "Program"),         MMMenu,
      MMNoCB, command_program_menu, 0, 0, 0 },
    { NM("commands", "Commands"),       MMMenu,
      MMNoCB, command_menu, 0, 0, 0  },
    { NM("maintenance", "Maintenance"), MMMenu | MMUnmanaged, MMNoCB, maintenance_menu, 
      &maintenance_w, 0, 0  },
#ifdef IF_MOTIF
    { NM("help", "Help"),               MMMenu | MMHelp,
      MMNoCB, simple_help_menu, 0, 0, 0  },
#endif // IF_MOTIF
    MMEnd
};

// Menu Bar for DDD source view
static MMDesc source_menubar[] = 
{
    { NM("file", "File"),       MMMenu,
      HIDE_0_BIND_1(PTR_FUN(gdbUpdateFileCB), source_file_menu), 
      source_file_menu, 0, 0, 0  },
    { NM("edit", "Edit"),       MMMenu,
      BIND_1(PTR_FUN(gdbUpdateEditCB), SourceWindow),
      source_edit_menu, 0, 0, 0  },
    { NM("view", "View"),       MMMenu,
      HIDE_0_BIND_1(PTR_FUN(gdbUpdateViewCB), source_view_menu),
      source_view_menu, 0, 0, 0  },
    { NM("program", "Program"), MMMenu,
      MMNoCB, source_program_menu, 0, 0, 0  },
    { NM("stack", "Status"),    MMMenu,
      MMNoCB, stack_menu, 0, 0, 0  },
    { NM("source", "Source"),   MMMenu,
      MMNoCB, source_menu, 0, 0, 0  },
#ifdef IF_MOTIF
    { NM("help", "Help"),       MMMenu | MMHelp,
      MMNoCB, simple_help_menu, 0, 0, 0  },
#endif // IF_MOTIF
    MMEnd
};

// Menu Bar for DDD data window
static MMDesc data_menubar[] = 
{
    { NM("file", "File"),       MMMenu,
      HIDE_0_BIND_1(PTR_FUN(gdbUpdateFileCB), data_file_menu), 
      data_file_menu, 0, 0, 0  },
    { NM("edit", "Edit"),       MMMenu,
      BIND_1(PTR_FUN(gdbUpdateEditCB), DataWindow),
      data_edit_menu, 0, 0, 0  },
    { NM("view", "View"),       MMMenu,
      HIDE_0_BIND_1(PTR_FUN(gdbUpdateViewCB), data_view_menu), 
      data_view_menu, 0, 0, 0  },
    { NM("program", "Program"), MMMenu,
      MMNoCB, data_program_menu, 0, 0, 0  },
    { NM("data", "Data"),       MMMenu,
      MMNoCB, data_menu, 0, 0, 0  },
#ifdef IF_MOTIF
    { NM("help", "Help"),       MMMenu | MMHelp,
      MMNoCB, simple_help_menu, 0, 0, 0  },
#endif // IF_MOTIF
    MMEnd
};

// Menu Bar for common DDD data/command window
static MMDesc common_menubar[] = 
{
    { NM("file", "File"),               MMMenu,
      HIDE_0_BIND_1(PTR_FUN(gdbUpdateFileCB), command_file_menu), 
      command_file_menu, 0, 0, 0  },
    { NM("edit", "Edit"),               MMMenu,
      BIND_1(PTR_FUN(gdbUpdateEditCB), CommonWindow),
      command_edit_menu, 0, 0, 0  },
    { NM("views", "Views"),             MMMenu,
      HIDE_0_BIND_1(PTR_FUN(gdbUpdateViewsCB), views_menu), 
      views_menu, 0, 0, 0  },
    { NM("program", "Program"),         MMMenu,
      MMNoCB, command_program_menu, 0, 0, 0  },
    { NM("commands", "Commands"),       MMMenu,
      MMNoCB, command_menu, 0, 0, 0  },
    { NM("stack", "Status"),            MMMenu,
      MMNoCB, stack_menu, 0, 0, 0  },
    { NM("source", "Source"),           MMMenu,
      MMNoCB, source_menu, 0, 0, 0  },
    { NM("data", "Data"),               MMMenu,
      MMNoCB, data_menu, 0, 0, 0  },
    { NM("maintenance", "Maintenance"), MMMenu | MMUnmanaged,
      MMNoCB, maintenance_menu, &maintenance_w, 0, 0  },
#ifdef IF_MOTIF
    { NM("help", "Help"),               MMMenu | MMHelp,
      MMNoCB, simple_help_menu, 0, 0, 0  },
#endif // IF_MOTIF
    MMEnd
};

struct PrintItems {
    enum ArgCmd { PrintRef, Dump, Whatis, Examine };
};

static Widget print_ref_w     = 0;
static Widget print_dump_w    = 0;
static Widget print_whatis_w  = 0;
static Widget print_examine_w = 0;

static MMDesc print_menu[] =
{
    { NM("printRef", "Print *()"),      MMPush,
      BIND_1(PTR_FUN(gdbPrintRefCB), false), 0, &print_ref_w, 0, 0 },
    { NM("dump", "Dump ()"),            MMPush, 
      BIND_1(PTR_FUN(gdbPrintCB), true), 0, &print_dump_w, 0, 0 },
    { NM("whatis", "Whatis ()"),        MMPush, 
      BIND_0(PTR_FUN(gdbWhatisCB)), 0, &print_whatis_w, 0, 0 },
    { NM("examine", "Examine ()..."),   MMPush, 
      BIND_0(PTR_FUN(gdbExamineCB)), 0, &print_examine_w, 0, 0 },
    MMEnd
};

struct DispItems {
    enum ArgCmd { DispRef };
};

static Widget disp_ref_w     = 0;

static MMDesc display_menu[] =
{
    { NM("dispRef", "Display *()"),        MMPush,
      BIND_0(PTR_FUN(gdbDispRefCB)), 0, &disp_ref_w, 0, 0 },
    MMEnd
};

struct WatchItems {
    enum ArgCmd { Properties, Enable, Sep, CWatch, RWatch, AWatch };
};

static MMDesc watch_menu[] =
{
    { NM("watchProperties", "Watchpoint Properties..."),  MMPush, 
      HIDE_0(PTR_FUN(gdbEditWatchpointPropertiesCB)), 0, 0, 0, 0},
    { NM("enableWatch", "Enable Watchpoint at ()"),       MMPush, 
      BIND_0(PTR_FUN(gdbToggleEnableWatchpointCB)), 0, 0, 0, 0},
    MMSep,
    { NM("cwatch", "Set Watchpoint on ()"),               MMPush, 
      BIND_1(PTR_FUN(gdbWatchCB), WATCH_CHANGE), 0, 0, 0, 0 },
    { NM("rwatch", "Set Read Watchpoint on ()"),          MMPush, 
      BIND_1(PTR_FUN(gdbWatchCB), WATCH_READ), 0, 0, 0, 0 },
    { NM("awatch", "Set Access Watchpoint on ()"),        MMPush, 
      BIND_1(PTR_FUN(gdbWatchCB), WATCH_ACCESS), 0, 0, 0, 0 },

    // It would be nice to have an `unwatch' command here, for
    // recording commands.  Unfortunately, GDB needs a watchpoint
    // number for deleting watchpoints.
#if 0	
    MMSep,
    { NM("unwatch", "Unwatch ()"),                        MMPush,
      BIND_0( PTR_FUN(gdbUnwatchCB) ), 0, 0, 0, 0},
#endif
    MMEnd
};

struct BreakItems {
    enum ArgCmd { TempBreak, RegexBreak, Properties, Enable, Sep1,
		  ContUntil, SetPC, Sep2, ClearAt2 };
};

static MMDesc break_menu[] = 
{
    { NM("tempBreakAt", "Set Temporary Breakpoint at ()"),     MMPush,
      BIND_0(PTR_FUN(gdbTempBreakAtCB)), 0, 0, 0, 0},
    { NM("regexBreakAt", "Set Breakpoints at Regexp ()"),      MMPush,
      BIND_0(PTR_FUN(gdbRegexBreakAtCB)), 0, 0, 0, 0},
    { NM("breakProperties", "Breakpoint Properties..."),       MMPush, 
      HIDE_0(PTR_FUN(gdbEditBreakpointPropertiesCB)), 0, 0, 0, 0},
    { NM("enableBreak", "Enable Breakpoint at ()"),            MMPush, 
      BIND_0(PTR_FUN(gdbToggleEnableBreakpointCB)), 0, 0, 0, 0},
    MMSep,
    { NM("contUntil", "Continue Until ()"),                    MMPush,
      BIND_0(PTR_FUN(gdbContUntilCB)), 0, 0, 0, 0},
    { NM("setPC", "Set Execution Position to ()"),             MMPush,
      BIND_0(PTR_FUN(gdbSetPCCB)), 0, 0, 0, 0},
    MMSep,
    { NM("clearAt2", "Clear Breakpoint at ()"),                MMPush,
      BIND_0(PTR_FUN(gdbClearAtCB)), 0, 0, 0, 0},
    MMEnd
};

struct FindItems {
    enum ArgCmd { FindForward, FindBackward };
};

static MMDesc find_menu[] = 
{
    { NM("findForward", "@charset Find@small>> @charset()@rm"),  MMPush, 
      BIND_1(PTR_FUN(gdbFindCB), SourceView::forward), 0, 0, 0, 0},
    { NM("findBackward", "@charset Find@small<< @charset()@rm"), MMPush, 
      BIND_1(PTR_FUN(gdbFindCB), SourceView::backward), 0, 0, 0, 0},
    MMEnd
};

struct ArgItems {
    enum ArgCmd { Lookup, Find, Break, Watch, Print, Display };
};

static MMDesc arg_cmd_area[] = 
{
    { IM("lookup", LOOKUP_ICON),      MMPush,
     HIDE_0(PTR_FUN(gdbLookupCB)), 0, 0, 0, 0},
    { IM("find", FIND_FORWARD_ICON),  MMPush | MMInsensitive, 
     BIND_0(PTR_FUN(gdbFindAgainCB)), find_menu, 0, 0, 0 },
    { IM("breakAt", BREAK_AT_ICON),   MMPush,  BIND_0(PTR_FUN(gdbToggleBreakCB)), break_menu, 0, 0, 0 },
    { IM("watch", WATCH_ICON),        MMPush,  
     BIND_1(PTR_FUN(gdbToggleWatchCB), WATCH_CHANGE), watch_menu, 0, 0, 0 },
    { IM("print", PRINT_ICON),        MMPush,  
     BIND_1(PTR_FUN(gdbPrintCB), false), print_menu, 0, 0, 0 },
    { IM("display", DISPLAY_ICON),    MMPush,  BIND_0(PTR_FUN(gdbDisplayCB)), display_menu, 0, 0, 0 },
    MMEnd
};


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// All communication with GDB passes through this variable
GDBAgent*     gdb = 0;

// Data display
DataDisp*     data_disp;

// Source display
SourceView*   source_view;

// Argument field
ArgField*     source_arg;

// Argument toolbar
static CONTAINER_P arg_cmd_w;

// GDB input/output widget
SCROLLEDTEXT_P gdb_w;

// GDB status line
STATUSBAR_P status_w;

// GDB activity led
static TOGGLEBUTTON_P led_w;

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

#ifdef IF_MOTIF
// The Xt Warning handler
static XtErrorHandler ddd_original_xt_warning_handler;
#endif // IF_MOTIF

// Initial delays
static StatusMsg *init_delay = 0;
static Delay *setup_delay = 0;

// Events to note for window visibility
const int STRUCTURE_MASK = StructureNotifyMask | VisibilityChangeMask;

// Message handling
static MString version_warnings;


//-----------------------------------------------------------------------------
// DDD main program
//-----------------------------------------------------------------------------

typedef enum {
  DDD_EXIT_FAILURE, DDD_EXIT_SUCCESS, DDD_CONTINUE
} ddd_exit_t;

static
ddd_exit_t pre_main_loop(int argc, char *argv[])
{
  // This function exists so that all destructors are called before
  // entering the event loop.

#ifdef LC_ALL
    // Let DDD locales be controlled by the locale-specific
    // environment variables -- especially $LANG.
    XtSetLanguageProc(NULL, NULL, NULL);
    setlocale(LC_ALL, "");
#endif

    // Save environment for restart.
    register_argv((const char**)argv);
    register_environ();

    // This one is required for error messages
    const char *program_name = (argc > 0 ? argv[0] : ddd_NAME);

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
    string gdb_name = "";
    setup_options(argc, (const char**)argv, saved_options, gdb_name, no_windows);

    // If we don't want windows, just start GDB.
    if (no_windows)
    {
	argv[0] = CONST_CAST(char*,gdb_name.chars());
	execvp(gdb_name.chars(), argv);
	perror(gdb_name.chars());
	return DDD_EXIT_FAILURE;
    }

#ifdef IF_MOTIF
    // Initialize X toolkit
    Arg args[10];
    int arg = 0;

    // Install X error handlers
    ddd_install_x_fatal();
    ddd_install_x_error();
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Error handlers?
#endif
#endif // IF_MOTIF

    std::ostringstream messages;

    // Set up a `~/.ddd/' directory hierarchy
    create_session_dir(DEFAULT_SESSION, messages);

    // Read ~/.ddd/init resources
    XrmDatabase dddinit = 
	GetFileDatabase(session_state_file(DEFAULT_SESSION));
    if (dddinit == 0) {
#ifdef IF_MOTIF
	dddinit = XrmGetStringDatabase("");
#else // NOT IF_MOTIF
	dddinit = get_string_database("");
#endif // IF_MOTIF
    }

    // Read ~/.ddd/tips resources
    XrmDatabase dddtips =
	GetFileDatabase(session_tips_file());
    if (dddtips != 0) {
#ifdef IF_MOTIF
	XrmMergeDatabases(dddtips, &dddinit);
#else // NOT IF_MOTIF
	merge_databases(dddtips, dddinit);
#endif // IF_MOTIF
    }

#ifdef IF_MOTIF
    // Let command-line arguments override ~/.ddd/init
    XrmParseCommand(&dddinit, options, XtNumber(options), 
		    DDD_CLASS_NAME, &argc, (char**)argv);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Implement XrmParseCommand analogue using popt
#endif
#endif // IF_MOTIF

    const _XtString session_id = 0;

#ifdef IF_MOTIF
    if (session_id == 0)
    {
	// Determine session
	char *session_rtype = 0;
#ifdef IF_MOTIF
	XrmValue session_value;
#else // NOT IF_MOTIF
	Glib::Value<std::string> session_value;
	session_value.init(Glib::Value<std::string>::value_type());
#endif // IF_MOTIF

	string Nsession   = string(DDD_CLASS_NAME ".") + XtNsession;
	string CSessionID = string(DDD_CLASS_NAME ".") + XtCSessionID;
#if XtSpecificationRelease >= 6
	string NsessionID = string(DDD_CLASS_NAME ".") + XtNsessionID;
#endif

	// Try resource or option
#ifdef IF_MOTIF
	if (
#if XtSpecificationRelease >= 6
	    XrmGetResource(dddinit, NsessionID.chars(), CSessionID.chars(),
			   &session_rtype, &session_value) ||
#endif
	    XrmGetResource(dddinit, Nsession.chars(), CSessionID.chars(),
			   &session_rtype, &session_value))
	{
	    if (session_value.addr != 0)
	    {
		static string id(REINTERPRET_CAST(char *, session_value.addr), 
				 session_value.size);
		session_id = id.chars();
	    }
	}
#else // NOT IF_MOTIF
	if (
	    get_resource(dddinit, Nsession.chars(), CSessionID.chars(),
			 session_value))
	{
	  session_id = session_value.get().c_str();
	}
#endif // IF_MOTIF

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
			
		    session_id = basename(argv[i]);
#ifdef IF_MOTIF
		    session_value.addr = CONST_CAST(char*,session_id);
		    session_value.size = strlen(session_id) + 1;
		    XrmPutResource(&dddinit, Nsession.chars(), XtRString, 
				   &session_value);
#else // NOT IF_MOTIF
		    session_value.set(CONST_CAST(char*,session_id));
		    put_resource(dddinit, Nsession.chars(), XtRString, 
				 session_value);
#endif // IF_MOTIF
		}
	    }
	}
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning BUGGER THIS: Glib::Value is totally undocumented cruft.
#endif
#endif // IF_MOTIF

    if (!restart_session().empty())
    {
	// A session is given in $DDD_SESSION: override everything.
	XrmDatabase session_db = 
	    GetFileDatabase(session_state_file(restart_session()));
	if (session_db != 0) {
#ifdef IF_MOTIF
	    XrmMergeDatabases(session_db, &dddinit);
#else // NOT IF_MOTIF
	    merge_databases(session_db, dddinit);
#endif // IF_MOTIF
	}
    }
    else if (session_id != 0)
    {
	// Merge in session resources; these override `~/.ddd/init' as
	// well as the command-line options.
	XrmDatabase session_db = 
	    GetFileDatabase(session_state_file(session_id));
	if (session_db != 0) {
#ifdef IF_MOTIF
	    XrmMergeDatabases(session_db, &dddinit);
#else // NOT IF_MOTIF
	    merge_databases(session_db, dddinit);
#endif // IF_MOTIF
	}
    }

#ifdef IF_MOTIF
#if HAVE_ATHENA
    // Initialize Xaw widget set, registering the Xaw Converters.
    // This is done before installing our own converters.
    XawInitializeWidgetSet();
#endif
#else // NOT IF_MOTIF
    Gtk::Main *gtk_main_loop = new Gtk::Main(argc, argv);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Register own converters.  This must be done here to install the
    // String -> Cardinal converter.
    registerOwnConverters();
#endif // IF_MOTIF

    // Handle `--version', `--help', etc.  We do this here, since we
    // might want to use them even without X access.
    setup_show(dddinit, basename(argv[0]), gdb_name.chars());

    // From this point on, we'll be running under X.

    // Open X connection and create top-level application shell
    XtAppContext app_context;

#ifdef IF_MOTIF
    arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;

#if XtSpecificationRelease >= 6
    if (session_id != 0)
    {
	XtSetArg(args[arg], XtNsessionID, session_id); arg++;
    }

    // Note: the cast on ddd_fallback_resources is safe.
    Widget toplevel =
	XtOpenApplication(&app_context, DDD_CLASS_NAME,
			  XrmOptionDescList(0), 0,
			  &argc, argv, (String *)ddd_fallback_resources,
			  sessionShellWidgetClass,
			  args, arg);

    XtAddCallback(toplevel, XtNsaveCallback, SaveSmSessionCB, XtNIL);
    XtAddCallback(toplevel, XtNdieCallback, ShutdownSmSessionCB, XtNIL);
#else
    // Note: the cast on ddd_fallback_resources is safe.
    Widget toplevel =
	XtAppInitialize(&app_context, DDD_CLASS_NAME,
			XrmOptionDescList(0), 0,
			&argc, argv, (String *)ddd_fallback_resources,
			args, arg);
#endif
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Should be some arguments here
#endif
    Gtk::Window *toplevel = new Gtk::Window;
    app_context = toplevel;
#endif // IF_MOTIF

#ifdef IF_MOTIF
    ddd_install_xt_error(app_context);
#endif // IF_MOTIF

    // Check Motif version.  We can do this only now after the first
    // Motif widget has been created.
    setup_motif_version_warnings();

#ifdef IF_MOTIF
    // Setup top-level actions; this must be done before reading
    // application defaults.
    XtAppAddActions(app_context, actions, XtNumber(actions));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XtAppAddActions: this stuff is a bit like glade...
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Register string -> OnOff converter; this must be done before
    // reading application defaults.
    XtSetTypeConverter(XmRString, XtROnOff, CvtStringToOnOff,
		       XtConvertArgList(0), 0, XtCacheAll, 
		       XtDestructor(0));

    // Get application resources into APP_DATA
    XtVaGetApplicationResources(toplevel, (XtPointer)&app_data,
				ddd_resources, ddd_resources_size,
				XtNIL);

    // If we have no application defaults so far, or if the
    // application defaults have the wrong version, try to load our own.
    XrmDatabase target = XtDatabase(XtDisplay(toplevel));
    if (app_data.app_defaults_version == 0 ||
	string(app_data.app_defaults_version) != DDD_VERSION)
    {
	if (app_data.app_defaults_version != 0)
	{
	    messages << "Ignoring app-defaults file for " 
		     << DDD_NAME " " << app_data.app_defaults_version << "\n";
	}

	XrmDatabase app_def = app_defaults(XtDisplay(toplevel));
	XrmMergeDatabases(app_def, &target);
    }

    // Merge ~/.ddd/init resources into application shell
    XrmMergeDatabases(dddinit, &target);

    // Read APP_DATA again
    XtVaGetApplicationResources(toplevel, (XtPointer)&app_data,
				ddd_resources, ddd_resources_size,
				XtNIL);

#if XtSpecificationRelease >= 6
    // Synchronize SESSION_ID and APP_DATA.session
    if (session_id == 0)
	XtVaGetValues(toplevel, XtNsessionID, &session_id, XtNIL);
    if (session_id != 0)
	app_data.session = session_id;
#endif

    if (app_data.session == 0)
	app_data.session = DEFAULT_SESSION.chars();
    if (!restart_session().empty())
    {
	static string s;
	s = restart_session();
	app_data.session = s.chars();
    }
    session_id = app_data.session;
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No resource database: no need for converters yet.
#endif
    get_application_resources(dddinit, &app_data,
			      ddd_resources, ddd_resources_size);
#endif // IF_MOTIF

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

#ifdef IF_MOTIF
    // Setup label hack
    arg = 0;
    XtCreateWidget("label_hack", xmLabelHackWidgetClass, toplevel, args, arg);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning LabelHack not defined - what is it?
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Set key bindings
    setup_cut_copy_paste_bindings(XtDatabase(XtDisplay(toplevel)));
    setup_select_all_bindings(XtDatabase(XtDisplay(toplevel)));

    // Handle `--version', `--help', etc.  once more, in case the user
    // gave appropriate X resources.  Somewhat paranoid.
    setup_show(XtDatabase(XtDisplay(toplevel)), XtName(toplevel), gdb_name.chars());

    // Define font macros
    setup_fonts(app_data, XtDatabase(XtDisplay(toplevel)));
    if (app_data.show_fonts)
	return DDD_EXIT_SUCCESS;
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XtDatabase not defined: is there any equivalent?
#endif
#endif // IF_MOTIF

    // Create a new auto_command_prefix if needed
    setup_auto_command_prefix();

    // Forward messages found so far into cerr
    {
	string msg(messages);
	std::cerr << msg;
    }

    // Set up VSL resources
    {
	// SGI CC wants this:
	const char **tmp_argv = (const char **)argv;

	if (VSEFlags::parse_vsl(argc, tmp_argv))
	{
	    // Show VSL usage...
	    std::cout << VSEFlags::explain(true);
	    return DDD_EXIT_FAILURE;
	}

	argv = (char **)tmp_argv;
    }

    // All remaining args are passed to the inferior debugger.
    if (argc == 1 && app_data.open_selection)
    {
	// SGI CC wants this:
	const char **tmp_argv = (const char**)argv;

#ifdef IF_MOTIF
	// No args given - try to get one from current selection
	add_arg_from_selection(toplevel, argc, tmp_argv);
#else // NOT IF_MOTIF
	std::cerr << "SORRY, add_arg_from_selection not supported yet\n";
#endif // IF_MOTIF

	argv = (char **)tmp_argv;
    }

#ifdef IF_MOTIF
    // Check the X configuration
    if (app_data.check_configuration)
    {
	if (check_x_configuration(toplevel, true))
	    return DDD_EXIT_FAILURE;
	else
	    return DDD_EXIT_SUCCESS;
    }

    // If needed, fix the X configuration silently
    check_x_configuration(toplevel, false);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Set up warning handler
    ddd_original_xt_warning_handler =
	XtAppSetWarningHandler(app_context, ddd_xt_warning);
#endif // IF_MOTIF

    // Determine debugger type
    DebuggerType debugger_type = DebuggerType(-1);

    if (debugger_type == DebuggerType(-1) && !gdb_name.empty())
    {
	// Use given debugger
	get_debugger_type(gdb_name, debugger_type);
    }

    if (debugger_type == DebuggerType(-1) && !app_data.auto_debugger)
    {
	// Use debugger from args or app_defaults
	get_debugger_type(app_data.debugger, debugger_type);
    }

    if (debugger_type == DebuggerType(-1))
    {
	// Guess debugger type from args
	DebuggerInfo info(argc, argv);
	debugger_type = info.type;

	if (!app_data.auto_debugger)
	{
	    std::cerr << "Unknown debugger type " << quote(app_data.debugger)
		 << ", using " << quote(default_debugger(app_data.debugger,
							 debugger_type))
		 << "instead\n";
	}
    }

    if (app_data.debugger_command[0] == '\0')
    {
	if (!gdb_name.empty())
	    app_data.debugger_command = gdb_name.chars();
	else
	    app_data.debugger_command = 
		default_debugger(app_data.debugger, debugger_type);
    }

    // Set host specification
    if (app_data.debugger_rhost && app_data.debugger_rhost[0] != '\0')
	gdb_host = app_data.debugger_rhost;
    else if (app_data.debugger_host && app_data.debugger_host[0] != '\0')
	gdb_host = app_data.debugger_host;
    else
	gdb_host = "";

    if (gdb_host.contains('@') && string(app_data.debugger_host_login).empty())
    {
	static string new_login = gdb_host.before('@');
	app_data.debugger_host_login = new_login.chars();
	gdb_host = gdb_host.after('@');
    }

    if (gdb_host.empty() && !string(app_data.debugger_host_login).empty())
    {
	std::cerr << argv[0] << ": --login requires --rhost or --host\n";
	return DDD_EXIT_FAILURE;
    }

    // Check for `--play-log'
    if (app_data.play_log != 0)
    {
	// Invoke self with `--PLAY LOGFILE' as inferior
	app_data.debugger_command = argv[0];

	if (!remote_gdb())
	{
	    // Override debugger type from log
	    check_log(app_data.play_log, debugger_type);
	}

	// Don't overwrite existing log files
	app_data.trace = true;
    }

    // Create a `~/.ddd/log' file for this session; 
    // log invocation and configuration
    init_dddlog();

    // Warn for incompatible `Ddd' and `~/.ddd/init' files
    setup_ddd_version_warnings();

#ifdef IF_MOTIF
    // Global variables: Set LessTif version
    lesstif_version = app_data.lesstif_version;
#endif // IF_MOTIF

    // Global variables: Set maximum lengths
    max_value_tip_length              = app_data.max_value_tip_length;
    max_value_doc_length              = app_data.max_value_doc_length;
    DispBox::max_display_title_length = app_data.max_display_title_length;
    SourceView::max_popup_expr_length = app_data.max_popup_expr_length;

    // Global variables: Setup VSL message handler
    VSLLib::echo = vsl_echo;

    // Global variables: Setup data display
    DataDisp::bump_displays           = app_data.bump_displays;
    DispValue::expand_repeated_values = app_data.expand_repeated_values;
    DispGraph::hide_inactive_displays = app_data.hide_inactive_displays;

    // Global variables: Setup plot settings
    PlotAgent::plot_2d_settings = app_data.plot_2d_settings;
    PlotAgent::plot_3d_settings = app_data.plot_3d_settings;

    // Global variables: Set delays for button and value tips
#ifdef IF_MOTIF
    help_button_tip_delay = app_data.button_tip_delay;
    help_value_tip_delay  = app_data.value_tip_delay;
    help_button_doc_delay = app_data.button_doc_delay;
    help_value_doc_delay  = app_data.value_doc_delay;
    help_clear_doc_delay  = app_data.clear_doc_delay;
    help_clear_tip_delay  = app_data.clear_tip_delay;
#endif // IF_MOTIF

    // Re-register own converters.  Motif may have overridden some of
    // these, so register them again.
#ifdef IF_MOTIF
    registerOwnConverters();
#endif // IF_MOTIF

    Boolean iconic;

#ifdef IF_MOTIF
    // Show splash screen
    XtVaGetValues(toplevel, XmNiconic, &iconic, XtNIL);
    if (app_data.splash_screen && !iconic && restart_session().empty())
	popup_splash_screen(toplevel, app_data.splash_screen_color_key);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Forget the splash screen for now.
#endif
#endif // IF_MOTIF

    // Re-register own converters.  Motif has overridden some of
    // these, so register them again.
#ifdef IF_MOTIF
    registerOwnConverters();
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Install special Motif converters
#if XmVersion >= 1002
    XmRepTypeInstallTearOffModelConverter();
#endif
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XmRepTypeInstallTearOffModelConverter?
#endif
#endif // IF_MOTIF

    // Lock `~/.ddd/'.
    LockInfo lock_info;
    bool lock_ok = lock_ddd(toplevel, lock_info);
    if (!lock_ok)
    {
	// A minimal warning.
	messages << "Another " DDD_NAME " is running (pid "
		 << lock_info.pid;
	if (lock_info.hostname != fullhostname())
	    messages << ", host " << cook(lock_info.hostname);
	if (lock_info.display != XDisplayString(XtDisplay(toplevel)))
	    messages << ", display " << cook(lock_info.display);
	messages << ")\n";
    }

    // Put saved options back again
    int i;
    for (i = argc + saved_options.size() - 1; i > saved_options.size(); i--)
	argv[i] = argv[i - saved_options.size()];
    for (i = saved_options.size() - 1; i >= 0; i--)
	argv[i + 1] = CONST_CAST(char*,saved_options[i].chars());
    argc += saved_options.size();
    argv[argc] = 0;

    // Create GDB interface
    gdb = new_gdb(debugger_type, app_data, app_context, argc, argv);
#ifdef IF_MOTIF
    const string s1 = gdb->title();
    defineConversionMacro("GDB",      s1.chars());
    defineConversionMacro("THEHOST",  DDD_HOST);
    defineConversionMacro("ROOT",     DDD_ROOT);
    defineConversionMacro("ALT_ROOT", DDD_ALT_ROOT);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Converters?
#endif
#endif // IF_MOTIF

    // Set up GDB handlers
    gdb->addHandler(ReadyForQuestion, gdb_readyHP);
    gdb->addHandler(InputEOF,         gdb_eofHP);
    gdb->addHandler(ErrorEOF,         gdb_eofHP);
    gdb->addHandler(Started,          ReportStartHP);
    gdb->addHandler(Died,             gdb_diedHP);
    gdb->addHandler(Died,             DisableBlinkHP);
    gdb->addHandler(Died,             ReportDeathHP);
    gdb->addHandler(ExceptionState,   gdb_exceptionHP);
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

#ifdef IF_MOTIF
    arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    XtSetArg(args[arg], XmNargc,           original_argc); arg++;
    XtSetArg(args[arg], XmNargv,           original_argv); arg++;

    // Forward toplevel geometry (as given by `-geometry') to command shell
    String toplevel_geometry = 0;
    XtVaGetValues(toplevel, XmNgeometry, &toplevel_geometry, XtNIL);
    if (toplevel_geometry != 0)
    {
	XtSetArg(args[arg], XmNgeometry, toplevel_geometry); arg++;
    }

    // Make command shell a popup shell.  
    // The toplevel window is never realized.
    command_shell = verify(XtCreatePopupShell("command_shell",
					      applicationShellWidgetClass,
					      toplevel, args, arg));
    AddDeleteWindowCallback(command_shell, DDDCloseCB);
#else // NOT IF_MOTIF
    command_shell = new Gtk::Window();
    command_shell->set_name(XMST("command_shell"));
    command_shell->set_title(XMST("command_shell"));
    AddDeleteWindowCallback(command_shell, BIND_1(PTR_FUN(DDDCloseCB), command_shell));
#endif // IF_MOTIF


#ifdef IF_MOTIF
    // From this point on, we have a true top-level window.

    // Create main window
    arg = 0;

    Widget main_window = XmCreateMainWindow(command_shell, 
					    XMST("main_window"),
					    args, arg);
    XtManageChild(main_window);
#else // NOT IF_MOTIF

    CONTAINER_P main_window = new Gtk::VBox();
    main_window->set_name(XMST("main_window"));
    command_shell->add(*main_window);
    main_window->show();
#endif // IF_MOTIF

    // Re-register own converters to override Motif converters.
#ifdef IF_MOTIF
    registerOwnConverters();
#endif // IF_MOTIF

    // Create menu bar
    MMDesc *menubar = common_menubar;
    if (app_data.separate_data_window && app_data.separate_source_window)
	menubar = command_menubar;

    Widget menubar_w = MMcreateMenuBar(main_window, "menubar", menubar);
    MMaddCallbacks(menubar);
    MMaddHelpCallback(menubar, PTR_FUN(ImmediateHelpCB));
    verify_buttons(menubar);
    register_menu_shell(menubar);

    // Create Paned Window

#ifdef IF_MOTIF
    arg = 0;
    XtSetArg(args[arg], XmNborderWidth,     0); arg++;
    XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
    Widget paned_work_w = 
	verify(XmCreatePanedWindow(main_window,
				   XMST("paned_work_w"),
				   args, arg));
    XtManageChild(paned_work_w);
    CONTAINER_P main_vbox = paned_work_w;
#else // NOT IF_MOTIF
    // Note: On Motif it is possible to force a pane to have
    // fixed size.  On Gtk this does not seem possible.  Therefore
    // the toolbar and status bar must go in a VBox.
    Gtk::Box *main_vbox = new Gtk::VBox();
    main_vbox->show();
    main_window->add(*main_vbox);
#endif // IF_MOTIF

    // Status line
    if (!app_data.separate_source_window && !app_data.status_at_bottom)
	create_status(main_vbox);

    // Toolbar label type
    unsigned char label_type = XmSTRING;
    if (app_data.button_captions || app_data.button_images)
	label_type = XmPIXMAP;

    // Common toolbar
    if (!app_data.button_captions && !app_data.button_images)
	app_data.common_toolbar = false;

    BUTTON_P arg_label = 0;

    if (!app_data.separate_source_window &&
	!app_data.separate_data_window &&
	app_data.common_toolbar)
    {
	arg_cmd_area[ArgItems::Display].type |= MMUnmanaged;
	arg_cmd_w = create_toolbar(main_vbox, "common",
				   arg_cmd_area, DataDisp::graph_cmd_area,
				   arg_label, source_arg, XmPIXMAP);

	DataDisp::graph_cmd_w = arg_cmd_w;

	if (command_toolbar_w == 0)
	{
	    command_toolbar_w = make_buttons(main_vbox, 
					     "command_toolbar", 
					     app_data.tool_buttons);
	}
    }

#ifndef IF_MOTIF
    Gtk::Container *paned_work_w = new GtkMultiPaned();
    main_vbox->add(*paned_work_w);
#endif // IF_MOTIF
    
    // Install icons if not already done
#ifdef IF_MOTIF
    install_icons(command_shell, 
		  app_data.button_color_key,
		  app_data.active_button_color_key);
#endif // IF_MOTIF

    // Data window
    CONTAINER_P data_disp_parent = paned_work_w;
    Widget data_menubar_w = 0;
    CONTAINER_P data_main_window_w = 0;
    if (app_data.separate_data_window)
    {
#ifdef IF_MOTIF
	arg = 0;
	XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
	data_disp_shell =
	    verify(XtCreatePopupShell("data_disp_shell",
				      topLevelShellWidgetClass,
				      toplevel, args, arg));

	AddDeleteWindowCallback(data_disp_shell, DDDCloseCB);
#else // NOT IF_MOTIF
	data_disp_shell = new Gtk::Window();
	data_disp_shell->set_name(XMST("data_disp_shell"));
	data_disp_shell->set_title(XMST("data_disp_shell"));
	AddDeleteWindowCallback(data_disp_shell, BIND_1(PTR_FUN(DDDCloseCB), data_disp_shell));
#endif // IF_MOTIF


#ifdef IF_MOTIF
	arg = 0;
	data_main_window_w = 
	    XmCreateMainWindow(data_disp_shell, 
			       XMST("data_main_window"),
			       args, arg);
	XtManageChild(data_main_window_w);
#else // NOT IF_MOTIF
	data_main_window_w = new Gtk::VBox();
	data_main_window_w->set_name(XMST("data_main_window"));
	data_disp_shell->add(*data_main_window_w);
	data_main_window_w->show();
#endif // IF_MOTIF

	// Add menu bar
	data_menubar_w = 
	    MMcreateMenuBar (data_main_window_w, 
			     "menubar", data_menubar);
	MMaddCallbacks(data_menubar);
	MMaddHelpCallback(menubar, PTR_FUN(ImmediateHelpCB));
	verify_buttons(data_menubar);
	register_menu_shell(data_menubar);

#ifdef IF_MOTIF
	arg = 0;
	XtSetArg(args[arg], XmNborderWidth,     0); arg++;
	XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
	XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
	data_disp_parent = 
	    verify(XmCreatePanedWindow(data_main_window_w,
				       XMST("data_paned_work_w"),
				       args, arg));
	XtManageChild(data_disp_parent);
#else // NOT IF_MOTIF
	// Note: On Motif it is possible to force a pane to have
	// fixed size.  On Gtk this does not seem possible.  Therefore
	// the toolbar and status bar must go in a VBox.
	data_disp_parent = new Gtk::VBox();
	data_disp_parent->show();
	data_main_window_w->add(*data_disp_parent);
#endif // IF_MOTIF
    }

    // Create data display
    data_disp = new DataDisp(data_disp_parent, data_buttons_w);

#ifdef IF_MOTIF
    if (app_data.separate_data_window)
    {
	// More values for main window
	XtVaSetValues (data_main_window_w,
		       XmNmenuBar,    data_menubar_w,
		       XmNworkWindow, data_disp_parent,
		       XtNIL);
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Set values for data window?
#endif
#endif // IF_MOTIF

    if (data_buttons_w == 0)
	data_buttons_w = make_buttons(data_disp_parent, "data_buttons", 
				      app_data.data_buttons);

    // Source window
    CONTAINER_P source_view_parent = paned_work_w;
    Widget source_menubar_w = 0;
    CONTAINER_P source_main_window_w = 0;
    if (app_data.separate_source_window)
    {
#ifdef IF_MOTIF
	arg = 0;
	XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
	source_view_shell = 
	    verify(XtCreatePopupShell("source_view_shell",
				      topLevelShellWidgetClass,
				      toplevel, args, arg));
	AddDeleteWindowCallback(source_view_shell, DDDCloseCB);
#else // NOT IF_MOTIF
	source_view_shell = new Gtk::Window;
	source_view_shell->set_name(XMST("source_view_shell"));
	source_view_shell->set_title(XMST("source_view_shell"));
	AddDeleteWindowCallback(source_view_shell, BIND_1(PTR_FUN(DDDCloseCB), source_view_shell));
#endif // IF_MOTIF


#ifdef IF_MOTIF
	arg = 0;
	source_main_window_w = 
	    XmCreateMainWindow(source_view_shell,
			       XMST("source_main_window"),
			       args, arg);
	XtManageChild(source_main_window_w);
#else // NOT IF_MOTIF
	source_main_window_w = new Gtk::VBox();
	source_main_window_w->set_name(XMST("source_main_window"));
	source_view_shell->add(*source_main_window_w);
	source_main_window_w->show();
#endif // IF_MOTIF

	// Add menu bar
	source_menubar_w = 
	    MMcreateMenuBar (source_main_window_w, "menubar", source_menubar);
	MMaddCallbacks(source_menubar);
	MMaddHelpCallback(menubar, PTR_FUN(ImmediateHelpCB));
	verify_buttons(source_menubar);
	register_menu_shell(source_menubar);

	// Add source window
#ifdef IF_MOTIF
	arg = 0;
	XtSetArg(args[arg], XmNborderWidth,     0); arg++;
	XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
	XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
	source_view_parent = 
	    verify(XmCreatePanedWindow(source_main_window_w, 
				       XMST("source_paned_work_w"), 
				       args, arg));
	XtManageChild(source_view_parent);
#else // NOT IF_MOTIF
	// Note: On Motif it is possible to force a pane to have
	// fixed size.  On Gtk this does not seem possible.  Therefore
	// the toolbar and status bar must go in a VBox.
	source_view_parent = new Gtk::VBox();
	source_view_parent->show();
	source_main_window_w->add(*source_view_parent);
#endif // IF_MOTIF

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

#ifndef IF_MOTIF
    source_view_parent->modify_bg(Gtk::STATE_NORMAL, Gdk::Color("white"));
#endif
    source_view = new SourceView(source_view_parent);
#ifdef IF_MOTIF
    source_view->set_max_glyphs(app_data.max_glyphs);
    source_view->cache_glyph_images = app_data.cache_glyph_images;
#endif // IF_MOTIF

#ifdef IF_MOTIF
    if (app_data.separate_source_window)
    {
	// More values for main window
	XtVaSetValues (source_main_window_w,
		       XmNmenuBar,    source_menubar_w,
		       XmNworkWindow, source_view_parent,
		       XtNIL);
    }
#endif // IF_MOTIF

    // Source toolbar
    if (arg_cmd_w == 0)
	arg_cmd_w = create_toolbar(source_view_parent, "source",
				   arg_cmd_area, 0, arg_label, source_arg,
				   label_type);
#ifdef NAG_ME
#warning In general, it is better to connect() to a class method.
#endif
#ifdef IF_MOTIF
    XtAddCallback(arg_label, XmNactivateCallback, 
		  ClearTextFieldCB, source_arg->text());
#else // NOT IF_MOTIF
    arg_label->signal_activate().connect(sigc::bind(PTR_FUN(ClearTextFieldCB),
						    source_arg->text()));
#endif // IF_MOTIF

    if (DataDisp::graph_cmd_w == arg_cmd_w)
    {
	// Common toolbar
#ifdef IF_MOTIF
	XtAddCallback(arg_label, XmNactivateCallback, 
		      DataDisp::SelectionLostCB, XtNIL);
#else // NOT IF_MOTIF
	arg_label->signal_activate().connect(PTR_FUN(DataDisp::SelectionLostCB));
#endif // IF_MOTIF
    }

#ifdef IF_MOTIF
    XtAddCallback(source_arg->text(), XmNactivateCallback, 
		  ActivateCB, 
		  XtPointer(arg_cmd_area[ArgItems::Lookup].widget));
#else // NOT IF_MOTIF
    //source_arg->text()->signal_changed().connect(sigc::bind(PTR_FUN(ActivateCB),
    //							    arg_cmd_area[ArgItems::Lookup].widget));
#ifdef NAG_ME
#warning How to activate combobox?
#endif
#endif // IF_MOTIF
    sync_args(source_arg, data_disp->graph_arg);

    if (data_disp->graph_arg != 0) {
#ifdef IF_MOTIF
	XtAddCallback(data_disp->graph_arg->text(), XmNactivateCallback, 
		      ActivateCB, 
		      XtPointer(data_disp->graph_cmd_area[0].widget));
#else // NOT IF_MOTIF
	// data_disp->graph_arg->text()->signal_changed().connect(sigc::bind(PTR_FUN(ActivateCB),
	//								  data_disp->graph_cmd_area[0].widget));
#ifdef NAG_ME
#warning How to activate combobox widget?
#endif
#endif // IF_MOTIF
    }

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

#ifdef IF_MOTIF
    arg = 0;
    gdb_w = verify(XmCreateScrolledText(paned_work_w, 
					XMST("gdb_w"), args, arg));
#else // NOT IF_MOTIF
    gdb_w = new GtkScrolledText();
    gdb_w->set_size_request(400, 200);
    paned_work_w->add(*gdb_w);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtAddCallback (gdb_w,
		   XmNmodifyVerifyCallback,
		   gdbModifyCB,
		   XtNIL);
    XtAddCallback (gdb_w,
		   XmNmotionVerifyCallback,
		   gdbMotionCB,
		   XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning How to verify changes and motion?
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtAddCallback (gdb_w,
		   XmNvalueChangedCallback,
		   gdbChangeCB,
		   XtNIL);
#else // NOT IF_MOTIF
    gdb_w->signal_changed().connect(sigc::bind(PTR_FUN(gdbChangeCB), gdb_w));
#endif // IF_MOTIF
    XtManageChild (gdb_w);

#ifdef IF_MOTIF
    if (!app_data.separate_source_window || !app_data.separate_data_window)
    {
	// Don't resize the debugger console when resizing the main
	// window - resize source and/or data windows instead
	XtVaSetValues(XtParent(gdb_w), XmNskipAdjust, True, XtNIL);
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning skipAdjust?
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Set up the scrolled window
    XtVaSetValues(XtParent(gdb_w),
		  XmNspacing,         0,
		  XmNborderWidth,     0,
		  XmNshadowThickness, 0,
		  XtNIL);
#endif // IF_MOTIF

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
	create_status(main_vbox);

    // Paned Window is done
    XtManageChild (paned_work_w);

#ifdef IF_MOTIF
    // More values for main window
    XtVaSetValues (main_window,
		   XmNmenuBar,    menubar_w,
		   XmNworkWindow, paned_work_w,
		   XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Setup areas for main window
#endif
#endif // IF_MOTIF


    // All main widgets (except shells) are created at this point.

    // Setup theme manager
    setup_theme_manager();
    
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

#ifdef IF_MOTIF
    // Setup insertion position
    promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, messagePosition);
#else // NOT IF_MOTIF
    promptPosition = messagePosition = gdb_w->get_last_position();
    gdb_w->set_insertion_position(messagePosition);
#endif // IF_MOTIF

    // Setup help hooks
#ifdef IF_MOTIF
    PreHelpOnContextHook = PreHelpOnContext;
    PostHelpOnItemHook   = PostHelpOnItem;
#endif // IF_MOTIF

    // Setup version info
    setup_version_info();

    // Customize `settings' title.
    set_settings_title(command_edit_menu[EditItems::Settings].widget);
    set_settings_title(source_edit_menu[EditItems::Settings].widget);
    set_settings_title(data_edit_menu[EditItems::Settings].widget);

    // If we use annotations, we also want tty mode.
    if (app_data.annotate)
	app_data.tty_mode = True;

    // Close windows explicitly requested
    if (!app_data.separate_data_window && 
	!app_data.data_window && !app_data.annotate)
    {
	// We don't want the data window.
	gdbCloseDataWindowCB(CB_ARGS_1(gdb_w));
    }

    if (!app_data.separate_source_window && 
	(!app_data.source_window || app_data.annotate))
    {
	// We don't need the source window, since we're invoked by Emacs.
	gdbCloseSourceWindowCB(CB_ARGS_1(gdb_w));

	if (!app_data.disassemble)
	    gdbCloseToolWindowCB(CB_ARGS_NULL);
    }

    if (!app_data.disassemble)
    {
	// We don't disassemble.
	gdbCloseCodeWindowCB(CB_ARGS_1(gdb_w));
    }

    if ((!app_data.separate_source_window && have_source_window() || 
	 !app_data.separate_data_window && have_data_window()) &&
	(!app_data.debugger_console || app_data.tty_mode))
    {
	// We don't need the debugger console, since we have a TTY.
	gdbCloseCommandWindowCB(CB_ARGS_1(gdb_w));
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

#ifdef IF_MOTIF
    // Remove unnecessary sashes
    untraverse_sashes(source_view_parent);
#endif // IF_MOTIF

    // The sash in the source view is kept, as it separates source and
    // assembler code windows.
#if 0
    if (source_view_shell)
       unmanage_sashes(source_view_parent);
#endif

#ifdef IF_MOTIF
    untraverse_sashes(data_disp_parent);
    if (data_disp_shell)
	unmanage_sashes(data_disp_parent);

    untraverse_sashes(paned_work_w);
    if (source_view_shell && data_disp_shell)
	unmanage_sashes(paned_work_w);
#endif // IF_MOTIF

    // Create subshells.  We do this after the main window has been
    // realized, since LessTif won't make the shells transient otherwise.
    source_view->create_shells();
    data_disp->create_shells();
    set_shortcut_menu(data_disp);

    // Create preference panels
    make_preferences(paned_work_w);

    // Save option states
    save_option_state();

    // Setup option states
    setup_options();
    update_options();

    // Setup info buttons

#ifdef IF_MOTIF
    // Our info routines require that the widgets be named after the
    // info command.  Ugly hack.
    const string tmp_1_ = gdb->info_locals_command();
    strcpy(XtName(locals_w), tmp_1_.chars());
    const string tmp_2_ = gdb->info_args_command();
    strcpy(XtName(args_w),   tmp_2_.chars());
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Ugly hack for info locals and info args?
#endif
#endif // IF_MOTIF
    
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

#ifdef IF_MOTIF
    // Startup shells
    XtVaGetValues(toplevel, XmNiconic, &iconic, XtNIL);
#else // NOT IF_MOTIF
    // FIXME: Can the Gtk app start iconified?
    iconic = false;
#endif // IF_MOTIF
    if (iconic)
    {
	// Startup command shell iconified; others follow as needed
	initial_popup_shell(command_shell);
    }
    else if (!app_data.debugger_console || app_data.tty_mode)
    {
	// Debugger console is closed: wait for source to pop up
    }
    else
    {
	// Popup the command shell only; other shells follow as needed
	initial_popup_shell(command_shell);
    }

#ifdef IF_MOTIF
    // Trace positions and visibility of all DDD windows
    if (command_shell)
	XtAddEventHandler(command_shell, STRUCTURE_MASK, False,
			  StructureNotifyEH, XtNIL);
    if (source_view_shell)
	XtAddEventHandler(source_view_shell, STRUCTURE_MASK, False,
			  StructureNotifyEH, XtNIL);
    if (data_disp_shell)
	XtAddEventHandler(data_disp_shell, STRUCTURE_MASK, False,
			  StructureNotifyEH, XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No urgent need to track window positions
#endif
#endif // IF_MOTIF

#if 0
    // Wait for the command shell to be mapped, such that we don't
    // lose debugger output.  This also decreases system load on
    // single-processor machines since DDD is idle when the debugger
    // starts.
    wait_until_mapped(command_shell);
#endif

    // Setup TTY interface
    setup_command_tty();

    // Raise core limit if needed; required for getting session info.
    // Note: this must be done before starting GDB.
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

    // Make sure we see all messages accumulated so far
    {
	string msg(messages);
	while (!msg.empty())
	{
	    string line = msg.before('\n');
	    set_status(line);
	    msg = msg.after('\n');
	}
    }
    return DDD_CONTINUE;
}


int main(int argc, char *argv[])
{
    // As this is a C++ program, execution does not begin here.  At
    // this point, all global data objects already have been properly
    // initialized.

    switch(pre_main_loop(argc, argv)){
    case DDD_EXIT_FAILURE:
      return EXIT_FAILURE;
    case DDD_EXIT_SUCCESS:
      return EXIT_SUCCESS;
    case DDD_CONTINUE:
      break;
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

    // Restart blinker
    blink(gdb->recording() || !gdb->isReadyWithPrompt());

#ifdef IF_MOTIF
    // Get X event
    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);

    XEvent event;
    XtAppNextEvent(app_context, &event);

    // Check for window creation.
    SwallowerCheckEvents();

    // Check for grabs.
    switch (event.type)
    {
    case MotionNotify:
    case ButtonPress:
    case ButtonRelease:
    case EnterNotify:
    case LeaveNotify:
    case KeyPress:
    case KeyRelease:

	// User interaction - the pointer cannot be grabbed,
	// since we receive input events.
	check_grabs(false);
	break;

    default:
	break;
    }

    // Dispatch event.
    XtDispatchEvent(&event);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Cannot check events before dispatch?
#endif

    Gtk::Main::iteration();
#endif // IF_MOTIF

    // Update tear-off menus.
    gdbUpdateAllMenus();

#if HAVE_EXCEPTIONS
    }
#if HAVE_EXCEPTION && HAVE_STD_EXCEPTIONS && HAVE_TYPEINFO
    // Standard library exception: get its type and diagnostics.
    catch (const std::exception& err)
    {
	ddd_show_exception(typeid(err).name(), err.what());
    }
#endif // HAVE_EXCEPTION && HAVE_STD_EXCEPTIONS && HAVE_TYPEINFO
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

#ifdef IF_MOTIF
    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);
    while (XtAppPending(app_context) & XtIMXEvent)
	process_next_event();
#else // NOT IF_MOTIF
    while (Gtk::Main::events_pending())
	process_next_event();
#endif // IF_MOTIF
}


//-----------------------------------------------------------------------------
// Check if interaction events (key or mouse) are pending
//-----------------------------------------------------------------------------

static bool pending_interaction()
{
#ifdef IF_MOTIF
    XEvent event;
    const long mask = KeyPressMask | ButtonMotionMask | ButtonPressMask;
    Bool pending = XCheckMaskEvent(XtDisplay(command_shell), mask, &event);
    if (pending)
	XPutBackEvent(XtDisplay(command_shell), &event);
    return pending;
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Implement pending_interation?
#endif

    return false;
#endif // IF_MOTIF
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

    if (!restart_session().empty())
	return; // We told the user in the previous DDD instance

    // Tell user once more about version mismatches
    if (!version_warnings.isEmpty())
    {
#ifdef IF_MOTIF
	Arg args[10];
	int arg = 0;

	XtSetArg(args[arg], XmNmessageString, 
		 version_warnings.xmstring()); arg++;
	Widget warning = 
	    verify(XmCreateWarningDialog(command_shell, 
					 XMST("bad_version_warning"),
					 args, arg));
	Delay::register_shell(warning);
	XtUnmanageChild(XmMessageBoxGetChild(warning, XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(warning, XmNhelpCallback, ImmediateHelpCB, XtNIL);
#else // NOT IF_MOTIF
	Widget warning = new Gtk::MessageDialog(*command_shell,
						version_warnings.xmstring());
#ifdef NAG_ME
#warning HelpCB?
#endif
#endif // IF_MOTIF

	manage_and_raise(warning);
    }

    if (app_data.dddinit_version == 0 ||
	string(app_data.dddinit_version) != DDD_VERSION)
    {
	// We have no ~/.ddd/init file or an old one: show version info
#ifdef IF_MOTIF
	HelpOnVersionCB(command_shell, 0, 0);
#endif // IF_MOTIF

	// We have no ~/.ddd/init file: create a simple one
	if (app_data.dddinit_version == 0)
	{
	    static string session = app_data.session;
	    app_data.session = DEFAULT_SESSION.chars();
	    save_options(CREATE_OPTIONS);
	    app_data.session = session.chars();
	}
    }

    // Check for expired versions
    if (ddd_expired())
    {
	std::ostringstream msg;
	msg << "This " DDD_NAME " version (" DDD_VERSION ") has expired since "
	    << ddd_expiration_date() << ".\n"
	    << "Please upgrade to a more recent " DDD_NAME " version.";

	post_warning(string(msg), "expired_warning");
    }

#ifdef IF_MOTIF
    if (app_data.startup_tips)
	TipOfTheDayCB(gdb_w);
#endif // IF_MOTIF
}

// Read in database from FILENAME.  Upon version mismatch, ignore some
// resources such as window sizes.
XrmDatabase GetFileDatabase(const string& filename)
{
    const string tmpfile = tempfile();

    std::ofstream os(tmpfile.chars());
    std::ifstream is(filename.chars());

#if 0
    std::clog << "Copying " << filename.chars() << " to " << tmpfile << "\n";
#endif

#ifdef IF_MOTIF
    // Resources to ignore upon copying
    static const char * const do_not_copy[] = 
    { 
	XmNwidth, XmNheight,	              // Shell sizes
	XmNcolumns, XmNrows,	              // Text window sizes
	XtNtoolRightOffset, XtNtoolTopOffset, // Command tool offset
	XtNshowHints,		              // Show edge hints
    };
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Resource database not defined
#endif
#endif // IF_MOTIF

    bool version_mismatch = false;
    while (is)
    {
	char _line[ARG_MAX];
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
	        string version_found = line.after(":");
		strip_space(version_found);

		if (version_found != DDD_VERSION)
		    version_mismatch = true;
	    }

#define XtNdisplayShortcuts "displayShortcuts"

	    if (line.contains(XtNdisplayShortcuts ":"))
	    {
		line.gsub(XtNdisplayShortcuts ":", XtNgdbDisplayShortcuts ":");
	    }

#ifdef IF_MOTIF
	    for (int i = 0; copy && i < int(XtNumber(do_not_copy)); i++)
	    {
		string res(".");
		res += do_not_copy[i];
		res += ":";
		if (line.contains(res) && version_mismatch)
		{
#if 0
		    std::cerr << "Warning: ignoring " << line 
			 << " in " << filename.chars() << "\n";
#endif
		    copy = false;
		}
	    }
#endif // IF_MOTIF
	}

	if (copy)
	    os << line << '\n';
    }

    // Flush them all
    os.close();
    is.close();

    XrmDatabase db;
    if (version_mismatch)
    {
	// Read database from filtered file
	db = XrmGetFileDatabase(tmpfile.chars());
    }
    else
    {
	// No version mismatch - read from original file
	db = XrmGetFileDatabase(filename.chars());
    }

    unlink(tmpfile.chars());
    return db;
}



//-----------------------------------------------------------------------------
// Install DDD log
//-----------------------------------------------------------------------------

static std::ostringstream devnull;
std::ostream *_dddlog = &devnull;

void init_dddlog()
{
    if (_dddlog != &devnull)
	return;			// Already initialized

    if (app_data.trace)
    {
	_dddlog = &std::clog;
    }
    else
    {
        const string tmp_1_ = session_log_file();
	static std::ofstream log(tmp_1_.chars());
	_dddlog = &log;
    }

    show_configuration(dddlog);

    dddlog << "$ ";
    for (int i = 0; saved_argv()[i] != 0; i++)
	dddlog << " " << cook(saved_argv()[i]);
    dddlog << '\n';

    dddlog.flush();
}

//-----------------------------------------------------------------------------
// Install button tips
//-----------------------------------------------------------------------------

static void install_button_tips()
{
#ifdef IF_MOTIF
    const WidgetArray& shells = Delay::shells();
    for (int i = 0; i < shells.size(); i++)
    {
	Widget shell = shells[i];
	while (shell && !XmIsVendorShell(shell) && !XmIsMenuShell(shell))
	    shell = XtParent(shell);
	if (shell)
	    InstallButtonTips(shell);
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Do not install button tips for now
#endif
#endif // IF_MOTIF
}

void register_menu_shell(const MMDesc *items)
{
#ifdef IF_MOTIF
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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Delay not defined yet
#endif
#endif // IF_MOTIF
}


//-----------------------------------------------------------------------------
// Verify buttons
//-----------------------------------------------------------------------------

static void verify_button(const MMDesc *item, XtPointer)
{
    verify_button(item->widget);
}

static void verify_buttons(const MMDesc *items)
{
    MMonItems(items, verify_button);
}

//-----------------------------------------------------------------------------
// Create DataDisp shortcut menu
//-----------------------------------------------------------------------------

static void set_shortcut_menu(DataDisp *data_disp, const string& exprs)
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

	if (item.empty())
	    continue;

	items_s  += item;
	labels_s += label;
    }

    data_disp->set_shortcut_menu(items_s, labels_s);

    delete[] items;
}

static void set_shortcut_menu(DataDisp *data_disp)
{
    string display_shortcuts;
    switch (gdb->type())
    {
    case BASH: display_shortcuts = app_data.bash_display_shortcuts; break;
    case DBG:  display_shortcuts = app_data.dbg_display_shortcuts;  break;
    case DBX:  display_shortcuts = app_data.dbx_display_shortcuts;  break;
    case GDB:  display_shortcuts = app_data.gdb_display_shortcuts;  break;
    case JDB:  display_shortcuts = app_data.jdb_display_shortcuts;  break;
    case PERL: display_shortcuts = app_data.perl_display_shortcuts; break;
    case PYDB: display_shortcuts = app_data.pydb_display_shortcuts; break;
    case XDB:  display_shortcuts = app_data.xdb_display_shortcuts;  break;
    }
    set_shortcut_menu(data_disp, display_shortcuts);
}

//-----------------------------------------------------------------------------
// Fix size of status line
//-----------------------------------------------------------------------------

static void fix_status_size()
{
#ifdef IF_MOTIF
    if (!app_data.status_at_bottom)
	return;

    Widget status_form = XtParent(status_w);
    if (!XtIsRealized(status_form))
	return;

    Dimension pane_maximum, height;
    XtVaGetValues(status_form,
		  XmNpaneMaximum, &pane_maximum, 
		  XmNheight, &height,
		  XtNIL);

    if (height <= pane_maximum)
	return;

    XtVaSetValues(status_form,
		  XmNallowResize, True,
		  XmNheight, pane_maximum,
		  XmNallowResize, False,
		  XtNIL);

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
		      XtNIL);

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
		    XtVaGetValues(sash,  XmNy, &sash_y, XtNIL);
		    XtVaGetValues(child, XmNy, &child_y, XtNIL);
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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning fix_status_size is not needed?
#endif
#endif // IF_MOTIF
}


//---------------------------------------------------------------------------
// Locking stuff
//---------------------------------------------------------------------------

static bool continue_despite_lock = false;
static int lock_dialog_x = -1;
static int lock_dialog_y = -1;

static void ContinueDespiteLockCB(CB_ALIST_NULL)
{
    continue_despite_lock = true;
}

#ifdef IF_MOTIF
static void TryLock(XtPointer client_data, XtIntervalId *)
#else // NOT IF_MOTIF
static bool TryLock(XtPointer client_data)
#endif // IF_MOTIF
{
    Widget w = (Widget)client_data;

    LockInfo info;
    bool lock_ok = lock_session_dir(XtDisplay(w), DEFAULT_SESSION, info);

    if (lock_ok)
    {
	continue_despite_lock = true;
	return MAYBE_FALSE;
    }

#ifdef IF_MOTIF
    XtAppAddTimeOut(XtWidgetToApplicationContext(w), 500, 
		    PTR_FUN(TryLock), client_data);
#else // NOT IF_MOTIF
    static sigc::connection trylock_conn;
    if (!trylock_conn)
      trylock_conn = Glib::signal_timeout().connect(sigc::bind(PTR_FUN(TryLock), client_data), 500);
    return true;
#endif // IF_MOTIF
}

#if XmVersion >= 1002
static void KillLockerCB(CB_ARG_LIST_12(w, client_data))
{
    static int attempts_to_kill = 0;

    LockInfo *info = (LockInfo *)client_data;

    kill(info->pid, SIGHUP);

    if (attempts_to_kill++ == 0)
    {
	// Try locking again until successful
#ifdef IF_MOTIF
	TryLock(XtPointer(w), 0);
#else // NOT IF_MOTIF
	TryLock(XtPointer(w));
#endif // IF_MOTIF
    }
}
#endif

static bool lock_ddd(Widget parent, LockInfo& info)
{
    bool lock_ok = lock_session_dir(XtDisplay(parent), DEFAULT_SESSION, info);
    if (lock_ok)
	return true;

    if (!app_data.warn_if_locked)
	return false;

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
#ifdef NAG_ME
#warning XmVersion?
#endif
#if XmVersion >= 1002
    if (on_local_host)
	msg += rm("To kill the other " DDD_NAME " instance and continue, ")
	    + rm("click on ") + bf("Kill") + rm(".") + cr();
#endif

    msg += rm("To exit this " DDD_NAME " instance, click on ") 
	+ bf("Exit") + rm(".");

#ifdef IF_MOTIF
    Arg args[10];
    int arg = 0;
	
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
    XtSetArg(args[arg], XmNdialogStyle, 
	     XmDIALOG_FULL_APPLICATION_MODAL); arg++;

    string geometry;
    if (lock_dialog_x >= 0 && lock_dialog_y >= 0)
    {
	std::ostringstream os;
	os << "+" << lock_dialog_x << "+" << lock_dialog_y;
	geometry = string(os);
	XtSetArg(args[arg], XmNgeometry, geometry.chars()); arg++;
	XtSetArg(args[arg], XmNx, lock_dialog_x); arg++;
	XtSetArg(args[arg], XmNy, lock_dialog_y); arg++;
    }

    Widget lock_dialog =
	verify(XmCreateQuestionDialog(parent, XMST("lock_dialog"), 
				      args, arg));
#else // NOT IF_MOTIF
    Gtk::Dialog *lock_dialog = new Gtk::Dialog(XMST("lock_dialog"), *find_shell(parent));
#endif // IF_MOTIF

    Delay::register_shell(lock_dialog);

#ifdef IF_MOTIF
#if XmVersion >= 1002
    Widget kill = verify(XmCreatePushButton(lock_dialog, XMST("kill"), 
					    0, 0));
    XtManageChild(kill);
    XtAddCallback(kill, XmNactivateCallback,
		  KillLockerCB, XtPointer(&info));
    set_sensitive(kill, on_local_host);
#endif

    XtAddCallback(lock_dialog, XmNhelpCallback,
		  ImmediateHelpCB, XtNIL);
    XtAddCallback(lock_dialog, XmNokCallback,
		  ContinueDespiteLockCB, XtNIL);
    XtAddCallback(lock_dialog, XmNcancelCallback, ExitCB, 
		  XtPointer(EXIT_FAILURE));
#else // NOT IF_MOTIF
    Gtk::Button *button;
    button = lock_dialog->add_button(XMST("kill"), 0);
    button->signal_clicked().connect(sigc::bind(PTR_FUN(KillLockerCB), lock_dialog, XtPointer(&info)));
    button = lock_dialog->add_button(XMST("continue"), 0);
    button->signal_clicked().connect(PTR_FUN(ContinueDespiteLockCB));
    button = lock_dialog->add_button(XMST("cancel"), 0);
    button->signal_clicked().connect(sigc::bind(PTR_FUN(ExitCB), EXIT_FAILURE));
#endif // IF_MOTIF

#ifdef IF_MOTIF
    if (!geometry.empty())
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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No geometry settings yet
#endif
#endif // IF_MOTIF

    continue_despite_lock = false;

#ifdef IF_MOTIF
    manage_and_raise(lock_dialog);

    while (!continue_despite_lock)
	XtAppProcessEvent(XtWidgetToApplicationContext(lock_dialog), XtIMAll);

    XtDestroyWidget(lock_dialog);
#else // NOT IF_MOTIF
    lock_dialog->run();
#endif // IF_MOTIF

    // Try locking once more
    return lock_session_dir(XtDisplay(parent), DEFAULT_SESSION, info);
}


//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------

static Boolean session_setup_done(
#ifdef IF_MOTIF
				  XtPointer
#endif // IF_MOTIF
				  )
{
    if (emptyCommandQueue() && gdb->isReadyWithPrompt())
    {
	// Delete initialization delay, if any
	delete init_delay;
	init_delay = 0;

	// Say hello
	set_status_mstring(rm("Welcome to " DDD_NAME " " DDD_VERSION 
			      " \"" DDD_NICKNAME "\""
			      " (" DDD_HOST ")"));

	if (app_data.initial_session != 0)
	{
	    set_session(app_data.initial_session);
	    app_data.initial_session = 0;
	}

	// From now on, verify the state file for changes
	check_options_file();

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

	if (running_shells() == 0 ||
	    app_data.annotate && running_shells() == 1)
	{
	    // We have no shell (yet).  Be sure to popup at least one shell.
	    if (app_data.annotate)
		gdbOpenDataWindowCB(CB_ARGS_NULL);
	    else if (app_data.source_window)
		gdbOpenSourceWindowCB(CB_ARGS_NULL);
	    else if (app_data.data_window)
		gdbOpenDataWindowCB(CB_ARGS_NULL);
	    else
		gdbOpenCommandWindowCB(CB_ARGS_NULL);
	}

	main_loop_entered = true;

	DispBox::init_vsllib(process_pending_events);
	DataDisp::refresh_graph_edit();

	if (init_delay != 0 || app_data.initial_session != 0)
	{
	    // Restoring session may still take time
#ifdef IF_MOTIF
	    XtAppAddWorkProc(XtWidgetToApplicationContext(command_shell),
			     session_setup_done, 0);
#else // NOT IF_MOTIF
	    Glib::signal_idle().connect(PTR_FUN(session_setup_done));
#endif // IF_MOTIF
	}
	return True;		// Remove from the list of work procs
    }

    return False;		// Keep on processing the command queue
}



//-----------------------------------------------------------------------------
// Activate argument field
//-----------------------------------------------------------------------------

static void ActivateCB(Widget, XtPointer client_data, XtPointer call_data)
{
#ifdef IF_MOTIF
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
    
    Widget button = Widget(client_data);
    XtCallActionProc(button, "ArmAndActivate", cbs->event, (String *)0, 0);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning How do it call ArmAndActivate?
#endif
#endif // IF_MOTIF
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

#ifdef IF_MOTIF
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
    XtVaGetValues(w, XmNset, &old_state, XtNIL);

    if (old_state != new_state)
    {
	if (notify)
	    XmToggleButtonSetState(w, new_state, True);
	else
	    XtVaSetValues(w, XmNset, new_state, XtNIL);
    }
}

#else // NOT IF_MOTIF

static void set_toggle(Widget w, bool new_state, Widget ignore=NULL)
{
#ifdef NAG_ME
#warning set_toggle is sometimes called with a base Widget *
#endif
#ifdef NAG_ME
#warning set_toggle: "notify" is ignored
#endif

    if (w == 0)
	return;

    if (w == ignore)
	return;

    Gtk::ToggleButton *tb = dynamic_cast<Gtk::ToggleButton *>(w);
    if (tb) {
	if (tb->get_active() != new_state)
	{
	    tb->set_active(new_state);
	}
    }
    Gtk::CheckMenuItem *cmi = dynamic_cast<Gtk::CheckMenuItem *>(w);
    if (cmi) {
	if (cmi->get_active() != new_state)
	{
	    cmi->set_active(new_state);
	}
    }
}

#endif // IF_MOTIF

#ifdef IF_MOTIF
inline void notify_set_toggle(Widget w, Boolean new_state)
{
    set_toggle(w, new_state, true);
}
#else // NOT IF_MOTIF
inline void notify_set_toggle(Widget w, Boolean new_state)
{
    set_toggle(w, new_state, NULL);
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
static void set_string(Widget w, const _XtString value)
{
    if (w == 0)
	return;

    // Note: XtVaSetValues(w, XmNvalue, value, ...) 
    // doesn't work properly with LessTif 0.89.9
    XmTextFieldSetString(w, XMST(value));
    XtVaSetValues(w, XmNcursorPosition, 0, XtNIL);
}
#else // NOT IF_MOTIF
static void set_string(Gtk::Entry *w, const _XtString value)
{
    if (w == 0)
	return;

    w->set_text(XMST(value));
}
#endif // IF_MOTIF

static void set_string_int(ENTRY_P w, int value)
{
    string v = itostring(value);
    set_string(w, v.chars());
}

static bool have_cmd(const string& cmd)
{
    return cmd_file(cmd).contains('/', 0);
}

#ifdef IF_MOTIF
static void set_scale(Widget w, int val)
{
    if (w == 0)
	return;

    assert(XmIsScale(w));

    XtVaSetValues(w, XmNvalue, val, XtNIL);
}
#else // NOT IF_MOTIF
static void set_scale(Gtk::Scale *w, int val)
{
    if (w == 0)
	return;

    w->set_value(val);
}
#endif // IF_MOTIF

// Reflect state in option menus
void update_options(bool noupd)
{
#ifndef IF_MOTIF
    if (noupd)
	return;
#endif // IF_MOTIF
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

    set_toggle(command_save_options_w,   app_data.save_options_on_exit);
    set_toggle(source_save_options_w,    app_data.save_options_on_exit);
    set_toggle(data_save_options_w,      app_data.save_options_on_exit);

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
    set_toggle(warn_if_locked_w,         app_data.warn_if_locked);

    set_toggle(builtin_plot_window_w,
	       string(app_data.plot_term_type) == "xlib");
    set_toggle(extern_plot_window_w,
	       string(app_data.plot_term_type) == "x11");

    set_toggle(cache_source_files_w,     app_data.cache_source_files);
    set_toggle(cache_machine_code_w,     app_data.cache_machine_code);
    set_toggle(set_display_glyphs_w,     app_data.display_glyphs);
    set_toggle(set_display_text_w,       !app_data.display_glyphs);
    set_toggle(set_refer_path_w,         app_data.use_source_path);
    set_toggle(set_refer_base_w,         !app_data.use_source_path);
    set_toggle(line_numbers1_w,          app_data.display_line_numbers);
    set_toggle(line_numbers2_w,          app_data.display_line_numbers);

    if (tab_width_w != 0)
    {
	set_scale(tab_width_w, app_data.tab_width);
	set_scale(source_indent_w, app_data.indent_source);
	set_scale(code_indent_w,   app_data.indent_code);
    }

    set_toggle(led_w, app_data.blink_while_busy);

    Boolean show_grid, snap_to_grid, show_hints, auto_layout, show_annotations;
    LayoutMode layout_mode;
    Dimension grid_width, grid_height;

#ifdef IF_MOTIF
    XtVaGetValues(data_disp->graph_edit, 
 		  XtNshowGrid,   &show_grid,
 		  XtNsnapToGrid, &snap_to_grid,
		  XtNshowHints,  &show_hints,
		  XtNshowAnnotations, &show_annotations,
		  XtNautoLayout, &auto_layout,
		  XtNlayoutMode, &layout_mode,
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  XtNIL);
#else // NOT IF_MOTIF
    show_grid = data_disp->graph_edit->get_show_grid();
    snap_to_grid = data_disp->graph_edit->get_snap_to_grid();
    show_hints = data_disp->graph_edit->get_show_hints();
    show_annotations = data_disp->graph_edit->get_show_annotations();
    auto_layout = data_disp->graph_edit->get_auto_layout();
    layout_mode = data_disp->graph_edit->get_layout_mode();
    grid_width = data_disp->graph_edit->get_grid_width();
    grid_height = data_disp->graph_edit->get_grid_height();
#endif // IF_MOTIF

    set_toggle(detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_cluster_displays_w, app_data.cluster_displays);
    set_toggle(graph_align_2d_arrays_w, app_data.align_2d_arrays);

    if (graph_snap_to_grid_w != 0)
    {
	if (!show_grid && XtIsSensitive(graph_snap_to_grid_w))
	{
	    // Grid has been disabled - disable `snap to grid' as well
#ifdef IF_MOTIF
	    XtVaSetValues(data_disp->graph_edit, XtNsnapToGrid, False, 
			  XtNIL);
#else // NOT IF_MOTIF
	    data_disp->graph_edit->set_snap_to_grid(false);
#endif // IF_MOTIF
	}
	else if (show_grid && !XtIsSensitive(graph_snap_to_grid_w))
	{
	    // Grid has been re-enabled - restore `snap to grid' setting
#ifdef IF_MOTIF
	    XtVaSetValues(data_disp->graph_edit, 
			  XtNsnapToGrid, 
			  XmToggleButtonGetState(graph_snap_to_grid_w), 
			  XtNIL);
#else // NOT IF_MOTIF
	    data_disp->graph_edit->set_snap_to_grid(graph_snap_to_grid_w->get_active());
#endif // IF_MOTIF
	}
	else
	{
	    set_toggle(graph_snap_to_grid_w, snap_to_grid);
	}
    }

    set_sensitive(graph_snap_to_grid_w, show_grid);
    set_sensitive(align_w, show_grid);

    set_sensitive(graph_left_to_right_w, !app_data.cluster_displays);
    set_sensitive(graph_top_to_bottom_w, !app_data.cluster_displays);

    set_toggle(graph_left_to_right_w, 
	       app_data.display_placement == XmHORIZONTAL);
    set_toggle(graph_top_to_bottom_w,
	       app_data.display_placement == XmVERTICAL);

    set_toggle(graph_show_hints_w, show_hints);
    set_toggle(graph_show_annotations_w, show_annotations);
    set_toggle(graph_auto_layout_w, auto_layout);
    set_toggle(graph_compact_layout_w, layout_mode == CompactLayoutMode);
    set_toggle(graph_auto_close_w, app_data.auto_close_data_window);
    set_toggle(graph_show_dependent_titles_w,
	       app_data.show_dependent_display_titles);

    if (graph_grid_size_w != 0) {
#ifdef IF_MOTIF
	XtVaSetValues(graph_grid_size_w, XmNvalue, show_grid ? grid_width : 0, 
		      XtNIL);
#else // NOT IF_MOTIF
	graph_grid_size_w->set_value(show_grid ? grid_width : 0);
#endif // IF_MOTIF
    }


#ifdef IF_MOTIF
    unsigned char policy = '\0';
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &policy, 
		  XtNIL);
    set_toggle(set_focus_pointer_w,        policy == XmPOINTER);
    set_toggle(set_focus_explicit_w,       policy == XmEXPLICIT);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Focus policy not implemented
#endif
#endif // IF_MOTIF

    set_toggle(set_scrolling_panner_w,     app_data.panned_graph_editor);
    set_toggle(set_scrolling_scrollbars_w, !app_data.panned_graph_editor);

    set_toggle(set_button_images_w,        app_data.button_images);
    set_toggle(set_button_captions_w,      app_data.button_captions);

    set_toggle(set_flat_buttons_w,         app_data.flat_toolbar_buttons);
    set_sensitive(set_flat_buttons_w,
		  app_data.button_images || app_data.button_captions);

    string button_color_key        = app_data.button_color_key;
    string active_button_color_key = app_data.active_button_color_key;
#if XmVersion < 2000 || !defined(IF_MOTIF)
#ifdef NAG_ME
#warning XmINDETERMINATE has no analogue?
#endif
    set_toggle(set_color_buttons_w, button_color_key == 'c');
#else
    if (button_color_key == 'c' && active_button_color_key == 'c')
	set_toggle(set_color_buttons_w, XmSET);
    else if (button_color_key == active_button_color_key)
	set_toggle(set_color_buttons_w, XmUNSET);
    else
	set_toggle(set_color_buttons_w, XmINDETERMINATE);
#endif
    set_sensitive(set_color_buttons_w, app_data.button_images);

    Boolean separate = 
	app_data.separate_data_window || app_data.separate_source_window;

    set_toggle(set_toolbars_at_bottom_w, app_data.toolbars_at_bottom);
    set_sensitive(set_toolbars_at_bottom_w, separate ||
		  !app_data.button_images && !app_data.button_captions);

    set_toggle(set_tool_buttons_in_toolbar_w,      app_data.command_toolbar);
    set_toggle(set_tool_buttons_in_command_tool_w, !app_data.command_toolbar);

    set_toggle(set_separate_windows_w, separate);
    set_toggle(set_attached_windows_w, !separate);

    DebuggerType debugger_type = DebuggerType(-1);
    get_debugger_type(app_data.debugger, debugger_type);

    set_toggle(set_debugger_bash_w, debugger_type == BASH);
    set_toggle(set_debugger_dbg_w,  debugger_type == DBG);
    set_toggle(set_debugger_dbx_w,  debugger_type == DBX);
    set_toggle(set_debugger_gdb_w,  debugger_type == GDB);
    set_toggle(set_debugger_jdb_w,  debugger_type == JDB);
    set_toggle(set_debugger_perl_w, debugger_type == PERL);
    set_toggle(set_debugger_pydb_w, debugger_type == PYDB);
    set_toggle(set_debugger_xdb_w,  debugger_type == XDB);
    set_toggle(auto_debugger_w, app_data.auto_debugger);

    set_toggle(splash_screen_w, app_data.splash_screen);
#ifdef IF_MOTIF
    set_toggle(startup_tips_w,  app_data.startup_tips);
    set_toggle(set_startup_tips_w, app_data.startup_tips);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Tips not supported
#endif
#endif // IF_MOTIF

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
#ifdef IF_MOTIF
    source_view->set_display_glyphs(app_data.display_glyphs);
#endif // IF_MOTIF
    source_view->set_disassemble(gdb->type() == GDB && app_data.disassemble);
    source_view->set_all_registers(app_data.all_registers);
    source_view->set_tab_width(app_data.tab_width);
    source_view->set_indent(app_data.indent_source, app_data.indent_code);

    source_view->line_indent_amount   = app_data.line_number_width;
    source_view->script_indent_amount = app_data.indent_script;
    source_view->lines_above_cursor   = app_data.lines_above_cursor;
    source_view->lines_below_cursor   = app_data.lines_below_cursor;

    source_view->max_breakpoint_number = app_data.max_breakpoint_number;
    data_disp->max_display_number      = app_data.max_display_number;

    data_disp->set_detect_aliases(app_data.detect_aliases);
    data_disp->set_cluster_displays(app_data.cluster_displays);

    if (DispBox::align_2d_arrays != app_data.align_2d_arrays)
    {
	DispBox::align_2d_arrays = app_data.align_2d_arrays;
	// data_disp->refresh_display();
    }

    // Synchronize layout direction with placement
    switch (app_data.display_placement)
    {
    case XmVERTICAL:
#ifdef IF_MOTIF
	XtVaSetValues(data_disp->graph_edit, XtNrotation, 0, XtNIL);
#else // NOT IF_MOTIF
	data_disp->graph_edit->set_rotation(0);
#endif // IF_MOTIF
	break;
	    
    case XmHORIZONTAL:
#ifdef IF_MOTIF
	XtVaSetValues(data_disp->graph_edit, XtNrotation, 90, XtNIL);
#else // NOT IF_MOTIF
	data_disp->graph_edit->set_rotation(90);
#endif // IF_MOTIF
	break;
    }

    if (app_data.command_toolbar && 
	command_toolbar_w != 0 && !XtIsManaged(command_toolbar_w))
    {
	if (app_data.source_window)
	    XtManageChild(command_toolbar_w);
	gdbCloseToolWindowCB(CB_ARGS_NULL);
    }
    else if (!app_data.command_toolbar && 
	     command_toolbar_w != 0 && XtIsManaged(command_toolbar_w))
    {
	XtUnmanageChild(command_toolbar_w);
	if (app_data.source_window)
	    gdbOpenToolWindowCB(CB_ARGS_NULL);
    }

#ifdef IF_MOTIF
    EnableButtonTips(app_data.button_tips);
    EnableButtonDocs(app_data.button_docs);
    EnableTextTips(app_data.value_tips);
    EnableTextDocs(app_data.value_docs);
#endif // IF_MOTIF

    set_string(edit_command_w,       app_data.edit_command);
    set_string(plot_command_w,       app_data.plot_command);
    set_string(get_core_command_w,   app_data.get_core_command);
    set_string(ps_command_w,         app_data.ps_command);
    set_string(term_command_w,       app_data.term_command);
    set_string(uncompress_command_w, app_data.uncompress_command);
    set_string(www_command_w,        app_data.www_command);

    // Set `find' label
    Widget find_label_ref = 0;
#ifdef IF_MOTIF
    const char *icon = 0;
#else // NOT IF_MOTIF
    XIMAGE_P *icon = 0;
#endif // IF_MOTIF
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
#ifdef IF_MOTIF
    XtVaGetValues(find_label_ref, XmNlabelString, &label, XtNIL);
    MString new_label(label, true);
    XmStringFree(label);
#else // NOT IF_MOTIF
    MString new_label = get_label(find_label_ref);
#endif // IF_MOTIF

    set_label(arg_cmd_area[ArgItems::Find].widget, new_label, icon);

#ifdef IF_MOTIF
    // Font stuff
    if (font_names[DefaultDDDFont] != 0)
    {
	set_string(font_names[DefaultDDDFont], app_data.default_font);
	set_string(font_names[VariableWidthDDDFont], 
		   app_data.variable_width_font);
	set_string(font_names[FixedWidthDDDFont], app_data.fixed_width_font);
	set_string(font_names[DataDDDFont], app_data.data_font);

	set_string_int(font_sizes[DefaultDDDFont], 
		       app_data.default_font_size);
	set_string_int(font_sizes[VariableWidthDDDFont],
		       app_data.variable_width_font_size);
	set_string_int(font_sizes[FixedWidthDDDFont], 
		       app_data.fixed_width_font_size);
	set_string_int(font_sizes[DataDDDFont], 
		       app_data.data_font_size);
    }
#endif // IF_MOTIF

    // Key Bindings
    BindingStyle cut_copy_paste_style = app_data.cut_copy_paste_bindings;
    set_toggle(cut_copy_paste_kde_w,   cut_copy_paste_style == KDEBindings);
    set_toggle(cut_copy_paste_motif_w, cut_copy_paste_style == MotifBindings);

    set_cut_copy_paste_bindings(command_edit_menu, cut_copy_paste_style);
    set_cut_copy_paste_bindings(source_edit_menu,  cut_copy_paste_style);
    set_cut_copy_paste_bindings(data_edit_menu,    cut_copy_paste_style);

    BindingStyle select_all_style = app_data.select_all_bindings;
    set_toggle(select_all_kde_w,   select_all_style == KDEBindings);
    set_toggle(select_all_motif_w, select_all_style == MotifBindings);

    set_select_all_bindings(command_edit_menu, select_all_style);
    set_select_all_bindings(source_edit_menu,  select_all_style);
    set_select_all_bindings(data_edit_menu,    select_all_style);

    // Maintenance
    manage_child(maintenance_w, app_data.maintenance);
    const bool ValgrindLeak = ValgrindLeakBuiltin();
    manage_child(valgrindLeakCheck_w, ValgrindLeak);
    if (ValgrindLeak)
      set_sensitive(valgrindLeakCheck_w, RunningOnValgrind());

    set_toggle(crash_debug_w,
	       app_data.dump_core && app_data.debug_core_dumps);
    set_toggle(crash_dump_core_w, 
	       app_data.dump_core && !app_data.debug_core_dumps);
    set_toggle(crash_nothing_w, !app_data.dump_core);

    // Check for source toolbar
    Widget arg_cmd_w = XtParent(source_arg->top());
    if (data_disp->graph_cmd_w == arg_cmd_w)
    {
	// Don't close the common toolbar
    }
    else
    {
#ifdef IF_MOTIF
	if (XtIsManaged(source_view->source_form()) ||
	    XtIsManaged(source_view->code_form()))
	    manage_paned_child(arg_cmd_w);
	else
	    unmanage_paned_child(arg_cmd_w);
#else // NOT IF_MOTIF
	static int errcnt = 0;
	if (complain && !errcnt++) std::cerr << "Paned children?\n";
#endif // IF_MOTIF
    }

    // Setup undo buffer size
    UndoBuffer::max_history_depth = app_data.max_undo_depth;
    UndoBuffer::max_history_size  = app_data.max_undo_size;

    set_string_int(max_undo_size_w, app_data.max_undo_size / 1000);

    update_reset_preferences();
    fix_status_size();
}

static void set_settings_title(Widget w)
{
    if (w != 0)
    {
	MString settings_title(gdb->title() + " Settings...");
#ifdef IF_MOTIF
	XtVaSetValues(w, XmNlabelString, settings_title.xmstring(), 
		      XtNIL);
#else // NOT IF_MOTIF
	Gtk::MenuItem *mi = dynamic_cast<Gtk::MenuItem *>(w);
	mi->remove();
	mi->add_label(settings_title.xmstring());
#endif // IF_MOTIF
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

static DIALOG_P preferences_dialog;
static BUTTON_P reset_preferences_w;
#ifndef IF_MOTIF
static sigc::connection reset_preferences_connection;
#endif // IF_MOTIF
static Widget current_panel;

void save_option_state()
{
    initial_app_data = app_data;

    // Make sure the helper command strings are really duplicated
    static string edit_command;
    edit_command = initial_app_data.edit_command;
    initial_app_data.edit_command = edit_command.chars();

    static string plot_command;
    plot_command = initial_app_data.plot_command;
    initial_app_data.plot_command = plot_command.chars();

    static string get_core_command;
    get_core_command = initial_app_data.get_core_command;
    initial_app_data.get_core_command = get_core_command.chars();

    static string ps_command;
    ps_command = initial_app_data.ps_command;
    initial_app_data.ps_command = ps_command.chars();

    static string term_command;
    term_command = initial_app_data.term_command;
    initial_app_data.term_command = term_command.chars();

    static string uncompress_command;
    uncompress_command = initial_app_data.uncompress_command;
    initial_app_data.uncompress_command = uncompress_command.chars();

    static string www_command;
    www_command = initial_app_data.www_command;
    initial_app_data.www_command = www_command.chars();

    // Fetch data display resources
#ifdef IF_MOTIF
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowGrid,        &initial_show_grid,
		  XtNshowAnnotations, &initial_show_annotations,
		  XtNshowHints,       &initial_show_hints,
		  XtNsnapToGrid,      &initial_snap_to_grid,
		  XtNlayoutMode,      &initial_layout_mode, 
		  XtNautoLayout,      &initial_auto_layout,
		  XtNgridWidth,       &initial_grid_width,
		  XtNgridHeight,      &initial_grid_height,
		  XtNIL);
#else // NOT IF_MOTIF
    initial_show_grid = data_disp->graph_edit->get_show_grid();
    initial_show_annotations = data_disp->graph_edit->get_show_annotations();
    initial_show_hints = data_disp->graph_edit->get_show_hints();
    initial_snap_to_grid = data_disp->graph_edit->get_snap_to_grid();
    initial_layout_mode = data_disp->graph_edit->get_layout_mode(); 
    initial_auto_layout = data_disp->graph_edit->get_auto_layout();
    initial_grid_width = data_disp->graph_edit->get_grid_width();
    initial_grid_height = data_disp->graph_edit->get_grid_height();
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtVaGetValues(command_shell,
		  XmNkeyboardFocusPolicy, &initial_focus_policy, XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Focus policy not supported
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    Widget paned_work_w = XtParent(XtParent(gdb_w));
    save_preferred_paned_sizes(paned_work_w);
#endif // IF_MOTIF

    option_state_saved = true;
    update_reset_preferences();
}

static void ResetGeneralPreferencesCB(CB_ALIST_NULL)
{
    notify_set_toggle(button_tips_w, initial_app_data.button_tips);
    notify_set_toggle(button_docs_w, initial_app_data.button_docs);
    notify_set_toggle(value_tips_w,  initial_app_data.value_tips);
    notify_set_toggle(value_docs_w,  initial_app_data.value_docs);
    notify_set_toggle(set_global_completion_w, 
	       initial_app_data.global_tab_completion);

    set_string_int(max_undo_size_w, initial_app_data.max_undo_size / 1000);
    app_data.max_undo_size = initial_app_data.max_undo_size;

    notify_set_toggle(set_console_completion_w, 
	       !initial_app_data.global_tab_completion);
    notify_set_toggle(group_iconify_w, initial_app_data.group_iconify);
    notify_set_toggle(uniconify_when_ready_w, 
		      initial_app_data.uniconify_when_ready);
    notify_set_toggle(suppress_warnings_w, initial_app_data.suppress_warnings);
    notify_set_toggle(warn_if_locked_w, initial_app_data.warn_if_locked);
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

    if (app_data.max_undo_size != initial_app_data.max_undo_size)
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

    if (app_data.warn_if_locked != initial_app_data.warn_if_locked)
	return true;

    if (app_data.check_grabs != initial_app_data.check_grabs)
	return true;

    return false;
}

static void ResetSourcePreferencesCB(CB_ALIST_NULL)
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

    notify_set_toggle(line_numbers1_w, 
		      initial_app_data.display_line_numbers);
    notify_set_toggle(line_numbers2_w, 
		      initial_app_data.display_line_numbers);

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

static void ResetDataPreferencesCB(CB_ALIST_NULL)
{
    notify_set_toggle(detect_aliases_w, initial_app_data.detect_aliases);
    notify_set_toggle(graph_detect_aliases_w, initial_app_data.detect_aliases);
    notify_set_toggle(graph_left_to_right_w,
		      initial_app_data.display_placement == XmHORIZONTAL);
    notify_set_toggle(graph_top_to_bottom_w,
		      initial_app_data.display_placement == XmVERTICAL);
    notify_set_toggle(graph_cluster_displays_w, 
		      initial_app_data.cluster_displays);
    notify_set_toggle(graph_align_2d_arrays_w, 
		      initial_app_data.align_2d_arrays);
    notify_set_toggle(graph_show_hints_w, initial_show_hints);
    notify_set_toggle(graph_show_dependent_titles_w, 
		      initial_app_data.show_dependent_display_titles);
    notify_set_toggle(graph_auto_close_w,
		      initial_app_data.auto_close_data_window);

    Boolean show_hints, show_annotations;

#ifdef IF_MOTIF
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowHints,       &show_hints,
		  XtNshowAnnotations, &show_annotations,
		  XtNIL);
#else // NOT IF_MOTIF
    show_hints = data_disp->graph_edit->get_show_hints();
    show_annotations = data_disp->graph_edit->get_show_annotations();
#endif // IF_MOTIF

    if (show_hints  != initial_show_hints ||
	show_annotations != initial_show_annotations)
    {
#ifdef IF_MOTIF
	XtVaSetValues(data_disp->graph_edit,
		      XtNshowHints,       initial_show_hints,
		      XtNshowAnnotations, initial_show_annotations,
		      XtNIL);
#else // NOT IF_MOTIF
	data_disp->graph_edit->set_show_hints(initial_show_hints);
	data_disp->graph_edit->set_show_annotations(initial_show_annotations);
#endif // IF_MOTIF
		      
	update_options();
    }

    notify_set_toggle(graph_compact_layout_w, 
	       initial_layout_mode == CompactLayoutMode);
    notify_set_toggle(graph_auto_layout_w, initial_auto_layout);
    notify_set_toggle(graph_snap_to_grid_w, initial_snap_to_grid);

    Dimension grid_width, grid_height;
    Boolean show_grid;
#ifdef IF_MOTIF
    XtVaGetValues(data_disp->graph_edit, 
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  XtNshowGrid,   &show_grid,
		  XtNIL);
#else // NOT IF_MOTIF
    grid_width = data_disp->graph_edit->get_grid_width();
    grid_height = data_disp->graph_edit->get_grid_height();
    show_grid = data_disp->graph_edit->get_show_grid();
#endif // IF_MOTIF

    if (grid_width  != initial_grid_width || 
	grid_height != initial_grid_height ||
	show_grid   != initial_show_grid)
    {
#ifdef IF_MOTIF
	XtVaSetValues(data_disp->graph_edit,
		      XtNgridWidth,  initial_grid_width,
		      XtNgridHeight, initial_grid_height,
		      XtNshowGrid,   initial_show_grid,
		      XtNIL);
#else // NOT IF_MOTIF
	data_disp->graph_edit->set_grid_width(initial_grid_width);
	data_disp->graph_edit->set_grid_height(initial_grid_height);
	data_disp->graph_edit->set_show_grid(initial_show_grid);
#endif // IF_MOTIF
		      
	update_options();
    }
}

static bool data_preferences_changed()
{
    Boolean show_hints, show_annotations;
#ifdef IF_MOTIF
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowHints,       &show_hints,
		  XtNshowAnnotations, &show_annotations,
		  XtNIL);
#else // NOT IF_MOTIF
    show_hints = data_disp->graph_edit->get_show_hints();
    show_annotations = data_disp->graph_edit->get_show_annotations();
#endif // IF_MOTIF

    if (app_data.detect_aliases != initial_app_data.detect_aliases)
	return true;

    if (app_data.cluster_displays != initial_app_data.cluster_displays)
	return true;

    if (app_data.display_placement != initial_app_data.display_placement)
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
    Boolean show_grid, snap_to_grid, auto_layout;
    LayoutMode layout_mode;
    Dimension grid_width, grid_height;

#ifdef IF_MOTIF
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowGrid,   &show_grid,
		  XtNsnapToGrid, &snap_to_grid,
		  XtNlayoutMode, &layout_mode, 
		  XtNautoLayout, &auto_layout,
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  XtNIL);
#else // NOT IF_MOTIF
    show_grid = data_disp->graph_edit->get_show_grid();
    snap_to_grid = data_disp->graph_edit->get_snap_to_grid();
    layout_mode = data_disp->graph_edit->get_layout_mode(); 
    auto_layout = data_disp->graph_edit->get_auto_layout();
    grid_width = data_disp->graph_edit->get_grid_width();
    grid_height = data_disp->graph_edit->get_grid_height();
#endif // IF_MOTIF

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

static void ResetStartupPreferencesCB(CB_ALIST_NULL)
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
#if XmVersion < 2000 || !defined(IF_MOTIF)
#ifdef NAG_ME
#warning  XmINDETERMINATE has no analogue?
#endif
    notify_set_toggle(set_color_buttons_w, button_color_key == 'c');
#else
    if (button_color_key == 'c' && active_button_color_key == 'c')
	notify_set_toggle(set_color_buttons_w, XmSET);
    else if (button_color_key == active_button_color_key)
	notify_set_toggle(set_color_buttons_w, XmUNSET);
    else
	notify_set_toggle(set_color_buttons_w, XmINDETERMINATE);
#endif
    notify_set_toggle(set_toolbars_at_bottom_w, 
		      initial_app_data.toolbars_at_bottom);

    notify_set_toggle(set_focus_pointer_w, 
		      initial_focus_policy == XmPOINTER);
    notify_set_toggle(set_focus_explicit_w,
		      initial_focus_policy == XmEXPLICIT);

    notify_set_toggle(set_scrolling_panner_w, 
	       initial_app_data.panned_graph_editor);
    notify_set_toggle(set_scrolling_scrollbars_w, 
	       !initial_app_data.panned_graph_editor);

    DebuggerType debugger_type;
    bool type_ok = get_debugger_type(initial_app_data.debugger, debugger_type);
    notify_set_toggle(set_debugger_bash_w, type_ok && debugger_type == BASH);
    notify_set_toggle(set_debugger_dbg_w,  type_ok && debugger_type == DBG);
    notify_set_toggle(set_debugger_dbx_w,  type_ok && debugger_type == DBX);
    notify_set_toggle(set_debugger_gdb_w,  type_ok && debugger_type == GDB);
    notify_set_toggle(set_debugger_jdb_w,  type_ok && debugger_type == JDB);
    notify_set_toggle(set_debugger_pydb_w, type_ok && debugger_type == PYDB);
    notify_set_toggle(set_debugger_xdb_w,  type_ok && debugger_type == XDB);
    notify_set_toggle(set_debugger_perl_w, type_ok && debugger_type == PERL);
    notify_set_toggle(auto_debugger_w,
		      !type_ok || initial_app_data.auto_debugger);

    BindingStyle cut_copy_paste_style = 
	initial_app_data.cut_copy_paste_bindings;
    notify_set_toggle(cut_copy_paste_kde_w,   
		      cut_copy_paste_style == KDEBindings);
    notify_set_toggle(cut_copy_paste_motif_w, 
		      cut_copy_paste_style == MotifBindings);

    BindingStyle select_all_style = 
	initial_app_data.select_all_bindings;
    notify_set_toggle(select_all_kde_w,   
		      select_all_style == KDEBindings);
    notify_set_toggle(select_all_motif_w, 
		      select_all_style == MotifBindings);

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

    if (app_data.select_all_bindings != initial_app_data.select_all_bindings)
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

    if (app_data.toolbars_at_bottom != initial_app_data.toolbars_at_bottom)
	return true;

    if (string(app_data.button_color_key) !=
	string(initial_app_data.button_color_key))
	return true;

    if (string(app_data.active_button_color_key) !=
	string(initial_app_data.active_button_color_key))
	return true;

#ifdef IF_MOTIF
    unsigned char focus_policy;
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &focus_policy, 
		  XtNIL);
    if (focus_policy != initial_focus_policy)
	return true;
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Focus policy not implemented
#endif
#endif // IF_MOTIF

    if (app_data.panned_graph_editor != initial_app_data.panned_graph_editor)
	return true;

    if (app_data.auto_debugger != initial_app_data.auto_debugger)
	return true;

    if (string(app_data.debugger) != string(initial_app_data.debugger))
	return true;

    return false;
}

static void ResetFontPreferencesCB(CB_ALIST_NULL)
{
#ifdef IF_MOTIF
    set_font(DefaultDDDFont,       initial_app_data.default_font);
    set_font(VariableWidthDDDFont, initial_app_data.variable_width_font);
    set_font(FixedWidthDDDFont,    initial_app_data.fixed_width_font);
    set_font(DataDDDFont,          initial_app_data.data_font);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Font configuration
#endif
#endif // IF_MOTIF

    app_data.default_font_size = 
	initial_app_data.default_font_size;
    app_data.variable_width_font_size = 
	initial_app_data.variable_width_font_size;
    app_data.fixed_width_font_size = 
	initial_app_data.fixed_width_font_size;
    app_data.data_font_size = 
	initial_app_data.data_font_size;

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

    if (string(app_data.data_font) != 
	string(initial_app_data.data_font))
	return true;

    if (app_data.default_font_size != initial_app_data.default_font_size)
	return true;

    if (app_data.variable_width_font_size != 
	initial_app_data.variable_width_font_size)
	return true;

    if (app_data.fixed_width_font_size != 
	initial_app_data.fixed_width_font_size)
	return true;

    if (app_data.data_font_size != 
	initial_app_data.data_font_size)
	return true;

    return false;
}

static void ResetHelpersPreferencesCB(CB_ALIST_NULL)
{
    set_string(edit_command_w,       initial_app_data.edit_command);
    set_string(get_core_command_w,   initial_app_data.get_core_command);
    set_string(ps_command_w,         initial_app_data.ps_command);
    set_string(term_command_w,       initial_app_data.term_command);
    set_string(uncompress_command_w, initial_app_data.uncompress_command);
    set_string(www_command_w,        initial_app_data.www_command);
    set_string(plot_command_w,       initial_app_data.plot_command);

    notify_set_toggle(builtin_plot_window_w, 
		      string(initial_app_data.plot_term_type) == "xlib");
}

static bool helpers_preferences_changed()
{
    if (string(app_data.edit_command) != string(initial_app_data.edit_command))
	return true;

    if (string(app_data.plot_command) != string(initial_app_data.plot_command))
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

    if (string(app_data.plot_term_type) != 
	string(initial_app_data.plot_term_type))
	return true;

    return false;
}


static void ResetPreferencesCB(CB_ARG_LIST_2(client_data))
{
    Widget panel = (Widget)client_data;
    WidgetNameType panel_name = XtName(panel);

    if (panel_name == "general")
	ResetGeneralPreferencesCB(CB_ARGS_NULL);
    else if (panel_name == "source")
	ResetSourcePreferencesCB(CB_ARGS_NULL);
    else if (panel_name == "data")
	ResetDataPreferencesCB(CB_ARGS_NULL);
    else if (panel_name == "startup")
	ResetStartupPreferencesCB(CB_ARGS_NULL);
    else if (panel_name == "fonts")
	ResetFontPreferencesCB(CB_ARGS_NULL);
    else if (panel_name == "helpers")
	ResetHelpersPreferencesCB(CB_ARGS_NULL);
}

void update_reset_preferences()
{
    if (current_panel != 0 && reset_preferences_w != 0 && option_state_saved)
    {
	WidgetNameType panel_name = XtName(current_panel);

	bool sensitive = false;

	if (panel_name == "general")
	    sensitive = general_preferences_changed();
	else if (panel_name == "source")
	    sensitive = source_preferences_changed();
	else if (panel_name == "data")
	    sensitive = data_preferences_changed();
	else if (panel_name == "startup")
	    sensitive = startup_preferences_changed();
	else if (panel_name == "fonts")
	    sensitive = font_preferences_changed();
	else if (panel_name == "helpers")
	    sensitive = helpers_preferences_changed();

	set_sensitive(reset_preferences_w, sensitive);
    }

    if (gdb_initialized)
	check_options_file();
}

#ifdef IF_MOTIF
#ifdef NAG_ME
#warning ChangePanelCB: This stuff can be replaced by a Gtk::Notebook.
#endif
#endif // IF_MOTIF
static void ChangePanelCB(TOGGLEBUTTON_P w, XtPointer client_data, XtPointer call_data)
{
    Widget panel = (Widget)client_data;
#ifdef IF_MOTIF
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;
    bool set = cbs->set;
#else // NOT IF_MOTIF
    bool set = w->get_active();
#endif // IF_MOTIF

    if (set)
    {
	// Manage this child
	XtManageChild(panel);
#ifdef IF_MOTIF
	XtAddCallback(preferences_dialog, XmNhelpCallback,
		      HelpOnThisCB, XtPointer(panel));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning HelpCB?
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
	XtAddCallback(reset_preferences_w, XmNactivateCallback,
		      ResetPreferencesCB, XtPointer(panel));
#else // NOT IF_MOTIF
	reset_preferences_connection =
	    reset_preferences_w->signal_activate().connect(sigc::bind(PTR_FUN(ResetPreferencesCB),
								      panel));
#endif // IF_MOTIF
	current_panel = panel;

	update_reset_preferences();

#ifdef IF_MOTIF
	// Unmanage all other children
	WidgetList children;
	Cardinal num_children;
	XtVaGetValues(XtParent(panel), 
		      XmNchildren, &children,
		      XmNnumChildren, &num_children,
		      XtNIL);
#else // NOT IF_MOTIF
	WidgetList children = panel->get_parent()->get_children();
#endif // IF_MOTIF

#ifdef IF_MOTIF
	for (Cardinal i = 0; i < num_children; i++)
	{
	    Widget child = children[i];
#if 0 // FIX INDENTATION
	}
#endif
#else // NOT IF_MOTIF
	for (WidgetListIterator i = children.begin(); i != children.end(); i++)
	{
	    Widget child = *i;
#endif // IF_MOTIF
	    if (child != panel)
	    {
		XtUnmanageChild(child);
#ifdef IF_MOTIF
		XtRemoveCallback(preferences_dialog, XmNhelpCallback,
				 HelpOnThisCB, XtPointer(child));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning HelpCB?
#endif
#endif // IF_MOTIF
#ifdef IF_MOTIF
		XtRemoveCallback(reset_preferences_w, XmNactivateCallback,
				 ResetPreferencesCB, XtPointer(child));
#else // NOT IF_MOTIF
		reset_preferences_connection.disconnect();
#endif // IF_MOTIF
	    }
	}
    }
}

#ifdef IF_MOTIF
#define OLD_PANELS
#endif

#ifdef OLD_PANELS
static TOGGLEBUTTON_P add_panel(BOX_P parent,
				BOX_P buttons, 
				const _XtString name, MMDesc items[],
				Dimension& max_width, Dimension& max_height,
				bool set = false)
#else // NOT OLD_PANELS
static int add_panel(NOTEBOOK_P parent,
		     const _XtString name, Glib::ustring label,
		     MMDesc items[],
		     bool set = false)
#endif // OLD_PANELS
{
#ifdef IF_MOTIF
    Arg args[10];
    int arg;

    // Add two rows
    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    Widget form = verify(XmCreateRowColumn(parent, XMST(name), args, arg));
#else // NOT IF_MOTIF
    Gtk::Box *form = new Gtk::HBox();
    // Used to identify panels by ResetPreferencesCB,
    // update_reset_preferences.
    form->set_name(XMST(name));
#ifdef OLD_PANELS
    parent->pack_start(*form, Gtk::PACK_SHRINK);
#else
    int pageno = parent->get_n_pages();
    parent->append_page(*form, label);
#endif
#endif // IF_MOTIF
    XtManageChild(form);

    // Add panel
    Widget panel = MMcreatePanel(form, "panel", items);
    MMadjustPanel(items);
    MMaddCallbacks(items);
    MMaddHelpCallback(items, PTR_FUN(ImmediateHelpCB));
    XtManageChild(panel);
    register_menu_shell(items);

    // Fetch panel geometry
#ifdef IF_MOTIF
    XtWidgetGeometry size;
    size.request_mode = CWHeight | CWWidth;
    XtQueryGeometry(form, (XtWidgetGeometry *)0, &size);
    Dimension size_width = size.width;
    Dimension size_height = size.height;

    size_width  += 10;		// Compensate for small rounding errors
    size_height += 10;

    max_width  = max(max_width,  size_width);
    max_height = max(max_height, size_height);

    // Add button

    arg = 0;
    Widget button = verify(XmCreateToggleButton(buttons, XMST(name), args, arg));
    XtManageChild(button);
    XtAddCallback(button, XmNvalueChangedCallback, ChangePanelCB, 
		  XtPointer(form));
#else // NOT IF_MOTIF
#ifdef OLD_PANELS
    Gtk::RadioButton *button = NULL;
    Glib::ListHandle<Gtk::Widget*> kids = buttons->get_children();
    for (Glib::ListHandle<Gtk::Widget*>::iterator iter = kids.begin();
	 iter != kids.end();
	 iter++) {
	Gtk::RadioButton *old = dynamic_cast<Gtk::RadioButton *>(*iter);
	if (old) {
	    Gtk::RadioButtonGroup group = old->get_group();
	    button = new Gtk::RadioButton(group, XMST(name));
	    break;
	}
    }
    if (!button) {
	button = new Gtk::RadioButton(XMST(name));
    }
    buttons->pack_start(*button, Gtk::PACK_EXPAND_PADDING);
    button->show();
    button->signal_toggled().connect(sigc::bind(PTR_FUN(ChangePanelCB),
						button, form, XtNIL));
#endif
#endif // IF_MOTIF

#ifdef OLD_PANELS
    XmToggleButtonSetState(button, Boolean(set), False);

    if (set)
    {
#ifdef IF_MOTIF
	XmToggleButtonCallbackStruct cbs;
	cbs.set = set;
	ChangePanelCB(button, XtPointer(form), &cbs);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Do we need to call ChangePanelCB after set_active()?
#endif
	ChangePanelCB(button, form, NULL);
#endif // IF_MOTIF
    }
#endif

#ifdef OLD_PANELS
    return button;
#else
    return pageno;
#endif
}

static void OfferRestartCB(Widget dialog
#ifdef IF_MOTIF
			   , XtPointer, XtPointer
#endif // IF_MOTIF
			   )
{
    if (startup_preferences_changed() || font_preferences_changed())
    {
	static DIALOG_P restart_dialog = 0;
	if (restart_dialog == 0)
	{
#ifdef IF_MOTIF
	    restart_dialog = 
		verify(XmCreateQuestionDialog(find_shell(dialog), 
					      XMST("restart_dialog"),
					      0, 0));
#else // NOT IF_MOTIF
	    restart_dialog = new Gtk::Dialog(XMST("restart_dialog"), *find_shell(dialog));
#endif // IF_MOTIF
	    Delay::register_shell(restart_dialog);
#ifdef IF_MOTIF
	    XtAddCallback(restart_dialog, XmNokCallback,
			  DDDRestartCB, 0);
	    XtAddCallback(restart_dialog, XmNhelpCallback,
			  ImmediateHelpCB, 0);
#else // NOT IF_MOTIF
	    Gtk::Button *button;
	    button = restart_dialog->add_button(XMST("OK"), 0);
	    button->signal_clicked().connect(sigc::bind(PTR_FUN(DDDRestartCB), restart_dialog));
#endif // IF_MOTIF
	}
	manage_and_raise(restart_dialog);
    }
}

// Create preferences dialog
static void make_preferences(Widget parent)
{
#ifdef IF_MOTIF
    Arg args[10];
    int arg;

    arg = 0;
    preferences_dialog = 
	verify(XmCreatePromptDialog(parent, 
				    XMST("preferences"), args, arg));
#else // NOT IF_MOTIF
    preferences_dialog = 
	new Gtk::Dialog(XMST("preferences"), *find_shell(parent));
#endif // IF_MOTIF
    Delay::register_shell(preferences_dialog);
#ifdef IF_MOTIF
    XtVaSetValues(preferences_dialog, XmNdefaultButton, Widget(0), 
		  XtNIL);
    XtAddCallback(preferences_dialog, XmNunmapCallback, OfferRestartCB,
		  XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No default button
#endif
    preferences_dialog->signal_unmap().connect(sigc::bind(PTR_FUN(OfferRestartCB),
							  preferences_dialog));
#endif // IF_MOTIF

#ifdef IF_MOTIF
    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(preferences_dialog,
					       XmDIALOG_APPLY_BUTTON));
#endif // IF_MOTIF

#ifdef IF_MOTIF
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
#else // NOT IF_MOTIF
    reset_preferences_w = preferences_dialog->add_button(XMST("Reset"), 0);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    Widget box =
	verify(XmCreateRowColumn(preferences_dialog, XMST("box"), 
				 args, arg));
    XtManageChild(box);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    arg = 0;
    Widget buttons =
	verify(XmCreateRadioBox(box, XMST("buttons"), args, arg));
    XtManageChild(buttons);
    arg = 0;
    Widget frame = verify(XmCreateFrame(box, XMST("frame"), args, arg));
    XtManageChild(frame);
    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    Widget change =
	verify(XmCreateRowColumn(frame, XMST("change"), args, arg));
    XtManageChild(change);
#else // NOT IF_MOTIF
#ifdef OLD_PANELS
    Gtk::Box *buttons = new Gtk::HBox();
    box->pack_start(*buttons, Gtk::PACK_SHRINK);
    buttons->show();
    Gtk::Frame *frame = new Gtk::Frame(XMST("frame"));
    box->pack_start(*frame, Gtk::PACK_EXPAND_WIDGET);
    frame->show();
    Gtk::Box *change = new Gtk::HBox();
    frame->add(*change);
    change->show();
#else // NOT OLD_PANELS
    Gtk::Notebook *change = new Gtk::Notebook();
    preferences_dialog->get_vbox()->pack_start(*change, Gtk::PACK_SHRINK);
    change->show();
#endif // OLD_PANELS
#endif // IF_MOTIF

    Dimension max_width  = 0;
    Dimension max_height = 0;

#ifdef OLD_PANELS
    TOGGLEBUTTON_P general_button =
	add_panel(change, buttons, "general", general_preferences_menu, 
	      max_width, max_height, false);
    add_panel(change, buttons, "source",  source_preferences_menu, 
	      max_width, max_height, false);
    add_panel(change, buttons, "data",    data_preferences_menu, 
	      max_width, max_height, false);
    add_panel(change, buttons, "startup", startup_preferences_menu, 
	      max_width, max_height, false);
#ifdef IF_MOTIF
    add_panel(change, buttons, "fonts", font_preferences_menu,
	      max_width, max_height, false);
#endif // IF_MOTIF
    add_panel(change, buttons, "helpers", helpers_preferences_menu, 
	      max_width, max_height, false);
#else // NOT OLD_PANELS
    int general_button =
	add_panel(change, "general", "General", general_preferences_menu, 
		  false);
    add_panel(change, "source", "Source", source_preferences_menu, 
	      false);
    add_panel(change, "data", "Data", data_preferences_menu, 
	      false);
    add_panel(change, "startup", "Startup", startup_preferences_menu, 
	      false);
#ifdef IF_MOTIF
    add_panel(change, "fonts", "Fonts", font_preferences_menu,
	      false);
#endif // IF_MOTIF
    add_panel(change, "helpers", "Helpers", helpers_preferences_menu, 
	      false);
#endif

#ifdef IF_MOTIF
    XtVaSetValues(change,
		  XmNwidth, max_width,
		  XmNheight, max_height,
		  XmNresizeWidth, False,
		  XmNresizeHeight, False,
		  XtNIL);
#else // NOT IF_MOTIF
#ifdef OLD_PANELS
#ifdef NAG_ME
#warning How to calculate sizes of panels before realizing them?
#endif
    change->set_size_request(max_width, max_height);
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XmToggleButtonSetState(general_button, True, True);
#else // NOT IF_MOTIF
    change->set_current_page(general_button);
#endif // IF_MOTIF
}

// Popup Preference Panel
static void dddPopupPreferencesCB (CB_ALIST_NULL)
{
    manage_and_raise(preferences_dialog);
    check_options_file();
}


//-----------------------------------------------------------------------------
// Create status line
//-----------------------------------------------------------------------------

static void create_status(CONTAINER_P parent)
{
#ifdef IF_MOTIF
    Arg args[15];
    int arg = 0;
    XtSetArg(args[arg], XmNresizePolicy, XmRESIZE_ANY); arg++;
    Widget status_form = 
	verify(XmCreateForm(parent, XMST("status_form"), args, arg));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Note: We can use a Box instead of a Form because all we need
#warning is pack_start and pack_end.
#endif
    BOX_P status_form = new Gtk::HBox();
    status_form->set_name(XMST("status_form"));
    Gtk::Box *box = dynamic_cast<Gtk::Box *>(parent);
    if (box) {
	box->pack_end(*status_form, Gtk::PACK_SHRINK);
    }
    else {
	parent->add(*status_form);
    }
#endif // IF_MOTIF
    XtManageChild(status_form);

    // Create LED
#ifdef IF_MOTIF
    arg = 0;
    XtSetArg(args[arg], XmNtopAttachment,      XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment,   XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNrightAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNresizable,          False); arg++;
    XtSetArg(args[arg], XmNfillOnSelect,       True); arg++;
    XtSetArg(args[arg], XmNset,                True); arg++;

    MString spaces("   ");
    if (lesstif_version <= 87)
    {
	XtSetArg(args[arg], XmNlabelString, spaces.xmstring()); arg++;
    }

    led_w = verify(XmCreateToggleButton(status_form, 
					XMST("led"), args, arg));
#else // NOT IF_MOTIF
    led_w = new Gtk::ToggleButton(XMST(""));
    led_w->set_name(XMST("led"));
    status_form->pack_start(*led_w);
#ifdef NAG_ME
#warning How to fill toggle button with Green color?
#endif
#endif // IF_MOTIF
    XtManageChild(led_w);

#ifdef IF_MOTIF
    XtAddCallback(led_w, XmNvalueChangedCallback, ToggleBlinkCB, XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning We do not use the call_data here
#endif
    led_w->signal_toggled().connect(sigc::bind(PTR_FUN(ToggleBlinkCB), led_w, XtNIL));
#endif // IF_MOTIF

#ifdef IF_MOTIF
    Pixel arrow_foreground;
    XtVaGetValues(status_form, XmNbackground, &arrow_foreground, XtNIL);

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
	verify(XmCreateArrowButton(status_form, XMST("arrow"), args, arg));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Set arrow as pixmap in button.
#endif
    Widget arrow_w = new Gtk::Button();
    arrow_w->set_name(XMST("arrow"));
    status_form->pack_start(*arrow_w);
#endif

    XtManageChild(arrow_w);

    // Give some `dummy' status message.  Some Motif versions limit
    // the size of the status window to the length of the very first
    // message, so we give some huge string at the beginning.
    MString short_msg = rm("Hello, world!");
    MString long_msg = short_msg + rm(replicate(' ', 90));

#ifdef IF_MOTIF
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
    status_w = verify(XmCreatePushButton(status_form, 
					 XMST("status"), args, arg));
#else // NOT IF_MOTIF
    status_w = new Gtk::Statusbar();
    status_w->set_name(XMST("status"));
    status_form->pack_start(*status_w);
#endif // IF_MOTIF
    XtManageChild(status_w);

    // Initialize status history
    status_history_size = app_data.status_history_size;
    (void) status_history(parent);
    set_status("");
    set_status_mstring(short_msg);

#ifdef IF_MOTIF
    XtAddCallback(status_w, XmNarmCallback, 
		  PopupStatusHistoryCB, XtNIL);
    XtAddCallback(status_w, XmNactivateCallback, 
		  PopdownStatusHistoryCB, XtNIL);
    XtAddCallback(status_w, XmNdisarmCallback, 
		  PopdownStatusHistoryCB, XtNIL);
#else // NOT IF_MOTIF
    status_w->signal_button_press_event().connect(sigc::bind(PTR_FUN(PopupStatusHistoryCB), status_w));
    status_w->signal_button_release_event().connect(PTR_FUN(PopdownStatusHistoryCB));
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtAddCallback(arrow_w, XmNarmCallback, 
		  PopupStatusHistoryCB, XtNIL);
    XtAddCallback(arrow_w, XmNactivateCallback, 
		  PopdownStatusHistoryCB, XtNIL);
    XtAddCallback(arrow_w, XmNdisarmCallback, 
		  PopdownStatusHistoryCB, XtNIL);
#else // NOT IF_MOTIF
    arrow_w->signal_button_press_event().connect(sigc::bind(PTR_FUN(PopupStatusHistoryCB), arrow_w));
    arrow_w->signal_button_release_event().connect(PTR_FUN(PopdownStatusHistoryCB));
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Using LessTif 0.88, you can release button 1 while outside the
    // status bar; no disarm callback is invoked.  Prevent against this.
    XtAddEventHandler(status_w, ButtonReleaseMask, False,
		      PopdownStatusHistoryEH, XtNIL);
    XtAddEventHandler(arrow_w, ButtonReleaseMask, False, 
		      PopdownStatusHistoryEH, XtNIL);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(status_w, (XtWidgetGeometry *)0, &size);

    Dimension size_height = size.height;
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Is height set properly?
#endif
    Dimension size_height = status_w->get_height();
#endif // IF_MOTIF

#ifdef IF_MOTIF
    if (lesstif_version <= 87)
	XtVaSetValues(led_w, XmNindicatorSize, size_height - 4, XtNIL);
    else
	XtVaSetValues(led_w, XmNindicatorSize, size_height - 1, XtNIL);
#else // NOT IF_MOTIF
    led_w->set_size_request(-1, size_height-1);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtVaSetValues(arrow_w,
		  XmNheight, size_height - 2,
		  XmNwidth,  size_height - 2,
		  XtNIL);
#else // NOT IF_MOTIF
    arrow_w->set_size_request(size_height-2, size_height-2);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtVaSetValues(status_form,
		  XmNpaneMaximum, size_height,
		  XmNpaneMinimum, size_height,
		  XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XmNpaneMaximum analogue?
#endif
#endif // IF_MOTIF

    set_toggle(led_w, app_data.blink_while_busy);
    blink(true);
}


//-----------------------------------------------------------------------------
// Handle Status LED
//-----------------------------------------------------------------------------

static bool blinker_active        = false; // True iff status LED is active
#ifdef IF_MOTIF
static XtIntervalId blink_timer = 0;     // Timer for blinking
#else // NOT IF_MOTIF
static sigc::connection blink_timer;         // Timer for blinking
#endif // IF_MOTIF

static
#ifdef IF_MOTIF
void
#else // NOT IF_MOTIF
bool
#endif // IF_MOTIF
BlinkCB(
#ifdef IF_MOTIF
    XtPointer client_data, XtIntervalId *id
#else // NOT IF_MOTIF
    bool *set_p
#endif // IF_MOTIF
    )
{
#ifdef IF_MOTIF
    (void) id;			// use it
    assert(*id == blink_timer);
    blink_timer = 0;
#endif // IF_MOTIF

    static bool have_led_colors = false;
#ifdef IF_MOTIF
    static Pixel led_select_color;
    static Pixel led_background_color;
#else // NOT IF_MOTIF
    static Gdk::Color led_select_color;
    static Gdk::Color led_background_color;
#endif // IF_MOTIF


    if (!have_led_colors)
    {
#ifdef IF_MOTIF
	XtVaGetValues(led_w,
		      XmNbackground, &led_background_color,
		      XmNselectColor, &led_select_color,
		      XtNIL);
#else // NOT IF_MOTIF
	led_background_color = led_w->get_style()->get_bg(Gtk::STATE_NORMAL);
	led_select_color = led_w->get_style()->get_bg(Gtk::STATE_SELECTED);
#endif // IF_MOTIF
	have_led_colors = true;
    }

#ifdef IF_MOTIF
    bool set = bool(long(client_data));
    if (set)
	XtVaSetValues(led_w, XmNselectColor, led_select_color, 
		      XtNIL);
    else
	XtVaSetValues(led_w, XmNselectColor, led_background_color, 
		      XtNIL);
#else // NOT IF_MOTIF
    bool set = *set_p;
    *set_p = !set;
    if (set)
	led_w->get_style()->set_bg(Gtk::STATE_SELECTED, led_select_color);
    else
	led_w->get_style()->set_bg(Gtk::STATE_SELECTED, led_background_color);
#endif // IF_MOTIF

    XFlush(XtDisplay(led_w));
    XmUpdateDisplay(led_w);

    if ((blinker_active || set) && app_data.busy_blink_rate > 0)
    {
#ifdef IF_MOTIF
	blink_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(led_w),
				      app_data.busy_blink_rate, PTR_FUN(BlinkCB),
				      XtPointer(int(!set)));
#else // NOT IF_MOTIF
	return true;
#endif // IF_MOTIF
    }
#ifndef IF_MOTIF
    return false;
#endif // IF_MOTIF
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
#ifdef IF_MOTIF
	    BlinkCB(XtPointer(int(true)), &blink_timer);
#else // NOT IF_MOTIF
	    static bool blink_state = false;
	    blink_timer = Glib::signal_timeout().connect(sigc::bind(PTR_FUN(BlinkCB), &blink_state),
							 app_data.busy_blink_rate);
#endif // IF_MOTIF
	}
    }
}

static void DisableBlinkHP(Agent *, void *, void *)
{
    // GDB has died -- disable status LED
    XmToggleButtonSetState(led_w, False, False);
}

static void ToggleBlinkCB(TOGGLEBUTTON_P w,
#ifdef IF_MOTIF
			  XtPointer, 
#endif // IF_MOTIF
			  XtPointer call_data
			  )
{
    string debugger_status_indicator =
	"Debugger status indicator ";
#ifdef IF_MOTIF
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.blink_while_busy = info->set;

    if (info->set)
	set_status(debugger_status_indicator + "enabled.");
    else
	set_status(debugger_status_indicator + "disabled.");
#else // NOT IF_MOTIF
    bool set = w->get_active();

    app_data.blink_while_busy = set;

    if (set)
	set_status(debugger_status_indicator + "enabled.");
    else
	set_status(debugger_status_indicator + "disabled.");
#endif // IF_MOTIF

    // Restart blinker
    blink(blinker_active);
}

//-----------------------------------------------------------------------------
// Handle Status History
//-----------------------------------------------------------------------------

const Dimension y_popup_offset = 5;

static MENU_P history_shell = 0;
static BUTTON_CB_RETURN_TYPE
PopupStatusHistoryCB(
#ifdef IF_MOTIF
		     Widget w, XtPointer client_data, XtPointer call_data
#else // NOT IF_MOTIF
		     GdkEventButton *, Widget w
#endif // IF_MOTIF
		     )
{
    history_shell = status_history(w);

#ifdef IF_MOTIF
    Position shell_x, shell_y;
    XtTranslateCoords(find_shell(status_w), 0, 0, &shell_x, &shell_y);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Coordinates?
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    Position status_x, status_y;
    XtTranslateCoords(status_w, 0, 0, &status_x, &status_y);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Coordinates?
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtWidgetGeometry size;
    size.request_mode = CWHeight;
#endif // IF_MOTIF
    Dimension size_height;
    Position x, y;
    if (app_data.status_at_bottom)
    {
#ifdef IF_MOTIF
	XtQueryGeometry(history_shell, (XtWidgetGeometry *)0, &size);

	x = shell_x;
	y = status_y - size.height - y_popup_offset;
#else // NOT IF_MOTIF
	int size_height = history_shell->get_height();
#endif // IF_MOTIF
    }
    else
    {
#ifdef IF_MOTIF
	XtQueryGeometry(status_w, (XtWidgetGeometry *)0, &size);

	x = shell_x;
	y = status_y + size.height + y_popup_offset;
#else // NOT IF_MOTIF
	int size_height = status_w->get_height();
#endif // IF_MOTIF
    }

#ifdef IF_MOTIF
    XtVaSetValues(history_shell, XmNx, x, XmNy, y, XtNIL);
    XtPopup(history_shell, XtGrabNone);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Can we choose the position of the popup menu (SlotPositionCalc?)
#endif
    history_shell->popup(0, gtk_get_current_event_time());
#endif // IF_MOTIF
}

static BUTTON_CB_RETURN_TYPE
PopdownStatusHistoryCB(
#ifdef IF_MOTIF
		       Widget, XtPointer, XtPointer
#else // NOT IF_MOTIF
		       GdkEventButton *
#endif // IF_MOTIF
		       )
{
    if (history_shell != 0)
	XtPopdown(history_shell);
}

#ifdef IF_MOTIF
static void PopdownStatusHistoryEH(Widget w, XtPointer client_data, 
				   XEvent *event, Boolean *)
{
    PopdownStatusHistoryCB(w, client_data, (XtPointer)event);
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning PopdownStatusHistoryEH unimplemented?
#endif
#endif // IF_MOTIF


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

void update_arg_buttons()
{
    string arg = source_arg->get_string();

    bool can_find = (!arg.empty()) && !is_file_pos(arg) && 
	source_view->have_source();
    set_sensitive(arg_cmd_area[ArgItems::Find].widget, can_find);
    set_sensitive(find_forward_w, can_find);
    set_sensitive(find_backward_w, can_find);

    bool undoing = undo_buffer.showing_earlier_state();
    bool can_print = (!arg.empty()) && !is_file_pos(arg) && !undoing;
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

    string deref_arg = deref(arg, "()");

    MString print_ref_label("Print " + deref_arg);
#ifdef IF_MOTIF
    XtVaSetValues(print_menu[PrintItems::PrintRef].widget,
		  XmNlabelString, print_ref_label.xmstring(),
		  XtNIL);
#else // NOT IF_MOTIF
    set_label(print_menu[PrintItems::PrintRef].widget, print_ref_label);
#endif // IF_MOTIF

    MString disp_ref_label("Display " + deref_arg);
#ifdef IF_MOTIF
    XtVaSetValues(display_menu[DispItems::DispRef].widget,
		  XmNlabelString, disp_ref_label.xmstring(),
		  XtNIL);
#else // NOT IF_MOTIF
    set_label(display_menu[DispItems::DispRef].widget, disp_ref_label);
#endif // IF_MOTIF

    bool can_dereference = !gdb->dereferenced_expr("").empty();
    manage_child(print_ref_w, can_dereference);
    manage_child(disp_ref_w,  can_dereference);

    set_sensitive(stack_w,     !undoing);
    set_sensitive(registers_w, gdb->has_regs_command() && !undoing);
    set_sensitive(threads_w,   (gdb->type() == GDB
				|| gdb->type() == JDB
				|| (gdb->type() == DBX && gdb->isSunDBX())
				) &&
		                !undoing);
    set_sensitive(infos_w,     (gdb->type() == GDB || gdb->type() == PYDB) &&
		                !undoing);
}

// Arg changed - re-label buttons
static void source_argHP(void *, void *, void *)
{
    update_arg_buttons();
    data_disp->refresh_args();
    data_disp->language_changedHP(gdb, 0, 0);
}

// Language changed - re-label buttons
static void language_changedHP(Agent *, void *, void *)
{
    update_arg_buttons();
}


// Language changed - report it
static std::ostream& operator<< (std::ostream& os, ProgramLanguage lang)
{
    switch (lang)
    {
    case LANGUAGE_ADA:     os << "Ada";           break;
    case LANGUAGE_BASH:    os << "Bash";          break;
    case LANGUAGE_C:       os << "C/C++";         break;
    case LANGUAGE_CHILL:   os << "Chill";         break;
    case LANGUAGE_FORTRAN: os << "Fortran";       break;
    case LANGUAGE_JAVA:    os << "Java";          break;
    case LANGUAGE_PASCAL:  os << "Pascal/Modula"; break;
    case LANGUAGE_PERL:    os << "Perl";          break;
    case LANGUAGE_PHP:     os << "PHP";           break;
    case LANGUAGE_PYTHON:  os << "Python";        break;
    case LANGUAGE_OTHER:   os << "(unknown)";     break;
    }

    return os;
}

static void report_languageHP(Agent *, void *, void *)
{
    std::ostringstream os;
    os << "Current language: " << gdb->program_language();
    set_status(string(os));
}


void update_user_buttons()
{
#ifdef IF_MOTIF
    set_buttons(data_buttons_w,    app_data.data_buttons);
    set_buttons(source_buttons_w,  app_data.source_buttons);
    set_buttons(console_buttons_w, app_data.console_buttons);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning set_buttons not implemented
#endif
#endif // IF_MOTIF
    set_shortcut_menu(data_disp);
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
	if (!emptyCommandQueue()) {
#ifdef IF_MOTIF
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
			    PTR_FUN(processCommandQueue), XtNIL);
#else // NOT IF_MOTIF
	    Glib::signal_idle().connect(PTR_FUN(processCommandQueue));
#endif // IF_MOTIF
	}

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
    GTK_SLOT_W proc;
#ifdef IF_MOTIF
    XtPointer client_data;
    XmPushButtonCallbackStruct cbs;
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning WhenReady: callback structure not used?
#endif
#endif // IF_MOTIF
    XEvent event;

#ifdef IF_MOTIF
    WhenReadyInfo(const MString &msg, XtCallbackProc p, XtPointer cl_data,
		  const XmPushButtonCallbackStruct& c)
	: message(msg),
	  proc(p),
	  client_data(cl_data),
	  cbs(c)
	  // Initializing `event' causes an error in GCC 2.8.1
	  // Reported by "Derks, Victor" <Victor.Derks@nl2.vdogrp.de>
	  // , event()
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
#else // NOT IF_MOTIF
    WhenReadyInfo(const MString &msg, GTK_SLOT_W p)
	: message(msg),
	  proc(p)
    {
    }
#endif // IF_MOTIF
private:
    WhenReadyInfo(const WhenReadyInfo&);
    WhenReadyInfo& operator= (const WhenReadyInfo&);
};

static void DoneCB(const string& /* answer */, void *qu_data)
{
    WhenReadyInfo *info = (WhenReadyInfo *)qu_data;
    set_status_mstring(info->message + rm("done."));

#ifdef IF_MOTIF
    (*info->proc)(gdb_w, info->client_data, XtPointer(&info->cbs));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning We cannot pass the callback structure: maybe do not need it?
#endif
    (info->proc)(gdb_w);
#endif // IF_MOTIF
    delete info;
}

// Execute command in (XtCallbackProc)CLIENT_DATA as soon as GDB gets ready
static void WhenReady(Widget w, XtPointer client_data
#ifdef IF_MOTIF
		      , XtPointer call_data
#endif // IF_MOTIF
		      )
{
#ifdef IF_MOTIF
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    if (cbs == 0)
	return;	    // This happens with old LessTif versions
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XtCallbackProc proc = STATIC_CAST(WhenReadyProc_t*,client_data)->proc;
#else // NOT IF_MOTIF
    GTK_SLOT_W proc = *STATIC_CAST(GTK_SLOT_W*,client_data);
#endif // IF_MOTIF
    XtPointer user_client_data = 0; // No way to pass extra values here

    if (can_do_gdb_command())
    {
	// GDB is ready: do command now
#ifdef IF_MOTIF
	proc(w, user_client_data, call_data);
#else // NOT IF_MOTIF
	proc(w);
#endif // IF_MOTIF
	return;
    }

    // Execute command as soon as GDB gets ready
#ifdef IF_MOTIF
    XmString label = 0;
    XtVaGetValues(w, XmNlabelString, &label, XtNIL);
    MString _action(label, true);
    XmStringFree(label);
#else // NOT IF_MOTIF
    MString _action = get_label(w);
#endif // IF_MOTIF
    string action = _action.str();
    if (action.contains("...", -1))
	action = action.before("...");

    MString msg = rm(action + ": waiting until " + gdb->title() 
		     + " gets ready...");
#ifdef IF_MOTIF
    WhenReadyInfo *info = new WhenReadyInfo(msg, proc, user_client_data, *cbs);
#else // NOT IF_MOTIF
    WhenReadyInfo *info = new WhenReadyInfo(msg, proc);
#endif // IF_MOTIF

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

static void gdb_msgHP(Agent *source, void *call_data, const char *name)
{
    if (source->pid() == 0)
    {
	// We're the child.  Don't use the parent's X display.
        return;
    }

    string msg = STATIC_CAST(char *,call_data);
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

static void gdb_echo_detectedHP(Agent *, void *, void *call_data)
{
    static bool last_echoing = false;

    bool echoing = bool(long(call_data));
    if (echoing != last_echoing)
    {
	if (echoing)
	{
	    // Here is a subtle warning.
	    set_status(gdb->title() + " is running in echo mode.");

	    // Attempt to disable echo mode explicitly via stty command.
	    gdb_command(gdb->shell_command("stty -echo -onlcr"), 0, 0, 0, 
			false, false, COMMAND_PRIORITY_AGAIN);
	}
	else
	{
	    // Tell the user everything is fine.
	    set_status(gdb->title() + " is no longer running in echo mode.");
	}

	last_echoing = echoing;
    }
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

    // Recording state affects Undo/Redo buttons
    refresh_buttons();
    gdbUpdateAllMenus();
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
#ifdef IF_MOTIF
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
	    const int TAB_WIDTH = DEFAULT_TAB_WIDTH;
	    const int column = promptPosition - startOfLine;
	    const int spaces = TAB_WIDTH - column % TAB_WIDTH;
	    const string spacing = replicate(' ', spaces);
	
	    XmTextInsert(gdb_w, promptPosition, XMST(spacing.chars()));
	    promptPosition += spacing.length();
	}
	break;
		
	case '\r':
	{
	    // Erase last line
	    XmTextReplace(gdb_w, startOfLine, promptPosition, XMST(""));
	    promptPosition = startOfLine;
	}
	break;
	}
	break;
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Cannot handle tab or CR characters from gdb
#endif
#endif // IF_MOTIF
    case '\b':
    {
	// Erase last character
#ifdef IF_MOTIF
	XmTextReplace(gdb_w, promptPosition - 1, promptPosition, XMST(""));
#else // NOT IF_MOTIF
	gdb_w->replace(promptPosition - 1, promptPosition, XMST(""));
#endif // IF_MOTIF
	promptPosition--;
    }
    break;

    case '\n':
    {
	string c = ctrl;
#ifdef IF_MOTIF
	XmTextInsert(gdb_w, promptPosition, XMST(c.chars()));
#else // NOT IF_MOTIF
	gdb_w->insert(promptPosition, XMST(c.chars()));
#endif // IF_MOTIF
	promptPosition += c.length();

	// Flush output
#ifdef IF_MOTIF
	XmTextShowPosition(gdb_w, promptPosition);
#else // NOT IF_MOTIF
	gdb_w->show_position(promptPosition);
#endif // IF_MOTIF
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
#ifdef IF_MOTIF
	XmTextInsert(gdb_w, promptPosition, XMST(c.chars()));
#else // NOT IF_MOTIF
	gdb_w->insert(promptPosition, XMST(c.chars()));
#endif // IF_MOTIF
	promptPosition += c.length();
    }
    break;
    }

    // XmTextShowPosition(gdb_w, promptPosition);
}


// Append TEXT to GDB output
void _gdb_out(const string& txt)
{
    if (txt.empty())
	return;
    if (private_gdb_output)
	return;

    string text(txt);
    string buffered = buffered_gdb_output();
    if (!buffered.empty())
	text.prepend(buffered);

    gdb_input_at_prompt = gdb->ends_with_prompt(text);
    if (gdb_input_at_prompt)
	debuggee_running = false;

    if (promptPosition == 0) {
#ifdef IF_MOTIF
	promptPosition = XmTextGetLastPosition(gdb_w);
#else // NOT IF_MOTIF
	promptPosition = gdb_w->get_last_position();
#endif // IF_MOTIF
    }

    private_gdb_output = true;

    // Don't care for strings to be ignored
    static string empty;
    if (!gdb_out_ignore.empty())
	text.gsub(gdb_out_ignore, empty);

    // Pass TEXT to various functions
    set_buttons_from_gdb(console_buttons_w, text);
    set_buttons_from_gdb(source_buttons_w, text);
    set_buttons_from_gdb(data_buttons_w, text);
    set_buttons_from_gdb(command_toolbar_w, text);
    set_status_from_gdb(text);
    set_tty_from_gdb(text);

    // Output TEXT on TTY
    tty_out(USTRING(text));

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

#ifdef IF_MOTIF
	    XmTextInsert(gdb_w, promptPosition, XMST(block.chars()));
#else // NOT IF_MOTIF
	    gdb_w->insert(promptPosition, XMST(block.chars()));
#endif // IF_MOTIF
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
    } while (!text.empty());

#ifdef IF_MOTIF
    XmTextPosition lastPos = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, lastPos);
    XmTextShowPosition(gdb_w, lastPos);
#else // NOT IF_MOTIF
    XmTextPosition lastPos = gdb_w->get_last_position();
    gdb_w->set_insertion_position(lastPos);
    gdb_w->show_position(lastPos);
#endif // IF_MOTIF

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

static DDDWindow ddd_window(XtP(DDDWindow) client_data)
{
    if (source_view_shell == 0 && data_disp_shell == 0)
	return CommonWindow;
    else
	return DDDWindow(long(client_data));
}

static void gdbCutSelectionCB(CB_ALIST_12(Widget w, XtP(DDDWindow) client_data))
{
#ifdef IF_MOTIF
    Time tm = time(XtLastEventProcessed(XtDisplay(w)));
#endif // IF_MOTIF

    DDDWindow win = ddd_window(client_data);
    Boolean success = False;
    // Try data arg
    if (!success && (win == DataWindow || win == CommonWindow))
    {
	if (data_disp->have_selection())
	{
#ifdef IF_MOTIF
	    success = XmTextCopy(DataDisp::graph_selection_w, tm);
#else // NOT IF_MOTIF
	    std::cerr << "Copy from graph_selection_w not implemented\n";
	    // success = text_copy_from(DataDisp::graph_selection_w);
#endif // IF_MOTIF
	    if (success)
		DataDisp::deleteCB(CB_ARGS_1(w));
	}
    }

#ifdef IF_MOTIF
    // Try destination window
    Widget dest = XmGetDestination(XtDisplay(w));
    if (!success && dest != 0 && XmIsText(dest))
	success = XmTextCut(dest, tm);
    if (!success && dest != 0 && XmIsTextField(dest))
	success = XmTextFieldCut(dest, tm);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XmGetDestination?
#endif
#endif // IF_MOTIF

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow)) {
#ifdef IF_MOTIF
	success = XmTextCut(gdb_w, tm);
#else // NOT IF_MOTIF
	success = text_cut_from(gdb_w);
#endif // IF_MOTIF
    }

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow)) {
#ifdef IF_MOTIF
	success = XmTextFieldCut(source_arg->text(), tm);
#else // NOT IF_MOTIF
	success = text_cut_from(source_arg->text());
#endif // IF_MOTIF
    }

    if (success)
	gdbUnselectAllCB(CB_ARGS_NULL);
}

static void gdbCopySelectionCB(CB_ALIST_12(Widget w, XtP(DDDWindow) client_data))
{
#ifdef IF_MOTIF
    Time tm = time(XtLastEventProcessed(XtDisplay(w)));
#endif // IF_MOTIF
    
    DDDWindow win = ddd_window(client_data);
    Boolean success = False;

    // Try data arg
    if (!success && (win == DataWindow || win == CommonWindow))
	if (data_disp->have_selection())
	{
#ifdef IF_MOTIF
	    success = XmTextCopy(DataDisp::graph_selection_w, tm);
#else // NOT IF_MOTIF
	    std::cerr << "Copy from graph_selection_w not implemented\n";
	    // success = text_copy_from(DataDisp::graph_selection_w);
#endif // IF_MOTIF
	}

#ifdef IF_MOTIF
    // Try destination window
    Widget dest = XmGetDestination(XtDisplay(w));
    if (!success && dest != 0 && XmIsText(dest))
	success = XmTextCopy(dest, tm);
    if (!success && dest != 0 && XmIsTextField(dest))
	success = XmTextFieldCopy(dest, tm);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XmGetDestination?
#endif
#endif // IF_MOTIF

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow)) {
#ifdef IF_MOTIF
	success = XmTextCopy(gdb_w, tm);
#else // NOT IF_MOTIF
	success = text_copy_from(gdb_w);
#endif // IF_MOTIF
    }

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow)) {
#ifdef IF_MOTIF
	success = XmTextFieldCopy(source_arg->text(), tm);
#else // NOT IF_MOTIF
	success = text_copy_from(source_arg->text());
#endif // IF_MOTIF
    }

    // Try source
    if (!success && (win == SourceWindow || win == CommonWindow)) {
#ifdef IF_MOTIF
	success = XmTextCopy(source_view->source(), tm);
#else // NOT IF_MOTIF
	success = text_copy_from(source_view->source());
#endif // IF_MOTIF
    }

    // Try code
    if (!success && (win == SourceWindow || win == CommonWindow)) {
#ifdef IF_MOTIF
	success = XmTextCopy(source_view->code(), tm);
#else // NOT IF_MOTIF
	success = text_copy_from(source_view->code());
#endif // IF_MOTIF
    }
}

static void gdbPasteClipboardCB(CB_ALIST_12(Widget w, XtP(DDDWindow) client_data))
{
#ifdef IF_MOTIF
    DDDWindow win = ddd_window(client_data);

    // Try destination window
    bool success = false;
#ifdef IF_MOTIF
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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Paste: XmGetDestination?
#endif
#endif // IF_MOTIF
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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Paste not implemented
#endif
#endif // IF_MOTIF
}

static void gdbUnselectAllCB(CB_ALIST_NULL)
{
#ifdef IF_MOTIF
#ifdef NAG_ME
#warning FIXME: No timestamp.
#endif
    Time tm = Time(0);

    XmTextClearSelection(gdb_w, tm);
    XmTextFieldClearSelection(source_arg->text(), tm);
    XmTextClearSelection(source_view->source(), tm);
    XmTextClearSelection(source_view->code(), tm);

    if (data_disp->graph_arg != 0)
	XmTextFieldClearSelection(data_disp->graph_arg->text(), tm);
    if (data_disp->graph_selection_w != 0)
	XmTextClearSelection(data_disp->graph_selection_w, tm);

    DataDisp::unselectAllCB(CB_ARGS_NULL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning gdbUnselectAllCB not implemented
#endif
#endif // IF_MOTIF
}

static void gdbClearAllCB(CB_ALIST_NULL)
{
#ifdef NAG_ME
#warning Call data is only used for timestamp.
#endif
    gdbUnselectAllCB(CB_ARGS_NULL);
    source_arg->set_string("");
    gdbClearCB(CB_ARGS_NULL);
}

static void gdbSelectAllCB(CB_ALIST_12(Widget w, XtP(DDDWindow) client_data))
{
#ifdef IF_MOTIF
    DDDWindow win = ddd_window(client_data);

    Time tm = time(XtLastEventProcessed(XtDisplay(w)));

    bool success = false;
    Widget dest = XmGetDestination(XtDisplay(w));
    if (dest != 0)
    {
	if (!success && dest == DataDisp::graph_selection_w)
	{
	    DataDisp::selectAllCB(CB_ARGS_NULL);
	    success = true;
	}
	if (!success && XmIsText(dest))
	{
	    TextSetSelection(dest, 0, XmTextGetLastPosition(dest), tm);
	    success = true;
	}
	if (!success && XmIsTextField(dest))
	{
	    TextFieldSetSelection(dest, 0, 
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
	DataDisp::selectAllCB(CB_ARGS_NULL);
	break;

    case GDBWindow:
	XmTextSetSelection(gdb_w, promptPosition, 
			   XmTextGetLastPosition(gdb_w), tm);
	break;

    default:
	break;
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning gdbSelectAllCB not implemented
#endif
#endif // IF_MOTIF
}


static void gdbDeleteSelectionCB(CB_ALIST_12(Widget w, XtP(DDDWindow) client_data))
{
#ifdef IF_MOTIF
    DDDWindow win = ddd_window(client_data);
    Boolean success = False;
    Widget dest = XmGetDestination(XtDisplay(w));

    // Try data display
    if (!success && dest == DataDisp::graph_selection_w)
    {
	if (data_disp->have_selection())
	{
	    DataDisp::deleteCB(CB_ARGS_NULL);
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
	    DataDisp::deleteCB(CB_ARGS_NULL);
	    success = true;
	}
    }

    if (success)
	gdbUnselectAllCB(CB_ARGS_NULL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning gdbDeleteSelectionCB not implemented
#endif
#endif // IF_MOTIF
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
		      XtNIL);
	XtVaSetValues(menu[EditItems::Copy].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>C",
		      XmNacceleratorText, copy.xmstring(),
		      XtNIL);
	XtVaSetValues(menu[EditItems::Paste].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>V",
		      XmNacceleratorText, paste.xmstring(),
		      XtNIL);
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
		      XtNIL);
	XtVaSetValues(menu[EditItems::Copy].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>Insert",
		      XmNacceleratorText, copy.xmstring(),
		      XtNIL);
	XtVaSetValues(menu[EditItems::Paste].widget,
		      XmNaccelerator, "~Ctrl Shift<Key>Insert",
		      XmNacceleratorText, paste.xmstring(),
		      XtNIL);
	break;
    }
    }
#else
    (void) style;
#endif
}

static void setup_cut_copy_paste_bindings(XrmDatabase db)
{
    // Stupid OSF/Motif won't change the accelerators once created.
    // Set resources explicitly.
    const _XtString resources = 0;
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

#ifdef IF_MOTIF
    XrmDatabase bindings = XrmGetStringDatabase(resources);
#else // NOT IF_MOTIF
    XrmDatabase bindings = get_string_database(resources);
#endif // IF_MOTIF
    assert(bindings != 0);
#ifdef IF_MOTIF
    XrmMergeDatabases(bindings, &db);
#else // NOT IF_MOTIF
    merge_databases(bindings, db);
#endif // IF_MOTIF
}


// Update select all bindings
static void set_select_all_bindings(MMDesc *menu, BindingStyle style)
{
    if (menu == 0 || menu[0].widget == 0)
	return;

    switch (style)
    {
    case KDEBindings:
    {
	MString select_all("Ctrl+A");

#ifdef IF_MOTIF
	XtVaSetValues(menu[EditItems::SelectAll].widget,
		      XmNacceleratorText, select_all.xmstring(),
		      XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Set accelerator?
#endif
#endif // IF_MOTIF
	break;
    }

    case MotifBindings:
    {
	MString select_all("Shift+Ctrl+A");

#ifdef IF_MOTIF
	XtVaSetValues(menu[EditItems::SelectAll].widget,
		      XmNacceleratorText, select_all.xmstring(),
		      XtNIL);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Set accelerator?
#endif
#endif // IF_MOTIF
	break;
    }
    }
}

static void setup_select_all_bindings(XrmDatabase db)
{
    // Stupid OSF/Motif won't change the accelerators once created.
    // Set resources explicitly.
    const _XtString resources = 0;
    switch (app_data.select_all_bindings)
    {
    case KDEBindings:
	resources =
	    "*editMenu.selectAll.acceleratorText: Ctrl+A\n";
	break;

    case MotifBindings:
	resources =
	    "*editMenu.selectAll.acceleratorText: Shift+Ctrl+A\n";
	break;
    }

#ifdef IF_MOTIF
    XrmDatabase bindings = XrmGetStringDatabase(resources);
    assert(bindings != 0);
    XrmMergeDatabases(bindings, &db);
#else // NOT IF_MOTIF
    XrmDatabase bindings = get_string_database(resources);
    assert(bindings != 0);
    merge_databases(bindings, db);
#endif // IF_MOTIF
}


//-----------------------------------------------------------------------------
// Update menu entries
//-----------------------------------------------------------------------------

static int _mapped_menus = 0;

static int mapped_menus()
{
#ifdef IF_MOTIF
    if (lesstif_version <= 87)
    {
	// LessTif 0.87 and earlier does not issue a XmCR_MAP callback
	// when mapping RowColumn menus.  Hence, we always assume we
	// have some mapped menu.
	return 1;
    }
#endif // IF_MOTIF

    return _mapped_menus;
}

static void count_mapped_menus(
#ifdef IF_MOTIF
    Widget, XtPointer, XtPointer call_data
#else // NOT IF_MOTIF
    GdkEventAny *ev
#endif // IF_MOTIF
)
{
#ifdef IF_MOTIF
    XmRowColumnCallbackStruct *cbs = (XmRowColumnCallbackStruct *)call_data;

    if (cbs == 0)
	return;

    switch (cbs->reason)
    {
    case XmCR_MAP:
	_mapped_menus++;
	break;

    case XmCR_UNMAP:
	_mapped_menus--;
	break;

#if XmVersion >= 1002
    case XmCR_TEAR_OFF_ACTIVATE:
	_mapped_menus++;
	break;

    case XmCR_TEAR_OFF_DEACTIVATE:
	_mapped_menus--;
	break;
#endif

    default:
	break;
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Event data in count_mapped_menus?
#endif
    if (ev == 0)
	return;

    switch (ev->type)
    {
    case GDK_MAP:
	_mapped_menus++;
	break;

    case GDK_UNMAP:
	_mapped_menus--;
	break;

#ifdef NAG_ME
#warning Tear-off menus?
#endif
    default:
	break;
    }
#endif // IF_MOTIF
    // std::clog << _mapped_menus << " mapped menus\n";
}

static void gdbUpdateEditCB(CB_ALIST_12(Widget w, XtP(DDDWindow) client_data))
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
#ifdef NAG_ME
#warning count_mapped_menus arg
#endif
    count_mapped_menus(CB_ARGS_3(0));

    // Reset undo/redo actions
    string undo_action = undo_buffer.undo_action();
    string redo_action = undo_buffer.redo_action();

    if (undo_action == NO_GDB_ANSWER)
    {
	set_label(menu[EditItems::Undo].widget, "Undo");
	set_sensitive(menu[EditItems::Undo].widget, false);
    }
    else
    {
	MString label("Undo " + undo_action);
	set_label(menu[EditItems::Undo].widget, label);
	set_sensitive(menu[EditItems::Undo].widget, true);
    }

    if (redo_action == NO_GDB_ANSWER)
    {
	set_label(menu[EditItems::Redo].widget, "Redo");
	set_sensitive(menu[EditItems::Redo].widget, false);
    }
    else
    {
	MString label("Redo " + redo_action);
	set_label(menu[EditItems::Redo].widget, label);
	set_sensitive(menu[EditItems::Redo].widget, true);
    }

#ifdef IF_MOTIF
    // Check if we have something to cut
    XmTextPosition start, end;
    bool can_cut = false;
#ifdef IF_MOTIF
    Widget dest  = XmGetDestination(XtDisplay(w));

    // Try destination window
    if (!can_cut && dest != 0 && XmIsText(dest))
	can_cut = XmTextGetSelectionPosition(dest, &start, &end);
    if (!can_cut && dest != 0 && XmIsTextField(dest))
	can_cut = XmTextFieldGetSelectionPosition(dest, &start, &end);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XmGetDestination?
#endif
#endif // IF_MOTIF

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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Fix sensitivity of cut/copy/paste menu items
#endif
#endif // IF_MOTIF
}

static void gdbUpdateFileCB(CB_ARG_LIST_2(client_data))
{
    MMDesc *file_menu = (MMDesc *)client_data;
    if (file_menu == 0 || file_menu[0].widget == 0)
	return;

    count_mapped_menus(CB_ARGS_3(NULL));

    // Check whether we can print something
#ifdef IF_MOTIF
    Graph *graph = graphEditGetGraph(data_disp->graph_edit);
#else // NOT IF_MOTIF
    Graph *graph = data_disp->graph_edit->get_graph();
#endif // IF_MOTIF
    bool can_print = (graph->firstNode() != 0);
    set_sensitive(file_menu[FileItems::Print].widget,      can_print);
    set_sensitive(file_menu[FileItems::PrintAgain].widget, can_print);

    // Check whether we can close something
    bool can_close = (running_shells() > 1);
    set_sensitive(file_menu[FileItems::Close].widget, can_close);

#if 0
    // If we have only one window, remove the `Close' item
    Boolean one_window = 
	!app_data.separate_source_window && !app_data.separate_data_window;
    manage_child(file_menu[FileItems::Close].widget, !one_window);
#endif
}

static void gdbUpdateViewCB(CB_ARG_LIST_2(client_data))
{
    MMDesc *view_menu = (MMDesc *)client_data;
    if (view_menu == 0 || view_menu[0].widget == 0)
	return;

    count_mapped_menus(CB_ARGS_3(NULL));

    set_sensitive(view_menu[CodeWindow].widget, gdb->type() == GDB);
    set_sensitive(view_menu[ExecWindow].widget, gdb->has_redirection());
}

static void gdbUpdateViewsCB(CB_ARG_LIST_2(client_data))
{
    gdbUpdateViewCB(CB_ARGS_2(client_data));

    MMDesc *view_menu = (MMDesc *)client_data;
    if (view_menu == 0 || view_menu[0].widget == 0)
	return;

    set_toggle(view_menu[DataWindow].widget,   have_data_window());
    set_toggle(view_menu[SourceWindow].widget, have_source_window());
    set_toggle(view_menu[CodeWindow].widget,   app_data.disassemble);
    set_toggle(view_menu[GDBWindow].widget,    have_command_window());
}

void update_edit_menus()
{
    if (gdb_w == 0)
	return;

    XtPointer call_data = 0;

    gdbUpdateEditCB(CB_ARGS_12(gdb_w, GDBWindow));
    gdbUpdateEditCB(CB_ARGS_12(gdb_w, SourceWindow));
    gdbUpdateEditCB(CB_ARGS_12(gdb_w, DataWindow));
}

// In case we have tear-off menus, all these menus must be updated at
// all times.
static void gdbUpdateAllMenus()
{
    if (mapped_menus() == 0)
	return;			// No mapped menu

    update_edit_menus();

    XtPointer call_data = 0;

    gdbUpdateFileCB(CB_ARGS_2(XtPointer(command_file_menu)));
    gdbUpdateFileCB(CB_ARGS_2(XtPointer(source_file_menu)));
    gdbUpdateFileCB(CB_ARGS_2(XtPointer(data_file_menu)));

    gdbUpdateViewsCB(CB_ARGS_2(XtPointer(views_menu)));

    gdbUpdateViewCB(CB_ARGS_2(XtPointer(command_view_menu)));
    gdbUpdateViewCB(CB_ARGS_2(XtPointer(source_view_menu)));
    gdbUpdateViewCB(CB_ARGS_2(XtPointer(data_view_menu)));
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

#ifdef IF_MOTIF
#if HAVE_X11_XMU_EDITRES_H
    // Make the shell handle EditRes messages
    XtRemoveEventHandler(shell, EventMask(0), True,
			 XtEventHandler(_XEditResCheckMessages), 
			 XtNIL);
    XtAddEventHandler(shell, EventMask(0), True,
		      XtEventHandler(_XEditResCheckMessages),
		      XtNIL);
#endif
#endif // IF_MOTIF

    // Use DDD logo as icon of the new shell
#ifdef IF_MOTIF
    if (shell != 0 && XtIsRealized(shell))
	wm_set_icon(shell, iconlogo(w), iconmask(w));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning wm_set_icon not implemented
#endif
#endif // IF_MOTIF
}

//-----------------------------------------------------------------------------
// Misc functions
//-----------------------------------------------------------------------------

#ifdef IF_MOTIF
// Xt Warning handler
static void ddd_xt_warning(String message)
{
    if (!app_data.suppress_warnings)
    {
	ddd_original_xt_warning_handler(message);

	static bool informed = false;

	if (!informed)
	{
	    std::cerr << "(Annoyed?  "
		"Try 'Edit->Preferences->General->Suppress X Warnings'!)\n";
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
#endif // IF_MOTIF



//-----------------------------------------------------------------------------
// Splash Screen
//-----------------------------------------------------------------------------

static Widget splash_shell  = 0;
#ifdef IF_MOTIF
static Pixmap splash_pixmap = None;
#else // NOT IF_MOTIF
// Cannot initialize Glib::RefPtr.
static Pixmap splash_pixmap;
#endif // IF_MOTIF
static _Delay *splash_delay = 0;

static void popdown_splash_screen(XtPointer data, XtIntervalId *id)
{
    (void) id;			// use it

    if (data != 0)
    {
#ifdef IF_MOTIF
	XtIntervalId *timer = (XtIntervalId *)data;
	assert(*timer == *id);
	*timer = 0;
#else // NOT IF_MOTIF
	std::cerr << "popdown_splash_screen(): timer?\n";
#endif // IF_MOTIF
    }
    
    if (splash_shell != 0)
    {
	if (splash_pixmap) {
#ifdef IF_MOTIF
	    XFreePixmap(XtDisplay(splash_shell), splash_pixmap);
#else // NOT IF_MOTIF
	    splash_pixmap.clear();
#endif // IF_MOTIF
	}

	popdown_shell(splash_shell);
	DestroyWhenIdle(splash_shell);
	splash_shell = 0;

	delete splash_delay;
	splash_delay = 0;
    }
}

static void popup_splash_screen(Widget parent, const string& color_key)
{
#ifdef IF_MOTIF
    popdown_splash_screen();

#ifdef IF_MOTIF
    XErrorBlocker blocker(XtDisplay(parent));
#endif // IF_MOTIF

    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNallowShellResize, True);  arg++;
    XtSetArg(args[arg], XmNborderWidth, 0);          arg++;
    XtSetArg(args[arg], XmNdeleteResponse, XmUNMAP); arg++;

    XtSetArg(args[arg], XmNmwmDecorations, MWM_DECOR_BORDER); arg++;
    XtSetArg(args[arg], XmNmwmFunctions,   MWM_FUNC_CLOSE); arg++;
    splash_shell = verify(
        XmCreateDialogShell(parent, 
			    XMST( "splash_shell"),
			    args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNborderWidth, 0);  arg++;
    XtSetArg(args[arg], XmNmarginWidth, 0);  arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    Widget splash = 
	verify(XmCreateDrawingArea(splash_shell, XMST("splash"), args, arg));
    XtManageChild(splash);

    Dimension width, height;
    splash_pixmap = dddsplash(splash, color_key, width, height);

#ifdef IF_MOTIF
    if (blocker.error_occurred())
	splash_pixmap = None;
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XErrorBlocker not defined
#endif
#endif // IF_MOTIF

    if (splash_pixmap == None)
	return;

    XtVaSetValues(splash,
		  XmNbackgroundPixmap, splash_pixmap,
		  XmNwidth, width,
		  XmNheight, height,
		  XtNIL);

    int x = (WidthOfScreen(XtScreen(splash_shell)) - width) / 2;
    int y = (HeightOfScreen(XtScreen(splash_shell)) - height) / 2;

    XtVaSetValues(splash_shell, XmNx, x, XmNy, y, XtNIL);

    // Place lock warning on top of startup splash
    lock_dialog_x = x + 20;
    lock_dialog_y = y + 20;

    XtRealizeWidget(splash_shell);

    splash_delay = new _Delay(splash_shell);

    popup_shell(splash_shell);
    wait_until_mapped(splash, splash_shell);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Implement splash screen?
#endif
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
static void SetSplashScreenCB(CB_ARG_LIST_3(call_data))
#else // NOT IF_MOTIF
static void SetSplashScreenCB(TOGGLEBUTTON_P w)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.splash_screen = info->set;

    update_options(NO_UPDATE);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Implement splash screen?
#endif
#endif // IF_MOTIF
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
    std::cerr << "FIXME: ReportDeathHP\n";
    GDBAgent *gdb = ptr_cast(GDBAgent, agent);
    if (gdb == 0)
	return;

    char *reason = STATIC_CAST(char *,call_data);
    if (gdb != 0 && running_msg != 0)
    {
	running_msg->outcome = reason;
	delete running_msg;
	running_msg = 0;
    }
}

static void ClearDialogCB(Widget, XtPointer client_data, XtPointer)
{
    Widget *dialog = (Widget *)client_data;
    *dialog = 0;
}

static void vsl_echo(const string& msg)
{
    static Widget dialog = 0;

    if (dialog == 0)
    {
	// We report only the first message.  
	// Everything else only goes into the status line.
	MString message = rm("The VSL interpreter failed:") + cr() + tt(msg);

#ifdef IF_MOTIF
	Arg args[10];
	Cardinal arg = 0;

	XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY);          arg++;
	XtSetArg(args[arg], XmNautoUnmanage,   False);              arg++;
	XtSetArg(args[arg], XmNmessageString,  message.xmstring()); arg++;
	dialog = verify(XmCreateWarningDialog(find_shell(gdb_w), 
					      XMST("vsl_message"), 
					      args, arg));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, 
					     XmDIALOG_CANCEL_BUTTON));

	Delay::register_shell(dialog);
	XtAddCallback(dialog, XmNokCallback,      DestroyShellCB, 0);
	XtAddCallback(dialog, XmNcancelCallback,  DestroyShellCB, 0);
	XtAddCallback(dialog, XmNhelpCallback,    ImmediateHelpCB, 0);
	XtAddCallback(dialog, XmNdestroyCallback, ClearDialogCB, &dialog);
#else // NOT IF_MOTIF
	dialog = new Gtk::MessageDialog(message.xmstring(), false, Gtk::MESSAGE_WARNING);
#endif // IF_MOTIF

    }

    manage_and_raise(dialog);

    set_status_mstring(rm("VSL: ") + tt(msg));
}


//-----------------------------------------------------------------------------
// Emergency
//-----------------------------------------------------------------------------

extern "C" {
    static Bool is_emergency(DISPLAY_P, XEvent *event, char *)
    {
	switch (event->type)
	{
	case KeyPress:
	{
#ifdef IF_MOTIF
	    char buffer[1024];
	    KeySym keysym;

	    int len = XLookupString(&event->xkey, buffer, sizeof buffer, 
				    &keysym, (XComposeStatus *)0);
#else // NOT IF_MOTIF
	    int len = event->key.length;
	    char *buffer = event->key.string;
#endif // IF_MOTIF
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
}

bool process_emergencies()
{
    XEvent event;
#ifdef IF_MOTIF
    if (XCheckIfEvent(XtDisplay(gdb_w), &event, is_emergency, 0))
    {
	// Emergency: process this event immediately
	XtDispatchEvent(&event);
	return true;
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning process_emergencies: scan event queue for high-priority events?
#endif
#endif // IF_MOTIF

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
    std::ifstream log(logname.chars());
    if (log.bad())
    {
	(void) fopen(logname.chars(), "r");
	perror(logname.chars());
	exit(EXIT_FAILURE);
    }

    while (log)
    {
	char buffer[1024];
	log.getline(buffer, sizeof buffer);
	string log_line(buffer);

	if (log_line.contains("+  ", 0))
	{
	    get_debugger_type(log_line, type);
	    return;
	}
    }
}


//-----------------------------------------------------------------------------
// `Show' options
//-----------------------------------------------------------------------------

// Return true iff resource is defined and set
static string resource_value(XrmDatabase db, const string& app_name, const char *res_name)
{
    string str_name  = app_name + "." + res_name;
    string str_class = string(DDD_CLASS_NAME) + "." + res_name;

    char *type;
#ifdef IF_MOTIF
#ifdef IF_MOTIF
    XrmValue xrmvalue;
#else // NOT IF_MOTIF
    Glib::Value<std::string> xrmvalue;
    xrmvalue.init(Glib::Value<std::string>::value_type());
#endif // IF_MOTIF
#ifdef IF_MOTIF
    Bool success = XrmGetResource(db, str_name.chars(), str_class.chars(), &type, &xrmvalue);
#else // NOT IF_MOTIF
    Bool success = get_resource(db, str_name.chars(), str_class.chars(), xrmvalue);
#endif // IF_MOTIF
    if (!success)
	return "";		// Resource not found

#ifdef IF_MOTIF
    const char *str = (const char *)xrmvalue.addr;
    int len   = xrmvalue.size - 1; // includes the final `\0'
#else // NOT IF_MOTIF
    const char *str = xrmvalue.get().c_str();
    int len = strlen(str);
#endif // IF_MOTIF
    return string(str, len);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning BUGGER THIS: Glib::Value is totally undocumented cruft.
#endif
    return string();
#endif // IF_MOTIF
}

static bool is_set(string value)
{
    value.downcase();

    if (value == "on" || value == "true" || value == "yes")
	return true;
    else if (value == "off" || value == "false" || value == "no")
	return false;

    // Illegal value
    return false;
}

inline bool have_set_resource(XrmDatabase db, const char *app_name, const char *res_name)
{
    return is_set(resource_value(db, app_name, res_name));
}

static void setup_show(XrmDatabase db, const char *app_name, const char *gdb_name)
{
    // Check for `--version', `--help', `--news', etc.  This may be
    // invoked before we have connected to an X display, so check
    // APP_DATA as well as the resource database (initialized from the
    // command line).

    bool cont = true;
    if (app_data.show_version || 
	have_set_resource(db, app_name, XtNshowVersion))
    {
	show_version(std::cout);
	cont = false;
    }
    if (app_data.show_invocation ||
	have_set_resource(db, app_name, XtNshowInvocation))
    {
	show_invocation(gdb_name, std::cout);
	cont = false;
    }
    if (app_data.show_configuration ||
	have_set_resource(db, app_name, XtNshowConfiguration))
    {
	show_configuration(std::cout);
	cont = false;
    }
    if (app_data.show_news ||
	have_set_resource(db, app_name, XtNshowNews))
    {
	show(ddd_news);
	cont = false;
    }
    if (app_data.show_license ||
	have_set_resource(db, app_name, XtNshowLicense))
    {
	show(ddd_license);
	cont = false;
    }
    if (app_data.show_manual ||
	have_set_resource(db, app_name, XtNshowManual))
    {
	show(ddd_man);
	cont = false;
    }
    if (!cont)
	exit(EXIT_SUCCESS);
}


//-----------------------------------------------------------------------------
// Various setups
//-----------------------------------------------------------------------------

// This text is shown in `About DDD', appended to the value of the
// `helpOnVersionString' resource.
static void setup_version_info()
{
    std::ostringstream os;
    show_configuration(os);
    string cinfo(os);

    cinfo.gsub("(C)", "\251");

    // Set e-mail address in @tt; the remainder in @rm
    int cinfo_lt = cinfo.index('<', -1);
    int cinfo_gt = cinfo.index('>', -1);
#ifdef IF_MOTIF
    if (cinfo_lt >= 0 && cinfo_gt >= 0)
    {
	helpOnVersionExtraText = rm(cinfo.before(cinfo_lt));
#if 0
 	helpOnVersionExtraText += cr();	// place e-mail on separate line
#endif
	helpOnVersionExtraText += rm(cinfo.at(cinfo_lt, 1));
	helpOnVersionExtraText += 
	    tt(cinfo.at(cinfo_lt + 1, cinfo_gt - cinfo_lt - 1));
	helpOnVersionExtraText += rm(cinfo.from(cinfo_gt));
    }
    else
    {
	helpOnVersionExtraText = rm(cinfo);
    }
#endif // IF_MOTIF

    string expires = ddd_expiration_date();
    if (!expires.empty())
    {
	// Add expiration date
	string expired_msg = DDD_NAME " " DDD_VERSION " ";
	if (ddd_expired())
	    expired_msg += "has expired since " + expires;
	else
	    expired_msg += "expires " + expires;

#ifdef IF_MOTIF
	helpOnVersionExtraText += rm(expired_msg + ".") + cr();
#endif // IF_MOTIF
    }

#ifdef IF_MOTIF
    helpOnVersionExtraText += cr() 
	+ rm(DDD_NAME " is ") + sl("free software")
	+ rm(" and you are welcome to distribute copies of it") + cr()
	+ rm("under certain conditions; select ")
	+ bf("Help") + MString("\256", CHARSET_SYMBOL) 
	+ bf(DDD_NAME " License") + rm(" to see the") + cr()
	+ rm("conditions.  There is ") + sl("absolutely no warranty") 
	+ rm(" for " DDD_NAME "; see the ") + cr()
	+ rm(DDD_NAME " License for details.") + cr();

#if 0
    helpOnVersionExtraText += cr()
	+ rm("If you find " DDD_NAME " useful, please send "
	     "us a picture postcard:") + cr()
	+ cr()
	+ rm("    Free Software Foundation") + cr()
	+ rm("    59 Temple Place - Suite 330") + cr()
	+ rm("    Boston, MA  02111-1307, USA") + cr();
#endif
#endif // IF_MOTIF

    string log = session_log_file();
    if (log.contains(gethome(), 0))
	log = "~" + log.after(gethome());

#ifdef IF_MOTIF
    helpOnVersionExtraText += cr()
	+ rm("Send bug reports to <")
	+ tt(PACKAGE_BUGREPORT) + rm(">.") + cr()
	+ rm("Always include the ") + tt(log) + rm(" file;")
	+ rm(" see the " DDD_NAME " manual for details.") + cr()
	+ rm("Send comments and suggestions to <")
	+ tt("ddd@gnu.org") + rm(">.") + cr();

    helpOnVersionExtraText += cr()
	+ rm(DDD_NAME " WWW page: ") + tt(app_data.www_page) + cr()
	+ rm(DDD_NAME " mailing list: <")
	+ tt("ddd-request@gnu.org") + rm(">") + cr();
#endif // IF_MOTIF

#if SHOW_COPYRIGHT_IN_SOURCE_WINDOW
    // Duplicate version info in source window
    string s =
	"/----------------------------------------\\\n"
	"|  GGG  N   N U   U    DDDD  DDDD  DDDD  |\n"
	"| G     NN  N U   U    D   D D   D D   D |\n"
	"| G  GG N N N U   U    D   D D   D D   D |\n"
	"| G   G N  NN U   U    D   D D   D D   D |\n"
	"|  GGG  N   N  UUU     DDDD  DDDD  DDDD  |\n"
        "|                                        |\n"
        "|       The Data Display Debugger        |         "
	"by Dorothea L\374tkehaus\n"
	"\\----------------------------------------/         "
	"and Andreas Zeller\n"
        "\n";

    s += helpOnVersionExtraText.str();
    s.gsub("\256", "->");
    s += "To start, select `Help->What Now?'.";
    XmTextSetString(SourceView::source(), s);
#else
#ifdef IF_MOTIF
    XmTextSetString(gdb_w, XMST(
		    "GNU " DDD_NAME " " DDD_VERSION " (" DDD_HOST "), "
		    "by Dorothea L\374tkehaus and Andreas Zeller.\n"
		    "Copyright \251 1995-1999 "
		    "Technische Universit\344t Braunschweig, Germany.\n"
		    "Copyright \251 1999-2001 "
		    "Universit\344t Passau, Germany.\n"
		    "Copyright \251 2001 "
		    "Universit\344t des Saarlandes, Germany.\n"
		    "Copyright \251 2001-2004 "
		    "Free Software Foundation, Inc.\n"));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Translate this message to UTF-8
#endif
    gdb_w->set_text(XMST("GNU " DDD_NAME " " DDD_VERSION " (" DDD_HOST "), "
			 "by Dorothea Luetkehaus and Andreas Zeller.\n"
			 "Copyright (c) 1995-1999 "
			 "Technische Universitaet Braunschweig, Germany.\n"
			 "Copyright (c) 1999-2001 "
			 "Universitaet Passau, Germany.\n"
			 "Copyright (c) 2001 "
			 "Universitaet des Saarlandes, Germany.\n"
			 "Copyright (c) 2001-2004 "
			 "Free Software Foundation, Inc.\n"));
#endif // IF_MOTIF
#endif
}


#ifdef IF_MOTIF
// Add current selection as argument
static void add_arg_from_selection(Widget toplevel, int& argc, const char **&argv)
{
    assert(argc == 1);

    static string selection;

    for (int i = 0; i < 3; i++)
    {
	switch (i)
	{
	case 0:
	    selection = current_primary(toplevel);
	    break;

	case 1:
	    selection = current_cut_buffer(toplevel);
	    break;

	case 2:
	    selection = current_clipboard(toplevel);
	    break;
	}

	strip_space(selection);

	// Simple hack to handle URLs
	if (selection.contains("file://", 0))
	{
	    selection = selection.after("//");
	    selection = selection.from("/");
	}
	else if (selection.contains("file:", 0))
	    selection = selection.after(':');

	if (selection.empty())
	    continue;			// No selection

	const int new_argc = 2;
	static const char *new_argv[new_argc + 1];
	new_argv[0] = argv[0];
	new_argv[1] = selection.chars();
	new_argv[2] = 0;

	DebuggerInfo info(new_argc, new_argv);
	if (!(info.arg.empty()))
	{
	    // Selection is valid for some debugger.  Go for it.
	    argc = new_argc;
	    argv = new_argv;
	    return;
	}
    }
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Selection does not work yet
#endif
#endif // IF_MOTIF

static void setup_environment()
{
    // Set the type of the execution tty.
    switch (gdb->type())
    {
    case BASH:
    case DBG:
    case DBX:
    case GDB:
    case PERL:
	// The debugger console has few capabilities.
	// When starting the execution TTY, we set the correct type.
	put_environment("TERM", "dumb");
	break;

    case XDB:
    case JDB:
    case PYDB:
	// In these debuggers, we have no means to set the TTY type
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

static void setup_options(int& argc, const char *argv[],
			  StringArray& saved_options, string& gdb_name,
			  bool& no_windows)
{
    int gdb_option_pos = -1;
    int gdb_option_offset = 2;
    int i;
    bool save = false;

    for (i = 1; i < argc; i++)
    {
	string arg = string(argv[i]);

	if (arg == "--")
	{
	    // Anything after `--' is a debugger option.  Skip `--'
	    // and store all remaining options in SAVED_OPTIONS.
	    for (int j = i; j <= argc - 1; j++)
		argv[j] = argv[j + 1];
	    argc--;
	    i--;

	    save = true;
	    continue;
	}

	if (save)
	{
	    // Found `--' - save all remaining options
	    saved_options += arg;

	    for (int j = i; j <= argc - 1; j++)
		argv[j] = argv[j + 1];
	    argc--;
	    i--;

	    continue;
	}

	// Ordinary DDD options
	if ((arg == "--debugger" || arg == "-debugger") && i < argc - 1)
	{
	    gdb_name = argv[i + 1];
	    gdb_option_pos    = i;
	    gdb_option_offset = 2;
	}

	if (arg == "--dbx" || arg == "-dbx" 
	    || arg == "--gdb" || arg == "-gdb"
	    || arg == "--xdb" || arg == "-xdb"
	    || arg == "--jdb" || arg == "-jdb"
	    || arg == "--pydb" || arg == "-pydb"
	    || arg == "--perl" || arg == "-perl"
	    || arg == "--dbg"  || arg == "-dbg")
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
		for (int j = gdb_option_pos; 
		     j <= argc - gdb_option_offset; j++)
		{
		    argv[j] = argv[j + gdb_option_offset];
		}
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

static void setup_command_tty()
{
    if (app_data.tty_mode)
    {
#if WITH_READLINE
	// Initialize Readline
	rl_initialize();
	rl_readline_name = DDD_NAME;
#endif
	
	// Initialize TTY
	init_command_tty();

	// Issue init msg (using 7-bit characters)
#ifdef IF_MOTIF
	string init_msg = XmTextGetString(gdb_w);
	init_msg.gsub("\344", "ae");
	init_msg.gsub("\366", "oe");
	init_msg.gsub("\374", "ue");
	init_msg.gsub("\337", "ss");
	init_msg.gsub("\251", "(C)");
#else // NOT IF_MOTIF
	Glib::ustring init_msg = gdb_w->get_text();
#endif // IF_MOTIF
	tty_out(init_msg);
    }
}

static void setup_ddd_version_warnings()
{
    // Check for app-defaults
    if (app_data.app_defaults_version == 0)
    {
	std::cerr << "Error: No `" DDD_CLASS_NAME "' application defaults file\n"
	    "To resolve this problem, you can:\n"
	    "* set the XAPPLRESDIR environment variable "
	    "to the location of the file `" DDD_CLASS_NAME "', or\n"
	    "* set the " DDD_NAME "_HOME environment variable "
	    "to the location of `" ddd_NAME "/" DDD_CLASS_NAME 
	    "', or\n"
	    "* install the `" DDD_CLASS_NAME "' file in the X "
	    "application defaults directory, or\n"
	    "* recompile " DDD_NAME " with builtin app-defaults file;\n"
	    "  see the `--enable-builtin-app-defaults' option for details.\n";

	exit(EXIT_FAILURE);
    }
    else if (string(app_data.app_defaults_version) != DDD_VERSION)
    {
	std::cerr << "Warning: using `" DDD_CLASS_NAME "' app-defaults file"
	     << " for " DDD_NAME " " << app_data.app_defaults_version 
	     << " (this is " DDD_NAME " " DDD_VERSION ")\n"
	     << "Continue at own risk.\n";

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
	std::cerr << "Warning: using "
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

static void setup_motif_version_warnings()
{
    // Version warnings
    bool risky = false;

#ifdef IF_MOTIF
#if HAVE_XMUSEVERSION
    if (xmUseVersion != XmVersion)
    {
	std::cerr << "Warning: This " DDD_NAME " requires a Motif "
	     << XmVersion / 1000 << "." << XmVersion % 1000 
	     << " library (using Motif "
	     << xmUseVersion / 1000 << "." << xmUseVersion % 1000 
	     << ")\n";

	risky = true;
    }
#endif
#endif // IF_MOTIF

    if (risky)
	std::cerr << "Continue at own risk.\n";
}

static void setup_auto_command_prefix()
{
    if (app_data.auto_command_prefix == 0)
	app_data.auto_command_prefix = "";
    static string prefix = app_data.auto_command_prefix;

    if (prefix.length() < 3)
    {
	// No prefix or insufficient prefix -- generate a new one
	std::ostringstream key;
	key << ddd_NAME << "-" << getpid() << "-" 
	    << (long)time((time_t *)0) << ": ";
	prefix = key;
    }

    app_data.auto_command_prefix = prefix.chars();
}

// All options that remain fixed for a session go here.
static void setup_options()
{
    set_sensitive(disassemble_w, gdb->type() == GDB);
    set_sensitive(code_indent_w, gdb->type() == GDB);
    set_sensitive(examine_w,            gdb->has_examine_command());
    set_sensitive(print_examine_w,      gdb->has_examine_command());
    set_sensitive(cache_machine_code_w, gdb->type() == GDB);

    if (gdb->type() == DBG) {
	app_data.use_source_path = true;
	set_toggle(set_refer_base_w, false);
	set_toggle(set_refer_path_w, true);
    };

    set_sensitive(set_refer_base_w, gdb->type() != GDB && gdb->type() != DBG);
    set_sensitive(set_refer_path_w, gdb->type() != GDB && gdb->type() != DBG);
    set_sensitive(refer_sources_w,  gdb->type() != GDB && gdb->type() != DBG);
    
    set_sensitive(edit_watchpoints_w, gdb->has_watch_command() != 0);

    set_sensitive(command_separate_exec_window_w, gdb->has_redirection());
    set_sensitive(source_separate_exec_window_w,  gdb->has_redirection());
    set_sensitive(data_separate_exec_window_w,    gdb->has_redirection());

    bool have_core = gdb->has_core_files();
    set_sensitive(command_file_menu[FileItems::OpenCore].widget,    have_core);
    set_sensitive(source_file_menu[FileItems::OpenCore].widget,     have_core);
    set_sensitive(data_file_menu[FileItems::OpenCore].widget,       have_core);

    bool have_exec = gdb->has_exec_files() || gdb->type() == PERL || gdb->type() == DBG;
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

    bool have_settings = (gdb->type() != XDB && gdb->type() != PYDB);
    set_sensitive(command_edit_menu[EditItems::Settings].widget,have_settings);
    set_sensitive(source_edit_menu[EditItems::Settings].widget, have_settings);
    set_sensitive(data_edit_menu[EditItems::Settings].widget,   have_settings);

    set_sensitive(complete_w,  gdb->type() == GDB);
    set_sensitive(define_w,    gdb->type() == GDB);
    set_sensitive(signals_w,   gdb->type() == GDB);

    set_sensitive(set_debugger_bash_w, have_cmd("bash"));
    set_sensitive(set_debugger_dbg_w,  have_cmd("dbg"));
    set_sensitive(set_debugger_dbx_w,  have_cmd("dbx") || have_cmd("ladebug"));
    set_sensitive(set_debugger_gdb_w,  have_cmd("gdb"));
    set_sensitive(set_debugger_jdb_w,  have_cmd("jdb"));
    set_sensitive(set_debugger_perl_w, have_cmd("perl"));
    set_sensitive(set_debugger_pydb_w, have_cmd("pydb"));
    set_sensitive(set_debugger_xdb_w,  have_cmd("xdb"));

    bool can_dump = (gdb->type() == JDB);
    // (gdb->print_command("", true) != gdb->print_command("", false));
    manage_child(print_dump_w, can_dump);
}

static void setup_theme_manager()
{
#ifdef IF_MOTIF
    DispBox::theme_manager = ThemeManager(app_data.themes);
    StringArray available_themes;
    get_themes(available_themes);

    for (int i = 0; i < available_themes.size(); i++)
    {
	const char* theme = basename(available_themes[i].chars());

	if (!DispBox::theme_manager.has_pattern(theme))
	{
	    // Add missing themes as inactive unless it's "rednil",
            // which defaults to active 'cos... well... I like it.
	    ThemePattern p;
            if (strcmp(theme, "rednil.vsl") == 0) {
	        p.active() = true;
                p.add("*");
            } else {
	        p.active() = false;
            }
	    DispBox::theme_manager.pattern(theme) = p;
	}
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No themes yet
#endif
#endif // IF_MOTIF
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
