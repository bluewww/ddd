! $Id$
! DDD Resource File.  DO NOT INSTALL THIS FILE!  See below for details.
include(ifdef(`srcdir',srcdir()/macros.m4,macros.m4))dnl
DDD_APP_WARNING
! @configure_input@
Ddd*appDefaultsVersion: @VERSION@

! Copyright (C) 1995-1999 Technische Universitaet Braunschweig, Germany.
! Copyright (C) 1999-2001 Universitaet Passau, Germany.
! Copyright (C) 2001 Universitaet des Saarlandes, Germany.
! Copyright (C) 2001-2006 Free Software Foundation, Inc.
! Written by Andreas Zeller <zeller@gnu.org>.
! 
! This file is part of DDD.
! 
! DDD is free software; you can redistribute it and/or
! modify it under the terms of the GNU General Public
! License as published by the Free Software Foundation; either
! version 3 of the License, or (at your option) any later version.
! 
! DDD is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
! See the GNU General Public License for more details.
! 
! You should have received a copy of the GNU General Public
! License along with DDD -- see the file COPYING.
! If not, see <http://www.gnu.org/licenses/>.
! 
! DDD is the data display debugger.
! For details, see the DDD World-Wide-Web page, 
! `http://www.gnu.org/software/ddd/',
! or send a mail to the DDD developers <ddd@gnu.org>.

! =================================================================
! NOTE: If you're looking here for things to customize, look
! under the DDD `Preferences' menu first.  Many things are customizable
! from there.  Things which are settable via the Preferences dialog
! are stored in the `~/.ddd/init' file.
! =================================================================
!
! This file lists the default resources built into DDD.  You can use the
! stuff in this file for examples of how to customize DDD for your
! environment; normally you will do this by copying the few lines you
! want to alter to your private resource database, which usually lives
! in a file called `.Xdefaults' in your home directory.	 You may also 
! place such lines in the DDD-specific `.ddd/init' file in your home
! directory.
! 
! Site administrators **may** want to make changes here and install this
! file as the global app-defaults file for the site.  To do this, one
! would copy this file to `/usr/lib/X11/app-defaults/Ddd', or on SunOS,
! `/usr/openwin/lib/app-defaults/Ddd'.  (Actually this directory could
! be anywhere; only the person who installed X knows for sure.)
! 
! It is **STRONGLY RECOMMENDED** that you not install this file in the
! `app-defaults' directory unless you are really sure you know what you
! are doing.  There are a whole slew of version-skew problems associated
! with app-defaults files, which are best avoided by simply not using
! them.	 In particular, if the app-defaults file for a program is
! installed, and you get a new version of that program, you can't
! properly test out the new version without actually installing it,
! since it will pick up the resources for the previous version, and will
! quite likely malfunction.
! 
! In short, app-defaults files are bad.  Don't use them.  Use this file
! for informational purposes as suggested above.
!
!
! Special note for Debian users
! -----------------------------
!
! Debian policy currently requires application defaults to be included with
! packages, but considers them as part of the program code. Thus, you should
! not change this file, as it will get overwritten by an update. If you want
! to customise, the proper place is `/etc/X11/Xresources'.


!-----------------------------------------------------------------------------
! User-settable resources
!-----------------------------------------------------------------------------

! The inferior debugger type. One of: bash, dbx, gdb, jdb, mdb, perl, pydb, 
!	                              or xdb.
Ddd*debugger: gdb

! If true, determine debugger automatically from args upon start-up.
Ddd*autoDebugger: on

! The name under which the inferior debugger is to be invoked
! Default: debugger type.
Ddd*debuggerCommand:


! On if data and source are to be displayed in separate top-level
! windows.  If this is off, the data and source windows are attached
! to the debugger console.
Ddd*Separate: off


! You may also attach the source window or data window alone.
! Ddd*separateDataWindow: on
! Ddd*separateSourceWindow: on


! The windows may be initially on or off.  By default, the data window 
! remains closed since it will be opened anyway as soon as we create
! the first display.
Ddd*openDebuggerConsole: on
Ddd*openSourceWindow:    on
Ddd*openDataWindow:      off


! On if programs are to be run in a separate top-level window.
! If this is off, input/output is sent to the debugger console.
!
! Although a separate execution window has several advantages, it
! is less portable and more error-prone, which is why it is disabled 
! by default.
Ddd*separateExecWindow: off


! On if DDD windows are to be iconified/uniconified as a whole.
! If this is off, every DDD window can be iconified on its own.
Ddd*groupIconify: off


! On if DDD windows are to be uniconified when GDB becomes ready.
Ddd*uniconifyWhenReady: on


! On if the TAB key is to complete text in all DDD windows.
! If this is off, the TAB key completes in the debugger console only.
Ddd*globalTabCompletion: on


! The key bindings to use for Cut/Copy/Paste:
!
! * `Motif' means Cut/Copy/Paste is on Shift+Del/Ctrl+Ins/Shift+Ins.
!   This is conformant to the Motif style guide and the default
!   setting in DDD 2.x and earlier.
! * `KDE' means Cut/Copy/Paste is on Ctrl+X/Ctrl+C/Ctrl+V.  
!   This is conformant to the KDE style guide.  Note that this also 
!   means that you must use ESC to interrupt the debuggee.
!
! In this release, the default is still Motif, but this may change to KDE 
! in some future version.
Ddd*cutCopyPasteBindings: Motif


! The key bindings to use for Ctrl+A (Select All):
!
! * `Motif' means: Ctrl+A is Beginning of line
! * `KDE' means:   Ctrl+A is Select All
Ddd*selectAllBindings: KDE


! On if the debugger console is line-buffered (i.e. flushed upon 
! each newline).  This is slower, but gives nice scrolling.
! Off if newline has no special treatment.
Ddd*lineBufferedConsole: on


! `On' if the debugger console handles keyboard events from the source window,
! `Off' if not,
! `Auto' if only an *open* debugger console handles keyboard events.
Ddd*consoleHasFocus: on



! On if DDD should attempt to open the current selection 
! when invoked without args.
Ddd*openSelection: off


! The command to invoke the execution window for showing I/O of the debugged
! program.  To this string, DDD appends bourne shell commands which must be
! run in the execution window.
! The string `@FONT@' is replaced by the current DDD text font.
Ddd*termCommand: @XTERM@ -bg 'TEXT_BACKGROUND_COLOR' -fg 'black' \
-cr 'RUN_COLOR' -fn '@FONT@' -title 'DDD: Execution Window' -e /bin/sh -c

! The command to invoke to select fonts.
! The string `@FONT@' is replaced by the current DDD default font.
Ddd*fontSelectCommand: @XFONTSEL@ -bg 'grey' -fg 'black' -fn '@FONT@' \
-title 'DDD: @TYPE@ selector' -print


! The terminal type provided by `termCommand' (that is, the value of
! the TERM environment variable to be passed to the debugged program)
Ddd*termType: @XTERM@


! The `useTTYCommand' resource sets the redirection mode for the execution
! window. Basically, there are two ways to redirect process I/O.  Both have
! advantages and disadvantages.
!
! * GDB provides a `tty' command to redirect I/O to a separate tty.
!   This is portable, but on some systems, there is the nasty side-effect
!   that redirection can not be undone.
!   For DBX, this means using the `dbxenv run_io pty' and `dbxenv run_pty'
!   commands to redirect I/O to the execution window.
!   Set `useTTYCommand' to `on' to enable this behaviour.
! 
! * As an alternative, DDD can redirect the process I/O explicitly,
!   without using the `tty' command.  This is done with XDB and 
!   with DBX versions that do not support tty redirection.
!   Explicit redirection may fail if you use a non-standard shell.
!   Set `useTTYCommand' to `off' to enable this behaviour.
!   
! The default setting is `off', since explicit redirection is easier
! to handle (and to debug :-).  Maybe one day GDB will provide a way
! to undo redirection.
Ddd*useTTYCommand: off


! The `blockTTYInput' resource sets whether DDD should block when
! reading data from GDB via the pseudo-tty interface.  Some systems
! require this, such as Linux with libc 5.4.33 and earlier; set it to
! `on'.  Some other systems prohibit this, such as Linux with GNU libc
! 6 and later; set it to `off'.  The value `auto' will select the
! `best' choice (that is, the best choice known to the DDD developers).
Ddd*blockTTYInput: auto


! The `bufferGDBOutput' resource controls whether GDB output should be 
! buffered (i.e. kept in memory until the next prompt appears).  This helps
! DDD when parsing GDB output, but it may show up too late in the debugger
! console (esp. interaction on stdin/stdout).  The default is `auto',
! meaning to buffer when the execution window is active and not to buffer
! otherwise.
Ddd*bufferGDBOutput: auto


! The time (in seconds) to wait for synchronous GDB questions to complete
Ddd*questionTimeout: 10

! The time (in ms) to wait for GDB to finish a partial position information
Ddd*positionTimeout: 500

! The time (in ms) to wait for GDB to finish a partial display information
Ddd*displayTimeout: 2000


! Whether to interrupt running programs for debugger commands.
Ddd*stopAndContinue: on

! The time (in ms) to wait before interrupting a `run' command.
! Rationale: interrupting a `run' command immediately may interrupt
! process creation, rather than interrupting process execution.
Ddd*runInterruptDelay: 2000

! The time (in ms) to wait before interrupting a `cont' command
! Rationale: interrupting a `cont' command immediately may interrupt
! process restart, rather than interrupting process execution.
Ddd*contInterruptDelay: 200


! The `rsh' command to invoke tty-based commands on other hosts.
! On some systems, this is called `remsh'; on others, `on'.
! You may want to consult your system documentation for details.
Ddd*rshCommand: @RSH@


! If `rsh' is used without commands, using the `--rhost' option,
! the remote environment may need to be set up for DDD.  Especially, 
! echoing must be disabled; mapping of NL to CR/NL is harmful as well.
! The following commands are always issued first in the remote
! interactive shell.
Ddd*rHostInitCommands: stty -echo -onlcr


! The `ps' command to get a list of processes.
! The first line of the output must either contain a `PID' title, 
! or each line must begin with a process ID.
!
! We try the BSD style `ps x' first, then System V `ps -ef'.  If that
! fails, too, use plain `ps'.
Ddd*psCommand: ps x 2> /dev/null || ps -ef 2> /dev/null || ps


! The `print' command to print a postscript file.
! On some systems, this is called `lp'; on others, `lpr'.
! You may want to consult your system documentation for details.
Ddd*printCommand: @PRINT_COMMAND@


! Paper size, in format WIDTH x HEIGHT.  (Default: A4).
Ddd*paperSize: 210mm x 297mm




! The `edit' command to invoke an editor on the specific file.
! `@LINE@' is replaced by the current line number,
! `@FILE@' by the file name.  Default is to invoke $XEDITOR first,
! then $EDITOR, then VI.
Ddd*editCommand: \
${XEDITOR-false} +@LINE@ @FILE@ \
|| @XTERM@ -e ${EDITOR-vi} +@LINE@ @FILE@


! A command to get a core dump of a running process (typically, `gcore')
! `@FILE@' is replaced by the base name of the file to create;
! `@PID@' is replaced by the process id.  The output must be
! written to `@FILE@.@PID@'.
! Leave this entry empty if you have no `gcore' or similar command.
Ddd*getCoreCommand: @GCORE_COMMAND@


! Commands to list all executable/source/core/directory
! files matching @MASK@.
! These commands are executed on remote hosts only.
Ddd*listExecCommand:   \
	file @MASK@ | grep '.*:.*exec.*' | grep -v  '.*:.*script.*' \
	| cut -d: -f1 | grep -v '.*\\.o$'
Ddd*listSourceCommand: \
	file @MASK@ | grep '.*:.*text.*' | cut -d: -f1
Ddd*listCoreCommand:   \
	file @MASK@ | grep '.*:.*core.*' | cut -d: -f1
Ddd*listDirCommand:   \
	file @MASK@ | grep '.*:.*directory.*' | cut -d: -f1


! Command to uncompress the DDD manual.  Reads from standard input and
! writes to standard output.
Ddd*uncompressCommand: @UNCOMPRESS@


! The DDD WWW page.
Ddd*wwwPage: http://www.gnu.org/software/ddd/


! Command to invoke a WWW browser.  @URL@ stands for the URL to open.
! A large variety of choices, actually.  If you don't have WWW access,
! letting all these commands fail may take some time...

Ddd*wwwCommand: \
   firefox -remote 'openURL(@URL@)' \
|| mozilla -remote 'openURL(@URL@)' \
|| opera -remote 'openURL(@URL@)' \
|| ${WWWBROWSER-false} '@URL@' \
|| konqueror 'openURL(@URL@)' \
|| galeon 'openURL(@URL@)' \
|| skipstone 'openURL(@URL@)' \
|| light 'openURL(@URL@)' \
|| netscape -remote 'openURL(@URL@)' \
|| mozilla '@URL@' \
|| kfmbrowser '@URL@' \
|| netscape '@URL@' \
|| gnudoit '(w3-fetch \042@URL@\042)' \
|| mosaic '@URL@' \
|| Mosaic '@URL@' \
|| @XTERM@ -e lynx '@URL@'


! Plotting stuff.

! The Gnuplot command.
! The string `@FONT@' is replaced by the current DDD default font.
! The string '@NAME@' is replaced by a window name assigned by DDD.
Ddd*plotCommand: \
gnuplot -bg 'TEXT_BACKGROUND_COLOR' -font '@FONT@' -name '@NAME@' \
-geometry +5000+5000

! The Gnuplot window class.
Ddd*plotWindow: Gnuplot

! Gnuplot initialization commands.  Issued at the start of each Gnuplot
Ddd*plotInitCommands: \
set parametric\n\
set urange \1330:1\135\n\
set vrange \1330:1\135\n\
set trange \1330:1\135\n

! Gnuplot settings.  Issued for 2-D plots and 3-D plots, respectively.
Ddd*plot2dSettings:   set noborder
Ddd*plot3dSettings:   set border

! Which plot window to choose.
!
! `xlib' means to use the plot window built into DDD:
!      + no interference with window manager
!      + requires less X resources than Gnuplot window
!      - slower
!      - no customization
! `x11' instructs DDD to `swallow' the plot window supplied by Gnuplot:
!      + works just as Gnuplot users expect
!      - swallowing interferes with some window managers such as MWM.
!
! Default is `xlib' due to problems with MWM.
Ddd*plotTermType: xlib

! The time (in ms) to wait for notification that the swallowed plot window
! has been created.  After this time has passed, DDD searches the plot window
! among all windows, even without notification.
Ddd*plotWindowDelay: 2000


! Tool Bar appearance.

! Whether to have one common tool bar for one-window configurations.
Ddd*commonToolBar: on

! Whether tool bars should be placed at the bottom (Default in DDD 2.x 
! and earlier)
Ddd*toolbarsAtBottom: off

! Whether to display images and/or captions.  Turing both off causes
! tool bars to display strings (as in DDD 2.x and earlier).  The common
! tool bar, however, never displays strings - there is not enough place.
Ddd*buttonImages:   on
Ddd*buttonCaptions: on

! The image and caption areas within button images.
Ddd*buttonImageGeometry:   21x17+4+0
Ddd*buttonCaptionGeometry: 29x7+0-0

! Whether to use `flat' buttons that only have a 3d appearance when entered
Ddd*FlatButtons: on

! This may also be set separately for two categories.
! Ddd*flatToolbarButtons: on
! Ddd*flatDialogButtons:  on


