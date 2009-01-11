// $Id$
// DDD main program (and much more)

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 1999-2001 Universitaet Passau, Germany.
// Copyright (C) 2001-2006 Free Software Foundation, Inc.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
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

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
// #include "MemCheckD.h"

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "ddd.h"

#if defined(IF_XM)
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
#if defined(IF_XM)
#include <Xm/ToggleB.h>
#else
#include <Xmmm/RadioButton.h>
#endif
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
#endif

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
#if defined(IF_XM)
#include "LabelH.h"
#endif
#include "MakeMenu.h"
#include "PlotAgent.h"
#include "SourceView.h"
#if defined(IF_XM)
#include "Swallower.h"
#include "TextSetS.h"
#endif
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
#if defined(IF_XM)
#include "converters.h"
#endif
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
#if defined(IF_XM)
#include "fonts.h"
#endif
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
#if defined(IF_XM)
#include "sashes.h"
#endif
#include "select.h"
#if defined(IF_XM)
#include "selection.h"
#endif
#include "session.h"
#include "settings.h"
#include "shell.h"
#include "shorten.h"
#include "show.h"
#include "simpleMenu.h"
#include "source.h"
#include "status.h"
#include "strclass.h"
#include "string-fun.h"
#include "tabs.h"
#include "tempfile.h"
#if defined(IF_XM)
#include "tictactoe.h"
#endif
#include "tips.h"
#include "toolbar.h"
#include "ungrab.h"
#include "verify.h"
#include "version.h"
#include "windows.h"
#include "wm.h"
#if defined(IF_XM)
#include "xconfig.h"
#endif

#if !defined(IF_XM)
#include <sigc++/signal.h>
#include <sigc++/slot.h>
#include <GUI/Box.h>
#include <GUI/Dialog.h>
#include <GUI/Notebook.h>
#include <GUI/MultiPaned.h>
#include <GUI/CheckButton.h>
#include <GUI/CheckMenuItem.h>
#include <GUI/MenuItem.h>
#include <GUI/Button.h>
#include <GUI/Entry.h>
#include <GUI/Main.h>
#include <GUI/ImageW.h>
#endif
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

#if defined(IF_XM)
// Warning proc
static void ddd_xt_warning(String message);
#endif

// Getting ready
#if defined(IF_XM)
static void WhenReady            (Widget, XtPointer, XtPointer);
#else
static void WhenReady            (GUI::Widget *w, void *client_data);
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

// Cut and Paste
#if defined(IF_XM)
static void gdbCutSelectionCB    (Widget, XtPointer, XtPointer);
static void gdbCopySelectionCB   (Widget, XtPointer, XtPointer);
static void gdbPasteClipboardCB  (Widget, XtPointer, XtPointer);
static void gdbDeleteSelectionCB (Widget, XtPointer, XtPointer);
static void gdbClearAllCB        (Widget, XtPointer, XtPointer);
static void gdbSelectAllCB       (Widget, XtPointer, XtPointer);
static void gdbUnselectAllCB     (Widget, XtPointer, XtPointer);
#else
static void gdbCutSelectionCB    (GUI::Widget *, DDDWindow);
static void gdbCopySelectionCB   (GUI::Widget *, DDDWindow);
static void gdbPasteClipboardCB  (GUI::Widget *, DDDWindow);
static void gdbDeleteSelectionCB (GUI::Widget *, DDDWindow);
static void gdbClearAllCB        (void);
static void gdbSelectAllCB       (GUI::Widget *, DDDWindow);
static void gdbUnselectAllCB     (void);
#endif

#if defined(IF_XM)
// Update menus
static void gdbUpdateEditCB      (Widget, XtPointer, XtPointer);
static void gdbUpdateFileCB      (Widget, XtPointer, XtPointer);
static void gdbUpdateViewCB      (Widget, XtPointer, XtPointer);
static void gdbUpdateViewsCB     (Widget, XtPointer, XtPointer);
static void gdbUpdateAllMenus();
#else
// Update menus
static void gdbUpdateEditCB      (GUI::Widget *, DDDWindow);
static void gdbUpdateFileCB      (MMDesc *);
static void gdbUpdateViewCB      (MMDesc *);
static void gdbUpdateViewsCB     (MMDesc *);
static void gdbUpdateAllMenus();
#endif

#if defined(IF_XM)
// Preferences
static void make_preferences (Widget parent);
static void dddPopupPreferencesCB (Widget, XtPointer, XtPointer);
#else
// Preferences
static void make_preferences (GUI::Widget *parent);
static void dddPopupPreferencesCB (void);
#endif

// Synchronize `():' fields
static void sync_args(ArgField *a1, ArgField *a2);

// Return true if user interaction events are pending
static bool pending_interaction();

// Create status line
#if defined(IF_XM)
static void create_status(Widget parent);
#else
static void create_status(GUI::Container *parent);
#endif

// Status LED
static void blink(bool set);
#if defined(IF_XM)
static void ToggleBlinkCB(Widget, XtPointer client_data, XtPointer call_data);
#else
static bool ToggleBlinkCB(GUI::EventButton *ev);
#endif
static void DisableBlinkHP(Agent *, void *, void *);

// GDB Status
static void ReportStartHP(Agent *, void *, void *);
static void ReportDeathHP(Agent *, void *, void *);

// Status history
#if defined(IF_XM)
static void PopupStatusHistoryCB(Widget, XtPointer, XtPointer);
static void PopdownStatusHistoryCB(Widget, XtPointer, XtPointer);
static void PopdownStatusHistoryEH(Widget, XtPointer, XEvent *, Boolean *);

// Argument callback
static void ActivateCB(Widget, XtPointer client_data, XtPointer call_data);
#else
static bool PopupStatusHistoryCB(const GUI::EventButton *, GUI::Widget *);
static bool PopdownStatusHistoryCB(const GUI::EventButton *);

// Argument callback
static void ActivateCB(GUI::Widget *);
#endif

// Verify whether buttons are active
static void verify_buttons(const MMDesc *items);

// Setup shortcut menu
static void set_shortcut_menu(DataDisp *data_disp, const string& expressions);
static void set_shortcut_menu(DataDisp *data_disp);

// Fix the size of the status line
static void fix_status_size();

// Setup new shell
#if defined(IF_XM)
static void setup_new_shell(Widget w);
#else
static void setup_new_shell(GUI::Widget *w);
#endif

// Setup theme manager
static void setup_theme_manager();

#if defined(IF_XM)
// Set `Settings' title
static void set_settings_title(Widget w);
#else
// Set `Settings' title
static void set_settings_title(GUI::Widget *w);
#endif

// Set Cut/Copy/Paste bindings for MENU to STYLE
static void set_cut_copy_paste_bindings(MMDesc *menu, BindingStyle style);

// Set Select All bindings for MENU to STYLE
static void set_select_all_bindings(MMDesc *menu, BindingStyle style);

// Popup DDD splash screen upon start-up.
#if defined(IF_XM)
static void SetSplashScreenCB(Widget, XtPointer, XtPointer);
static void popup_splash_screen(Widget parent, const string& color_key);
static void popdown_splash_screen(XtPointer data = 0, XtIntervalId *id = 0);

// Read in database from FILENAME.  Upon version mismatch, ignore some
// resources such as window sizes.
static XrmDatabase GetFileDatabase(const string &filename);
#else
static void SetSplashScreenCB(GUI::CheckButton *);
static void popup_splash_screen(GUI::Widget *parent, const string& color_key);
static void popdown_splash_screen(void *data = 0, GUI::connection *id = 0);

// Read in database from FILENAME.
static xmlDoc *GetFileDatabase(const string &filename);
#endif

#if defined(IF_XM)
// Lock `~/.ddd'
static bool lock_ddd(Widget parent, LockInfo& info);
#else
// Lock `~/.ddd'
static bool lock_ddd(GUI::Widget *parent, LockInfo& info);
#endif

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
#if defined(IF_XM)
static void setup_cut_copy_paste_bindings(XrmDatabase db);
static void setup_select_all_bindings(XrmDatabase db);
static void setup_show(XrmDatabase db, const char *app_name, const char *gdb_name);

// Help hooks
static void PreHelpOnContext(Widget w, XtPointer, XtPointer);
static void PostHelpOnItem(Widget item);
#else
static void setup_cut_copy_paste_bindings(xmlDoc *db);
static void setup_select_all_bindings(xmlDoc *db);
static void setup_show(xmlDoc *db, const char *app_name, const char *gdb_name);
#endif

// Log player stuff
static void check_log(const string& logname, DebuggerType& type);

#if defined(IF_XM)
// Add current selection as argument
static void add_arg_from_selection(Widget toplevel, 
				   int& argc, const char **&argv);
#endif

#if defined(IF_XM)
#if XmVersion < 2000 || defined(LESSTIF_VERSION)
static void toggleOverstrikeAct (Widget, XEvent*, String*, Cardinal*)
{
    // Do nothing.  (Just a dummy.)
}
#endif
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


#if defined(IF_XM)
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

{ XRMOPTSTR("--make"),                 XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("make") },
{ XRMOPTSTR("-make"),                  XRMOPTSTR(XtNdebugger),             
                                        XrmoptionNoArg,  XPointer("make") },

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
/* -attach is used by jdb to attach to another another running JVM.
It is even described as a useful option to use in the ddd reference.
If we add this as an option as yet a 4th way to specify --attach-windows,
we prevent its use in jdb. 

{ XRMOPTSTR("-attach"),                XRMOPTSTR(XtCSeparate),             
                                        XrmoptionNoArg, XPointer(OFF) },
*/
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
#else
#ifdef NAG_ME
#warning Rewrite options using popt.
#endif
#endif

#if defined(IF_XM)
// Actions
static XtActionsRec actions [] = {
    {XTARECSTR("gdb-control"),            controlAct},
    {XTARECSTR("gdb-interrupt"),          interruptAct},
    {XTARECSTR("gdb-command"),            commandAct},
    {XTARECSTR("gdb-process"),            processAct},
    {XTARECSTR("gdb-delete-or-control"),  delete_or_controlAct},
    {XTARECSTR("gdb-prev-history"),       prev_historyAct},
    {XTARECSTR("gdb-previous-history"),   prev_historyAct},
    {XTARECSTR("gdb-next-history"),       next_historyAct},
    {XTARECSTR("gdb-beginning-of-line"),  beginning_of_lineAct},
    {XTARECSTR("gdb-end-of-line"),        end_of_lineAct},
    {XTARECSTR("gdb-forward-character"),  forward_characterAct},
    {XTARECSTR("gdb-backward-character"), backward_characterAct},
    {XTARECSTR("gdb-set-line"),           set_lineAct},
    {XTARECSTR("gdb-complete-command"),   complete_commandAct},
    {XTARECSTR("gdb-complete-arg"),       complete_argAct},
    {XTARECSTR("gdb-complete-tab"),       complete_tabAct},
    {XTARECSTR("gdb-insert-source-arg"),  insert_source_argAct},
    {XTARECSTR("gdb-insert-graph-arg"),   insert_graph_argAct},
    {XTARECSTR("gdb-isearch-prev"),       isearch_prevAct},
    {XTARECSTR("gdb-isearch-next"),       isearch_nextAct},
    {XTARECSTR("gdb-isearch-exit"),       isearch_exitAct},
    {XTARECSTR("gdb-popup-menu"),         popupAct},
    {XTARECSTR("ddd-next-tab-group"),     next_tab_groupAct},
    {XTARECSTR("ddd-prev-tab-group"),     prev_tab_groupAct},
    {XTARECSTR("ddd-previous-tab-group"), prev_tab_groupAct},
    {XTARECSTR("ddd-get-focus"),          get_focusAct},
    {XTARECSTR("ddd-select-all"),         select_allAct},
#if XmVersion < 2000 || defined(LESSTIF_VERSION)
    {XTARECSTR("toggle-overstrike"),      toggleOverstrikeAct},
#endif
};
#else
#ifdef NAG_ME
#warning Actions not supported
#endif
#endif


//-----------------------------------------------------------------------------
// Menus
//-----------------------------------------------------------------------------

#define RECENT_MENU							\
{									\
    GENTRYL("r1", N_("r1"), MMPush,					\
	    BIND(gdbOpenRecentCB, 1),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 1)),	\
	    0, 0),							\
    GENTRYL("r2", N_("r2"), MMPush,					\
	    BIND(gdbOpenRecentCB, 2),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 2)),	\
	    0, 0),							\
    GENTRYL("r3", N_("r3"), MMPush,					\
	    BIND(gdbOpenRecentCB, 3),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 3)),	\
	    0, 0),							\
    GENTRYL("r4", N_("r4"), MMPush,					\
	    BIND(gdbOpenRecentCB, 4),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 4)),	\
	    0, 0),							\
    GENTRYL("r5", N_("r5"), MMPush,					\
	    BIND(gdbOpenRecentCB, 5),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 5)),	\
	    0, 0),							\
    GENTRYL("r6", N_("r6"), MMPush,					\
	    BIND(gdbOpenRecentCB, 6),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 6)),	\
	    0, 0),							\
    GENTRYL("r7", N_("r7"), MMPush,					\
	    BIND(gdbOpenRecentCB, 7),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 7)),	\
	    0, 0),							\
    GENTRYL("r8", N_("r8"), MMPush,					\
	    BIND(gdbOpenRecentCB, 8),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 8)),	\
	    0, 0),							\
    GENTRYL("r9", N_("r9"), MMPush,					\
	    BIND(gdbOpenRecentCB, 9),					\
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbOpenRecentCB), 9)),	\
	    0, 0),							\
    MMEnd								\
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

#if defined(IF_XM)
// Auxiliary struct used by WhenReady. A pointer to function can not
// be passed directly in a XtPointer.
struct WhenReadyProc_t {
  XtCallbackProc proc;
};
#define DECL_WR(PROC) \
 static WhenReadyProc_t WR_##PROC = { PROC }
#else
// Bind everything except the widget argument.  The widget will be
// filled in when we queue the event for processing.
struct WhenReadyProc_t {
    sigc::slot<void, GUI::Widget *> ready;
    WhenReadyProc_t(sigc::slot<void, GUI::Widget *> ready0) {
	ready = ready0;
    }
};

#define DECL_WR(NAME, PROC)						\
    static WhenReadyProc_t NAME (sigc::slot<void, GUI::Widget *>(PROC))
#endif
#if defined(IF_XM)
DECL_WR(gdbOpenClassCB);
DECL_WR(gdbOpenFileCB);
DECL_WR(gdbOpenCoreCB);
DECL_WR(OpenSessionCB);
DECL_WR(SaveSessionAsCB);
DECL_WR(gdbOpenProcessCB);
DECL_WR(gdbChangeDirectoryCB);
DECL_WR(gdbMakeCB);
DECL_WR(gdbMakeAgainCB);
#else
DECL_WR(WR_gdbOpenClassCB, sigc::ptr_fun(gdbOpenClassCB));
DECL_WR(WR_gdbOpenFileCB, sigc::ptr_fun(gdbOpenFileCB));
DECL_WR(WR_gdbOpenCoreCB, sigc::ptr_fun(gdbOpenCoreCB));
DECL_WR(WR_OpenSessionCB, sigc::ptr_fun(OpenSessionCB));
DECL_WR(WR_SaveSessionAsCB, sigc::ptr_fun(SaveSessionAsCB));
DECL_WR(WR_gdbOpenProcessCB, sigc::ptr_fun(gdbOpenProcessCB));
DECL_WR(WR_gdbChangeDirectoryCB, sigc::ptr_fun(gdbChangeDirectoryCB));
DECL_WR(WR_gdbMakeCB, sigc::ptr_fun(gdbMakeCB));
DECL_WR(WR_gdbMakeAgainCB, sigc::ptr_fun(gdbMakeAgainCB));
#endif

#define FILE_MENU(recent_menu)						\
{									\
    GENTRYL("open_class", N_("Open Class..."), MMPush | MMUnmanaged,	\
	    BIND(WhenReady, &WR_gdbOpenClassCB),			\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbOpenClassCB),	\
	    0, 0),							\
    GENTRYL("open_file", N_("Open Program..."), MMPush,			\
	    BIND(WhenReady, &WR_gdbOpenFileCB),				\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbOpenFileCB),	\
	    0, 0),							\
    GENTRYL("recent", N_("Open Recent"), MMMenu,			\
	    MMNoCB,							\
	    MDUMMY,							\
	    recent_menu, 0),						\
    GENTRYL("open_core", N_("Open Core Dump..."), MMPush,		\
	    BIND(WhenReady, &WR_gdbOpenCoreCB),				\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbOpenCoreCB),	\
	    0, 0),							\
    GENTRYL("open_source", N_("Open Source..."), MMPush, 		\
	    BIND(gdbLookupSourceCB, 0),					\
	    sigc::ptr_fun(gdbLookupSourceCB),				\
	    0, 0),							\
    MMSep,								\
    GENTRYL("open_session", N_("Open Session..."), MMPush,		\
	    BIND(WhenReady, &WR_OpenSessionCB),				\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_OpenSessionCB),	\
	    0, 0),							\
    GENTRYL("save_session", N_("Save Session As..."), MMPush,		\
	    BIND(WhenReady, &WR_SaveSessionAsCB),			\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_SaveSessionAsCB),	\
	    0, 0),							\
    MMSep,								\
    GENTRYL("attach", N_("Attach to Process..."), MMPush,		\
	    BIND(WhenReady, &WR_gdbOpenProcessCB),			\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbOpenProcessCB), \
	    0, 0),							\
    GENTRYL("detach", N_("Detach Process"), MMPush,			\
	    BIND(gdbCommandCB, "detach"),				\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "detach"),		\
	    0, 0),							\
    MMSep,								\
    GENTRYL("print", N_("Print Graph..."), MMPush,			\
	    BIND(PrintGraphCB, 0),					\
	    sigc::retype(sigc::ptr_fun(PrintGraphCB)),			\
	    0, 0),							\
    GENTRYL("printAgain", N_("Print Again"), MMPush | MMUnmanaged,	\
	    BIND(PrintAgainCB, 1L),					\
	    sigc::bind(sigc::retype(sigc::ptr_fun(PrintAgainCB)), 1L),	\
	    0, 0),							\
    GENTRYL("separator", N_(""), MMSeparator | MMUnmanaged, 		\
	    MMNoCB,							\
	    MDUMMY,							\
	    0, 0),							\
    GENTRYL("cd", N_("Change Directory..."), MMPush,			\
	    BIND(WhenReady, &WR_gdbChangeDirectoryCB),			\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbChangeDirectoryCB), \
	    0, 0),							\
    MMSep,								\
    GENTRYL("make", N_("Make..."), MMPush,				\
	    BIND(WhenReady, &WR_gdbMakeCB),				\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbMakeCB),	\
	    0, 0),							\
    GENTRYL("makeAgain", N_("Make Again"), MMPush | MMUnmanaged,	\
	    BIND(WhenReady, &WR_gdbMakeAgainCB),			\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbMakeAgainCB),	\
	    0, 0),							\
    MMSep,								\
    GENTRYL("close", N_("Close"), MMPush,				\
	    BIND(DDDCloseCB, 0),					\
	    sigc::ptr_fun(DDDCloseCB),					\
	    0, 0),							\
    GENTRYL("restart", N_("Restart"), MMPush,				\
	    BIND(DDDRestartCB, 0),					\
	    sigc::ptr_fun(DDDRestartCB),				\
	    0, 0),							\
    GENTRYL("exit", N_("Exit"), MMPush,					\
	    BIND(DDDExitCB, EXIT_SUCCESS),				\
	    sigc::bind(sigc::ptr_fun(DDDExitCB), EXIT_SUCCESS),		\
	    0, 0),							\
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
    GENTRYL("run", N_("Run..."), MMPush, BIND(gdbRunCB, 0),		\
	    sigc::ptr_fun(gdbRunCB), 0, 0),				\
    GENTRYL("run_again", N_("Run Again"), MMPush,			\
	    BIND(gdbCommandCB, "run"),					\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "run"),		\
	    0, 0),							\
    MMSep,								\
    GENTRYL("separateExecWindow", N_("Run in Execution Window"),	\
	    MMCheckItem,						\
	    BIND(dddToggleSeparateExecWindowCB, 0),			\
	    sigc::retype(sigc::ptr_fun(dddToggleSeparateExecWindowCB)),	\
	    0, &(w)),							\
    MMSep,								\
    GENTRYL("step", N_("Step"), MMPush,					\
	    BIND(gdbCommandCB, "step"),					\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "step"),		\
	    0, 0),							\
    GENTRYL("stepi", N_("Step Instruction"), MMPush,			\
	    BIND(gdbCommandCB, "stepi"),				\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "stepi"),		\
	    0, 0),							\
    GENTRYL("next", N_("Next"), MMPush,					\
	    BIND(gdbCommandCB, "next"),					\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "next"),		\
	    0, 0),							\
    GENTRYL("nexti", N_("Next Instruction"), MMPush,			\
	    BIND(gdbCommandCB, "nexti"),				\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "nexti"),		\
	    0, 0),							\
    GENTRYL("until", N_("Until"), MMPush,				\
	    BIND(gdbCommandCB, "until"),				\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "until"),		\
	    0, 0),							\
    GENTRYL("finish", N_("Finish"), MMPush,				\
	    BIND(gdbCommandCB, "finish"),				\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "finish"),		\
	    0, 0),							\
    MMSep,								\
    GENTRYL("cont", N_("Continue"), MMPush,				\
	    BIND(gdbCommandCB, "cont"),					\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "cont"),		\
	    0, 0),							\
    GENTRYL("signal0", N_("Continue Without Signal"), MMPush,		\
	    BIND(gdbCommandCB, "signal 0"),				\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "signal 0"),	\
	    0, 0),							\
    MMSep,								\
    GENTRYL("kill", N_("Kill"), MMPush,					\
	    BIND(gdbCommandCB, "kill"),					\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "kill"),		\
	    0, 0),							\
    GENTRYL("break", N_("Interrupt"), MMPush,				\
	    BIND(gdbCommandCB, "\003"),					\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "\003"),		\
	    0, 0),							\
    GENTRYL("quit", N_("Abort"), MMPush,				\
	    BIND(gdbCommandCB, "\034"),					\
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "\034"),		\
	    0, 0),							\
    MMEnd								\
}

#if defined(IF_XM)
static Widget command_separate_exec_window_w;
static Widget source_separate_exec_window_w;
static Widget data_separate_exec_window_w;
#else
static GUI::RadioButton *command_separate_exec_window_w;
static GUI::RadioButton *source_separate_exec_window_w;
static GUI::RadioButton *data_separate_exec_window_w;
#endif

static MMDesc command_program_menu[]
    = PROGRAM_MENU(command_separate_exec_window_w);
static MMDesc source_program_menu[]
    = PROGRAM_MENU(source_separate_exec_window_w);
static MMDesc data_program_menu[]
    = PROGRAM_MENU(data_separate_exec_window_w);

// Note: The order of entries within the view menus must correspond to
// that within enum DDDWindow!

#define VIEW_MENU(x)						\
{								\
    GENTRYL("tool", N_("Command Tool..."), MMPush,		\
	    BIND(gdbOpenToolWindowCB, 0),			\
	    sigc::hide(sigc::ptr_fun(gdbOpenToolWindowCB)),	\
	    0, 0),						\
    GENTRYL("exec", N_("Execution Window..."), MMPush,		\
	    BIND(gdbOpenExecWindowCB, 0),			\
	    sigc::hide(sigc::ptr_fun(gdbOpenExecWindowCB)),	\
	    0, 0),						\
    MMSep,							\
    GENTRYL("console", N_("@GDB@ Console"), MMPush,		\
	    BIND(gdbOpenCommandWindowCB, 0),			\
	    sigc::hide(sigc::ptr_fun(gdbOpenCommandWindowCB)),	\
	    0, 0),						\
    GENTRYL("source", N_("Source Window"), MMPush,		\
	    BIND(gdbOpenSourceWindowCB, 0),			\
	    sigc::hide(sigc::ptr_fun(gdbOpenSourceWindowCB)),	\
	    0, 0),						\
    GENTRYL("data", N_("Data Window"), MMPush,			\
	    BIND(gdbOpenDataWindowCB, 0),			\
	    sigc::hide(sigc::ptr_fun(gdbOpenDataWindowCB)),	\
	    0, 0),						\
    GENTRYL("code", N_("Machine Code Window"), MMCheckItem | x,	\
	    BIND(gdbToggleCodeWindowCB, 0),			\
	    sigc::retype(sigc::ptr_fun(gdbToggleCodeWindowCB)),	\
	    0, 0),						\
    MMEnd							\
}

static MMDesc command_view_menu[] = VIEW_MENU(MMUnmanaged);
static MMDesc source_view_menu[]  = VIEW_MENU(0);
static MMDesc data_view_menu[]    = VIEW_MENU(MMUnmanaged);

static MMDesc views_menu[] =
{
    GENTRYL("tool", N_("Command Tool..."), MMPush,
	    BIND(gdbOpenToolWindowCB, 0),
	    sigc::hide(sigc::ptr_fun(gdbOpenToolWindowCB)),
	    0, 0),
    GENTRYL("exec", N_("Execution Window..."), MMPush,
	    BIND(gdbOpenExecWindowCB, 0),
	    sigc::hide(sigc::ptr_fun(gdbOpenExecWindowCB)),
	    0, 0),
    MMSep,
    GENTRYL("console", N_("@GDB@ Console"), MMCheckItem,
	    BIND(gdbToggleCommandWindowCB, 0),
	    sigc::retype(sigc::ptr_fun(gdbToggleCommandWindowCB)),
	    0, 0),
    GENTRYL("source", N_("Source Window"), MMCheckItem,
	    BIND(gdbToggleSourceWindowCB, 0),
	    sigc::retype(sigc::ptr_fun(gdbToggleSourceWindowCB)),
	    0, 0),
    GENTRYL("data", N_("Data Window"), MMCheckItem,
	    BIND(gdbToggleDataWindowCB, 0),
	    sigc::retype(sigc::ptr_fun(gdbToggleDataWindowCB)),
	    0, 0),
    GENTRYL("code", N_("Machine Code Window"), MMCheckItem,
	    BIND(gdbToggleCodeWindowCB, 0),
	    sigc::retype(sigc::ptr_fun(gdbToggleCodeWindowCB)),
	    0, 0),
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

#if defined(IF_XM)
DECL_WR(dddPopupSettingsCB);
#else
DECL_WR(WR_dddPopupSettingsCB, sigc::ptr_fun(dddPopupSettingsCB));
#endif

#define EDIT_MENU(win, w)						\
{									\
    GENTRYL("undo", N_("Undo"), MMPush,					\
	    BIND(gdbUndoCB, 0),						\
	    sigc::hide(sigc::ptr_fun(gdbUndoCB)),			\
	    0, 0),							\
    GENTRYL("redo", N_("Redo"), MMPush,					\
	    BIND(gdbRedoCB, 0),						\
	    sigc::hide(sigc::ptr_fun(gdbRedoCB)),			\
	    0, 0),							\
    MMSep,								\
    GENTRYL("cut", N_("Cut"), MMPush,					\
	    BIND(gdbCutSelectionCB, win),				\
	    sigc::bind(sigc::ptr_fun(gdbCutSelectionCB), win),		\
	    0, 0),							\
    GENTRYL("copy", N_("Copy"), MMPush,					\
	    BIND(gdbCopySelectionCB, win),				\
	    sigc::bind(sigc::ptr_fun(gdbCopySelectionCB), win),		\
	    0, 0),							\
    GENTRYL("paste", N_("Paste"), MMPush,				\
	    BIND(gdbPasteClipboardCB, win),				\
	    sigc::bind(sigc::ptr_fun(gdbPasteClipboardCB), win),	\
	    0, 0),							\
    GENTRYL("clearAll", N_("Clear"), MMPush,				\
	    BIND(gdbClearAllCB, 0),					\
	    sigc::hide(sigc::ptr_fun(gdbClearAllCB)),			\
	    0, 0),							\
    GENTRYL("delete", N_("Delete"), MMPush,				\
	    BIND(gdbDeleteSelectionCB, win),				\
	    sigc::bind(sigc::ptr_fun(gdbDeleteSelectionCB), win),	\
	    0, 0),							\
    MMSep,								\
    GENTRYL("selectAll", N_("Select All"), MMPush,			\
	    BIND(gdbSelectAllCB, win),					\
	    sigc::bind(sigc::ptr_fun(gdbSelectAllCB), win),		\
	    0, 0),							\
    MMSep,								\
    GENTRYL("preferences", N_("Preferences..."), MMPush,		\
	    BIND(dddPopupPreferencesCB, 0),				\
	    sigc::hide(sigc::ptr_fun(dddPopupPreferencesCB)),		\
	    0, 0),							\
    GENTRYL("settings", N_("Settings..."), MMPush,			\
	    BIND(WhenReady, &WR_dddPopupSettingsCB),			\
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_dddPopupSettingsCB), \
	    0, 0),							\
    MMSep,								\
    GENTRYL("saveOptions", N_("Save Options"), MMCheckItem,		\
	    BIND(dddToggleSaveOptionsOnExitCB, 0),			\
	    sigc::retype(sigc::ptr_fun(dddToggleSaveOptionsOnExitCB)),	\
	    0, &w),							\
    MMEnd								\
}


#if defined(IF_XM)
static Widget command_save_options_w;
static Widget source_save_options_w;
static Widget data_save_options_w;
#else
static GUI::CheckMenuItem *command_save_options_w;
static GUI::CheckMenuItem *source_save_options_w;
static GUI::CheckMenuItem *data_save_options_w;
#endif

static MMDesc command_edit_menu[] = EDIT_MENU(GDBWindow, 
					      command_save_options_w);
static MMDesc source_edit_menu[]  = EDIT_MENU(SourceWindow,
					      source_save_options_w);
static MMDesc data_edit_menu[]    = EDIT_MENU(DataWindow,
					      data_save_options_w);

#if defined(IF_XM)
static Widget complete_w;
static Widget define_w;
#else
static GUI::Button *complete_w;
static GUI::Button *define_w;
#endif

#if defined(IF_XM)
DECL_WR(gdbCompleteCB);
DECL_WR(gdbApplyCB);
#else
DECL_WR(WR_gdbCompleteCB, sigc::ptr_fun(gdbCompleteCB));
DECL_WR(WR_gdbApplyCB, sigc::ptr_fun(gdbApplyCB));
#endif

static MMDesc command_menu[] =
{
    GENTRYL("history", N_("Command History..."), MMPush,
	    BIND(gdbHistoryCB, 0),
	    sigc::ptr_fun(gdbHistoryCB),
	    0, 0),
    MMSep,
    GENTRYL("prev", N_("Previous"), MMPush,
	    BIND(gdbPrevCB, 0),
	    sigc::ptr_fun(gdbPrevCB),
	    0, 0),
    GENTRYL("next", N_("Next"), MMPush,
	    BIND(gdbNextCB, 0),
	    sigc::ptr_fun(gdbNextCB),
	    0, 0),
    MMSep,
    GENTRYL("isearch_prev", N_("Find Backward"), MMPush,
	    BIND(gdbISearchPrevCB, 0),
	    sigc::ptr_fun(gdbISearchPrevCB),
	    0, 0),
    GENTRYL("isearch_next", N_("Find Forward"), MMPush,
	    BIND(gdbISearchNextCB, 0),
	    sigc::ptr_fun(gdbISearchNextCB),
	    0, 0),
    GENTRYL("isearch_exit", N_("Quit Search"), MMPush,
	    BIND(gdbISearchExitCB, 0),
	    sigc::ptr_fun(gdbISearchExitCB),
	    0, 0),
    MMSep,
    GENTRYL("complete", N_("Complete"), MMPush,
	    BIND(WhenReady, &WR_gdbCompleteCB), 
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbCompleteCB), 
	    0, &complete_w),
    GENTRYL("apply", N_("Apply"), MMPush,
	    BIND(WhenReady, &WR_gdbApplyCB),
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_gdbApplyCB),
	    0, 0),
    MMSep,
    GENTRYL("clear_line", N_("Clear Line"), MMPush,
	    BIND(gdbClearCB, 0), 
	    sigc::hide(sigc::ptr_fun(gdbClearCB)), 
	    0, 0),
    GENTRYL("clear_window", N_("Clear Window"), MMPush,
	    BIND(gdbClearWindowCB, 0), 
	    sigc::hide(sigc::ptr_fun(gdbClearWindowCB)), 
	    0, 0),
    MMSep,
    GENTRYL("define", N_("Define Command..."), MMPush,
	    BIND(dddDefineCommandCB, 0),
	    sigc::ptr_fun(dddDefineCommandCB),
	    0, &define_w),
    GENTRYL("buttons", N_("Edit Buttons..."), MMPush,
	    BIND(dddEditButtonsCB, 0),
	    sigc::ptr_fun(dddEditButtonsCB),
	    0, 0),
    MMEnd
};

#if defined(IF_XM)
static Widget stack_w;
static Widget registers_w;
static Widget threads_w;
static Widget signals_w;
#else
static GUI::Button *stack_w;
static GUI::Button *registers_w;
static GUI::Button *threads_w;
static GUI::Button *signals_w;
#endif

#if defined(IF_XM)
static WhenReadyProc_t WR_ViewStackFramesCB = { SourceView::ViewStackFramesCB };
static WhenReadyProc_t WR_ViewRegistersCB = { SourceView::ViewRegistersCB };
static WhenReadyProc_t WR_ViewThreadsCB = { SourceView::ViewThreadsCB };
DECL_WR(dddPopupSignalsCB);
#else
DECL_WR(WR_ViewStackFramesCB, sigc::ptr_fun(SourceView::ViewStackFramesCB));
DECL_WR(WR_ViewRegistersCB, sigc::ptr_fun(SourceView::ViewRegistersCB));
DECL_WR(WR_ViewThreadsCB, sigc::ptr_fun(SourceView::ViewThreadsCB));
DECL_WR(WR_dddPopupSignalsCB, sigc::ptr_fun(dddPopupSignalsCB));
#endif
static MMDesc stack_menu[] =
{
    GENTRYL("stack", N_("Backtrace..."), MMPush,
	    BIND(WhenReady, &WR_ViewStackFramesCB),
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_ViewStackFramesCB),
	    0, &stack_w),
    GENTRYL("registers", N_("Registers..."), MMPush,
	    BIND(WhenReady, &WR_ViewRegistersCB),
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_ViewRegistersCB),
	    0, &registers_w),
    GENTRYL("threads", N_("Threads..."), MMPush,
	    BIND(WhenReady, &WR_ViewThreadsCB),
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_ViewThreadsCB),
	    0, &threads_w),
    GENTRYL("signals", N_("Signals..."), MMPush,
	    BIND(WhenReady, &WR_dddPopupSignalsCB),
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_dddPopupSignalsCB),
	    0, &signals_w),
    MMSep,
    GENTRYL("up", N_("Up"), MMPush,
	    BIND(gdbCommandCB, "up"),
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "up"),
	    0, 0),
    GENTRYL("down", N_("Down"), MMPush,
	    BIND(gdbCommandCB, "down"),
	    sigc::bind(sigc::ptr_fun(gdbCommandCB), "down"),
	    0, 0),
    MMEnd
};

#if defined(IF_XM)
static Widget lookup_w;
static Widget find_forward_w;
static Widget find_backward_w;
static Widget find_words_only_w;
static Widget find_case_sensitive_w;
static Widget disassemble_w;
static Widget edit_source_w;
static Widget reload_source_w;
static Widget line_numbers1_w;
#else
static GUI::Widget *lookup_w;
static GUI::Widget *find_forward_w;
static GUI::Widget *find_backward_w;
static GUI::CheckButton *find_words_only_w;
static GUI::CheckButton *find_case_sensitive_w;
static GUI::CheckButton *disassemble_w;
static GUI::Widget *edit_source_w;
static GUI::Widget *reload_source_w;
static GUI::CheckButton *line_numbers1_w;
#endif

static MMDesc source_menu[] =
{
    GENTRYL("breakpoints", N_("Breakpoints..."), MMPush,
	    BIND(SourceView::EditBreakpointsCB, 0),
	    sigc::ptr_fun(SourceView::EditBreakpointsCB),
	    0, 0),
    MMSep,
    GENTRYL("lookup", N_("Lookup"), MMPush,
	    BIND(gdbLookupCB, 0),
	    sigc::hide(sigc::ptr_fun(gdbLookupCB)),
	    0, &lookup_w),
    GENTRYL("findForward", N_("@charset Find@small>> @charset()@rm"), MMPush,
	    BIND(gdbFindCB, SourceView::forward),
	    sigc::bind(sigc::ptr_fun(gdbFindCB), SourceView::forward),
	    0, &find_forward_w),
    GENTRYL("findBackward", N_("@charset Find@small<< @charset()@rm"), MMPush,
	    BIND(gdbFindCB, SourceView::backward),
	    sigc::bind(sigc::ptr_fun(gdbFindCB), SourceView::backward),
	    0, &find_backward_w),
    MMSep,
    GENTRYL("findWordsOnly", N_("Find Words Only"), MMCheckItem,
	    BIND(sourceToggleFindWordsOnlyCB, 0), 
	    sigc::retype(sigc::ptr_fun(sourceToggleFindWordsOnlyCB)),
	    0, &find_words_only_w),
    GENTRYL("findCaseSensitive", N_("Find Case Sensitive"), MMCheckItem,
	    BIND(sourceToggleFindCaseSensitiveCB, 0), 
	    sigc::retype(sigc::ptr_fun(sourceToggleFindCaseSensitiveCB)),
	    0, &find_case_sensitive_w),
    MMSep,
    GENTRYL("lineNumbers", N_("Display Line Numbers"), MMCheckItem,
	    BIND(sourceToggleDisplayLineNumbersCB, 0),
	    sigc::retype((sigc::ptr_fun(sourceToggleDisplayLineNumbersCB))),
	    0, &line_numbers1_w),
    GENTRYL("disassemble", N_("Display Machine Code"), MMCheckItem,
	    BIND(gdbToggleCodeWindowCB, 0),
	    sigc::retype(sigc::ptr_fun(gdbToggleCodeWindowCB)),
	    0, &disassemble_w),
    MMSep,
    GENTRYL("edit", N_("Edit Source..."), MMPush, 
	    BIND(gdbEditSourceCB, 0),
	    sigc::ptr_fun(gdbEditSourceCB),
	    0, &edit_source_w),
    GENTRYL("reload", N_("Reload Source"), MMPush, 
	    BIND(gdbReloadSourceCB, 0),
	    sigc::hide(sigc::ptr_fun(gdbReloadSourceCB)),
	    0, &reload_source_w),
    MMEnd
};


// Preferences

// General preferences

#if defined(IF_XM)
static Widget button_tips_w;
static Widget button_docs_w;
#else
static GUI::CheckButton *button_tips_w;
static GUI::CheckButton *button_docs_w;
#endif
static MMDesc button_menu [] = 
{
    GENTRYL("tips", N_("as Popup Tips"), MMToggle,
	    BIND(dddToggleButtonTipsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleButtonTipsCB)),
	    0, &button_tips_w),
    GENTRYL("docs", N_("in the Status Line"), MMToggle,
	    BIND(dddToggleButtonDocsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleButtonDocsCB)),
	    0, &button_docs_w),
    MMEnd
};

#if defined(IF_XM)
static Widget value_tips_w;
static Widget value_docs_w;
#else
static GUI::CheckButton *value_tips_w;
static GUI::CheckButton *value_docs_w;
#endif
static MMDesc value_menu [] = 
{
    GENTRYL("tips", N_("as Popup Tips"), MMToggle,
	    BIND(dddToggleValueTipsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleValueTipsCB)),
	    0, &value_tips_w),
    GENTRYL("docs", N_("in the Status Line"), MMToggle,
	    BIND(dddToggleValueDocsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleValueDocsCB)),
	    0, &value_docs_w),
    MMEnd
};

#if defined(IF_XM)
static Widget set_global_completion_w;
static Widget set_console_completion_w;
#else
static GUI::RadioButton *set_global_completion_w;
static GUI::RadioButton *set_console_completion_w;
#endif
static MMDesc completion_menu [] = 
{
    GENTRYL("inAllWindows", N_("in All Windows"), MMRadio, 
	    BIND(dddSetGlobalTabCompletionCB, True), 
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetGlobalTabCompletionCB)), true), 
	    0, &set_global_completion_w),
    GENTRYL("inConsole", N_("in Console Only"), MMRadio,
	    BIND(dddSetGlobalTabCompletionCB, False),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetGlobalTabCompletionCB)), false),
	    0, &set_console_completion_w),
    MMEnd
};

#if defined(IF_XM)
static Widget max_undo_size_w;
#else
static GUI::Entry *max_undo_size_w;
#endif
static MMDesc undo_menu [] =
{
    GENTRYL("size", N_("Undo Buffer Size"), MMTextField | MMUnmanagedLabel,
	    BIND(dddSetUndoBufferSizeCB, 0), 
	    sigc::retype(sigc::ptr_fun(dddSetUndoBufferSizeCB)),
	    0, &max_undo_size_w),
    GENTRYL("kbytes", N_("kBytes"), MMLabel,
	    MMNoCB, MDUMMY, 0, 0),
    GENTRYL("clear", N_("Clear Undo Buffer"), MMPush,
	    BIND(dddClearUndoBufferCB, 0),
	    sigc::hide(sigc::ptr_fun(dddClearUndoBufferCB)),
	    0, 0),
    MMEnd
};

#if defined(IF_XM)
static Widget group_iconify_w;
static Widget uniconify_when_ready_w;
static Widget suppress_warnings_w;
static Widget warn_if_locked_w;
static Widget check_grabs_w;
#else
static GUI::CheckButton *group_iconify_w;
static GUI::CheckButton *uniconify_when_ready_w;
static GUI::CheckButton *suppress_warnings_w;
static GUI::CheckButton *warn_if_locked_w;
static GUI::CheckButton *check_grabs_w;
#endif

static MMDesc general_preferences_menu[] = 
{
    GENTRYL("buttonHints", N_("Automatic Display of Button Hints"), MMButtonPanel,
	    MMNoCB, MDUMMY, button_menu, 0),
    GENTRYL("valueHints", N_("Automatic Display of Variable Values"), MMButtonPanel,
	    MMNoCB, MDUMMY, value_menu, 0),
    GENTRYL("tabCompletion", N_("TAB Key Completes"), MMRadioPanel,
	    MMNoCB, MDUMMY, completion_menu, 0),
    GENTRYL("groupIconify", N_("Iconify all Windows at Once"), MMToggle,
	    BIND(dddToggleGroupIconifyCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleGroupIconifyCB)),
	    0, &group_iconify_w),
    GENTRYL("uniconifyWhenReady", N_("Uniconify When Ready"), MMToggle,
	    BIND(dddToggleUniconifyWhenReadyCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleUniconifyWhenReadyCB)),
	    0, &uniconify_when_ready_w),
    GENTRYL("suppressWarnings", N_("Suppress X Warnings"), MMToggle,
	    BIND(dddToggleSuppressWarningsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleSuppressWarningsCB)),
	    0, &suppress_warnings_w),
    GENTRYL("warnIfLocked", N_("Warn if Multiple DDD Instances are Running"), MMToggle,
	    BIND(dddToggleWarnIfLockedCB, 0), 
	    sigc::retype(sigc::ptr_fun(dddToggleWarnIfLockedCB)),
	    0, &warn_if_locked_w),
    GENTRYL("checkGrabs", N_("Continue Automatically when Mouse Pointer is Frozen"), MMToggle,
	    BIND(dddToggleCheckGrabsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleCheckGrabsCB)),
	    0, &check_grabs_w),
    GENTRYL("undoSize", N_("Undo Buffer Size"), MMPanel,
	    MMNoCB, MDUMMY, undo_menu, 0),
    MMEnd
};


// Source preferences
#if defined(IF_XM)
static Widget set_display_glyphs_w;
static Widget set_display_text_w;
#else
static GUI::RadioButton *set_display_glyphs_w;
static GUI::RadioButton *set_display_text_w;
#endif
static MMDesc glyph_menu[] =
{
    GENTRYL("asGlyphs", N_("as Glyphs"), MMRadio,
	    BIND(sourceSetDisplayGlyphsCB, True),
	    sigc::hide(sigc::bind(sigc::ptr_fun(sourceSetDisplayGlyphsCB), true)),
	    0, &set_display_glyphs_w),
    GENTRYL("asText", N_("as Text Characters"), MMRadio,
	    BIND(sourceSetDisplayGlyphsCB, False),
	    sigc::hide(sigc::bind(sigc::ptr_fun(sourceSetDisplayGlyphsCB), false)),
	    0, &set_display_text_w),
    MMEnd
};

#if defined(IF_XM)
static Widget set_tool_buttons_in_toolbar_w;
static Widget set_tool_buttons_in_command_tool_w;
#else
static GUI::RadioButton *set_tool_buttons_in_toolbar_w;
static GUI::RadioButton *set_tool_buttons_in_command_tool_w;
#endif
static MMDesc tool_buttons_menu [] = 
{
    GENTRYL("commandTool", N_("Command Tool"), MMRadio,
	    BIND(dddSetToolBarCB, False),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetToolBarCB)), false),
	    0, &set_tool_buttons_in_command_tool_w),
    GENTRYL("sourceWindow", N_("Source Window"), MMRadio,
	    BIND(dddSetToolBarCB, True),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetToolBarCB)), true),
	    0, &set_tool_buttons_in_toolbar_w),
    MMEnd
};

#if defined(IF_XM)
static Widget set_refer_path_w;
static Widget set_refer_base_w;
#else
static GUI::RadioButton *set_refer_path_w;
static GUI::RadioButton *set_refer_base_w;
#endif
static MMDesc refer_menu[] =
{
    GENTRYL("byPath", N_("by Path Name"), MMRadio,
	    BIND(sourceSetUseSourcePathCB, XtPointer(True)),
	    sigc::hide(sigc::bind(sigc::ptr_fun(sourceSetUseSourcePathCB), true)),
	    0, &set_refer_path_w),
    GENTRYL("byBase", N_("by Base Name"), MMRadio,
	    BIND(sourceSetUseSourcePathCB, XtPointer(False)),
	    sigc::hide(sigc::bind(sigc::ptr_fun(sourceSetUseSourcePathCB), false)),
	    0, &set_refer_base_w),
    MMEnd
};

#if defined(IF_XM)
static Widget words_only_w;
static Widget case_sensitive_w;
#else
static GUI::CheckButton *words_only_w;
static GUI::CheckButton *case_sensitive_w;
#endif
static MMDesc find_preferences_menu[] =
{
    GENTRYL("wordsOnly", N_("Words Only"), MMToggle,
	    BIND(sourceToggleFindWordsOnlyCB, 0), 
	    sigc::retype(sigc::ptr_fun(sourceToggleFindWordsOnlyCB)),
	    0, &words_only_w),
    GENTRYL("caseSensitive", N_("Case Sensitive"), MMToggle,
	    BIND(sourceToggleFindCaseSensitiveCB, 0), 
	    sigc::retype(sigc::ptr_fun(sourceToggleFindCaseSensitiveCB)),
	    0, &case_sensitive_w),
    MMEnd
};

#if defined(IF_XM)
static Widget cache_source_files_w;
static Widget cache_machine_code_w;
#else
static GUI::CheckButton *cache_source_files_w;
static GUI::CheckButton *cache_machine_code_w;
#endif
static MMDesc cache_menu[] =
{
    GENTRYL("cacheSource", N_("Source Files"), MMToggle,
	    BIND(sourceToggleCacheSourceFilesCB, 0), 
	    sigc::retype(sigc::ptr_fun(sourceToggleCacheSourceFilesCB)), 
	    0, &cache_source_files_w),
    GENTRYL("cacheCode", N_("Machine Code"), MMToggle,
	    BIND(sourceToggleCacheMachineCodeCB, 0), 
	    sigc::retype(sigc::ptr_fun(sourceToggleCacheMachineCodeCB)), 
	    0, &cache_machine_code_w),
    MMEnd
};

#if defined(IF_XM)
static Widget tab_width_w;
static Widget source_indent_w;
static Widget code_indent_w;
#else
static GUI::Scale *tab_width_w;
static GUI::Scale *source_indent_w;
static GUI::Scale *code_indent_w;
#endif
static MMDesc scales_menu[] = {
    GENTRYL("tabWidth", N_("Tab Width"), MMScale, 
	    BIND(sourceSetTabWidthCB, 0),
	    sigc::retype(sigc::ptr_fun(sourceSetTabWidthCB)),
	    0, &tab_width_w),
    GENTRYL("sourceIndent", N_("Source Indentation"), MMScale, 
	    BIND(sourceSetSourceIndentCB, 0),
	    sigc::retype(sigc::ptr_fun(sourceSetSourceIndentCB)),
	    0, &source_indent_w),
    GENTRYL("codeIndent", N_("Machine Code Indentation"), MMScale, 
	    BIND(sourceSetCodeIndentCB, 0),
	    sigc::retype(sigc::ptr_fun(sourceSetCodeIndentCB)),
	    0, &code_indent_w),
    MMEnd
};


#if defined(IF_XM)
static Widget line_numbers2_w;
static Widget refer_sources_w;
#else
static GUI::CheckButton *line_numbers2_w;
static GUI::Widget *refer_sources_w;
#endif
static MMDesc source_preferences_menu[] = 
{
    GENTRYL("showExecPos", N_("Show Position and Breakpoints"), MMRadioPanel,
	    MMNoCB, MDUMMY, glyph_menu, 0),
    GENTRYL("toolButtons", N_("Tool Buttons Location"), MMRadioPanel,
	    MMNoCB, MDUMMY, tool_buttons_menu, 0),
    GENTRYL("referSources", N_("Refer to Program Sources"), MMRadioPanel,
	    MMNoCB, MDUMMY, refer_menu, &refer_sources_w),
    GENTRYL("find", N_("Find"), MMButtonPanel,
	    MMNoCB, MDUMMY, find_preferences_menu, 0),
    GENTRYL("cache", N_("Cache"), MMButtonPanel,
	    MMNoCB, MDUMMY, cache_menu, 0),
    GENTRYL("lineNumbers", N_("Display Source Line Numbers"), MMToggle,
	    BIND(sourceToggleDisplayLineNumbersCB, 0),
	    sigc::retype(sigc::ptr_fun(sourceToggleDisplayLineNumbersCB)),
	    0, &line_numbers2_w),
    GENTRYL("scales", N_("scales"), MMPanel | MMUnmanagedLabel, 
	    MMNoCB, MDUMMY, scales_menu, 0),
    MMEnd
};


// Data preferences

#if defined(IF_XM)
static Widget graph_show_hints_w;
static Widget graph_show_annotations_w;
static Widget graph_show_dependent_titles_w;
#else
static GUI::CheckButton *graph_show_hints_w;
static GUI::CheckButton *graph_show_annotations_w;
static GUI::CheckButton *graph_show_dependent_titles_w;
#endif

static MMDesc show_menu[] = 
{
    GENTRYL("hints", N_("Edge Hints"), MMToggle,
	    BIND(graphToggleShowHintsCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleShowHintsCB)),
	    0, &graph_show_hints_w),
    GENTRYL("annotations", N_("Edge Annotations"), MMToggle,
	    BIND(graphToggleShowAnnotationsCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleShowAnnotationsCB)),
	    0, &graph_show_annotations_w),
    GENTRYL("dependentTitles", N_("Titles of Dependent Displays"), MMToggle,
	    BIND(graphToggleShowDependentTitlesCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleShowDependentTitlesCB)),
	    0, &graph_show_dependent_titles_w),
    MMEnd
};

#if defined(IF_XM)
static Widget graph_compact_layout_w;
static Widget graph_auto_layout_w;
#else
static GUI::CheckButton *graph_compact_layout_w;
static GUI::CheckButton *graph_auto_layout_w;
#endif

static MMDesc layout_menu[] =
{
    GENTRYL("compact", N_("Compact"), MMToggle,
	    BIND(graphToggleCompactLayoutCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleCompactLayoutCB)),
	    0, &graph_compact_layout_w),
    GENTRYL("auto", N_("Automatic"), MMToggle,
	    BIND(graphToggleAutoLayoutCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleAutoLayoutCB)),
	    0, &graph_auto_layout_w),
    MMEnd
};

#if defined(IF_XM)
static Widget graph_top_to_bottom_w;
static Widget graph_left_to_right_w;
#else
static GUI::RadioButton *graph_top_to_bottom_w;
static GUI::RadioButton *graph_left_to_right_w;
#endif

static MMDesc direction_menu[] =
{
    GENTRYL("topToBottom", N_("Top to bottom"), MMRadio,
	    BIND(graphSetDisplayPlacementCB, XmVERTICAL),
	    sigc::bind(sigc::retype(sigc::ptr_fun(graphSetDisplayPlacementCB)), GUI::ORIENTATION_VERTICAL),
	    0, &graph_top_to_bottom_w),
    GENTRYL("leftToRight", N_("Left to right"), MMRadio,
	    BIND(graphSetDisplayPlacementCB, XmHORIZONTAL),
	    sigc::bind(sigc::retype(sigc::ptr_fun(graphSetDisplayPlacementCB)), GUI::ORIENTATION_HORIZONTAL),
	    0, &graph_left_to_right_w),
    MMEnd
};

#if defined(IF_XM)
static Widget graph_cluster_displays_w;
#else
static GUI::CheckButton *graph_cluster_displays_w;
#endif

static MMDesc placement_menu[] =
{
    GENTRYL("direction", N_(""), MMRadioPanel | MMUnmanagedLabel,
	    MMNoCB, MDUMMY, direction_menu, 0),
    GENTRYL("clusterDisplays", N_("Clustered"), MMToggle,
	    BIND(graphToggleClusterDisplaysCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleClusterDisplaysCB)),
	    0, &graph_cluster_displays_w),
    MMEnd
};

#if defined(IF_XM)
static Widget graph_snap_to_grid_w;
#else
static GUI::CheckButton *graph_snap_to_grid_w;
#endif

static MMDesc themes_menu[] = 
{
    GENTRYL("snapToGrid", N_("Auto-Align Displays on Nearest Grid Point"), MMToggle,
	    BIND(graphToggleSnapToGridCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleSnapToGridCB)),
	    0, &graph_snap_to_grid_w),
    GENTRYL("themes", N_("Themes..."), MMPush,
	    BIND(dddPopupThemesCB, 0),
	    sigc::hide(sigc::ptr_fun(dddPopupThemesCB)),
	    0, 0),
    MMEnd
};

#if defined(IF_XM)
static Widget graph_grid_size_w;
#else
static GUI::Scale *graph_grid_size_w;
#endif

static MMDesc grid_menu[] =
{
    GENTRYL("gridSize", N_("Grid Size"), MMScale,
	    BIND(graphSetGridSizeCB, 0),
	    sigc::retype(sigc::ptr_fun(graphSetGridSizeCB)),
	    0, &graph_grid_size_w),
    MMEnd
};

#if defined(IF_XM)
static Widget graph_detect_aliases_w;
static Widget graph_align_2d_arrays_w;
static Widget graph_auto_close_w;
#else
static GUI::CheckButton *graph_detect_aliases_w;
static GUI::CheckButton *graph_align_2d_arrays_w;
static GUI::CheckButton *graph_auto_close_w;
#endif

static MMDesc data_preferences_menu[] = 
{
    GENTRYL("show", N_("Show"), MMPanel,
	    MMNoCB, MDUMMY, show_menu, 0),
    GENTRYL("placement", N_("Placement"), MMPanel,
	    MMNoCB, MDUMMY, placement_menu, 0),
    GENTRYL("layout", N_("Layout"), MMPanel,
	    MMNoCB, MDUMMY, layout_menu, 0),
    GENTRYL("detectAliases", N_("Detect Aliases"), MMToggle,
	    BIND(graphToggleDetectAliasesCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleDetectAliasesCB)),
	    0, &graph_detect_aliases_w),
    GENTRYL("align2dArrays", N_("Display Two-Dimensional Arrays as Tables"), MMToggle,
	    BIND(graphToggleAlign2dArraysCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleAlign2dArraysCB)),
	    0, &graph_align_2d_arrays_w),
    GENTRYL("autoClose", N_("Close Data Window when Deleting last Display"), MMToggle,
	    BIND(graphToggleAutoCloseCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleAutoCloseCB)),
	    0, &graph_auto_close_w),
    GENTRYL("themes", N_(""), MMPanel | MMUnmanagedLabel, 
	    MMNoCB, MDUMMY, themes_menu, 0),
    GENTRYL("grid", N_(""), MMPanel | MMUnmanagedLabel,
	    MMNoCB, MDUMMY, grid_menu, 0),
    MMEnd
};


// Startup preferences
#if defined(IF_XM)
static Widget set_separate_windows_w;
static Widget set_attached_windows_w;
#else
static GUI::RadioButton *set_separate_windows_w;
static GUI::RadioButton *set_attached_windows_w;
#endif
static MMDesc window_mode_menu [] = 
{
    GENTRYL("attached", N_("Stacked Windows"), MMRadio,
	    BIND(dddSetSeparateWindowsCB, 1),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetSeparateWindowsCB)), 1),
	    0, &set_attached_windows_w),
    GENTRYL("separate", N_("Separate Windows"), MMRadio,
	    BIND(dddSetSeparateWindowsCB, 0),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetSeparateWindowsCB)), 0),
	    0, &set_separate_windows_w),
    MMEnd
};

#if defined(IF_XM)
static Widget set_button_images_w;
static Widget set_button_captions_w;
static Widget set_flat_buttons_w;
static Widget set_color_buttons_w;
static Widget set_toolbars_at_bottom_w;
#else
static GUI::CheckButton *set_button_images_w;
static GUI::CheckButton *set_button_captions_w;
static GUI::CheckButton *set_flat_buttons_w;
static GUI::CheckButton *set_color_buttons_w;
static GUI::CheckButton *set_toolbars_at_bottom_w;
#endif
static MMDesc button_appearance_menu [] = 
{
    GENTRYL("images", N_("Images"), MMToggle,
	    BIND(dddToggleButtonImagesCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleButtonImagesCB)),
	    0, &set_button_images_w),
    GENTRYL("captions", N_("Captions"), MMToggle,
	    BIND(dddToggleButtonCaptionsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleButtonCaptionsCB)),
	    0, &set_button_captions_w),
    GENTRYL("flat", N_("Flat"), MMToggle,
	    BIND(dddToggleFlatButtonsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleFlatButtonsCB)),
	    0, &set_flat_buttons_w),
    GENTRYL("color", N_("Color"), MMToggle,
	    BIND(dddToggleColorButtonsCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleColorButtonsCB)),
	    0, &set_color_buttons_w),
    GENTRYL("bottom", N_("Bottom"), MMToggle,
	    BIND(dddToggleToolbarsAtBottomCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleToolbarsAtBottomCB)),
	    0, &set_toolbars_at_bottom_w),
    MMEnd
};

#if defined(IF_XM)
static Widget set_focus_pointer_w;
static Widget set_focus_explicit_w;
#else
static GUI::RadioButton *set_focus_pointer_w;
static GUI::RadioButton *set_focus_explicit_w;
#endif
static MMDesc keyboard_focus_menu [] = 
{
    GENTRYL("pointer", N_("Point to Type"), MMRadio,
	    BIND(dddSetKeyboardFocusPolicyCB, XmPOINTER),
	    sigc::bind(sigc::ptr_fun(dddSetKeyboardFocusPolicyCB), 1),
	    0, &set_focus_pointer_w),
    GENTRYL("explicit", N_("Click to Type"), MMRadio,
	    BIND(dddSetKeyboardFocusPolicyCB, XmEXPLICIT),
	    sigc::bind(sigc::ptr_fun(dddSetKeyboardFocusPolicyCB), 0),
	    0, &set_focus_explicit_w),
    MMEnd
};

#if defined(IF_XM)
static Widget set_scrolling_panner_w;
static Widget set_scrolling_scrollbars_w;
#else
static GUI::RadioButton *set_scrolling_panner_w;
static GUI::RadioButton *set_scrolling_scrollbars_w;
#endif
static MMDesc data_scrolling_menu [] = 
{
    GENTRYL("panner", N_("Panner"), MMRadio,
	    BIND(dddSetPannerCB, true),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetPannerCB)), true),
	    0, &set_scrolling_panner_w),
    GENTRYL("scrollbars", N_("Scrollbars"), MMRadio,
	    BIND(dddSetPannerCB, false),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetPannerCB)), false),
	    0, &set_scrolling_scrollbars_w),
    MMEnd
};

#if defined(IF_XM)
static Widget set_debugger_bash_w;
static Widget set_debugger_dbg_w;
static Widget set_debugger_dbx_w;
static Widget set_debugger_gdb_w;
static Widget set_debugger_jdb_w;
static Widget set_debugger_make_w;
static Widget set_debugger_perl_w;
static Widget set_debugger_pydb_w;
static Widget set_debugger_xdb_w;
#else
static GUI::RadioButton *set_debugger_bash_w;
static GUI::RadioButton *set_debugger_dbg_w;
static GUI::RadioButton *set_debugger_dbx_w;
static GUI::RadioButton *set_debugger_gdb_w;
static GUI::RadioButton *set_debugger_jdb_w;
static GUI::RadioButton *set_debugger_make_w;
static GUI::RadioButton *set_debugger_perl_w;
static GUI::RadioButton *set_debugger_pydb_w;
static GUI::RadioButton *set_debugger_xdb_w;
#endif
static MMDesc debugger_menu [] = 
{
    GENTRYL("bash", N_("Bash"), MMRadio,
	    BIND(dddSetDebuggerCB, BASH),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetDebuggerCB)), BASH),
	    0, &set_debugger_bash_w),
    GENTRYL("dbg", N_("DBG"), MMRadio,
	    BIND(dddSetDebuggerCB, DBG),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetDebuggerCB)), DBG),
	    0, &set_debugger_dbg_w),
    GENTRYL("dbx", N_("DBX"), MMRadio,
	    BIND(dddSetDebuggerCB, DBX),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetDebuggerCB)), DBX),
	    0, &set_debugger_dbx_w),
    GENTRYL("gdb", N_("GDB"), MMRadio,
	    BIND(dddSetDebuggerCB, GDB),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetDebuggerCB)), GDB),
	    0, &set_debugger_gdb_w),
    GENTRYL("jdb", N_("JDB"), MMRadio,
	    BIND(dddSetDebuggerCB, JDB),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetDebuggerCB)), JDB),
	    0, &set_debugger_jdb_w),
    GENTRYL("perl", N_("Perl"), MMRadio,
	    BIND(dddSetDebuggerCB, PERL),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetDebuggerCB)), PERL),
	    0, &set_debugger_perl_w),
    GENTRYL("pydb", N_("PYDB"), MMRadio,
	    BIND(dddSetDebuggerCB, PYDB),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetDebuggerCB)), PYDB),
	    0, &set_debugger_pydb_w),
    GENTRYL("xdb", N_("XDB"), MMRadio,
	    BIND(dddSetDebuggerCB, XDB),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetDebuggerCB)), XDB),
	    0, &set_debugger_xdb_w),
    MMEnd
};

#if defined(IF_XM)
static Widget auto_debugger_w;
#else
static GUI::CheckButton *auto_debugger_w;
#endif
static MMDesc auto_debugger_menu [] =
{
    GENTRYL("automatic", N_("Determine Automatically from Arguments"), MMToggle,
	    BIND(dddToggleAutoDebuggerCB, 0),
	    sigc::retype(sigc::ptr_fun(dddToggleAutoDebuggerCB)),
	    0, &auto_debugger_w),
    MMEnd
};

#if defined(IF_XM)
static Widget startup_tips_w;
static Widget splash_screen_w;
#else
static GUI::CheckButton *startup_tips_w;
static GUI::CheckButton *splash_screen_w;
#endif

static MMDesc startup_menu [] =
{
    GENTRYL("splashScreen", N_("DDD Splash Screen"), MMToggle,
	    BIND(SetSplashScreenCB, 0),
	    sigc::retype(sigc::ptr_fun(SetSplashScreenCB)),
	    0, &splash_screen_w),
    GENTRYL("startupTips", N_("Tip of the Day"), MMToggle,
	    BIND(SetStartupTipsCB, 0),
	    sigc::retype(sigc::ptr_fun(SetStartupTipsCB)),
	    0, &startup_tips_w),
    MMEnd
};

#if defined(IF_XM)
static Widget cut_copy_paste_kde_w;
static Widget cut_copy_paste_motif_w;
#else
static GUI::RadioButton *cut_copy_paste_kde_w;
static GUI::RadioButton *cut_copy_paste_motif_w;
#endif

static MMDesc cut_copy_paste_menu [] =
{
    GENTRYL("kde", N_("Copy"), MMRadio,
	    BIND(dddSetCutCopyPasteBindingsCB, KDEBindings), 
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetCutCopyPasteBindingsCB)), KDEBindings ), 
	    0, &cut_copy_paste_kde_w),
    GENTRYL("motif", N_("Interrupt"), MMRadio,
	    BIND(dddSetCutCopyPasteBindingsCB, MotifBindings),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetCutCopyPasteBindingsCB)), MotifBindings ),
	    0, &cut_copy_paste_motif_w),
    MMEnd
};

#if defined(IF_XM)
static Widget select_all_kde_w;
static Widget select_all_motif_w;
#else
static GUI::RadioButton *select_all_kde_w;
static GUI::RadioButton *select_all_motif_w;
#endif

static MMDesc select_all_menu [] =
{
    GENTRYL("kde", N_("Select All"), MMRadio,
	    BIND(dddSetSelectAllBindingsCB, KDEBindings),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetSelectAllBindingsCB)), KDEBindings), 
	    0, &select_all_kde_w),
    GENTRYL("motif", N_("Beginning of Line"), MMRadio,
	    BIND(dddSetSelectAllBindingsCB, MotifBindings),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetSelectAllBindingsCB)), MotifBindings),
	    0, &select_all_motif_w),
    MMEnd
};

static MMDesc startup_preferences_menu [] =
{
    GENTRYL("windows", N_("Window Layout"), MMRadioPanel,
	    MMNoCB, MDUMMY, window_mode_menu, 0),
    GENTRYL("cutCopyPaste", N_("Ctrl+C is"), MMRadioPanel,
	    MMNoCB, MDUMMY, cut_copy_paste_menu, 0),
    GENTRYL("selectAll", N_("Ctrl+A is"), MMRadioPanel,
	    MMNoCB, MDUMMY, select_all_menu, 0),
    GENTRYL("buttons", N_("Tool Bar Appearance"), MMButtonPanel,
	    MMNoCB, MDUMMY, button_appearance_menu, 0),
    GENTRYL("keyboardFocus", N_("Keyboard Focus"), MMRadioPanel,
	    MMNoCB, MDUMMY, keyboard_focus_menu, 0),
    GENTRYL("dataScrolling", N_("Data Scrolling"), MMRadioPanel,
	    MMNoCB, MDUMMY, data_scrolling_menu, 0),
    GENTRYL("autoDebugger", N_("Debugger Type"), MMButtonPanel,
	    MMNoCB, MDUMMY, auto_debugger_menu, 0),
    GENTRYL("debugger", N_("Debugger"), MMRadioPanel,
	    MMNoCB, MDUMMY, debugger_menu, 0),
    GENTRYL("startupWindows", N_("Startup Windows"), MMButtonPanel,
	    MMNoCB, MDUMMY, startup_menu, 0),
    MMEnd
};


#if defined(IF_XM)
static Widget font_names[5];
static Widget font_sizes[5];
#else
static GUI::Entry *font_names[5];
static GUI::Entry *font_sizes[5];
#endif

#if defined(IF_XM)
#define FONT_MENU(font)						\
{								\
    GENTRYL("name", N_(""), MMTextField | MMUnmanagedLabel,		\
	    BIND(SetFontNameCB, font),				\
	    sigc::bind(sigc::ptr_fun(SetFontNameCB), font ),	\
	    0, &font_names[int(font)]),				\
    GENTRYL("size", N_("Size"), MMTextField,			\
	    BIND(SetFontSizeCB, font),				\
	    sigc::bind(sigc::ptr_fun(SetFontSizeCB), font ),	\
	    0, &font_sizes[int(font)]),				\
    GENTRYL("browse", N_("Browse..."), MMPush,			\
	    BIND(BrowseFontCB, font),				\
	    sigc::bind(sigc::ptr_fun(BrowseFontCB), font),	\
	    0, 0),						\
    MMEnd							\
}

static MMDesc default_font_menu        [] = FONT_MENU(DefaultDDDFont);
static MMDesc variable_width_font_menu [] = FONT_MENU(VariableWidthDDDFont);
static MMDesc fixed_width_font_menu    [] = FONT_MENU(FixedWidthDDDFont);
static MMDesc data_font_menu           [] = FONT_MENU(DataDDDFont);
#endif

#if defined(IF_XM)
static MMDesc font_preferences_menu [] =
{
    GENTRYL("default", N_("Default Font"), MMPanel,
	    MMNoCB, MDUMMY, default_font_menu, 0),
    GENTRYL("variableWidth", N_("Variable Width"), MMPanel,
	    MMNoCB, MDUMMY, variable_width_font_menu, 0),
    GENTRYL("fixedWidth", N_("Fixed Width"), MMPanel,
	    MMNoCB, MDUMMY, fixed_width_font_menu, 0),
    GENTRYL("data", N_("Data"), MMPanel,
	    MMNoCB, MDUMMY, data_font_menu, 0),
    MMEnd
};
#endif


#if defined(IF_XM)
static Widget extern_plot_window_w;
static Widget builtin_plot_window_w;
#else
static GUI::RadioButton *extern_plot_window_w;
static GUI::RadioButton *builtin_plot_window_w;
#endif

static MMDesc plot_window_menu [] =
{
    GENTRYL("extern", N_("External"), MMRadio,
	    BIND(dddSetBuiltinPlotWindowCB, false),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetBuiltinPlotWindowCB)), false),
	    0, &extern_plot_window_w),
    GENTRYL("builtin", N_("Builtin"), MMRadio,
	    BIND(dddSetBuiltinPlotWindowCB, true),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetBuiltinPlotWindowCB)), true),
	    0, &builtin_plot_window_w),
    MMEnd
};

#if defined(IF_XM)
static Widget edit_command_w;
static Widget get_core_command_w;
static Widget ps_command_w;
static Widget term_command_w;
static Widget uncompress_command_w;
static Widget www_command_w;
static Widget plot_command_w;
#else
static GUI::Entry *edit_command_w;
static GUI::Entry *get_core_command_w;
static GUI::Entry *ps_command_w;
static GUI::Entry *term_command_w;
static GUI::Entry *uncompress_command_w;
static GUI::Entry *www_command_w;
static GUI::Entry *plot_command_w;
#endif

static MMDesc helpers_preferences_menu [] =
{
    GENTRYL("edit", N_("Edit Sources"), MMTextField,
	    BIND(dddSetEditCommandCB, 0), 
	    sigc::retype(sigc::ptr_fun(dddSetEditCommandCB)), 
	    0, &edit_command_w),
    GENTRYL("get_core", N_("Get Core File"), MMTextField,
	    BIND(dddSetGetCoreCommandCB, 0), 
	    sigc::retype(sigc::ptr_fun(dddSetGetCoreCommandCB)), 
	    0, &get_core_command_w),
    GENTRYL("ps", N_("List Processes"), MMTextField,
	    BIND(dddSetPSCommandCB, 0),
	    sigc::retype(sigc::ptr_fun(dddSetPSCommandCB)),
	    0, &ps_command_w),
    GENTRYL("term", N_("Execution Window"), MMTextField,
	    BIND(dddSetTermCommandCB, 0),
	    sigc::retype(sigc::ptr_fun(dddSetTermCommandCB)),
	    0, &term_command_w),
    GENTRYL("uncompress", N_("Uncompress"), MMTextField,
	    BIND(dddSetUncompressCommandCB, 0),
	    sigc::retype(sigc::ptr_fun(dddSetUncompressCommandCB)),
	    0, &uncompress_command_w),
    GENTRYL("www", N_("Web Browser"), MMTextField,
	    BIND(dddSetWWWCommandCB, 0),
	    sigc::retype(sigc::ptr_fun(dddSetWWWCommandCB)),
	    0, &www_command_w),
    GENTRYL("plot", N_("Plot"), MMTextField,
	    BIND(dddSetPlotCommandCB, 0), 
	    sigc::retype(sigc::ptr_fun(dddSetPlotCommandCB)), 
	    0, &plot_command_w),
    GENTRYL("plot_window", N_("Plot Window"), MMRadioPanel,
	    MMNoCB, MDUMMY, plot_window_menu, 0),
    MMEnd
};


// Data
#if defined(IF_XM)
static Widget print_w            = 0;
static Widget display_w          = 0;
static Widget examine_w          = 0;
static Widget locals_w           = 0;
static Widget args_w             = 0;
static Widget detect_aliases_w   = 0;
static Widget infos_w            = 0;
static Widget align_w            = 0;
static Widget edit_watchpoints_w = 0;
#else
static GUI::Widget *print_w            = 0;
static GUI::Widget *display_w          = 0;
static GUI::Widget *examine_w                 = 0;
static GUI::CheckMenuItem *locals_w           = 0;
static GUI::CheckMenuItem *args_w             = 0;
static GUI::CheckMenuItem *detect_aliases_w   = 0;
static GUI::Widget *infos_w            = 0;
static GUI::Widget *align_w            = 0;
static GUI::Widget *edit_watchpoints_w = 0;
#endif

#if defined(IF_XM)
DECL_WR(dddPopupInfosCB);
#else
DECL_WR(WR_dddPopupInfosCB, sigc::ptr_fun(dddPopupInfosCB));
#endif
static MMDesc data_menu[] = 
{
    GENTRYL("displays", N_("Displays..."), MMPush,
	    BIND(DataDisp::EditDisplaysCB, 0),
	    sigc::hide(sigc::ptr_fun(DataDisp::EditDisplaysCB)),
	    0, 0),
    GENTRYL("themes", N_("Themes..."), MMPush,
	    BIND(dddPopupThemesCB, 0), 
	    sigc::hide(sigc::ptr_fun(dddPopupThemesCB)), 
	    0, 0),
    GENTRYL("watchpoints", N_("Watchpoints..."), MMPush,
	    BIND(SourceView::EditBreakpointsCB, 0), 
	    sigc::ptr_fun(SourceView::EditBreakpointsCB), 
	    0, &edit_watchpoints_w),
    GENTRYL("examine", N_("Memory..."), MMPush,
	    BIND(gdbExamineCB, 0),
	    sigc::ptr_fun(gdbExamineCB),
	    0, &examine_w),
    MMSep,
    GENTRYL("print", N_("Print ()"), MMPush, 
	    BIND(gdbPrintCB, False),
	    sigc::bind(sigc::ptr_fun(gdbPrintCB), false),
	    0, &print_w),
    GENTRYL("display", N_("Display ()"), MMPush,
	    BIND(gdbDisplayCB, 0),
	    sigc::ptr_fun(gdbDisplayCB),
	    0, &display_w),
    MMSep,
    GENTRYL("detectAliases", N_("Detect Aliases"), MMCheckItem,
	    BIND(graphToggleDetectAliasesCB, 0),
	    sigc::retype(sigc::ptr_fun(graphToggleDetectAliasesCB)),
	    0, &detect_aliases_w),
    MMSep,
    GENTRYL("info locals", N_("Display Local Variables"), MMCheckItem,
	    BIND(graphToggleLocalsCB, 0), 
	    sigc::retype(sigc::ptr_fun(graphToggleLocalsCB)), 
	    0, &locals_w),
    GENTRYL("info args", N_("Display Arguments"), MMCheckItem,
	    BIND(graphToggleArgsCB, 0), 
	    sigc::retype(sigc::ptr_fun(graphToggleArgsCB)), 
	    0, &args_w),
    GENTRYL("infos", N_("Status Displays..."), MMPush,
	    BIND(WhenReady, &WR_dddPopupInfosCB), 
	    sigc::bind(sigc::ptr_fun(WhenReady), &WR_dddPopupInfosCB), 
	    0, &infos_w),
    MMSep,
    GENTRYL("align", N_("Align on Grid"), MMPush,
	    BIND(graphAlignCB, 0),
	    sigc::hide(sigc::ptr_fun(graphAlignCB)),
	    0, &align_w),
    GENTRYL("rotate", N_("Rotate Graph"), MMPush,
	    BIND(graphRotateCB, 0),
	    sigc::hide(sigc::ptr_fun(graphRotateCB)),
	    0, 0),
    GENTRYL("layout", N_("Layout Graph"), MMPush,
	    BIND(graphLayoutCB, 0),
	    sigc::hide(sigc::ptr_fun(graphLayoutCB)),
	    0, 0),
    MMSep,
    GENTRYL("refresh", N_("Refresh"), MMPush,
	    BIND(DataDisp::refreshCB, 0),
	    sigc::ptr_fun(DataDisp::refreshCB),
	    0, 0),
    MMEnd
};


// Maintenance
#if defined(IF_XM)
static Widget crash_debug_w     = 0;
static Widget crash_dump_core_w = 0;
static Widget crash_nothing_w   = 0;
#else
static GUI::RadioButton *crash_debug_w     = 0;
static GUI::RadioButton *crash_dump_core_w = 0;
static GUI::RadioButton *crash_nothing_w   = 0;
#endif

static MMDesc crash_menu[] = 
{
    GENTRYL("debug", N_("Debug DDD"), MMRadioItem,
	    BIND(dddSetCrashCB, 2),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetCrashCB)), 2),
	    0, &crash_debug_w),
    GENTRYL("dumpCore", N_("Dump Core Now"), MMRadioItem,
	    BIND(dddSetCrashCB, 1),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetCrashCB)), 1),
	    0, &crash_dump_core_w),
    GENTRYL("nothing", N_("Do Nothing"), MMRadioItem,
	    BIND(dddSetCrashCB, 0),
	    sigc::bind(sigc::retype(sigc::ptr_fun(dddSetCrashCB)), 0),
	    0, &crash_nothing_w),
    MMEnd
};

#if defined(IF_XM)
static Widget debug_ddd_w       = 0;
static Widget dump_core_w       = 0;
static Widget valgrindLeakCheck_w = 0;
#else
static GUI::Widget *debug_ddd_w       = 0;
static GUI::Widget *dump_core_w       = 0;
static GUI::Widget *valgrindLeakCheck_w = 0;
#endif

static MMDesc maintenance_menu[] = 
{
    GENTRYL("debug", N_("Debug DDD..."), MMPush,
	    BIND(DDDDebugCB, 0),
	    sigc::hide(sigc::bind(sigc::ptr_fun(DDDDebugCB), 0)),
	    0, &debug_ddd_w),
    GENTRYL("dumpCore", N_("Dump Core Now"), MMPush,
	    BIND(DDDDumpCoreCB, 0),
	    sigc::hide(sigc::ptr_fun(DDDDumpCoreCB)),
	    0, &dump_core_w),
    //GENTRYL("tictactoe", N_("Tic Tac Toe..."), MMPush,
    //	    BIND(TicTacToeCB, 0),
    //	    sigc::hide(sigc::ptr_fun(TicTacToeCB)),
    //	    0, 0),
    MMSep,
    GENTRYL("crash", N_("When DDD Crashes"), MMRadioMenu,
	    MMNoCB, MDUMMY, crash_menu, 0),
    MMSep,
    GENTRYL("valgrindLeak", N_("Do Valgrind Leak Check"), MMPush | MMUnmanaged,
	    BIND(dddValgrindLeakCheckCB, 0),
	    sigc::hide(sigc::ptr_fun(dddValgrindLeakCheckCB)),
	    0, &valgrindLeakCheck_w),
    MMSep,
    GENTRYL("remove", N_("Remove Menu"), MMPush,
	    BIND(dddClearMaintenanceCB, 0),
	    sigc::hide(sigc::ptr_fun(dddClearMaintenanceCB)),
	    0, 0),
    MMEnd
};

#if defined(IF_XM)
static Widget maintenance_w = 0;
#else
static GUI::Widget *maintenance_w = 0;
#endif

// Menu Bar for DDD command window
static MMDesc command_menubar[] = 
{
    GENTRYL("file", N_("File"), MMMenu,
	    BIND(gdbUpdateFileCB, command_file_menu), 
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbUpdateFileCB), command_file_menu)), 
	    command_file_menu, 0),
    GENTRYL("edit", N_("Edit"), MMMenu,
	    BIND(gdbUpdateEditCB, GDBWindow),
	    sigc::bind(sigc::ptr_fun(gdbUpdateEditCB), GDBWindow),
	    command_edit_menu, 0),
    GENTRYL("view", N_("View"), MMMenu,
	    BIND(gdbUpdateViewCB, command_view_menu), 
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbUpdateViewCB), command_view_menu)), 
	    command_view_menu, 0),
    GENTRYL("program", N_("Program"), MMMenu,
	    MMNoCB, MDUMMY, command_program_menu, 0),
    GENTRYL("commands", N_("Commands"), MMMenu,
	    MMNoCB, MDUMMY, command_menu, 0),
    GENTRYL("maintenance", N_("Maintenance"), MMMenu | MMUnmanaged,
	    MMNoCB, MDUMMY, maintenance_menu, &maintenance_w),
#if defined(IF_XM)
    GENTRYL("help", N_("Help"), MMMenu | MMHelp,
	    MMNoCB, MDUMMY, simple_help_menu, 0),
#endif
    MMEnd
};

// Menu Bar for DDD source view
static MMDesc source_menubar[] = 
{
    GENTRYL("file", N_("File"), MMMenu,
	    BIND(gdbUpdateFileCB, source_file_menu), 
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbUpdateFileCB), source_file_menu)), 
	    source_file_menu, 0),
    GENTRYL("edit", N_("Edit"), MMMenu,
	    BIND(gdbUpdateEditCB, SourceWindow),
	    sigc::bind(sigc::ptr_fun(gdbUpdateEditCB), SourceWindow),
	    source_edit_menu, 0),
    GENTRYL("view", N_("View"), MMMenu,
	    BIND(gdbUpdateViewCB, source_view_menu),
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbUpdateViewCB), source_view_menu)),
	    source_view_menu, 0),
    GENTRYL("program", N_("Program"), MMMenu,
	    MMNoCB, MDUMMY, source_program_menu, 0),
    GENTRYL("stack", N_("Status"), MMMenu,
	    MMNoCB, MDUMMY, stack_menu, 0),
    GENTRYL("source", N_("Source"), MMMenu,
	    MMNoCB, MDUMMY, source_menu, 0),
#if defined(IF_XM)
    GENTRYL("help", N_("Help"), MMMenu | MMHelp,
	    MMNoCB, MDUMMY, simple_help_menu, 0),
#endif
    MMEnd
};

// Menu Bar for DDD data window
static MMDesc data_menubar[] = 
{
    GENTRYL("file", N_("File"), MMMenu,
	    BIND(gdbUpdateFileCB, data_file_menu), 
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbUpdateFileCB), data_file_menu)), 
	    data_file_menu, 0),
    GENTRYL("edit", N_("Edit"), MMMenu,
	    BIND(gdbUpdateEditCB, DataWindow),
	    sigc::bind(sigc::ptr_fun(gdbUpdateEditCB), DataWindow),
	    data_edit_menu, 0),
    GENTRYL("view", N_("View"), MMMenu,
	    BIND(gdbUpdateViewCB, data_view_menu), 
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbUpdateViewCB), data_view_menu)), 
	    data_view_menu, 0),
    GENTRYL("program", N_("Program"), MMMenu,
	    MMNoCB, MDUMMY, data_program_menu, 0),
    GENTRYL("data", N_("Data"), MMMenu,
	    MMNoCB, MDUMMY, data_menu, 0),
#if defined(IF_XM)
    GENTRYL("help", N_("Help"), MMMenu | MMHelp,
	    MMNoCB, MDUMMY, simple_help_menu, 0),
#endif
    MMEnd
};

// Menu Bar for common DDD data/command window
static MMDesc common_menubar[] = 
{
    GENTRYL("file", N_("File"), MMMenu,
	    BIND(gdbUpdateFileCB, command_file_menu), 
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbUpdateFileCB), command_file_menu)), 
	    command_file_menu, 0),
    GENTRYL("edit", N_("Edit"), MMMenu,
	    BIND(gdbUpdateEditCB, CommonWindow),
	    sigc::bind(sigc::ptr_fun(gdbUpdateEditCB), CommonWindow),
	    command_edit_menu, 0),
    GENTRYL("views", N_("Views"), MMMenu,
	    BIND(gdbUpdateViewsCB, views_menu), 
	    sigc::hide(sigc::bind(sigc::ptr_fun(gdbUpdateViewsCB), views_menu)), 
	    views_menu, 0),
    GENTRYL("program", N_("Program"), MMMenu,
	    MMNoCB, MDUMMY, command_program_menu, 0),
    GENTRYL("commands", N_("Commands"), MMMenu,
	    MMNoCB, MDUMMY, command_menu, 0),
    GENTRYL("stack", N_("Status"), MMMenu,
	    MMNoCB, MDUMMY, stack_menu, 0),
    GENTRYL("source", N_("Source"), MMMenu,
	    MMNoCB, MDUMMY, source_menu, 0),
    GENTRYL("data", N_("Data"), MMMenu,
	    MMNoCB, MDUMMY, data_menu, 0),
    GENTRYL("maintenance", N_("Maintenance"), MMMenu | MMUnmanaged,
	    MMNoCB, MDUMMY, maintenance_menu, &maintenance_w),
    GENTRYL("help", N_("Help"), MMMenu | MMHelp,
	    MMNoCB, MDUMMY, simple_help_menu, 0),
    MMEnd
};

struct PrintItems {
    enum ArgCmd { PrintRef, Dump, Whatis, Examine };
};

#if defined(IF_XM)
static Widget print_ref_w     = 0;
static Widget print_dump_w    = 0;
static Widget print_whatis_w  = 0;
static Widget print_examine_w = 0;
#else
static GUI::Widget *print_ref_w     = 0;
static GUI::Widget *print_dump_w    = 0;
static GUI::Widget *print_whatis_w  = 0;
static GUI::Widget *print_examine_w = 0;
#endif

static MMDesc print_menu[] =
{
    GENTRYL("printRef", N_("Print *()"), MMPush,
	    BIND(gdbPrintRefCB, false),
	    sigc::bind(sigc::ptr_fun(gdbPrintRefCB), false),
	    0, &print_ref_w),
    GENTRYL("dump", N_("Dump ()"), MMPush, 
	    BIND(gdbPrintCB, true),
	    sigc::bind(sigc::ptr_fun(gdbPrintCB), true),
	    0, &print_dump_w),
    GENTRYL("whatis", N_("Whatis ()"), MMPush, 
	    BIND(gdbWhatisCB, 0),
	    sigc::ptr_fun(gdbWhatisCB),
	    0, &print_whatis_w),
    GENTRYL("examine", N_("Examine ()..."), MMPush, 
	    BIND(gdbExamineCB, 0),
	    sigc::ptr_fun(gdbExamineCB),
	    0, &print_examine_w),
    MMEnd
};

struct DispItems {
    enum ArgCmd { DispRef };
};

#if defined(IF_XM)
static Widget disp_ref_w     = 0;
#else
static GUI::Widget *disp_ref_w     = 0;
#endif

static MMDesc display_menu[] =
{
    GENTRYL("dispRef", N_("Display *()"), MMPush,
	    BIND(gdbDispRefCB, 0),
	    sigc::ptr_fun(gdbDispRefCB),
	    0, &disp_ref_w),
    MMEnd
};

struct WatchItems {
    enum ArgCmd { Properties, Enable, Sep, CWatch, RWatch, AWatch };
};

static MMDesc watch_menu[] =
{
    GENTRYL("watchProperties", N_("Watchpoint Properties..."), MMPush, 
	    BIND(gdbEditWatchpointPropertiesCB, 0),
	    sigc::hide(sigc::ptr_fun(gdbEditWatchpointPropertiesCB)),
	    0, 0),
    GENTRYL("enableWatch", N_("Enable Watchpoint at ()"), MMPush, 
	    BIND(gdbToggleEnableWatchpointCB, 0),
	    sigc::ptr_fun(gdbToggleEnableWatchpointCB),
	    0, 0),
    MMSep,
    GENTRYL("cwatch", N_("Set Watchpoint on ()"), MMPush, 
	    BIND(gdbWatchCB, WATCH_CHANGE),
	    sigc::bind(sigc::ptr_fun(gdbWatchCB), WATCH_CHANGE),
	    0, 0),
    GENTRYL("rwatch", N_("Set Read Watchpoint on ()"), MMPush, 
	    BIND(gdbWatchCB, WATCH_READ),
	    sigc::bind(sigc::ptr_fun(gdbWatchCB), WATCH_READ),
	    0, 0),
    GENTRYL("awatch", N_("Set Access Watchpoint on ()"), MMPush, 
	    BIND(gdbWatchCB, WATCH_ACCESS),
	    sigc::bind(sigc::ptr_fun(gdbWatchCB), WATCH_ACCESS),
	    0, 0),

    // It would be nice to have an `unwatch' command here, for
    // recording commands.  Unfortunately, GDB needs a watchpoint
    // number for deleting watchpoints.
#if 0	
    MMSep,
    GENTRYL("unwatch", N_("Unwatch ()"), MMPush,
	    BIND(gdbUnwatchCB, 0),
	    sigc::ptr_fun(gdbUnwatchCB),
	    0, 0),
#endif
    MMEnd
};

struct BreakItems {
    enum ArgCmd { TempBreak, RegexBreak, Properties, Enable, Sep1,
		  ContUntil, SetPC, Sep2, ClearAt2 };
};

static MMDesc break_menu[] = 
{
    GENTRYL("tempBreakAt", N_("Set Temporary Breakpoint at ()"), MMPush,
	    BIND(gdbTempBreakAtCB, 0),
	    sigc::ptr_fun(gdbTempBreakAtCB),
	    0, 0),
    GENTRYL("regexBreakAt", N_("Set Breakpoints at Regexp ()"), MMPush,
	    BIND(gdbRegexBreakAtCB, 0),
	    sigc::ptr_fun(gdbRegexBreakAtCB),
	    0, 0),
    GENTRYL("breakProperties", N_("Breakpoint Properties..."), MMPush, 
	    BIND(gdbEditBreakpointPropertiesCB, 0),
	    sigc::hide(sigc::ptr_fun(gdbEditBreakpointPropertiesCB)),
	    0, 0),
    GENTRYL("enableBreak", N_("Enable Breakpoint at ()"), MMPush, 
	    BIND(gdbToggleEnableBreakpointCB, 0),
	    sigc::ptr_fun(gdbToggleEnableBreakpointCB),
	    0, 0),
    MMSep,
    GENTRYL("contUntil", N_("Continue Until ()"), MMPush,
	    BIND(gdbContUntilCB, 0),
	    sigc::ptr_fun(gdbContUntilCB),
	    0, 0),
    GENTRYL("setPC", N_("Set Execution Position to ()"), MMPush,
	    BIND(gdbSetPCCB, 0),
	    sigc::ptr_fun(gdbSetPCCB),
	    0, 0),
    MMSep,
    GENTRYL("clearAt2", N_("Clear Breakpoint at ()"), MMPush,
	    BIND(gdbClearAtCB, 0),
	    sigc::ptr_fun(gdbClearAtCB),
	    0, 0),
    MMEnd
};

struct FindItems {
    enum ArgCmd { FindForward, FindBackward };
};

static MMDesc find_menu[] = 
{
    GENTRYL("findForward", N_("@charset Find@small>> @charset()@rm"), MMPush, 
	    BIND(gdbFindCB, SourceView::forward),
	    sigc::bind(sigc::ptr_fun(gdbFindCB), SourceView::forward),
	    0, 0),
    GENTRYL("findBackward", N_("@charset Find@small<< @charset()@rm"), MMPush, 
	    BIND(gdbFindCB, SourceView::backward),
	    sigc::bind(sigc::ptr_fun(gdbFindCB), SourceView::backward),
	    0, 0),
    MMEnd
};

struct ArgItems {
    enum ArgCmd { Lookup, Find, Break, Watch, Print, Display };
};

static MMDesc arg_cmd_area[] = 
{
    GENTRYI("lookup", LOOKUP_ICON, MMPush,
	    BIND(gdbLookupCB, 0),
	    sigc::hide(sigc::ptr_fun(gdbLookupCB)),
	    0, 0),
    GENTRYI("find", FIND_FORWARD_ICON, MMPush | MMInsensitive, 
	    BIND(gdbFindAgainCB, 0),
	    sigc::ptr_fun(gdbFindAgainCB),
	    find_menu, 0),
    GENTRYI("breakAt", BREAK_AT_ICON, MMPush,
	    BIND(gdbToggleBreakCB, 0),
	    sigc::ptr_fun(gdbToggleBreakCB),
	    break_menu, 0),
    GENTRYI("watch", WATCH_ICON, MMPush, 
	    BIND(gdbToggleWatchCB, WATCH_CHANGE),
	    sigc::bind(sigc::ptr_fun(gdbToggleWatchCB), WATCH_CHANGE),
	    watch_menu, 0),
    GENTRYI("print", PRINT_ICON, MMPush, 
	    BIND(gdbPrintCB, false),
	    sigc::bind(sigc::ptr_fun(gdbPrintCB), false),
	    print_menu, 0),
    GENTRYI("display", DISPLAY_ICON, MMPush,
	    BIND(gdbDisplayCB, 0),
	    sigc::ptr_fun(gdbDisplayCB),
	    display_menu, 0),
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
#if defined(IF_XM)
static Widget arg_cmd_w;
#else
static GUI::Container *arg_cmd_w;
#endif

#if defined(IF_XM)
// GDB input/output widget
Widget gdb_w;

// GDB status line
Widget status_w;

// GDB activity led
static Widget led_w;
#else
// GDB input/output widget
GUI::ScrolledText *gdb_w;

// GDB status line
GUI::Button *status_w;

// GDB activity led
// static GUI::CheckButton *led_w;
static GUI::Image *led_w;
static bool led_w_active = true;
#endif

#if defined(IF_XM)
// Last output position
XmTextPosition promptPosition;

// Last message position
XmTextPosition messagePosition;
#else
// Last output position
long promptPosition;

// Last message position
long messagePosition;
#endif

// Buttons
#if defined(IF_XM)
static Widget console_buttons_w;
static Widget source_buttons_w;
static Widget data_buttons_w;
static Widget command_toolbar_w;
#else
static GUI::Container *console_buttons_w;
static GUI::Container *source_buttons_w;
static GUI::Container *data_buttons_w;
static GUI::Container *command_toolbar_w;
#endif

// Strings to be ignored in GDB output
string gdb_out_ignore = "";

// Command management
bool private_gdb_output;   // true if output is running
bool private_gdb_input;    // true if input is running

// true, if initial gdb prompt appeared
bool gdb_initialized;

#if defined(IF_XM)
// The Xt Warning handler
static XtErrorHandler ddd_original_xt_warning_handler;
#endif

// Initial delays
static StatusMsg *init_delay = 0;
static Delay *setup_delay = 0;

#if defined(IF_XM)
// Events to note for window visibility
const int STRUCTURE_MASK = StructureNotifyMask | VisibilityChangeMask;
#endif

// Message handling
static MString version_warnings;


//-----------------------------------------------------------------------------
// DDD main program
//-----------------------------------------------------------------------------

#if !defined(IF_XM)
typedef sigc::slot<void, GUI::Widget *> slot_gui_w;
#endif
typedef enum {
  DDD_EXIT_FAILURE, DDD_EXIT_SUCCESS, DDD_CONTINUE
} ddd_exit_t;

#if defined(IF_XM)
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

    // Initialize X toolkit
    Arg args[10];
    int arg = 0;

    // Install X error handlers
    ddd_install_x_fatal();
    ddd_install_x_error();

    std::ostringstream messages;

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
		    DDD_CLASS_NAME, &argc, (char**)argv);

    const _XtString session_id = 0;
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
		    session_value.addr = CONST_CAST(char*,session_id);
		    session_value.size = strlen(session_id) + 1;
		    XrmPutResource(&dddinit, Nsession.chars(), XtRString, 
				   &session_value);
		}
	    }
	}
    }

    if (!restart_session().empty())
    {
	// A session is given in $DDD_SESSION: override everything.
	XrmDatabase session_db = 
	    GetFileDatabase(session_state_file(restart_session()));
	if (session_db != 0)
	    XrmMergeDatabases(session_db, &dddinit);
    }
    else if (session_id != 0)
    {
	// Merge in session resources; these override `~/.ddd/init' as
	// well as the command-line options.
	XrmDatabase session_db = 
	    GetFileDatabase(session_state_file(session_id));
	if (session_db != 0)
	    XrmMergeDatabases(session_db, &dddinit);
    }

#if HAVE_ATHENA
    // Initialize Xaw widget set, registering the Xaw Converters.
    // This is done before installing our own converters.
    XawInitializeWidgetSet();
#endif

    // Register own converters.  This must be done here to install the
    // String -> Cardinal converter.
    registerOwnConverters();

    // Handle `--version', `--help', etc.  We do this here, since we
    // might want to use them even without X access.
    setup_show(dddinit, basename(argv[0]), gdb_name.chars());

    // From this point on, we'll be running under X.

    // Open X connection and create top-level application shell
    XtAppContext app_context;
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

    XtAddCallback(toplevel, XtNsaveCallback, SaveSmSessionCB, XtPointer(0));
    XtAddCallback(toplevel, XtNdieCallback, ShutdownSmSessionCB, XtPointer(0));
#else
    // Note: the cast on ddd_fallback_resources is safe.
    Widget toplevel =
	XtAppInitialize(&app_context, DDD_CLASS_NAME,
			XrmOptionDescList(0), 0,
			&argc, argv, (String *)ddd_fallback_resources,
			args, arg);
#endif
    ddd_install_xt_error(app_context);

    // Check Motif version.  We can do this only now after the first
    // Motif widget has been created.
    setup_motif_version_warnings();

    // Setup top-level actions; this must be done before reading
    // application defaults.
    XtAppAddActions(app_context, actions, XtNumber(actions));

    // Register string -> OnOff converter; this must be done before
    // reading application defaults.
    XtSetTypeConverter(XmRString, XtROnOff, CvtStringToOnOff,
		       XtConvertArgList(0), 0, XtCacheAll, 
		       XtDestructor(0));

    // Get application resources into APP_DATA
    XtVaGetApplicationResources(toplevel, (XtPointer)&app_data,
				ddd_resources, ddd_resources_size,
				XtPointer(0));

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
				XtPointer(0));

#if XtSpecificationRelease >= 6
    // Synchronize SESSION_ID and APP_DATA.session
    if (session_id == 0)
	XtVaGetValues(toplevel, XtNsessionID, &session_id, XtPointer(0));
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
    setup_select_all_bindings(XtDatabase(XtDisplay(toplevel)));

    // Handle `--version', `--help', etc.  once more, in case the user
    // gave appropriate X resources.  Somewhat paranoid.
    setup_show(XtDatabase(XtDisplay(toplevel)), XtName(toplevel), gdb_name.chars());

    // Define font macros
    setup_fonts(app_data, XtDatabase(XtDisplay(toplevel)));
    if (app_data.show_fonts)
	return DDD_EXIT_SUCCESS;

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

	// No args given - try to get one from current selection
	add_arg_from_selection(toplevel, argc, tmp_argv);

	argv = (char **)tmp_argv;
    }

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

    // Set up warning handler
    ddd_original_xt_warning_handler =
	XtAppSetWarningHandler(app_context, ddd_xt_warning);

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

    // Global variables: Set LessTif version
    lesstif_version = app_data.lesstif_version;

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
    help_button_tip_delay = app_data.button_tip_delay;
    help_value_tip_delay  = app_data.value_tip_delay;
    help_button_doc_delay = app_data.button_doc_delay;
    help_value_doc_delay  = app_data.value_doc_delay;
    help_clear_doc_delay  = app_data.clear_doc_delay;
    help_clear_tip_delay  = app_data.clear_tip_delay;

    // Re-register own converters.  Motif may have overridden some of
    // these, so register them again.
    registerOwnConverters();

    // Show splash screen
    Boolean iconic;
    XtVaGetValues(toplevel, XmNiconic, &iconic, XtPointer(0));
    if (app_data.splash_screen && !iconic && restart_session().empty())
	popup_splash_screen(toplevel, app_data.splash_screen_color_key);

    // Re-register own converters.  Motif has overridden some of
    // these, so register them again.
    registerOwnConverters();

    // Install special Motif converters
#if XmVersion >= 1002
    XmRepTypeInstallTearOffModelConverter();
#endif

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
    const string s1 = gdb->title();
    defineConversionMacro("GDB",      s1.chars());
    defineConversionMacro("THEHOST",  DDD_HOST);
    defineConversionMacro("ROOT",     DDD_ROOT);
    defineConversionMacro("ALT_ROOT", DDD_ALT_ROOT);

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

    arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    XtSetArg(args[arg], XmNargc,           original_argc); arg++;
    XtSetArg(args[arg], XmNargv,           original_argv); arg++;

    // Forward toplevel geometry (as given by `-geometry') to command shell
    String toplevel_geometry = 0;
    XtVaGetValues(toplevel, XmNgeometry, &toplevel_geometry, XtPointer(0));
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

    // From this point on, we have a true top-level window.

    // Create main window
    arg = 0;
    Widget main_window = XmCreateMainWindow(command_shell, 
					    XMST("main_window"),
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
    Widget paned_work_w = 
	verify(XmCreatePanedWindow(main_window,
				   XMST("paned_work_w"),
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

	AddDeleteWindowCallback(data_disp_shell, DDDCloseCB);

	arg = 0;
	data_main_window_w = 
	    XmCreateMainWindow(data_disp_shell, 
			       XMST("data_main_window"),
			       args, arg);
	XtManageChild(data_main_window_w);

	// Add menu bar
	data_menubar_w = 
	    MMcreateMenuBar (data_main_window_w, 
			     "menubar", data_menubar);
	MMaddCallbacks(data_menubar);
	MMaddHelpCallback(menubar, ImmediateHelpCB);
	verify_buttons(data_menubar);
	register_menu_shell(data_menubar);

	arg = 0;
	XtSetArg(args[arg], XmNborderWidth,     0); arg++;
	XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
	XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
	data_disp_parent = 
	    verify(XmCreatePanedWindow(data_main_window_w,
				       XMST("data_paned_work_w"),
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
		       XtPointer(0));
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
	AddDeleteWindowCallback(source_view_shell, DDDCloseCB);

	arg = 0;
	source_main_window_w = 
	    XmCreateMainWindow(source_view_shell,
			       XMST("source_main_window"),
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
				       XMST("source_paned_work_w"), 
				       args, arg));
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
		       XtPointer(0));
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
    gdb_w = verify(XmCreateScrolledText(paned_work_w, 
					XMST("gdb_w"), args, arg));

    XtAddCallback (gdb_w,
		   XmNmodifyVerifyCallback,
		   gdbModifyCB,
		   XtPointer(0));
    XtAddCallback (gdb_w,
		   XmNmotionVerifyCallback,
		   gdbMotionCB,
		   XtPointer(0));
    XtAddCallback (gdb_w,
		   XmNvalueChangedCallback,
		   gdbChangeCB,
		   XtPointer(0));
    XtManageChild (gdb_w);

    if (!app_data.separate_source_window || !app_data.separate_data_window)
    {
	// Don't resize the debugger console when resizing the main
	// window - resize source and/or data windows instead
	XtVaSetValues(XtParent(gdb_w), XmNskipAdjust, True, XtPointer(0));
    }

    // Set up the scrolled window
    XtVaSetValues(XtParent(gdb_w),
		  XmNspacing,         0,
		  XmNborderWidth,     0,
		  XmNshadowThickness, 0,
		  XtPointer(0));

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
		   XtPointer(0));

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

    // If we use annotations, we also want tty mode.
    if (app_data.annotate)
	app_data.tty_mode = true;

    // Close windows explicitly requested
    if (!app_data.separate_data_window && 
	!app_data.data_window && !app_data.annotate)
    {
	// We don't want the data window.
	gdbCloseDataWindowCB(gdb_w, 0, 0);
    }

    if (!app_data.separate_source_window && 
	(!app_data.source_window || app_data.annotate))
    {
	// We don't need the source window, since we're invoked by Emacs.
	gdbCloseSourceWindowCB(gdb_w, 0, 0);

	if (!app_data.disassemble || !gdb->has_disassembly())
	    gdbCloseToolWindowCB(gdb_w, 0, 0);
    }

    if (!app_data.disassemble || !gdb->has_disassembly())
    {
	// We don't disassemble.
	gdbCloseCodeWindowCB(gdb_w, 0, 0);
    }

    if ((!app_data.separate_source_window && have_source_window() || 
	 !app_data.separate_data_window && have_data_window()) &&
	(!app_data.debugger_console || app_data.tty_mode))
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
    set_shortcut_menu(data_disp);

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
    const string tmp_1_ = gdb->info_locals_command();
    strcpy(XtName(locals_w), tmp_1_.chars());
    const string tmp_2_ = gdb->info_args_command();
    strcpy(XtName(args_w),   tmp_2_.chars());
    
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
    XtVaGetValues(toplevel, XmNiconic, &iconic, XtPointer(0));
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
#else
static bool
gdb_console_key_pressed(GUI::EventKey *evk)
{
    static long lastPrompt = 0;
    if (promptPosition > lastPrompt) {
	gdb_w->protect(lastPrompt, promptPosition);
	lastPrompt = promptPosition;
    }
    switch (evk->keyval) {
    case GDK_Up: prev_historyAct(); break;
    case GDK_Down: next_historyAct(); break;
    default:
	return false;
    }
    return true;
}

static
ddd_exit_t pre_main_loop(int argc, char *argv[])
{
  // This function exists so that all destructors are called before
  // entering the event loop.

#ifdef LC_ALL
    // Let DDD locales be controlled by the locale-specific
    // environment variables -- especially $LANG.
#if defined(IF_XM)
    XtSetLanguageProc(NULL, NULL, NULL);
#endif
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

    std::cerr << "Install error handlers?\n";

    std::ostringstream messages;

    // Set up a `~/.ddd/' directory hierarchy
    create_session_dir(DEFAULT_SESSION, messages);

    // Read ~/.ddd/init resources
    xmlDoc *dddinit = 
	GetFileDatabase(session_state_file(DEFAULT_SESSION));
    if (dddinit == 0) {
	dddinit = get_string_database("");
    }

    // Read ~/.ddd/tips resources
    xmlDoc *dddtips =
	GetFileDatabase(session_tips_file());
    if (dddtips != 0) {
	merge_databases(dddtips, dddinit);
    }

    std::cerr << "Implement XrmParseCommand analogue using popt?\n";

    const char *session_id = 0;

    if (session_id == 0)
    {
	// Determine session
	char *session_rtype = 0;
	// Glib::Value<std::string> session_value;
	// session_value.init(Glib::Value<std::string>::value_type());
	DDDValueBase session_value;

	string Nsession   = string(DDD_CLASS_NAME ".") + XtNsession;
	string CSessionID = string(DDD_CLASS_NAME ".") + XtCSessionID;
#if XtSpecificationRelease >= 6
	string NsessionID = string(DDD_CLASS_NAME ".") + XtNsessionID;
#endif

	// Try resource or option
	if (
	    get_resource(dddinit, Nsession.chars(), CSessionID.chars(),
			 session_value))
	{
	  session_id = session_value.get().c_str();
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
			
		    session_id = basename(argv[i]);
		    session_value.set(CONST_CAST(char*,session_id));
		    put_resource(dddinit, Nsession.chars(), "String", 
				 session_value);
		}
	    }
	}
    }

    if (!restart_session().empty())
    {
	// A session is given in $DDD_SESSION: override everything.
	xmlDoc *session_db = 
	    GetFileDatabase(session_state_file(restart_session()));
	if (session_db != 0) {
	    merge_databases(session_db, dddinit);
	}
    }
    else if (session_id != 0)
    {
	// Merge in session resources; these override `~/.ddd/init' as
	// well as the command-line options.
	xmlDoc *session_db = 
	    GetFileDatabase(session_state_file(session_id));
	if (session_db != 0) {
	    merge_databases(session_db, dddinit);
	}
    }

    std::cerr << "No converters\n";

    // Handle `--version', `--help', etc.  We do this here, since we
    // might want to use them even without X access.
    setup_show(dddinit, basename(argv[0]), gdb_name.chars());

    // From this point on, we'll be running under X.

    // GUI::Window *toplevel = new GUI::Window("toplevel");
    GUI::Window *toplevel;
    GUI::Main *app_context = new GUI::Main(toplevel, DDD_CLASS_NAME, session_id,
					   ddd_fallback_resources, argc, argv);


    // Check Motif version.  We can do this only now after the first
    // Motif widget has been created.
    setup_motif_version_warnings();

    std::cerr << "Actions not implemented yet\n";

    std::cerr << "No resource database: no need for converters yet.\n";
    get_application_resources(dddinit, &app_data,
			      ddd_resources, ddd_resources_size);
    xmlFreeDoc(dddinit);

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

    std::cerr << "XtDatabase not defined: is there any equivalent?\n";

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

	std::cerr << "SORRY, add_arg_from_selection not supported yet\n";

	argv = (char **)tmp_argv;
    }

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
    //help_button_tip_delay = app_data.button_tip_delay;
    //help_value_tip_delay  = app_data.value_tip_delay;
    //help_button_doc_delay = app_data.button_doc_delay;
    //help_value_doc_delay  = app_data.value_doc_delay;
    //help_clear_doc_delay  = app_data.clear_doc_delay;
    //help_clear_tip_delay  = app_data.clear_tip_delay;

    // Re-register own converters.  Motif may have overridden some of
    // these, so register them again.
    std::cerr << "No equivalent for converters\n";

    bool iconic;

    std::cerr << "Popup splash screen?\n";

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
	if (lock_info.display != string(toplevel->get_display()->get_name()))
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
    std::cerr << "Define Converters?\n";


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

    command_shell = new GUI::Window(*app_context, "command_shell", "command_shell");
    command_shell->set_default_size(800,600);
    command_shell->signal_delete_event().connect(sigc::bind_return(sigc::hide(sigc::bind(sigc::ptr_fun(DDDCloseCB), command_shell)), false));


    GUI::Container *main_window = new GUI::VBox(*command_shell, GUI::PACK_SHRINK, "main_window");
    main_window->show();

    // Create menu bar
    MMDesc *menubar = common_menubar;
    if (app_data.separate_data_window && app_data.separate_source_window)
	menubar = command_menubar;

    GUI::MenuBar *menubar_w = MMcreateMenuBar(*main_window, "menubar", menubar);
    MMaddCallbacks(menubar);
    MMaddHelpCallback(menubar, sigc::ptr_fun(ImmediateHelpCB));
    verify_buttons(menubar);
    register_menu_shell(menubar);

    // Create Paned Window

    // Note: On Motif it is possible to force a pane to have
    // fixed size.  On Gtk this does not seem possible.  Therefore
    // the toolbar and status bar must go in a VBox.
    GUI::Box *main_vbox = new GUI::VBox(*main_window, GUI::PACK_EXPAND_WIDGET);
    main_vbox->show();

    // Status line
    if (!app_data.separate_source_window && !app_data.status_at_bottom)
	create_status(main_vbox);

    // Toolbar label type
    LabelType label_type = LABEL_TYPE_STRING;
    if (app_data.button_captions || app_data.button_images)
	label_type = LABEL_TYPE_PIXMAP;

    // Common toolbar
    if (!app_data.button_captions && !app_data.button_images)
	app_data.common_toolbar = false;

    GUI::Button *arg_label = 0;

    if (!app_data.separate_source_window &&
	!app_data.separate_data_window &&
	app_data.common_toolbar)
    {
	arg_cmd_area[ArgItems::Display].type |= MMUnmanaged;
	arg_cmd_w = create_toolbar(main_vbox, "common",
				   arg_cmd_area, DataDisp::graph_cmd_area,
				   arg_label, source_arg, label_type);

	DataDisp::graph_cmd_w = arg_cmd_w;

	if (command_toolbar_w == 0)
	{
	    command_toolbar_w = make_buttons(main_vbox, 
					     "command_toolbar", 
					     app_data.tool_buttons);
	}
    }

    GUI::MultiPaned *paned_work_w = new GUI::MultiPaned(*main_vbox, GUI::PACK_EXPAND_WIDGET);
    
    std::cerr << "Install icons here?\n";

    // Data window
    GUI::Container *data_disp_parent = paned_work_w;
    GUI::MenuBar *data_menubar_w = 0;
    GUI::Container *data_main_window_w = 0;
    if (app_data.separate_data_window)
    {
	data_disp_shell = new GUI::Window(*app_context, "data_disp_shell", "data_disp_shell");
	data_disp_shell->set_default_size(800,600);
	AddDeleteWindowCallback(data_disp_shell, sigc::bind(sigc::ptr_fun(DDDCloseCB), data_disp_shell));


	data_main_window_w = new GUI::VBox(*data_disp_shell, GUI::PACK_SHRINK, "data_main_window");
	data_main_window_w->show();

	// Add menu bar
	data_menubar_w = 
	    MMcreateMenuBar (*data_main_window_w, 
			     "menubar", data_menubar);
	MMaddCallbacks(data_menubar);
	MMaddHelpCallback(menubar, sigc::ptr_fun(ImmediateHelpCB));
	verify_buttons(data_menubar);
	register_menu_shell(data_menubar);

	// Note: On Motif it is possible to force a pane to have
	// fixed size.  On Gtk this does not seem possible.  Therefore
	// the toolbar and status bar must go in a VBox.
	data_disp_parent = new GUI::VBox(*data_main_window_w);
	data_disp_parent->show();
    }

    // Create data display
    data_disp = new DataDisp(data_disp_parent, data_buttons_w);

    std::cerr << "Set values for data window?\n";

    if (!data_buttons_w)
	data_buttons_w = make_buttons(data_disp_parent, "data_buttons", 
				      app_data.data_buttons);

    // Source window
    GUI::MultiPaned *source_view_parent = paned_work_w;
    GUI::Container *source_vbox = main_vbox;
    GUI::MenuBar *source_menubar_w = 0;
    GUI::Container *source_main_window_w = 0;
    if (app_data.separate_source_window)
    {
	source_view_shell = new GUI::Window("source_view_shell", "source_view_shell");
	source_view_shell->set_default_size(800,600);
	AddDeleteWindowCallback(source_view_shell, sigc::bind(sigc::ptr_fun(DDDCloseCB), source_view_shell));


	source_main_window_w = new GUI::VBox(*source_view_shell, GUI::PACK_EXPAND_WIDGET, "source_main_window");
	source_main_window_w->show();

	// Add menu bar
	source_menubar_w = 
	    MMcreateMenuBar (*source_main_window_w, "menubar", source_menubar);
	MMaddCallbacks(source_menubar);
	MMaddHelpCallback(menubar, sigc::ptr_fun(ImmediateHelpCB));
	verify_buttons(source_menubar);
	register_menu_shell(source_menubar);

	// Add source window
	// Note: On Motif it is possible to force a pane to have
	// fixed size.  On Gtk this does not seem possible.  Therefore
	// the toolbar and status bar must go in a VBox.
	// FIXME: But, it is possible to prevent a pane from being resized when
	// the container is resized.  It is not possible, though, to prevent the user
	// from pulling the separator.
	source_vbox = new GUI::VBox(*source_main_window_w, GUI::PACK_EXPAND_WIDGET,
				    "source_vbox");
	source_vbox->show();

	// Status line
	if (!app_data.status_at_bottom)
	    create_status(source_vbox);
    }

    // Add toolbar
    if (arg_cmd_w == 0 && !app_data.toolbars_at_bottom)
    {
	arg_cmd_w = create_toolbar(source_vbox, "source",
				   arg_cmd_area, 0, arg_label, source_arg,
				   label_type);
    }

    if (command_toolbar_w == 0 && !app_data.toolbars_at_bottom)
    {
	command_toolbar_w = make_buttons(source_vbox, 
					 "command_toolbar", 
					 app_data.tool_buttons);
    }

    if (source_buttons_w == 0 && !app_data.toolbars_at_bottom)
    {
	source_buttons_w = make_buttons(source_vbox, 
					"source_buttons", 
					app_data.source_buttons);
    }

    if (app_data.separate_source_window) {
	source_view_parent = new GUI::MultiPaned(*source_vbox, GUI::PACK_EXPAND_WIDGET);
	source_view_parent->show();
    }
    source_view = new SourceView(source_view_parent);

    std::cerr << "Set values for main window?\n";

    // Source toolbar
    if (arg_cmd_w == 0)
	arg_cmd_w = create_toolbar(source_vbox, "source",
				   arg_cmd_area, 0, arg_label, source_arg,
				   label_type);
    arg_label->signal_clicked().connect(sigc::bind(sigc::ptr_fun(ClearTextFieldCB),
						   source_arg->text()));

    if (DataDisp::graph_cmd_w == arg_cmd_w)
    {
	// Common toolbar
	arg_label->signal_clicked().connect(sigc::ptr_fun(DataDisp::SelectionLostCB));
    }

    std::cerr << "More callbacks missing?\n";
    //source_arg->text()->signal_changed().connect(sigc::bind(sigc::ptr_fun(ActivateCB),
    //							    arg_cmd_area[ArgItems::Lookup].widget));
    sync_args(source_arg, data_disp->graph_arg);

    if (data_disp->graph_arg != 0) {
	// data_disp->graph_arg->text()->signal_changed().connect(sigc::bind(sigc::ptr_fun(ActivateCB),
	//								     data_disp->graph_cmd_area[0].widget));
    }

    if (command_toolbar_w == 0)
    {
	command_toolbar_w = make_buttons(source_vbox, 
					 "command_toolbar", 
					 app_data.tool_buttons);
    }
    if (command_toolbar_w != 0)
	command_toolbar_w->hide();

    if (source_buttons_w == 0)
	source_buttons_w = make_buttons(source_vbox, "source_buttons", 
					app_data.source_buttons);

    // Status line
    if (app_data.separate_source_window && app_data.status_at_bottom)
	create_status(source_vbox);

    // Debugger console
    if (console_buttons_w == 0 && !app_data.toolbars_at_bottom)
	console_buttons_w = make_buttons(paned_work_w, "console_buttons", 
					 app_data.console_buttons);

    gdb_w = new GUI::ScrolledText(*paned_work_w, GUI::PACK_EXPAND_WIDGET);

    gdb_w->signal_key_press_pre_event().connect(sigc::ptr_fun(gdb_console_key_pressed));

    std::cerr << "How to verify changes and motion?\n";

    gdb_w->signal_changed().connect(sigc::bind(sigc::ptr_fun(gdbChangeCB), gdb_w));
    gdb_w->show();

    std::cerr << "skipAdjust?\n";

    // Give the ScrolledWindow the size specified for the debugger console
    set_scrolled_window_size(gdb_w);

    if (console_buttons_w == 0)
	console_buttons_w = make_buttons(paned_work_w, "console_buttons", 
					 app_data.console_buttons);

    // Status line
    if (app_data.status_at_bottom && !app_data.separate_source_window)
	create_status(main_vbox);

    // Paned Window is done
    paned_work_w->show();

    std::cerr << "Setup areas for main window\n";


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

    promptPosition = messagePosition = gdb_w->get_last_position();
    gdb_w->set_insertion_position(messagePosition);

    std::cerr << "Setup help hooks?\n";
    // Setup help hooks
    //PreHelpOnContextHook = PreHelpOnContext;
    //PostHelpOnItemHook   = PostHelpOnItem;

    // Setup version info
    setup_version_info();

    // Customize `settings' title.
    set_settings_title(command_edit_menu[EditItems::Settings].widget);
    set_settings_title(source_edit_menu[EditItems::Settings].widget);
    set_settings_title(data_edit_menu[EditItems::Settings].widget);

    // If we use annotations, we also want tty mode.
    if (app_data.annotate)
	app_data.tty_mode = true;

    // Close windows explicitly requested
    if (!app_data.separate_data_window && 
	!app_data.data_window && !app_data.annotate)
    {
	// We don't want the data window.
	gdbCloseDataWindowCB(gdb_w);
    }

    if (!app_data.separate_source_window && 
	(!app_data.source_window || app_data.annotate))
    {
	// We don't need the source window, since we're invoked by Emacs.
	gdbCloseSourceWindowCB(gdb_w);

	if (!app_data.disassemble && !gdb->has_disassembly())
	    gdbCloseToolWindowCB();
    }

    if (!app_data.disassemble && !gdb->has_disassembly())
    {
	// We don't disassemble.
	gdbCloseCodeWindowCB(gdb_w);
    }

    if ((!app_data.separate_source_window && have_source_window() || 
	 !app_data.separate_data_window && have_data_window()) &&
	(!app_data.debugger_console || app_data.tty_mode))
    {
	// We don't need the debugger console, since we have a TTY.
	gdbCloseCommandWindowCB(gdb_w);
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
    // gtk_widget_realize(command_shell->gobj());
    Delay::register_shell(command_shell);

    if (data_disp_shell)
    {
	// gtk_widget_realize(data_disp_shell->gobj());
	Delay::register_shell(data_disp_shell);
    }

    if (source_view_shell)
    {
	// gtk_widget_realize(source_view_shell->gobj());
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

    std::cerr << "Do we need ugly hack for info locals and info args?\n";
    
    if (gdb->info_locals_command() != gdb->info_args_command())
    {
	register_info_button(locals_w);
	register_info_button(args_w);
    }
    else
    {
	register_info_button(locals_w);
	args_w->hide();
    }
    update_infos();

    // FIXME: Can the Gtk app start iconified?
    iconic = false;
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

    std::cerr << "No pressing need to track window positions...\n";

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
#endif


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

#if defined(IF_XM)
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
#else
#ifdef NAG_ME
#warning Cannot check events before dispatch?
#endif

    Gtk::Main::iteration();
#endif

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
#if defined(IF_XM)
    if (command_shell == 0)
	return;

    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);
    while (XtAppPending(app_context) & XtIMXEvent)
	process_next_event();
#else
    if (!command_shell)
	return;

    while (Gtk::Main::events_pending())
	process_next_event();
#endif
}


//-----------------------------------------------------------------------------
// Check if interaction events (key or mouse) are pending
//-----------------------------------------------------------------------------

static bool pending_interaction()
{
#if defined(IF_XM)
    XEvent event;
    const long mask = KeyPressMask | ButtonMotionMask | ButtonPressMask;
    Bool pending = XCheckMaskEvent(XtDisplay(command_shell), mask, &event);
    if (pending)
	XPutBackEvent(XtDisplay(command_shell), &event);
    return pending;
#else
#ifdef NAG_ME
#warning Implement pending_interation?
#endif

    return false;
#endif
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
#if defined(IF_XM)
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
	XtAddCallback(warning, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
#else
	GUI::Widget *warning = new GUI::MessageDialog(*command_shell,
						      "version_warning",
						      version_warnings.xmstring());
#endif

	manage_and_raise(warning);
    }

    if (app_data.dddinit_version == 0 ||
	string(app_data.dddinit_version) != DDD_VERSION)
    {
	// We have no ~/.ddd/init file or an old one: show version info
#if defined(IF_XM)
	HelpOnVersionCB(command_shell, 0, 0);
#endif

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

#if defined(IF_XM)
    if (app_data.startup_tips)
	TipOfTheDayCB(gdb_w);
#endif
}

#if defined(IF_XM)
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

    // Resources to ignore upon copying
    static const char * const do_not_copy[] = 
    { 
	XmNwidth, XmNheight,	              // Shell sizes
	XmNcolumns, XmNrows,	              // Text window sizes
	XtNtoolRightOffset, XtNtoolTopOffset, // Command tool offset
	XtNshowHints,		              // Show edge hints
    };

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
#else
// Read in database from FILENAME.  Upon version mismatch, ignore some
// resources such as window sizes.
xmlDoc *GetFileDatabase(const string& filename)
{
    std::cerr << "No version check here!\n";
    return get_file_database(filename.chars());
}
#endif



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
#if defined(IF_XM)
    const WidgetArray& shells = Delay::shells();
    for (int i = 0; i < shells.size(); i++)
    {
	Widget shell = shells[i];
	while (shell && !XmIsVendorShell(shell) && !XmIsMenuShell(shell))
	    shell = XtParent(shell);
	if (shell)
	    InstallButtonTips(shell);
    }
#else
#ifdef NAG_ME
#warning Do not install button tips for now
#endif
#endif
}

void register_menu_shell(const MMDesc *items)
{
#if defined(IF_XM)
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
#else
#ifdef NAG_ME
#warning Delay not defined yet
#endif
#endif
}


//-----------------------------------------------------------------------------
// Verify buttons
//-----------------------------------------------------------------------------

#if defined(IF_XM)
static void verify_button(const MMDesc *item, XtPointer)
{
    verify_button(item->widget);
}
#else
static void verify_button(const MMDesc *item, void *)
{
    verify_button(item->widget);
}
#endif

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
    case MAKE: display_shortcuts = app_data.bash_display_shortcuts; break;
      //    case MAKE: display_shortcuts = app_data.make_display_shortcuts; break;
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
#if defined(IF_XM)
    if (!app_data.status_at_bottom)
	return;

    Widget status_form = XtParent(status_w);
    if (!XtIsRealized(status_form))
	return;

    Dimension pane_maximum, height;
    XtVaGetValues(status_form,
		  XmNpaneMaximum, &pane_maximum, 
		  XmNheight, &height,
		  XtPointer(0));

    if (height <= pane_maximum)
	return;

    XtVaSetValues(status_form,
		  XmNallowResize, True,
		  XmNheight, pane_maximum,
		  XmNallowResize, False,
		  XtPointer(0));

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
		      XtPointer(0));

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
		    XtVaGetValues(sash,  XmNy, &sash_y, XtPointer(0));
		    XtVaGetValues(child, XmNy, &child_y, XtPointer(0));
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
#else
#ifdef NAG_ME
#warning fix_status_size is not needed?
#endif
#endif
}


//---------------------------------------------------------------------------
// Locking stuff
//---------------------------------------------------------------------------

static bool continue_despite_lock = false;
static int lock_dialog_x = -1;
static int lock_dialog_y = -1;

#if defined(IF_XM)
static void ContinueDespiteLockCB(Widget, XtPointer, XtPointer)
{
    continue_despite_lock = true;
}
#else
static void ContinueDespiteLockCB(void)
{
    continue_despite_lock = true;
}
#endif

#if defined(IF_XM)
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
#else
static bool TryLock(GUI::Widget *w)
{
    LockInfo info;
    bool lock_ok = lock_session_dir(w->get_display(), DEFAULT_SESSION, info);

    if (lock_ok)
    {
	continue_despite_lock = true;
	return false;
    }

    static sigc::connection trylock_conn;
    if (!trylock_conn)
      trylock_conn = Glib::signal_timeout().connect(sigc::bind(sigc::ptr_fun(TryLock), w), 500);
    return true;
}
#endif

#if defined(IF_XM)
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
#else
static void KillLockerCB(GUI::Widget *w, LockInfo *info)
{
    static int attempts_to_kill = 0;

    kill(info->pid, SIGHUP);

    if (attempts_to_kill++ == 0)
    {
	// Try locking again until successful
	TryLock(w);
    }
}
#endif

#if defined(IF_XM)
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
    Delay::register_shell(lock_dialog);

#if XmVersion >= 1002
    Widget kill = verify(XmCreatePushButton(lock_dialog, XMST("kill"), 
					    0, 0));
    XtManageChild(kill);
    XtAddCallback(kill, XmNactivateCallback,
		  KillLockerCB, XtPointer(&info));
    set_sensitive(kill, on_local_host);
#endif

    XtAddCallback(lock_dialog, XmNhelpCallback,
		  ImmediateHelpCB, XtPointer(0));
    XtAddCallback(lock_dialog, XmNokCallback,
		  ContinueDespiteLockCB, XtPointer(0));
    XtAddCallback(lock_dialog, XmNcancelCallback, ExitCB, 
		  XtPointer(EXIT_FAILURE));

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

    continue_despite_lock = false;
    manage_and_raise(lock_dialog);

    while (!continue_despite_lock)
	XtAppProcessEvent(XtWidgetToApplicationContext(lock_dialog), XtIMAll);

    XtDestroyWidget(lock_dialog);

    // Try locking once more
    return lock_session_dir(XtDisplay(parent), DEFAULT_SESSION, info);
}
#else
static bool lock_ddd(GUI::Widget *parent, LockInfo& info)
{
    bool lock_ok = lock_session_dir(parent->get_display(), DEFAULT_SESSION, info);
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

    msg += rm("Otherwise, make sure that you are not running "
	      "another " DDD_NAME " and ") + cr()
	+ rm("delete the ") + tt(lock_file) + rm(" file.") + cr()
	+ cr()
	+ rm("To continue despite the other " DDD_NAME " instance, ")
	+ rm("click on ") + bf("Continue") + rm(".") + cr();

    msg += rm("To exit this " DDD_NAME " instance, click on ") 
	+ bf("Exit") + rm(".");

    GUI::Dialog *lock_dialog = new GUI::Dialog(*find_shell(parent), "lock_dialog");

    Delay::register_shell(lock_dialog);

    GUI::Button *button;
    button = lock_dialog->add_button("kill", _("Kill"));
    button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(KillLockerCB), lock_dialog, &info));
    button->show();
    button = lock_dialog->add_button("continue", _("Continue"));
    button->signal_clicked().connect(sigc::ptr_fun(ContinueDespiteLockCB));
    button->show();
    button = lock_dialog->add_button("cancel", _("Cancel"));
    button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(ExitCB), EXIT_FAILURE));
    button->show();

#ifdef NAG_ME
#warning No geometry settings yet
#endif

    continue_despite_lock = false;

    lock_dialog->run();

    // Try locking once more
    return lock_session_dir(parent->get_display(), DEFAULT_SESSION, info);
}
#endif


//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------

#if defined(IF_XM)
static Boolean session_setup_done(XtPointer)
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
#else
static bool session_setup_done()
{
    if (emptyCommandQueue() && gdb->isReadyWithPrompt())
    {
	std::cerr << "session_setup_done().\n";
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

	return false;		// Remove from the list of work procs
    }

    return true;		// Get called again
}
#endif

#if defined(IF_XM)
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
		gdbOpenDataWindowCB(gdb_w, 0, 0);
	    else if (app_data.source_window)
		gdbOpenSourceWindowCB(gdb_w, 0, 0);
	    else if (app_data.data_window)
		gdbOpenDataWindowCB(gdb_w, 0, 0);
	    else
		gdbOpenCommandWindowCB(gdb_w, 0, 0);
	}

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

    return False;		// Keep on processing the command queue
}
#else
bool ddd_setup_done(void)
{
    // Delete setup delay, if any
    delete setup_delay;
    setup_delay = 0;

    if (emptyCommandQueue() && gdb->isReadyWithPrompt())
    {
	std::cerr << "ddd_setup_done().\n";
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
		gdbOpenDataWindowCB();
	    else if (app_data.source_window)
		gdbOpenSourceWindowCB();
	    else if (app_data.data_window)
		gdbOpenDataWindowCB();
	    else
		gdbOpenCommandWindowCB();
	}

	main_loop_entered = true;

	DispBox::init_vsllib(process_pending_events);
	DataDisp::refresh_graph_edit();

	if (init_delay != 0 || app_data.initial_session != 0)
	{
	    // Restoring session may still take time
	    GUI::signal_idle().connect(sigc::ptr_fun(session_setup_done));
	}
	return false;		// Remove from the list of work procs
    }

    return true;		// Keep on processing the command queue
}
#endif



//-----------------------------------------------------------------------------
// Activate argument field
//-----------------------------------------------------------------------------

#if defined(IF_XM)
static void ActivateCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
    
    Widget button = Widget(client_data);
    XtCallActionProc(button, "ArmAndActivate", cbs->event, (String *)0, 0);
}
#else
static void ActivateCB(GUI::Widget *)
{
    std::cerr << "How to call ArmAndActivate action?\n";
}
#endif


//-----------------------------------------------------------------------------
// Context help
//-----------------------------------------------------------------------------

static StatusMsg *context_help_msg = 0;

#if defined(IF_XM)
static void PreHelpOnContext(Widget w, XtPointer, XtPointer)
{
    delete context_help_msg;
    context_help_msg = 
	new StatusMsg("Please click on the item you want information for");

    XFlush(XtDisplay(w));
}
#else
static void PreHelpOnContext(GUI::Widget *w)
{
    delete context_help_msg;
    context_help_msg = 
	new StatusMsg("Please click on the item you want information for");

    w->get_display()->flush();
}
#endif

#if defined(IF_XM)
static void PostHelpOnItem(Widget item)
#else
static void PostHelpOnItem(GUI::Widget *item)
#endif
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

#if defined(IF_XM)
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
    XtVaGetValues(w, XmNset, &old_state, XtPointer(0));

    if (old_state != new_state)
    {
	if (notify)
	    XmToggleButtonSetState(w, new_state, True);
	else
	    XtVaSetValues(w, XmNset, new_state, XtPointer(0));
    }
}

inline void notify_set_toggle(Widget w, Boolean new_state)
{
    set_toggle(w, new_state, true);
}
#else
static void set_toggle(GUI::Bipolar *w, bool new_state, bool notify = false)
{
    if (w == 0)
	return;

    w->set_active(new_state, notify);
}

static void set_toggle(GUI::Widget *w, bool new_state, bool notify = false)
{
    if (w == 0)
	return;

    GUI::Bipolar *b = dynamic_cast<GUI::Bipolar *>(w);
    if (b)
	b->set_active(new_state, notify);
    else
	std::cerr << "\"" << w->get_name().c_str() << "\" is not bipolar\n";
}

inline void notify_set_toggle(GUI::Bipolar *w, bool new_state)
{
    set_toggle(w, new_state, true);
}
#endif

#if defined(IF_XM)
static void set_string(Widget w, const _XtString value)
{
    if (w == 0)
	return;

    // Note: XtVaSetValues(w, XmNvalue, value, ...) 
    // doesn't work properly with LessTif 0.89.9
    XmTextFieldSetString(w, XMST(value));
    XtVaSetValues(w, XmNcursorPosition, 0, XtPointer(0));
}
#else
static void set_string(GUI::Entry *w, const char *value)
{
    if (w == 0)
	return;

    w->set_text(value);
}
#endif

#if defined(IF_XM)
static void set_string_int(Widget w, int value)
{
    string v = itostring(value);
    set_string(w, v.chars());
}
#else
static void set_string_int(GUI::Entry *w, int value)
{
    string v = itostring(value);
    set_string(w, v.chars());
}
#endif

static bool have_cmd(const string& cmd)
{
    return cmd_file(cmd).contains('/', 0);
}

#if defined(IF_XM)
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
	XtVaSetValues(tab_width_w,     XmNvalue, app_data.tab_width,     
		      XtPointer(0));
	XtVaSetValues(source_indent_w, XmNvalue, app_data.indent_source, 
		      XtPointer(0));
	XtVaSetValues(code_indent_w,   XmNvalue, app_data.indent_code,   
		      XtPointer(0));
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
		  XtPointer(0));

    set_toggle(detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_cluster_displays_w, app_data.cluster_displays);
    set_toggle(graph_align_2d_arrays_w, app_data.align_2d_arrays);

    if (graph_snap_to_grid_w != 0)
    {
	if (!show_grid && XtIsSensitive(graph_snap_to_grid_w))
	{
	    // Grid has been disabled - disable `snap to grid' as well
	    XtVaSetValues(data_disp->graph_edit, XtNsnapToGrid, False, 
			  XtPointer(0));
	}
	else if (show_grid && !XtIsSensitive(graph_snap_to_grid_w))
	{
	    // Grid has been re-enabled - restore `snap to grid' setting
	    XtVaSetValues(data_disp->graph_edit, 
			  XtNsnapToGrid, 
			  XmToggleButtonGetState(graph_snap_to_grid_w), 
			  XtPointer(0));
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

    if (graph_grid_size_w != 0)
	XtVaSetValues(graph_grid_size_w, XmNvalue, show_grid ? grid_width : 0, 
		      XtPointer(0));


    unsigned char policy = '\0';
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &policy, 
		  XtPointer(0));
    set_toggle(set_focus_pointer_w,        policy == XmPOINTER);
    set_toggle(set_focus_explicit_w,       policy == XmEXPLICIT);

    set_toggle(set_scrolling_panner_w,     app_data.panned_graph_editor);
    set_toggle(set_scrolling_scrollbars_w, !app_data.panned_graph_editor);

    set_toggle(set_button_images_w,        app_data.button_images);
    set_toggle(set_button_captions_w,      app_data.button_captions);

    set_toggle(set_flat_buttons_w,         app_data.flat_toolbar_buttons);
    set_sensitive(set_flat_buttons_w,
		  app_data.button_images || app_data.button_captions);

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
    set_toggle(startup_tips_w,  app_data.startup_tips);
    set_toggle(set_startup_tips_w, app_data.startup_tips);

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
    source_view->set_disassemble(gdb->type() == GDB || gdb->type() == PYDB
				 && app_data.disassemble);
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
	XtVaSetValues(data_disp->graph_edit, XtNrotation, 0, XtPointer(0));
	break;
	    
    case XmHORIZONTAL:
	XtVaSetValues(data_disp->graph_edit, XtNrotation, 90, XtPointer(0));
	break;
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
    set_string(plot_command_w,       app_data.plot_command);
    set_string(get_core_command_w,   app_data.get_core_command);
    set_string(ps_command_w,         app_data.ps_command);
    set_string(term_command_w,       app_data.term_command);
    set_string(uncompress_command_w, app_data.uncompress_command);
    set_string(www_command_w,        app_data.www_command);

    // Set `find' label
    Widget find_label_ref = 0;
    const char *icon = 0;
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
    XtVaGetValues(find_label_ref, XmNlabelString, &label, XtPointer(0));
    MString new_label(label, true);
    XmStringFree(label);

    set_label(arg_cmd_area[ArgItems::Find].widget, new_label, icon);

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
	if (XtIsManaged(source_view->source_form()) ||
	    XtIsManaged(source_view->code_form()))
	    manage_paned_child(arg_cmd_w);
	else
	    unmanage_paned_child(arg_cmd_w);
    }

    // Setup undo buffer size
    UndoBuffer::max_history_depth = app_data.max_undo_depth;
    UndoBuffer::max_history_size  = app_data.max_undo_size;

    set_string_int(max_undo_size_w, app_data.max_undo_size / 1000);

    update_reset_preferences();
    fix_status_size();
}
#else
// Reflect state in option menus
static bool real_update_options(bool noupd)
{
    if (noupd)
	return false;
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
	tab_width_w->set_value(app_data.tab_width);
	source_indent_w->set_value(app_data.indent_source);
	code_indent_w->set_value(app_data.indent_code);
    }

    set_toggle(led_w, app_data.blink_while_busy);

    bool show_grid, snap_to_grid, show_hints, auto_layout, show_annotations;
    LayoutMode layout_mode;
    Dimension grid_width, grid_height;

    show_grid = data_disp->graph_edit->get_show_grid();
    snap_to_grid = data_disp->graph_edit->get_snap_to_grid();
    show_hints = data_disp->graph_edit->get_show_hints();
    show_annotations = data_disp->graph_edit->get_show_annotations();
    auto_layout = data_disp->graph_edit->get_auto_layout();
    layout_mode = data_disp->graph_edit->get_layout_mode();
    grid_width = data_disp->graph_edit->get_grid_width();
    grid_height = data_disp->graph_edit->get_grid_height();

    set_toggle(detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_detect_aliases_w, app_data.detect_aliases);
    set_toggle(graph_cluster_displays_w, app_data.cluster_displays);
    set_toggle(graph_align_2d_arrays_w, app_data.align_2d_arrays);

    if (graph_snap_to_grid_w != 0)
    {
	if (!show_grid && graph_snap_to_grid_w->is_sensitive())
	{
	    // Grid has been disabled - disable `snap to grid' as well
	    data_disp->graph_edit->set_snap_to_grid(false);
	}
	else if (show_grid && !graph_snap_to_grid_w->is_sensitive())
	{
	    // Grid has been re-enabled - restore `snap to grid' setting
	    data_disp->graph_edit->set_snap_to_grid(graph_snap_to_grid_w->get_active());
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

#if defined(IF_XM)
    set_toggle(graph_left_to_right_w, 
	       app_data.display_placement == XmHORIZONTAL);
    set_toggle(graph_top_to_bottom_w,
	       app_data.display_placement == XmVERTICAL);
#else
    set_toggle(graph_left_to_right_w, 
	       app_data.display_placement == GUI::ORIENTATION_HORIZONTAL);
    set_toggle(graph_top_to_bottom_w,
	       app_data.display_placement == GUI::ORIENTATION_VERTICAL);
#endif

    set_toggle(graph_show_hints_w, show_hints);
    set_toggle(graph_show_annotations_w, show_annotations);
    set_toggle(graph_auto_layout_w, auto_layout);
    set_toggle(graph_compact_layout_w, layout_mode == CompactLayoutMode);
    set_toggle(graph_auto_close_w, app_data.auto_close_data_window);
    set_toggle(graph_show_dependent_titles_w,
	       app_data.show_dependent_display_titles);

    if (graph_grid_size_w != 0) {
	graph_grid_size_w->set_value(show_grid ? grid_width : 0);
    }


    unsigned char policy = '\0';
    std::cerr << "Focus policy?\n";
    // XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &policy, 
    //		  XtPointer(0));

    set_toggle(set_focus_pointer_w,        policy == 1);
    set_toggle(set_focus_explicit_w,       policy == 0);

    set_toggle(set_scrolling_panner_w,     app_data.panned_graph_editor);
    set_toggle(set_scrolling_scrollbars_w, !app_data.panned_graph_editor);

    set_toggle(set_button_images_w,        app_data.button_images);
    set_toggle(set_button_captions_w,      app_data.button_captions);

    set_toggle(set_flat_buttons_w,         app_data.flat_toolbar_buttons);
    set_sensitive(set_flat_buttons_w,
		  app_data.button_images || app_data.button_captions);

    string button_color_key        = app_data.button_color_key;
    string active_button_color_key = app_data.active_button_color_key;
    set_toggle(set_color_buttons_w, button_color_key == 'c');
    set_sensitive(set_color_buttons_w, app_data.button_images);

    bool separate = 
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
#ifdef NAG_ME
#warning Tips not supported
#endif

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
#ifdef NAG_ME
#warning set_display_glyphs?
#endif
    // source_view->set_display_glyphs(app_data.display_glyphs);
    source_view->set_disassemble(gdb->type() == GDB || gdb->type() == PYDB
				 && app_data.disassemble);
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

#if defined(IF_XM)
    // Synchronize layout direction with placement
    switch (app_data.display_placement)
    {
    case XmVERTICAL:
	data_disp->graph_edit->set_rotation(0);
	break;
	    
    case XmHORIZONTAL:
	data_disp->graph_edit->set_rotation(90);
	break;
    }
#else
    // Synchronize layout direction with placement
    switch (app_data.display_placement)
    {
    case GUI::ORIENTATION_VERTICAL:
	data_disp->graph_edit->set_rotation(0);
	break;
	    
    case GUI::ORIENTATION_HORIZONTAL:
	data_disp->graph_edit->set_rotation(90);
	break;
    }
#endif

#if defined(IF_XM)
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
#else
    if (app_data.command_toolbar && 
	command_toolbar_w != 0 && !command_toolbar_w->is_visible())
    {
	if (app_data.source_window)
	    command_toolbar_w->show();
	gdbCloseToolWindowCB();
    }
    else if (!app_data.command_toolbar && 
	     command_toolbar_w != 0 && command_toolbar_w->is_visible())
    {
	command_toolbar_w->hide();
	if (app_data.source_window)
	    gdbOpenToolWindowCB();
    }
#endif

#if 0
    EnableButtonTips(app_data.button_tips);
    EnableButtonDocs(app_data.button_docs);
    EnableTextTips(app_data.value_tips);
    EnableTextDocs(app_data.value_docs);
#endif
#ifdef NAG_ME
#warning Button tips and docs.
#endif

    set_string(edit_command_w,       app_data.edit_command);
    set_string(plot_command_w,       app_data.plot_command);
    set_string(get_core_command_w,   app_data.get_core_command);
    set_string(ps_command_w,         app_data.ps_command);
    set_string(term_command_w,       app_data.term_command);
    set_string(uncompress_command_w, app_data.uncompress_command);
    set_string(www_command_w,        app_data.www_command);

    // Set `find' label
    GUI::Widget *find_label_ref = 0;
    GUI::ImageHandle *icon = 0;
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
    GUI::String new_label = get_label(find_label_ref);

    set_label(arg_cmd_area[ArgItems::Find].widget, new_label, icon);

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
    GUI::Widget *arg_cmd_w = source_arg->top()->get_parent();
    if (data_disp->graph_cmd_w == arg_cmd_w)
    {
	// Don't close the common toolbar
    }
    else
    {
	static int errcnt = 0;
	if (!errcnt++) std::cerr << "Paned children?\n";
    }

    // Setup undo buffer size
    UndoBuffer::max_history_depth = app_data.max_undo_depth;
    UndoBuffer::max_history_size  = app_data.max_undo_size;

    set_string_int(max_undo_size_w, app_data.max_undo_size / 1000);

    update_reset_preferences();
    fix_status_size();
    return false;
}
#endif

#if !defined(IF_XM)

void update_options(bool noupd)
{
    // Danger: update_options() is sometimes called from a toggle
    // callback.  Potentially this can lead to recursive calls.  So we
    // defer this until the idle loop.
    Glib::signal_idle().connect(sigc::bind(sigc::ptr_fun(real_update_options), noupd));
}
#endif

#if defined(IF_XM)
static void set_settings_title(Widget w)
{
    if (w != 0)
    {
	MString settings_title(gdb->title() + " Settings...");
	XtVaSetValues(w, XmNlabelString, settings_title.xmstring(), 
		      XtPointer(0));
    }
}
#else
static void set_settings_title(GUI::Widget *w)
{
    if (w != 0)
    {
	MString settings_title(gdb->title() + " Settings...");
	GUI::MenuItem *mi = dynamic_cast<GUI::MenuItem *>(w);
	mi->remove();
	mi->add_label(settings_title.xmstring().s());
    }
}
#endif



//-----------------------------------------------------------------------------
// Preferences
//-----------------------------------------------------------------------------

// Original application resources
static bool          option_state_saved = false;
static AppData       initial_app_data;
#if defined(IF_XM)
static Boolean       initial_show_grid;
static Boolean       initial_show_hints;
static Boolean       initial_show_annotations;
static Boolean       initial_snap_to_grid;
static Boolean       initial_auto_layout;
#else
static bool       initial_show_grid;
static bool       initial_show_hints;
static bool       initial_show_annotations;
static bool       initial_snap_to_grid;
static bool       initial_auto_layout;
#endif
static Dimension     initial_grid_width;
static Dimension     initial_grid_height;
static LayoutMode    initial_layout_mode;
static unsigned char initial_focus_policy;

#if defined(IF_XM)
static Widget preferences_dialog;
static Widget reset_preferences_w;
#else
static GUI::Dialog *preferences_dialog = NULL;
static GUI::Button *reset_preferences_w = NULL;
#endif
#if !defined(IF_XM)
static sigc::connection reset_preferences_connection;
#endif
#if defined(IF_XM)
static Widget current_panel;
#else
static GUI::Widget *current_panel;
#endif

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
#if defined(IF_XM)
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowGrid,        &initial_show_grid,
		  XtNshowAnnotations, &initial_show_annotations,
		  XtNshowHints,       &initial_show_hints,
		  XtNsnapToGrid,      &initial_snap_to_grid,
		  XtNlayoutMode,      &initial_layout_mode, 
		  XtNautoLayout,      &initial_auto_layout,
		  XtNgridWidth,       &initial_grid_width,
		  XtNgridHeight,      &initial_grid_height,
		  XtPointer(0));
#else
    initial_show_grid = data_disp->graph_edit->get_show_grid();
    initial_show_annotations = data_disp->graph_edit->get_show_annotations();
    initial_show_hints = data_disp->graph_edit->get_show_hints();
    initial_snap_to_grid = data_disp->graph_edit->get_snap_to_grid();
    initial_layout_mode = data_disp->graph_edit->get_layout_mode(); 
    initial_auto_layout = data_disp->graph_edit->get_auto_layout();
    initial_grid_width = data_disp->graph_edit->get_grid_width();
    initial_grid_height = data_disp->graph_edit->get_grid_height();
#endif

#if defined(IF_XM)
    XtVaGetValues(command_shell,
		  XmNkeyboardFocusPolicy, &initial_focus_policy, XtPointer(0));
#else
#ifdef NAG_ME
#warning Focus policy not supported
#endif
#endif

#if defined(IF_XM)
    Widget paned_work_w = XtParent(XtParent(gdb_w));
    save_preferred_paned_sizes(paned_work_w);
#endif

    option_state_saved = true;
    update_reset_preferences();
}

#if defined(IF_XM)
static void ResetGeneralPreferencesCB(Widget, XtPointer, XtPointer)
#else
static void ResetGeneralPreferencesCB(void)
#endif
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

#if defined(IF_XM)
static void ResetSourcePreferencesCB(Widget, XtPointer, XtPointer)
#else
static void ResetSourcePreferencesCB(void)
#endif
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

#if defined(IF_XM)
static void ResetDataPreferencesCB(Widget, XtPointer, XtPointer)
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

    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowHints,       &show_hints,
		  XtNshowAnnotations, &show_annotations,
		  XtPointer(0));

    if (show_hints  != initial_show_hints ||
	show_annotations != initial_show_annotations)
    {
	XtVaSetValues(data_disp->graph_edit,
		      XtNshowHints,       initial_show_hints,
		      XtNshowAnnotations, initial_show_annotations,
		      XtPointer(0));
		      
	update_options();
    }

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
		  XtPointer(0));

    if (grid_width  != initial_grid_width || 
	grid_height != initial_grid_height ||
	show_grid   != initial_show_grid)
    {
	XtVaSetValues(data_disp->graph_edit,
		      XtNgridWidth,  initial_grid_width,
		      XtNgridHeight, initial_grid_height,
		      XtNshowGrid,   initial_show_grid,
		      XtPointer(0));
		      
	update_options();
    }
}
#else
static void ResetDataPreferencesCB(void)
{
    notify_set_toggle(detect_aliases_w, initial_app_data.detect_aliases);
    notify_set_toggle(graph_detect_aliases_w, initial_app_data.detect_aliases);
    notify_set_toggle(graph_left_to_right_w,
		      initial_app_data.display_placement == GUI::ORIENTATION_HORIZONTAL);
    notify_set_toggle(graph_top_to_bottom_w,
		      initial_app_data.display_placement == GUI::ORIENTATION_VERTICAL);
    notify_set_toggle(graph_cluster_displays_w, 
		      initial_app_data.cluster_displays);
    notify_set_toggle(graph_align_2d_arrays_w, 
		      initial_app_data.align_2d_arrays);
    notify_set_toggle(graph_show_hints_w, initial_show_hints);
    notify_set_toggle(graph_show_dependent_titles_w, 
		      initial_app_data.show_dependent_display_titles);
    notify_set_toggle(graph_auto_close_w,
		      initial_app_data.auto_close_data_window);

    bool show_hints, show_annotations;

    show_hints = data_disp->graph_edit->get_show_hints();
    show_annotations = data_disp->graph_edit->get_show_annotations();

    if (show_hints  != initial_show_hints ||
	show_annotations != initial_show_annotations)
    {
	data_disp->graph_edit->set_show_hints(initial_show_hints);
	data_disp->graph_edit->set_show_annotations(initial_show_annotations);
		      
	update_options();
    }

    notify_set_toggle(graph_compact_layout_w, 
	       initial_layout_mode == CompactLayoutMode);
    notify_set_toggle(graph_auto_layout_w, initial_auto_layout);
    notify_set_toggle(graph_snap_to_grid_w, initial_snap_to_grid);

    Dimension grid_width, grid_height;
    bool show_grid;
    grid_width = data_disp->graph_edit->get_grid_width();
    grid_height = data_disp->graph_edit->get_grid_height();
    show_grid = data_disp->graph_edit->get_show_grid();

    if (grid_width  != initial_grid_width || 
	grid_height != initial_grid_height ||
	show_grid   != initial_show_grid)
    {
	data_disp->graph_edit->set_grid_width(initial_grid_width);
	data_disp->graph_edit->set_grid_height(initial_grid_height);
	data_disp->graph_edit->set_show_grid(initial_show_grid);
		      
	update_options();
    }
}
#endif

static bool data_preferences_changed()
{
    bool show_hints, show_annotations;
#if defined(IF_XM)
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowHints,       &show_hints,
		  XtNshowAnnotations, &show_annotations,
		  XtPointer(0));
#else
    show_hints = data_disp->graph_edit->get_show_hints();
    show_annotations = data_disp->graph_edit->get_show_annotations();
#endif

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
    bool show_grid, snap_to_grid, auto_layout;
    LayoutMode layout_mode;
    Dimension grid_width, grid_height;

#if defined(IF_XM)
    XtVaGetValues(data_disp->graph_edit, 
		  XtNshowGrid,   &show_grid,
		  XtNsnapToGrid, &snap_to_grid,
		  XtNlayoutMode, &layout_mode, 
		  XtNautoLayout, &auto_layout,
		  XtNgridWidth,  &grid_width,
		  XtNgridHeight, &grid_height,
		  XtPointer(0));
#else
    show_grid = data_disp->graph_edit->get_show_grid();
    snap_to_grid = data_disp->graph_edit->get_snap_to_grid();
    layout_mode = data_disp->graph_edit->get_layout_mode(); 
    auto_layout = data_disp->graph_edit->get_auto_layout();
    grid_width = data_disp->graph_edit->get_grid_width();
    grid_height = data_disp->graph_edit->get_grid_height();
#endif

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

#if defined(IF_XM)
static void ResetStartupPreferencesCB(Widget, XtPointer, XtPointer)
#else
static void ResetStartupPreferencesCB(void)
#endif
{
    bool separate = initial_app_data.separate_data_window 
	|| initial_app_data.separate_source_window;

    notify_set_toggle(set_separate_windows_w, separate);
    notify_set_toggle(set_attached_windows_w, !separate);

    notify_set_toggle(set_button_captions_w, initial_app_data.button_captions);
    notify_set_toggle(set_button_images_w,   initial_app_data.button_images);
    notify_set_toggle(set_flat_buttons_w,    
		      initial_app_data.flat_toolbar_buttons);

    string button_color_key        = initial_app_data.button_color_key;
    string active_button_color_key = initial_app_data.active_button_color_key;
#if XmVersion < 2000 || !defined(IF_XM)
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

#if defined(IF_XM)
    notify_set_toggle(set_focus_pointer_w, 
		      initial_focus_policy == XmPOINTER);
    notify_set_toggle(set_focus_explicit_w,
		      initial_focus_policy == XmEXPLICIT);
#else
    std::cerr << "Focus policy?\n";
    notify_set_toggle(set_focus_pointer_w, 
		      initial_focus_policy == 1);
    notify_set_toggle(set_focus_explicit_w,
		      initial_focus_policy == 0);
#endif

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

    bool initial_separate = (initial_app_data.separate_data_window ||
			     initial_app_data.separate_source_window);
    bool separate = (app_data.separate_data_window || 
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

#if defined(IF_XM)
    unsigned char focus_policy;
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &focus_policy, 
		  XtPointer(0));
    if (focus_policy != initial_focus_policy)
	return true;
#else
#ifdef NAG_ME
#warning Focus policy not implemented
#endif
#endif

    if (app_data.panned_graph_editor != initial_app_data.panned_graph_editor)
	return true;

    if (app_data.auto_debugger != initial_app_data.auto_debugger)
	return true;

    if (string(app_data.debugger) != string(initial_app_data.debugger))
	return true;

    return false;
}

#if defined(IF_XM)
static void ResetFontPreferencesCB(Widget, XtPointer, XtPointer)
{
    set_font(DefaultDDDFont,       initial_app_data.default_font);
    set_font(VariableWidthDDDFont, initial_app_data.variable_width_font);
    set_font(FixedWidthDDDFont,    initial_app_data.fixed_width_font);
    set_font(DataDDDFont,          initial_app_data.data_font);

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
#else
static void ResetFontPreferencesCB(void)
{
    std::cerr << "Font configuration: not implemented yet\n";

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
#endif


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

#if defined(IF_XM)
static void ResetHelpersPreferencesCB(Widget, XtPointer, XtPointer)
#else
static void ResetHelpersPreferencesCB(void)
#endif
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


#if defined(IF_XM)
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
    else if (panel_name == "fonts")
	ResetFontPreferencesCB(w, client_data, call_data);
    else if (panel_name == "helpers")
	ResetHelpersPreferencesCB(w, client_data, call_data);
}
#else
static void ResetPreferencesCB(GUI::Notebook *nb)
{
    std::cerr << "*** Resetting preferences... " << nb << " ***\n";
    GUI::Widget *panel = nb->get_current_child();
    if (!panel) {
	std::cerr << "Notebook child is not a GUI widget!\n";
	return;
    }
    GUI::String panel_name = panel->get_name();
    if (panel_name == GUI::String("general"))
	ResetGeneralPreferencesCB();
    else if (panel_name == GUI::String("source"))
	ResetSourcePreferencesCB();
    else if (panel_name == GUI::String("data"))
	ResetDataPreferencesCB();
    else if (panel_name == GUI::String("startup"))
	ResetStartupPreferencesCB();
    else if (panel_name == GUI::String("fonts"))
	ResetFontPreferencesCB();
    else if (panel_name == GUI::String("helpers"))
	ResetHelpersPreferencesCB();
    else
	std::cerr << "\007Error: panel_name not recognized\n";
}
#endif

#if defined(IF_XM)
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
	else if (panel_name == "fonts")
	    sensitive = font_preferences_changed();
	else if (panel_name == "helpers")
	    sensitive = helpers_preferences_changed();

	set_sensitive(reset_preferences_w, sensitive);
    }

    if (gdb_initialized)
	check_options_file();
}
#else
void update_reset_preferences()
{
    if (current_panel != (GUI::Widget*)0
	&& reset_preferences_w != (GUI::Widget*)0
	&& option_state_saved)
    {
	GUI::String panel_name = current_panel->get_name();

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
#endif

#if defined(IF_XM)
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
		      XtPointer(0));

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
#else
static void ChangePanelCB(GUI::CheckButton *w, GUI::Widget *panel)
{
    bool set = w->get_active();

    if (set)
    {
	// Manage this child
	panel->show();
	std::cerr << "ChangePanelCB: set help callback?\n";
	reset_preferences_connection =
	    reset_preferences_w->signal_clicked().connect(sigc::bind(sigc::retype(sigc::ptr_fun(ResetPreferencesCB)),
								     panel));
	current_panel = panel;

	update_reset_preferences();

	GUI::ChildList children = panel->get_parent()->get_children();

	for (GUI::ChildIterator i = children.begin(); i != children.end(); i++)
	{
	    GUI::Widget *child = *i;
	    if (child != panel)
	    {
		child->hide();
		std::cerr << "ChangePanelCB: remove help callback?\n";
		reset_preferences_connection.disconnect();
	    }
	}
    }
}
#endif

#if defined(IF_XM)
static Widget add_panel(Widget parent, Widget buttons, 
			const _XtString name, MMDesc items[],
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
    Widget form = verify(XmCreateRowColumn(parent, XMST(name), args, arg));
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
    XtQueryGeometry(form, (XtWidgetGeometry *)0, &size);

    size.width  += 10;		// Compensate for small rounding errors
    size.height += 10;

    max_width  = max(max_width,  size.width);
    max_height = max(max_height, size.height);

    // Add button
    arg = 0;
    Widget button = verify(XmCreateToggleButton(buttons, XMST(name), args, arg));
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
#else
static int add_panel(GUI::Notebook *parent,
		     const char *name, GUI::String label,
		     MMDesc items[],
		     bool set = false)
{
    std::cerr << ">> add_panel <<\n";
    int pageno = parent->get_n_pages();
    GUI::HBox *form = new GUI::HBox(*parent, GUI::PACK_SHRINK, name, label);
    // Do not show() the form here; this is under the control of the
    // Notebook widget.

    // Add panel
    GUI::Widget *panel = MMcreatePanel(form, "panel", items, GUI::ORIENTATION_VERTICAL);
    panel->show();
    MMadjustPanel(items);
    MMaddCallbacks(items);
    MMaddHelpCallback(items, sigc::ptr_fun(ImmediateHelpCB));
    register_menu_shell(items);

    return pageno;
}
#endif

#if defined(IF_XM)
static void OfferRestartCB(Widget dialog, XtPointer, XtPointer)
{
    if (startup_preferences_changed() || font_preferences_changed())
    {
	static Widget restart_dialog = 0;
	if (restart_dialog == 0)
	{
	    restart_dialog = 
		verify(XmCreateQuestionDialog(find_shell(dialog), 
					      XMST("restart_dialog"),
					      0, 0));
	    Delay::register_shell(restart_dialog);
	    XtAddCallback(restart_dialog, XmNokCallback,
			  DDDRestartCB, 0);
	    XtAddCallback(restart_dialog, XmNhelpCallback,
			  ImmediateHelpCB, 0);
	}
	manage_and_raise(restart_dialog);
    }
}
#else
static void OfferRestartCB(GUI::Dialog *&dialog)
{
    if (startup_preferences_changed() || font_preferences_changed())
    {
	static GUI::Dialog *restart_dialog = 0;
	if (restart_dialog == 0)
	{
	    const char *message = "DDD startup preferences were modified.\nRestart DDD to see their effect?";
	    restart_dialog = new GUI::MessageDialog(*find_shell(dialog),
						    "restart_dialog",
						    message);
	    Delay::register_shell(restart_dialog);
	    GUI::Button *button;
	    button = restart_dialog->add_button("ok", _("OK"));
	    button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(DDDRestartCB), restart_dialog));
	    button->show();
	}
	manage_and_raise(restart_dialog);
    }
}
#endif

#if defined(IF_XM)
// Create preferences dialog
static void make_preferences(Widget parent)
{
    Arg args[10];
    int arg;

    arg = 0;
    preferences_dialog = 
	verify(XmCreatePromptDialog(parent, 
				    XMST("preferences"), args, arg));
    Delay::register_shell(preferences_dialog);
    XtVaSetValues(preferences_dialog, XmNdefaultButton, Widget(0), 
		  XtPointer(0));
    XtAddCallback(preferences_dialog, XmNunmapCallback, OfferRestartCB,
		  XtPointer(0));

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
	verify(XmCreateRowColumn(preferences_dialog, XMST("box"), 
				 args, arg));
    XtManageChild(box);

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
    add_panel(change, buttons, "fonts", font_preferences_menu,
	      max_width, max_height, false);
    add_panel(change, buttons, "helpers", helpers_preferences_menu, 
	      max_width, max_height, false);

    XtVaSetValues(change,
		  XmNwidth, max_width,
		  XmNheight, max_height,
		  XmNresizeWidth, False,
		  XmNresizeHeight, False,
		  XtPointer(0));

    XmToggleButtonSetState(general_button, True, True);
}

// Popup Preference Panel
static void dddPopupPreferencesCB (Widget, XtPointer, XtPointer)
{
    manage_and_raise(preferences_dialog);
    check_options_file();
}
#else
// Create preferences dialog
static void make_preferences(GUI::Widget *parent)
{
    preferences_dialog = 
	new GUI::Dialog(*find_shell(parent), "preferences");
    Delay::register_shell(preferences_dialog);
    preferences_dialog->signal_unmap().connect(sigc::bind(sigc::ptr_fun(OfferRestartCB),
							  preferences_dialog));

    GUI::Notebook *change = new GUI::Notebook(*preferences_dialog, GUI::PACK_SHRINK, "change");
    change->show();

    GUI::Button *button = preferences_dialog->add_button("ok", _("OK"));
    button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(UnmanageThisCB), preferences_dialog));
    button->show();

    reset_preferences_w = preferences_dialog->add_button("reset", _("Reset"));
    reset_preferences_w->signal_clicked().connect(sigc::bind(sigc::ptr_fun(ResetPreferencesCB), change));
    reset_preferences_w->show();

    Dimension max_width  = 0;
    Dimension max_height = 0;

    int general_button =
	add_panel(change, "general", "General", general_preferences_menu, 
		  false);
    add_panel(change, "source", "Source", source_preferences_menu, 
	      false);
    add_panel(change, "data", "Data", data_preferences_menu, 
	      false);
    add_panel(change, "startup", "Startup", startup_preferences_menu, 
	      false);
    add_panel(change, "helpers", "Helpers", helpers_preferences_menu, 
	      false);

    change->set_current_page(general_button);
}

// Popup Preference Panel
static void dddPopupPreferencesCB (void)
{
    manage_and_raise(preferences_dialog);
    check_options_file();
}
#endif


//-----------------------------------------------------------------------------
// Create status line
//-----------------------------------------------------------------------------

#if defined(IF_XM)
static void create_status(Widget parent)
{
    Arg args[15];
    int arg = 0;
    XtSetArg(args[arg], XmNresizePolicy, XmRESIZE_ANY); arg++;
    Widget status_form = 
	verify(XmCreateForm(parent, XMST("status_form"), args, arg));
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
    if (lesstif_version <= 87)
    {
	XtSetArg(args[arg], XmNlabelString, spaces.xmstring()); arg++;
    }

    led_w = verify(XmCreateToggleButton(status_form, 
					XMST("led"), args, arg));
    XtManageChild(led_w);

    XtAddCallback(led_w, XmNvalueChangedCallback, ToggleBlinkCB, XtPointer(0));

    Pixel arrow_foreground;
    XtVaGetValues(status_form, XmNbackground, &arrow_foreground, XtPointer(0));

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
    status_w = verify(XmCreatePushButton(status_form, 
					 XMST("status"), args, arg));
    XtManageChild(status_w);

    // Initialize status history
    status_history_size = app_data.status_history_size;
    (void) status_history(parent);
    set_status("");
    set_status_mstring(short_msg);

    XtAddCallback(status_w, XmNarmCallback, 
		  PopupStatusHistoryCB, XtPointer(0));
    XtAddCallback(status_w, XmNactivateCallback, 
		  PopdownStatusHistoryCB, XtPointer(0));
    XtAddCallback(status_w, XmNdisarmCallback, 
		  PopdownStatusHistoryCB, XtPointer(0));

    XtAddCallback(arrow_w, XmNarmCallback, 
		  PopupStatusHistoryCB, XtPointer(0));
    XtAddCallback(arrow_w, XmNactivateCallback, 
		  PopdownStatusHistoryCB, XtPointer(0));
    XtAddCallback(arrow_w, XmNdisarmCallback, 
		  PopdownStatusHistoryCB, XtPointer(0));

    // Using LessTif 0.88, you can release button 1 while outside the
    // status bar; no disarm callback is invoked.  Prevent against this.
    XtAddEventHandler(status_w, ButtonReleaseMask, False,
		      PopdownStatusHistoryEH, XtPointer(0));
    XtAddEventHandler(arrow_w, ButtonReleaseMask, False, 
		      PopdownStatusHistoryEH, XtPointer(0));

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(status_w, (XtWidgetGeometry *)0, &size);

    if (lesstif_version <= 87)
	XtVaSetValues(led_w, XmNindicatorSize, size.height - 4, XtPointer(0));
    else
	XtVaSetValues(led_w, XmNindicatorSize, size.height - 1, XtPointer(0));

    XtVaSetValues(arrow_w,
		  XmNheight, size.height - 2,
		  XmNwidth,  size.height - 2,
		  XtPointer(0));
    XtVaSetValues(status_form,
		  XmNpaneMaximum, size.height,
		  XmNpaneMinimum, size.height,
		  XtPointer(0));

    set_toggle(led_w, app_data.blink_while_busy);
    blink(true);
}
#else
/* GdkPixbuf RGB C-Source image dump 1-byte-run-length-encoded */

#ifdef __SUNPRO_C
#pragma align 4 (whitelight_data)
#endif
#ifdef __GNUC__
static const guint8 whitelight_data[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 whitelight_data[] = 
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (12) */
  "\0\0\0$"
  /* pixdata_type (0x2010001) */
  "\2\1\0\1"
  /* rowstride (48) */
  "\0\0\0""0"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\377\377\377\377\377\377\377\377\202\377\377\377"};


/* GdkPixbuf RGB C-Source image dump 1-byte-run-length-encoded */

#ifdef __SUNPRO_C
#pragma align 4 (redlight_data)
#endif
#ifdef __GNUC__
static const guint8 redlight_data[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 redlight_data[] = 
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (12) */
  "\0\0\0$"
  /* pixdata_type (0x2010001) */
  "\2\1\0\1"
  /* rowstride (48) */
  "\0\0\0""0"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\377\377\0\0\377\377\0\0\202\377\0\0"};


static GUI::ImageHandle whitelight;
static GUI::ImageHandle redlight;

static void create_status(GUI::Container *parent)
{
    GUI::Box *status_form = new GUI::HBox(*parent, GUI::PACK_SHRINK, "status_form");
    status_form->show();

    whitelight = GUI::image_create_from_inline(-1, whitelight_data);
    redlight = GUI::image_create_from_inline(-1, redlight_data);

    // Create LED
    led_w = new GUI::Image(*status_form, GUI::PACK_SHRINK, "led", whitelight);
    led_w->show();

    led_w->signal_button_press_event().connect(sigc::ptr_fun(ToggleBlinkCB));

#ifdef NAG_ME
#warning Set arrow as pixmap in button.
#endif
    GUI::Button *arrow_w = new GUI::Button(*status_form, GUI::PACK_SHRINK, "arrow", "_");

    arrow_w->show();

    // Give some `dummy' status message.  Some Motif versions limit
    // the size of the status window to the length of the very first
    // message, so we give some huge string at the beginning.
    MString short_msg = rm("Hello, world!");
    MString long_msg = short_msg + rm(replicate(' ', 90));

    status_w = new GUI::Button(*status_form, GUI::PACK_EXPAND_WIDGET, "status", "Status");
    status_w->show();

    status_w->set_size_request(0, -1);

    // Initialize status history
    status_history_size = app_data.status_history_size;
    (void) status_history(parent);
    set_status("");
    set_status_mstring(short_msg);

    status_w->signal_button_press_event().connect(sigc::bind(sigc::ptr_fun(PopupStatusHistoryCB), status_w));
    status_w->signal_button_release_event().connect(sigc::ptr_fun(PopdownStatusHistoryCB));

    arrow_w->signal_button_press_event().connect(sigc::bind(sigc::ptr_fun(PopupStatusHistoryCB), arrow_w));
    arrow_w->signal_button_release_event().connect(sigc::ptr_fun(PopdownStatusHistoryCB));


#ifdef NAG_ME
#warning Is height set properly?
#endif
    Dimension size_height = status_w->get_height();

    led_w->set_size_request(-1, size_height-1);

    arrow_w->set_size_request(size_height-2, size_height-2);

#ifdef NAG_ME
#warning XmNpaneMaximum analogue?
#endif

    set_toggle(led_w, app_data.blink_while_busy);
    blink(true);
}
#endif


//-----------------------------------------------------------------------------
// Handle Status LED
//-----------------------------------------------------------------------------

static bool blinker_active        = false; // True iff status LED is active
#if defined(IF_XM)
static XtIntervalId blink_timer   = 0;     // Timer for blinking
#else
static sigc::connection blink_timer;         // Timer for blinking
#endif

#if defined(IF_XM)
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
		      XtPointer(0));
	have_led_colors = true;
    }

    bool set = bool(long(client_data));
    if (set)
	XtVaSetValues(led_w, XmNselectColor, led_select_color, 
		      XtPointer(0));
    else
	XtVaSetValues(led_w, XmNselectColor, led_background_color, 
		      XtPointer(0));

    XFlush(XtDisplay(led_w));
    XmUpdateDisplay(led_w);

    if ((blinker_active || set) && app_data.busy_blink_rate > 0)
    {
	blink_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(led_w),
				      app_data.busy_blink_rate, BlinkCB,
				      XtPointer(int(!set)));
    }
}
#else
static
bool
BlinkCB(bool *set_p)
{
    bool set = *set_p;
    *set_p = !set;
    if (set) {
	led_w->set(redlight);
    }
    else {
	led_w->set(whitelight);
    }

    // led_w->get_display()->flush();
    led_w->queue_draw();

    if ((blinker_active || set) && app_data.busy_blink_rate > 0)
    {
	return true;
    }
    return false;
}
#endif

#if defined(IF_XM)
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
#else
// Enable or disable blinking
static void blink(bool set)
{
    blinker_active = set;

    if (!led_w_active)
	return;			// Button is not active

    if (blink_timer == 0)
    {
	if (blinker_active)
	{
	    // Restart blink timer
	    static bool blink_state = false;
	    blink_timer = Glib::signal_timeout().connect(sigc::bind(sigc::ptr_fun(BlinkCB), &blink_state),
							 app_data.busy_blink_rate);
	}
    }
}
#endif

#if defined(IF_XM)
static void DisableBlinkHP(Agent *, void *, void *)
{
    // GDB has died -- disable status LED
    XmToggleButtonSetState(led_w, False, False);
}
#else
static void DisableBlinkHP(Agent *, void *, void *)
{
    // GDB has died -- disable status LED
    blinker_active = false;
}
#endif

#if defined(IF_XM)
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
#else
static bool ToggleBlinkCB(GUI::EventButton *ev)
{
    string debugger_status_indicator =
	"Debugger status indicator ";
    led_w_active = !led_w_active;
    bool set = led_w_active;

    app_data.blink_while_busy = set;

    if (set)
	set_status(debugger_status_indicator + "enabled.");
    else
	set_status(debugger_status_indicator + "disabled.");

    // Restart blinker
    blink(blinker_active);
    return false;
}
#endif

//-----------------------------------------------------------------------------
// Handle Status History
//-----------------------------------------------------------------------------

const Dimension y_popup_offset = 5;

#if defined(IF_XM)
static Widget history_shell = 0;
#else
static GUI::Menu *history_shell = 0;
#endif
#if defined(IF_XM)
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
	XtQueryGeometry(history_shell, (XtWidgetGeometry *)0, &size);

	x = shell_x;
	y = status_y - size.height - y_popup_offset;
    }
    else
    {
	XtQueryGeometry(status_w, (XtWidgetGeometry *)0, &size);

	x = shell_x;
	y = status_y + size.height + y_popup_offset;
    }

    XtVaSetValues(history_shell, XmNx, x, XmNy, y, XtPointer(0));
    XtPopup(history_shell, XtGrabNone);
}
#else
static bool
PopupStatusHistoryCB(const GUI::EventButton *, GUI::Widget *w)
{
    history_shell = status_history(w);

    //Position shell_x, shell_y;
    //XtTranslateCoords(find_shell(status_w), 0, 0, &shell_x, &shell_y);
#ifdef NAG_ME
#warning Coordinates?
#endif

    //Position status_x, status_y;
    //XtTranslateCoords(status_w, 0, 0, &status_x, &status_y);
#ifdef NAG_ME
#warning Coordinates?
#endif

    //XtWidgetGeometry size;
    //size.request_mode = CWHeight;
    Dimension size_height;
    Position x, y;
    if (app_data.status_at_bottom)
    {
	int size_height = history_shell->get_height();
    }
    else
    {
	int size_height = status_w->get_height();
    }

#ifdef NAG_ME
#warning Can we choose the position of the popup menu (SlotPositionCalc?)
#endif
    history_shell->popup();
}
#endif

#if defined(IF_XM)
static void PopdownStatusHistoryCB(Widget, XtPointer, XtPointer)
{
    if (history_shell != 0)
	XtPopdown(history_shell);
}
#else
static bool
PopdownStatusHistoryCB(const GUI::EventButton *)
{
    if (history_shell != 0)
	history_shell->hide();
}
#endif

#if defined(IF_XM)
static void PopdownStatusHistoryEH(Widget w, XtPointer client_data, 
				   XEvent *event, Boolean *)
{
    PopdownStatusHistoryCB(w, client_data, (XtPointer)event);
}
#else
#ifdef NAG_ME
#warning PopdownStatusHistoryEH unimplemented?
#endif
#endif



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

#if defined(IF_XM)
    string deref_arg = deref(arg, "()");

    MString print_ref_label("Print " + deref_arg);
    XtVaSetValues(print_menu[PrintItems::PrintRef].widget,
		  XmNlabelString, print_ref_label.xmstring(),
		  XtPointer(0));

    MString disp_ref_label("Display " + deref_arg);
    XtVaSetValues(display_menu[DispItems::DispRef].widget,
		  XmNlabelString, disp_ref_label.xmstring(),
		  XtPointer(0));
#else
    GUI::String deref_arg = deref(arg, "()").chars();

    GUI::String print_ref_label= GUI::String("Print ") + deref_arg;
    set_label(print_menu[PrintItems::PrintRef].widget, print_ref_label);

    GUI::String disp_ref_label = GUI::String("Display ") + deref_arg;
    set_label(display_menu[DispItems::DispRef].widget, disp_ref_label);
#endif

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
    case LANGUAGE_MAKE:    os << "GNU Make";      break;
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
#if defined(IF_XM)
    set_buttons(data_buttons_w,    app_data.data_buttons);
    set_buttons(source_buttons_w,  app_data.source_buttons);
    set_buttons(console_buttons_w, app_data.console_buttons);
#else
#ifdef NAG_ME
#warning set_buttons not implemented
#endif
#endif
    set_shortcut_menu(data_disp);
}


//-----------------------------------------------------------------------------
// Handlers
//-----------------------------------------------------------------------------

#if defined(IF_XM)
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
#else
static void gdb_readyHP(Agent *, void *, void *call_data)
{
    bool gdb_ready = bool(long(call_data));
    if (gdb_ready)
    {
	if (!gdb_initialized)
	{
	    gdb_initialized = true;
	    gdb_w->set_editable(true);
	}

	// Process next pending command as soon as we return
	if (!emptyCommandQueue()) {
	    GUI::signal_idle().connect(sigc::ptr_fun(processCommandQueue));
	}

	// Check for mouse pointer grabs
	check_grabs(true);

	// Completion is done
	clear_completion_delay();

	// Selection is done
	if (gdb_selection_dialog != 0)
	    gdb_selection_dialog->hide();

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
#endif


struct WhenReadyInfo {
    MString message;
#if defined(IF_XM)
    XtCallbackProc proc;
    XtPointer client_data;
    XmPushButtonCallbackStruct cbs;
    XEvent event;

    WhenReadyInfo(const MString &msg, XtCallbackProc p,
		  XtPointer cl_data,
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
#else
    sigc::slot<void, GUI::Widget *> proc;
    WhenReadyInfo(const MString &msg, sigc::slot<void, GUI::Widget *> p)
	: message(msg),
	  proc(p)
    {
    }
#endif

private:
    WhenReadyInfo(const WhenReadyInfo&);
    WhenReadyInfo& operator= (const WhenReadyInfo&);
};
#if !defined(IF_XM)
struct WhenReadyInfo1 {
    MString message;
    slot_gui_w proc;
    GUI::Event *event;

    WhenReadyInfo1(const MString &msg, slot_gui_w p)
	: message(msg),
	  proc(p)
    {
    }
private:
    WhenReadyInfo1(const WhenReadyInfo1&);
    WhenReadyInfo1& operator= (const WhenReadyInfo1&);
};
#endif

static void DoneCB(const string& /* answer */, void *qu_data)
{
    WhenReadyInfo *info = (WhenReadyInfo *)qu_data;
    set_status_mstring(info->message + rm("done."));

#if defined(IF_XM)
    (*info->proc)(gdb_w, info->client_data, XtPointer(&info->cbs));
#else
    (info->proc)(gdb_w);
#endif
    delete info;
}
#if !defined(IF_XM)
static void DoneCB1(const string& /* answer */, void *qu_data)
{
    WhenReadyInfo1 *info = (WhenReadyInfo1 *)qu_data;
    set_status_mstring(info->message + rm("done."));

    std::cerr << "Passing null pointer, this may crash...\n";
    (info->proc)(NULL);
    // (info->proc)(gdb_w);
    delete info;
}
#endif

#if defined(IF_XM)
// Execute command in (XtCallbackProc)CLIENT_DATA as soon as GDB gets ready
static void WhenReady(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    if (cbs == 0)
	return;	    // This happens with old LessTif versions

    XtCallbackProc proc = STATIC_CAST(WhenReadyProc_t*,client_data)->proc;
    XtPointer user_client_data = 0; // No way to pass extra values here

    if (can_do_gdb_command())
    {
	// GDB is ready: do command now
	proc(w, user_client_data, call_data);
	return;
    }

    // Execute command as soon as GDB gets ready
    XmString label = 0;
    XtVaGetValues(w, XmNlabelString, &label, XtPointer(0));
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
#endif

#if !defined(IF_XM)
// Execute command in (XtCallbackProc)CLIENT_DATA as soon as GDB gets ready
static void WhenReady(GUI::Widget *w, void *client_data)
{
    WhenReadyProc_t &wrp = *reinterpret_cast<WhenReadyProc_t *>(client_data);
    sigc::slot<void, GUI::Widget *> &proc = wrp.ready;

    if (can_do_gdb_command())
    {
	// GDB is ready: do command now
	proc(w);
	return;
    }

    // Execute command as soon as GDB gets ready
    GUI::String _action = get_label(w);
    string action = _action.c_str();
    if (action.contains("...", -1))
	action = action.before("...");

    MString msg = rm(action + ": waiting until " + gdb->title() 
		     + " gets ready...");
    WhenReadyInfo1 *info = new WhenReadyInfo1(msg, proc);

    // We don't want to lock the status, hence we use an ordinary
    // `set_status' call instead of the StatusMsg class.
    set_status_mstring(msg);

    Command c(gdb->nop_command(w->get_name().c_str()));
    c.origin   = w;
    c.callback = DoneCB1;
    c.data     = (void *)info;
    c.verbose  = false;
    c.prompt   = false;
    c.check    = false;
    c.priority = COMMAND_PRIORITY_USER;

    gdb_command(c);
}
#endif

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
#if defined(IF_XM)
	    gdb_command(gdb->shell_command("stty -echo -onlcr"), 0, 0, 0, 
			false, false, COMMAND_PRIORITY_AGAIN);
#else
	    gdb_command(gdb->shell_command("stty -echo -onlcr"), (GUI::Widget*)(0), 0, 0, 
			false, false, COMMAND_PRIORITY_AGAIN);
#endif
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
#if defined(IF_XM)
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
#else
#ifdef NAG_ME
#warning Cannot handle tab or CR characters from gdb
#endif
#endif

    case '\b':
    {
	// Erase last character
#if defined(IF_XM)
	XmTextReplace(gdb_w, promptPosition - 1, promptPosition, XMST(""));
#else
	gdb_w->replace(promptPosition - 1, promptPosition, XMST(""));
#endif
	promptPosition--;
    }
    break;

    case '\n':
    {
	string c = ctrl;
#if defined(IF_XM)
	XmTextInsert(gdb_w, promptPosition, XMST(c.chars()));
#else
	gdb_w->insert(promptPosition, XMST(c.chars()));
#endif
	promptPosition += c.length();

	// Flush output
#if defined(IF_XM)
	XmTextShowPosition(gdb_w, promptPosition);
#else
	gdb_w->show_position(promptPosition);
#endif
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
#if defined(IF_XM)
	XmTextInsert(gdb_w, promptPosition, XMST(c.chars()));
#else
	gdb_w->insert(promptPosition, XMST(c.chars()));
#endif
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

#if defined(IF_XM)
    if (promptPosition == 0)
	promptPosition = XmTextGetLastPosition(gdb_w);
#else
    if (promptPosition == 0)
	promptPosition = gdb_w->get_last_position();
#endif

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

#if defined(IF_XM)
	    XmTextInsert(gdb_w, promptPosition, XMST(block.chars()));
#else
	    gdb_w->insert(promptPosition, XMST(block.chars()));
#endif
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

#if defined(IF_XM)
    XmTextPosition lastPos = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, lastPos);
    XmTextShowPosition(gdb_w, lastPos);
#else
    long lastPos = gdb_w->get_last_position();
    gdb_w->set_insertion_position(lastPos);
    gdb_w->show_position(lastPos);
#endif

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

#if defined(IF_XM)
static DDDWindow ddd_window(XtPointer client_data)
{
    if (source_view_shell == 0 && data_disp_shell == 0)
	return CommonWindow;
    else
	return DDDWindow(long(client_data));
}
#else
static DDDWindow ddd_window(DDDWindow win)
{
    if (source_view_shell == 0 && data_disp_shell == 0)
	return CommonWindow;
    else
	return win;
}
#endif

#if defined(IF_XM)
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
#else
static void gdbCutSelectionCB(GUI::Widget *w, DDDWindow client_data)
{

    DDDWindow win = ddd_window(client_data);
    bool success = false;
    // Try data arg
    if (!success && (win == DataWindow || win == CommonWindow))
    {
	if (data_disp->have_selection())
	{
	    std::cerr << "Copy from graph_selection_w not implemented\n";
	    // success = text_copy_from(DataDisp::graph_selection_w);
	    if (success)
		DataDisp::deleteCB(w);
	}
    }

#ifdef NAG_ME
#warning XmGetDestination?
#endif

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow)) {
	success = text_cut_from(gdb_w);
    }

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow)) {
	success = text_cut_from(source_arg->text());
    }

    if (success)
	gdbUnselectAllCB();
}

static void gdbCopySelectionCB(GUI::Widget *w, DDDWindow client_data)
{
    
    DDDWindow win = ddd_window(client_data);
    bool success = false;

    // Try data arg
    if (!success && (win == DataWindow || win == CommonWindow))
	if (data_disp->have_selection())
	{
	    std::cerr << "Copy from graph_selection_w not implemented\n";
	    // success = text_copy_from(DataDisp::graph_selection_w);
	}

#ifdef NAG_ME
#warning XmGetDestination?
#endif

    // Try debugger console
    if (!success && (win == GDBWindow || win == CommonWindow)) {
	success = text_copy_from(gdb_w);
    }

    // Try source arg
    if (!success && (win == SourceWindow || win == CommonWindow)) {
	success = text_copy_from(source_arg->text());
    }

    // Try source
    if (!success && (win == SourceWindow || win == CommonWindow)) {
	success = text_copy_from(source_view->source());
    }

    // Try code
    if (!success && (win == SourceWindow || win == CommonWindow)) {
	success = text_copy_from(source_view->code());
    }
}

static void gdbPasteClipboardCB(GUI::Widget *w, DDDWindow client_data)
{
#ifdef NAG_ME
#warning Paste not implemented
#endif
    std::cerr << "Paste not implemented!\n";
}
#endif

#if defined(IF_XM)
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
#else
static void gdbUnselectAllCB(void)
{
    std::cerr << "gdbUnselectAllCB not implemented yet\n";
}
#endif

#if defined(IF_XM)
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
#else
static void gdbClearAllCB(void)
{
#ifdef NAG_ME
#warning Call data is only used for timestamp.
#endif
    gdbUnselectAllCB();
    source_arg->set_string("");
    gdbClearCB();
}

static void gdbSelectAllCB(GUI::Widget *w, DDDWindow client_data)
{
#ifdef NAG_ME
#warning gdbSelectAllCB not implemented
#endif
    std::cerr << "gdbSelectAllCB not implemented\n";
}
#endif


#if defined(IF_XM)
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
#else
static void gdbDeleteSelectionCB(GUI::Widget *w, DDDWindow client_data)
{
#ifdef NAG_ME
#warning gdbDeleteSelectionCB not implemented
#endif
    std::cerr << "gdbDeleteSelectionCB not implemented\n";
}
#endif

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
		      XtPointer(0));
	XtVaSetValues(menu[EditItems::Copy].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>C",
		      XmNacceleratorText, copy.xmstring(),
		      XtPointer(0));
	XtVaSetValues(menu[EditItems::Paste].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>V",
		      XmNacceleratorText, paste.xmstring(),
		      XtPointer(0));
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
		      XtPointer(0));
	XtVaSetValues(menu[EditItems::Copy].widget,
		      XmNaccelerator, "~Shift Ctrl<Key>Insert",
		      XmNacceleratorText, copy.xmstring(),
		      XtPointer(0));
	XtVaSetValues(menu[EditItems::Paste].widget,
		      XmNaccelerator, "~Ctrl Shift<Key>Insert",
		      XmNacceleratorText, paste.xmstring(),
		      XtPointer(0));
	break;
    }
    }
#else
    (void) style;
#endif
}

#if defined(IF_XM)
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

    XrmDatabase bindings = XrmGetStringDatabase(resources);
    assert(bindings != 0);
    XrmMergeDatabases(bindings, &db);
}
#else
static void setup_cut_copy_paste_bindings(xmlDoc *db)
{
    std::cerr << "setup_cut_copy_paste_bindings: not implemented\n";
}
#endif


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

#if defined(IF_XM)
	XtVaSetValues(menu[EditItems::SelectAll].widget,
		      XmNacceleratorText, select_all.xmstring(),
		      XtPointer(0));
#else
#ifdef NAG_ME
#warning Set accelerator?
#endif
#endif
	break;
    }

    case MotifBindings:
    {
	MString select_all("Shift+Ctrl+A");

#if defined(IF_XM)
	XtVaSetValues(menu[EditItems::SelectAll].widget,
		      XmNacceleratorText, select_all.xmstring(),
		      XtPointer(0));
#else
#ifdef NAG_ME
#warning Set accelerator?
#endif
#endif
	break;
    }
    }
}

#if defined(IF_XM)
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

    XrmDatabase bindings = XrmGetStringDatabase(resources);
    assert(bindings != 0);
    XrmMergeDatabases(bindings, &db);
}
#else
static void setup_select_all_bindings(xmlDoc *db)
{
    std::cerr << "setup_select_all_bindings: not implemented\n";
}
#endif


//-----------------------------------------------------------------------------
// Update menu entries
//-----------------------------------------------------------------------------

static int _mapped_menus = 0;

static int mapped_menus()
{
#if defined(IF_XM)
    if (lesstif_version <= 87)
    {
	// LessTif 0.87 and earlier does not issue a XmCR_MAP callback
	// when mapping RowColumn menus.  Hence, we always assume we
	// have some mapped menu.
	return 1;
    }
#endif

    return _mapped_menus;
}

#if defined(IF_XM)
static void count_mapped_menus(Widget, XtPointer, XtPointer call_data)
#else
static void count_mapped_menus(GdkEventAny *ev)
#endif
{
#if defined(IF_XM)
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
#else
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
#endif

    // std::clog << _mapped_menus << " mapped menus\n";
}

#if defined(IF_XM)
static void gdbUpdateEditCB(Widget w, XtPointer client_data, 
			    XtPointer call_data)
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

    count_mapped_menus(w, client_data, call_data);

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
#else
static void gdbUpdateEditCB(GUI::Widget *w, DDDWindow client_data)
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
	GUI::String label = GUI::String("Undo ") + GUI::String(undo_action.chars());
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
	GUI::String label = GUI::String("Redo ") + GUI::String(redo_action.chars());
	set_label(menu[EditItems::Redo].widget, label);
	set_sensitive(menu[EditItems::Redo].widget, true);
    }
#ifdef NAG_ME
#warning Fix sensitivity of cut/copy/paste
#endif
}
#endif

#if defined(IF_XM)
static void gdbUpdateFileCB(Widget w, XtPointer client_data, 
			    XtPointer call_data)
{
    MMDesc *file_menu = (MMDesc *)client_data;
    if (file_menu == 0 || file_menu[0].widget == 0)
	return;

    count_mapped_menus(w, client_data, call_data);

    // Check whether we can print something
    Graph *graph = graphEditGetGraph(data_disp->graph_edit);
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
#else
static void gdbUpdateFileCB(MMDesc *file_menu)
{
    if (file_menu == 0 || file_menu[0].widget == 0)
	return;

    // Check whether we can print something
    Graph *graph = data_disp->graph_edit->get_graph();
    if (!graph)
	std::cerr << "ERROR: get_graph() returned NULL\n";
    else
    {
	bool can_print = (graph->firstNode() != 0);
	set_sensitive(file_menu[FileItems::Print].widget, can_print);
	set_sensitive(file_menu[FileItems::PrintAgain].widget, can_print);
    }


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
#endif

#if defined(IF_XM)
static void gdbUpdateViewCB(Widget w, XtPointer client_data, 
			    XtPointer call_data)
{
    MMDesc *view_menu = (MMDesc *)client_data;
    if (view_menu == 0 || view_menu[0].widget == 0)
	return;

    count_mapped_menus(w, client_data, call_data);

    set_sensitive(view_menu[CodeWindow].widget, gdb->type() == GDB);
    set_sensitive(view_menu[ExecWindow].widget, gdb->has_redirection());
    set_toggle(view_menu[CodeWindow].widget, app_data.disassemble);
}
#else
static void gdbUpdateViewCB(MMDesc *view_menu)
{
    if (view_menu == 0 || view_menu[0].widget == 0)
	return;

    set_sensitive(view_menu[CodeWindow].widget, gdb->type() == GDB);
    set_sensitive(view_menu[ExecWindow].widget, gdb->has_redirection());
    set_toggle(view_menu[CodeWindow].widget, app_data.disassemble);
}
#endif

#if defined(IF_XM)
static void gdbUpdateViewsCB(Widget w, XtPointer client_data, 
			     XtPointer call_data)
{
    gdbUpdateViewCB(w, client_data, call_data);

    MMDesc *view_menu = (MMDesc *)client_data;
    if (view_menu == 0 || view_menu[0].widget == 0)
	return;

    set_toggle(view_menu[DataWindow].widget,   have_data_window());
    set_toggle(view_menu[SourceWindow].widget, have_source_window());
    set_toggle(view_menu[GDBWindow].widget,    have_command_window());
}
#else
static void gdbUpdateViewsCB(MMDesc *view_menu)
{
    gdbUpdateViewCB(view_menu);

    if (view_menu == 0 || view_menu[0].widget == 0)
	return;

    set_toggle(view_menu[DataWindow].widget,   have_data_window());
    set_toggle(view_menu[SourceWindow].widget, have_source_window());
    set_toggle(view_menu[GDBWindow].widget,    have_command_window());
}
#endif

void update_edit_menus()
{
    if (gdb_w == 0)
	return;

#if defined(IF_XM)
    XtPointer call_data = 0;

    gdbUpdateEditCB(gdb_w, XtPointer(GDBWindow),    call_data);
    gdbUpdateEditCB(gdb_w, XtPointer(SourceWindow), call_data);
    gdbUpdateEditCB(gdb_w, XtPointer(DataWindow),   call_data);
#else
    gdbUpdateEditCB(gdb_w, GDBWindow);
    gdbUpdateEditCB(gdb_w, SourceWindow);
    gdbUpdateEditCB(gdb_w, DataWindow);
#endif
}

#if defined(IF_XM)
// In case we have tear-off menus, all these menus must be updated at
// all times.
static void gdbUpdateAllMenus()
{
    if (mapped_menus() == 0)
	return;			// No mapped menu

    update_edit_menus();

    XtPointer call_data = 0;

    gdbUpdateFileCB(gdb_w, XtPointer(command_file_menu), call_data);
    gdbUpdateFileCB(gdb_w, XtPointer(source_file_menu),  call_data);
    gdbUpdateFileCB(gdb_w, XtPointer(data_file_menu),    call_data);

    gdbUpdateViewsCB(gdb_w, XtPointer(views_menu),       call_data);

    gdbUpdateViewCB(gdb_w, XtPointer(command_view_menu), call_data);
    gdbUpdateViewCB(gdb_w, XtPointer(source_view_menu),  call_data);
    gdbUpdateViewCB(gdb_w, XtPointer(data_view_menu),    call_data);
}
#else
// In case we have tear-off menus, all these menus must be updated at
// all times.
static void gdbUpdateAllMenus()
{
    if (mapped_menus() == 0)
	return;			// No mapped menu

    update_edit_menus();

    gdbUpdateFileCB(command_file_menu);
    gdbUpdateFileCB(source_file_menu);
    gdbUpdateFileCB(data_file_menu);

    gdbUpdateViewsCB(views_menu);

    gdbUpdateViewCB(command_view_menu);
    gdbUpdateViewCB(source_view_menu);
    gdbUpdateViewCB(data_view_menu);
}
#endif

//-----------------------------------------------------------------------------
// Configure new shell
//-----------------------------------------------------------------------------

#if defined(IF_XM)
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
    XtRemoveEventHandler(shell, EventMask(0), True,
			 XtEventHandler(_XEditResCheckMessages), 
			 XtPointer(0));
    XtAddEventHandler(shell, EventMask(0), True,
		      XtEventHandler(_XEditResCheckMessages),
		      XtPointer(0));
#endif

    // Use DDD logo as icon of the new shell
    if (shell != 0 && XtIsRealized(shell))
	wm_set_icon(shell, iconlogo(w), iconmask(w));
}
#else
static void setup_new_shell(GUI::Widget *w)
{
    if (w == 0)
	return;

    GUI::Shell *shell = findShellParent(w);
    if (shell == 0 && dynamic_cast<GUI::Shell *>(w))
	shell = dynamic_cast<GUI::Shell *>(w);
    if (shell == 0)
	return;

    // Use DDD logo as icon of the new shell
    std::cerr << "setup_new_shell stuff not implemented.\n";
}
#endif

//-----------------------------------------------------------------------------
// Misc functions
//-----------------------------------------------------------------------------

#if defined(IF_XM)
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
#endif




//-----------------------------------------------------------------------------
// Splash Screen
//-----------------------------------------------------------------------------

#if defined(IF_XM)
static Widget splash_shell  = 0;
static Pixmap splash_pixmap = None;
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
	if (splash_pixmap != None)
	    XFreePixmap(XtDisplay(splash_shell), splash_pixmap);

	popdown_shell(splash_shell);
	DestroyWhenIdle(splash_shell);
	splash_shell = 0;

	delete splash_delay;
	splash_delay = 0;
    }
}
#else
static GUI::Dialog *splash_shell  = 0;
static GUI::RefPtr<GUI::Pixmap> splash_pixmap;
static _Delay *splash_delay = 0;

static void popdown_splash_screen(void *data, GUI::connection *id)
{
    (void) id;			// use it

    if (data != 0)
    {
	std::cerr << "popdown_splash_screen(): timer?\n";
    }
    
    if (splash_shell != 0)
    {
	if (splash_pixmap) {
	    splash_pixmap.clear();
	}

	popdown_shell(splash_shell);
	DestroyWhenIdle(splash_shell);
	splash_shell = 0;

	delete splash_delay;
	splash_delay = 0;
    }
}
#endif

#if defined(IF_XM)
static void popup_splash_screen(Widget parent, const string& color_key)
{
    popdown_splash_screen();

    XErrorBlocker blocker(XtDisplay(parent));

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

    if (blocker.error_occurred())
	splash_pixmap = None;

    if (splash_pixmap == None)
	return;

    XtVaSetValues(splash,
		  XmNbackgroundPixmap, splash_pixmap,
		  XmNwidth, width,
		  XmNheight, height,
		  XtPointer(0));

    int x = (WidthOfScreen(XtScreen(splash_shell)) - width) / 2;
    int y = (HeightOfScreen(XtScreen(splash_shell)) - height) / 2;

    XtVaSetValues(splash_shell, XmNx, x, XmNy, y, XtPointer(0));

    // Place lock warning on top of startup splash
    lock_dialog_x = x + 20;
    lock_dialog_y = y + 20;

    XtRealizeWidget(splash_shell);

    splash_delay = new _Delay(splash_shell);

    popup_shell(splash_shell);
    wait_until_mapped(splash, splash_shell);
}
#else
static void popup_splash_screen(GUI::Widget *parent, const string& color_key)
{
    std::cerr << "popup_splash_screen not implemented yet.\n";
}
#endif

#if defined(IF_XM)
static void SetSplashScreenCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.splash_screen = info->set;

    update_options();
}
#else
static void SetSplashScreenCB(GUI::CheckButton *w)
{
    app_data.splash_screen = w->get_active();

    update_options();
}
#endif


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

    char *reason = STATIC_CAST(char *,call_data);
    if (gdb != 0 && running_msg != 0)
    {
	running_msg->outcome = reason;
	delete running_msg;
	running_msg = 0;
    }
}

#if defined(IF_XM)
static void ClearDialogCB(Widget, XtPointer client_data, XtPointer)
{
    Widget *dialog = (Widget *)client_data;
    *dialog = 0;
}
#endif

#if defined(IF_XM)
static void vsl_echo(const string& msg)
{
    static Widget dialog = 0;

    if (dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;

	// We report only the first message.  
	// Everything else only goes into the status line.
	MString message = rm("The VSL interpreter failed:") + cr() + tt(msg);

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
    }

    manage_and_raise(dialog);

    set_status_mstring(rm("VSL: ") + tt(msg));
}
#else
static void vsl_echo(const string& msg)
{
    static GUI::Widget *dialog = 0;

    if (dialog == 0)
    {
	// We report only the first message.  
	// Everything else only goes into the status line.
	MString message = rm("The VSL interpreter failed:") + cr() + tt(msg);

	dialog = new GUI::MessageDialog("vsl_warning", message.xmstring(),
					GUI::MESSAGE_WARNING);

    }

    manage_and_raise(dialog);

    set_status_mstring(rm("VSL: ") + tt(msg));
}
#endif


//-----------------------------------------------------------------------------
// Emergency
//-----------------------------------------------------------------------------

#if defined(IF_XM)
extern "C" {
    static Bool is_emergency(Display *, XEvent *event, char *)
    {
	switch (event->type)
	{
	case KeyPress:
	{
	    char buffer[1024];
	    KeySym keysym;

	    int len = XLookupString(&event->xkey, buffer, sizeof buffer, 
				    &keysym, (XComposeStatus *)0);
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
#endif

bool process_emergencies()
{
#if defined(IF_XM)
    XEvent event;
    if (XCheckIfEvent(XtDisplay(gdb_w), &event, is_emergency, 0))
    {
	// Emergency: process this event immediately
	XtDispatchEvent(&event);
	return true;
    }
#endif

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

#if defined(IF_XM)
// Return true iff resource is defined and set
static string resource_value(XrmDatabase db, const string& app_name, const char *res_name)
{
    string str_name  = app_name + "." + res_name;
    string str_class = string(DDD_CLASS_NAME) + "." + res_name;

    char *type;
    XrmValue xrmvalue;
    Bool success = XrmGetResource(db, str_name.chars(), str_class.chars(), &type, &xrmvalue);
    if (!success)
	return "";		// Resource not found

    const char *str = (const char *)xrmvalue.addr;
    int len   = xrmvalue.size - 1; // includes the final `\0'
    return string(str, len);
}
#else
// Return true iff resource is defined and set
static string resource_value(xmlDoc *db, const string& app_name, const char *res_name)
{
    string str_name  = app_name + "." + res_name;
    string str_class = string(DDD_CLASS_NAME) + "." + res_name;

    char *type;
    std::cerr << "resource_value not implemented\n";
    // Glib::Value<std::string> xrmvalue;
    // xrmvalue.init(Glib::Value<std::string>::value_type());
    DDDValueBase xrmvalue;
    bool success = get_resource(db, str_name.chars(), str_class.chars(), xrmvalue);
    if (!success)
	return "";		// Resource not found

    const char *str = xrmvalue.get().c_str();
    int len = strlen(str);
    return string(str, len);
}
#endif

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

#if defined(IF_XM)
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
#else
inline bool have_set_resource(xmlDoc *db, const char *app_name, const char *res_name)
{
    return is_set(resource_value(db, app_name, res_name));
}

static void setup_show(xmlDoc *db, const char *app_name, const char *gdb_name)
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
#endif


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
#if defined(IF_XM)
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
#endif

    string expires = ddd_expiration_date();
    if (!expires.empty())
    {
	// Add expiration date
	string expired_msg = DDD_NAME " " DDD_VERSION " ";
	if (ddd_expired())
	    expired_msg += "has expired since " + expires;
	else
	    expired_msg += "expires " + expires;

#if defined(IF_XM)
	helpOnVersionExtraText += rm(expired_msg + ".") + cr();
#endif
    }

#if defined(IF_XM)
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
#endif

    string log = session_log_file();
    if (log.contains(gethome(), 0))
	log = "~" + log.after(gethome());

#if defined(IF_XM)
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
#endif

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
#if defined(IF_XM)
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
#else
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
#endif
#endif
}


#if defined(IF_XM)
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
#else
#ifdef NAG_ME
#warning Selection does not work yet
#endif
#endif

static void setup_environment()
{
    // Set the type of the execution tty.
    switch (gdb->type())
    {
    case BASH:
    case DBG:
    case DBX:
    case GDB:
    case MAKE:
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

#if FIXED
	if ( arg == "-python"   || arg == "--python" )
	  arg = "-pydb";
	else if ( arg == "-mdb" || arg == "--mdb" )
	  arg = "--make";
	else if ( arg == "-bashdb" || arg == "--bashdb" )
	  arg = "--bash";
	else if ( arg == "-java"|| arg == "--java" )
	  arg = "--jdb";
#endif

	if ( arg == "--bash"    || arg == "-bash" 
	     || arg == "--dbg"  || arg == "-dbg"
	     || arg == "--dbx"  || arg == "-dbx"
	     || arg == "--gdb"  || arg == "-gdb"
	     || arg == "--jdb"  || arg == "-jdb"
	     || arg == "--make" || arg == "-make"
	     || arg == "--pydb" || arg == "-pydb"
	     || arg == "--perl" || arg == "-perl"
	     || arg == "--xdb"  || arg == "-xdb" )
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
#if defined(IF_XM)
	string init_msg = XmTextGetString(gdb_w);
	init_msg.gsub("\344", "ae");
	init_msg.gsub("\366", "oe");
	init_msg.gsub("\374", "ue");
	init_msg.gsub("\337", "ss");
	init_msg.gsub("\251", "(C)");
#else
	GUI::String init_msg = gdb_w->get_text();
#endif
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

#if defined(IF_XM)
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
#endif

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
    set_sensitive(disassemble_w, gdb->has_disassembly());
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

    bool have_settings = (gdb->type() != XDB);
    set_sensitive(command_edit_menu[EditItems::Settings].widget,have_settings);
    set_sensitive(source_edit_menu[EditItems::Settings].widget, have_settings);
    set_sensitive(data_edit_menu[EditItems::Settings].widget,   have_settings);

    set_sensitive(complete_w,  gdb->type() == BASH || gdb->type() == GDB 
		  || gdb->type() == PYDB);
    set_sensitive(define_w,    gdb->type() == GDB);
    set_sensitive(signals_w,   gdb->type() == GDB);

    set_sensitive(set_debugger_bash_w, have_cmd("bashdb"));
    set_sensitive(set_debugger_dbg_w,  have_cmd("dbg"));
    set_sensitive(set_debugger_dbx_w,  have_cmd("dbx") || have_cmd("ladebug"));
    set_sensitive(set_debugger_gdb_w,  have_cmd("gdb"));
    set_sensitive(set_debugger_jdb_w,  have_cmd("jdb"));
    set_sensitive(set_debugger_make_w, have_cmd("remake"));
    set_sensitive(set_debugger_perl_w, have_cmd("perl"));
    set_sensitive(set_debugger_pydb_w, have_cmd("pydb"));
    set_sensitive(set_debugger_xdb_w,  have_cmd("xdb"));

    bool can_dump = (gdb->type() == JDB);
    // (gdb->print_command("", true) != gdb->print_command("", false));
    manage_child(print_dump_w, can_dump);
}

static void setup_theme_manager()
{
#if defined(IF_XM)
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
            //ZARKO - automatsko aktiviranje flags i regs tema
            } else if (strcmp(theme, "x86.vsl") == 0) {
	        p.active() = true;
                p.add("$eax*;$ebx*;$ecx*;$edx*");
                p.add("($eflags &*");
            } else {
	        p.active() = false;
            }
	    DispBox::theme_manager.pattern(theme) = p;
	}
    }
#else
#ifdef NAG_ME
#warning No themes yet
#endif
#endif
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