! The color key to use for buttons.
!   Color (`c')  - color
!   Grey  (`g')  - greyscale
!   Mono  (`m')  - monochrome
! See the XPM documentation for details.
Ddd*buttonColorKey:       g
Ddd*activeButtonColorKey: c

! Whether to check buttons if their commands are actually supported by
! the inferior debugger.
Ddd*verifyButtons: on


! Newline-separated lists of GDB commands for which buttons are to be created.
!
! All buttons send the named command to GDB.
! Commands ending with '...' insert their name, followed by a space.
! Commands ending with a control character insert the given
! control character.  As of DDD 2.0, commands ending with `()' insert
! the current contents of the argument field `()'.
!
! The following button names have special meanings:
! - `Clear'    : Clear current command
! - `Prev'     : Show previous command
! - `Next'     : Show next command
! - `Apply'    : Send the given command to GDB
! - `Undo'     : Undo last action
! - `Redo'     : Redo last action
! - `Edit'     : Edit current source file
! - `Make'     : Remake current program
! - `Reload'   : Reload source file
! - `Complete' : Complete current command
! - `Yes'      : Answer current GDB prompt
! - `No'       : Answer current GDB prompt

! The debugger console buttons (none: no buttons)
Ddd*consoleButtons:
! The following setting was effective in DDD 1.4d and earlier:
! Ddd*consoleButtons: \
!	Yes\nNo\ninterrupt^C
! Here is a very extensive setting:
! Ddd*consoleButtons: \
!	Yes\nNo\nrun\nClear\nPrev\nNext\nApply\ninterrupt^C

! The source window buttons (none: no buttons)
Ddd*sourceButtons:
! The following setting was effective in DDD 1.x:
! Ddd*sourceButtons:	    \
! run\nstep\nnext\nstepi\nnexti\ncont\nfinish\nkill\n\
! up\ndown\nBack\nForward\nEdit\ninterrupt^C

! The data window buttons (none: no buttons)
Ddd*dataButtons:

! The command tool buttons, as of DDD 2.0 and later.
Ddd*toolButtons: \
run\nbreak^C\nstep\nstepi\nnext\nnexti\nuntil\nfinish\ncont\n\kill\n\
up\ndown\nUndo\nRedo\nEdit\nMake

Ddd*break.labelString: Interrupt


! Command tool stuff.

! Do we want a sticky command tool?  If on, the command tool follows
! every movement of the source window, such that the relative position
! remains unchanged.
Ddd*stickyTool: on

! Do we want a decorated command tool?
! * If `on', DDD will set up the command tool as a top-level window,
!   which is decorated like DDD main windows.
! * If `off', DDD realizes the command tool as a transient window, which
!   may be decorated or not, depending on the settings of your window
!   manager.
! * If `auto', DDD checks the window manager.  If it decorates
!   transients, the command tool is realized as a transient, and as
!   a top-level-window. otherwise.
Ddd*decorateTool: auto

! Do we want an auto-raised command tool?  If on, DDD will always 
! keep the command tool on top of other DDD windows.  If this setting
! interferes with your window manager, set this resource to off.
Ddd*autoRaiseTool: on

! Do we want a command tool bar instead of the command tool?
Ddd*commandToolBar: off

! The offset between the upper right corner of the command tool
! and the upper right courner in the source window.
Ddd*toolRightOffset: 8
Ddd*toolTopOffset:   8

! Do we want auto-raised menus?  This is handy with certain window managers:
! An auto-raised DDD might obscure popped up pulldown menus.  There is
! a risk that this might interfere with Motif, but we have not received
! any bug reports so far.
Ddd*autoRaiseMenu: on

! The delay during which an initial auto-raise blocks further auto-raises 
! (in ms).  This prevents `auto-raise loops'.
Ddd*autoRaiseMenuDelay: 100


! Shortcuts
! A newline-separated list of expressions to be included in the
! `Display ()' menu.  This is usually edited at run-time.
!
! As defaults, we provide some useful base conversions.
Ddd*gdbDisplayShortcuts: \
/t ()	// Convert to Bin\n\
/d ()	// Convert to Dec\n\
/x ()	// Convert to Hex\n\
/o ()	// Convert to Oct

Ddd*dbxDisplayShortcuts:  
Ddd*xdbDisplayShortcuts:
Ddd*jdbDisplayShortcuts:

Ddd*pydbDisplayShortcuts:  \
/t ()	// Convert to Bin\n\
/d ()	// Convert to Dec\n\
/x ()	// Convert to Hex\n\
/o ()	// Convert to Oct

Ddd*perlDisplayShortcuts:  \
sprintf("%#x", ())	// Convert to Hex\n\
sprintf("%#o", ())	// Convert to Oct


! Tab width in source texts
Ddd*tabWidth:	        8

! Number of characters to indent the source and machine code
Ddd*indentSource:	0
Ddd*indentCode:	        4

! Width of line numbers (if enabled; see the `displayLineNumbers' resource)
! Line numbers wider than this value extend into the breakpoint space.
Ddd*lineNumberWidth:	5

! The minimum number of lines to show before the current location.
Ddd*linesAboveCursor:	2

! The minimum number of lines to show after the current location.
Ddd*linesBelowCursor:	3


! The GDB initialization commands.  DDD depends on these settings,
! so don't change them unless required by your GDB version.
! Use a personal `~/.gdbinit' file instead to customize GDB.
!
! If a `set' command begins in the first column, the appropriate entry
! in the `settings' panel is disabled - that is, the setting is frozen.
!
! Users of GDB 4.12 and earlier may wish to add a line `display /x $pc',
! as these GDB versions do not issue the current PC location when source
! code is not available.
!
! Note: GDB 4.16 and earlier do not understand the `set annotate' command;
! just ignore the warning message.
Ddd*gdbInitCommands:	\
set prompt (gdb) \n\
set height 0\n\
set width 0\n\
set annotate 1\n\
 set verbose off\n

! The GDB settings.  Usually overridden in `~/.ddd/init'.
Ddd*gdbSettings: \
set print asm-demangle on\n


! The DBX initialization commands. Make sure the TTY is in a proper state,
! by invoking a `stty' command; Some DBXes require this.
!
! Do not change these settings.  Use a personal `~/.dbxinit' 
! or `~/.dbxrc' file to customize DBX.
Ddd*dbxInitCommands: \
sh stty -echo -onlcr\n\
set $page = 1\n

! The DBX settings.  Usually overridden in `~/.ddd/init'.
Ddd*dbxSettings:


! The XDB initialization commands.  Define some macros to make common 
! commands available.
!
! Do not change these settings.  Use a personal `~/.xdbrc' file
! to customize DBX.
Ddd*xdbInitCommands: \
sm\n\
def run r\n\
def cont c\n\
def next S\n\
def step s\n\
def quit q\n\
def finish { bu \\1t ; c ; L }\n

Ddd*bashInitCommands: \
  set prompt bashdb$_Dbg_less$_Dbg_greater$_Dbg_space\n

! The Bash Debugger settings.  Usually overridden in `~/.ddd/init'.
Ddd*bashSettings:

! The PHP DBG initialization commands.  Nothing special.
Ddd*dbgInitCommands:

! The PHP DBG Debugger settings.  Usually overridden in `~/.ddd/init'.
Ddd*dbgSettings:

! The JDB initialization commands.  Nothing special.
Ddd*jdbInitCommands:

! The JDB settings.  Usually overridden in `~/.ddd/init'.
Ddd*jdbSettings:

! The Make Debuggger initialization commands.  Nothing special.
Ddd*makeInitCommands:

! The Make Debugger Debugger settings.  Usually overridden in `~/.ddd/init'.
Ddd*makeSettings:

! The Perl Debugger initialization commands.  
! Disable ornaments; enable emacs mode; set output format.
Ddd*perlInitCommands: \
o CommandSet=580\n

! The Perl Debugger settings.  Usually overridden in `~/.ddd/init'.
Ddd*perlSettings:

! The PYDB initialization commands.  Nothing special.
Ddd*pydbInitCommands:

! The PYDB settings.  Usually overridden in `~/.ddd/init'.
Ddd*pydbSettings:

! The XDB settings.  Usually overridden in `~/.ddd/init'.
Ddd*xdbSettings:

! If on, let GDB source all initialization commands from a temporary file.
! This is slightly faster than processing each command separately,
! but gives bad diagnostics.
Ddd*sourceInitCommands: on


! Startup symbol
! When DDD loads a file, and GDB does not report a source file,
! DDD looks for these symbols.
Ddd*initSymbols: \
main\n\
MAIN\n\
main_\n\
MAIN_\n\
main__\n\
MAIN__\n\
_main\n\
_MAIN\n\
__main\n\
__MAIN\n\
my_main\n\
the_main\n\
vxworks_main\n\
main_vxworks

! Auto-command defaults

! Are auto-commands enabled?  If yes, any output of the inferior debugger
! in the form `PREFIX: COMMAND' (where PREFIX is the autoCommandPrefix, 
! as defined below) will cause DDD to execute the so-called 
! auto-command COMMAND.  This is useful for defining own DDD command 
! sequences.
! Unfortunately, this is also a security problem - just imagine some
! malicuous program to issue a string like `PREFIX: shell rm foobar'.
! Hence, we create PREFIX dynamically for each DDD session.
Ddd*autoCommands: on

! The prefix of auto-commands.
! No value means to generate a new PREFIX for each DDD session.
Ddd*autoCommandPrefix:


! Graph Editor Defaults

! Shall we detect aliases?  (expensive)
Ddd*detectAliases:		on

! Shall we require structural equivalence for aliases?
Ddd*typedAliases:		on

! Shall we delete aliases together with the aliased displays?
Ddd*deleteAliasDisplays:	on

! Shall we layout two-dimensional arrays as tables?
Ddd*align2dArrays:		on

! After a change in a display, shall we move adjacent displays?
Ddd*bumpDisplays:		on

! Shall we expand `VALUE <repeated N times>' to N instances of VALUE?
Ddd*expandRepeatedValues:       off

! Shall we hide displays that are out of scope?
Ddd*hideInactiveDisplays:	on

! Shall we show titles on base displays? (recommended)
Ddd*showBaseDisplayTitles:      on

! Shall we show titles on base displays?
! (Useful when annotations are off, as in DDD 3.0 and earlier.)
Ddd*showDependentDisplayTitles: off


! The VSL library for displaying graph nodes.  `builtin' means the VSL
! library compiled into DDD, named `ddd.vsl'.  If you wish to use
! another VSL library, put its name here and set `vslPath' to a
! colon-separated list of directories where the VSL file and its
! includes can be found.  
! The following directory names are special:
! `user_themes' stands for `~/.ddd/themes'.
! `ddd_themes'  stands for `/usr/local/share/ddd-VERSION/themes/'.
!
! If you use the builtin library, you can use the `vslDefs' resource,
! to override (customize) certain settings.  See `colors', below.
Ddd*vslLibrary:			builtin
Ddd*vslPath:			user_themes:ddd_themes:.

! What shall we use if the graph gets too large to be displayed?
! If on, an Athena panner is used (a kind of two-directional scrollbar).
! If off, two Motif scrollbars are used.
!
! Although a panned graph editor is much more comfortable, we set-up Motif
! scrollbars as a default: Many OSF/Motif installations choke
! on intermixed Motif/Athena applications and we don't want to get bug
! reports that should be directed to OSF.
Ddd*pannedGraphEditor:		off

! Nodes are moved when dragged more than 4 pixels.
Ddd*graph_edit.moveDelta:	4

! The selection tile pixmap.  Usual choices are Motif built-in pixmaps.
! If this resource is left unspecified, nodes are simply inverted.
! Ddd*graph_edit.selectTile:	25_foreground

! Do we wish opaque movement? (doesn't work yet)
Ddd*graph_edit.opaqueMove:	off

! Do we wish to display edges when moving nodes?
Ddd*graph_edit.rubberEdges:	on

! Do we wish to display arrow heads when moving nodes?
Ddd*graph_edit.rubberArrows:	on

! Do we wish to display annotations when moving nodes?
Ddd*graph_edit.rubberAnnotations: off

! Do we wish a grid?
Ddd*graph_edit.showGrid:	on

! Do we wish to snap the position to the grid?
Ddd*graph_edit.snapToGrid:	on

! Do we wish to show edge hints?
Ddd*graph_edit.showHints:	on

! Do we wish to show edge annotations?
Ddd*graph_edit.showAnnotations:	on

! What kind of layout do we wish? (regular, compact)
Ddd*graph_edit.layoutMode:	regular

! Do we wish to re-layout the graph after each change?
Ddd*graph_edit.autoLayout:	off

! Width and height of the grid
! Ddd*graph_edit.gridWidth:	16
! Ddd*graph_edit.gridHeight:	16
Ddd*graph_edit.GridSize:	16


! Source View Defaults

! Do we wish to find complete words only?
Ddd*findWordsOnly:		on

! Do we wish case-sensitive search?
Ddd*findCaseSensitive:		on

! Do we wish to show machine code? (Makes DDD run somewhat slower)
Ddd*disassemble: off

! How many bytes of the current function should we disassemble? (0
! means disassemble entire function; default in DDD 2.2 and earlier)
Ddd*maxDisassemble: 256

! Do we wish to display glyphs? (Makes DDD run somewhat slower)
Ddd*displayGlyphs: on

! What is the maximum number of glyphs at one time?
Ddd*maxGlyphs: 10

! The time (in ms) to wait before updating glyph positions while scrolling
Ddd*glyphUpdateDelay: 5

! Shall we cache glyph images?  (Reported to fail with Motif 2.1 on XFree86)
! Ddd*cacheGlyphImages: on

! Do we wish to display line numbers? (Default in DDD 2.1.1 and earlier)
Ddd*displayLineNumbers: off

! Do we wish to cache source files in memory?
Ddd*cacheSourceFiles: on

! Do we wish to cache machine code in memory?
Ddd*cacheMachineCode: on

! Do we wish to refer to sources using the full path name?
Ddd*useSourcePath: off

! Do we wish to save history on exit?
Ddd*saveHistoryOnExit: on

! Do we wish to save options on exit?
Ddd*saveOptionsOnExit: on

! Do we wish to show all registers, instead of only integer registers?
Ddd*allRegisters: off

! Tips (aka ballon help or bubble help) are small windows that pop up
! when moving the pointer over some item.  Instructive for DDD
! novices, but maybe annoying for DDD experts.

! Do we want tips on buttons, telling us their meaning?
Ddd*buttonTips: on

! Do we want tips on variables in the source code, telling us their value?
Ddd*valueTips: on


! The same info as in tips may also be shown in the status line (`docs').
! This is less annoying, but may slow down DDD a bit.
! Do we want docs on buttons, telling us their meaning?
Ddd*buttonDocs: on

! Do we want docs on variables in the source code, telling us their value?
Ddd*valueDocs: on


! Delays for showing tips and docs

! Time (in ms) to spend before raising a button or value tip.
Ddd*buttonTipDelay: 750
Ddd*valueTipDelay:  750

! Time (in ms) to spend before showing a button or value doc.
Ddd*buttonDocDelay: 100
Ddd*valueDocDelay:  100

! Time (in ms) to spend before clearing the value doc.
Ddd*clearDocDelay: 1000

! Time (in ms) to spend before clearing a button or value tip.
Ddd*clearTipDelay:   50

! Time (in ms) to spend before a push button menu pops up
Ddd*pushMenuPopupTime: 400



! Popdown history

! The maximum number of items in pop-down histories.  0 means `unlimited'.
Ddd*popdownHistorySize: 0

! Shall pop-down histories be sorted?
! on: sort alphabetically
! off: don't sort; least-recently-used comes first
Ddd*sortPopdownHistory: on


! Status line

! Should the status line be placed at the bottom?
Ddd*statusAtBottom: on

! Number of status messages to be kept in status history
Ddd*statusHistorySize: 20

! Do we want to see the debugger status (blinking while busy)?
Ddd*blinkWhileBusy: on

! Blink rate (in ms) of the busy LED - the LED showing that the inferior
! debugger is busy.  A value of 0 prevents the busy LED from blinking.
Ddd*busyBlinkRate: 500


! Maximum lengths

! The maximum length of a data display title (in characters)
! Longer names are replaced by `..'; `foo->bar->blue' becomes `foo->..->blue'.
Ddd*maxDisplayTitleLength: 20

! The maximum length of an expression in the source popup.
Ddd*maxPopupExprLength: 20

! The maximum length of a value tip.
Ddd*maxValueTipLength: 60

! The maximum length of a value doc.
Ddd*maxValueDocLength: 80


! Undo and Redo
! How many actions should we be able to undo?  (0: none, <0: unlimited).
Ddd*maxUndoDepth: -1

! How many bytes can the undo buffer grow? (<0: unlimited)
Ddd*maxUndoSize: 2000000

! Focus Policy
! POINTER means point-and-type, EXPLICIT means click-and-type.
Ddd*keyboardFocusPolicy: POINTER

! Warnings
! Do we wish to suppress X warnings?
Ddd*suppressWarnings: off

! Do we wish to warn if multiple DDD instances are running?
Ddd*warnIfLocked: off

! Options file checking.
! Check for changes to the options file (typically, `~/.ddd/init')
! every N seconds.
Ddd*checkOptions: 30


! Grab checking.

! Shall DDD check for grabbed (frozen) mouse pointers
! when interrupting a modal X application?
Ddd*checkGrabs: on

! The delay to check for grabs after the last GDB prompt.
Ddd*checkGrabDelay: 5000

! The delay to wait for effective action.
Ddd*grabActionDelay:   10000

! The debugger command to execute when the pointer is still grabbed.
Ddd*grabAction:  cont


! Fonts.

! The default font to use for labels, etc.
Ddd*defaultFont:       helvetica-medium

! The font to use for help texts.
Ddd*variableWidthFont: helvetica-medium

! The font to use for fixed-width fields, such as the source code.
Ddd*fixedWidthFont:    lucidatypewriter-medium

! The font to use for data displays.
Ddd*dataFont:          lucidatypewriter-medium

! The base font size; overrides the point settings as set above.
Ddd*FontSize:          120


! A little less highlighting.
Ddd*highlightThickness: 1


! Window sizes.
Ddd*graph_edit.height:      100

Ddd*source_text_w.columns:  80
Ddd*source_text_w.rows:     31

Ddd*code_text_w.columns:    80
Ddd*code_text_w.rows:       4

Ddd*gdb_w.columns:          80
Ddd*gdb_w.rows:		    5


! Menus.

! Enable tear-off pulldown menus
Ddd*menubar*tearOffModel: XmTEAR_OFF_ENABLED



! Splash screen

! Whether to show the DDD splash screen
Ddd*splashScreen: on

! DDD can display its splash screen in four different ways:
!
!   Color (`c')  - using up to 42 shades of blue. (Nice, but costly.)
!   Grey  (`g')  - using up to 42 shades of grey. (Nice on greyscale displays.)
!   Grey4 (`g4') - using 4 shades of grey.        (A balanced compromise.)
!   Mono  (`m')  - using only black and white.    (On monochrome displays.)
!
! Use the key given in parentheses as resource value.  Default is `c'.
Ddd*splashScreenColorKey: c


! Logos and icons

! Whether to use multi-color icons.  Multi-color icons are not strictly
! ICCM-compliant, although supported by most window managers.  In case
! your window manager has trouble with DDD icons, set this to `off'.
Ddd*colorWMIcons: on


! Session Defaults

! DDD attempts to preserve breakpoint and display numbers across
! sessions.  Unfortunately, if they grow too large, startup takes
! much time due to `dummy' commands inserted in order to push up
! the debugger's breakpoint and display numbers.  Hence, we impose
! a limit: Breakpoints or displays with numbers higher than 99 get
! re-assigned new numbers from 1 on when restoring state.
!
! Setting these resources to 0 ensures a re-assignment of numbers
! upon each restart.
Ddd*maxBreakpointNumber: 99
Ddd*maxDisplayNumber:    99


! Themes

! The theme to apply to delete a value from a display
Ddd*suppressTheme: suppress.vsl


! Tips of the day

! Whether to show a tip of the day upon startup
Ddd*startupTips: on

! Which tip to start with.  This is usually overridden by `~/.ddd/tips'.
Ddd*startupTipCount: 1


! Window Manager Stuff

! Enable DDD selections in the CDE window manager.
Ddd*enableBtn1Transfer: False


!-----------------------------------------------------------------------------
! Colors
!-----------------------------------------------------------------------------

! Rationale for color usage:
!
! * BACKGROUND_COLOR (grey) is the default DDD background color, because 
!   this works well with all possible color schemes.  It also gives 
!   DDD a professional look.
!
! * STOP_COLOR (red4) is used for all objects associated with
!   stopping the debugged program (red traffic light).  This applies
!   to breakpoints (when activated) as well as everything associated
!   with signal handling (interrupt button, signal arrow).
!
! * RUN_COLOR (DarkGreen) is used for all objects associated
!   with running the debugged program (green traffic light).  This applies
!   to the current execution position as well as to the run button, but
!   also to the `GDB busy' indicator (meaning that the program is running.)
!
! * DATA_COLOR (blue4) is used for objects associated with references
!   (blue is the default color for links in popular WWW browswers).  This
!   applies to pointers and edges in the data display.

Ddd*foreground:			   FOREGROUND_COLOR
Ddd*background:			   BACKGROUND_COLOR
Ddd*XmText.background:		   TEXT_BACKGROUND_COLOR
Ddd*XmTextField.background:	   TEXT_BACKGROUND_COLOR
Ddd*GraphEdit.background:	   TEXT_BACKGROUND_COLOR
Ddd*XmList.background:		   TEXT_BACKGROUND_COLOR
Ddd*graph_edit_panner.background:  BACKGROUND_COLOR
Ddd*graph_edit_panner.foreground:  BACKGROUND_COLOR
Ddd*graph_edit_panner.shadowColor: FOREGROUND_COLOR

! Red, green and blue buttons.
Ddd*break.foreground:		   STOP_COLOR
Ddd*quit.foreground:		   STOP_COLOR
Ddd*send.foreground:		   STOP_COLOR
Ddd*run.foreground:		   RUN_COLOR
Ddd*run_again.foreground:	   RUN_COLOR
! Ddd*dereference.foreground:	   DATA_COLOR

! Highlight special dialogs
Ddd*maintenance.foreground:	   STOP_COLOR
Ddd*fatal_dialog*foreground:	   STOP_COLOR
Ddd*terminated_dialog*foreground:  STOP_COLOR
Ddd*exception_dialog*foreground:   STOP_COLOR
Ddd*exited_dialog*foreground:      STOP_COLOR
Ddd*no_debugger_dialog*foreground: STOP_COLOR

! Glyph colors
Ddd*plain_arrow.foreground:  RUN_COLOR
Ddd*grey_arrow.foreground:   RUN_COLOR
Ddd*past_arrow.foreground:   RUN_COLOR
Ddd*signal_arrow.foreground: STOP_COLOR

Ddd*plain_stop.foreground:   STOP_COLOR
Ddd*plain_cond.foreground:   STOP_COLOR
Ddd*plain_temp.foreground:   STOP_COLOR

Ddd*grey_stop.foreground:    DISABLED_COLOR
Ddd*grey_cond.foreground:    DISABLED_COLOR
Ddd*grey_temp.foreground:    DISABLED_COLOR

! Note: In Motif 1.2 and later, colors of dragged glyphs 
! (drag_...) are copied from the drag source.
Ddd*drag_arrow.foreground:   RUN_COLOR
Ddd*drag_stop.foreground:    STOP_COLOR
Ddd*drag_cond.foreground:    STOP_COLOR
Ddd*drag_temp.foreground:    STOP_COLOR

! Graph colors
Ddd*graph_edit.nodeColor:	FOREGROUND_COLOR
Ddd*graph_edit.nodePrintColor:	FOREGROUND_COLOR
Ddd*graph_edit.edgeColor:	DATA_COLOR
Ddd*graph_edit.edgePrintColor:	DATA_COLOR
Ddd*graph_edit.selectColor:	FOREGROUND_COLOR
Ddd*graph_edit.gridColor:	FOREGROUND_COLOR
Ddd*graph_edit.frameColor:	DISABLED_COLOR
Ddd*graph_edit.outlineColor:	DISABLED_COLOR

! For colors of nodes in the data display, see below.


!-----------------------------------------------------------------------------
! Bitmaps
!-----------------------------------------------------------------------------

! These bitmap names are predefined by DDD.  Feel free to use others.
! Note: if `cacheGlyphImages' is set, these bitmap names are ignored.

Ddd*plain_arrow.labelPixmap:	plain_arrow
Ddd*grey_arrow.labelPixmap:	grey_arrow
Ddd*past_arrow.labelPixmap:	past_arrow
Ddd*temp_arrow.labelPixmap:	temp_arrow
Ddd*signal_arrow.labelPixmap:	signal_arrow

Ddd*plain_stop.labelPixmap:	plain_stop
Ddd*plain_cond.labelPixmap:	plain_cond
Ddd*plain_temp.labelPixmap:	plain_temp

Ddd*drag_stop.labelPixmap:	drag_stop
Ddd*drag_cond.labelPixmap:	drag_cond
Ddd*drag_temp.labelPixmap:	drag_temp

Ddd*grey_stop.labelPixmap:	grey_stop
Ddd*grey_cond.labelPixmap:	grey_cond
Ddd*grey_temp.labelPixmap:	grey_temp


!-----------------------------------------------------------------------------
! Data display fonts and colors
!-----------------------------------------------------------------------------

! The value of the `vslDefs' resource are appended to the builtin VSL library.
! The general pattern to replace a function definition FUNCTION with a new
! definition NEW_DEF is:
!
! #pragma replace FUNCTION
! FUNCTION(ARGS...) = NEW_DEF;
!
! The following settings may be useful:
! 
! - color(BOX, FOREGROUND, BACKGROUND) sets the FOREGROUND and BACKGROUND
!   colors of BOX.
! - stdfontfamily() is the font family used: one of
!   family_times(), family_courier(), family_helvetica(), 
!   family_new_century(), family_typewriter().
! - stdfontsize() is the font size in pixels.
!   0 means to use stdfontpoints() instead.
! - stdfontpoints() is the font size in 1/10 points.
!   0 means to use stdfontsize() instead.
! - stdfontweight() is the weight used: one of
!   weight_medium(), weight_bold().
!
! Here is an example which you can copy to `~/.ddd/init' and customize:
!
! Ddd*vslDefs: \
! #pragma replace display_color   \n\
! #pragma replace title_color	   \n\
! #pragma replace disabled_color  \n\
! #pragma replace simple_color	   \n\
! #pragma replace pointer_color   \n\
! #pragma replace struct_color	   \n\
! #pragma replace array_color	   \n\
! #pragma replace reference_color \n\
! \n\
! display_color(box)   = color(box, "black", "white");	\n\
! title_color(box)     = color(box, "black");		\n\
! disabled_color(box)  = color(box, "white", "grey50");	\n\
! simple_color(box)    = color(box, "black");		\n\
! pointer_color(box)   = color(box, "blue4");		\n\
! struct_color(box)    = color(box, "black");		\n\
! array_color(box)     = color(box, "blue4");		\n\
! reference_color(box) = color(box, "blue4");		\n\
! \n\
! #pragma replace stdfontfamily\n\
! #pragma replace stdfontsize	\n\
! #pragma replace stdfontpoints\n\
! #pragma replace stdfontweight\n\
! stdfontfamily() = family_typewriter();\n\
! stdfontsize()   = 0;	\n\
! stdfontpoints() = 90;	\n\
! stdfontweight() = weight_medium();

! See the file `ddd.vsl' for further definitions to override here.


!-----------------------------------------------------------------------------
! Keyboard translations
!-----------------------------------------------------------------------------

! `Doesn't all this crap just make you want to hurl?'
! 
! -- JAMIE ZAWINSKI <jwz@netscape.com>, in the Netscape app-defaults file

Ddd*defaultVirtualBindings: \
osfBackSpace:	<Key>BackSpace\n\
osfCancel:	<Key>Escape\n\
osfRight: 	<Key>Right\n\
osfLeft:	<Key>Left\n\
osfUp:		<Key>Up\n\
osfHelp:	<Key>HELP_KEY\n\
osfDown:	<Key>Down\n\
osfPageUp:	<Key>Prior\n\
osfPageDown:	<Key>Next\n\
osfBeginLine:	<Key>Home\n\
osfEndLine:	<Key>End\n

dnl These command shortcuts were used in DDD 3.0,
dnl before being bound to function keys.
define(SHORTCUTS,[\
Shift Ctrl<Key>E:     	gdb-command(run)\n\
Shift Ctrl<Key>K:       gdb-command(kill)\n\
Ctrl<Key>bracketleft:   gdb-command(until)\n\
Ctrl<Key>bracketright:  gdb-command(finish)\n\
~Shift Ctrl<Key>I:      gdb-command(step)\n\
Shift Ctrl<Key>I:       gdb-command(stepi)\n\
~Shift Ctrl<Key>J:      gdb-command(next)\n\
Shift Ctrl<Key>J:       gdb-command(nexti)\n\
~Shift Ctrl<Key>T:      gdb-command(cont)\n\
Shift Ctrl<Key>T:       gdb-command(signal 0)\n])dnl

! The Debugger Console uses an EMACS-like editing style.
! `R7' and `R13' are generated by the developer's SUNs for `Home' and `End'
Ddd*gdb_w.@TRANSLATIONS@: #override\n\
SHORTCUTS \
<Btn3Down>:		gdb-popup-menu()            \n\
~Ctrl ~Shift<Btn1Up>:	source-set-arg() extend-end() \n\
~Shift Ctrl<Key>A:	ddd-select-all()	    \n\
Shift Ctrl<Key>A:	select-all()	    	    \n\
Ctrl<Key>B:		gdb-isearch-prev()    	    \n\
Ctrl<Key>C:		gdb-control(^C)		    \n\
Ctrl<Key>D:		gdb-delete-or-control(^D)   \n\
~Ctrl ~Shift<Key>Delete: delete-next-character()    \n\
~Shift Ctrl<Key>E:	gdb-end-of-line()           \n\
Ctrl<Key>F:		gdb-isearch-next()     	    \n\
Ctrl<Key>G:		gdb-control(^C)		    \n\
<Key>Break:		gdb-interrupt()		    \n\
Ctrl<Key>H:		delete-previous-character() \n\
~Shift Ctrl<Key>K:	delete-to-end-of-line()	    \n\
Ctrl<Key>N:		gdb-next-history()	    \n\
Ctrl<Key>P:		gdb-previous-history()	    \n\
Ctrl<Key>U: 		gdb-set-line("")	    \n\
Ctrl<Key>V:		next-page()		    \n\
Meta<Key>V:		previous-page()		    \n\
Ctrl<Key>W:		delete-previous-word()	    \n\
Ctrl<Key>osfBackSpace:	delete-previous-word()	    \n\
Ctrl<Key>Y:		unkill()		    \n\
Ctrl<Key>backslash:	gdb-control(^\\)	    \n\
Ctrl<Key>R:		gdb-isearch-prev()          \n\
Ctrl<Key>S:		gdb-isearch-next()  	    \n\
~Shift<Key>osfPageUp:	previous-page()		    \n\
~Shift<Key>osfPageDown:	next-page()	            \n\
~Shift<Key>Prior:	previous-page()		    \n\
~Shift<Key>Next:	next-page()	            \n\
Shift<Key>osfPageUp:	previous-page(extend)	    \n\
Shift<Key>osfPageDown:	next-page(extend)           \n\
Shift<Key>Prior:	previous-page(extend)	    \n\
Shift<Key>Next:		next-page(extend)	    \n\
~Ctrl Shift<Key>Delete:  cut-clipboard()            \n\
~Shift Ctrl<Key>Insert:  copy-clipboard()           \n\
~Ctrl Shift<Key>Insert:  paste-clipboard()          \n\
~Ctrl ~Shift<Key>Insert: toggle-overstrike()        \n\
~Ctrl ~Shift ~Meta<Key>Escape:		gdb-interrupt()         \n\
~Ctrl ~Shift ~Meta<Key>osfCancel:	gdb-interrupt()         \n\
~Ctrl ~Shift ~Meta<Key>osfUp:		gdb-previous-history()	\n\
~Ctrl ~Shift ~Meta<Key>osfDown:		gdb-next-history()	\n\
~Ctrl ~Shift ~Meta<Key>osfLeft:		gdb-backward-character()\n\
~Ctrl ~Shift ~Meta<Key>osfRight:	gdb-forward-character()	\n\
~Ctrl ~Shift ~Meta<Key>osfBeginLine:	gdb-beginning-of-line()	\n\
~Ctrl ~Shift ~Meta<Key>osfEndLine:	gdb-end-of-line()	\n\
~Ctrl ~Shift ~Meta<Key>osfBeginData:	gdb-beginning-of-line()	\n\
~Ctrl ~Shift ~Meta<Key>osfEndData:	gdb-end-of-line()	\n\
~Ctrl ~Shift ~Meta<Key>Home:		gdb-beginning-of-line()	\n\
~Ctrl ~Shift ~Meta<Key>End:		gdb-end-of-line()	\n\
~Ctrl ~Shift ~Meta<Key>R7:		gdb-beginning-of-line()	\n\
~Ctrl ~Shift ~Meta<Key>R13:		gdb-end-of-line()	\n\
~Ctrl ~Shift ~Meta<Key>Tab:		gdb-complete-command()	\n

dnl Basic translations for all other texts and text fields
define(EMACS_TRANSLATIONS, [SHORTCUTS \
~Shift Ctrl<Key>A:	ddd-select-all()	    \n\
Shift Ctrl<Key>A:	select-all()	    	    \n\
Ctrl<Key>B:		gdb-backward-character()    \n\
Ctrl<Key>D:		delete-next-character()     \n\
~Ctrl ~Shift<Key>Delete:delete-next-character()     \n\
Ctrl<Key>E:		end-of-line()               \n\
Ctrl<Key>F:		gdb-forward-character()	    \n\
Ctrl<Key>G:		gdb-control(^C)		    \n\
<Key>Break:		gdb-interrupt()		    \n\
Ctrl<Key>H:		delete-previous-character() \n\
~Shift Ctrl<Key>K:	delete-to-end-of-line()	    \n\
Ctrl<Key>U: 		beginning-of-line()	    \
			delete-to-end-of-line()	    \n\
Ctrl<Key>W:		delete-previous-word()	    \n\
Ctrl<Key>osfBackSpace:	delete-previous-word()	    \n\
~Shift <Key>R7:		beginning-of-line()	    \n\
~Shift <Key>R13:	end-of-line()		    \n\
~Shift <Key>Home:	beginning-of-line()	    \n\
~Shift <Key>End:	end-of-line()		    \n\
<Key>Escape:	        gdb-interrupt()             \n\
<Key>osfCancel:         gdb-interrupt()             \n\
~Ctrl ~Shift<Key>osfBeginLine: beginning-of-line()  \n\
~Ctrl ~Shift<Key>osfEndLine:   end-of-line()        \n\
~Ctrl ~Shift<Key>osfBeginData: beginning-of-line()  \n\
~Ctrl ~Shift<Key>osfEndData:   end-of-line()        \n\
~Ctrl Shift<Key>Delete:  cut-clipboard()            \n\
~Shift Ctrl<Key>Insert:  copy-clipboard()           \n\
~Ctrl Shift<Key>Insert:  paste-clipboard()          \n\
~Ctrl ~Shift<Key>Insert: toggle-overstrike()        \n])dnl
dnl
dnl Emacs + TAB completion.
define(COMPLETE_TRANSLATIONS, [EMACS_TRANSLATIONS \
~Shift <Key>Tab:	gdb-complete-tab($1)	    \n])dnl
dnl
dnl Wheel mouse translations.  In XFree86, moving the wheel up sends 
dnl fake button4 events and button5 ones on down motions.
dnl See `http://www.inria.fr/koala/colas/mouse-wheel-scroll/' for a discussion.
define(WHEEL_TRANSLATIONS,[\
~Shift Ctrl<Btn4Down>,<Btn4Up>:		previous-page()\n\
~Ctrl Shift<Btn4Down>,<Btn4Up>:		previous-line()\n\
~Shift ~Ctrl<Btn4Down>,<Btn4Up>:	previous-line()\
					previous-line()\
					previous-line()\
					previous-line()\
					previous-line()\n\
~Shift Ctrl<Btn5Down>,<Btn5Up>:		next-page()\n\
~Ctrl Shift<Btn5Down>,<Btn5Up>:		next-line()\n\
~Shift ~Ctrl<Btn5Down>,<Btn5Up>:	next-line()\
					next-line()\
					next-line()\
					next-line()\
					next-line()\n])dnl
dnl
dnl
dnl Emacs + extra pageup/pagedown and newline capabilities + wheel
define(TEXT_TRANSLATIONS, [EMACS_TRANSLATIONS WHEEL_TRANSLATIONS \
Ctrl<Key>M:		newline()                   \n\
Ctrl<Key>N:		gdb-next-history()	    \n\
Ctrl<Key>P:		gdb-previous-history()      \n\
Meta<Key>V:		previous-page()		    \n\
Ctrl<Key>V:		next-page()		    \n\
~Shift <Key>Prior:	previous-page()		    \n\
~Shift <Key>Next:	next-page()	            \n\
Shift<Key>osfPageUp:	previous-page(extend)	    \n\
Shift<Key>osfPageDown:	next-page(extend)           \n\
Shift<Key>Prior:	previous-page(extend)	    \n\
Shift<Key>Next:		next-page(extend)	    \n])dnl
dnl
dnl Emacs + pageup/pagedown + TAB completion.
define(COMPLETE_TEXT_TRANSLATIONS, [TEXT_TRANSLATIONS] [\
~Shift <Key>Tab:	gdb-complete-tab($1)	    \n])dnl
dnl
dnl In dialogs, we can always use KDE Cut/Copy/Paste bindings.
define(CLIPBOARD_TRANSLATIONS, [\
~Shift Ctrl<Key>X:      cut-clipboard()             \n\
~Shift Ctrl<Key>C:      copy-clipboard()            \n\
~Shift Ctrl<Key>V:      paste-clipboard()           \n])dnl

! Have some of these in argument fields as well

Ddd*XmText.@TRANSLATIONS@:      \
#override\n COMPLETE_TEXT_TRANSLATIONS(print)

Ddd*XmTextField.@TRANSLATIONS@: \
#override\n COMPLETE_TRANSLATIONS(print)

! In breakpoint dialogs, use a `break' completion
Ddd*new_breakpoint_dialog*XmTextField.@TRANSLATIONS@: \
#override\n COMPLETE_TRANSLATIONS(break) CLIPBOARD_TRANSLATIONS

! In watchpoint dialogs, use a `print' completion
Ddd*new_watchpoint_dialog*XmTextField.@TRANSLATIONS@: \
#override\n COMPLETE_TRANSLATIONS(print) CLIPBOARD_TRANSLATIONS


! In file dialogs, use a `file' completion
Ddd*XmFileSelectionBox*XmTextField.@TRANSLATIONS@: \
#override\n COMPLETE_TRANSLATIONS(file) CLIPBOARD_TRANSLATIONS


! In `print', `make', and `cd' dialogs, use a `file' completion, too.
Ddd*print*XmTextField.@TRANSLATIONS@:        	   \
#override\n COMPLETE_TRANSLATIONS(file) CLIPBOARD_TRANSLATIONS

Ddd*make_dialog*XmTextField.@TRANSLATIONS@:        \
#override\n COMPLETE_TRANSLATIONS(file) CLIPBOARD_TRANSLATIONS

Ddd*cd_dialog*XmTextField.@TRANSLATIONS@:          \
#override\n COMPLETE_TRANSLATIONS(file) CLIPBOARD_TRANSLATIONS

! In selection boxes, use `shell' completion.
Ddd*XmSelectionBox*XmTextField.@TRANSLATIONS@:     \
#override\n COMPLETE_TRANSLATIONS(shell) CLIPBOARD_TRANSLATIONS
Ddd*XmDialogShell*XmTextField.@TRANSLATIONS@:     \
#override\n COMPLETE_TRANSLATIONS(shell) CLIPBOARD_TRANSLATIONS

! In the button editor, we have no completion.
Ddd*edit_buttons*XmText.@TRANSLATIONS@: 	 	 \
#override\n TEXT_TRANSLATIONS CLIPBOARD_TRANSLATIONS

! In the theme editor, we have no completion.
Ddd*themes*XmTextField.@TRANSLATIONS@: 	 	 \
#override\n EMACS_TRANSLATIONS CLIPBOARD_TRANSLATIONS

! In command definitions, we have command completion.
Ddd*breakpoint_properties*XmTextField.@TRANSLATIONS@: \
#override\n COMPLETE_TRANSLATIONS(print) CLIPBOARD_TRANSLATIONS

Ddd*define_command*XmTextField.@TRANSLATIONS@:        \
#override\n COMPLETE_TRANSLATIONS(" ") CLIPBOARD_TRANSLATIONS



! The source window is especially tuned for word selection.
define(SOURCE_TRANSLATIONS,[\
<Btn3Down>:		source-popup-menu()         \n\
~Shift<Btn1Down>:	source-start-select-word()  \n\
~Shift<Btn1Up>:		source-end-select-word() source-double-click() \n\
~Shift Ctrl<Key>A:	ddd-select-all()	    \n\
Shift Ctrl<Key>A:	select-all()	    	    \n\
Ctrl<Key>B:		gdb-isearch-prev()    	    \n\
Ctrl<Key>C:             gdb-control(^C)             \n\
Ctrl<Key>E:             end-of-line()               \n\
Ctrl<Key>F:		gdb-isearch-next()	    \n\
Ctrl<Key>G:             gdb-control(^C)             \n\
Ctrl<Key>N:		gdb-next-history()	    \n\
Ctrl<Key>P:		gdb-previous-history()      \n\
Ctrl<Key>R:		gdb-isearch-prev()          \n\
Ctrl<Key>S:		gdb-isearch-next()  	    \n\
Ctrl<Key>V:		next-page()		    \
			source-update-glyphs()	    \n\
Meta<Key>V:		previous-page()		    \
			source-update-glyphs()	    \n\
~Ctrl Shift<Key>Delete:  cut-clipboard()            \n\
~Shift Ctrl<Key>Insert:  copy-clipboard()           \n\
~Ctrl Shift<Key>Insert:  paste-clipboard()          \n\
~Ctrl ~Shift<Key>Insert: toggle-overstrike()        \n\
~Shift <Key>R7:		beginning-of-line()	    \n\
~Shift <Key>R13:	end-of-line()		    \n\
~Shift <Key>Tab:	PrimitiveNextTabGroup()	    \n\
~Shift <Key>osfPageUp:	previous-page()		    \
			source-update-glyphs()	    \n\
~Shift <Key>osfPageDown: next-page()		    \
			source-update-glyphs()	    \n\
Shift<Key>osfPageUp:	previous-page(extend)	    \
			source-update-glyphs()	    \n\
Shift<Key>osfPageDown:	next-page(extend)	    \
			source-update-glyphs()	    \n\
~Shift <Key>Prior:	previous-page()		    \
			source-update-glyphs()	    \n\
~Shift <Key>Next:	next-page()		    \
			source-update-glyphs()	    \n\
Shift<Key>Prior:	previous-page(extend)	    \
			source-update-glyphs()	    \n\
Shift<Key>Next:		next-page(extend)	    \
			source-update-glyphs()	    \n\
~Shift <Key>Home:	beginning-of-line()	    \n\
~Shift <Key>End:	end-of-line()		    \n])dnl

! Have all characters be processed by the GDB console.
! If you know of a better way than by using such huge translation tables,
! please let me know.
define(GDB_TRANSLATIONS,[\
~Meta<Key>osfBackSpace:		     gdb-process(delete-previous-character)\n\
~Ctrl ~Meta<Key>BackSpace:	     gdb-process(delete-previous-character)\n\
~Meta<Key>Tab:			     gdb-process(process-tab)\n\
~Ctrl ~Meta<Key>Return:		     gdb-process(process-return)\n\
~Ctrl ~Meta<Key>Linefeed:	     gdb-process(process-return)\n\
~Ctrl ~Meta<Key>Escape:		     gdb-interrupt()\n\
~Meta<Key>osfCancel:		     gdb-interrupt()\n\
~Meta<Key>Delete:		     gdb-process(delete-next-character)\n\
~Ctrl ~Meta<Key>space:		     gdb-process()\n\
~Ctrl ~Meta<Key>exclam:		     gdb-process()\n\
~Ctrl ~Meta<Key>quotedbl:	     gdb-process()\n\
~Ctrl ~Meta<Key>numbersign:	     gdb-process()\n\
~Ctrl ~Meta<Key>dollar:		     gdb-process()\n\
~Ctrl ~Meta<Key>percent:	     gdb-process()\n\
~Ctrl ~Meta<Key>ampersand:	     gdb-process()\n\
~Ctrl ~Meta<Key>apostrophe:	     gdb-process()\n\
~Ctrl ~Meta<Key>parenleft:	     gdb-process()\n\
~Ctrl ~Meta<Key>parenright:	     gdb-process()\n\
~Ctrl ~Meta<Key>asterisk:	     gdb-process()\n\
~Ctrl ~Meta<Key>plus:		     gdb-process()\n\
~Ctrl ~Meta<Key>comma:		     gdb-process()\n\
~Ctrl ~Meta<Key>minus:		     gdb-process()\n\
~Ctrl ~Meta<Key>period:		     gdb-process()\n\
~Ctrl ~Meta<Key>slash:		     gdb-process()\n\
~Ctrl ~Meta<Key>0:		     gdb-process()\n\
~Ctrl ~Meta<Key>1:		     gdb-process()\n\
~Ctrl ~Meta<Key>2:		     gdb-process()\n\
~Ctrl ~Meta<Key>3:		     gdb-process()\n\
~Ctrl ~Meta<Key>4:		     gdb-process()\n\
~Ctrl ~Meta<Key>5:		     gdb-process()\n\
~Ctrl ~Meta<Key>6:		     gdb-process()\n\
~Ctrl ~Meta<Key>7:		     gdb-process()\n\
~Ctrl ~Meta<Key>8:		     gdb-process()\n\
~Ctrl ~Meta<Key>9:		     gdb-process()\n\
~Ctrl ~Meta<Key>colon:		     gdb-process()\n\
~Ctrl ~Meta<Key>semicolon:	     gdb-process()\n\
~Ctrl ~Meta<Key>less:		     gdb-process()\n\
~Ctrl ~Meta<Key>equal:		     gdb-process()\n\
~Ctrl ~Meta<Key>greater:	     gdb-process()\n\
~Ctrl ~Meta<Key>question:	     gdb-process()\n\
~Ctrl ~Meta<Key>at:		     gdb-process()\n\
~Ctrl ~Meta<Key>A:		     gdb-process()\n\
~Ctrl ~Meta<Key>B:		     gdb-process()\n\
~Ctrl ~Meta<Key>C:		     gdb-process()\n\
~Ctrl ~Meta<Key>D:		     gdb-process()\n\
~Ctrl ~Meta<Key>E:		     gdb-process()\n\
~Ctrl ~Meta<Key>F:		     gdb-process()\n\
~Ctrl ~Meta<Key>G:		     gdb-process()\n\
~Ctrl ~Meta<Key>H:		     gdb-process()\n\
~Ctrl ~Meta<Key>I:		     gdb-process()\n\
~Ctrl ~Meta<Key>J:		     gdb-process()\n\
~Ctrl ~Meta<Key>K:		     gdb-process()\n\
~Ctrl ~Meta<Key>L:		     gdb-process()\n\
~Ctrl ~Meta<Key>M:		     gdb-process()\n\
~Ctrl ~Meta<Key>N:		     gdb-process()\n\
~Ctrl ~Meta<Key>O:		     gdb-process()\n\
~Ctrl ~Meta<Key>P:		     gdb-process()\n\
~Ctrl ~Meta<Key>Q:		     gdb-process()\n\
~Ctrl ~Meta<Key>R:		     gdb-process()\n\
~Ctrl ~Meta<Key>S:		     gdb-process()\n\
~Ctrl ~Meta<Key>T:		     gdb-process()\n\
~Ctrl ~Meta<Key>U:		     gdb-process()\n\
~Ctrl ~Meta<Key>V:		     gdb-process()\n\
~Ctrl ~Meta<Key>W:		     gdb-process()\n\
~Ctrl ~Meta<Key>X:		     gdb-process()\n\
~Ctrl ~Meta<Key>Y:		     gdb-process()\n\
~Ctrl ~Meta<Key>Z:		     gdb-process()\n\
~Ctrl ~Meta<Key>bracketleft:	     gdb-process()\n\
~Ctrl ~Meta<Key>backslash:	     gdb-process()\n\
~Ctrl ~Meta<Key>bracketright:	     gdb-process()\n\
~Ctrl ~Meta<Key>asciicircum:	     gdb-process()\n\
~Ctrl ~Meta<Key>underscore:	     gdb-process()\n\
~Ctrl ~Meta<Key>grave:		     gdb-process()\n\
~Ctrl ~Meta<Key>a:		     gdb-process()\n\
~Ctrl ~Meta<Key>b:		     gdb-process()\n\
~Ctrl ~Meta<Key>c:		     gdb-process()\n\
~Ctrl ~Meta<Key>d:		     gdb-process()\n\
~Ctrl ~Meta<Key>e:		     gdb-process()\n\
~Ctrl ~Meta<Key>f:		     gdb-process()\n\
~Ctrl ~Meta<Key>g:		     gdb-process()\n\
~Ctrl ~Meta<Key>h:		     gdb-process()\n\
~Ctrl ~Meta<Key>i:		     gdb-process()\n\
~Ctrl ~Meta<Key>j:		     gdb-process()\n\
~Ctrl ~Meta<Key>k:		     gdb-process()\n\
~Ctrl ~Meta<Key>l:		     gdb-process()\n\
~Ctrl ~Meta<Key>m:		     gdb-process()\n\
~Ctrl ~Meta<Key>n:		     gdb-process()\n\
~Ctrl ~Meta<Key>o:		     gdb-process()\n\
~Ctrl ~Meta<Key>p:		     gdb-process()\n\
~Ctrl ~Meta<Key>q:		     gdb-process()\n\
~Ctrl ~Meta<Key>r:		     gdb-process()\n\
~Ctrl ~Meta<Key>s:		     gdb-process()\n\
~Ctrl ~Meta<Key>t:		     gdb-process()\n\
~Ctrl ~Meta<Key>u:		     gdb-process()\n\
~Ctrl ~Meta<Key>v:		     gdb-process()\n\
~Ctrl ~Meta<Key>w:		     gdb-process()\n\
~Ctrl ~Meta<Key>x:		     gdb-process()\n\
~Ctrl ~Meta<Key>y:		     gdb-process()\n\
~Ctrl ~Meta<Key>z:		     gdb-process()\n\
~Ctrl ~Meta<Key>braceleft:	     gdb-process()\n\
~Ctrl ~Meta<Key>bar:		     gdb-process()\n\
~Ctrl ~Meta<Key>braceright:	     gdb-process()\n\
~Ctrl ~Meta<Key>asciitilde:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_0:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_1:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_2:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_3:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_4:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_5:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_6:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_7:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_8:	     gdb-process()\n\
~Ctrl ~Meta ~Shift<Key>KP_9:	     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Space:	     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Tab:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Enter:	     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Equal:	     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Multiply:	     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Add:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Separator:	     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Subtract:	     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Decimal:	     gdb-process()\n\
~Ctrl ~Meta<Key>KP_Divide:	     gdb-process()\n\
~Meta Ctrl<Key>C:		     gdb-process()\n\
~Meta Ctrl<Key>D:		     gdb-process(process-delete)\n\
~Meta Ctrl<Key>G:		     gdb-process()\n\
~Meta Ctrl ~Shift<Key>K:	     gdb-process(delete-to-end-of-line)\n\
~Meta Ctrl<Key>W:		     gdb-process(delete-previous-word)\n\
~Meta Ctrl<Key>U:		     gdb-process()\n])dnl

Ddd*source_text_w.@TRANSLATIONS@: \
#override\n SOURCE_TRANSLATIONS GDB_TRANSLATIONS TEXT_TRANSLATIONS
Ddd*code_text_w.@TRANSLATIONS@:   \
#override\n SOURCE_TRANSLATIONS GDB_TRANSLATIONS TEXT_TRANSLATIONS

! Realize glyph drag and drop as well as glyph menu popups
define(GLYPH_TRANSLATIONS,[\
<Btn1Down>:	  ArmAndActivate() source-drag-glyph() \n\
<Btn1Motion>: 	  source-follow-glyph()   \n\
~Shift <Btn1Up>:  source-drop-glyph(move) \n\
Shift <Btn1Up>:   source-drop-glyph(copy) \n\
<Btn2Down>:	  source-drag-glyph()	  \n\
<Btn2Motion>: 	  source-follow-glyph()   \n\
~Shift <Btn2Up>:  source-drop-glyph(move) \n\
Shift <Btn2Up>:   source-drop-glyph(copy) \n\
<Btn3Down>:	  source-popup-menu()     \n])dnl

dnl This uses too much space:
dnl Ddd*source_form_w.XmPushButton.@TRANSLATIONS@: \
dnl #override\n GLYPH_TRANSLATIONS GDB_TRANSLATIONS
dnl Ddd*code_form_w.XmPushButton.@TRANSLATIONS@:   \
dnl #override\n GLYPH_TRANSLATIONS GDB_TRANSLATIONS
dnl
dnl Use this instead:
Ddd*source_form_w.XmPushButton.@TRANSLATIONS@: #override\n GLYPH_TRANSLATIONS
Ddd*code_form_w.XmPushButton.@TRANSLATIONS@:   #override\n GLYPH_TRANSLATIONS

! This defines the popup menus in the data window.
define(GRAPH_TRANSLATIONS,[\
~Shift<Btn1Down>: graph-select-or-move()\n\
Shift<Btn1Down>:  graph-toggle-or-move()\n\
<Btn2Down>:	  graph-toggle-or-move()\n\
<Btn3Down>:	  graph-select() graph-popup-menu()\n])dnl

Ddd*graph_edit.@TRANSLATIONS@: #override\n GRAPH_TRANSLATIONS GDB_TRANSLATIONS

dnl The following would forward all keyboard events to the debugger console,
dnl but it does not work for Motif 1.1
dnl Ddd*translations: \
dnl #augment\n
dnl GDB_TRANSLATIONS
dnl 
dnl An alternative would be to define translations for all labels 
dnl and several buttons.  The following might work:
dnl define(BUTTON_TRANSLATIONS,[\
dnl ~Ctrl<Btn1Down>:	Arm()\n\
dnl ~Ctrl<Btn1Up>:        Activate() Disarm()\n])dnl
dnl
dnl Ddd*tool_shell*XmPushButton.@TRANSLATIONS@: \
dnl #augment\n BUTTON_TRANSLATIONS GDB_TRANSLATIONS
dnl Ddd*toolbar*XmPushButton.@TRANSLATIONS@: \
dnl #augment\n BUTTON_TRANSLATIONS GDB_TRANSLATIONS
dnl Ddd*source.buttons*XmPushButton.@TRANSLATIONS@: \
dnl #augment\n BUTTON_TRANSLATIONS GDB_TRANSLATIONS
dnl
dnl Ddd*XmLabel.@TRANSLATIONS@: \
dnl #augment\n\
dnl GDB_TRANSLATIONS


!-----------------------------------------------------------------------------
! Combo Boxes
!-----------------------------------------------------------------------------

! The default number of visible items in a combo box
Ddd*XmComboBox*visibleItemCount:    10
Ddd*comboBoxShell*visibleItemCount: 10

! In dialogs, have less items
Ddd*XmSelectionBox*XmComboBox*visibleItemCount:    5
Ddd*XmSelectionBox*comboBoxShell*visibleItemCount: 5

! Have somewhat lighter combo box arrows
Ddd*XmComboBox.arrowSize:  15
Ddd*comboBoxArrow.width:   12


!-----------------------------------------------------------------------------
! Spin Boxes
!-----------------------------------------------------------------------------

! Have somewhat lighter spin box arrows
Ddd*spinBoxArrow.width:   10


!-----------------------------------------------------------------------------
! Titles
!-----------------------------------------------------------------------------

! Generic title
Ddd*title:				DDD Window

! Main Window
Ddd.title:				DDD: The Data Display Debugger
Ddd.iconName:				DDD

! Separate windows
Ddd*command_shell.title:		DDD: Debugger Console
Ddd*command_shell.iconName:		DDD: Console

Ddd*source_view_shell.title:		DDD: Program Source
Ddd*source_view_shell.iconName:		DDD: Source

Ddd*data_disp_shell.title:		DDD: Program Data
Ddd*data_disp_shell.iconName:		DDD: Data

! Command tool
Ddd*tool_shell.title:			DDD
Ddd*tool_shell.iconName:		DDD: Command Tool

! Default for everything else
Ddd*title:				DDD Dialog
Ddd*iconName:				DDD Dialog



!-----------------------------------------------------------------------------
! MWM Decorations
!-----------------------------------------------------------------------------

! Don't place resize handles on dialogs
Ddd*XmDialogShell.mwmDecorations: 26

! Exception: the manual and license browsers.
Ddd*manual_help_popup.mwmDecorations: 1
Ddd*license_popup.mwmDecorations:     1
Ddd*news_popup.mwmDecorations:        1


!-----------------------------------------------------------------------------
! Help
!-----------------------------------------------------------------------------

! Make help button a little wider
Ddd*helpLabelString: \ \ \ Help\ \ \ 

! Set this to `on' to view widget names in help texts.
Ddd*helpShowTitle: off

! Help is never acknowledged, always closed.
Ddd*help.okLabelString: 	Close
Ddd*license.okLabelString: 	Close
Ddd*news.okLabelString: 	Close
Ddd*manual_help.okLabelString: 	Close
Ddd*tip_dialog.okLabelString: 	Close

! Titles
Ddd*help.dialogTitle:           DDD Help
Ddd*manual_help.title:    	DDD Reference
Ddd*manual_help.iconName: 	DDD Reference
Ddd*license.title:        	DDD License
Ddd*license.iconName:     	DDD License
Ddd*news.title:           	DDD News
Ddd*news.iconName:     		DDD News

! Icons
Ddd*help.symbolPixmap: ddd

! Help on help.
Ddd*help*helpString: \
@rm Welcome to DDD Help\!\n\
\n\
To get help about an item, just point on it and press KEY_HELP.\n\
The item's functionality will be explained here.\n\
\n\
To find out what you can do next, press KEY(Ctrl+HELP_KEY).  Depending on\n\
the current DDD state, you will be given some general advice.\n\
\n\
Have fun with DDD!


! The official announcement.
! Build information (like `ddd --configuration') is appended to this text.
Ddd*helpOnVersionString:	\
@llogo DDD@logo: \
The @llogo D@logo ata @llogo D@logo isplay @llogo D@logo ebugger\n\
@rm by Dorothea L\374tkehaus and Andreas Zeller, maintained by Peter Wainwright.\n\n

! The default Help Text.
Ddd*helpString:	\
@rm This item has no function, or is not documented.

! Some trival help
Ddd*XmScrollBar.helpString: \
WIDGET(Scroll Bar)\n\
\n\
Click or hold BUTTON(1) on an arrow button\n\
to scroll the window in the arrow direction.\n\
\n\
Drag the slider with BUTTON(1) to select the\n\
visible part of the window.
! Ddd*XmScrollBar.tipString: \
! @rm Scroll window

Ddd*XmSash.helpString: \
WIDGET(Paned Window Sash)\n\
\n\
Drag the sash with BUTTON(1) to resize window parts.
Ddd*XmSash.tipString: \
@rm Resize window

Ddd*Separator.helpString: \
WIDGET(Paned Window Sash)\n\
\n\
Drag the sash with BUTTON(1) to resize window parts.
Ddd*Separator.tipString: \
@rm Resize window

Ddd*separator.helpString: \
WIDGET(Paned Window Sash)\n\
\n\
Drag the sash with BUTTON(1) to resize window parts.
Ddd*separator.tipString: \
@rm Resize window

Ddd*arg_label.helpString: \
WIDGET(Argument Prompt)\n\
\n\
Enter the argument LBL(()) on the right.\n\
Click on LBL(():) to clear the argument LBL(()).

Ddd*arg_label.tipString: \
@rm Clear argument LBL(())

Ddd*arg_label.documentationString: \
@rm Click to clear the argument LBL(()).


!-----------------------------------------------------------------------------
! Traversal
!-----------------------------------------------------------------------------

Ddd*arg_label.traversalOn:		off
Ddd*status_form*traversalOn:		off
Ddd*XmSash.traversalOn:			off


!-----------------------------------------------------------------------------
! DDD manual
!-----------------------------------------------------------------------------

Ddd*manual_help*title.labelString:	DDD Reference
Ddd*manual_help*title.alignment:	XmALIGNMENT_BEGINNING

Ddd*help_area.allowResize:		on

Ddd*manual_help*index.selectionPolicy:	XmSINGLE_SELECT
Ddd*manual_help*index.visibleItemCount:	5

Ddd*manual_help*text.rows:		25
Ddd*manual_help*text.wordWrap:		on
Ddd*manual_help*text.scrollHorizontal:	off

Ddd*manual_help*source.labelString:     	Reference
Ddd*manual_help*source.mnemonic:        	R

Ddd*sourceMenu*viewIndex.labelString:  		Index
Ddd*sourceMenu*viewIndex.mnemonic:     		I
Ddd*sourceMenu*viewIndex.accelerator:		Meta<Key>1
Ddd*sourceMenu*viewIndex.acceleratorText:	Alt+1
Ddd*sourceMenu*viewIndex.documentationString:\
@rm Toggle display of manual index

Ddd*help_area*findBackward.labelString: 	   LBL_FIND_BACKWARD
Ddd*help_area*findBackward.labelType:              XmPIXMAP
Ddd*help_area*findBackward.labelPixmap:            find_backward
Ddd*help_area*findBackward.labelInsensitivePixmap: find_backward-xx
Ddd*help_area*findBackward.armPixmap:              find_backward-arm
Ddd*help_area*findBackward.highlightPixmap:        find_backward-hi

Ddd*help_area*findBackward.tipString:	  \
@rm Find previous LBL(()) in text
Ddd*help_area*findBackward.documentationString:	\
@rm Search the previous occurrence of LBL(()) in the text

Ddd*help_area*findForward.labelString:  	  LBL_FIND_FORWARD
Ddd*help_area*findForward.labelType:              XmPIXMAP
Ddd*help_area*findForward.labelPixmap:            find_forward
Ddd*help_area*findForward.labelInsensitivePixmap: find_forward-xx
Ddd*help_area*findForward.armPixmap:              find_forward-arm
Ddd*help_area*findForward.highlightPixmap:        find_forward-hi

Ddd*help_area*findForward.tipString:	  \
@rm Find next LBL(()) in text
Ddd*help_area*findForward.documentationString:	\
@rm Search the next occurrence of LBL(()) in the text

Ddd*manual_help*helpString: \
WIDGET([DDD] Manual Browser)\n\
\n\
To view a specific section, select its title from the index at the top.\n\
\n\
To find a string in the manual, enter it in the LBL(()) argument field \
and press KEY_RETURN.\n\
Click on LBL_FIND_BACKWARD to search the next occurrence of LBL(()).\n\
Click on LBL_FIND_FORWARD to search the previous occurrence of LBL(()).


!-----------------------------------------------------------------------------
! DDD license and news
!-----------------------------------------------------------------------------

Ddd*license*form.borderWidth:		0

Ddd*license*title.labelString:      	DDD License
Ddd*license*title.alignment:	        XmALIGNMENT_BEGINNING

Ddd*license*source.labelString:     	License
Ddd*license*source.mnemonic:        	L

Ddd*license*text.rows:	      		25
Ddd*license*text.wordWrap:	        on
Ddd*license*text.scrollHorizontal:    	off

Ddd*license*helpString: \
WIDGET([DDD] License)\n\
\n\
The DDD License, a GNU General Public License, describes the terms\n\
and conditions for copying, distributing and modifying DDD.\n\
\n\
To find a string in the license, enter it in the LBL(()) argument field \
and press KEY_RETURN.\n\
Click on LBL_FIND_BACKWARD to search the next occurrence of LBL(()).\n\
Click on LBL_FIND_FORWARD to search the previous occurrence of LBL(()).\n\
\n\
Please see section 11, TEXT(NO WARRANTY), for the various kinds of warranty\n\
you do not have.


Ddd*news*form.borderWidth:		0

Ddd*news*title.labelString:      	DDD News
Ddd*news*title.alignment:	        XmALIGNMENT_BEGINNING

Ddd*news*source.labelString:     	News
Ddd*news*source.mnemonic:        	N

Ddd*news*text.rows:	      		25
Ddd*news*text.wordWrap:	        	on
Ddd*news*text.scrollHorizontal:    	off

Ddd*news*helpString: \
WIDGET([DDD] News)\n\
\n\
What's new in this DDD release relative to previous releases.\n\
\n\
To find a string in the text, enter it in the LBL(()) argument field \
and press KEY_RETURN.\n\
Click on LBL_FIND_BACKWARD to search the next occurrence of LBL(()).\n\
Click on LBL_FIND_FORWARD to search the previous occurrence of LBL(()).



!-----------------------------------------------------------------------------
! Button tips
!-----------------------------------------------------------------------------

! These colors are taken from Netscape.  The basic idea is that you might
! have a Netscape already running, so we won't use up another color cell.

Ddd*tipLabel.background:  TIP_COLOR
Ddd*tipLabel.foreground:  FOREGROUND_COLOR
Ddd*tipRow.background:    FOREGROUND_COLOR
Ddd*tipShell.borderColor: FOREGROUND_COLOR
Ddd*tipShell.borderWidth: 1


!-----------------------------------------------------------------------------
! Menu Bar
!-----------------------------------------------------------------------------

Ddd*menubar.helpString:		\
WIDGET(Menu Bar)\n\
\n\
DESC(File, [select files and exit DDD])\n\
DESC(Edit, [cut, copy, and paste text, and change DDD options])\n\
DESC(View, [view DDD windows])\n\
DESC(Program, [run the debugged program])\n\
DESC(Commands, [view command history])\n\
DESC(Status, [show program status (backtrace, registers, threads)])\n\
DESC(Source, [edit breakpoints and lookup previous positions])\n\
DESC(Data, [edit displays])\n\
DESC(Help, [on-line help and version information])

Ddd*menubar.marginWidth:     0
Ddd*menubar.marginHeight:    1
! Ddd*menubar.shadowThickness: 1

! File menu
Ddd*menubar.file.labelString:		File
Ddd*menubar.file*documentationString:	\
@rm Open program and source files
Ddd*menubar.file.mnemonic:		F

define(FILE_HELP, [\
WIDGET(File Menu)\n\
\n\
DESC(Open Program..., [open the program to be debugged])\n\
DESC(Open Recent, [open a recently used program])\n\
DESC(Open Core Dump..., [open a core dump])\n\
DESC(Open Source..., [open a source file])\n\
\n\
DESC(Open Session..., [open a DDD debugging session])\n\
DESC(Save Session As..., [save current debugging session])\n\
\n\
DESC(Attach to Process..., [attach to a process outside of DDD])\n\
DESC(Detach Process, [detach a process previously attached])\n\
\n\
DESC(Print Graph..., [print the data display graph])\n\
DESC(Change Directory..., [change current directory])\n\
DESC(Make..., [run the make program])\n\
\n\
DESC(Close, [close this window])\n\
DESC(Restart, [restart DDD])\n\
DESC(Exit, [exit DDD])
])dnl

Ddd*menubar.file*helpString: FILE_HELP
Ddd*fileMenu*helpString:     FILE_HELP
Ddd*fileMenu*tearOffTitle:   File

Ddd*menubar*fileMenu.open_file.labelString: Open Program...
Ddd*menubar*fileMenu.open_file.mnemonic:    O
Ddd*menubar*fileMenu.open_file.accelerator:      ~Shift Ctrl<Key>O
Ddd*menubar*fileMenu.open_file.acceleratorText:  Ctrl+O
Ddd*menubar*fileMenu.open_file.documentationString:   \
@rm Open a program to be debugged

Ddd*menubar*fileMenu.open_class.labelString: Open Class...
Ddd*menubar*fileMenu.open_class.mnemonic:    O
Ddd*menubar*fileMenu.open_class.accelerator:      ~Shift Ctrl<Key>O
Ddd*menubar*fileMenu.open_class.acceleratorText:  Ctrl+O
Ddd*menubar*fileMenu.open_class.documentationString:   \
@rm Open a class to be debugged

Ddd*menubar*fileMenu.recent.labelString: Open Recent
Ddd*menubar*fileMenu.recent.mnemonic:    R
Ddd*menubar*fileMenu.recent.documentationString:   \
@rm Reopen a program to be debugged

Ddd*menubar*fileMenu.open_core.labelString: Open Core Dump...
Ddd*menubar*fileMenu.open_core.mnemonic:    D
Ddd*menubar*fileMenu.open_core.documentationString:   \
@rm Open a file as core dump for examining memory and registers

Ddd*menubar*fileMenu.open_source.labelString: Open Source...
Ddd*menubar*fileMenu.open_source.mnemonic:    S
Ddd*menubar*fileMenu.open_source.documentationString:   \
@rm Open a source file in the source window

Ddd*menubar*fileMenu.open_session.labelString:	        Open Session...
Ddd*menubar*fileMenu.open_session.mnemonic:	        n
Ddd*menubar*fileMenu.open_session.accelerator:          ~Shift Ctrl<Key>N
Ddd*menubar*fileMenu.open_session.acceleratorText:      Ctrl+N
Ddd*menubar*fileMenu.open_session.documentationString:  \
@rm Restore a previously saved DDD session

Ddd*menubar*fileMenu.save_session.labelString:	        Save Session As...
Ddd*menubar*fileMenu.save_session.accelerator:          ~Shift Ctrl<Key>S
Ddd*menubar*fileMenu.save_session.acceleratorText:      Ctrl+S
Ddd*menubar*fileMenu.save_session.mnemonic:	        v
Ddd*menubar*fileMenu.save_session.documentationString:  \
@rm Save this DDD session for resuming later

Ddd*menubar*fileMenu.attach.labelString: Attach to Process...
Ddd*menubar*fileMenu.attach.mnemonic:    A
Ddd*menubar*fileMenu.attach.documentationString:   \
@rm Attach to a process outside of DDD

Ddd*menubar*fileMenu.detach.labelString:       Detach Process
Ddd*menubar*fileMenu.detach.mnemonic:	       t
Ddd*menubar*fileMenu.detach.documentationString:

Ddd*menubar*fileMenu.print.labelString:	 	Print Graph...
Ddd*menubar*fileMenu.print.mnemonic:	 	P
Ddd*menubar*fileMenu.print.accelerator:         ~Shift Ctrl<Key>P
Ddd*menubar*fileMenu.print.acceleratorText:     Ctrl+P
Ddd*menubar*fileMenu.print.documentationString:   \
@rm Print the contents of the data display

Ddd*menubar*fileMenu.printAgain.labelString: 	    Print Again
Ddd*menubar*fileMenu.printAgain.mnemonic: 	    g
Ddd*menubar*fileMenu.printAgain.documentationString:   \
@rm Print the contents of the data display, using previous settings

Ddd*menubar*fileMenu.cd.labelString:	 Change Directory...
Ddd*menubar*fileMenu.cd.mnemonic:	 i
Ddd*menubar*fileMenu.cd.documentationString:  \
@rm Change the current directory

Ddd*menubar*fileMenu.make.labelString:	 Make...
Ddd*menubar*fileMenu.make.mnemonic:	 M
Ddd*menubar*fileMenu.make.accelerator:      ~Shift Ctrl<Key>M
Ddd*menubar*fileMenu.make.acceleratorText:  Ctrl+M
Ddd*menubar*fileMenu.make.documentationString:  \
@rm Run the make program

Ddd*menubar*fileMenu.makeAgain.labelString:	 Make Again
Ddd*menubar*fileMenu.makeAgain.mnemonic:	 i
Ddd*menubar*fileMenu.makeAgain.accelerator:      Shift Ctrl<Key>M
Ddd*menubar*fileMenu.makeAgain.acceleratorText:  Shift+Ctrl+M
Ddd*menubar*fileMenu.makeAgain.documentationString:  \
@rm Run the make program with the most recently given arguments

Ddd*menubar*fileMenu.close.labelString:	 Close
Ddd*menubar*fileMenu.close.mnemonic:	 C
Ddd*menubar*fileMenu.close.accelerator:      ~Shift Ctrl<Key>W
Ddd*menubar*fileMenu.close.acceleratorText:  Ctrl+W
Ddd*menubar*fileMenu.close.documentationString:   \
@rm Close this window

Ddd*menubar*fileMenu.restart.labelString: Restart
Ddd*menubar*fileMenu.restart.mnemonic:	 e
Ddd*menubar*fileMenu.restart.accelerator:      Shift Ctrl<Key>Q
! Ddd*menubar*fileMenu.restart.acceleratorText:  Shift+Ctrl+Q
Ddd*menubar*fileMenu.restart.documentationString:   \
@rm Close all windows and restart DDD from scratch

Ddd*menubar*fileMenu.exit.labelString:	 Exit
Ddd*menubar*fileMenu.exit.mnemonic:	 x
Ddd*menubar*fileMenu.exit.accelerator:	    ~Shift Ctrl<Key>Q
Ddd*menubar*fileMenu.exit.acceleratorText:  Ctrl+Q
Ddd*menubar*fileMenu.exit.documentationString:   \
@rm Close all windows and exit DDD


! Recent menu
Ddd*menubar*recentMenu.r1.mnemonic: 1
Ddd*menubar*recentMenu.r2.mnemonic: 2
Ddd*menubar*recentMenu.r3.mnemonic: 3
Ddd*menubar*recentMenu.r4.mnemonic: 4
Ddd*menubar*recentMenu.r5.mnemonic: 5
Ddd*menubar*recentMenu.r6.mnemonic: 6
Ddd*menubar*recentMenu.r7.mnemonic: 7
Ddd*menubar*recentMenu.r8.mnemonic: 8
Ddd*menubar*recentMenu.r9.mnemonic: 9

! Get the file names dynamically
Ddd*menubar*recentMenu*documentationString: 


! Edit menu
Ddd*menubar.edit.labelString:		Edit
Ddd*menubar.edit.mnemonic:		E
Ddd*menubar.edit*documentationString:	\
@rm Cut, copy, paste from and to the clipboard

define(EDIT_HELP, [\
WIDGET(Edit Menu)\n\
\n\
DESC(Undo, [undo last action])\n\
DESC(Redo, [redo next action])\n\
\n\
DESC(Cut, [remove the selected text to the clipboard])\n\
DESC(Copy, [copy the selected text to the clipboard\n\
    without removing the original])\n\
DESC(Paste, [paste the clipboard contents into the text])\n\
DESC(Clear, [clear argument fields and @GDB@ command line])\n\
DESC(Delete, [delete the selected text])\n\
DESC(Select All, [select the entire text])\n\
\n\
DESC(Preferences..., [invokes a panel for setting DDD options])\n\
DESC(@GDB@ Settings..., [invokes a panel for setting @GDB@ options])\n\
\n\
DESC(Save Options, [if set, options, preferences, and @GDB@ settings\n\
    are saved across DDD invocations.])
])dnl

Ddd*menubar.edit*helpString: EDIT_HELP
Ddd*editMenu*helpString:     EDIT_HELP
Ddd*editMenu*tearOffTitle:   Edit

Ddd*editMenu.undo.labelString:				Undo
Ddd*editMenu.undo.mnemonic:				U
Ddd*editMenu.undo.accelerator:				Ctrl<Key>Z
Ddd*editMenu.undo.acceleratorText:			Ctrl+Z
Ddd*editMenu.undo.documentationString: \
@rm Undo last action

Ddd*editMenu.redo.labelString:				Redo
Ddd*editMenu.redo.mnemonic:				R
Ddd*editMenu.redo.accelerator:				Ctrl<Key>Y
Ddd*editMenu.redo.acceleratorText:			Ctrl+Y
Ddd*editMenu.redo.documentationString: \
@rm Redo next action

! Have standard Motif bindings
! Ddd*editMenu.cut.accelerator:		Shift<Key>Delete
! Ddd*editMenu.cut.acceleratorText:	Shift+Del
! Ddd*editMenu.copy.accelerator:  	Ctrl<Key>Insert
! Ddd*editMenu.copy.acceleratorText:	Ctrl+Ins
! Ddd*editMenu.paste.accelerator:  	Shift<Key>Insert
! Ddd*editMenu.paste.acceleratorText:	Shift+Ins

! Alternative, KDE-like bindings
! Ddd*editMenu.cut.accelerator:				~Shift Ctrl<Key>X
! Ddd*editMenu.cut.acceleratorText:			Ctrl+X
! Ddd*editMenu.copy.accelerator:			~Shift Ctrl<Key>C
! Ddd*editMenu.copy.acceleratorText:			Ctrl+C
! Ddd*editMenu.paste.accelerator:			~Shift Ctrl<Key>V
! Ddd*editMenu.paste.acceleratorText:			Ctrl+V


Ddd*editMenu.cut.labelString:				Cut
Ddd*editMenu.cut.mnemonic:				t
Ddd*editMenu.cut.documentationString: \
@rm Cut the selection and put it on the clipboard

Ddd*editMenu.copy.labelString:				Copy
Ddd*editMenu.copy.mnemonic:				C
Ddd*editMenu.copy.documentationString: \
@rm Copy the selection and put it on the clipboard

Ddd*editMenu.paste.labelString:				Paste
Ddd*editMenu.paste.mnemonic:				P
Ddd*editMenu.paste.documentationString: \
@rm Paste clipboard contents

Ddd*editMenu.clearAll.labelString:			Clear
Ddd*editMenu.clearAll.mnemonic:				l
Ddd*editMenu.clearAll.documentationString: \
@rm Clear the selection, all arguments, and the @GDB@ command line
Ddd*editMenu.clearAll.acceleratorText:			Ctrl+U

Ddd*editMenu.delete.labelString:			Delete
Ddd*editMenu.delete.mnemonic:				D
Ddd*editMenu.delete.documentationString: \
@rm Delete current selection

Ddd*editMenu.selectAll.labelString:			Select All
Ddd*editMenu.selectAll.mnemonic:			A
Ddd*editMenu.selectAll.acceleratorText:			Ctrl+A
Ddd*editMenu.selectAll.documentationString: \
@rm Select all items

Ddd*editMenu.preferences.labelString:			Preferences...
Ddd*editMenu.preferences.mnemonic:			e
Ddd*editMenu.preferences.documentationString:\
@rm Modify the DDD configuration

! Note: The debugger name will be prepended to this label:
Ddd*editMenu.settings.labelString:			Settings...
Ddd*editMenu.settings.mnemonic:				S
Ddd*editMenu.settings.documentationString:\
@rm Modify the @GDB@ configuration

Ddd*editMenu.saveOptions.labelString:			Save Options
Ddd*editMenu.saveOptions.mnemonic:			O
Ddd*editMenu.saveOptions.documentationString:\
@rm If set, preferences and settings are saved across DDD invocations


! View menu (for single-window DDD)
Ddd*menubar.views.labelString:		View
Ddd*menubar.views.mnemonic:		V
Ddd*menubar.view*documentationString:	\
@rm Open and close DDD windows

define(VIEWS_HELP, [\
WIDGET(View Menu)\n\
\n\
DESC(Command Tool..., [open and recenter the command tool])\n\
DESC(Execution Window..., [open the execution window])\n\
\n\
DESC(@GDB@ Console, [toggle display of debugger console])\n\
DESC(Source Window, [toggle display of program source])\n\
DESC(Data Window, [toggle display of program data])\n\
DESC(Machine Code Window, [toggle display of assembler code])
])dnl

Ddd*menubar.views.helpString:	VIEWS_HELP
Ddd*viewsMenu*helpString:	VIEWS_HELP
Ddd*viewsMenu*tearOffTitle:     View

Ddd*viewsMenu.tool.labelString:			Command Tool...
Ddd*viewsMenu.tool.mnemonic:			T
Ddd*viewsMenu.tool.acceleratorText:		Alt+8
Ddd*viewsMenu.tool.accelerator:			Meta<Key>8
Ddd*viewsMenu.tool.documentationString: \
@rm Open the command tool and move it to its original position

Ddd*viewsMenu.exec.labelString:			Execution Window...
Ddd*viewsMenu.exec.mnemonic:			x
Ddd*viewsMenu.exec.acceleratorText:		Alt+9
Ddd*viewsMenu.exec.accelerator:			Meta<Key>9
Ddd*viewsMenu.exec.documentationString: \
@rm Open the program execution window

Ddd*viewsMenu.console.labelString:		@GDB@ Console
Ddd*viewsMenu.console.mnemonic:			C
Ddd*viewsMenu.console.acceleratorText:		Alt+1
Ddd*viewsMenu.console.accelerator:		Meta<Key>1
Ddd*viewsMenu.console.documentationString: \
@rm Toggle showing the @GDB@ console

Ddd*viewsMenu.source.labelString:		Source Window
Ddd*viewsMenu.source.mnemonic:			S
Ddd*viewsMenu.source.acceleratorText:		Alt+2
Ddd*viewsMenu.source.accelerator:		Meta<Key>2
Ddd*viewsMenu.source.documentationString: \
@rm Toggle showing the source window

Ddd*viewsMenu.data.labelString:			Data Window
Ddd*viewsMenu.data.mnemonic:			D
Ddd*viewsMenu.data.acceleratorText:		Alt+3
Ddd*viewsMenu.data.accelerator:			Meta<Key>3
Ddd*viewsMenu.data.documentationString: \
@rm Toggle showing the data window

Ddd*viewsMenu.code.labelString:			Machine Code Window
Ddd*viewsMenu.code.mnemonic:			M
Ddd*viewsMenu.code.acceleratorText:		Alt+4
Ddd*viewsMenu.code.accelerator:			Meta<Key>4
Ddd*viewsMenu.code.documentationString: \
@rm Toggle showing the machine code window


! View menu (for multi-window DDD)
Ddd*menubar.view.labelString:		View
Ddd*menubar.view.mnemonic:		V
Ddd*menubar.view*documentationString:	\
@rm Open DDD windows

define(VIEW_HELP, [\
\
WIDGET(View Menu)\n\
\n\
DESC(Command Tool..., [open and recenter the command tool])\n\
DESC(Execution Window..., [open the execution window])\n\
\n\
DESC(@GDB@ Console..., [open the debugger console window])\n\
DESC(Source Window..., [open the program source window])\n\
DESC(Data Window..., [open the program data window])
])dnl

Ddd*menubar.view.helpString:	VIEW_HELP
Ddd*viewMenu*helpString:	VIEW_HELP
Ddd*viewMenu*tearOffTitle:      View

Ddd*viewMenu.tool.labelString:			Command Tool...
Ddd*viewMenu.tool.mnemonic:			T
Ddd*viewMenu.tool.acceleratorText:		Alt+8
Ddd*viewMenu.tool.accelerator:			Meta<Key>8
Ddd*viewMenu.tool.documentationString: \
@rm Open the command tool and move it to its original position

Ddd*viewMenu.exec.labelString:			Execution Window...
Ddd*viewMenu.exec.mnemonic:			x
Ddd*viewMenu.exec.acceleratorText:		Alt+9
Ddd*viewMenu.exec.accelerator:			Meta<Key>9
Ddd*viewMenu.exec.documentationString: \
@rm Open the program execution window

Ddd*viewMenu.console.labelString:		@GDB@ Console...
Ddd*viewMenu.console.mnemonic:			C
Ddd*viewMenu.console.acceleratorText:		Alt+1
Ddd*viewMenu.console.accelerator:		Meta<Key>1
Ddd*viewMenu.console.documentationString: \
@rm Open the @GDB@ console

Ddd*viewMenu.source.labelString:		Source Window...
Ddd*viewMenu.source.mnemonic:			S
Ddd*viewMenu.source.acceleratorText:		Alt+2
Ddd*viewMenu.source.accelerator:		Meta<Key>2
Ddd*viewMenu.source.documentationString: \
@rm Open the source window

Ddd*viewMenu.data.labelString:			Data Window...
Ddd*viewMenu.data.mnemonic:			D
Ddd*viewMenu.data.acceleratorText:		Alt+3
Ddd*viewMenu.data.accelerator:			Meta<Key>3
Ddd*viewMenu.data.documentationString: \
@rm Open the data window


! Program menu
Ddd*menubar.program.labelString:		Program
Ddd*menubar.program.mnemonic:			P
Ddd*menubar.program*documentationString:	\
@rm Control the execution of the debugged program

define(PROGRAM_HELP, [\
\
WIDGET(Program Menu)\n\
\n\
DESC(Run..., [start the debugged program])\n\
DESC(Run Again, [run with current arguments])\n\
\n\
ITEM If LBL(Run in Execution Window) is set, \
the program is run in a separate\n\
    terminal window; useful for programs using terminal I/O.  Otherwise,\n\
    the program is run in the @GDB@ console window.\n\
    This change takes effect the next time the program is run.\n\
\n\
DESC(Step, [step program until it reaches a different source line])\n\
DESC(Step Instruction, [step exactly one machine instruction])\n\
DESC(Next, [step program, but proceed through subroutine calls])\n\
DESC(Next Instruction, [step instruction, \
but proceed through subroutine calls])\n\
DESC(Until, [execute until program reaches a line after the current])\n\
DESC(Finish, [execute until function returns])\n\
\n\
DESC(Continue, [continue program after signal or breakpoint])\n\
DESC(Continue Without Signal, [continue, but don't give a signal])\n\
\n\
DESC(Kill, [kill execution of program being debugged])\n\
DESC(Interrupt, [interrupt program (or current @GDB@ command)])\n\
DESC(Abort, [abort program (or current @GDB@ command)])
])dnl

Ddd*menubar.program*helpString:	PROGRAM_HELP
Ddd*programMenu*helpString:	PROGRAM_HELP
Ddd*programMenu*tearOffTitle:   Program

Ddd*menubar*programMenu.run.labelString:	   Run...
Ddd*menubar*programMenu.run.mnemonic:		   R
Ddd*menubar*programMenu.run.accelerator:	   <Key>F2
Ddd*menubar*programMenu.run.acceleratorText:	   F2
Ddd*menubar*programMenu.run.documentationString: \
@rm Start debugged program.  You may specify arguments to give it.

Ddd*menubar*programMenu.run_again.labelString:	   Run Again
Ddd*menubar*programMenu.run_again.mnemonic:	   A
Ddd*menubar*programMenu.run_again.accelerator:	   <Key>F3
Ddd*menubar*programMenu.run_again.acceleratorText: F3
Ddd*menubar*programMenu.run_again.documentationString: \
@rm Restart debugged program, using the previous arguments.

Ddd*menubar*programMenu.separateExecWindow.labelString:	Run in Execution Window
Ddd*menubar*programMenu.separateExecWindow.mnemonic:	E
Ddd*menubar*programMenu.separateExecWindow.documentationString:\
@rm Toggle execution of debugged program in separate window

Ddd*menubar*programMenu.step.labelString:	   Step
Ddd*menubar*programMenu.step.mnemonic:		   S
Ddd*menubar*programMenu.step.accelerator:	   ~Shift<Key>F5
Ddd*menubar*programMenu.step.acceleratorText:	   F5
Ddd*menubar*programMenu.step.documentationString:

Ddd*menubar*programMenu.stepi.labelString:	   Step Instruction
Ddd*menubar*programMenu.stepi.mnemonic:		   I
Ddd*menubar*programMenu.stepi.accelerator:	   Shift<Key>F5
Ddd*menubar*programMenu.stepi.acceleratorText:	   Shift+F5
Ddd*menubar*programMenu.stepi.documentationString:

Ddd*menubar*programMenu.next.labelString:	   Next
Ddd*menubar*programMenu.next.mnemonic:		   N
Ddd*menubar*programMenu.next.accelerator:	   ~Shift<Key>F6
Ddd*menubar*programMenu.next.acceleratorText:	   F6
Ddd*menubar*programMenu.next.documentationString:

Ddd*menubar*programMenu.nexti.labelString:	   Next Instruction
Ddd*menubar*programMenu.nexti.mnemonic:		   x
Ddd*menubar*programMenu.nexti.accelerator:	   Shift<Key>F6
Ddd*menubar*programMenu.nexti.acceleratorText:	   Shift+F6
Ddd*menubar*programMenu.nexti.documentationString:

Ddd*menubar*programMenu.until.labelString:	   Until
Ddd*menubar*programMenu.until.mnemonic:	   	   U
Ddd*menubar*programMenu.until.accelerator:	   <Key>F7
Ddd*menubar*programMenu.until.acceleratorText:	   F7
Ddd*menubar*programMenu.until.documentationString:

Ddd*menubar*programMenu.finish.labelString:	   Finish
Ddd*menubar*programMenu.finish.mnemonic:	   F
Ddd*menubar*programMenu.finish.accelerator:	   <Key>F8
Ddd*menubar*programMenu.finish.acceleratorText:	   F8
Ddd*menubar*programMenu.finish.documentationString:

Ddd*menubar*programMenu.cont.labelString:	   Continue
Ddd*menubar*programMenu.cont.mnemonic:		   C
Ddd*menubar*programMenu.cont.accelerator:	   ~Shift<Key>F9
Ddd*menubar*programMenu.cont.acceleratorText:	   F9
Ddd*menubar*programMenu.cont.documentationString:

Ddd*menubar*programMenu.signal0.labelString:	   Continue Without Signal
Ddd*menubar*programMenu.signal0.mnemonic:          t
Ddd*menubar*programMenu.signal0.accelerator:	   Shift<Key>F9
Ddd*menubar*programMenu.signal0.acceleratorText:   Shift+F9
Ddd*menubar*programMenu.signal0.documentationString: \
@rm Continue program without giving it a signal.

Ddd*menubar*programMenu.kill.labelString:	   Kill
Ddd*menubar*programMenu.kill.mnemonic:		   K
Ddd*menubar*programMenu.kill.accelerator:	   <Key>F4
Ddd*menubar*programMenu.kill.acceleratorText:	   F4
Ddd*menubar*programMenu.kill.documentationString:

Ddd*menubar*programMenu.break.labelString:	   Interrupt
Ddd*menubar*programMenu.break.mnemonic:		   p
! Ddd*menubar*programMenu.break.accelerator:	   <Key>Escape
Ddd*menubar*programMenu.break.acceleratorText:	   Esc
Ddd*menubar*programMenu.break.documentationString: \
@rm Interrupt program (or current @GDB@ command)

Ddd*menubar*programMenu.quit.labelString:	   Abort
Ddd*menubar*programMenu.quit.mnemonic:		   b
Ddd*menubar*programMenu.quit.accelerator:	   Ctrl<Key>\\ 
Ddd*menubar*programMenu.quit.acceleratorText:	   Ctrl+\\ 
Ddd*menubar*programMenu.quit.documentationString: \
@rm Abort program (or current @GDB@ command)


! Commands
Ddd*menubar.commands.labelString:	Commands
Ddd*menubar.commands.mnemonic:		C
Ddd*menubar.commands*documentationString: \
@rm Enter and modify @GDB@ commands

define(COMMANDS_HELP, [\
\
WIDGET(Commands Menu)\n\
\n\
DESC(Command History..., [show all previous commands])\n\
\n\
DESC(Previous, [show the previous command])\n\
DESC(Next, [show the next command])\n\
\n\
DESC(Search Previous, [do incremental search forward])\n\
DESC(Search Next, [do incremental search backward])\n\
DESC(Quit Search, [quit incremental search])\n\
\n\
DESC(Complete, [complete the current command])\n\
DESC(Apply, [execute the current command])\n\
\n\
DESC(Clear Line, [clear @GDB@ command line])\n\
DESC(Clear Window, [clear all before last @GDB@ prompt])\n\
\n\
DESC(Define Command..., [define @GDB@ command])\n\
DESC(Edit Buttons..., [edit command buttons])
])dnl

Ddd*menubar.commands*helpString:	COMMANDS_HELP
Ddd*commandsMenu*helpString:		COMMANDS_HELP
Ddd*commandsMenu*tearOffTitle:          Commands

Ddd*commandsMenu.history.labelString:	        Command History...
Ddd*commandsMenu.history.mnemonic:	        H
Ddd*commandsMenu.history.documentationString: \
@rm Show previous commands

Ddd*commandsMenu.prev.labelString:	        Previous
Ddd*commandsMenu.prev.mnemonic:		        P
! Ddd*commandsMenu.prev.accelerator:	        Ctrl<Key>P
Ddd*commandsMenu.prev.acceleratorText:	        Up
Ddd*commandsMenu.prev.documentationString: \
@rm Get previous command

Ddd*commandsMenu.next.labelString:	        Next
Ddd*commandsMenu.next.mnemonic:		        N
! Ddd*commandsMenu.next.accelerator:	        Ctrl<Key>N
Ddd*commandsMenu.next.acceleratorText:          Down
Ddd*commandsMenu.next.documentationString: \
@rm Get next command

Ddd*commandsMenu.complete.labelString:	        Complete
Ddd*commandsMenu.complete.mnemonic:	        C
! Ddd*commandsMenu.complete.accelerator:	~Ctrl ~Shift ~Meta<Key>Tab
! Ddd*commandsMenu.complete.accelerator:	Ctrl<Key>I
Ddd*commandsMenu.complete.acceleratorText:	Tab
Ddd*commandsMenu.complete.documentationString: \
@rm Complete current command

Ddd*commandsMenu.apply.labelString:	        Apply
Ddd*commandsMenu.apply.mnemonic:	        A
! Ddd*commandsMenu.apply.accelerator:	        ~Ctrl ~Shift ~Meta<Key>Return
Ddd*commandsMenu.apply.acceleratorText:         Return
Ddd*commandsMenu.apply.documentationString: \
@rm Execute current command

Ddd*commandsMenu.isearch_prev.labelString:	Find Backward
Ddd*commandsMenu.isearch_prev.mnemonic:	        B
! Ddd*commandsMenu.isearch_prev.accelerator:	Ctrl<Key>B
Ddd*commandsMenu.isearch_prev.acceleratorText:  Ctrl+B
Ddd*commandsMenu.isearch_prev.documentationString: \
@rm Do incremental search backward in command history

Ddd*commandsMenu.isearch_next.labelString:	Find Forward
Ddd*commandsMenu.isearch_next.mnemonic:	        F
! Ddd*commandsMenu.isearch_next.accelerator:	Ctrl<Key>F
Ddd*commandsMenu.isearch_next.acceleratorText:  Ctrl+F
Ddd*commandsMenu.isearch_next.documentationString: \
@rm Do incremental search forward in command history

Ddd*commandsMenu.isearch_exit.labelString:	Quit Search
Ddd*commandsMenu.isearch_exit.mnemonic:	        Q
Ddd*commandsMenu.isearch_exit.acceleratorText:  Esc
Ddd*commandsMenu.isearch_exit.documentationString: \
@rm Quit incremental search

Ddd*commandsMenu.clear_line.labelString:        Clear Line
Ddd*commandsMenu.clear_line.mnemonic:	        L
! *commandsMenu.clear_line.accelerator:	        ~Shift ~Meta Ctrl<Key>U
Ddd*commandsMenu.clear_line.acceleratorText:    Ctrl+U
Ddd*commandsMenu.clear_line.documentationString: \
@rm Clear @GDB@ command line

Ddd*commandsMenu.clear_window.labelString:      Clear Window
Ddd*commandsMenu.clear_window.mnemonic:	        W
Ddd*commandsMenu.clear_window.accelerator:      ~Meta Shift Ctrl<Key>U
Ddd*commandsMenu.clear_window.acceleratorText:  Shift+Ctrl+U
Ddd*commandsMenu.clear_window.documentationString: \
@rm Clear all before last @GDB@ prompt

Ddd*commandsMenu.define.labelString:		Define Command...
Ddd*commandsMenu.define.mnemonic:	        D
Ddd*commandsMenu.define.documentationString: \
@rm Define @GDB@ command

Ddd*commandsMenu.buttons.labelString:	        Edit Buttons...
Ddd*commandsMenu.buttons.mnemonic:	        u
Ddd*commandsMenu.buttons.documentationString: \
@rm Edit command buttons


! Status
Ddd*menubar.stack.labelString:		Status
Ddd*menubar.stack.mnemonic:		t
Ddd*menubar.stack*documentationString:  \
@rm Show the current program state

define(STACK_HELP, [\
\
WIDGET(Status Menu)\n\
\n\
DESC(Backtrace..., [give a summary of how your program got where it is])\n\
DESC(Registers..., [show current processor registers])\n\
DESC(Threads..., [show current program threads])\n\
DESC(Signals..., [show current signal handling])\n\
\n\
DESC(Up, [show the function that called the current one])\n\
DESC(Down, [show the function that was called by the current one])
])dnl

Ddd*menubar.stack.helpString:	STACK_HELP
Ddd*stackMenu*helpString:	STACK_HELP
Ddd*stackMenu*tearOffTitle:     Status

Ddd*stackMenu.stack.labelString:	Backtrace...
Ddd*stackMenu.stack.mnemonic:		B
Ddd*stackMenu.stack.documentationString: \
@rm Give a summary of how the debugged program got where it is

Ddd*stackMenu.registers.labelString:	Registers...
Ddd*stackMenu.registers.mnemonic:	R
Ddd*stackMenu.registers.documentationString: \
@rm Show registers and their contents

Ddd*stackMenu.threads.labelString:	Threads...
Ddd*stackMenu.threads.mnemonic:		T
Ddd*stackMenu.threads.documentationString: \
@rm Show and select current program threads

Ddd*stackMenu.signals.labelString:	Signals...
Ddd*stackMenu.signals.mnemonic:		S
Ddd*stackMenu.signals.documentationString: \
@rm Show and edit current signal handling

Ddd*stackMenu.up.labelString:		Up
Ddd*stackMenu.up.mnemonic:		U
Ddd*stackMenu.up.accelerator:		Ctrl<Key>Up
Ddd*stackMenu.up.acceleratorText:	Ctrl+Up
Ddd*stackMenu.up.documentationString: \
@rm Show the function that called the current one

Ddd*stackMenu.down.labelString:		Down
Ddd*stackMenu.down.mnemonic:		D
Ddd*stackMenu.down.accelerator:		Ctrl<Key>Down
Ddd*stackMenu.down.acceleratorText:	Ctrl+Down
Ddd*stackMenu.down.documentationString: \
@rm Show the function that was called by the current one



! Source menu
Ddd*menubar.source.labelString:		Source
Ddd*menubar.source.mnemonic:		S
Ddd*menubar.source*documentationString: \
@rm Set and edit breakpoints in source files

define(SOURCE_HELP, [\
\
WIDGET(Source Menu)\n\
\n\
DESC(Breakpoints..., [set, view, and edit breakpoints])\n\
\n\
DESC(Lookup (), [lookup LBL(()) in the source])\n\
DESC(LBL_FIND_FORWARD, [find next occurrence of LBL(())])\n\
DESC(LBL_FIND_BACKWARD, [find previous occurrence of LBL(())])\n\
\n\
ITEM LBL(Find Words Only) enables finding complete words only.\n\
    If unset, arbitrary occurrences are found.\n\
ITEM LBL(Find Case Sensitive) enables case-sensitive search.\n\
    If unset, occurrences are found regardless of case.\n\
\n\
DESC(Display Line Numbers, [enable numbered source lines])\n\
DESC(Display Machine Code, [enable disassembling of the current function])\n\
\n\
DESC(Edit Source..., [invoke text editor for current source])\n\
DESC(Reload Source , [reload current source file])])dnl

Ddd*menubar.source.helpString:	SOURCE_HELP
Ddd*sourceMenu.helpString:	SOURCE_HELP

Ddd*sourceMenu.breakpoints.labelString:		Breakpoints...
Ddd*sourceMenu.breakpoints.mnemonic:		B
Ddd*sourceMenu.breakpoints.documentationString: \
@rm Set, view, and edit breakpoints

Ddd*sourceMenu.lookup.labelString:	        Lookup ()
Ddd*sourceMenu.lookup.mnemonic:			L
Ddd*sourceMenu.lookup.accelerator:		Ctrl<Key>slash
Ddd*sourceMenu.lookup.acceleratorText:		Ctrl+/
Ddd*sourceMenu.lookup.documentationString:\
@rm Lookup LBL(()) in the source

Ddd*sourceMenu.findForward.labelString:	        LBL_FIND_FORWARD
! Ddd*sourceMenu.findForward.mnemonic:		>
Ddd*sourceMenu.findForward.accelerator:		Ctrl<Key>period
Ddd*sourceMenu.findForward.acceleratorText:	Ctrl+.
Ddd*sourceMenu.findForward.documentationString:\
@rm Search the next occurrence of LBL(())

Ddd*sourceMenu.findBackward.labelString:	LBL_FIND_BACKWARD
! Ddd*sourceMenu.findBackward.mnemonic:		<
Ddd*sourceMenu.findBackward.accelerator:	Ctrl<Key>comma
Ddd*sourceMenu.findBackward.acceleratorText:	Ctrl+,
Ddd*sourceMenu.findBackward.documentationString:\
@rm Search the previous occurrence of LBL(())

Ddd*sourceMenu.findWordsOnly.labelString:	Find Words Only
Ddd*sourceMenu.findWordsOnly.mnemonic:		W
Ddd*sourceMenu.findWordsOnly.accelerator:	Meta<Key>W
Ddd*sourceMenu.findWordsOnly.acceleratorText:	Alt+W
Ddd*sourceMenu.findWordsOnly.documentationString:\
@rm Switch between finding complete words and finding arbitrary occurrences

Ddd*sourceMenu.findCaseSensitive.labelString:	Find Case Sensitive
Ddd*sourceMenu.findCaseSensitive.mnemonic:	i
Ddd*sourceMenu.findCaseSensitive.accelerator:	Meta<Key>I
Ddd*sourceMenu.findCaseSensitive.acceleratorText: Alt+I
Ddd*sourceMenu.findCaseSensitive.documentationString:\
@rm Toggle case-sensitive search

Ddd*sourceMenu.disassemble.labelString:		Display Machine Code
Ddd*sourceMenu.disassemble.mnemonic:		M
Ddd*sourceMenu.disassemble.accelerator:		Meta<Key>4
Ddd*sourceMenu.disassemble.acceleratorText:	Alt+4
Ddd*sourceMenu.disassemble.documentationString:\
@rm Toggle display of machine code window

Ddd*sourceMenu.lineNumbers.labelString:		Display Line Numbers
Ddd*sourceMenu.lineNumbers.mnemonic:		N
Ddd*sourceMenu.lineNumbers.accelerator:		Meta<Key>N
Ddd*sourceMenu.lineNumbers.acceleratorText:	Alt+N
Ddd*sourceMenu.lineNumbers.documentationString:\
@rm Toggle displaying source line numbers

Ddd*sourceMenu.edit.labelString:	Edit Source...
Ddd*sourceMenu.edit.mnemonic:		E
Ddd*sourceMenu.edit.accelerator:	~Meta Shift Ctrl<Key>V
Ddd*sourceMenu.edit.acceleratorText:	Shift+Ctrl+V
Ddd*sourceMenu.edit.documentationString: \
@rm Invoke text editor for the current source

Ddd*sourceMenu.reload.labelString:	Reload Source
Ddd*sourceMenu.reload.mnemonic:		R
Ddd*sourceMenu.reload.accelerator:	~Meta Shift Ctrl<Key>L
Ddd*sourceMenu.reload.acceleratorText:	Shift+Ctrl+L
Ddd*sourceMenu.reload.documentationString: \
@rm Reload current source file


! Data menu
Ddd*menubar.data.labelString:		Data
Ddd*menubar.data.mnemonic:		D
Ddd*menubar.data*documentationString:   \
@rm Create and modify data displays

define(DATA_HELP, [\
\
WIDGET(Data Menu)\n\
\n\
DESC(Displays..., [select, enable and delete data displays])\n\
DESC(Themes..., [customize data displays])\n\
DESC(Watchpoints..., [set, view, and edit watchpoints])\n\
DESC(Examine Memory..., [examine memory in any of several formats])\n\
\n\
DESC(Print (),   [print LBL(()) in @GDB@ console])\n\
DESC(Display (), [display LBL(()) in data window])\n\
\n\
DESC(Detect Aliases, [toggle alias detection])\n\
\n\
DESC(Display Local Variables, [show current local variables])\n\
DESC(Display Arguments, [show current argument variables])\n\
DESC(Status Displays..., [show other things about the debugged program])\n\
\n\
DESC(Align on Grid, [align all displays on the next grid point])\n\
DESC(Rotate Graph, [rotate the graph clockwise by 90 degrees])\n\
DESC(Layout Graph, [layout the graph])\n\
\n\
DESC(Refresh, [update all displays])
])dnl

Ddd*menubar.data*helpString:	DATA_HELP
Ddd*dataMenu*helpString:	DATA_HELP
Ddd*dataMenu*tearOffTitle:      Data

Ddd*dataMenu.displays.labelString:	Displays...
Ddd*dataMenu.displays.mnemonic:		D
Ddd*dataMenu.displays.documentationString: \
@rm Select, enable and delete data displays

Ddd*dataMenu.themes.labelString:	Themes...
Ddd*dataMenu.themes.mnemonic:		T
Ddd*dataMenu.themes.documentationString: \
@rm Customize data displays

Ddd*dataMenu.watchpoints.labelString:	Watchpoints...
Ddd*dataMenu.watchpoints.mnemonic:	W
Ddd*dataMenu.watchpoints.documentationString: \
@rm Set, view, and edit watchpoints

Ddd*dataMenu.examine.labelString:	Memory...
Ddd*dataMenu.examine.mnemonic:		M
Ddd*dataMenu.examine.documentationString: \
@rm Examine memory in any of several formats

Ddd*dataMenu.print.labelString:		Print ()
Ddd*dataMenu.print.mnemonic:		P
Ddd*dataMenu.print.accelerator:		Ctrl<Key>equal
Ddd*dataMenu.print.acceleratorText:	Ctrl+=
Ddd*dataMenu.print.documentationString: \
@rm Print the argument LBL(()) in the @GDB@ console

Ddd*dataMenu.display.labelString:	Display ()
Ddd*dataMenu.display.mnemonic:		i
Ddd*dataMenu.display.accelerator:	Ctrl<Key>minus
Ddd*dataMenu.display.acceleratorText:	Ctrl+-
Ddd*dataMenu.display.documentationString: \
@rm Display the argument LBL(()) in the data window

Ddd*dataMenu.detectAliases.labelString:	Detect Aliases
Ddd*dataMenu.detectAliases.mnemonic:	A
Ddd*dataMenu.detectAliases.accelerator:	Meta<Key>A
Ddd*dataMenu.detectAliases.acceleratorText: Alt+A
Ddd*dataMenu.detectAliases.documentationString:	\
@rm Detect aliases (shared data structures)

! This item is visible in GDB only.
Ddd*dataMenu.info locals.labelString:	Display Local Variables
Ddd*dataMenu.info locals.mnemonic:	L
Ddd*dataMenu.info locals.accelerator:	Meta<Key>L
Ddd*dataMenu.info locals.acceleratorText: Alt+L
Ddd*dataMenu.info locals.documentationString: \
@rm Display the local variables of the current stack frame

! This item is visible in GDB only.
Ddd*dataMenu.info args.labelString:	Display Arguments
Ddd*dataMenu.info args.mnemonic:	u
Ddd*dataMenu.info args.accelerator:	Meta<Key>U
Ddd*dataMenu.info args.acceleratorText: Alt+U
Ddd*dataMenu.info args.documentationString: \
@rm Display the argument variables of the current stack frame

! This item is visible in DBX only.
Ddd*dataMenu.dump.labelString:		Display Local Variables
Ddd*dataMenu.dump.mnemonic:		L
Ddd*dataMenu.dump.accelerator:		Meta<Key>L
Ddd*dataMenu.dump.acceleratorText:	Alt+L
Ddd*dataMenu.dump.documentationString: \
@rm Display all local variables of the current stack frame

! This item is visible in XDB only.
Ddd*dataMenu.l.labelString:		Display Local Variables
Ddd*dataMenu.l.mnemonic:		L
Ddd*dataMenu.l.accelerator:		Meta<Key>L
Ddd*dataMenu.l.acceleratorText:		Alt+L
Ddd*dataMenu.l.documentationString: \
@rm Display all local variables of the current stack frame

! This item is visible in JDB only.
Ddd*dataMenu.locals.labelString:	Display Local Variables
Ddd*dataMenu.locals.mnemonic:		L
Ddd*dataMenu.locals.accelerator:	Meta<Key>L
Ddd*dataMenu.locals.acceleratorText:	Alt+L
Ddd*dataMenu.locals.documentationString: \
@rm Display all local variables of the current stack frame

Ddd*dataMenu.infos.labelString:		Status Displays...
Ddd*dataMenu.infos.mnemonic:		S
Ddd*dataMenu.infos.documentationString: \
@rm Show various things about the program being debugged

Ddd*dataMenu.align.labelString:		Align on Grid
Ddd*dataMenu.align.mnemonic:		G
Ddd*dataMenu.align.accelerator:		Meta<Key>G
Ddd*dataMenu.align.acceleratorText:	Alt+G
Ddd*dataMenu.align.documentationString:	\
@rm Align all displays on the next grid point

Ddd*dataMenu.rotate.labelString:	Rotate Graph
Ddd*dataMenu.rotate.mnemonic:		R
Ddd*dataMenu.rotate.accelerator:	Meta<Key>R
Ddd*dataMenu.rotate.acceleratorText:	Alt+R
Ddd*dataMenu.rotate.documentationString:	\
@rm Rotate the graph clockwise by 90 degrees

Ddd*dataMenu.layout.labelString:	Layout Graph
Ddd*dataMenu.layout.mnemonic:		y
Ddd*dataMenu.layout.accelerator:	Meta<Key>Y
Ddd*dataMenu.layout.acceleratorText:	Alt+Y
Ddd*dataMenu.layout.documentationString:	\
@rm Layout the graph

Ddd*dataMenu.refresh.labelString:	Refresh
Ddd*dataMenu.refresh.mnemonic:		f
Ddd*dataMenu.refresh.accelerator:	~Shift ~Meta Ctrl<Key>L
Ddd*dataMenu.refresh.acceleratorText:	Ctrl+L
Ddd*dataMenu.refresh.documentationString:	\
@rm Redraw all displays, using current values



! Maintenance menu
Ddd*menubar.maintenance.labelString:	Maintenance
Ddd*menubar.maintenance.mnemonic:	M
Ddd*menubar.maintenance*documentationString:   \
@rm Debug DDD

define(MAINTENANCE_HELP, [\
\
WIDGET(Maintenance Menu)\n\
\n\
This menu is for DDD maintenance only.\n\
It becomes active when DDD gets a fatal signal.\n\
\n\
DESC(Debug [[[[DDD]]]]..., [invoke a debugger on this DDD instance])\n\
DESC(Dump Core, [dump core now])\n\
DESC(Tic Tac Toe..., [play Tic Tac Toe against DDD])\n\
\n\
DESC(When [[[[DDD]]]] Crashes, [what to do when DDD crashes])\n\
\n\
DESC(Remove Menu, [remove this menu])
])dnl

Ddd*menubar.maintenance*helpString: MAINTENANCE_HELP
Ddd*maintenanceMenu*helpString:     MAINTENANCE_HELP
Ddd*maintenanceMenu*tearOffTitle:   Maintenance

Ddd*maintenanceMenu.debug.labelString:		Debug DDD...
Ddd*maintenanceMenu.debug.mnemonic:		D
Ddd*maintenanceMenu.debug.accelerator:		~Shift ~Ctrl<Key>F12
Ddd*maintenanceMenu.debug.acceleratorText:	F12
Ddd*maintenanceMenu.debug.documentationString:	\
@rm Invoke a debugger on this DDD instance

Ddd*maintenanceMenu.dumpCore.labelString:	Dump Core Now
Ddd*maintenanceMenu.dumpCore.mnemonic:		C
Ddd*maintenanceMenu.dumpCore.documentationString:	\
@rm Make DDD dump core now

Ddd*maintenanceMenu.tictactoe.labelString:	Tic Tac Toe...
Ddd*maintenanceMenu.tictactoe.mnemonic:		T
Ddd*maintenanceMenu.tictactoe.documentationString:	\
@rm Play Tic Tac Toe against DDD

Ddd*maintenanceMenu.crash.labelString:		When DDD Crashes
Ddd*maintenanceMenu.crash.mnemonic:		W
Ddd*maintenanceMenu.crash.documentationString:	\
@rm What to do when DDD crashes

Ddd*maintenanceMenu.valgrindLeak.labelString:	Do Valgrind Leak Check
Ddd*maintenanceMenu.valgrindLeak.mnemonic:	L
Ddd*maintenanceMenu.valgrindLeak.documentationString:	\
@rm Do a self Valgrind leak check

Ddd*maintenanceMenu.remove.labelString:         Remove Menu
Ddd*maintenanceMenu.remove.mnemonic:		R
Ddd*maintenanceMenu.remove.documentationString:	\
@rm Remove this menu


define(CRASH_HELP, [\
\
WIDGET(Crash Menu)\n\
\n\
When DDD crashes...\n\
DESC(Debug [[[[DDD]]]], [invoke a debugger])\n\
DESC(Dump Core, [dump core])\n\
DESC(Do Nothing, [neither invoke a debugger nor debug DDD])
])dnl

Ddd*crashMenu*helpString:     			CRASH_HELP
Ddd*crashMenu*tearOffTitle:   			When DDD Crashes

Ddd*crashMenu.debug.labelString:		Debug DDD
Ddd*crashMenu.debug.mnemonic:			D
Ddd*crashMenu.debug.documentationString:	\
@rm Invoke debugger on DDD when it crashes

Ddd*crashMenu.dumpCore.labelString:		Dump Core
Ddd*crashMenu.dumpCore.mnemonic:		C
Ddd*crashMenu.dumpCore.documentationString:	\
@rm Dump core when DDD crashes

Ddd*crashMenu.nothing.labelString:		Do Nothing
Ddd*crashMenu.nothing.mnemonic:			N
Ddd*crashMenu.nothing.documentationString:	\
@rm Neither invoke a debugger nor dump core


! Help menu
Ddd*menubar.help.labelString:		Help
Ddd*menubar.help.mnemonic:		H
Ddd*menubar.help*documentationString:   \
@rm Get more information

define(HELP_HELP, [\
\
WIDGET(Help Menu)\n\
\n\
DESC(Overview, [help on how to use the help facility])\n\
\n\
DESC(On Item...,   [give help on the current item])\n\
DESC(On Window..., [help on the application itself])\n\
\n\
DESC(What Now?..., [give help on what to do now])\n\
DESC(Tip of the Day..., [give a useful tip on DDD usage])\n\
\n\
DESC([[[[DDD]]]] Reference..., [the DDD on-line manual])\n\
DESC([[[[DDD]]]] News..., [what's new in DDD])\n\
DESC(@GDB@ Reference..., [the @GDB@ on-line manual])\n\
\n\
DESC([[[[DDD]]]] License..., [copying, distributing, and modifying DDD])\n\
DESC([[[[DDD]]]] WWW Page..., [the DDD WWW page])\n\
\n\
DESC(About [[[[DDD]]]]..., [some general information about DDD])
])dnl

Ddd*menubar.help*helpString: HELP_HELP
Ddd*helpMenu*helpString:     HELP_HELP
Ddd*helpMenu*tearOffTitle:   Help

Ddd*helpMenu.onHelp.labelString:		Overview...
Ddd*helpMenu.onHelp.mnemonic:			v
Ddd*helpMenu.onHelp.documentationString:	\
@rm How to use the help facility

Ddd*helpMenu.onItem.labelString:		On Item...
Ddd*helpMenu.onItem.mnemonic:			O
Ddd*helpMenu.onItem.accelerator:		~Shift ~Ctrl<Key>HELP_KEY
! *helpMenu.onItem.accelerator:		        <Key>osfHelp

! In fact, both HELP_KEY and Shift+HELP_KEY invoke help on item.
! However, if we press HELP_KEY, we get immediate help, while
! Shift+HELP_KEY will get `on context' help, where an item must be
! selected first.  Since `on context' help is also what we get
! when `on item' is selected via the menu, display the `alternate'
! accelerator instead.
Ddd*helpMenu.onItem.acceleratorText:	        Shift+HELP_KEY
Ddd*helpMenu.onItem.documentationString:	\
@rm Get help on the current item

Ddd*helpMenu.onWindow.labelString:		On Window...
Ddd*helpMenu.onWindow.mnemonic:			W
Ddd*helpMenu.onWindow.documentationString:	\
@rm Information about the current window

! To make Shift+HELP_KEY work even on widgets that have no help callback
! installed, we use this accelerator to invoke `on window' help.  `on window'
! help will detect that Shift is pressed and call `on context' help instead.
Ddd*helpMenu.onWindow.accelerator:		Shift ~Ctrl<Key>HELP_KEY


Ddd*helpMenu.whatNext.labelString:		What Now?...
Ddd*helpMenu.whatNext.mnemonic:			N
Ddd*helpMenu.whatNext.accelerator:		~Shift Ctrl<Key>HELP_KEY
Ddd*helpMenu.whatNext.acceleratorText:		Ctrl+HELP_KEY
Ddd*helpMenu.whatNext.documentationString:	\
@rm Explain the current situation and suggest what to do next

Ddd*helpMenu.tipOfTheDay.labelString:		Tip of the Day...
Ddd*helpMenu.tipOfTheDay.mnemonic:		T
Ddd*helpMenu.tipOfTheDay.documentationString:	\
@rm Give a useful tip on DDD usage

Ddd*helpMenu.dddManual.labelString:		DDD Reference...
Ddd*helpMenu.dddManual.mnemonic:		R
Ddd*helpMenu.dddManual.documentationString:	\
@rm The DDD reference manual

Ddd*helpMenu.news.labelString:			DDD News...
Ddd*helpMenu.news.mnemonic:			e
Ddd*helpMenu.news.documentationString:		\
@rm What's new in this DDD version

Ddd*helpMenu.gdbManual.labelString:		@GDB@ Reference...
Ddd*helpMenu.gdbManual.mnemonic:		B
Ddd*helpMenu.gdbManual.documentationString:	\
@rm The @GDB@ reference manual

Ddd*helpMenu.license.labelString:		DDD License...
Ddd*helpMenu.license.mnemonic:			L
Ddd*helpMenu.license.documentationString:	\
@rm Terms and conditions for copying, distributing, and modifying DDD

Ddd*helpMenu.www.labelString:			DDD WWW Page...
Ddd*helpMenu.www.mnemonic:			P
Ddd*helpMenu.www.documentationString:	\
@rm Invoke a WWW browser with up-to-date DDD information

Ddd*helpMenu.onVersion.labelString:		About DDD...
Ddd*helpMenu.onVersion.mnemonic:		A
Ddd*helpMenu.onVersion.documentationString:	\
@rm General information about this DDD version


!-----------------------------------------------------------------------------
! Command Tool Layout
!-----------------------------------------------------------------------------

! The number of rows in the command tool.
Ddd*tool_buttons.fractionBase:			90

! The positions are set up according to the following scheme:
! 
!    0 45 90	Each line has its individual number.
!  0 +--+--+	`topPosition' refers to the line above the button;
!    |  |  |	`bottomPosition' is the line below.  Likewise,
! 10 +--+--+	`leftPosition' is the line at the left, and
!    |  |  |	`rightPosition' is the line to the right.  For instance,
! 20 +--+--+	the line above the `break' button
!    |  |  |	is line 1 (topPosition), the line below is line 2
! 30 +--+--+	(bottomPosition), the line to the left is line 0
!    |  |  |	(leftPosition), and the line to the right is line 8
! 40 +--+--+	(rightPosition).
!    |  |  |	
! 50 +--+--+	To replace a button by another command, change the
!    |  |  |	`toolButtons' resource, above, and insert appropriate
! 60 +--+--+	resource values for the new button, specifying its
!    |  |  |	location.
! 70 +--+--+	
!    |  |  |	
! 80 +--+--+	
!    |  |  |	
! 90 +--+--+	

Ddd*tool_buttons.run.topPosition:		0
Ddd*tool_buttons.run.bottomPosition:		10
Ddd*tool_buttons.run.leftPosition:		0
Ddd*tool_buttons.run.rightPosition:		90

Ddd*tool_buttons.break.topPosition:		10
Ddd*tool_buttons.break.bottomPosition:		20
Ddd*tool_buttons.break.leftPosition:		0
Ddd*tool_buttons.break.rightPosition:		90

Ddd*tool_buttons.step.topPosition:		20
Ddd*tool_buttons.step.bottomPosition:		30
Ddd*tool_buttons.step.leftPosition:		0
Ddd*tool_buttons.step.rightPosition:		45

Ddd*tool_buttons.stepi.topPosition:		20
Ddd*tool_buttons.stepi.bottomPosition:		30
Ddd*tool_buttons.stepi.leftPosition:		45
Ddd*tool_buttons.stepi.rightPosition:		90

Ddd*tool_buttons.next.topPosition:		30
Ddd*tool_buttons.next.bottomPosition:		40
Ddd*tool_buttons.next.leftPosition:		0
Ddd*tool_buttons.next.rightPosition:		45

Ddd*tool_buttons.nexti.topPosition:		30
Ddd*tool_buttons.nexti.bottomPosition:		40
Ddd*tool_buttons.nexti.leftPosition:		45
Ddd*tool_buttons.nexti.rightPosition:		90

Ddd*tool_buttons.until.topPosition:		40
Ddd*tool_buttons.until.bottomPosition:		50
Ddd*tool_buttons.until.leftPosition:		0
Ddd*tool_buttons.until.rightPosition:		45

Ddd*tool_buttons.finish.topPosition:		40
Ddd*tool_buttons.finish.bottomPosition:		50
Ddd*tool_buttons.finish.leftPosition:		45
Ddd*tool_buttons.finish.rightPosition:		90

Ddd*tool_buttons.cont.topPosition:		50
Ddd*tool_buttons.cont.bottomPosition:		60
Ddd*tool_buttons.cont.leftPosition:		0
Ddd*tool_buttons.cont.rightPosition:		45

Ddd*tool_buttons.kill.topPosition:		50
Ddd*tool_buttons.kill.bottomPosition:		60
Ddd*tool_buttons.kill.leftPosition:		45
Ddd*tool_buttons.kill.rightPosition:		90

Ddd*tool_buttons.up.topPosition:		60
Ddd*tool_buttons.up.bottomPosition:		70
Ddd*tool_buttons.up.leftPosition:		0
Ddd*tool_buttons.up.rightPosition:		45

Ddd*tool_buttons.down.topPosition:		60
Ddd*tool_buttons.down.bottomPosition:		70
Ddd*tool_buttons.down.leftPosition:		45
Ddd*tool_buttons.down.rightPosition:		90

Ddd*tool_buttons.Undo.topPosition:		70
Ddd*tool_buttons.Undo.bottomPosition:		80
Ddd*tool_buttons.Undo.leftPosition:		0
Ddd*tool_buttons.Undo.rightPosition:		45

Ddd*tool_buttons.Redo.topPosition:		70
Ddd*tool_buttons.Redo.bottomPosition:		80
Ddd*tool_buttons.Redo.leftPosition:		45
Ddd*tool_buttons.Redo.rightPosition:		90

Ddd*tool_buttons.Edit.topPosition:		80
Ddd*tool_buttons.Edit.bottomPosition:		90
Ddd*tool_buttons.Edit.leftPosition:		0
Ddd*tool_buttons.Edit.rightPosition:		45

Ddd*tool_buttons.Make.topPosition:		80
Ddd*tool_buttons.Make.bottomPosition:		90
Ddd*tool_buttons.Make.leftPosition:		45
Ddd*tool_buttons.Make.rightPosition:		90

! Command tool button placement.  Don't change this.
Ddd*tool_buttons*topAttachment:			XmATTACH_POSITION
Ddd*tool_buttons*bottomAttachment:		XmATTACH_POSITION
Ddd*tool_buttons*leftAttachment:		XmATTACH_POSITION
Ddd*tool_buttons*rightAttachment:		XmATTACH_POSITION

! Make command tool and tool bar buttons a little lighter
Ddd*tool_buttons*shadowThickness:		1
Ddd*source_buttons*shadowThickness:		1
Ddd*console_buttons*shadowThickness:		1
Ddd*data_buttons*shadowThickness:		1
Ddd*command_toolbar*shadowThickness:		1
Ddd*toolbar.?.shadowThickness:			1


!-----------------------------------------------------------------------------
! Preferences
!-----------------------------------------------------------------------------

! Ddd*preferences.okLabelString: 		Close
Ddd*preferences.cancelLabelString:		Reset
Ddd*preferences.dialogTitle:			DDD Preferences

Ddd*preferences*buttons.orientation: 		XmHORIZONTAL
Ddd*preferences*buttons.borderWidth:  		0
Ddd*preferences*buttons.marginWidth:  		0
Ddd*preferences*buttons.marginHeight:		0
Ddd*preferences*buttons.entryAlignment:		XmALIGNMENT_CENTER
Ddd*preferences*buttons*indicatorOn:		off
Ddd*preferences*buttons*shadowThickness:	2
Ddd*preferences*buttons*alignment:		XmALIGNMENT_CENTER
Ddd*preferences*panel*XmRowColumn.marginHeight: 0
Ddd*preferences*packing:	      	        XmPACK_TIGHT

Ddd*preferences*buttons*general.labelString:	\ \ \ \ General\ \ \ \ 
Ddd*preferences*buttons*source.labelString:	Source
Ddd*preferences*buttons*data.labelString:	Data
Ddd*preferences*buttons*startup.labelString:	Startup
Ddd*preferences*buttons*fonts.labelString:	Fonts
Ddd*preferences*buttons*helpers.labelString:	Helpers
Ddd*preferences*buttons*packing:                XmPACK_COLUMN

Ddd*preferences*general*helpString:	\
WIDGET(General Preferences)\n\
\n\
ITEM When you move the mouse pointer over a button, DDD can give a hint \
on its usage.\n\
    SUBITEM Select LBL(as popup tips) if you want the hint to be shown \
in a popup window.\n\
    SUBITEM Select LBL(in the status line) if you want the hint to be shown \
in the status line.\n\
    You can also select both options, or none at all.\n\
ITEM When you move the mouse pointer over a variable in the source window,\n\
    DDD can automatically display the variable value.\n\
    SUBITEM Select LBL(as popup tips) if you want the value to be shown \
in a popup window.\n\
    SUBITEM Select LBL(in the status line) if you want it to be shown \
in the status line.\n\
    You can also select both options, or none at all.\n\
ITEM In DDD, the KEY(Tab) key can complete its arguments.\n\
    SUBITEM Set LBL(in all windows) if you want completion in all DDD \
windows.\n\
    SUBITEM Set LBL(in console only) if you need the KEY(Tab) key to move \
the focus.\n\
ITEM If LBL(Iconify all windows at once) is set, all DDD windows are \
iconified as a group.\n\
    Otherwise, windows are iconified separately.\n\
ITEM If LBL(Uniconify when ready) is set, the DDD windows are \
automatically uniconified\n\
    when DDD becomes ready (e.g. after reaching a breakpoint).\n\
    Otherwise, DDD windows remain iconified.\n\
ITEM If LBL(Suppress X warnings) is set, X warnings are silently ignored.\n\
ITEM If LBL(Continue automatically...) is set, DDD will continue execution\n\
    of programs that were interrupted while grabbing the mouse pointer.\n\
ITEM LBL(Warn if multiple...) enables warnings about multiple DDD instances\n\
    that share (and may overwrite) preferences and other state files.\n\
ITEM LBL(Undo Buffer Size) sets the maximum size of the undo buffer.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.


Ddd*preferences*buttonHints.labelString:	 \
Automatic Display of Button Hints
Ddd*preferences*valueHints.labelString:	 \
Automatic Display of Variable Values

Ddd*preferences*tips.labelString: as Popup Tips
Ddd*preferences*docs.labelString: in the Status Line

Ddd*preferences*tabCompletion.labelString: \
TAB Key Completes
Ddd*preferences*inAllWindows.labelString: in All Windows
Ddd*preferences*inConsole.labelString:    in Console Only

Ddd*preferences*undoSize.labelString: \
Undo Buffer Size
Ddd*preferences*size.text.columns:  5
Ddd*preferences*kbytes.labelString: kBytes
Ddd*preferences*clear.labelString:  Clear Undo Buffer


Ddd*preferences*groupIconify.labelString:	\
Iconify all Windows at Once
Ddd*preferences*uniconifyWhenReady.labelString:	\
Uniconify When Ready
Ddd*preferences*suppressWarnings.labelString:	\
Suppress X Warnings
Ddd*preferences*checkGrabs.labelString:	 	\
Continue Automatically when Mouse Pointer is Frozen
Ddd*preferences*warnIfLocked.labelString:	\
Warn if Multiple DDD Instances are Running


Ddd*preferences*source*helpString:	\
WIDGET(Source Preferences)\n\
\n\
ITEM DDD can show the current position and breakpoints\n\
    SUBITEM LBL(as glyphs), using small images in the text,\n\
    SUBITEM LBL(as text), using ordinary text characters.\n\
ITEM The LBL(Tool Buttons) can be placed\n\
    SUBITEM in the LBL(Command Tool) which can be moved around DDD, or\n\
    SUBITEM in the LBL(Source Window), as line of buttons.\n\
ITEM Using DBX, DDD can refer to program sources\n\
    SUBITEM by LBL(path name), or\n\
    SUBITEM by LBL(base name), as required by some DBX variants.\n\
ITEM If LBL(Find words only) is set, only complete words are found.\n\
    Otherwise, DDD finds arbitrary occurrences.\n\
ITEM If LBL(Find case sensitive) is set, search is case-sensitive.\n\
    Otherwise, DDD finds occurrences regardless of case.\n\
ITEM If LBL(Cache source files) is set, source texts are cached in memory.\n\
    Otherwise, sources are read from disk upon each source change.\n\
ITEM If LBL(Cache machine code) is set, disassembled code is cached \
in memory.\n\
    Otherwise, code is re-disassembled upon each function change.\n\
ITEM LBL(Display line numbers) enables numbered source lines.\n\
ITEM In LBL(Tab Width), you can set the spacing of tab stops.\n\
    Setting the tab width to 8 sets a tab stop every 8 characters.\n\
ITEM In LBL(Indentation), you can indent the source and machine code,\n\
    giving additional room for the breakpoint area.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.

Ddd*preferences*showExecPos.labelString:	Show Position and Breakpoints
Ddd*preferences*asGlyphs.labelString:		as Glyphs
Ddd*preferences*asText.labelString:		as Text Characters

Ddd*preferences*toolButtons.labelString:	Tool Buttons Location
Ddd*toolButtonsMenu*commandTool.labelString:	\
Command Tool
Ddd*toolButtonsMenu*sourceWindow.labelString:   \
Source Window\ \ \ \ \ 

Ddd*preferences*referSources.labelString:	Refer to Program Sources
Ddd*preferences*byPath.labelString:		by Path Name
Ddd*preferences*byBase.labelString:		by Base Name

Ddd*preferences*cache.labelString:	        Cache
Ddd*preferences*cacheSource.labelString:	Source Files
Ddd*preferences*cacheCode.labelString:		Machine Code

Ddd*preferences*find.labelString:	        Find
Ddd*preferences*wordsOnly.labelString:		Words Only\ 
Ddd*preferences*caseSensitive.labelString:	Case Sensitive

Ddd*preferences*lineNumbers.labelString:	Display Source Line Numbers

Ddd*preferences*tabWidth.orientation:		XmHORIZONTAL
Ddd*preferences*tabWidth.minimum:		1
Ddd*preferences*tabWidth.maximum:		32
Ddd*preferences*tabWidth.showValue:		on
Ddd*preferences*tabWidth.titleString:		\
Tab Width\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ 

Ddd*preferences*sourceIndent.orientation:	XmHORIZONTAL
Ddd*preferences*sourceIndent.minimum:		0
Ddd*preferences*sourceIndent.maximum:		16
Ddd*preferences*sourceIndent.showValue:		on
Ddd*preferences*sourceIndent.titleString:	\
Source Indentation\ \ \ \ \ \ \ \ \ \ \ 

Ddd*preferences*codeIndent.orientation:		XmHORIZONTAL
Ddd*preferences*codeIndent.minimum:		0
Ddd*preferences*codeIndent.maximum:		16
Ddd*preferences*codeIndent.showValue:		on
Ddd*preferences*codeIndent.titleString:		\
Machine Code Indentation


Ddd*preferences*data*helpString:	\
WIDGET(Data Preferences)\n\
\n\
ITEM LBL(Show edge hints) enables handles for multi-line and arc edges.\n\
ITEM LBL(Show edge annotations) enables named edges.\n\
ITEM LBL(Show titles) enables titles of dependent displays.\n\
ITEM LBL(Placement) controls how new displays are to be placed:\n\
    SUBITEM If LBL(Top to bottom) is set, new independent displays\n\
        will be placed below the downmost display.\n\
    SUBITEM If LBL(Left to right) is set, new independent displays\n\
        will be placed on the right of the rightmost display.\n\
    SUBITEM If LBL(Clustered) is set, independent data displays\n\
        are merged into one single cluster, saving screen space.\n\
    Dependent displays are placed in the orthogonal direction.\n\
ITEM LBL(Layout) controls the automatic layout:\n\
    SUBITEM LBL(Compact layout) enables a compact layout,\n\
        suitable for homogeneous structures only.\n\
    SUBITEM LBL(Automatic layout) makes DDD relayout the graph\n\
        after each change.\n\
ITEM If LBL(Detect aliases) is set, DDD detects displays with the same\n\
    physical address and suppresses all aliases except the one that was\n\
    least recently changed.  Useful for examining shared data structures.\n\
ITEM If LBL(Display two-dimensional arrays as tables) is set, the elements\n\
    of two-dimensional arrays are aligned in a table.\n\
    Otherwise, two-dimensional arrays are displayed as nested linear arrays.\n\
ITEM If LBL(Close data window when deleting last display) is set,\n\
    deleting the last display automatically closes the data window.\n\
ITEM If LBL(Auto-align displays) is set,\n\
    each display is aligned on the nearest grid point.\n\
ITEM In LBL(Grid size), you can change the spacing of grid points.\n\
    A spacing of 0 disables the grid.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.

Ddd*preferences*detectAliases.labelString:	\
Detect Aliases (shared data structures)
Ddd*preferences*align2dArrays.labelString:	\
Display Two-Dimensional Arrays as Tables
Ddd*preferences*snapToGrid.labelString:	        \
Auto-Align Displays on Nearest Grid Point\ \ \ \ \ \ \ \ \ \ 
Ddd*preferences*hints.labelString:	        \
Edge Hints
Ddd*preferences*annotations.labelString:	\
Edge Annotations
Ddd*preferences*dependentTitles.labelString:    \
Titles of Dependent Displays
Ddd*preferences*autoClose.labelString:	        \
Close Data Window when Deleting last Display

Ddd*preferences*show.labelString:	        \
Show
Ddd*preferences*layout.labelString:	        \
Layout
Ddd*preferences*auto.labelString:	        \
Automatic
Ddd*preferences*compact.labelString:	        \
Compact

Ddd*preferences*placement.labelString:	        \
Placement
Ddd*preferences*topToBottom.labelString:        \
Top to bottom
Ddd*preferences*leftToRight.labelString:        \
Left to right
Ddd*preferences*clusterDisplays.labelString:    \
Clustered

Ddd*preferences*gridSize.orientation:		XmHORIZONTAL
Ddd*preferences*gridSize.minimum:		0
Ddd*preferences*gridSize.maximum:		64
Ddd*preferences*gridSize.width:	                192
Ddd*preferences*gridSize.showValue:		on
Ddd*preferences*gridSize.titleString:		Grid Size

Ddd*preferences*themes.labelString:             \ \ \ \ Themes...\ \ \ \ 


Ddd*preferences*startup*helpString:   \
WIDGET(Startup Preferences)\n\
\n\
These preferences are effective only after a restart.\n\
\n\
ITEM LBL(Window Layout) sets the window layout.\n\
    SUBITEM LBL(Stacked Windows) means to use one top-level window\n\
        where source, data, and the @GDB@ console are stacked.\n\
    SUBITEM LBL(Separate Windows) means to use a separate top-level window\n\
        for each of source, data, and the @GDB@ console.\n\
ITEM The LBL(Ctrl+C) and LBL(Ctrl+A) keys can be bound to different actions.\n\
ITEM The LBL(Tool Bar) can appear as follows:\n\
    SUBITEM LBL(Images) shows a small symbol for each action, and/or\n\
    SUBITEM LBL(Captions) shows the action name below the image.\n\
    If neither LBL(Images) nor LBL(Captions) is set, \
buttons have ordinary labels.\n\
    SUBITEM LBL(Flat) enables the button border only when entered.\n\
    SUBITEM LBL(Color) enables colored button images.\n\
    SUBITEM LBL(Bottom) places the tool bar at the bottom of the window.\n\
    This can only be done with separate windows or ordinary labels.\n\
ITEM LBL(Keyboard Focus) sets the keyboard focus policy.\n\
    SUBITEM LBL(Click to Type) means that you must click on a window\n\
        to direct the keyboard focus to it.\n\
    SUBITEM LBL(Point to Type) means that pointing to the window suffices.\n\
ITEM LBL(Data Scrolling) sets the data window scrolling mode.\n\
    SUBITEM LBL(Panner) means to use a two-dimensional scrollbar\n\
        (not available in all DDD configurations).\n\
    SUBITEM LBL(Scrollbars) means to use two scrollbars.\n\
ITEM LBL(Debugger Type) sets the type of the inferior debugger.\n\
    If LBL(Determine Automatically...) is set, the inferior debugger is set\n\
    dependent on the arguments given to DDD.\n\
ITEM LBL(Show Splash Screen) sets whether DDD shows its splash screen.\n\
ITEM LBL(Show Tip of the Day) sets whether DDD shows its tip of the day.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.

Ddd*preferences*startup*adjustMargin:		off

Ddd*preferences*windows.labelString:		Window Layout
Ddd*windowsMenu*attached.labelString:		Stacked Windows
Ddd*windowsMenu*separate.labelString:		Separate Windows

Ddd*preferences*cutCopyPaste.labelString:	Ctrl+C is
Ddd*cutCopyPasteMenu*kde.labelString:		Copy
Ddd*cutCopyPasteMenu*motif.labelString:		Interrupt

Ddd*preferences*selectAll.labelString:		Ctrl+A is
Ddd*selectAllMenu*kde.labelString:		Select All
Ddd*selectAllMenu*motif.labelString:		Beginning of Line

Ddd*preferences*buttons.labelString:		Tool Bar Appearance
Ddd*buttonsMenu*images.labelString:		Images
Ddd*buttonsMenu*captions.labelString:		Captions
Ddd*buttonsMenu*flat.labelString:		Flat
Ddd*buttonsMenu*color.labelString:		Color
Ddd*buttonsMenu*color.toggleMode:		XmTOGGLE_INDETERMINATE
Ddd*buttonsMenu*bottom.labelString:		Bottom

Ddd*preferences*keyboardFocus.labelString:	Keyboard Focus
Ddd*keyboardFocusMenu*explicit.labelString:	Click to Type
Ddd*keyboardFocusMenu*pointer.labelString:	Point to Type

Ddd*preferences*dataScrolling.labelString:	Data Scrolling
Ddd*dataScrollingMenu*panner.labelString:	Panner
Ddd*dataScrollingMenu*scrollbars.labelString:	Scrollbars

Ddd*preferences*autoDebugger.labelString:	Debugger Type
Ddd*preferences*automatic.labelString:		\
Determine Automatically from Arguments
Ddd*preferences*debugger.labelString:		
Ddd*debuggerMenu*bash.labelString:		Bash
Ddd*debuggerMenu*dbg.labelString:		DBG
Ddd*debuggerMenu*dbx.labelString:		DBX
Ddd*debuggerMenu*gdb.labelString:		GDB
Ddd*debuggerMenu*jdb.labelString:		JDB
Ddd*debuggerMenu*perl.labelString:		Perl
Ddd*debuggerMenu*pydb.labelString:		PYDB
Ddd*debuggerMenu*xdb.labelString:		XDB
Ddd*debuggerMenu.numColumns:	      	        1

Ddd*preferences*startupWindows.labelString:     Startup Windows
Ddd*preferences*splashScreen.labelString:	DDD Splash Screen
Ddd*preferences*startupTips.labelString:	Tip of the Day


Ddd*preferences*fonts*helpString: \
WIDGET([DDD] fonts)\n\
\n\
ITEM LBL(Default Font) is the font used for DDD labels and buttons.\n\
ITEM LBL(Variable Width) is the font used for messages (such as this one).\n\
ITEM LBL(Fixed Width) is the font used for program code and text fields.\n\
ITEM LBL(Data) is the font used for data displays.\n\
\n\
To change a font, enter its name and size (in 1/10 points).\n\
A pair VAR(family)-VAR(weight) as font name is sufficient.\n\
\n\
Using LBL(Browse), you can select fonts using the CODE(xfontsel)\n\
font selection program.\n\
Click on LBL(select) to select the chosen font.\n\
Wildcard entries (SAMP(*)) will be ignored.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.


Ddd*preferences*fonts*default.labelString:	 Default Font
Ddd*preferences*fonts*variableWidth.labelString: Variable Width
Ddd*preferences*fonts*fixedWidth.labelString:	 Fixed Width
Ddd*preferences*fonts*data.labelString:	         Data

Ddd*preferences*fonts*name.text.columns:	 30
Ddd*preferences*fonts*size.label.labelString:	 Size
Ddd*preferences*fonts*size.text.columns:	 3
Ddd*preferences*fonts*browse.labelString:	 Browse...


Ddd*preferences*helpers*helpString: \
WIDGET([DDD] Helpers)\n\
\n\
Helpers are external applications used by DDD.\n\
\n\
ITEM LBL(Edit Sources) invokes an X editor for the current source file.\n\
    SAMP(@ FILE@ ) is replaced by the current file name.\n\
    SAMP(@ LINE@ ) is replaced by the current line.\n\
    Example: SAMP(nedit @ FILE@ )\n\
ITEM LBL(Get Core File) is a command to get a core file \
from a running process.\n\
    SAMP(@ FILE@ ) is replaced by the base name of the target core file.\n\
    SAMP(@ PID@ ) is replaced by the process ID.\n\
    The output must be written to SAMP(@ FILE@ .@ PID@ ).\n\
    Example: SAMP(gcore -o @ FILE@  @ PID@ )\n\
    Leave this empty if you have no SAMP(gcore) or similar command.\n\
ITEM LBL(List Processes) is a command to get a list of processes.\n\
    Example: SAMP(ps)\n\
ITEM LBL(Execution Window) is a command to start a terminal emulator.\n\
    To this command, DDD appends bourne shell commands to be executed\n\
    within the execution window.\n\
    Example: SAMP(@XTERM@ -e /bin/sh -c)\n\
ITEM LBL(Uncompress) is an uncompression command.\n\
    The command reads from standard input and writes to standard output.\n\
    Example: SAMP(gunzip -c)\n\
ITEM LBL(Web Browser) invokes a WWW browser.\n\
    SAMP(@ URL@ ) is replaced by the URL to be shown.\n\
    Example: SAMP(netscape @ URL@ )\n\
ITEM LBL(Plot) invokes a Gnuplot program for plotting data.\n\
    Example: SAMP(gnuplot)\n\
ITEM LBL(Plot Window) indicates the type of plot window to use.\n\
    SUBITEM The LBL(builtin) DDD window uses fewer resources.\n\
    SUBITEM The LBL(external) Gnuplot window is much faster, but\n\
        does not work with all window managers.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.

Ddd*preferences*edit*labelString:		Edit Sources
Ddd*preferences*get_core*labelString:		Get Core File
Ddd*preferences*ps*labelString:			List Processes
Ddd*preferences*term*labelString:		Execution Window
Ddd*preferences*uncompress*labelString:		Uncompress
Ddd*preferences*www*labelString:		Web Browser
Ddd*preferences*plot*labelString:		Plot
Ddd*preferences*plot_window*labelString:	Plot Window
Ddd*preferences*extern*labelString:		External
Ddd*preferences*builtin*labelString:		Builtin
Ddd*preferences*text.columns:			47



!-----------------------------------------------------------------------------
! Settings
!-----------------------------------------------------------------------------

Ddd*settings.dialogTitle: DDD: Debugger Settings
Ddd*settings.okLabelString: Apply
Ddd*settings.applyLabelString: Reset
Ddd*settings.cancelLabelString: Close
Ddd*settings.textAccelerators:

Ddd*settings*help.labelString:		?
Ddd*settings*XmTextField.columns:	10
Ddd*settings*XmTextField.marginHeight:	3

! Settings for special fields
Ddd*settings*dir.columns:        36
Ddd*settings*path.columns:       36
Ddd*settings*use.columns:        50
Ddd*settings*v8.labelString:     SPARC V8
Ddd*settings*v9.labelString:     SPARC V9
Ddd*settings*v9sgi.labelString:  SPARC V9 with Special Graphics Instructions
Ddd*settings*parent.labelString: stay with parent
Ddd*settings*child.labelString:  follow child
Ddd*settings*both.labelString:   follow both
Ddd*settings*ask.labelString:    ask for an option
Ddd*settings*stdio.labelString:  Standard I/O
Ddd*settings*pty.labelString:    PTY

Ddd*settings*help.tipString:\
@rm Get help on this setting

! An empty help string means that the help text for this command
! is to be requested from GDB (via the GDB `help' command).
Ddd*settings*scroll*helpString:

! The panel itself has a help button, too.
Ddd*settings*helpString: \
WIDGET(@GDB@ Settings)\n\
\n\
These settings control @GDB@ operations.\n\
Click on LBL(?) near an item to get further information.\n\
\n\
LBL(Edit, Save Options) saves all @GDB@ settings, except\n\
session-dependent settings such as source or object paths.\n\
LBL(File, Save Session) also saves session-dependent @GDB@ settings.\n\
\n\
Click on LBL(Apply) to apply changed text settings.\n\
Click on LBL(Reset) to restore the saved settings.\n\
Click on LBL(Close) to close this window.


!-----------------------------------------------------------------------------
! Status displays
!-----------------------------------------------------------------------------

Ddd*infos.dialogTitle: DDD: Status Displays
Ddd*infos.okLabelString:     Apply
Ddd*infos.applyLabelString:  Reset
Ddd*infos.cancelLabelString: Close

Ddd*infos*help.labelString:			?
Ddd*infos*help.tipString:\
@rm Get help on this status display

! An empty help string means that the help text for this command
! is to be requested from GDB (via the GDB `help' command).
Ddd*infos*scroll*helpString:

! The panel itself has a help button, too.
Ddd*infos*helpString: \
WIDGET(Status Displays)\n\
\n\
A EMPH(status display) shows things about the program being debugged\n\
in the data window.  Please pick your choice from the list.\n\
\n\
Click on LBL(?) near an item to get further information.\n\
\n\
Click on LBL(Reset) to delete all status displays.\n\
Click on LBL(Close) to close this window.



!-----------------------------------------------------------------------------
! Signals
!-----------------------------------------------------------------------------

! Ddd*signals.okLabelString: Close
Ddd*signals.dialogTitle: DDD: Signal Handling
Ddd*signals.okLabelString:     Apply
Ddd*signals.applyLabelString:  Reset
Ddd*signals.cancelLabelString: Close

Ddd*signals*send.labelString:	   Send
Ddd*signals*send.rightOffset:	   5
Ddd*signals*send.tipString: \
@rm Continue execution with this signal

Ddd*signals*help.labelString:	   ?
Ddd*signals*help.tipString: \
@rm Get help on this signal

! Help for `All signals'
Ddd*signals*all*helpString: \
WIDGET(All Signals)\n\
\n\
Use these settings to modify all signals at once - expect those\n\
used by the debugger, typically SIGTRAP and SIGINT.

! The panel itself has a help button, too.
Ddd*signals*helpString: \
WIDGET(Signal Handling)\n\
\n\
This panel controls how signals are handled by @GDB@.\n\
\n\
DESC(Stop, [interrupt program if this signal happens (implies LBL(Print))])\n\
DESC(Print, [print a message in the @GDB@ console if this signal happens])\n\
DESC(Pass, [let program see this signal; otherwise program doesn't know])\n\
DESC(Send, [continue program execution with this signal])\n\
\n\
Use LBL(All Signals) to modify all signals at once.\n\
Click on LBL(?) to get further information on the specific signal.\n\
\n\
Click on LBL(Reset) to restore the original settings.\n\
Click on LBL(Close) to close this window.


!-----------------------------------------------------------------------------
! Themes
!-----------------------------------------------------------------------------

! Ddd*themes.okLabelString: Close
Ddd*themes.dialogTitle: DDD: Themes
Ddd*themes.okLabelString:     Apply
Ddd*themes.applyLabelString:  Reset
Ddd*themes.cancelLabelString: Close

Ddd*themes*XmTextField.columns:	    40
Ddd*themes*XmTextField.marginHeight: 3

Ddd*themes*help.labelString:	   ?
Ddd*themes*help.tipString: \
@rm Get help on this theme

! The panel itself has a help button, too.
Ddd*themes*helpString: \
WIDGET(Themes)\n\
\n\
This panel controls how values are to be displayed by DDD.\n\
\n\
Each EMPH(theme) is a visual modifier that can be applied on \
several expressions.\n\
The toggle button controls whether a theme is active or not.\n\
\n\
If a theme is active, it is applied on all expressions that match one\n\
of the given patterns.\n\
Patterns are separated by SAMP(;) and contain shell-like metacharacters:\n\
ITEM SAMP(*) matches any sequence of characters\n\
ITEM SAMP(?) matches any single character\n\
ITEM SAMP(\133VAR(set)\135) matches any character in VAR(set)\n\
ITEM SAMP(\133!VAR(set)\135) matches any character not in the VAR(set)\n\
ITEM SAMP(\1330-9a-zA-Z_\135) is the set of characters allowed \
in C identifiers.\n\
To suppress the special syntactic significance of any metacharacter\n\
and match the character exactly, precede it with SAMP(\\).\n\
To suppress the syntactic significance of EMPH(all) metacharacters,\n\
enclose the pattern in double or single quotes.\n\
\n\
Click on LBL(?) to get further information on the specific theme.\n\
\n\
Click on LBL(Apply) to apply changed text settings.\n\
Click on LBL(Reset) to restore the original settings.\n\
Click on LBL(Close) to close this window.

!-----------------------------------------------------------------------------
! Button Editor and Shortcut Editor
!-----------------------------------------------------------------------------

Ddd*edit_buttons.dialogTitle: DDD: Button Editor
! Ddd*edit_buttons.okLabelString: Close
Ddd*edit_buttons.cancelLabelString: Reset

Ddd*edit_buttons*buttons.entryAlignment:	XmALIGNMENT_CENTER
Ddd*edit_buttons*buttons*indicatorOn:		off
Ddd*edit_buttons*buttons*shadowThickness:	2
Ddd*edit_buttons*buttons*alignment:		XmALIGNMENT_CENTER

Ddd*edit_buttons*console.labelString:		Console Buttons
Ddd*edit_buttons*source.labelString:		Source Buttons
Ddd*edit_buttons*data.labelString:		Data Buttons
Ddd*edit_buttons*shortcuts.labelString:		Shortcuts

Ddd*edit_buttons*text.rows: 			12
Ddd*edit_buttons*text.wordWrap:	        	on
Ddd*edit_buttons*text.scrollHorizontal:    	off

Ddd*edit_buttons*verify.labelString: 		Enable supported buttons only

Ddd*edit_buttons*helpString: \
WIDGET(Button Editor)\n\
\n\
Each line specifies a user-defined button, in the format\n\
\n\
    VAR(command)\ \ \ \ CODE(//) VAR(label)\n\
\n\
If SAMP(// VAR(label)) is omitted, VAR(command) is used as label.\n\
\n\
The following strings have a special meaning in VAR(command):\n\
\n\
ITEM SAMP(()) is replaced by the current argument field.\n\
    Example: SAMP(print *(()) // Print *())\n\
ITEM SAMP(^VAR(character)) causes VAR(command) to issue \
KEY(Ctrl+VAR(character)).\n\
    Example: SAMP(Interrupt^C)\n\
ITEM SAMP(...) at the end keeps VAR(command) from issuing a newline.\n\
    Example: SAMP(help ...)\n\
\n\
If LBL(Enable supported buttons only) is set, DDD enables a button\n\
if and only if its command is supported by the inferior debugger.\n\
\n\
Click on LBL(OK) to apply the changes and close this window.\n\
Use LBL(Edit, Save Options) to save all button specifications.

Ddd*edit_buttons*shortcuts*helpString: \
WIDGET(Shortcut Editor)\n\
\n\
Each line specifies a display shortcut, in the format\n\
\n\
    VAR(expression)\ \ \ \ CODE(//) VAR(label)\n\
\n\
If SAMP(// VAR(label)) is omitted, LBL(Display VAR(expression)) \
is used as label.\n\
In VAR(expression), SAMP(()) is replaced by the current argument field.\n\
\n\
Useful shortcuts include:\n\
\n\
ITEM Data conversions.  SAMP(/x ()) converts to hex.\n\
ITEM Type casts.  SAMP((char *)()) casts to a string.\n\
ITEM Member access.  SAMP(().str( )) invokes the CODE(str()) method.\n\
    (We use SAMP(( )) instead of SAMP(()) to prevent argument substitution.)\n\
ITEM Pointer chains.  SAMP(*(().next)) shows what the CODE(next) \
member points at.\n\
\n\
Click on LBL(OK) to apply the changes and close this window.\n\
Use LBL(Edit, Save Options) to save all shortcuts.




!-----------------------------------------------------------------------------
! Splash screen
!-----------------------------------------------------------------------------

Ddd*splash_shell.title:                         Welcome to DDD!
Ddd*splash_shell.splash.background:	        white
Ddd*splash_shell.splash.foreground:	        black



!-----------------------------------------------------------------------------
! Scrolled and paned windows
!-----------------------------------------------------------------------------

Ddd*XmPanedWindow.marginWidth:		2
Ddd*XmPanedWindow.marginHeight:		2
Ddd*XmPanedWindow.shadowThickness:	0
Ddd*XmPanedWindow.spacing:		4
! Ddd*XmText.marginWidth:		0
! Ddd*XmText.marginHeight:		0
! Ddd*XmText.shadowThickness:		1



!-----------------------------------------------------------------------------
! Graph Editor
!-----------------------------------------------------------------------------

Ddd*graph_edit_panner.backgroundPixmap:	25_foreground
Ddd*graph_edit_panner.defaultScale:	10
Ddd*graph_edit_panner.@TRANSLATIONS@: \
#override\n\
<Btn1Down>:	   ddd-get-focus() start()\n\
<Btn2Down>:	   ddd-get-focus() abort()\n\
<Key>osfLeft:	   page(-.5p, +0)\n\
<Key>osfRight:	   page(+.5p, +0)\n\
<Key>osfUp:	   page(+0, -.5p)\n\
<Key>osfDown:	   page(+0, +.5p)\n\
<Key>osfBeginLine: page(0,0)\n\
<Key>osfBackSpace: page(-1p, -1p)\n\
<Key>osfDelete:	   page(-1p, -1p)\n\
Shift<Key>Tab:	   ddd-prev-tab-group()\n\
<Key>Tab:	   ddd-next-tab-group()\n

Ddd*graph_edit.highlightOnEnter:	On
Ddd*graph_edit.navigationType:		TAB_GROUP

! Set this to have a crosshair cursor
! Ddd*graph_edit.defaultCursor:    	crosshair

define(GRAPH_EDIT_HELP,[\
WIDGET(Data Window)\n\
\n\
The data window shows the data displays of the debugged program.\n\
\n\
STRONG(Selecting displays)\n\
ITEM Using BUTTON(1), click on a display \
to select it.\n\
ITEM Double-click on a display title to select all connected displays.\n\
ITEM Double-click on the background to select all displays.\n\
ITEM Drag on the background to select several displays.\n\
\n\
Using BUTTON(2) (or KEY(Shift) + BUTTON(1)) instead\n\
EMPH(toggles) the selection rather than setting it.\n\
\n\
STRONG(Moving displays)\n\
ITEM Using BUTTON(1), drag on a display to move\n\
    all selected displays.\n\
\n\
STRONG(Modifying displays)\n\
ITEM Using BUTTON(1), double-click on a value for details.\n\
ITEM Using BUTTON(3), click on some display to modify it.\n\
ITEM Using KEY(Shift) + BUTTON(3), select modifications from\n\
    a shortcut menu.\n\
\n\
STRONG(Creating displays)\n\
ITEM Using BUTTON(1), double-click on a pointer to dereference it.\n\
ITEM Using BUTTON(3), click on the background\n\
    to create new displays via a popup menu.
])dnl

Ddd*graph_edit.helpString:	GRAPH_EDIT_HELP
Ddd*data_disp_shell.helpString:	GRAPH_EDIT_HELP

Ddd*graph_edit_panner.helpString:	\
WIDGET(Panner)\n\
\n\
Drag the slider with BUTTON(1) to scroll the data window.
Ddd*graph_edit_panner.tipString:	\
@rm Scroll data window


!-----------------------------------------------------------------------------
! Graph Popup Menus
!-----------------------------------------------------------------------------

Ddd*graph_popup.selectAll.labelString:	Select All
Ddd*graph_popup.selectAll.documentationString: \
@rm Select all displays

Ddd*graph_popup.refresh.labelString:	Refresh
Ddd*graph_popup.refresh.documentationString: \
@rm Redraw all displays

Ddd*graph_popup.new.labelString:	New Display...
Ddd*graph_popup.new.documentationString: \
@rm Create a new display at the selected position

Ddd*node_popup.dereference.labelString:	Display *
Ddd*node_popup.dereference.documentationString: \
@rm Dereference the selected display

Ddd*node_popup.new.labelString:		New Display
Ddd*node_popup.new.documentationString: \
@rm Create a new display dependent on the selected display

Ddd*node_popup.theme.labelString:	Theme
Ddd*node_popup.theme.documentationString: \
@rm Select a theme for the selected display

Ddd*node_popup.detail.labelString:	Show Detail
Ddd*node_popup.detail.documentationString: \
@rm Show/Hide details of the selected display

Ddd*node_popup.rotate.labelString:	Rotate
Ddd*node_popup.rotate.documentationString: \
@rm Rotate the selected display

Ddd*node_popup.set.labelString:		Set Value...
Ddd*node_popup.set.documentationString: \
@rm Change the selected display value

Ddd*node_popup.delete.labelString:	Undisplay
Ddd*node_popup.delete.documentationString: \
@rm Delete the selected display

Ddd*newMenu.other.labelString:		Other...
Ddd*newMenu.other.documentationString: \
@rm Modify the selected display expression to create a new display

Ddd*newMenu.edit.labelString:		Edit Menu...
Ddd*newMenu.edit.documentationString: \
@rm Edit this menu

Ddd*themeMenu.edit.labelString:		Edit Themes...
Ddd*themeMenu.edit.documentationString: \
@rm Edit themes

Ddd*shortcut_popup.other.labelString:	Other...
Ddd*shortcut_popup.other.documentationString: \
@rm Modify the selected display expression to create a new display

Ddd*shortcut_popup.edit.labelString:	Edit Menu...
Ddd*shortcut_popup.edit.documentationString: \
@rm Edit this menu

! Compute all other documentation strings as needed
Ddd*newMenu*documentationString:
Ddd*shortcut_popup*documentationString:




!-----------------------------------------------------------------------------
! Tool Bar argument
!-----------------------------------------------------------------------------

Ddd*arg_label.labelString:	():

Ddd*arg.value:			main
Ddd*arg*Text.value:		main

Ddd*arg.helpString:	\
WIDGET(Argument)\n\
\n\
This is the argument LBL(()) for the command buttons on the right.\n\
\n\
Edit LBL(()) using the usual editing functions.\n\
Set LBL(()) by selecting items from the source window or the data window.\n\
Clear LBL(()) by clicking on the prompt LBL(():).



!-----------------------------------------------------------------------------
! Common Tool Bar
!-----------------------------------------------------------------------------

Ddd*toolbar*helpString: \
WIDGET(Tool Bar)\n\
\n\
Set the argument LBL(()) for the command buttons on the right.\n\
\n\
To get help on a command button, just point on it and press KEY_HELP.\n\
The item's functionality will be explained here.


! All other values from the graph and source tool bar, below, apply here, too.


!-----------------------------------------------------------------------------
! Graph Tool Bar
!-----------------------------------------------------------------------------

! Ddd*graph_toolbar*helpString: \
! @rm Commands related to the data window.

Ddd*toolbar*dereference.labelString: 	Display *()
Ddd*toolbar*plot.labelString: 	        Plot ()
Ddd*toolbar*history.labelString:	Plot history of ()
Ddd*toolbar*detail.labelString:		Show ()
Ddd*toolbar*show_more.labelString:   	Show More ()
Ddd*toolbar*show_just.labelString:   	Show Just ()
Ddd*toolbar*show_detail.labelString: 	Show All ()
Ddd*toolbar*hide_detail.labelString: 	Hide ()
Ddd*toolbar*rotate.labelString:		Rotate ()
Ddd*toolbar*rotateAll.labelString:	Rotate All ()
Ddd*toolbar*new.labelString:		Display ()
Ddd*toolbar*new2.labelString:	        Display ()
Ddd*toolbar*dereference2.labelString:	Display *()
Ddd*toolbar*delete2.labelString:	Undisplay ()
Ddd*toolbar*set.labelString:		Set ()
Ddd*toolbar*delete.labelString:		Undisplay ()
Ddd*toolbar*cluster.labelString:	Cluster ()

Ddd*toolbar*dereference.labelPixmap: 		dispref
Ddd*toolbar*dereference.labelInsensitivePixmap:	dispref-xx
Ddd*toolbar*dereference.armPixmap:		dispref-arm
Ddd*toolbar*dereference.highlightPixmap:	dispref-hi

Ddd*toolbar*plot.labelPixmap: 			plot
Ddd*toolbar*plot.labelInsensitivePixmap:	plot-xx
Ddd*toolbar*plot.armPixmap:			plot-arm
Ddd*toolbar*plot.highlightPixmap:		plot-hi

Ddd*toolbar*detail.labelPixmap:			show
Ddd*toolbar*detail.labelInsensitivePixmap:	show-xx
Ddd*toolbar*detail.armPixmap:			show-arm
Ddd*toolbar*detail.highlightPixmap:		show-hi

Ddd*toolbar*rotate.labelPixmap:			rotate
Ddd*toolbar*rotate.labelInsensitivePixmap:	rotate-xx
Ddd*toolbar*rotate.armPixmap:			rotate-arm
Ddd*toolbar*rotate.highlightPixmap:		rotate-hi

Ddd*toolbar*new.labelPixmap:			display
Ddd*toolbar*new.labelInsensitivePixmap:		display-xx
Ddd*toolbar*new.armPixmap:			display-arm
Ddd*toolbar*new.highlightPixmap:		display-hi

Ddd*toolbar*set.labelPixmap:			set
Ddd*toolbar*set.labelInsensitivePixmap:		set-xx
Ddd*toolbar*set.armPixmap:			set-arm
Ddd*toolbar*set.highlightPixmap:		set-hi

Ddd*toolbar*delete.labelPixmap:			undisplay
Ddd*toolbar*delete.labelInsensitivePixmap:	undisplay-xx
Ddd*toolbar*delete.armPixmap:			undisplay-arm
Ddd*toolbar*delete.highlightPixmap:		undisplay-hi

define(ANNOUNCE_PULLDOWN,[\
Pulldown menu functions (press and hold BUTTON(1)):\
])dnl

define(MORE_PULLDOWN,[(hold for menu)])dnl

define(NEW_DISPLAY_MENU_HELP, [\
DESC(Shortcut 1, [User-defined shortcut 1])\n\
DESC(Shortcut 2, [User-defined shortcut 2]) ...\n\
DESC(Other..., [Enter new shortcut])\n\
DESC(Edit..., [Edit shortcuts])
])dnl

Ddd*toolbar*new.helpString:\
LBL(Display () / Display *())\n\
\n\
Display or dereference the argument LBL(()) in the data window.\n\
\n\
ANNOUNCE_PULLDOWN\n\
NEW_DISPLAY_MENU_HELP
Ddd*newMenu*helpString: NEW_DISPLAY_MENU_HELP

Ddd*toolbar*new.tipString:\
@rm Display LBL(())
Ddd*toolbar*new.documentationString:\
@rm Display the argument LBL(()) in the data window \
MORE_PULLDOWN()

Ddd*toolbar*new2.documentationString:\
@rm Display the argument LBL(()) in the data window.

Ddd*toolbar*dereference.helpString:\
LBL(Display *())\n\
\n\
Dereference the selected display.
Ddd*toolbar*dereference.tipString:\
@rm Dereference LBL(())
Ddd*toolbar*dereference.documentationString:\
@rm Display the dereferenced argument LBL(()) in the data window.

Ddd*toolbar*dereference2.documentationString:\
@rm Display the dereferenced argument LBL(()) in the data window.

Ddd*toolbar*plot.helpString:\
LBL(Plot ())\n\
\n\
Plot the argument LBL(()).\n\
\n\
ANNOUNCE_PULLDOWN\n\
DESC(Plot History, [Plot previous and current values of LBL(())])

Ddd*toolbar*plot.tipString:\
@rm Plot LBL(())
Ddd*toolbar*plot.documentationString:\
@rm Plot the argument LBL(())
Ddd*toolbar*history.documentationString:\
@rm Plot the value history of LBL(()), as recorded at previous stops


define(DETAIL_HELP, [\
DESC(Show More, [Show more details])\n\
DESC(Show Just, [Show details, excluding substructures])\n\
DESC(Show All, [Show all details, including substructures])\n\
DESC(Hide, [Hide Details])
])dnl

Ddd*toolbar*detail.helpString:	\
LBL(Show ()) / LBL(Hide ())\n\
\n\
Show/Hide details of the selected displays.\n\
\n\
ANNOUNCE_PULLDOWN\n\
DETAIL_HELP

Ddd*detailMenu*helpString:	DETAIL_HELP

Ddd*toolbar*detail.tipString:	\
@rm Show/Hide selected details
Ddd*toolbar*detail.documentationString: \
@rm Show/Hide details of the selected displays MORE_PULLDOWN()

Ddd*toolbar*show_more.documentationString: \
@rm Show more details of the selected displays
Ddd*toolbar*show_just.documentationString: \
@rm Show details of the selected displays, hiding substructures
Ddd*toolbar*show_detail.documentationString: \
@rm Show all details of the selected displays, including substructures
Ddd*toolbar*hide_detail.documentationString: \
@rm Hide details of the selected displays

define(ROTATE_HELP, [\
DESC(Rotate All, [Rotate substructures too])
])dnl

Ddd*toolbar*rotate.helpString:	\
LBL(Rotate ())\n\
\n\
Rotate the selected displays.\n\
\n\
ANNOUNCE_PULLDOWN\n\
ROTATE_HELP

Ddd*rotateMenu*helpString:	ROTATE_HELP

Ddd*toolbar*rotate.tipString:	\
@rm Rotate selected displays
Ddd*toolbar*rotate.documentationString: \
@rm Rotate the selected displays MORE_PULLDOWN()
Ddd*toolbar*rotateAll.documentationString: \
@rm Rotate the selected displays, including substructures

Ddd*toolbar*set.helpString:	\
LBL(Set ())\n\
\n\
Change the value of the argument LBL(()).
Ddd*toolbar*set.tipString:	\
@rm Set the value of LBL(())
Ddd*toolbar*set.documentationString:	\
@rm Change the value of the argument LBL(())

Ddd*toolbar*delete.helpString:	\
LBL(Undisplay ())\n\
\n\
Delete the selected displays.\n\
\n\
ANNOUNCE_PULLDOWN\n\
DESC(Cluster (), [Merge selected displays into one single cluster])\n\
DESC(Uncluster (), [Break cluster into individual displays])

Ddd*toolbar*delete.tipString:	\
@rm Undisplay LBL(())
Ddd*toolbar*delete.documentationString:	\
@rm Delete the selected displays


!-----------------------------------------------------------------------------
! Source Tool Bar
!-----------------------------------------------------------------------------

Ddd*toolbar*lookup.labelString:		Lookup ()
Ddd*toolbar*breakAt.labelString:	Break at ()
Ddd*toolbar*breakAt2.labelString:	Set Breakpoint at ()
Ddd*toolbar*clearAt2.labelString:	Clear Breakpoint at ()
Ddd*toolbar*tempBreakAt.labelString:	Set Temporary Breakpoint at ()
Ddd*toolbar*regexBreakAt.labelString:	Set Breakpoints at Regexp ()
Ddd*toolbar*contUntil.labelString:	Continue Until ()
Ddd*toolbar*enableBreak.labelString:	Enable Breakpoint at ()
Ddd*toolbar*enableWatch.labelString:	Enable Watchpoint at ()
Ddd*toolbar*breakProperties.labelString: Breakpoint Properties...
Ddd*toolbar*watchProperties.labelString: Watchpoint Properties...
Ddd*toolbar*setPC.labelString:		Set Execution Position to ()
Ddd*toolbar*watch.labelString:		Unwatch ()
Ddd*toolbar*cwatch.labelString:		Set Watchpoint on ()
Ddd*toolbar*rwatch.labelString:		Set Read Watchpoint on ()
Ddd*toolbar*awatch.labelString:		Set Access Watchpoint on ()
Ddd*toolbar*unwatch.labelString:	Delete Watchpoint on ()
Ddd*toolbar*print.labelString:		Print ()
Ddd*toolbar*printRef.labelString:	Print *()
Ddd*toolbar*dump.labelString:	        Dump ()
Ddd*toolbar*whatis.labelString:		Whatis ()
Ddd*toolbar*examine.labelString:	Examine ()...
Ddd*toolbar*display.labelString:	Display ()
Ddd*toolbar*dispRef.labelString:	Display *()
Ddd*toolbar*find.labelString:		LBL_FIND_FORWARD
Ddd*toolbar*findBackward.labelString:	LBL_FIND_BACKWARD
Ddd*toolbar*findForward.labelString:	LBL_FIND_FORWARD

Ddd*toolbar*lookup.labelPixmap:			lookup
Ddd*toolbar*lookup.labelInsensitivePixmap:	lookup-xx
Ddd*toolbar*lookup.armPixmap:			lookup-arm
Ddd*toolbar*lookup.highlightPixmap:		lookup-hi

Ddd*toolbar*breakAt.labelPixmap:		break_at
Ddd*toolbar*breakAt.labelInsensitivePixmap: 	break_at-xx
Ddd*toolbar*breakAt.armPixmap: 			break_at-arm
Ddd*toolbar*breakAt.highlightPixmap:		break_at-hi

Ddd*toolbar*watch.labelPixmap:		 	watch
Ddd*toolbar*watch.labelInsensitivePixmap: 	watch-xx
Ddd*toolbar*watch.armPixmap: 			watch-arm
Ddd*toolbar*watch.highlightPixmap:		watch-hi

Ddd*toolbar*print.labelPixmap:		 	print
Ddd*toolbar*print.labelInsensitivePixmap: 	print-xx
Ddd*toolbar*print.armPixmap: 			print-arm
Ddd*toolbar*print.highlightPixmap:		print-hi

Ddd*toolbar*display.labelPixmap:		display
Ddd*toolbar*display.labelInsensitivePixmap: 	display-xx
Ddd*toolbar*display.armPixmap: 			display-arm
Ddd*toolbar*display.highlightPixmap:		display-hi

Ddd*toolbar*dispRef.labelPixmap:		dispref
Ddd*toolbar*dispRef.labelInsensitivePixmap: 	dispref-xx
Ddd*toolbar*dispRef.armPixmap: 			dispref-arm
Ddd*toolbar*dispRef.highlightPixmap:		dispref-hi

Ddd*toolbar*find.labelPixmap:		 	find_forward
Ddd*toolbar*find.labelInsensitivePixmap: 	find_forward-xx
Ddd*toolbar*find.armPixmap: 			find_forward-arm
Ddd*toolbar*find.highlightPixmap:		find_forward-hi

Ddd*toolbar*lookup.helpString:	\
LBL(Lookup ())\n\
\n\
Lookup a line, file, function, variable, or breakpoint in the source.\n\
ITEM If the argument LBL(()) is empty, \
lookup the current execution position\n\
    (or cursor position, if there is no current execution position).\n\
ITEM If LBL(()) contains a number, \
lookup that line number in the current source.\n\
ITEM If LBL(()) contains SAMP(VAR(filename):VAR(line)), lookup \
line number VAR(line)\n\
    in the source file VAR(filename).\n\
ITEM If LBL(()) contains SAMP([#]VAR(number)), lookup breakpoint \
VAR(number).\n\
ITEM If LBL(()) contains an address, lookup that address.\n\
ITEM If LBL(()) contains a function or variable name, lookup \
the definition\n\
    of that function or variable.

Ddd*toolbar*lookup.tipString:	\
@rm Lookup LBL(()) in the source
Ddd*toolbar*lookup.documentationString:	\
@rm Lookup a line, file, function, variable or breakpoint in the source

define(BREAK_HELP, [\
DESC(Set Temporary Breakpoint at (), [set temporary breakpoint])\n\
DESC(Set Breakpoints at Regexp (), \
[break at all functions matching LBL(())])\n\
DESC(Continue Until (), [set temporary breakpoint and resume execution])\n\
DESC(Enable / Disable Breakpoint at (), [enable or disable breakpoint])\n\
DESC(Breakpoint Properties..., [set properties of breakpoint at LBL(())])\n\
DESC(Set Execution Position to (), [move execution position])
])dnl

Ddd*toolbar*breakAt.helpString:	\
LBL(Break at ()) / LBL(Clear at ())\n\
\n\
Set or delete a breakpoint at the argument LBL(()).\n\
\n\
ANNOUNCE_PULLDOWN\n\
BREAK_HELP

Ddd*breakAtMenu*helpString:	BREAK_HELP

Ddd*toolbar*breakAt.tipString:	\
@rm Set/Delete breakpoint at LBL(())
Ddd*toolbar*breakAt.documentationString:	\
@rm Set or delete a breakpoint at the argument LBL(()) MORE_PULLDOWN()

Ddd*toolbar*tempBreakAt.documentationString: \
@rm Set a temporary breakpoint at the argument LBL(())
Ddd*toolbar*regexBreakAt.documentationString: \
@rm Set breakpoints on all functions matching the regular expression in LBL(())
Ddd*toolbar*contUntil.documentationString: \
@rm Set a temporary breakpoint at LBL(()) and resume execution
Ddd*toolbar*enableBreak.documentationString: \
@rm Enable or disable the breakpoint at the argument LBL(())
Ddd*toolbar*enableWatch.documentationString: \
@rm Enable or disable the watchpoint on the argument LBL(())
Ddd*toolbar*breakProperties.documentationString: \
@rm Edit the properties of the breakpoint at the argument LBL(())
Ddd*toolbar*watchProperties.documentationString: \
@rm Edit the properties of the watchpoint on the argument LBL(())
Ddd*toolbar*setPC.documentationString: \
@rm Set the current execution position to LBL(())

define(PRINT_HELP, [\
DESC(Print *(), [print dereferenced argument])\n\
DESC(Whatis (), [print type of argument])\n\
DESC(Examine (), [examine memory starting at argument])
])dnl

Ddd*toolbar*print.helpString:	\
LBL(Print ())\n\
\n\
Print the argument LBL(()) in the @GDB@ console.\n\
\n\
ANNOUNCE_PULLDOWN\n\
PRINT_HELP
Ddd*printMenu*helpString:	PRINT_HELP

Ddd*toolbar*print.tipString:	\
@rm Print LBL(()) in the debugger console
Ddd*toolbar*print.documentationString:	\
@rm Print the argument LBL(()) in the @GDB@ console MORE_PULLDOWN()
Ddd*toolbar*printRef.documentationString:	\
@rm Print the dereferenced argument LBL(()) in the @GDB@ console
Ddd*toolbar*dump.documentationString:	\
@rm Print the detailed argument LBL(()) in the @GDB@ console
Ddd*toolbar*whatis.documentationString:	\
@rm Print the type of the argument LBL(()) in the @GDB@ console
Ddd*toolbar*examine.documentationString:	\
@rm Examine memory starting at LBL(())

define(DISPLAY_HELP, [\
DESC(Display *(), [display dereferenced argument])
])dnl

Ddd*toolbar*display.helpString:	\
LBL(Display ())\n\
\n\
Display the argument LBL(()) in the data window.\n\
\n\
ANNOUNCE_PULLDOWN\n\
DISPLAY_HELP
Ddd*displayMenu*helpString:	DISPLAY_HELP


Ddd*toolbar*display.tipString:	\
@rm Display LBL(()) in the data window
Ddd*toolbar*display.documentationString:	\
@rm Display the argument LBL(()) in the data window MORE_PULLDOWN()
Ddd*toolbar*dispRef.documentationString:	\
@rm Display the dereferenced argument LBL(()) in the data window

define(WATCH_HELP, [\
DESC(Watchpoint Properties..., [set properties of watchpoint on LBL(())])\n\
DESC(Set watchpoint on (), [stop whenever LBL(()) changes; same as LBL(Watch())])\n\
DESC(Set read watchpoint on (), [stop whenever LBL(()) is read])\n\
DESC(Set access watchpoint on (), [stop whenever LBL(()) is either read or written])\n\
\n\
Unless you have special hardware support, watchpoints slow down the\n\
debugged program by about two orders of magnitude.
])dnl

Ddd*toolbar*watch.helpString:	\
LBL(Watch ()) / LBL(Unwatch())\n\
\n\
Stop whenever the value of LBL(()) changes.\n\
\n\
LBL(Watch ()) sets a EMPH(watchpoint) on LBL(()) - a special breakpoint that\n\
stops your program whenever the value of LBL(()) changes.\n\
LBL(Unwatch()) deletes the EMPH(watchpoint) associated with LBL(()).\n\
\n\
To see all watchpoints, select LBL(Data, Edit Watchpoints).\n\
\n\
ANNOUNCE_PULLDOWN\n\
WATCH_HELP
Ddd*watchMenu*helpString:	WATCH_HELP

Ddd*toolbar*watch.tipString:	\
@rm Stop whenever LBL(()) changes
Ddd*toolbar*watch.documentationString:	\
@rm Stop whenever the value of LBL(()) changes MORE_PULLDOWN()
Ddd*toolbar*cwatch.documentationString:	\
@rm Stop whenever the value of LBL(()) changes
Ddd*toolbar*rwatch.documentationString:	\
@rm Stop whenever LBL(()) is read (requires hardware support)
Ddd*toolbar*awatch.documentationString:	\
@rm Stop whenever LBL(()) is either read or written (requires hardware support)

define(FIND_HELP, [\
DESC(LBL_FIND_BACKWARD, [find backwards])\n\
DESC(LBL_FIND_FORWARD, [find forwards])
])dnl

Ddd*toolbar*find.helpString:	\
LBL(LBL_FIND_BACKWARD / LBL_FIND_FORWARD)\n\
\n\
Search an occurrence of LBL(()) in the current source text.\n\
\n\
See LBL(Edit, Preferences, Source) for search settings.\n\
\n\
ANNOUNCE_PULLDOWN\n\
FIND_HELP
Ddd*findMenu*helpString:	FIND_HELP

Ddd*toolbar*find.tipString:	\
@rm Find LBL(()) in source
Ddd*toolbar*find.documentationString:	\
@rm Search LBL(()) in the current source MORE_PULLDOWN()
Ddd*toolbar*findBackward.documentationString:	\
@rm Search the previous occurrence of LBL(()) in the source
Ddd*toolbar*findForward.documentationString:	\
@rm Search the next occurrence of LBL(()) in the source


!-----------------------------------------------------------------------------
! Status line
!-----------------------------------------------------------------------------

Ddd*status_form*helpString: \
WIDGET(Status Line)\n\
\n\
The status line shows the last @GDB@ message as well as short DDD messages.\n\
To view tthe most recent messages, just click on the status line.\n\
\n\
The @GDB@ status indicator on the right side blinks while @GDB@ is busy.\n\
You can disable it by clicking on the indicator.

Ddd*status_form*borderWidth:		0
Ddd*status_form*marginWidth:		0
Ddd*status_form*marginHeight:		0
Ddd*status_form*marginTop:		0
Ddd*status_form*marginBottom:		0
Ddd*status_form*spacing:		0
Ddd*status_form*highlightThickness: 	0

Ddd*status_form.arrow.tipString: \
@rm Show recent messages
Ddd*status_form.arrow.documentationString: \n\n

Ddd*status_form.status.alignment:	XmALIGNMENT_BEGINNING
Ddd*status_form.status.leftOffset:	5
Ddd*status_form.status.rightOffset:	5
Ddd*status_form.status.labelString:	Ready.
Ddd*status_form.status.fillOnArm:	off

Ddd*status_form.led.highlightThickness: 0
Ddd*status_form.led.labelString:
Ddd*status_form.led.visibleWhenOff:	off
Ddd*status_form.led.selectColor:	RUN_COLOR
Ddd*status_form.led.tipString:	\
@rm @GDB@ status
Ddd*status_form.led.documentationString: \
@rm @GDB@ status indicator: blinks while @GDB@ is busy \
(click to disable or enable)

Ddd*status_lines.marginWidth:		0
Ddd*status_lines.marginHeight:		0
Ddd*status_lines.borderWidth:		0
Ddd*status_lines.spacing:		0

Ddd*status_history.row.background:	Black


!-----------------------------------------------------------------------------
! Source View
!-----------------------------------------------------------------------------

Ddd*source_text_w.scrollHorizontal:	off
Ddd*source_text_w.wordWrap:		on

Ddd*code_text_w.scrollHorizontal:	off
Ddd*code_text_w.wordWrap:		on

Ddd*source_text_w.helpString:	\
WIDGET(Source Text Window)\n\
\n\
The source text window shows the source text of the debugged program.\n\
\n\
STRONG(Breakpoint area)\n\
On the left of the source code, line numbers and breakpoints are shown.\n\
ITEM A plain stop sign or TEXT([#]VAR(n)[#]) indicates \
an enabled breakpoint.\n\
ITEM A grey stop sign or TEXT([_]VAR(n)[_]) indicates \
a disabled breakpoint.\n\
ITEM The next executed line is indicated by an arrow or TEXT(>).\n\
\n\
Click BUTTON(1) to select positions.\n\
Click BUTTON(3) to set and modify breakpoints.\n\
\n\
STRONG(Source text area)\n\
On the right side, the source code is shown.\n\
\n\
Click BUTTON(1) to select words.\n\
Drag with BUTTON(1) to extend your selection.\n\
Click BUTTON(3) to show values and lookup places.\n\
\n\
To view a specific source, use the LBL(Lookup ()) button below.

Ddd*code_text_w.helpString:	\
WIDGET(Machine Code Window)\n\
\n\
The machine code window shows the machine code of the debugged program.\n\
\n\
STRONG(Breakpoint area)\n\
On the left of the machine code, addresses and breakpoints are shown.\n\
ITEM A plain stop sign or TEXT([#]VAR(n)[#]) indicates \
an enabled breakpoint.\n\
ITEM A grey stop sign or TEXT([_]VAR(n)[_]) indicates \
a disabled breakpoint.\n\
ITEM The next executed location is indicated by an arrow or TEXT(>).\n\
\n\
Click BUTTON(1) to select addresses.\n\
Click BUTTON(3) to set and modify breakpoints.\n\
\n\
STRONG(Machine code area)\n\
On the right side, the machine code instructions are shown.\n\
\n\
Click BUTTON(1) to select words.\n\
Drag with BUTTON(1) to extend your selection.\n\
Click BUTTON(3) to show values and lookup places.\n\
\n\
To disassemble a specific function or address,\n\
use the LBL(Lookup ()) button below.

Ddd*source_view_shell.helpString:	\
WIDGET(Source Window)\n\
\n\
The source window displays the source and machine code of the \
debugged program.\n\
\n\
The upper area shows the source code.\n\
The lower area shows the machine code.\n\
\n\
Use LBL(Help, On Item) to get\n\
detailed help on each of these areas.


!-----------------------------------------------------------------------------
! Source Popup Menus
!-----------------------------------------------------------------------------

Ddd*line_popup.set.labelString:			Set Breakpoint
Ddd*line_popup.set.documentationString:	\
@rm Set a breakpoint at the selected position

Ddd*line_popup.set_temp.labelString:		Set Temporary Breakpoint
Ddd*line_popup.set_temp.documentationString:	\
@rm Set a temporary breakpoint which will be deleted when hit

Ddd*line_popup.temp_n_cont.labelString:		Continue Until Here
Ddd*line_popup.temp_n_cont.documentationString:	\
@rm Set a temporary breakpoint and resume execution

Ddd*line_popup.set_pc.labelString:		Set Execution Position
Ddd*line_popup.set_pc.documentationString:	\
@rm Set the current execution position to the selected position

Ddd*bp_popup.disable.labelString:		Disable Breakpoint
Ddd*bp_popup.disable.documentationString:	\
@rm Disable the selected breakpoint

Ddd*bp_popup.properties.labelString:		Properties...
Ddd*bp_popup.properties.documentationString:	\
@rm Edit the properties of the selected breakpoint

Ddd*bp_popup.delete.labelString:		Delete Breakpoint
Ddd*bp_popup.delete.documentationString:	\
@rm Delete the selected breakpoint

Ddd*bp_popup.set_pc.labelString:		Set Execution Position
Ddd*bp_popup.set_pc.documentationString:	\
@rm Set the current execution position to the selected position

Ddd*text_popup.print.documentationString: \
@rm Print the value of the selected item in the @GDB@ console

Ddd*text_popup.disp.documentationString: \
@rm Display the selected item in the data window

Ddd*text_popup.watch.documentationString: \
@rm Stop execution whenever the selected item changes

Ddd*text_popup.printRef.documentationString: \
@rm Print the dereferenced value of the selected item in the @GDB@ console

Ddd*text_popup.dump.documentationString: \
@rm Print the detailed value of the selected item in the @GDB@ console

Ddd*text_popup.dispRef.documentationString: \
@rm Display the dereferenced item in the data window

Ddd*text_popup.watchRef.documentationString: \
@rm Stop execution whenever the dereferenced item changes

Ddd*text_popup.whatis.documentationString: \
@rm Print the type of the selected item in the @GDB@ console

Ddd*text_popup.lookup.documentationString: \
@rm Lookup definition of the selected item

Ddd*text_popup.breakAt.documentationString: \
@rm Set a breakpoint at the selected function

Ddd*text_popup.clearAt.documentationString: \
@rm Delete a breakpoint at the selected function


!-----------------------------------------------------------------------------
! Source and Code Glyphs
!-----------------------------------------------------------------------------

Ddd*source_form_w.XmPushButton.shadowThickness:	   0
Ddd*source_form_w.XmPushButton.highlightThickness: 0
Ddd*source_form_w.XmPushButton.marginHeight:	   0
Ddd*source_form_w.XmPushButton.marginWidth:	   0
Ddd*source_form_w.XmPushButton.borderWidth:	   0

Ddd*code_form_w.XmPushButton.shadowThickness:	   0
Ddd*code_form_w.XmPushButton.highlightThickness:   0
Ddd*code_form_w.XmPushButton.marginHeight:	   0
Ddd*code_form_w.XmPushButton.marginWidth:	   0
Ddd*code_form_w.XmPushButton.borderWidth:	   0

! Plain arrows
Ddd*source_form_w.plain_arrow*helpString: \
STRONG(Current Execution Position)\n\
The plain arrow shows the current execution position in the lowest frame.\n\
\n\
To change the execution position, drag it with BUTTON(1).
Ddd*source_form_w.plain_arrow*tipString: \
@rm Current execution position
Ddd*source_form_w.plain_arrow*documentationString: \
@rm Current execution position (drag to change)

Ddd*code_form_w.plain_arrow*helpString: \
STRONG(Current Program Counter)\n\
The plain arrow shows the current program counter in the lowest frame.\n\
\n\
To change the program counter, drag it with BUTTON(1).
Ddd*code_form_w.plain_arrow*tipString: \
@rm Current program counter
Ddd*code_form_w.plain_arrow*documentationString: \
@rm Current program counter (drag to change)

! Grey arrows
Ddd*source_form_w.grey_arrow*helpString: \
STRONG(Last Execution Position)\n\
The grey arrow shows the last execution position in the current frame.
Ddd*source_form_w.grey_arrow*tipString: \
@rm Last execution position in current frame

Ddd*code_form_w.grey_arrow*helpString: \
STRONG(Last Program Counter)\n\
The grey arrow shows the last program counter in the current frame.
Ddd*code_form_w.grey_arrow*tipString: \
@rm Last program counter in current frame

! Dashed arrows
Ddd*source_form_w.past_arrow*helpString: \
STRONG(Earlier Execution Position)\n\
The dashed arrow shows an earlier execution position.
Ddd*source_form_w.past_arrow*tipString: \
@rm Earlier execution position

Ddd*code_form_w.past_arrow*helpString: \
STRONG(Earlier Program Counter)\n\
The dashed arrow shows an earlier program counter.
Ddd*code_form_w.past_arrow*tipString: \
@rm Earlier program counter

! Signal arrows
Ddd*source_form_w.signal_arrow*helpString: \
STRONG(Current Execution Position (signaled))\n\
The lightning arrow shows the current execution position after being\n\
stopped by a signal.\n\
\n\
To change the execution position, drag it with BUTTON(1).
Ddd*source_form_w.signal_arrow*tipString: \
@rm Current execution position (stopped by signal)

Ddd*code_form_w.signal_arrow*helpString: \
STRONG(Current Program Counter (signaled))\n\
The lightning arrow shows the current program counter after being\n\
stopped by a signal.\n\
\n\
To change the program counter, drag it with BUTTON(1).
Ddd*code_form_w.signal_arrow*tipString: \
@rm Current program counter (stopped by signal)

! Breakpoints
define(BREAKPOINT_HELP, [\n\
To change the breakpoint properties, press BUTTON(3).\n\
To move it to another location, drag it using BUTTON(1).\n\
To copy it to another location, drag it using KEY(Shift)+BUTTON(1).])

Ddd*plain_stop.helpString: \
STRONG(Breakpoint (enabled))\n\
The debugged program will stop when reaching this location.\n\
BREAKPOINT_HELP
Ddd*plain_stop.tipString: 
Ddd*plain_stop.documentationString: 

Ddd*grey_stop.helpString: \
STRONG(Breakpoint (disabled))\n\
A disabled breakpoint has no effect until reenabled.\n\
BREAKPOINT_HELP
Ddd*grey_stop.tipString: 
Ddd*grey_stop.documentationString: 

Ddd*plain_cond.helpString: \
STRONG(Conditional breakpoint (enabled))\n\
The debugged program will stop when reaching this location\n\
if the associated condition is true.\n\
BREAKPOINT_HELP
Ddd*plain_cond.tipString: 
Ddd*plain_cond.documentationString: 

Ddd*grey_cond.helpString: \
STRONG(Conditional breakpoint (disabled))\n\
A disabled breakpoint has no effect until reenabled.\n\
BREAKPOINT_HELP
Ddd*grey_cond.tipString: 
Ddd*grey_cond.documentationString: 

Ddd*plain_temp.helpString: \
STRONG(Temporary breakpoint (enabled))\n\
The debugged program will stop when reaching this location.\n\
The breakpoint will be deleted when hit.\n\
BREAKPOINT_HELP
Ddd*plain_temp.tipString: 
Ddd*plain_temp.documentationString: 

Ddd*grey_temp.helpString: \
STRONG(Temporary breakpoint (disabled))\n\
A disabled breakpoint has no effect until reenabled.\n\
BREAKPOINT_HELP
Ddd*grey_temp.tipString: 
Ddd*grey_temp.documentationString: 



!-----------------------------------------------------------------------------
! Main Window
!-----------------------------------------------------------------------------

define(MAIN_WINDOW_HELP, [\
WIDGET([DDD] Main Window)\n\
\n\
The main window consists of the following areas:\n\
\n\
ITEM At the top, the EMPH(Data Window) shows the data displays\n\
    of the debugged program.\n\
ITEM In the center, the EMPH(Source Text Window) shows the \
currently\n\
    executed source code.\n\
ITEM Optionally, there may be a EMPH(Machine Code Window) below\n\
    which shows the current machine code.\n\
ITEM At the bottom, the EMPH(@GDB@ Console) allows for \
interaction\n\
    with @GDB@ and the debugged program.\n\
\n\
Use LBL(Help, On Item) to get detailed \
help on any of these areas.])dnl

Ddd.helpString:	            MAIN_WINDOW_HELP
Ddd.main_window.helpString: MAIN_WINDOW_HELP



!-----------------------------------------------------------------------------
! Debugger Console
!-----------------------------------------------------------------------------

Ddd*gdb_w.value: 
Ddd*gdb_w.editable:			on
Ddd*gdb_w.allowResize:			on
Ddd*gdb_w.editMode:			XmMULTI_LINE_EDIT
Ddd*gdb_w.scrollHorizontal:		off
Ddd*gdb_w.wordWrap:			on
Ddd*gdb_w.autoShowCursorPosition:	on
Ddd*gdb_w.cursorPositionVisible:	on

define(CONSOLE_HELP,[\
WIDGET(@GDB@ Console)\n\
\n\
The @GDB@ console shows the @GDB@ input and output\n\
as well as the input and output of the debugged program.\n\
\n\
You can enter @GDB@ commands here.\n\
\n\
To get a list of @GDB@ commands, enter KBD(help) at the @GDB@ prompt.\n\
See the LBL(Commands) menu for the most important editing commands.
])dnl

Ddd*gdb_w.helpString:		CONSOLE_HELP
Ddd*command_shell.helpString:	CONSOLE_HELP


!-----------------------------------------------------------------------------
! Console Popup Menu
!-----------------------------------------------------------------------------

Ddd*gdb_popup.clear_line.labelString:		Clear Line
Ddd*gdb_popup.clear_line.documentationString:	\
@rm Clear @GDB@ command line

Ddd*gdb_popup.clear_window.labelString:		Clear Window
Ddd*gdb_popup.clear_window.documentationString:	\
@rm Clear all before last @GDB@ prompt


!-----------------------------------------------------------------------------
! Command Tool
!-----------------------------------------------------------------------------

! The command tool is frequently moved around on top of other windows.
! Hence, avoid extra redisplays.
Ddd*tool_shell*saveUnder: on

Ddd*tool_shell.helpString:		\
WIDGET(Command Tool)\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
Press KEY_HELP on a button to get a short command description.

Ddd*tool_buttons.helpString:\
WIDGET(Command Tool)\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
Press KEY_HELP on a button to get a short command description.


!-----------------------------------------------------------------------------
! Plots
!-----------------------------------------------------------------------------

define(PLOT_WINDOW_HELP, [\
WIDGET(Plot Window)\n\
\n\
This window shows a plot of numerical arrays and/or scalars\n\
of the debugged program.\n\
\n\
In a 2-D plot, a point at X/Y shows the value Y at the array index X.\n\
In a 3-D plot, a point at X/Y/Z shows the value Z at the array index X/Y.\n\
Scalars are shown as lines and can be rotated using the LBL(Rotate ()) button.\n\
\n\
You can modify the plot appearance via the options in the menu bar.\n\
LBL(File, Command) gives you direct access to the Gnuplot program.\n\
\n\
Use LBL(File, Close) to close this window.
])dnl

Ddd*plot.helpString:            PLOT_WINDOW_HELP
Ddd*plot*swallower.helpString:  PLOT_WINDOW_HELP
Ddd*plot*area.helpString:       PLOT_WINDOW_HELP

Ddd*plot*replot.labelString:    Refresh Plot
Ddd*plot*replot.mnemonic:       R
Ddd*plot*replot.accelerator:	~Shift ~Meta Ctrl<Key>L
Ddd*plot*replot.acceleratorText: Ctrl+L
Ddd*plot*replot.documentationString:   \
@rm Refresh plot, using current values

Ddd*plot*command.labelString:    Command...
Ddd*plot*command.mnemonic:       o
Ddd*plot*command.documentationString:   \
@rm Enter plotting command

Ddd*plot*export.labelString:    Save Data As...
Ddd*plot*export.mnemonic:       S
Ddd*plot*export.accelerator:	~Shift ~Meta Ctrl<Key>S
Ddd*plot*export.acceleratorText: Ctrl+S
Ddd*plot*export.documentationString:   \
@rm Write plotted data to file

Ddd*plot*print.labelString: 	Print Plot...
Ddd*plot*print.mnemonic:        P
Ddd*plot*print.documentationString:   \
@rm Print the current plot

Ddd*plot*menubar.helpString:		\
WIDGET(Menu Bar)\n\
\n\
DESC(File, [print plot and close this window])\n\
DESC(Edit, [cut, copy, and paste text])\n\
DESC(View, [set plot options])\n\
DESC(Plot, [set plot style])\n\
DESC(Scale, [set scale style])\n\
DESC(Contour, [set contour style])\n\
DESC(Help, [on-line help and version information])

Ddd*menubar.plotView.labelString:  	View
Ddd*menubar.plotView.mnemonic:     	V
Ddd*menubar.plotView.documentationString:     	\
@rm Set plot options

define(PLOT_VIEW_HELP, [\
WIDGET(View Menu)\n\
\n\
DESC(Border, [display border])\n\
DESC(Time, [display time of plot])\n\
\n\
DESC(Grid, [display grid])\n\
DESC(X Zero Axis, [display X zero axis])\n\
DESC(Y Zero Axis, [display Y zero axis])
])dnl

Ddd*menubar.plotView.helpString: PLOT_VIEW_HELP
Ddd*plotViewMenu*helpString:     PLOT_VIEW_HELP
Ddd*plotViewMenu*tearOffTitle:   View

Ddd*plotViewMenu.border.labelString:	Border
Ddd*plotViewMenu.border.mnemonic:	B
Ddd*plotViewMenu.border.documentationString:	\
@rm Toggle border

Ddd*plotViewMenu.grid.labelString:	Grid
Ddd*plotViewMenu.grid.mnemonic:		G
Ddd*plotViewMenu.grid.documentationString:	\
@rm Toggle grid

Ddd*plotViewMenu.time.labelString:	Time
Ddd*plotViewMenu.time.mnemonic:      	T
Ddd*plotViewMenu.time.documentationString:	\
@rm Toggle plot time

Ddd*plotViewMenu.xzeroaxis.labelString:	X Zero Axis
Ddd*plotViewMenu.xzeroaxis.mnemonic:	X
Ddd*plotViewMenu.xzeroaxis.documentationString:	\
@rm Toggle X zero axis

Ddd*plotViewMenu.yzeroaxis.labelString:	Y Zero Axis
Ddd*plotViewMenu.yzeroaxis.mnemonic:	Y
Ddd*plotViewMenu.yzeroaxis.documentationString:	\
@rm Toggle Y zero axis



Ddd*menubar.plot.labelString:  		Plot
Ddd*menubar.plot.mnemonic:     		P
Ddd*menubar.plot.documentationString:   \
@rm Set plotting style

define(PLOT_HELP, [\
\
WIDGET(Plot Menu)\n\
\n\
DESC(Points, [display a small symbol at each point])\n\
DESC(Lines, [connect adjacent points with lines])\n\
DESC(3-D Lines, [remove hidden lines, creating a 3-D effect])\n\
DESC(Points and Lines, [do both Points and Lines])\n\
DESC(Impulses, [draw a vertical line from the base to each point])\n\
DESC(Dots, [plot a tiny dot at each point])\n\
DESC(Steps, [connect adjacent points with two line segments])\n\
DESC(Boxes, [draw a vertical box from the x axis to each point])
])dnl

Ddd*menubar.plot.helpString:	PLOT_HELP
Ddd*plotMenu*helpString:	PLOT_HELP
Ddd*plotMenu*tearOffTitle:	Plot

Ddd*plotMenu.points.labelString:	Points
Ddd*plotMenu.points.mnemonic:		P
Ddd*plotMenu.points.documentationString:	\
@rm Display a small symbol at each point

Ddd*plotMenu.lines.labelString:	  	Lines
Ddd*plotMenu.lines.mnemonic:		L
Ddd*plotMenu.lines.documentationString:	\
@rm Connect adjacent points with lines

Ddd*plotMenu.lines3d.labelString:	3-D Lines
Ddd*plotMenu.lines3d.mnemonic:		3
Ddd*plotMenu.lines3d.documentationString:	\
@rm Remove hidden lines, creating a 3-D effect

Ddd*plotMenu.linespoints.labelString:	Points and Lines
Ddd*plotMenu.linespoints.mnemonic:	a
Ddd*plotMenu.linespoints.documentationString:	\
@rm Do both Points and Lines

Ddd*plotMenu.linespoints3d.labelString:	Points and 3-D Lines
Ddd*plotMenu.linespoints3d.mnemonic:	t
Ddd*plotMenu.linespoints3d.documentationString:	\
@rm Do both Points and 3-D Lines

Ddd*plotMenu.impulses.labelString:	Impulses
Ddd*plotMenu.impulses.mnemonic:		I
Ddd*plotMenu.impulses.documentationString:	\
@rm Draw a vertical line from the base to each point

Ddd*plotMenu.dots.labelString:		Dots
Ddd*plotMenu.dots.mnemonic:		D
Ddd*plotMenu.dots.documentationString:	\
@rm Plot a tiny dot at each point

Ddd*plotMenu.steps2d.labelString:	Steps
Ddd*plotMenu.steps2d.mnemonic:		S
Ddd*plotMenu.steps2d.documentationString:	\
@rm Connect adjacent points with two line segments

Ddd*plotMenu.boxes2d.labelString:	Boxes
Ddd*plotMenu.boxes2d.mnemonic:		B
Ddd*plotMenu.boxes2d.documentationString:	\
@rm Draw a vertical box from the x axis to each point


Ddd*menubar.scale.labelString:  	Scale
Ddd*menubar.scale.mnemonic:     	S
Ddd*menubar.scale.documentationString:   \
@rm Set scale style

define(SCALE_HELP, [\
WIDGET(Scale Menu)\n\
\n\
DESC(Logarithmic, [toggle logarithmic scaling])\n\
\n\
DESC(X Tics, [display tics on X scale])\n\
DESC(Y Tics, [display tics on Y scale])\n\
DESC(Z Tics, [display tics on Z scale])
])dnl

Ddd*menubar.scale.helpString: SCALE_HELP
Ddd*scaleMenu*helpString:     SCALE_HELP
Ddd*scaleMenu*tearOffTitle:   Scale

Ddd*scaleMenu.logscale.labelString:	Logarithmic
Ddd*scaleMenu.logscale.mnemonic:	L
Ddd*scaleMenu.logscale.documentationString:	\
@rm Toggle logarithmic scaling

Ddd*scaleMenu.xtics.labelString:	X Tics
Ddd*scaleMenu.xtics.mnemonic:		X
Ddd*scaleMenu.xtics.documentationString:	\
@rm Toggle tics on X scale

Ddd*scaleMenu.ytics.labelString:	Y Tics
Ddd*scaleMenu.ytics.mnemonic:		Y
Ddd*scaleMenu.ytics.documentationString:	\
@rm Toggle tics on Y scale

Ddd*scaleMenu.ztics.labelString:	Z Tics
Ddd*scaleMenu.ztics.mnemonic:		Z
Ddd*scaleMenu.ztics.documentationString:	\
@rm Toggle tics on Z scale


Ddd*menubar.contour.labelString:  	Contour
Ddd*menubar.contour.mnemonic:     	C
Ddd*menubar.contour.documentationString:   \
@rm Set contour style

define(CONTOUR_HELP, [\
WIDGET(Contour Menu)\n\
\n\
DESC(Base, [draw contour on base])\n\
DESC(Surface, [draw contour on surface])
])dnl

Ddd*menubar.contour.helpString: CONTOUR_HELP
Ddd*contourMenu*helpString:     CONTOUR_HELP
Ddd*contourMenu*tearOffTitle:   Contour

Ddd*contourMenu.base.labelString:	Base
Ddd*contourMenu.base.mnemonic:		B
Ddd*contourMenu.base.documentationString:	\
@rm Draw contour on base

Ddd*contourMenu.surface.labelString:	Surface
Ddd*contourMenu.surface.mnemonic:	S
Ddd*contourMenu.surface.documentationString:	\
@rm Draw contour on surface


Ddd*plot_command_dialog.dialogTitle:		DDD: Plot Command
Ddd*plot_command_dialog.cancelLabelString:	Close
Ddd*plot_command.promptString:			gnuplot>
Ddd*plot_command_dialog*helpString:     \
WIDGET(Plot Command)\n\
\n\
You can now enter a Gnuplot command at the SAMP(gnuplot>) prompt.\n\
The plot will automatically be refreshed after execution.\n\
\n\
See the Gnuplot documentation for useful commands.


! The default Gnuplot window is 640x540 pixels wide.
Ddd*plot*area.width:        640
Ddd*plot*area.height:       450
Ddd*plot*area.background:   TEXT_BACKGROUND_COLOR

! Ideally, these values would come from the swallowed window.
! Unfortunately, LessTif has problems with these, so we set them explicitly.
Ddd*plot*swallower.width:        640
Ddd*plot*swallower.height:       450
Ddd*plot*swallower.background:   TEXT_BACKGROUND_COLOR

Ddd*plot*XmScrollBar.borderWidth:		0
Ddd*plot*XmScrollBar.highlightThickness:        0


!-----------------------------------------------------------------------------
! Debugger Buttons
!-----------------------------------------------------------------------------

Ddd*source_buttons.helpString:\
WIDGET(Command Area)\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
Press KEY_HELP on a button to get a short command description.

Ddd*command_buttons.helpString:\
WIDGET(Command Area)\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
Press KEY_HELP on a button to get a short command description.

Ddd*data_buttons.helpString:\
WIDGET(Command Area)\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
Press KEY_HELP on a button to get a short command description.

! An empty help string means that the help text for this button
! is to be requested from GDB (via the GDB `help' command).
Ddd*source_buttons*XmPushButton.helpString:
Ddd*console_buttons*XmPushButton.helpString:
Ddd*data_buttons*XmPushButton.helpString:
Ddd*tool_buttons*XmPushButton.helpString:
Ddd*command_toolbar*XmPushButton.helpString:

! Same applies for button tips.
Ddd*source_buttons*XmPushButton.tipString:
Ddd*console_buttons*XmPushButton.tipString:
Ddd*data_buttons*XmPushButton.tipString:
Ddd*tool_buttons*XmPushButton.tipString:
Ddd*command_toolbar*XmPushButton.tipString:

! Special spacing
Ddd*source_buttons.spacing:	    0
Ddd*console_buttons.spacing:	    0
Ddd*data_buttons.spacing:	    0
Ddd*source_buttons.packing:	    XmPACK_TIGHT
Ddd*console_buttons.packing:	    XmPACK_TIGHT
Ddd*data_buttons.packing:	    XmPACK_TIGHT
Ddd*source_buttons.entryAlignment:  XmALIGNMENT_CENTER
Ddd*console_buttons.entryAlignment: XmALIGNMENT_CENTER
Ddd*data_buttons.entryAlignment:    XmALIGNMENT_CENTER
Ddd*source_buttons*alignment:	    XmALIGNMENT_CENTER
Ddd*console_buttons*alignment:	    XmALIGNMENT_CENTER
Ddd*data_buttons*alignment:	    XmALIGNMENT_CENTER

Ddd*source_buttons.marginWidth:     0
Ddd*source_buttons.marginHeight:    0
Ddd*console_buttons.marginWidth:    0
Ddd*console_buttons.marginHeight:   0
Ddd*data_buttons.marginWidth:       0
Ddd*data_buttons.marginHeight:      0

! Some buttons have a special meaning; 
! so we must provide help on ourselves.
Ddd*?*break.helpString: \
LBL(Interrupt)\n\
\n\
Interrupt the debugged process (or current @GDB@ command).\n\
(Equivalent to KEY(Esc)).
Ddd*source_buttons*break.tipString: \
@rm Interrupt debugged program
Ddd*data_buttons*break.tipString: \
@rm Interrupt debugged program
Ddd*console_buttons*break.tipString: \
@rm Interrupt debugged program
Ddd*tool_buttons*break.tipString: \
@rm Interrupt debugged program
Ddd*command_toolbar*break.tipString: \
@rm Interrupt debugged program
Ddd*?*break.documentationString: \
@rm Interrupt the debugged process (or current @GDB@ command)

Ddd*?*Yes.helpString:	\
LBL(Yes)\n\
\n\
Confirm the @GDB@ question.
Ddd*?*Yes.tipString: \
@rm Just say yes

Ddd*?*No.helpString:	\
LBL(No)\n\
\n\
Do EMPH(not) confirm the @GDB@ question.
Ddd*?*No.tipString: \
@rm Just say no

Ddd*?*Complete.helpString:	\
LBL(Complete)\n\
\n\
Complete the current debugging command.  (Equivalent to KEY(TAB)).
Ddd*?*Complete.tipString: \
@rm Complete current command

Ddd*?*Clear.helpString:	\
LBL(Clear)\n\
\n\
Clear the current debugging command.  (Equivalent to KEY(Ctrl+U)).
Ddd*?*Clear.tipString: \
@rm Clear current command

Ddd*?*Prev.helpString:	\
LBL(Prev)\n\
\n\
Get the previous debugging command \
(Equivalent to KEY(Up)).
Ddd*?*Prev.tipString: \
@rm Get previous command

Ddd*?*Next.helpString:	\
LBL(Next)\n\
\n\
Get the next debugging command \
(Equivalent to KEY(Down)).
Ddd*?*Next.tipString: \
@rm Get next command

Ddd*?*Apply.helpString:	\
LBL(Apply)\n\
\n\
Execute the current debugging command.  (Equivalent to KEY_RETURN).
Ddd*?*Apply.tipString: \
@rm Execute current command

Ddd*?*Undo.helpString:	\
LBL(Undo)\n\
\n\
Undo last command.  Almost all commands can be undone this way.\n\
\n\
As a special exception, if the last command affected the state of the\n\
debuggee, DDD does EMPH(not) restore the previous program state.\n\
Instead, DDD goes into EMPH(historic mode) showing you all it remembers\n\
about this state.
Ddd*?*Undo.tipString: 

Ddd*?*Redo.helpString:	\
LBL(Redo)\n\
\n\
Redo next command.  Every command undone can be redone this way.
Ddd*?*Redo.tipString: 

Ddd*?*Edit.helpString:	\
LBL(Edit)\n\
\n\
Invoke an editor for the current source file.
Ddd*?*Edit.tipString: \
@rm Edit source file
Ddd*?*Edit.documentationString: \
@rm Invoke text editor for the current source

Ddd*?*Make.helpString:	\
LBL(Make)\n\
\n\
Run the make program with the most recently given arguments.
Ddd*?*Make.tipString: \
@rm Run the make program
Ddd*?*Make.documentationString: \
@rm Run the make program with the most recently given arguments.

Ddd*?*Reload.helpString:	\
LBL(Reload)\n\
\n\
Reload the source from file.
Ddd*?*Reload.tipString: \
@rm Reload source file
Ddd*?*Reload.documentationString: \
@rm Reload current source file




!-----------------------------------------------------------------------------
! Print Dialog
!-----------------------------------------------------------------------------

Ddd*print.dialogTitle: DDD: Print
Ddd*print.okLabelString: Print

Ddd*print*to.labelString:		      Print To
Ddd*print*toMenu.printer.labelString: 	      \
\ \ \ \ \ \ \ \ \ \ \ Printer\ \ \ \ \ \ \ \ \ \ \ 
Ddd*print*toMenu.file.labelString:    	      File

Ddd*print*toMenu.?.indicatorOn:               off
Ddd*print*toMenu.?.shadowThickness: 	      2
Ddd*print*toMenu.entryAlignment: 	      XmALIGNMENT_CENTER

Ddd*print*command.label.labelString:	      Print Command
Ddd*print*command.text.columns:	      	      40

Ddd*print*name.labelString:	      	      File Name
Ddd*print*name.text.columns:	      	      29
Ddd*print*name.text.value:	      	      ddd()graph.ps

Ddd*print*browse.labelString:	      	      Browse...

Ddd*print*type.labelString:	      	      File Type
Ddd*print*postscript.labelString:	      PostScript
Ddd*print*xfig.labelString:		      FIG

Ddd*print*what.labelString:	      	      Print
Ddd*print*displays.labelString:		      Displays
Ddd*print*plots.labelString:		      Plots
Ddd*print*selected.labelString:		      Selected Only
Ddd*print*color.labelString:		      Color

Ddd*print*orientation.labelString:            Orientation
Ddd*print*portrait.labelString:		      Portrait
Ddd*print*landscape.labelString:	      Landscape

Ddd*print*size.labelString:	      	      Paper Size
Ddd*print*sizeMenu.orientation:	      	      XmVERTICAL
Ddd*print*sizeMenu.numColumns:	      	      3
Ddd*print*a4.labelString:		      A4 (210mm TIMES 297mm)
Ddd*print*a3.labelString:		      A3 (297mm TIMES 420mm)
Ddd*print*letter.labelString:		      Letter (8ONE_HALF" TIMES 11")
Ddd*print*legal.labelString:		      Legal (8ONE_HALF" TIMES 14")
Ddd*print*executive.labelString:	      Executive (7ONE_HALF" TIMES 10")
Ddd*print*custom.labelString:		      Other...

Ddd*print*helpString: \
@rm You can print on a POSTSCRIPT printer or print to a file.\n\
Enter the print command or the file name in the appropriate fields.\n\
\n\
Files can be created in the following formats:\n\
DESC(PostScript, POSTSCRIPT format)\n\
DESC(FIG, [FIG format (for the CODE(xfig) program)])\n\
\n\
Select LBL(Print Displays) to print displays.\n\
Select LBL(Print Plots) to print the current plots.\n\
\n\
To print selected items only, select LBL(Selected Only).\n\
For POSTSCRIPT format only, you can also specify orientation and paper size.\n\
\n\
To print, click on LBL(Print).

Ddd*paper_size_dialog.dialogTitle:  		DDD: Paper Size
Ddd*paper_size_dialog.selectionLabelString: 	Paper size
Ddd*paper_size_dialog.textAccelerators:
Ddd*paper_size_dialog*helpString: \
@rm Please enter the paper size in the format \
KBD(VAR(width) CODE(x) VAR(height)).\n\
\n\
Examples:\n\
KBD(42cm x 59.4cm) DASH A2 paper\n\
KBD(7.5in x 10in) DASH Executive paper\n\
\n\
Supported units for VAR(width) and VAR(height) include:\n\
KBD(pt) (points), KBD(in) (inches),\n\
KBD(mm) (millimeters), KBD(cm) (centimeters).

Ddd*browse_print.dialogTitle:       DDD: Print to File
Ddd*browse_print.selectionLabelString: Print to File
Ddd*browse_print*helpString:	   \
@rm You can select the file to print to.\n\
\n\
Click on LBL(OK) to set the file name.\n\
Click on LBL(Filter) to apply the given filter.



!-----------------------------------------------------------------------------
! File Selection Dialogs
!-----------------------------------------------------------------------------

Ddd*exec_files.dialogTitle:		   DDD: Open Program
Ddd*exec_files.okLabelString:	   Open
Ddd*exec_files.selectionLabelString: Program
Ddd*exec_files*helpString:	   \
WIDGET(Open Program)\n\
\n\
The dialog shows executable files in the current directory.\n\
Enter the name of the program to be debugged in the argument field.\n\
\n\
The program is read for its symbols, for getting the contents\
 of pure memory,\n\
and it is the program executed when you use the KBD(run) command.\n\
If the program file cannot be found as specified,\
 your execution directory path\n\
(CODE($PATH)) is searched for a command of that name.\n\
No argument means to have no executable file and no symbols.\n\
\n\
Click on LBL(Open) to open the selected file.\n\
Click on LBL(Filter) to apply the given filter.

Ddd*core_files.dialogTitle:		   DDD: Open Core Dump
Ddd*core_files.okLabelString:	   Open
Ddd*core_files.selectionLabelString: Core Dump
Ddd*core_files*helpString:	   \
WIDGET(Open Core Dump)\n\
\n\
The dialog shows core dumps in the current directory.\n\
Enter the name of a core dump in the argument field.\n\
\n\
The core dump is used for examining memory and registers.\n\
No argument means to have no core file.\n\
\n\
Before using LBL(Open Core), you should use LBL(File, Open Program)\n\
to specify the program causing the core dump, and to load its symbol table.\n\
\n\
Click on LBL(Open) to open the selected core dump.\n\
Click on LBL(Filter) to apply the given filter.

Ddd*source_files.dialogTitle:		   DDD: Open Source
Ddd*source_files.okLabelString:	   Open
Ddd*source_files.selectionLabelString: Source File
Ddd*source_files*helpString:    \
WIDGET(Open Source)\n\
\n\
The dialog shows source files in the current directory.\n\
Enter the name of a source file in the argument field.\n\
\n\
The source file is shown in the source window and may be used\n\
for setting or clearing breakpoints.\n\
\n\
Before using LBL(Open Source), you should use LBL(File, Open Program)\n\
to specify the executable program, and to load its symbol table.\n\
\n\
Click on LBL(Open) to open the selected source file.\n\
Click on LBL(Filter) to apply the given filter.

Ddd*processes.dialogTitle: DDD: Attach to Process
Ddd*processes.listLabelString:	Processes
Ddd*processes.selectionPolicy:	XmSINGLE_SELECT

Ddd*processes.okLabelString:	Attach
Ddd*processes.applyLabelString:	Update

Ddd*processes*helpString:	\
WIDGET(Attach to Process)\n\
\n\
The dialog shows the currently running processes.\n\
Select a running process of the program to be debugged.\n\
\n\
An attached process is stopped by DDD. You can examine and modify an\n\
attached process with all the DDD commands that are ordinarily available\n\
when you start processes with LBL(Run). You can insert breakpoints; you \n\
can step and continue; you can modify storage.  If you would rather the\n\
process continue running, you may use the LBL(Continue) command after\n\
attaching DDD to the process.\n\
\n\
When you have finished debugging the attached process, you can use\n\
LBL(Detach Process) to release it from DDD control.  Detaching \
the process\n\
continues its execution.\n\
\n\
Before using LBL(Attach), you should use LBL(File, Open Program)\n\
to specify the program running in the process, and to load its symbol table.\n\
\n\
Click on LBL(Attach) to attach to the selected process.\n\
Click on LBL(Update) to update the list of processes.


Ddd*classes.dialogTitle: 		DDD: Open Class
Ddd*classes.listLabelString:		Classes
Ddd*classes.selectionLabelString: 	Open Class
Ddd*classes*selectionPolicy:		XmSINGLE_SELECT
Ddd*classes*okLabelString:	 	Open
Ddd*classes*applyLabelString:	 	Update
Ddd*classes*helpString:	\
WIDGET(Open Class)\n\
\n\
The dialog shows the currently accessible Java classes.\n\
Please select a class to be debugged.\n\
\n\
These classes all have corresponding CODE(.java) sources in the class path.\n\
To search for other classes, select LBL(Edit, JDB Settings)\n\
and add their directories to the class path.\n\
\n\
Click on LBL(Open) to open the selected class.\n\
Click on LBL(Update) to scan the class path once more.


Ddd*sources.dialogTitle: 		DDD: Open Source
Ddd*sources.listLabelString:		Sources
Ddd*sources.selectionLabelString: 	Open Source
Ddd*sources*label.labelString:		Filter
Ddd*sources*sharedlibrary.labelString:  Load Shared Object Library Symbols
Ddd*sources*selectionPolicy:		XmSINGLE_SELECT
Ddd*sources*okLabelString:	 	Open
Ddd*sources*lookup.labelString:		Lookup
Ddd*sources*applyLabelString:	 	Filter
Ddd*sources*helpString:	\
WIDGET(Open Source)\n\
\n\
The dialog shows the currently accessible source files of your program.\n\
Please select a source to be opened.\n\
\n\
Source files from shared libraries are only shown after the library\n\
has been loaded into @GDB@.  To load shared libraries, click on\n\
LBL(Load Shared Object Library Symbols).\n\
\n\
To search for other sources, select LBL(Edit, @GDB@ Settings)\n\
and add their directories to the source path.\n\
\n\
Click on LBL(Open) to open the selected source and close this window.\n\
Click on LBL(Lookup) to lookup the selected source.\n\
Click on LBL(Filter) to apply the given filter.


Ddd*sessions_to_open.dialogTitle: 		DDD: Open Session
Ddd*sessions_to_open.listLabelString:		Sessions
Ddd*sessions_to_open.selectionLabelString: 	Open Session
Ddd*sessions_to_open*selectionPolicy:		XmSINGLE_SELECT
Ddd*sessions_to_open*okLabelString:	 	Open
Ddd*sessions_to_open*applyLabelString:	 	Delete
Ddd*sessions_to_open*helpString:	\
WIDGET(Open Session)\n\
\n\
The dialog shows your DDD sessions.\n\
Enter the name of a DDD session in the argument field.\n\
\n\
A DDD session records the state of a debugging session,\n\
such that you can exit debugging sessions and resume later.\n\
\n\
Click on LBL(Open) to open the selected session.\n\
Click on LBL(Delete) to delete the selected session.


Ddd*sessions_to_save.dialogTitle: 		DDD: Save Session
Ddd*sessions_to_save.listLabelString:		Sessions
Ddd*sessions_to_save.selectionLabelString: 	Save Session
Ddd*sessions_to_save*selectionPolicy:		XmSINGLE_SELECT
Ddd*sessions_to_save*okLabelString:	 	Save
Ddd*sessions_to_save*applyLabelString:	 	Delete
Ddd*sessions_to_save*dump.labelString:		Include Core Dump
Ddd*sessions_to_save*dump.alignment:		XmALIGNMENT_BEGINNING
Ddd*sessions_to_save*method.labelString:	via
Ddd*sessions_to_save*kill.labelString:		Killing the Debuggee
Ddd*sessions_to_save*ptrace.labelString:	The ptrace() call
Ddd*sessions_to_save*gcore.labelString:		The `gcore' Command

Ddd*sessions_to_save*helpString:	\
WIDGET(Save Session)\n\
\n\
The dialog shows your DDD sessions.\n\
Enter the name of a DDD session in the argument field.\n\
\n\
A DDD session records the state of a debugging session,\n\
such that you can exit debugging sessions and resume later.\n\
\n\
If LBL(Include Core Dump) is set, DDD includes a core dump of the\n\
program being debugged.  This allows DDD to restore memory\n\
contents and data displays when restoring the session.\n\
\n\
DDD provides up to three ways to get a core dump:\n\
DESC([Killing the Debuggee], [kill the debugged program.])\n\
    Works fine, but loses the current process.\n\
DESC([The `gcore' Command], [an external program to get a core\n\
    file from a running process.])\n\
    The process may continue for a few steps after the `gcore'\n\
    command is done.\n\
DESC([The ptrace() call], [a system call to get a core file])\n\
    Safe, but may result in a hanging GDB.\n\
\n\
Click on LBL(Save) to save the current DDD session.\n\
Click on LBL(Delete) to delete the selected session.


Ddd*export_data.dialogTitle:       DDD: Save Plot Data
Ddd*export_data.okLabelString:	   Save
Ddd*export_data.selectionLabelString: Data File
Ddd*export_data*helpString:	   \
@rm You can save the plot data in a file.\n\
Enter the file name in the argument field.\n\
\n\
Click on LBL(Save) to save the plot data.\n\
Click on LBL(Filter) to apply the given filter.


!-----------------------------------------------------------------------------
! Breakpoint Editor
!-----------------------------------------------------------------------------

Ddd*edit_breakpoints_dialog.dialogTitle: DDD: Breakpoints and Watchpoints
Ddd*edit_breakpoints_dialog.listLabelString: Breakpoints and Watchpoints
Ddd*edit_breakpoints_dialog*buttons.orientation: XmHORIZONTAL
Ddd*edit_breakpoints_dialog.okLabelString:	 Close

Ddd*edit_breakpoints_dialog*buttons*labelType:  XmPIXMAP
Ddd*edit_breakpoints_dialog.childPlacement:  XmPLACE_TOP

Ddd*edit_breakpoints_dialog*helpString:	     \
WIDGET(Breakpoint and Watchpoint Editor)\n\
\n\
DESC(Props..., [set or modify an item's properties])\n\
DESC(Lookup, [lookup selected item])\n\
DESC(Print, [print value of selected breakpoint])\n\
DESC(Break..., [create a new breakpoint])\n\
DESC(Watch..., [create a new watchpoint])\n\
DESC(Enable, [enable all selected items])\n\
DESC(Disable, [disable all selected items])\n\
DESC(Delete, [delete all selected items])\n\
\n\
Use KEY(Ctrl)+BUTTON(1) to toggle selections.

Ddd*edit_breakpoints_dialog*new_bp.labelString:	           New Breakpoint...
Ddd*edit_breakpoints_dialog*new_bp.labelPixmap:	           new_break
Ddd*edit_breakpoints_dialog*new_bp.labelInsensitivePixmap: new_break-xx
Ddd*edit_breakpoints_dialog*new_bp.armPixmap:              new_break-arm
Ddd*edit_breakpoints_dialog*new_bp.highlightPixmap:        new_break-hi

Ddd*edit_breakpoints_dialog*new_bp.tipString:     \
@rm Set new breakpoint
Ddd*edit_breakpoints_dialog*new_bp.documentationString: \
@rm Set breakpoint at specified line or function

Ddd*edit_breakpoints_dialog*new_wp.labelString:	      New Watchpoint...
Ddd*edit_breakpoints_dialog*new_wp.labelPixmap:	           new_watch
Ddd*edit_breakpoints_dialog*new_wp.labelInsensitivePixmap: new_watch-xx
Ddd*edit_breakpoints_dialog*new_wp.armPixmap:              new_watch-arm
Ddd*edit_breakpoints_dialog*new_wp.highlightPixmap:        new_watch-hi

Ddd*edit_breakpoints_dialog*new_wp.tipString:     \
@rm Set new watchpoint
Ddd*edit_breakpoints_dialog*new_wp.documentationString: \
@rm Set watchpoint on specified variable

Ddd*edit_breakpoints_dialog*lookup.labelString:	       Lookup
Ddd*edit_breakpoints_dialog*lookup.labelPixmap:	           lookup
Ddd*edit_breakpoints_dialog*lookup.labelInsensitivePixmap: lookup-xx
Ddd*edit_breakpoints_dialog*lookup.armPixmap:              lookup-arm
Ddd*edit_breakpoints_dialog*lookup.highlightPixmap:        lookup-hi

Ddd*edit_breakpoints_dialog*lookup.tipString:     \
@rm Lookup selected item
Ddd*edit_breakpoints_dialog*lookup.documentationString: \
@rm Lookup selected item in the source

Ddd*edit_breakpoints_dialog*print.labelString:	       Print
Ddd*edit_breakpoints_dialog*print.labelPixmap:	          print
Ddd*edit_breakpoints_dialog*print.labelInsensitivePixmap: print-xx
Ddd*edit_breakpoints_dialog*print.armPixmap:              print-arm
Ddd*edit_breakpoints_dialog*print.highlightPixmap:        print-hi

Ddd*edit_breakpoints_dialog*print.tipString:     \
@rm Print watched expression
Ddd*edit_breakpoints_dialog*print.documentationString: \
@rm Print value of watched expression in the @GDB@ console

Ddd*edit_breakpoints_dialog*enable.labelString:	       Enable
Ddd*edit_breakpoints_dialog*enable.labelPixmap:	           enable
Ddd*edit_breakpoints_dialog*enable.labelInsensitivePixmap: enable-xx
Ddd*edit_breakpoints_dialog*enable.armPixmap:              enable-arm
Ddd*edit_breakpoints_dialog*enable.highlightPixmap:        enable-hi

Ddd*edit_breakpoints_dialog*enable.tipString:     \
@rm Enable selected items
Ddd*edit_breakpoints_dialog*enable.documentationString: \
@rm Enable the selected items

Ddd*edit_breakpoints_dialog*disable.labelString:       Disable
Ddd*edit_breakpoints_dialog*disable.labelPixmap:	    disable
Ddd*edit_breakpoints_dialog*disable.labelInsensitivePixmap: disable-xx
Ddd*edit_breakpoints_dialog*disable.armPixmap:              disable-arm
Ddd*edit_breakpoints_dialog*disable.highlightPixmap:        disable-hi

Ddd*edit_breakpoints_dialog*disable.tipString:     \
@rm Disable selected items
Ddd*edit_breakpoints_dialog*disable.documentationString: \
@rm Disable the selected items

Ddd*edit_breakpoints_dialog*properties.labelString:     Properties...
Ddd*edit_breakpoints_dialog*properties.labelPixmap:	       properties
Ddd*edit_breakpoints_dialog*properties.labelInsensitivePixmap: properties-xx
Ddd*edit_breakpoints_dialog*properties.armPixmap:              properties-arm
Ddd*edit_breakpoints_dialog*properties.highlightPixmap:        properties-hi

Ddd*edit_breakpoints_dialog*properties.tipString:     \
@rm Edit properties
Ddd*edit_breakpoints_dialog*properties.documentationString:     \
@rm Specify the properties of the selected items

Ddd*edit_breakpoints_dialog*delete.labelString:	       Delete
Ddd*edit_breakpoints_dialog*delete.labelPixmap:	           delete
Ddd*edit_breakpoints_dialog*delete.labelInsensitivePixmap: delete-xx
Ddd*edit_breakpoints_dialog*delete.armPixmap:              delete-arm
Ddd*edit_breakpoints_dialog*delete.highlightPixmap:        delete-hi

Ddd*edit_breakpoints_dialog*delete.tipString:     \
@rm Delete selected items
Ddd*edit_breakpoints_dialog*delete.documentationString:     \
@rm Delete the selected items



!-----------------------------------------------------------------------------
! New Breakpoint, New Watchpoint dialogs
!-----------------------------------------------------------------------------

Ddd*new_breakpoint_dialog.dialogTitle: DDD: New Breakpoint
Ddd*new_breakpoint_dialog*label.labelString: Set Breakpoint at
Ddd*new_breakpoint_dialog.okLabelString: Set
Ddd*new_breakpoint_dialog.textAccelerators:

Ddd*new_breakpoint_dialog*helpString:	\
\
@rm You can now enter a breakpoint you want to set.\n\
Enter its name in the argument field.\n\
\n\
The argument may be a EMPH(line number), a EMPH(function name),\
 or KBD(*) and an address.\n\
If a EMPH(line number) is specified, break at start of code for that line.\n\
If a EMPH(function name) is specified, break at start of code\
 for that function.\n\
If an EMPH(address) is specified, break at that exact address.\n\
Without argument, use current execution address of selected stack frame.\n\
This is useful for breaking on return to a stack frame.\n\
\n\
Multiple breakpoints at one place are permitted, and useful if conditional.\n\
\n\
Click on LBL(Set) to set the specified breakpoint.


Ddd*new_watchpoint_dialog.dialogTitle: DDD: New Watchpoint
Ddd*new_watchpoint_dialog*set.labelString:	Set
Ddd*new_watchpoint_dialog*cwatch.labelString:	Watchpoint
Ddd*new_watchpoint_dialog*rwatch.labelString:	Read Watchpoint
Ddd*new_watchpoint_dialog*awatch.labelString:	Access Watchpoint
Ddd*new_watchpoint_dialog*on.labelString:	on
Ddd*new_watchpoint_dialog.okLabelString: Set
Ddd*new_watchpoint_dialog.textAccelerators:

Ddd*new_watchpoint_dialog*helpString:	\
@rm You can now enter a variable whose value you want to watch.\n\
Enter its name in the argument field.\n\
\n\
DESC(Set Watchpoint, [stop whenever the variable value changes])\n\
DESC(Set Read Watchpoint, [stop whenever the variable is read])\n\
DESC(Set Access Watchpoint, \
[stop whenever the variable is either read or written])\n\
\n\
Click on LBL(Set) to set the specified watchpoint.



!-----------------------------------------------------------------------------
! Breakpoint Properties
!-----------------------------------------------------------------------------

Ddd*breakpoint_properties.dialogTitle:    DDD: Properties

Ddd*breakpoint_properties.cancelLabelString:    	Close

Ddd*breakpoint_properties*title.labelString:		Breakpoint
Ddd*breakpoint_properties*titleMenu.marginWidth:	0
Ddd*breakpoint_properties*titleMenu.marginWidth:	0
Ddd*breakpoint_properties*titlePanel.marginWidth:	0
Ddd*breakpoint_properties*titlePanel.marginHeight:	0

Ddd*breakpoint_properties*enabled.labelString:		Enabled
Ddd*breakpoint_properties*temporary.labelString:	Temporary

Ddd*breakpoint_properties*lookup.labelType:		XmPIXMAP
Ddd*breakpoint_properties*lookup.labelString:		Lookup
Ddd*breakpoint_properties*lookup.labelPixmap:	         lookup
Ddd*breakpoint_properties*lookup.labelInsensitivePixmap: lookup-xx
Ddd*breakpoint_properties*lookup.armPixmap:              lookup-arm
Ddd*breakpoint_properties*lookup.highlightPixmap:        lookup-hi
Ddd*breakpoint_properties*lookup.tipString:             \
@rm Lookup breakpoint

Ddd*breakpoint_properties*print.labelType:		XmPIXMAP
Ddd*breakpoint_properties*print.labelString:		Print
Ddd*breakpoint_properties*print.labelPixmap:	        print
Ddd*breakpoint_properties*print.labelInsensitivePixmap: print-xx
Ddd*breakpoint_properties*print.armPixmap:              print-arm
Ddd*breakpoint_properties*print.highlightPixmap: 	print-hi
Ddd*breakpoint_properties*print.tipString:             \
@rm Print watchpoint value

Ddd*breakpoint_properties*enable.labelType:		XmPIXMAP
Ddd*breakpoint_properties*enable.labelString:		Enable
Ddd*breakpoint_properties*enable.labelPixmap:	         enable
Ddd*breakpoint_properties*enable.labelInsensitivePixmap: enable-xx
Ddd*breakpoint_properties*enable.armPixmap:              enable-arm
Ddd*breakpoint_properties*enable.highlightPixmap:	 enable-hi
Ddd*breakpoint_properties*enable.tipString:             \
@rm Enable breakpoint

Ddd*breakpoint_properties*disable.labelType:		XmPIXMAP
Ddd*breakpoint_properties*disable.labelString:		Disable
Ddd*breakpoint_properties*disable.labelPixmap:	          disable
Ddd*breakpoint_properties*disable.labelInsensitivePixmap: disable-xx
Ddd*breakpoint_properties*disable.armPixmap:              disable-arm
Ddd*breakpoint_properties*disable.highlightPixmap:        disable-hi
Ddd*breakpoint_properties*disable.tipString:             \
@rm Disable breakpoint

Ddd*breakpoint_properties*temporary.labelType:		XmPIXMAP
Ddd*breakpoint_properties*temporary.labelString:	Temp 
Ddd*breakpoint_properties*temporary.labelPixmap:	    maketemp
Ddd*breakpoint_properties*temporary.labelInsensitivePixmap: maketemp-xx
Ddd*breakpoint_properties*temporary.armPixmap:              maketemp-arm
Ddd*breakpoint_properties*temporary.highlightPixmap:        maketemp-hi
Ddd*breakpoint_properties*temporary.tipString:           \
@rm Make breakpoint temporary
Ddd*breakpoint_properties*temporary.documentationString:           \
@rm Make breakpoint temporary, such that it will be deleted when hit

Ddd*breakpoint_properties*delete.labelType:		XmPIXMAP
Ddd*breakpoint_properties*delete.labelString:		Delete
Ddd*breakpoint_properties*delete.labelPixmap:	         delete
Ddd*breakpoint_properties*delete.labelInsensitivePixmap: delete-xx
Ddd*breakpoint_properties*delete.armPixmap:              delete-arm
Ddd*breakpoint_properties*delete.highlightPixmap:        delete-hi
Ddd*breakpoint_properties*delete.tipString:           \
@rm Delete breakpoint

Ddd*breakpoint_properties*condition.label.labelString:	Condition
Ddd*breakpoint_properties*condition*text*columns:       30

Ddd*breakpoint_properties*ignore.label.labelString:	Ignore Count
Ddd*breakpoint_properties*ignore*text.columns:          4
Ddd*breakpoint_properties*ignore*text.maximumValue:	9999

Ddd*breakpoint_properties*spinBoxChildType:		XmNUMERIC
Ddd*breakpoint_properties*arrowLayout:		        XmARROWS_END
Ddd*breakpoint_properties*spin.marginWidth:		0
Ddd*breakpoint_properties*spin.marginHeight:		0

Ddd*breakpoint_properties*commands.labelString:		Commands

Ddd*breakpoint_properties*commandsMenu.packing:		XmPACK_COLUMN
Ddd*breakpoint_properties*commandsMenu.entryAlignment:	XmALIGNMENT_CENTER
Ddd*breakpoint_properties*record.labelString:		\ \ \ Record\ \ \ 
Ddd*breakpoint_properties*end.labelString:		End
Ddd*breakpoint_properties*edit.labelString:		Edit @small>>

Ddd*breakpoint_properties*record.tipString: \
@rm Record breakpoint commands
Ddd*breakpoint_properties*record.documentationString: \
@rm Record commands to be executed when breakpoint is hit

Ddd*breakpoint_properties*end.tipString: \
@rm End recording
Ddd*breakpoint_properties*end.documentationString: \
@rm End command recording

Ddd*breakpoint_properties*edit.tipString: \
@rm Edit breakpoint commands


Ddd*breakpoint_properties*text.columns:		        40
Ddd*breakpoint_properties*scrollBarDisplayPolicy:  	XmAS_NEEDED

Ddd*breakpoint_properties*helpString:		\
@rm WIDGET(Breakpoint Properties)\n\
\n\
DESC(Lookup, [lookup the breakpoint.])\n\
DESC(Print, [print the value of the watched expression.])\n\
DESC(Enable, [enable the breakpoint.])\n\
DESC(Disable, [disable the breakpoint.])\n\
DESC(Temp, [make the breakpoint temporary.])\n\
DESC(Delete, [delete the breakpoint.])\n\
\n\
DESC(Condition, [specify a breakpoint condition])\n\
    The breakpoint breaks only if the condition evaluates to non-zero.\n\
DESC(Ignore Count, [set an ignore count VAR(count)])\n\
    The next VAR(count) hits of the breakpoint will be ignored.\n\
DESC(Commands, [record and edit @GDB@ command sequences])\n\
    These commands will be executed when the breakpoint is hit.\n\
    To record a command sequence, follow these steps:\n\
    SUBITEM Click on LBL(Record) to begin the recording.\n\
    SUBITEM Now interact with DDD.\n\
       The recorded @GDB@ commands are shown in the @GDB@ console.\n\
    SUBITEM Click on LBL(End) to stop the recording.\n\
    SUBITEM Use LBL(Edit @small>>) to edit the recorded commands.\n\
\n\
Click on LBL(Apply) to apply the changes you made.\n\
Click on LBL(Close) to close this window.



!-----------------------------------------------------------------------------
! Define Command
!-----------------------------------------------------------------------------

Ddd*define_command.dialogTitle:    		DDD: Define Command

Ddd*define_command.applyLabelString:    	Execute
Ddd*define_command.cancelLabelString:    	Close

Ddd*define_command*name.labelString:		Command
Ddd*define_command*name*text*columns:       	25

Ddd*define_command*arg.labelString:		()
Ddd*define_command*arg*indicatorOn:		off
Ddd*define_command*arg*shadowThickness:		2

Ddd*define_command*arg.tipString: \
@rm Toggle symbolic argument
Ddd*define_command*arg.documentationString: \
@rm When enabled, use LBL(()) as symbolic argument

Ddd*define_command*commands.labelString:	Definition
Ddd*define_command*commandsMenu.packing:	XmPACK_COLUMN
Ddd*define_command*commandsMenu.entryAlignment:	XmALIGNMENT_CENTER

Ddd*define_command*record.labelString:		\ \ \ Record\ \ \ 
Ddd*define_command*end.labelString:		End
Ddd*define_command*edit.labelString:		Edit @small>>

Ddd*define_command*record.tipString: \
@rm Record command definition
Ddd*define_command*record.documentationString: \
@rm Record command definition

Ddd*define_command*end.tipString: \
@rm End recording
Ddd*define_command*end.documentationString: \
@rm End command recording

Ddd*define_command*edit.tipString: \
@rm Edit command definition

Ddd*define_command*text.columns: 		40
Ddd*define_command*scrollBarDisplayPolicy: 	XmAS_NEEDED

Ddd*define_command*button.labelString:		Button
Ddd*define_command*console.labelString:		Console
Ddd*define_command*source.labelString:		Source
Ddd*define_command*data.labelString:		Data

Ddd*define_command*helpString:		\
@rm WIDGET(Define Command)\n\
\n\
DESC(Command, [the name of the @GDB@ command being defined])\n\
DESC(Definition, [record and edit the command definition])\n\
DESC(Button, [assign a button to this command])\n\
\n\
To define a @GDB@ command, follow these steps:\n\
ITEM Enter the command name in the LBL(Command) field.\n\
ITEM Click on LBL(Record) to begin the recording.\n\
ITEM Now interact with DDD.\n\
   The recorded @GDB@ commands are shown in the @GDB@ console.\n\
ITEM Click on LBL(End) to stop the recording.\n\
ITEM Use LBL(Edit @small>>) to edit the command definition.\n\
ITEM Use LBL(Button) to place the new command on a button.\n\
\n\
To define a command with argument, enable the LBL(()) toggle.\n\
This will make the command use LBL(()) as symbolic argument.\n\
\n\
Click on LBL(Execute) to run the new command.\n\
Click on LBL(Close) to close this window.


!-----------------------------------------------------------------------------
! Show Backtrace
!-----------------------------------------------------------------------------

Ddd*stack_dialog.dialogTitle:		  DDD: Backtrace
Ddd*stack_dialog.listLabelString:	  Backtrace
Ddd*stack_dialog.okLabelString:		  Up
Ddd*stack_dialog.applyLabelString:	  Down
Ddd*stack_dialog.cancelLabelString:	  Close
Ddd*stack_dialog*visibleItemCount:	  10

Ddd*stack_dialog*helpString:	  \
WIDGET(Backtrace)\n\
\n\
The backtrace is a summary of how your program got where it is.\n\
It shows one line per stack frame, for many frames, starting with\n\
the initial calling function (usually CODE(main())), the function\n\
called by CODE(main()), the function called by this function and\n\
so on down the stack, down to the currently executing frame.\n\
\n\
You can make a specific frame the current one by selecting it\n\
in this window.  You can also move around by pressing \
LBL(Up) or LBL(Down).


!-----------------------------------------------------------------------------
! Registers
!-----------------------------------------------------------------------------

Ddd*register_dialog.dialogTitle:	     DDD: Registers
Ddd*register_dialog.listLabelString:	     Registers
Ddd*register_dialog.okLabelString:	     Close
Ddd*register_dialog.listVisibleItemCount:    12

Ddd*register_dialog*box.orientation: XmHORIZONTAL
Ddd*register_dialog*box.marginHeight: 0
Ddd*register_dialog*box.marginWidth:  0

Ddd*register_dialog*all_registers.labelString: All registers
Ddd*register_dialog*int_registers.labelString: Integer registers

Ddd*register_dialog*helpString:	     \
WIDGET(Machine Registers)\n\
\n\
Select any register to have its name copied to the argument LBL(()).\n\
\n\
If LBL(Integer registers) is set, integer registers are shown.\n\
If LBL(All registers) is set, all registers are shown.


!-----------------------------------------------------------------------------
! Threads
!-----------------------------------------------------------------------------

Ddd*thread_dialog.dialogTitle:	     	     DDD: Threads
Ddd*thread_dialog.listLabelString:	     Threads
Ddd*thread_dialog.cancelLabelString:	     Close
Ddd*thread_dialog.okLabelString:	     Suspend
Ddd*thread_dialog.applyLabelString:	     Resume
Ddd*thread_dialog.listVisibleItemCount:      10

Ddd*thread_dialog*box.orientation:  XmHORIZONTAL
Ddd*thread_dialog*box.marginHeight: 0
Ddd*thread_dialog*box.marginWidth:  0

Ddd*thread_dialog*helpString:	     \
WIDGET(Execution Threads)\n\
\n\
The current thread is highlighted.\n\
All debugging commands show information from the perspective\n\
of the current thread.\n\
\n\
Select any thread to make it the current thread.\n\
\n\
In JDB, you can also\n\
ITEM select a threadgroup to switch between viewing all threads\n\
    and the threads of the selected threadgroup.\n\
ITEM click on LBL(Suspend) to suspend execution of the selected threads.\n\
ITEM click on LBL(Resume) to resume execution of the selected threads.


!-----------------------------------------------------------------------------
! Command History
!-----------------------------------------------------------------------------

Ddd*history_dialog.dialogTitle:		    DDD: Command History
Ddd*history_dialog.listLabelString:	    Command History
Ddd*history_dialog*visibleItemCount:	    10
Ddd*history_dialog.cancelLabelString:	    Close

Ddd*history_dialog*helpString:	    \
WIDGET(Command History)\n\
\n\
DDD keeps track of the commands you type during your debugging\n\
sessions, so that you can be certain of precisely what happened.\n\
Use this window to manage the DDD command history facility.\n\
\n\
Select a specific command to have it copied at the @GDB@ prompt.\n\
Click on LBL(Apply) to execute the current command.


!-----------------------------------------------------------------------------
! Display Editor
!-----------------------------------------------------------------------------

Ddd*edit_displays_dialog.dialogTitle: DDD: Displays
Ddd*edit_displays_dialog*listLabelString: Displays
Ddd*edit_displays_dialog*buttons.orientation: XmHORIZONTAL
Ddd*edit_displays_dialog.okLabelString:	      Close

Ddd*edit_displays_dialog.childPlacement:  XmPLACE_TOP
Ddd*edit_displays_dialog*buttons*labelType:  XmPIXMAP


Ddd*edit_displays_dialog*helpString:	  \
WIDGET(Display Editor)\n\
\n\
Columns:\n\
DESC(Num, [the display number])\n\
DESC(Expression, [the display expression])\n\
DESC(State, [the display state (enabled, disabled, alias, or deferred)])\n\
DESC(Scope, [the scope in which the display was created])\n\
DESC(Address, [the location of the display expression in memory])\n\
\n\
Buttons:\n\
DESC(Display..., [create a new display])\n\
DESC(Disp *, [dereference the selected display])\n\
DESC(Show, [show the details of all selected displays])\n\
DESC(Hide, [hide the details of all selected displays])\n\
DESC(Set, [change a value in the selected display])\n\
DESC(Undisp, [delete all selected displays])\n\
\n\
Use KEY(Ctrl)+BUTTON(1) to toggle selections.



Ddd*edit_displays_dialog*new.labelString:	    New...
Ddd*edit_displays_dialog*new.labelPixmap:		new_display
Ddd*edit_displays_dialog*new.labelInsensitivePixmap:	new_display-xx
Ddd*edit_displays_dialog*new.armPixmap:			new_display-arm
Ddd*edit_displays_dialog*new.highlightPixmap:		new_display-hi

Ddd*edit_displays_dialog*new.tipString:  \
@rm Create new display
Ddd*edit_displays_dialog*new.documentationString:  \
@rm Create new data display (possibly depending on the selected data display)

Ddd*edit_displays_dialog*dereference.labelString:   Display *
Ddd*edit_displays_dialog*dereference.labelPixmap:		dispref
Ddd*edit_displays_dialog*dereference.labelInsensitivePixmap:	dispref-xx
Ddd*edit_displays_dialog*dereference.armPixmap:			dispref-arm
Ddd*edit_displays_dialog*dereference.highlightPixmap:		dispref-hi

Ddd*edit_displays_dialog*dereference.tipString:  \
@rm Dereference selected display
Ddd*edit_displays_dialog*dereference.documentationString: \
@rm Dereference the selected data display

Ddd*edit_displays_dialog*show_detail.labelString:   Show Detail
Ddd*edit_displays_dialog*show_detail.labelPixmap:		show
Ddd*edit_displays_dialog*show_detail.labelInsensitivePixmap:	show-xx
Ddd*edit_displays_dialog*show_detail.armPixmap:			show-arm
Ddd*edit_displays_dialog*show_detail.highlightPixmap:		show-hi

Ddd*edit_displays_dialog*show_detail.tipString:  \
@rm Show selected details
Ddd*edit_displays_dialog*show_detail.documentationString: \
@rm Show detail on the selected data displays

Ddd*edit_displays_dialog*hide_detail.labelString:   Hide Detail
Ddd*edit_displays_dialog*hide_detail.labelPixmap:		hide
Ddd*edit_displays_dialog*hide_detail.labelInsensitivePixmap:	hide-xx
Ddd*edit_displays_dialog*hide_detail.armPixmap:			hide-arm
Ddd*edit_displays_dialog*hide_detail.highlightPixmap:		hide-hi

Ddd*edit_displays_dialog*hide_detail.tipString:  \
@rm Hide selected details
Ddd*edit_displays_dialog*hide_detail.documentationString: \
@rm Hide detail on the selected data displays

Ddd*edit_displays_dialog*set.labelString:	    Set Value
Ddd*edit_displays_dialog*set.labelPixmap:		set
Ddd*edit_displays_dialog*set.labelInsensitivePixmap:	set-xx
Ddd*edit_displays_dialog*set.armPixmap:			set-arm
Ddd*edit_displays_dialog*set.highlightPixmap:		set-hi

Ddd*edit_displays_dialog*set.tipString:  \
@rm Set display value
Ddd*edit_displays_dialog*set.documentationString: \
@rm Change a value in the selected data display

Ddd*edit_displays_dialog*cluster.labelString:	    Cluster
Ddd*edit_displays_dialog*cluster.labelPixmap:			cluster
Ddd*edit_displays_dialog*cluster.labelInsensitivePixmap:	cluster-xx
Ddd*edit_displays_dialog*cluster.armPixmap:			cluster-arm
Ddd*edit_displays_dialog*cluster.highlightPixmap:		cluster-hi

Ddd*edit_displays_dialog*cluster.tipString:  \
@rm Cluster displays
Ddd*edit_displays_dialog*cluster.documentationString: \
@rm Cluster the selected data displays into one

Ddd*edit_displays_dialog*uncluster.labelString:	    Uncluster
Ddd*edit_displays_dialog*uncluster.labelPixmap:			uncluster
Ddd*edit_displays_dialog*uncluster.labelInsensitivePixmap:	uncluster-xx
Ddd*edit_displays_dialog*uncluster.armPixmap:			uncluster-arm
Ddd*edit_displays_dialog*uncluster.highlightPixmap:		uncluster-hi

Ddd*edit_displays_dialog*uncluster.tipString:  \
@rm Uncluster displays
Ddd*edit_displays_dialog*uncluster.documentationString: \
@rm Move the selected displays out of its cluster

Ddd*edit_displays_dialog*delete.labelString:	    Undisplay
Ddd*edit_displays_dialog*delete.labelPixmap:		undisplay
Ddd*edit_displays_dialog*delete.labelInsensitivePixmap:	undisplay-xx
Ddd*edit_displays_dialog*delete.armPixmap:		undisplay-arm
Ddd*edit_displays_dialog*delete.highlightPixmap:	undisplay-hi

Ddd*edit_displays_dialog*delete.tipString:  \
@rm Delete selected displays
Ddd*edit_displays_dialog*delete.documentationString:  \
@rm Delete the selected data displays


!-----------------------------------------------------------------------------
! New Display
!-----------------------------------------------------------------------------

define(NEW_DISPLAY_HELP,
[@rm You can now enter an expression in the argument field.\n\
The value of the expression is displayed each time the program stops.\n\
\n\
Variables accessible are those of the lexical environment of the current\n\
stack frame, plus all those whose scope is global or an entire file.\n\
\n\
If LBL(Include in `Display ()' Menu) is set, the new display expression\n\
becomes an item in the LBL(Display ()) menu.])dnl

Ddd*new_display_dialog.dialogTitle:          	   DDD: New Display
Ddd*new_display_dialog.okLabelString:              Display
Ddd*new_display_dialog*label.labelString:    	   Display Expression
Ddd*new_display_dialog*shortcut.labelString:   	   Include in 
Ddd*new_display_dialog*display.labelType:   	   XmPIXMAP
Ddd*new_display_dialog*display.labelString:   	   `Display ()'
Ddd*new_display_dialog*display.labelPixmap:   	   display
Ddd*new_display_dialog*menu.labelString:   	   menu
Ddd*new_display_dialog*helpString:	           NEW_DISPLAY_HELP

Ddd*dependent_display_dialog.dialogTitle:          DDD: New Dependent Display
Ddd*dependent_display_dialog.okLabelString:        Display
Ddd*dependent_display_dialog*label.labelString:    Display Expression
Ddd*dependent_display_dialog*shortcut.labelString: Include in 
Ddd*dependent_display_dialog*display.labelType:    XmPIXMAP
Ddd*dependent_display_dialog*display.labelString:  `Display ()'
Ddd*dependent_display_dialog*display.labelPixmap:  display
Ddd*dependent_display_dialog*menu.labelString:     menu
Ddd*dependent_display_dialog*helpString: 	   NEW_DISPLAY_HELP\n\
\n\
The new display will be made dependent on the currently selected display.


!-----------------------------------------------------------------------------
! Examine Memory
!-----------------------------------------------------------------------------
 
Ddd*examine_dialog.dialogTitle:	DDD: Examine Memory

Ddd*examine_dialog*examine.label.labelString:	Examine
Ddd*examine_dialog*examine*text.columns:	4
Ddd*examine_dialog*examine*text.maximumValue:	9999
Ddd*examine_dialog*examine*text.minimumValue:	1

Ddd*examine_dialog*spinBoxChildType:		XmNUMERIC
Ddd*examine_dialog*arrowLayout:		        XmARROWS_END
Ddd*examine_dialog*spin.marginWidth:		0
Ddd*examine_dialog*spin.marginHeight:		0

Ddd*examine_dialog*o.labelString:	octal
Ddd*examine_dialog*x.labelString:	hex
Ddd*examine_dialog*d.labelString:	decimal
Ddd*examine_dialog*u.labelString:	unsigned
Ddd*examine_dialog*t.labelString:	binary
Ddd*examine_dialog*f.labelString:	float
Ddd*examine_dialog*a.labelString:	address
Ddd*examine_dialog*i.labelString:	instruction
Ddd*examine_dialog*c.labelString:	char
Ddd*examine_dialog*C.labelString:	wide char
Ddd*examine_dialog*s.labelString:	string
Ddd*examine_dialog*W.labelString:	wide string

Ddd*examine_dialog*b.labelString:	bytes
Ddd*examine_dialog*h.labelString:	halfwords (2)
Ddd*examine_dialog*w.labelString:	words (4)
Ddd*examine_dialog*g.labelString:	giants (8)
Ddd*examine_dialog*G.labelString:	longs (16)

Ddd*examine_dialog*address.label.labelString: from

Ddd*examine_dialog.okLabelString:	Print
Ddd*examine_dialog.applyLabelString:	Display
Ddd*examine_dialog.cancelLabelString:	Close

Ddd*examine_dialog*helpString:	\
@rm You can examine memory in any of several formats,\n\
independently of your program's data types.\n\
\n\
You can select the number of items to examine, the format,\n\
and the starting address.\n\
\n\
Click on LBL(Print) to print the items in the debugger console.\n\
Click on LBL(Display) to display the items in the data window.\n\
Click on LBL(Close) to close this window.




!-----------------------------------------------------------------------------
! Run, make, and CD dialogs
!-----------------------------------------------------------------------------

Ddd*run_dialog.dialogTitle: DDD: Run Program
Ddd*run_dialog.listLabelString:		Arguments
Ddd*run_dialog.selectionLabelString:	Run with Arguments
Ddd*run_dialog.okLabelString:		Run

Ddd*run_dialog*selectionPolicy:		XmSINGLE_SELECT

Ddd*run_dialog*helpString:	\
@rm You can now run the debugged program.\n\
\n\
DDD keeps track of the arguments you gave to the debugged program,\n\
such that you can re-use them at a later time.\n\
Select specific arguments to have them copied to the argument prompt.\n\
\n\
Click on LBL(Run) to start the debugged program with the selected arguments.


Ddd*make_dialog.dialogTitle: DDD: Make
Ddd*make_dialog.listLabelString:	Targets
Ddd*make_dialog.selectionLabelString:	Make Target
Ddd*make_dialog.okLabelString:		Make

Ddd*make_dialog*selectionPolicy:	XmSINGLE_SELECT

Ddd*make_dialog*helpString:	\
@rm You can now start the make program.\n\
\n\
DDD keeps track of the arguments you gave to the make program,\n\
such that you can re-use them at a later time.\n\
Select specific arguments to have them copied to the argument prompt.\n\
\n\
Click on LBL(Make) to start the make program with the selected arguments.


Ddd*cd_dialog.dialogTitle: DDD: Change Directory
Ddd*cd_dialog.listLabelString:	Directories
Ddd*cd_dialog.selectionLabelString:	Change Directory To
Ddd*cd_dialog.okLabelString:		Change Dir

Ddd*cd_dialog*selectionPolicy:	XmSINGLE_SELECT

Ddd*cd_dialog*helpString:	\
@rm You can now change the current directory.\n\
\n\
DDD keeps track of the directories you gave to the cd command,\n\
such that you can re-use them at a later time.\n\
Select specific directories to have them copied to the argument prompt.\n\
\n\
Click on LBL(Change Dir) to change the current directory.


!-----------------------------------------------------------------------------
! Tic Tac Toe
!-----------------------------------------------------------------------------

Ddd*tictactoe.dialogTitle: Tic Tac Toe
Ddd*tictactoe.okLabelString: New Game
Ddd*tictactoe*field.foreground: grey60
Ddd*tictactoe*helpString: \
WIDGET(Tic Tac Toe)\n\
\n\
Try to get three stop signs in a row.


!-----------------------------------------------------------------------------
! Other Dialogs
!-----------------------------------------------------------------------------

Ddd*busy_dialog.dialogTitle: DDD: Debugger Busy
Ddd*busy_dialog.messageString:		\
@rm @GDB@ is busy!
Ddd*busy_dialog*helpString:	\
@rm @GDB@ is busy on some debugger action.\n\
\n\
You may wish to wait until the @GDB@ prompt appears,\n\
or interrupt the current debugging command by selecting\n\
LBL(Program, Interrupt) (or typing KEY(Esc)).\n\
\n\
If @GDB@ is busy running your debugged program, interrupting\n\
@GDB@ means to interrupt program execution, such that you can\n\
examine the current program state.\n\
\n\
If @GDB@ is busy recording commands, simply enter KBD(end) 
at the @GDB@ prompt.

Ddd*quit_dialog.dialogTitle: DDD: Debugger Still Busy
! Ddd*quit_dialog.messageString:	 \
! @rm @GDB@ is still busy.  Exit anyway (and kill it)?
Ddd*quit_dialog*okLabelString:	     Yes
Ddd*quit_dialog*cancelLabelString:   No
Ddd*quit_dialog*defaultButtonType:   XmDIALOG_OK_BUTTON
Ddd*quit_dialog*helpString:	\
@rm @GDB@ is busy and does not react to the KBD(quit) command.\n\
The debugged program may be still running.\n\
\n\
Before killing a running @GDB@, you should try to interrupt it\n\
(by selecting LBL(Program, Interrupt)).

Ddd*shutdown_dialog.dialogTitle: DDD: Program Running
! Ddd*shutdown_dialog.messageString:	 \
! @rm The program is running.  Shutdown anyway (and kill it)?
Ddd*shutdown_dialog*okLabelString:	     Yes
Ddd*shutdown_dialog*cancelLabelString:   No
Ddd*shutdown_dialog*defaultButtonType:   XmDIALOG_OK_BUTTON
Ddd*shutdown_dialog*helpString:	\
@rm Your program is still running.\n\
\n\
Shutting down DDD now will kill your program.\n\
Please confirm that this is what you want.

Ddd*terminated_dialog.dialogTitle: DDD: Debugger Terminated
! Ddd*terminated_dialog.messageString:   \
! @rm @GDB@ terminated abnormally.
Ddd*terminated_dialog*okLabelString:	   Restart @GDB@
Ddd*terminated_dialog*cancelLabelString:   Exit
Ddd*terminated_dialog*helpString:	\
@rm @GDB@ terminated abnormally.  Without @GDB@, DDD cannot run.\n\
\n\
Click on LBL(Restart @GDB@) to restart @GDB@.\n\
Click on LBL(Exit) to exit DDD.

Ddd*exception_dialog.dialogTitle: DDD: Internal Debugger Exception
! Ddd*exception_dialog.messageString:   \
! @rm Internal @GDB@ error.
Ddd*exception_dialog*okLabelString:	  Restart @GDB@
Ddd*exception_dialog*cancelLabelString:   Exit
Ddd*exception_dialog*helpString:	\
@rm @GDB@ caught an internal exception and may be in an undefined state.\n\
\n\
DDD users have reported that this frequently happens when @GDB@ attempts\n\
to access undefined variables.  You can avoid this by disabling the\n\
automatic display of variable values\n\
(via LBL(Edit, Preferences, General, Automatic Display of Variable Values)).\n\
\n\
Click on LBL(Restart @GDB@) to restart @GDB@.\n\
Click on LBL(Exit) to exit DDD.

Ddd*exited_dialog.dialogTitle: DDD: Debugger Exited
! Ddd*exited_dialog.messageString:   \
! @rm @GDB@ exited.
Ddd*exited_dialog*okLabelString:	Restart @GDB@
Ddd*exited_dialog*cancelLabelString:	Exit
Ddd*exited_dialog*helpString:	\
@rm @GDB@ has exited.  Without @GDB@, DDD cannot run.\n\
\n\
Click on LBL(Restart @GDB@) to restart @GDB@.\n\
Click on LBL(Exit) to exit DDD.

Ddd*done_dialog.dialogTitle: DDD: Program Exited
! Ddd*done_dialog.messageString:   \
! @rm Your program exited.
Ddd*done_dialog*okLabelString:		Restart @GDB@
Ddd*done_dialog*cancelLabelString:	Exit
Ddd*done_dialog*helpString:	\
@rm Your program has exited, and so did @GDB@.\n\
\n\
Click on LBL(Restart @GDB@) to restart @GDB@.\n\
Click on LBL(Exit) to exit DDD.

Ddd*no_debugger_dialog.dialogTitle: DDD: No Debugger
! Ddd*no_debugger_dialog.messageString:   \
! @rm @GDB@ could not be started.
Ddd*no_debugger_dialog*okLabelString:	   Exit
Ddd*no_debugger_dialog*helpString:	\
@rm @GDB@ could not be started.\n\
\n\
DDD needs some inferior debugger to run.\n\
You may wish to examine the diagnostic messages before exiting DDD.\n\
\n\
Here are some hints that may help you out:\n\
ITEM If DDD looked for the wrong inferior debugger, re-invoke DDD,\n\
    but use SAMP(--auto) to have DDD infer the correct inferior debugger.\n\
ITEM If @GDB@ is the correct inferior debugger, but could not be found,\n\
    re-invoke DDD, but use the SAMP(--debugger VAR(path)) option to specify\n\
    the full @GDB@ path VAR(path).\n\
ITEM If you started DDD without arguments, re-invoke DDD,\n\
    but give it an executable as argument.  XDB and some DBX versions\n\
    require being invoked with an executable.\n\
ITEM DDD passes all unknown options to @GDB@.  If @GDB@ choked on\n\
    some options intended for DDD, use the DDD SAMP(--help) option to\n\
    get a list of valid DDD options.\n\
ITEM If @GDB@ could not be invoked due to some communication failure,\n\
    try changing the SAMP(.blockTTYInput) resource.\n\
For details on options and resources, see the DDD manual: select\n\
LBL(Help, [[[[DDD]]]] Reference) \
or invoke DDD with the SAMP(--manual) option.\n\
\n\
Click on LBL(Exit) to exit DDD.

Ddd*no_plotter_dialog.dialogTitle: DDD: No Plot
! Ddd*no_plotter_dialog.messageString:   \
! @rm Gnuplot could not be started.
Ddd*no_plotter_dialog*helpString:	\
@rm The Gnuplot program could not be started.\n\
\n\
DDD requires the Gnuplot program to create plots.\n\
Please install Gnuplot (available from any GNU site)\n\
or enter its path in LBL(Edit, Preferences, Helpers, Plot).

Ddd*fatal_dialog.dialogTitle: DDD: Oops
Ddd*fatal_dialog.messageString:	  \
@rm Internal error
Ddd*fatal_dialog*okLabelString:	       Restart
Ddd*fatal_dialog*debug.labelString:    Debug
Ddd*fatal_dialog*exit.labelString:     Exit
Ddd*fatal_dialog*cancelLabelString:    Ignore
Ddd*fatal_dialog*helpString: \
@rm Oops!  You have found a bug in DDD.\n\
You should better restart DDD now; DDD may fail again soon.\n\
\n\
If you can reproduce this bug, please send a bug report\n\
to EMAIL(@PACKAGE_BUGREPORT@), giving a subject like\n\
\n\
    DDD @VERSION@ (@THEHOST@) gets @CAUSE@\n\
\n\
To enable us to fix the bug, you should include the following information:\n\
ITEM What you were doing to get this message.  Report all the facts.\n\
ITEM The FILE(~/.[ddd]/log) file as generated by this session.\n\
Please read also the section ``Reporting Bugs'' in the DDD manual.\n\
\n\
Thanks in advance for your support.\n\
\n\
Click on LBL(Restart) to restart DDD from scratch.\n\
Click on LBL(Exit) to exit DDD.\n\
Click on LBL(Ignore) to resume your current DDD session.


Ddd*yn_dialog.dialogTitle: DDD: Debugger Question
Ddd*yn_dialog*okLabelString:	   Yes
Ddd*yn_dialog*cancelLabelString:   No
Ddd*yn_dialog*defaultButtonType:   XmDIALOG_OK_BUTTON
Ddd*yn_dialog*helpString:	\
WIDGET(@GDB@ Question)\n\
\n\
@GDB@ requires confirmation for a potentially dangerous operation.\n\
The @GDB@ documentation has details on why this is dangerous.\n\
Click on LBL(Yes) or LBL(No), or enter KBD(yes) or KBD(no) \
in the @GDB@ console.\n\
\n\
To turn off these confirmations in GDB, unset\n\
LBL(Edit, GDB Settings, Confirm potentially dangerous operations).

Ddd*gdb_message_dialog.dialogTitle: DDD: Debugger Message
Ddd*gdb_message_dialog*helpString:	\
WIDGET(@GDB@ Message)\n\
\n\
For details on this @GDB@ message, consult the @GDB@ documentation.

Ddd*gdb_selection_dialog.dialogTitle: DDD: Selection
Ddd*gdb_selection_dialog*listLabelString:  Selection
Ddd*gdb_selection_dialog*helpString:	\
WIDGET(@GDB@ Selection Menu)\n\
\n\
Please select an item from the list and click on LBL(OK).\n\
For more details, consult the @GDB@ documentation.

Ddd*confirm_overwrite_dialog.dialogTitle: DDD: File Exists
Ddd*confirm_overwrite_dialog*messageString:	  \
@rm Overwrite existing file?
Ddd*confirm_overwrite_dialog*okLabelString:	  Yes
Ddd*confirm_overwrite_dialog*cancelLabelString:	  No
Ddd*confirm_overwrite_dialog*defaultButtonType: XmDIALOG_OK_BUTTON
Ddd*confirm_overwrite_dialog*helpString:	\
@rm The file already exists.\n\
Click on LBL(Yes) to overwrite the existing file.

Ddd*restart_dialog.dialogTitle: DDD: Restart
Ddd*restart_dialog.messageString:	 \
@rm DDD startup preferences were modified.\n\
Restart DDD to see their effect?
Ddd*restart_dialog*okLabelString:	Yes
Ddd*restart_dialog*cancelLabelString:   No
Ddd*restart_dialog*defaultButtonType:   XmDIALOG_OK_BUTTON
Ddd*restart_dialog*helpString:	\
@rm You have changed some DDD startup preferences.\n\
\n\
To make your changes take effect, you can\n\
ITEM EMPH(Save Options), affecting all future DDD sessions.\n\
ITEM EMPH(Restart [DDD]), affecting the restarted DDD session only.\n\
\n\
To see your changes take effect, click on LBL(Yes) to restart DDD.\n\
Otherwise, click on LBL(No) and be sure to save your changes\n\
using LBL(Edit, Save Options).

Ddd*set_dialog.dialogTitle: DDD: Set Value
Ddd*set_dialog.textAccelerators:
Ddd*set_dialog*helpString:	\
@rm You can now change the value of the variable in LBL(()).\n\
If you want to keep it unchanged, click on LBL(Cancel).

Ddd*kill_to_save_dialog.dialogTitle: DDD: Save Session
Ddd*kill_to_save_dialog.messageString: \
@rm Kill the program being debugged?
Ddd*kill_to_save_dialog.okLabelString:     Yes
Ddd*kill_to_save_dialog.cancelLabelString: No
Ddd*kill_to_save_dialog*helpString: \
@rm In order to restore the current memory contents and the\n\
current execution position, DDD requires a core dump.\n\
Unfortunately, DDD must kill the program for that.\n\
\n\
To kill your program and save memory contents, click on LBL(Yes).\n\
To cancel saving the session, click on LBL(No).

Ddd*confirm_restart_dialog.dialogTitle: DDD: Restart Session
Ddd*confirm_restart_dialog.messageString: \
@rm The program being debugged has been started already.\n\
Restart anyway (and kill it)?
Ddd*confirm_restart_dialog.okLabelString:     Yes
Ddd*confirm_restart_dialog.cancelLabelString: No
Ddd*confirm_restart_dialog*helpString: \
WIDGET(Restart Session)\n\
\n\
For details on this @GDB@ message, consult the @GDB@ documentation.

Ddd*confirm_restart_gdb_dialog.dialogTitle: DDD: Restart Debugger
Ddd*confirm_restart_gdb_dialog.messageString: \
@rm The program being debugged has been started already.\n\
Start it from the beginning (and restart @GDB@)?
Ddd*confirm_restart_gdb_dialog.okLabelString:     Yes
Ddd*confirm_restart_gdb_dialog.cancelLabelString: No
Ddd*confirm_restart_gdb_dialog*helpString: \
WIDGET(Restart Debugger)\n\
\n\
@GDB@ can only handle one program run per session.\n\
To rerun the program being debugged, DDD must restart @GDB@.\n\
Essential @GDB@ settings (such as breakpoints) will be restored,\n\
but other settings may be lost.\n\
\n\
To rerun your program (and restart @GDB@), click on LBL(Yes).\n\
To cancel restarting the program, click on LBL(No).

Ddd*data_not_saved_dialog.dialogTitle: DDD: Save Session
Ddd*data_not_saved_dialog.messageString: \
@rm Without a core dump, the current execution position\n\
and memory contents will not be saved.  Proceed anyway?
Ddd*data_not_saved_dialog.okLabelString:     Yes
Ddd*data_not_saved_dialog.cancelLabelString: No
Ddd*data_not_saved_dialog*helpString: \
@rm In order to restore the current execution position and memory contents,\n\
DDD requires a core dump of the debugged program.\n\
\n\
Without a core dump,\n\
ITEM The current execution position and memory contents will be lost.\n\
    The debugged program must be restarted when restoring the session.\n\
ITEM The current data displays will be deferred.\n\
    When restoring the session and running the program, they will be created\n\
    as soon as their current scope is reached.\n\
\n\
To save the entire state without a core dump, click on LBL(Yes).\n\
To cancel saving the session, click on LBL(No).

define(OPTIONS_CHANGED_HELP, 
[@rm The options file has changed on disk.\n\
\n\
This typically happens when\n\
ITEM options were saved from another DDD instance, or\n\
ITEM the options file has been edited using a text editor.\n])

Ddd*reload_options_dialog.dialogTitle: DDD: Reload Options
Ddd*reload_options_dialog.messageString: \
@rm Options have changed on disk.  Reload them?
Ddd*reload_options_dialog*helpString: \
OPTIONS_CHANGED_HELP\n\
To load the changed options file into this DDD instance, click on LBL(OK).\n\
To ignore the changed options file, click on LBL(Cancel).\n\
\n\
Use LBL(File, Save Session) to save the current options in a session.


Ddd*overwrite_options_dialog.dialogTitle: DDD: Overwrite Options
Ddd*overwrite_options_dialog.messageString: \
@rm Options have changed on disk.  Overwrite them?
Ddd*overwrite_options_dialog*helpString: \
OPTIONS_CHANGED_HELP\n\
To overwrite the changed options file, click on LBL(OK).\n\
To cancel saving options, click on LBL(Cancel).\n\
\n\
Use LBL(File, Save Session) to save the current options in a session.


Ddd*lock_dialog.dialogTitle: DDD: There can be only one
Ddd*lock_dialog*helpString:	\
@rm Another DDD is running, sharing state with this DDD instance.\n\
\n\
To resume execution of this DDD, click on LBL(Continue).\n\
To kill the other DDD instance, click on LBL(Kill).\n\
Otherwise, click on LBL(Exit).\n\
\n\
You can disable this warning by disabling\n\
LBL(Edit, Preferences, General, Warn if multiple DDD instances are running).
Ddd*lock_dialog.okLabelString:		Continue
Ddd*lock_dialog*kill.labelString:	Kill
Ddd*lock_dialog.cancelLabelString: 	Exit

define(THEME_HELP, 
[WIDGET(Apply Theme)\n\
\n\
You are about to apply a theme to an expression or a pattern.\n\
\n\
Applying a theme is EMPH(permanent); that is, it will be applied to all\n\
present and future occurrences of the expression. (You can always\n\
unapply the theme via LBL(Data, Themes).)\n])

Ddd*select_apply_theme_dialog.dialogTitle: DDD: Apply Theme
Ddd*select_apply_theme_dialog.okLabelString:     Apply on all
Ddd*select_apply_theme_dialog*apply.labelString: Apply on this
Ddd*select_apply_theme_dialog*messageString:	\
@rm Apply EMPH(@THEME_DOC@) on all\n\
expressions matching CODE("@PATTERN@")?
Ddd*select_apply_theme_dialog*helpString:	\
THEME_HELP\n\
You can choose where EMPH(@THEME_DOC@) should be applied:\n\
\n\
ITEM To apply EMPH(@THEME_DOC@) on all expressions \
matching CODE("@PATTERN@"),\n\
    click on LBL(Apply on all).\n\
ITEM To apply EMPH(@THEME_DOC@) on CODE(@EXPR@) only,\n\
    click on LBL(Apply on this).\n\
ITEM To cancel applying the theme, click on LBL(Cancel).


Ddd*confirm_apply_theme_dialog.dialogTitle: DDD: Apply Theme
Ddd*confirm_apply_theme_dialog.okLabelString:     Apply
Ddd*confirm_apply_theme_dialog*messageString:	\
@rm Apply EMPH(@THEME_DOC@) on all expressions CODE("@EXPR@")?
Ddd*confirm_apply_theme_dialog*helpString:	\
THEME_HELP\n\
To apply EMPH(@THEME_DOC@) on CODE(@EXPR@), click on LBL(Apply).\n\
To cancel applying the theme, click on LBL(Cancel).

Ddd*launch_tty_dialog.dialogTitle: DDD: Starting Execution Window
Ddd*launch_tty_dialog.messageString:		\
@rm Starting Execution Window...

Ddd*launch_plot_dialog.dialogTitle: DDD: Starting Plot


!-----------------------------------------------------------------------------
! Messages
!-----------------------------------------------------------------------------

! If we have an old app-defaults file installed, we may see an old
! version of this warning.  Gee, app-defaults files are just plain stupid.
Ddd*bad_version_warning.dialogTitle: DDD: Version Mismatch
Ddd*bad_version_warning*helpString:	\
@rm This DDD version does not match the version of the\n\
app-defaults file FILE([Ddd]) or your FILE(~/.[ddd]/init) file.\n\
\n\
ITEM If the app-defaults file FILE([Ddd]) is out of date, DDD may not work as \
expected.\n\
    For instance, this message (being specified in an app-defaults file) may \
be\n\
    out of date.  Please install a recent app-defaults file, or better yet,\n\
    remove it: DDD works perfectly without app-defaults file.  Instead, use \
a\n\
    personal FILE(~/.[ddd]/init) file to customize DDD.\n\
\n\
ITEM If your personal FILE(~/.[ddd]/init) file or some session \
FILE(init) file is out of date,\n\
    use LBL(Edit, Save Options) to update it.

Ddd*class_error.dialogTitle: DDD: No Class
Ddd*class_error*helpString:	\
@rm The given class could not be loaded.\n\
Please check the @GDB@ search path via LBL(Edit, @GDB@ settings).

Ddd*source_file_error.dialogTitle: DDD: No Source
Ddd*source_file_error*helpString:	\
@rm The given source file could not be loaded.\n\
Please check the @GDB@ search path via LBL(Edit, @GDB@ settings).

Ddd*remote_file_error.dialogTitle: DDD: No Remote Source
Ddd*remote_file_error*helpString:	\
@rm The given source file could not be loaded from the remote host.\n\
Please check the @GDB@ search path via LBL(Edit, @GDB@ settings).

Ddd*source_file_from_gdb_warning.dialogTitle: DDD: Source From Debugger
Ddd*source_file_from_gdb_warning*helpString:	\
@rm The given source file could not be loaded directly.\n\
Instead, the source was obtained from @GDB@ via the KBD(list) command.\n\
Please proceed as usual.

Ddd*source_trunc_error.dialogTitle: DDD: Source Truncated
Ddd*source_trunc_error*helpString:	\
@rm The given source file could not be loaded entirely;\n\
the file size has changed during loading.\n\
Please try again as soon as the source file contents are stable.

Ddd*source_empty_warning.dialogTitle: DDD: Empty Source
Ddd*source_empty_warning*helpString:	\
@rm The given source file contains no data.\n\
This is very unusual for source files.\n\
Please verify the file name and try again.

Ddd*source_binary_warning.dialogTitle: DDD: Binary Source
Ddd*source_binary_warning*helpString:	\
@rm The given source file contains NUL characters. \n\
You probably loaded a binary file instead of a source file.\n\
Please verify the file name and try again.

Ddd*manual_find_error.dialogTitle: DDD: Not Found
Ddd*manual_find_error*helpString:	\
@rm The given search key LBL(()) can not be found in the on-line manual.\n\
Please use an alternate key and tru again.

Ddd*no_source_error.dialogTitle: DDD: No Source
Ddd*no_source_error*helpString:	\
@rm There is no current source text in which to search.\n\
Please open a program first, using LBL(File, Open Program).

Ddd*no_source_edit_error.dialogTitle: DDD: No Source
Ddd*no_source_edit_error*helpString:	\
@rm There is no current source text to edit.\n\
Please open a program first, using LBL(File, Open Program).

Ddd*no_such_line_error.dialogTitle: DDD: No such line
Ddd*no_such_line_error*helpString:	\
@rm There is no such line in the current source text.\n\
Please try another line number.

Ddd*no_such_breakpoint_error.dialogTitle: DDD: No Such Breakpoint
Ddd*no_such_breakpoint_error*helpString:	\
@rm There is no such breakpoint in the current program.\n\
Please verify the breakpoint number.

Ddd*no_completion_error.dialogTitle: DDD: No Completion
Ddd*no_completion_error*helpString:	\
@rm @GDB@ does not support command and argument completion.\n\
\n\
Command completion requires the GDB KBD(complete) command,\n\
which is present in GDB 4.13 and later.\n\
\n\
To use command completion, upgrade to a recent @GDB@ version.


Ddd*no_program.dialogTitle: DDD: No Program
Ddd*no_program*helpString: \
@rm You have not opened any program yet.  @GDB@ needs a debuggee\n\
to access core dumps, source files, or attaching to processes.\n\
\n\
You can ignore this warning and proceed, but @GDB@ may be unable\n\
to display symbolic values and determine locations.\n\
\n\
To open a program, use LBL(File, Open Program).


Ddd*print_empty_graph_error.dialogTitle: DDD: Nothing to Print
Ddd*print_empty_graph_error*helpString:	\
@rm There is nothing to print.\n\
The printing has been cancelled.\n\
\n\
Please print again as soon as there is some data to print.

Ddd*print_failed_error.dialogTitle: DDD: Printing Failed
Ddd*print_failed_error*helpString:	\
@rm The file holding the graph picture could not be written.\n\
The printing has been cancelled.\n\
\n\
Please try another file name and try again.

Ddd*export_failed_error.dialogTitle: DDD: Export Failed
Ddd*export_failed_error*helpString:	\
@rm The file holding the plot data could not be written.\n\
The export has been cancelled.\n\
\n\
Please try another file name and try again.

Ddd*unknown_plot_term_type_error: DDD: Unknown Plot Terminal Type
Ddd*unknown_plot_term_type_error*helpString:	\
@rm DDD does not know the plot terminal type specified in the\n\
SAMP(.plotTermType) resource.\n\
Possible SAMP(.plotTermType) values include SAMP(xlib) and SAMP(x11).\n\
\n\
Please set up the SAMP(.plotTermType) resource\n\
in your FILE(~/.[ddd]/init) file and try again.

Ddd*tty_exec_error.dialogTitle: DDD: No Execution Window
Ddd*tty_exec_error*helpString:	\
@rm The terminal emulator for the execution window\n\
could not be invoked.  The debugged process will\n\
execute in the @GDB@ console instead.\n\
\n\
Please verify the contents of the\n\
LBL(Edit, Preferences, Helpers, Execution Window)\n\
setting and make sure that the given\n\
terminal program is within your executable path.

Ddd*tty_command_error.dialogTitle: DDD: TTY Failed
Ddd*tty_command_error*helpString:	\
@rm DDD sent a KBD(tty) command to @GDB@ in order to redirect\n\
input/output of the debugged process to the execution window.\n\
This command failed.\n\
\n\
DDD will now redirect the process I/O explicitly.  This means that\n\
the command window remains the controlling terminal.\n\
\n\
You can tell DDD to not use the KBD(tty) command, but to use explicit\n\
redirection instead, using shell redirection operators.\n\
To do so, include a line\n\
\n\
CODE([Ddd]*useTTYCommand: off)\n\
\n\
in your FILE(~/.[ddd]/init) file and restart DDD.

Ddd*tty_type_error.dialogTitle: DDD: Term Failed
Ddd*tty_type_error*helpString:	\
@rm DDD could not set the TERM environment variable\n\
to the type of the execution window.  This means that\n\
your program will execute with a terminal type\n\
set to SAMP(dumb) instead of SAMP(@XTERM@).\n\
Please see FILE(~/.[ddd]/log) for further diagnostics.

Ddd*history_save_error.dialogTitle: DDD: Save History Failed
Ddd*history_save_error*helpString:	\
@rm The command history could not be saved.\n\
Please verify whether FILE(~/.[ddd]/history) is writable and try again.

Ddd*options_save_error.dialogTitle: DDD: Save Options Failed
Ddd*options_save_error*helpString:	\
@rm The DDD options could not be saved.\n\
Please verify whether FILE(~/.[ddd]/init) is writable and try again.

define(COULD_NOT_BE_UNCOMPRESSED,
[could not be uncompressed.\n\
\n\
Please verify the contents of the\n\
LBL(Edit, Preferences, Helpers, Uncompress)\n\
setting and make sure that the given\n\
uncompressing program is within your executable path.])dnl

Ddd*no_license_error.dialogTitle: DDD: No License
Ddd*no_license_error*helpString:	\
@rm The DDD license COULD_NOT_BE_UNCOMPRESSED

Ddd*no_news_error.dialogTitle: DDD: No news is good news
Ddd*no_news_error*helpString:	\
@rm The DDD news COULD_NOT_BE_UNCOMPRESSED

Ddd*no_ddd_manual_error.dialogTitle: DDD: No Manual
Ddd*no_ddd_manual_error*helpString:	\
@rm The DDD manual COULD_NOT_BE_UNCOMPRESSED

Ddd*no_ddd_man_page_error.dialogTitle: DDD: No Manual
Ddd*no_ddd_man_page_error*helpString:	\
@rm It seems the DDD manual page is not installed.\n\
Please install the DDD manual page and try again.

Ddd*startup_warning.dialogTitle: DDD: New Startup Required
Ddd*startup_warning*helpString: \
@rm This setting cannot be changed while DDD is running.\n\
Please save options and restart DDD to see the effects.

Ddd*grab_warning.dialogTitle: DDD: Pointer Frozen
Ddd*grab_warning*helpString: \
@rm DDD has detected a frozen (grabbed) mouse pointer.\n\
\n\
This may be because @GDB@ has interrupted the debuggee\n\
while it was grabbing the mouse pointer.\n\
If DDD remains in this state, further interaction will\n\
probably be impossible.

Ddd*no_display_number_warning.dialogTitle: DDD: No Display Number
Ddd*no_display_number_warning*helpString: \
@rm DDD could not determine the number of the new display\n\
as assigned by @GDB@.  DDD will use a default number instead.

Ddd*invalid_range_error.dialogTitle: DDD: Invalid Range
Ddd*invalid_range_error*helpString: \
@rm The displays to be created have an invalid range VAR(x)..VAR(y).\n\
The range must be where VAR(x)..VAR(y) with VAR(x) < VAR(y).\n\
Please verify the range and try again.

Ddd*expired_warning.dialogTitle: DDD: DDD Expired
Ddd*expired_warning*helpString: \
@rm DDD @VERSION@ has been superseded by a newer DDD version.\n\
DDD @VERSION@ should no longer be used.\n\
\n\
Please upgrade to a recent DDD version.\n\
For details, see the LBL(Help, [[[[DDD]]]] WWW Page).

Ddd*core_missing_warning.dialogTitle: DDD: Core Missing
Ddd*core_missing_warning*helpString: \
@rm DDD could not get a core dump of the debugged program.\n\
This means that the current memory contents and execution position\n\
will be lost when restoring the session.\n\
\n\
Please verify the settings in\n\
LBL(Edit, Preferences, Helpers, Get Core File);\n\
try again without helper if appropriate.\n\
\n\
Also be sure that you have not disabled core dumps\n\
(hint: look for SAMP(limit) and SAMP(ulimit) commands \
in your shell startup file)

Ddd*program_name_missing_warning.dialogTitle: DDD: Program Name Missing
Ddd*program_name_missing_warning*helpString: \
@rm DDD could not determine the name of the current debuggee.\n\
Please try again when @GDB@ is ready.

Ddd*breakpoint_missing_warning.dialogTitle: DDD: Breakpoints Missing
Ddd*breakpoint_missing_warning*helpString: \
@rm DDD could not save the current breakpoint state.\n\
Please try again when @GDB@ is ready.

Ddd*displays_missing_warning.dialogTitle: DDD: Data Displays Missing
Ddd*displays_missing_warning*helpString: \
@rm DDD could not save the entire data display state.\n\
Please try again when @GDB@ is ready.

Ddd*no_sessions_error.dialogTitle: DDD: No Sessions
Ddd*no_sessions_error*helpString: \
@rm DDD could not access the session repository.\n\
Please verify whether the FILE(~/.[ddd]/sessions/) is accessible.

Ddd*delete_session_error.dialogTitle: DDD: Delete Session Failed
Ddd*delete_session_error*helpString: \
@rm DDD could not entirely remove the session directory from the repository.\n\
Please verify whether the FILE(~/.[ddd]/sessions/) is writable.

Ddd*too_many_shortcuts_warning.dialogTitle: DDD: Too many Shortcuts
Ddd*too_many_shortcuts_warning*helpString:	\
@rm DDD has not enough room for all display shortcuts and will\n\
only display the most recently created shortcuts.\n\
Please edit the list of display shortcuts, using LBL(Display, Edit Menu).

Ddd*gdb_io_error.dialogTitle: DDD: I/O Error
Ddd*gdb_io_error*helpString: \
@rm The communication between @GDB@ and DDD does not work correctly.\n\
This is probably due to a bad DDD configuration.\n\
\n\
DDD cannot work around this problem, so you may not be able to continue.\n\
Restart DDD and try again.

Ddd*gdb_io_warning.dialogTitle: DDD: I/O Warning
Ddd*gdb_io_warning*helpString: \
@rm The communication between @GDB@ and DDD does not work correctly.\n\
This is probably due to a bad DDD configuration.\n\
\n\
DDD can work around this problem, so you may continue working.

Ddd*out_of_glyphs_warning.dialogTitle: DDD: Out of Glyphs
Ddd*out_of_glyphs_warning*helpString: \
@rm DDD can only display a limited amount of breakpoint symbols at one time.\n\
The other breakpoints are still maintained, but not visible right now.\n\
\n\
To increase the maximum number VAR(N) of displayed breakpoint symbols, \
include a line\n\
\n\
CODE([Ddd]*maxGlyphs: VAR(N))\n\
\n\
in your FILE(~/.[ddd]/init) file and restart DDD.

Ddd*no_vsllib_error.dialogTitle: DDD: Could not load VSL library
Ddd*no_vsllib_error*helpString: \
@rm DDD could not load its VSL library\n\
and will not be able to show data displays.\n\
\n\
This error typically happens when DDD installation is (yet) incomplete.\n\
Did you already type LBL(make install)?\n\
\n\
If you don't want to install DDD, you can work around this error by\n\
ITEM installing the file FILE(vsllib/[ddd].vsl)\n\
    into FILE(@ROOT@/vsllib/[ddd].vsl), or\n\
ITEM have the environment variable VAR(DDD_HOME) point at the place\n\
    where the FILE(vsllib) directory resides.\n\
\n\
To get a DDD demo before installation, simply type LBL(make check).\n\
If this error persists, something else has gone wrong.

Ddd*vsl_message.dialogTitle: DDD: VSL Messages
Ddd*vsl_message*helpString:	\
The VSL interpreter issued a message while parsing and/or evaluating\n\
VSL expressions.  Your data displays may not be rendered successfully.\n\
\n\
STRONG(Please note:) This dialog only lists the EMPH(first) problem encountered.\n\
For further diagnostics, see the status line (and the FILE(~/.[ddd]/log) file).

Ddd*no_themes_warning.dialogTitle: DDD: No Themes
Ddd*no_themes_warning*helpString:	\
@rm The specified directory could not be found.\n\
\n\
This error typically happens when DDD installation is (yet) incomplete.\n\
Did you already type LBL(make install)?\n\
\n\
Please verify whether the directories in the SAMP(vslPath) resource\n\
(including your home directory) exist.\n\
\n\
If you work on Windows, be sure not to have SAMP(:) in your HOME environment\n\
variable.




!-----------------------------------------------------------------------------
! Hints on specific DDD states (`What now')
!-----------------------------------------------------------------------------

Ddd*no_program.helpString: \
@rm You have not opened any program yet.  @GDB@ needs a debuggee\n\
to access core dumps, source files, or attaching to processes.\n\
\n\
To open a program, use LBL(File, Open Program).

define(NO_SOURCE_HELP,
[ITEM Use LBL(File, Open Source) to select a source file.\n\
ITEM If this does not work, your program may lack debugging information.\n\
    Without debugging information, @GDB@ will be unable to locate the source\n\
    code.  To request debugging information, specify the SAMP(-g) option\n\
    when you run the compiler.\n\
ITEM You may need to tell @GDB@ where the source code files are.\n\
    Invoke LBL(Edit, @GDB@ Settings) and look for appropriate entries.\n\
    Using GDB, you can also create a local FILE(.gdbinit) file that contains\n\
    SAMP(directory VAR(path)), where VAR(path) is the source path.\n\
ITEM Using GDB, you can continue at machine code level \
by enabling the\n\
    Machine Code Window.  Use LBL(Source, Display Machine Code).])dnl

Ddd*no_source_and_no_code.helpString: \
@rm @GDB@ cannot find the source code of your program.\n\
\n\
Here are some hints that may help you out:\n\
NO_SOURCE_HELP

Ddd*recording.helpString: \
@rm @GDB@ is recording commands.\n\
\n\
While recording, commands are not executed; instead, \
@GDB@ EMPH(records) them\n\
for execution when a breakpoint is reached or for user-defined commands.\n\
\n\
ITEM To end the recording, click on LBL(End) \
or enter KBD(end) at the @GDB@ prompt.\n\
ITEM To cancel the recording, select LBL(Program, Interrupt) \
or press KEY(Esc).

Ddd*busy.helpString: \
@rm @GDB@ is busy on some debugger action.\n\
\n\
You may\n\
ITEM wish to wait until the @GDB@ prompt appears, or\n\
ITEM interrupt the current debugging command by selecting\n\
    LBL(Program, Interrupt) (or typing KEY(Esc)).

Ddd*running.helpString: \
@rm @GDB@ is executing the debugged program.\n\
\n\
You may\n\
ITEM interact with the program until a breakpoint is reached, or\n\
ITEM interrupt program execution by selecting\n\
    LBL(Program, Interrupt) (or typing KEY(Esc)).

Ddd*program_not_running.helpString: \
@rm Your program is not running.\n\
\n\
You may now choose between the following:\n\
ITEM Start the program, using LBL(Program, Run). \
You may wish to \n\
    set some breakpoints beforehand, using LBL(Break at ()) and \n\
    LBL(Source, Edit Breakpoints).\n\
    To find specific functions, use LBL(Lookup ()).\n\
ITEM If your program is already running outside of DDD, you can\n\
    attach to its running process, using LBL(File, Attach to Process).\n\
ITEM If some previous invocation of your program has failed, and\n\
    you want to know why, use LBL(File, Open Core Dump) to open\n\
    its core dump.

Ddd*item_selected.helpString: \
@rm You have selected some item from the program source code.\n\
\n\
ITEM To look up further occurences in the source code, use LBL_FIND_BACKWARD\n\
    and LBL_FIND_FORWARD.  \
LBL(Lookup ()) leads you to the item's definition.\n\
ITEM To display the value, use LBL(Print ()) (for simple values)\n\
    and LBL(Display ()) (for complex values and dynamic displays).

Ddd*display_selected.helpString: \
@rm You have selected some data display.\n\
\n\
ITEM To examine further members of this data structure,\n\
     use LBL(New Display ()) and LBL(Display *()).\n\
ITEM To modify the appearance of the selected display,\n\
     use LBL(Hide ()) and LBL(Rotate ()).\n\
ITEM To view numerical data as a plot, use LBL(Plot ()).\n\
ITEM To change the value of the selected display, use LBL(Set ()).\n\
ITEM To delete the selected displays, use LBL(Delete ()).\n\
ITEM Use LBL(Data, Edit Displays) for further editing.

Ddd*command_entered.helpString: \
@rm You have entered some command at the @GDB@ prompt.\n\
\n\
ITEM To execute the current command, press KEY_RETURN.\n\
ITEM To complete the current word, use KEY(TAB).\n\
ITEM To move through the command history, use KEY(Up) and KEY(Down).\n\
ITEM To clear the current line, use KEY(Ctrl+U).\n\
ITEM For further options, see the LBL(Commands) menu.

Ddd*code_but_no_source.helpString: \
@rm Your program @PROGRAM_STATE@,\n\
and there is no source code for the current execution position.\n\
\n\
Here are some hints that may help you out:\n\
ITEM Use LBL(Status, Up) to step out of libraries \
and to enter your own functions.\n\
NO_SOURCE_HELP

define(STOPPED_HELP,
[ITEM To find out how you got here, use LBL(Status, Backtrace).\n\
ITEM To examine a variable value, select it first (click on an \
occurrence).\n\
    Then use LBL(Print ()) (simple values) or LBL(Display ()) \
(complex values).\n\
    Press BUTTON(3) on the variable name for a shortcut.\n\
ITEM You can display all local variables at once using\n\
    LBL(Data, Display Local Variables).\n\
ITEM To set and clear further breakpoints, use \
LBL(Source, Edit Breakpoints).\n\
    Press BUTTON(3) on a location or breakpoint to get a shortcut.])dnl

Ddd*stopped_at_passed_signal.helpString: \
@rm Your program @PROGRAM_STATE@.\n\
\n\
STOPPED_HELP\n\
\n\
When resuming execution, the @SIGNAL_DESCRIPTION@ signal\n\
will be passed to the program.\n\
ITEM To continue execution without giving a @SIGNAL@ signal,\n\
    use LBL(Commands, Continue Without Signal).\n\
ITEM To see or change the current signal handling, use LBL(Status, Signals).

Ddd*stopped_at_ignored_signal.helpString: \
@rm Your program @PROGRAM_STATE@.\n\
\n\
STOPPED_HELP\n\
\n\
When resuming execution, the @SIGNAL_DESCRIPTION@ signal \
will EMPH(not) be passed to the program.\n\
ITEM To pass this signal to the program, \
use the @GDB@ command KBD(signal @SIGNAL@).\n\
ITEM To see or change the current signal handling, use LBL(Status, Signals).

Ddd*stopped.helpString: \
@rm Your program @PROGRAM_STATE@.\n\
\n\
STOPPED_HELP\n\
ITEM Use the functions of the LBL(Program) menu to resume execution\n\
    and step through your program.

Ddd*showing_earlier_state.helpString: \
@rm DDD is showing an earlier state of your program.\n\
\n\
This happens when you undo a command affecting the program state.\n\
DDD cannot restore the earlier program state.  Instead, DDD goes into\n\
EMPH(historic mode) showing you all it remembers about this state.\n\
\n\
Most normal DDD commands that would query further information\n\
are disabled in historic mode.  However, you can examine\n\
the current execution position, or the active displays.\n\
\n\
ITEM To revert to the current program state, use LBL(Edit, Redo).\n\
ITEM To view even earlier states of your program, use LBL(Edit, Undo).


!-----------------------------------------------------------------------------
! Startup tips
!-----------------------------------------------------------------------------

Ddd*tip_dialog.dialogTitle: 		DDD Tip of the Day
Ddd*tip_dialog*helpString:	\
@rm This is the DDD tip of the day.
Ddd*set_startup_tips.labelString:       \
@small Show Tip of the Day when starting DDD
Ddd*set_startup_tips.alignment:         XmALIGNMENT_BEGINNING
Ddd*tip_dialog.applyLabelString:	Turn Off
Ddd*tip_dialog.cancelLabelString:	Prev Tip
Ddd*tip_dialog.helpLabelString:		Next Tip
Ddd*tip_dialog.symbolPixmap: 		ddd

dnl Tips will be appended here.
! Avoid automatic merging of these resources.
! Local Variables:
! xrdb-master-file: "~/.Xdefaults"
! End:
