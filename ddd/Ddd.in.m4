! $Id$ -*- xrdb -*-
! DDD Resource File.  DO NOT INSTALL THIS FILE!  See below for details.
! @configure_input@
! Generated automatically from Ddd.in.m4 by @M4@.
include(ifdef(`srcdir',srcdir()/macros.m4,macros.m4))dnl
Ddd*appDefaultsVersion: @VERSION@

! Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
! Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
! 
! This file is part of DDD.
! 
! DDD is free software; you can redistribute it and/or
! modify it under the terms of the GNU General Public
! License as published by the Free Software Foundation; either
! version 2 of the License, or (at your option) any later version.
! 
! DDD is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
! See the GNU General Public License for more details.
! 
! You should have received a copy of the GNU General Public
! License along with DDD -- see the file COPYING.
! If not, write to the Free Software Foundation, Inc.,
! 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
! 
! DDD is the data display debugger.
! For details, see the DDD World-Wide-Web page, 
! `http://www.cs.tu-bs.de/softech/ddd/',
! or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

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


!-----------------------------------------------------------------------------
! User-settable resources
!-----------------------------------------------------------------------------

! The inferior debugger type (dbx, gdb, xdb, or jdb; defaults to gdb)
Ddd*debugger:


! The name under which the inferior debugger is to be invoked
! (default: debugger type)
Ddd*debuggerCommand:


! True if data and source are to be displayed in separate top-level
! windows.  If this is false, the data and source windows are attached
! to the debugger console.
Ddd*Separate: false


! You may also attach the source window or data window alone.
! Ddd*separateDataWindow: true
! Ddd*separateSourceWindow: true


! True if programs are to be run in a separate top-level window.
! If this is false, input/output is sent to the debugger console.
!
! Although a separate execution window has several advantages, it
! is less portable and more error-prone, which is why it is disabled 
! by default.
Ddd*separateExecWindow: false


! True if DDD windows are to be iconified/uniconified as a whole.
! If this is false, every DDD window can be iconified on its own.
Ddd*groupIconify: false


! True if DDD windows are to be uniconified when GDB becomes ready.
Ddd*uniconifyWhenReady: true


! True if the TAB key is to complete text in all DDD windows.
! If this is false, the TAB key completes in the debugger console only.
Ddd*globalTabCompletion: true


! True if the debugger console is line-buffered (i.e. flushed upon 
! each newline).  This is slower, but gives nice scrolling.
! False if newline has no special treatment.
Ddd*lineBufferedConsole: true


! True if options are to be saved in `~/.ddd/init' across DDD invocations.
Ddd*saveOptionsOnExit: false


! The command to invoke the execution window for showing I/O of the debugged
! program.  To this string, DDD appends bourne shell commands which must be
! run in the execution window.
Ddd*termCommand: xterm \
-bg 'grey90' -fg 'black' -cr 'DarkGreen' \
-fn '-*-lucidatypewriter-medium-r-*-*-12-*-*-*-*-*-*-*' \
-title 'DDD: Execution Window' -e /bin/sh -c


! The terminal type provided by `termCommand' (that is, the value of
! the TERM environment variable to be passed to the debugged program)
Ddd*termType: xterm


! The `useTTYCommand' resource sets the redirection mode for the execution
! window. Basically, there are two ways to redirect process I/O.  Both have
! advantages and disadvantages.
!
! * GDB provides a `tty' command to redirect I/O to a separate tty.
!   This is portable, but on some systems, there is the nasty side-effect
!   that redirection can not be undone.
!   For DBX, this means using the `dbxenv run_io pty' and `dbxenv run_pty'
!   commands to redirect I/O to the execution window.
!   Set `useTTYCommand' to `true' to enable this behaviour.
! 
! * As an alternative, DDD can redirect the process I/O explicitly,
!   without using the `tty' command.  This is done with XDB and 
!   with DBX versions that do not support tty redirection.
!   Explicit redirection may fail if you use a non-standard shell.
!   Set `useTTYCommand' to `false' to enable this behaviour.
!   
! The default setting is `false', since explicit redirection is easier
! to handle (and to debug :-).  Maybe one day GDB will provide a way
! to undo redirection.
Ddd*useTTYCommand: false


! The `blockTTYInput' resource sets whether DDD should block when
! reading data from GDB via the pseudo-tty interface.  Some systems
! require this, such as Linux with libc 5.4.33 and earlier; set it to
! `true'.  Some other systems prohibit this, such as Linux with GNU libc
! 6 and later; set it to `false'.  The value `auto' will select the
! `best' choice (that is, the best choice known to the DDD developers).
Ddd*blockTTYInput: auto


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
! We try the BSD style `ps -x' first, then System V `ps -ef'.  If that
! fails, too, use plain `ps'.
Ddd*psCommand: ps -x 2> /dev/null || ps -ef 2> /dev/null || ps


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
|| xterm -e ${EDITOR-vi} +@LINE@ @FILE@


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
Ddd*wwwPage: http://www.cs.tu-bs.de/softech/ddd/

! Command to invoke the WWW page.  @URL@ stands for the URL to open.
! Default is to try a running Netscape first, then to invoke a new 
! Netscape process, then to let a running Emacs do the job, then to
! invoke Mosaic, then to invoke Lynx.
! Note that if you don't have WWW access, letting all these commands fail
! may take a lot of time...
Ddd*wwwCommand: \
   netscape -remote 'openURL(@URL@)' \
|| netscape '@URL@' \
|| gnudoit '(w3-fetch \042@URL@\042)' \
|| mosaic '@URL@' \
|| Mosaic '@URL@' \
|| xterm -e lynx '@URL@'

! The time (in seconds) to wait for synchronous GDB questions to complete
Ddd*questionTimeout: 10


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
! - `Back'     : Lookup previously selected source position
! - `Forward'  : Lookup next selected source position
! - `Edit'     : Edit current source file
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
run\nstep\nstepi\nnext\nnexti\ncont\nfinish\n\
up\ndown\nBack\nForward\nEdit\nkill\nbreak^C

Ddd*break.labelString: Interrupt

! The number of rows in the command tool.
Ddd*tool_buttons.fractionBase:			8

! The positions are set up according to the following scheme:
! 
!    0 4 8	Each line has its individual number.
!  0 +-+-+	`topPosition' refers to the line above the button;
!    | | |	`bottomPosition' is the line below.  Likewise,
!  1 +-+-+	`leftPosition' is the line at the left, and
!    | | |	`rightPosition' is the line to the right.  For instance,
!  2 +-+-+	the line above the `break' button
!    | | |	is line 1 (topPosition), the line below is line 2
!  3 +-+-+	(bottomPosition), the line to the left is line 0
!    | | |	(leftPosition), and the line to the right is line 8
!  4 +-+-+	(rightPosition).
!    | | |	
!  5 +-+-+	To replace a button by another command, change the
!    | | |	`toolButtons' resource, above, and insert appropriate
!  6 +-+-+	resource values for the new button, specifying its
!    | | |	location.
!  7 +-+-+	
!    | | |	
!  8 +-+-+	

Ddd*tool_buttons.run.topPosition:		0
Ddd*tool_buttons.run.bottomPosition:		1
Ddd*tool_buttons.run.leftPosition:		0
Ddd*tool_buttons.run.rightPosition:		8

Ddd*tool_buttons.break.topPosition:		1
Ddd*tool_buttons.break.bottomPosition:		2
Ddd*tool_buttons.break.leftPosition:		0
Ddd*tool_buttons.break.rightPosition:		8

Ddd*tool_buttons.step.topPosition:		2
Ddd*tool_buttons.step.bottomPosition:		3
Ddd*tool_buttons.step.leftPosition:		0
Ddd*tool_buttons.step.rightPosition:		4

Ddd*tool_buttons.stepi.topPosition:		2
Ddd*tool_buttons.stepi.bottomPosition:		3
Ddd*tool_buttons.stepi.leftPosition:		4
Ddd*tool_buttons.stepi.rightPosition:		8

Ddd*tool_buttons.next.topPosition:		3
Ddd*tool_buttons.next.bottomPosition:		4
Ddd*tool_buttons.next.leftPosition:		0
Ddd*tool_buttons.next.rightPosition:		4

Ddd*tool_buttons.nexti.topPosition:		3
Ddd*tool_buttons.nexti.bottomPosition:		4
Ddd*tool_buttons.nexti.leftPosition:		4
Ddd*tool_buttons.nexti.rightPosition:		8

Ddd*tool_buttons.cont.topPosition:		4
Ddd*tool_buttons.cont.bottomPosition:		5
Ddd*tool_buttons.cont.leftPosition:		0
Ddd*tool_buttons.cont.rightPosition:		4

Ddd*tool_buttons.finish.topPosition:		4
Ddd*tool_buttons.finish.bottomPosition:		5
Ddd*tool_buttons.finish.leftPosition:		4
Ddd*tool_buttons.finish.rightPosition:		8

Ddd*tool_buttons.up.topPosition:		5
Ddd*tool_buttons.up.bottomPosition:		6
Ddd*tool_buttons.up.leftPosition:		0
Ddd*tool_buttons.up.rightPosition:		4

Ddd*tool_buttons.down.topPosition:		5
Ddd*tool_buttons.down.bottomPosition:		6
Ddd*tool_buttons.down.leftPosition:		4
Ddd*tool_buttons.down.rightPosition:		8

Ddd*tool_buttons.Back.topPosition:		6
Ddd*tool_buttons.Back.bottomPosition:		7
Ddd*tool_buttons.Back.leftPosition:		0
Ddd*tool_buttons.Back.rightPosition:		4

Ddd*tool_buttons.Forward.topPosition:		6
Ddd*tool_buttons.Forward.bottomPosition:	7
Ddd*tool_buttons.Forward.leftPosition:		4
Ddd*tool_buttons.Forward.rightPosition:		8

Ddd*tool_buttons.Edit.topPosition:		7
Ddd*tool_buttons.Edit.bottomPosition:		8
Ddd*tool_buttons.Edit.leftPosition:		0
Ddd*tool_buttons.Edit.rightPosition:		4

Ddd*tool_buttons.kill.topPosition:		7
Ddd*tool_buttons.kill.bottomPosition:		8
Ddd*tool_buttons.kill.leftPosition:		4
Ddd*tool_buttons.kill.rightPosition:		8

! Command tool button placement.  Don't change this.
Ddd*tool_buttons*topAttachment:			XmATTACH_POSITION
Ddd*tool_buttons*bottomAttachment:		XmATTACH_POSITION
Ddd*tool_buttons*leftAttachment:		XmATTACH_POSITION
Ddd*tool_buttons*rightAttachment:		XmATTACH_POSITION

! Some special labels
Ddd*tool_buttons*Forward.labelString:   Fwd
Ddd*tool_bar*Cont.labelString:   Continue



! Command tool stuff.

! Do we want a sticky command tool?  If true, the command tool follows
! every movement of the source window, such that the relative position
! remains unchanged.
Ddd*stickyTool: true

! Do we want a decorated command tool?
! * If `true', DDD will set up the command tool as a top-level window,
!   which is decorated like DDD main windows.
! * If `false', DDD realizes the command tool as a transient window, which
!   may be decorated or not, depending on the settings of your window
!   manager.
! * If `auto', DDD checks the window manager.  If it decorates
!   transients, the command tool is realized as a transient, and as
!   a top-level-window. otherwise.
Ddd*decoratedTool: auto

! Do we want an auto-raised command tool?  If true, DDD will always 
! keep the command tool on top of other DDD windows.  If this setting
! interferes with your window manager, set this resource to false.
Ddd*autoRaiseTool: true

! Do we want auto-raised menus?  This is handy with certain window managers:
! An auto-raised DDD might obscure popped up pulldown menus.  However, there's
! a risk that this might interfere with Motif, which is why this is `false'
Ddd*autoRaiseMenu: false


! Shortcuts
! A newline-separated list of expressions to be included in the
! `New Display' menu.  This is usually edited at run-time.
Ddd*displayShortcuts: \
/d ()	// Convert to Dec\n\
/x ()	// Convert to Hex\n\
/o ()	// Convert to Oct


! Tab width in source texts
Ddd*tabWidth:	        8

! Number of characters to indent the source and machine code
Ddd*indentSource:	4
Ddd*indentCode:	        4

! Width of line numbers (if enabled; see the `displayLineNumbers' resource)
! Line numbers wider than this value extend into the breakpoint space.
Ddd*lineNumberWidth:	4

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
Ddd*gdbInitCommands:	\
set prompt (gdb) \n\
set height 0\n\
set width 0\n\
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

! The XDB settings.  Usually overridden in `~/.ddd/init'.
Ddd*xdbSettings:


! The JDB initialization commands.  Nothing special.
Ddd*jdbInitCommands:

! The JDB settings.  Usually overridden in `~/.ddd/init'.
Ddd*jdbSettings:


! Auto-command defaults

! Are auto-commands enabled?  If yes, any output of the inferior debugger
! in the form `ddd: COMMAND' will cause DDD to execute the so-called 
! auto-command COMMAND.  This is useful for defining own DDD command 
! sequences.
! Unfortunately, this is also a security problem - just imagine some
! malicuous program to issue a string like `ddd: shell rm foobar'.
! Consequently, this feature is disabled by default.
Ddd*autoCommands: false

! The prefix of auto-commands (by default, `ddd: ' - note the trailing space).
! You are encouraged to change this value in your `~/.ddd/init' file.
Ddd*autoCommandPrefix: ddd:\ 


! Graph Editor Defaults

! Shall we detect aliases?  (expensive)
Ddd*detectAliases:		false

! Shall we layout two-dimensional arrays as tables?
Ddd*align2dArrays:		true

! After a change in a display, shall we move adjacent displays?
Ddd*bumpDisplays:		true

! Shall we hide displays that are out of scope?
Ddd*hideInactiveDisplays:	true

! The VSL library for displaying graph nodes.  `builtin' means the VSL
! library compiled into DDD, named `ddd.vsl'.  If you wish to use
! another VSL library, put its name here and set `vslPath' to a
! colon-separated list of directories where the VSL file and its
! includes can be found.
!
! If you use the builtin library, you can use the `vslDefs' resource,
! to override (customize) certain settings.  See `colors', below.
Ddd*vslLibrary:			builtin
Ddd*vslPath:			.

! What shall we use if the graph gets too large to be displayed?
! If true, an Athena panner is used (a kind of two-directional scrollbar).
! If false, two Motif scrollbars are used.
!
! Although a panned graph editor is much more comfortable, we set-up Motif
! scrollbars as a default: Many OSF/Motif installations choke
! on intermixed Motif/Athena applications and we don't want to get bug
! reports that should be directed to OSF.
Ddd*pannedGraphEditor:		false

! Nodes are moved when dragged more than 4 pixels.
Ddd*graph_edit.moveDelta:	4

! The selection tile pixmap.  Usual choices are Motif built-in pixmaps.
! If this resource is left unspecified, nodes are simply inverted.
! Ddd*graph_edit.selectTile:	25_foreground

! Do we wish opaque movement? (doesn't work yet)
Ddd*graph_edit.opaqueMove:	false

! Do we wish rubber edges when moving nodes?
Ddd*graph_edit.rubberEdges:	true

! Do we wish rubber arrows when moving nodes?
Ddd*graph_edit.rubberArrows:	true

! Do we wish a grid?
Ddd*graph_edit.showGrid:	true

! Do we wish to snap the position to the grid?
Ddd*graph_edit.snapToGrid:	true

! Do we wish to show edge hints?
Ddd*graph_edit.showHints:	true

! What kind of layout do we wish? (regular, compact)
Ddd*graph_edit.layoutMode:	regular

! Do we wish to re-layout the graph after each change?
Ddd*graph_edit.autoLayout:	false

! Width and height of the grid
! Ddd*graph_edit.gridWidth:	16
! Ddd*graph_edit.gridHeight:	16
Ddd*graph_edit.GridSize:	16


! Source View Defaults

! Do we wish to find complete words only?
Ddd*findWordsOnly:		true

! Do we wish case-sensitive search?
Ddd*findCaseSensitive:		true

! Do we wish to show machine code? (Makes DDD run somewhat slower)
Ddd*disassemble: false

! How many bytes of the current function should we disassemble? (0
! means disassemble entire function; default in DDD 2.2 and earlier)
Ddd*maxDisassemble: 1024

! Do we wish to display glyphs? (Makes DDD run somewhat slower)
Ddd*displayGlyphs: true

! What is the maximum number of glyphs at one time?
Ddd*maxGlyphs: 10

! Shall we cache glyph images?  (Won't work with Motif 2.1 on XFree86)
! Ddd*cacheGlyphImages: true

! Do we wish to display line numbers? (Default in DDD 2.1.1 and earlier)
Ddd*displayLineNumbers: false

! Do we wish to cache source files in memory?
Ddd*cacheSourceFiles: true

! Do we wish to cache machine code in memory?
Ddd*cacheMachineCode: true

! Do we wish to refer to sources using the full path name?
Ddd*useSourcePaths: false

! Do we wish to show all registers, instead of only integer registers?
Ddd*allRegisters: false

! Tips (aka ballon help or bubble help) are small windows that pop up
! when moving the pointer over some item.  Instructive for DDD
! novices, but maybe annoying for DDD experts.

! Do we want tips on buttons, telling us their meaning?
Ddd*buttonTips: true

! Do we want tips on variables in the source code, telling us their value?
Ddd*valueTips: true


! The same info as in tips may also be shown in the status line (`docs').
! This is less annoying, but may slow down DDD a bit.
! Do we want docs on buttons, telling us their meaning?
Ddd*buttonDocs: true

! Do we want docs on variables in the source code, telling us their value?
Ddd*valueDocs: true


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


! Should the status line be placed at the bottom?
Ddd*statusAtBottom: true

! Number of status messages to be kept in status history
Ddd*statusHistorySize: 20

! Do we want to see the debugger status (blinking while busy)?
Ddd*blinkWhileBusy: true

! Blink rate (in ms) of the busy LED - the LED showing that the inferior
! debugger is busy.  A value of 0 prevents the busy LED from blinking.
Ddd*busyBlinkRate: 500


! The maximum length of a data display title (in characters)
! Longer names are replaced by `..'; `foo->bar->blue' becomes `foo->..->blue'.
Ddd*maxDisplayTitleLength: 20

! The maximum length of an expression in the source popup.
Ddd*maxPopupExprLength: 20

! The maximum length of a value tip.
Ddd*maxValueTipLength: 60

! The maximum length of a value doc.
Ddd*maxValueDocLength: 80



! Focus Policy
! POINTER means point-and-type, EXPLICIT means click-and-type.
! We accept the user's default setting here.
! Ddd*keyboardFocusPolicy:	POINTER

! X Warnings
! Do we wish to suppress X warnings?
Ddd*suppressWarnings: false

! Shall DDD ungrab the mouse pointer when interrupting a modal X application?
Ddd*ungrabMousePointer: false

! A little less highlighting
Ddd*highlightThickness: 1


! Menus
! Enable tear-off pulldown menus
Ddd*menubar*tearOffModel: XmTEAR_OFF_ENABLED


! Logos and icons

! DDD can display its splash screen in five different ways:
!
!   Color (`c')  - using 42 shades of blue.     (Nice, but costly.)
!   Grey  (`g')  - using 42 shades of grey.     (Nice on greyscale displays.)
!   Grey4 (`g4') - using 4 shades of grey.      (A balanced compromise.)
!   Mono  (`m')  - using only black and white.  (Ugly; on monochrome displays.)
!   None  (`')   - don't show the DDD logo.     (Makes DDD start up faster.)
!
! Use the key given in parentheses as resource value.
! Default is `c' for color.
Ddd*showStartupLogo: c

! Whether to use multi-color icons.  Multi-color icons are not strictly
! ICCM-compliant, although supported by most window managers.  In case
! your window manager has trouble with DDD icons, set this to `false'.
Ddd*colorIcons: true


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


!-----------------------------------------------------------------------------
! Fonts
!-----------------------------------------------------------------------------

! Font definitions.  @<font> is used to switch between fonts.
! @charset is the default font.
! @tt (teletype) is the fixed-width font used in texts.
! @tb (teletype bold) is the bold variant of @tt.
! @key is the font used for keys.
! @rm (roman), @sl (slanted), @bf (bold face), @bs (bold slanted)
! are some fonts for usual highlighting.
! @Logo and @logo are used in the DDD logo.
!
! Note: These font names are also used in DDD.  See `ddd/charsets.h'.
!
! Note: LessTif 0.79 always wants `FONTLIST_DEFAULT_TAG_STRING' 
! instead of `charset' for TextField and Text widgets; this must also 
! be the first font specified.

Ddd*fontList: \
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=charset,\
-*-helvetica-bold-r-*-*-10-*-*-*-*-*-*-*=small,\
-*-lucidatypewriter-medium-r-*-*-12-*-*-*-*-*-*-*=tt,\
-*-lucidatypewriter-bold-r-*-*-12-*-*-*-*-*-*-*=tb,\
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=key,\
-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*=rm,\
-*-helvetica-medium-o-*-*-12-*-*-*-*-*-*-*=sl,\
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=bf,\
-*-helvetica-bold-o-*-*-12-*-*-*-*-*-*-*=bs,\
-*-helvetica-bold-r-*-*-14-*-*-*-*-*-*-*=Logo,\
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=logo,\
-*-symbol-*-*-*-12-*-*-*-*-*-*-*=symbol

! An alternate, resolution-independent setting.
! Ddd*fontList: \
! -*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
! -*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=charset,\
! -*-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*=small,\
! -*-lucidatypewriter-medium-r-*-*-*-90-*-*-*-*-*-*=tt,\
! -*-lucidatypewriter-bold-r-*-*-*-90-*-*-*-*-*-*=tb,\
! -*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=key,\
! -*-helvetica-medium-r-*-*-*-90-*-*-*-*-*-*=rm,\
! -*-helvetica-medium-o-*-*-*-90-*-*-*-*-*-*=sl,\
! -*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=bf,\
! -*-helvetica-bold-o-*-*-*-90-*-*-*-*-*-*=bs,\
! -*-helvetica-bold-r-*-*-*-120-*-*-*-*-*-*=Logo,\
! -*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=logo,\
! -*-symbol-*-*-*-*-90-*-*-*-*-*-*=symbol

! Fixed-width fonts.  `lucidatypewriter' is the font the DDD developers
! prefer; another popular choice is `courier'.
Ddd*XmTextField.FontList: \
-*-lucidatypewriter-medium-r-*-*-12-*-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-lucidatypewriter-medium-r-*-*-12-*-*-*-*-*-*-*=charset
Ddd*XmText.FontList:	  \
-*-lucidatypewriter-medium-r-*-*-12-*-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-lucidatypewriter-medium-r-*-*-12-*-*-*-*-*-*-*=charset

! The resolution-independent alternative.
! Ddd*XmTextField.FontList: \
! -*-lucidatypewriter-medium-r-*-*-*-90-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
! -*-lucidatypewriter-medium-r-*-*-*-90-*-*-*-*-*-*=charset
! Ddd*XmText.FontList:	  \
! -*-lucidatypewriter-medium-r-*-*-*-90-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
! -*-lucidatypewriter-medium-r-*-*-*-90-*-*-*-*-*-*=charset


! Command tool fonts.  Slightly smaller.
Ddd*tool_buttons.run.fontList: \
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=charset
Ddd*tool_buttons.break.fontList:  \
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*=charset
Ddd*tool_buttons*fontList:  \
-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*=charset

! The resolution-independent alternative.
! Ddd*tool_buttons.run.fontList: \
-*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=charset
! Ddd*tool_buttons.break.fontList: \
-*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-helvetica-bold-r-*-*-*-90-*-*-*-*-*-*=charset
! Ddd*tool_buttons*fontList: \
-*-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*=FONTLIST_DEFAULT_TAG_STRING,\
-*-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*=charset

! For fonts in the execution window, see the `termCommand' resource, above.
! For fonts in the data display, see the `vslDefs' resource, below.


!-----------------------------------------------------------------------------
! Colors
!-----------------------------------------------------------------------------

! Rationale for color usage:
!
! * GREY is the default DDD background color, because this works well with
!   all possible color schemes.  It also gives DDD a professional look.
!
! * RED (actually, red4) is used for all objects associated with
!   stopping the debugged program (red traffic light).  This applies
!   to breakpoints (when activated) as well as everything associated
!   with signal handling (interrupt button, signal arrow).
!
! * GREEN (actually, DarkGreen) is used for all objects associated
!   with running the debugged program (green traffic light).  This applies
!   to the current execution position as well as to the run button, but
!   also to the `GDB busy' indicator (meaning that the program is running.)
!
! * BLUE (actually, blue4) is used for objects associated with references
!   (blue is the default color for links in popular WWW browswers).  This
!   applies to pointers and edges in the data display.

Ddd*foreground:			   black
Ddd*background:			   grey
Ddd*XmText.background:		   grey90
Ddd*XmTextField.background:	   grey90
Ddd*GraphEdit.background:	   grey90
Ddd*XmList.background:		   grey90
Ddd*graph_edit_panner.background:  grey
Ddd*graph_edit_panner.foreground:  grey
Ddd*graph_edit_panner.shadowColor: black

! Red, green and blue buttons.
Ddd*break.foreground:		   red4
Ddd*quit.foreground:		   red4
Ddd*run.foreground:		   DarkGreen
Ddd*run_again.foreground:	   DarkGreen
! Ddd*dereference.foreground:	   blue4

! Highlight special dialogs
Ddd*fatal_dialog*foreground:	   red4
Ddd*terminated_dialog*foreground:  red4
Ddd*exited_dialog*foreground:      red4
Ddd*no_debugger_dialog*foreground: red4

! Glyph colors
! Note: In Motif 1.2 and later, colors of dragged glyphs 
! (temp_*) are taken from the drag source.
Ddd*source_form_w.XmPushButton.background: grey90
Ddd*source_form_w.plain_arrow.foreground:  DarkGreen
Ddd*source_form_w.grey_arrow.foreground:   DarkGreen
Ddd*source_form_w.temp_arrow.foreground:   DarkGreen
Ddd*source_form_w.signal_arrow.foreground: red4
Ddd*source_form_w.plain_stop.foreground:   red4
Ddd*source_form_w.plain_cond.foreground:   red4
Ddd*source_form_w.temp_stop.foreground:    red4
Ddd*source_form_w.grey_stop.foreground:	   grey50
Ddd*source_form_w.grey_cond.foreground:	   grey50

Ddd*code_form_w.XmPushButton.background:   grey90
Ddd*code_form_w.plain_arrow.foreground:	   DarkGreen
Ddd*code_form_w.grey_arrow.foreground:	   DarkGreen
Ddd*code_form_w.temp_arrow.foreground:     DarkGreen
Ddd*code_form_w.signal_arrow.foreground:   red4
Ddd*code_form_w.plain_stop.foreground:	   red4
Ddd*code_form_w.plain_cond.foreground:	   red4
Ddd*code_form_w.temp_stop.foreground:      red4
Ddd*code_form_w.grey_stop.foreground:	   grey50
Ddd*code_form_w.grey_cond.foreground:	   grey50

! Graph colors
Ddd*graph_edit.nodeColor:	   black
Ddd*graph_edit.edgeColor:	   blue4
Ddd*graph_edit.selectColor:	   black
Ddd*graph_edit.gridColor:	   black
Ddd*graph_edit.frameColor:	   grey50
Ddd*graph_edit.outlineColor:	   grey50

! For colors of nodes in the data display, see below.


!-----------------------------------------------------------------------------
! Bitmaps
!-----------------------------------------------------------------------------

! These bitmap names are predefined by DDD.  Feel free to use others.
! Note: if `cacheGlyphImages' is set, these bitmap names are ignored.
Ddd*plain_arrow.labelPixmap:	plain_arrow
Ddd*grey_arrow.labelPixmap:	grey_arrow
Ddd*temp_arrow.labelPixmap:	temp_arrow
Ddd*signal_arrow.labelPixmap:	signal_arrow
Ddd*plain_stop.labelPixmap:	plain_stop
Ddd*plain_cond.labelPixmap:	plain_cond
Ddd*temp_stop.labelPixmap:	temp_stop
Ddd*grey_stop.labelPixmap:	grey_stop
Ddd*grey_cond.labelPixmap:	grey_cond


!-----------------------------------------------------------------------------
! Data display fonts and colors
!-----------------------------------------------------------------------------

! The value of the `vslDefs' resource are appended to the builtin VSL library.
! The general pattern to replace a function definition FUNCTION with a new
! definition NEW_DEF is:
!
! #replace FUNCTION
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
! #replace display_color   \n\
! #replace title_color	   \n\
! #replace disabled_color  \n\
! #replace simple_color	   \n\
! #replace pointer_color   \n\
! #replace struct_color	   \n\
! #replace array_color	   \n\
! #replace reference_color \n\
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
! #replace stdfontfamily\n\
! #replace stdfontsize	\n\
! #replace stdfontpoints\n\
! #replace stdfontweight\n\
! stdfontfamily() = family_typewriter();\n\
! stdfontsize()   = 12;	\n\
! stdfontpoints() = 0;	\n\
! stdfontweight() = weight_medium();

! See the file `ddd.vsl' for further definitions to override here.


!-----------------------------------------------------------------------------
! Keyboard translations
!-----------------------------------------------------------------------------

Ddd*defaultVirtualBindings: \
osfBackSpace:	<Key>BackSpace\n\
osfCancel:	<Key>Escape\n\
osfRight: 	<Key>Right\n\
osfLeft:	<Key>Left\n\
osfUp:		<Key>Up\n\
osfHelp:	<Key>F1\n\
osfDown:	<Key>Down\n\
osfPageUp:	<Key>Prior\n\
osfPageDown:	<Key>Next\n\
osfBeginLine:	<Key>Home\n\
osfEndLine:	<Key>End\n


! The GDB window uses an EMACS-like editing style.
! `R7' and `R13' are generated by the developer's SUNs for `Home' and `End'
Ddd*gdb_w.translations: #override\n\
<Btn3Down>:		gdb-popup-menu()            \n\
Ctrl ~Shift<Key>A:	gdb-beginning-of-line()	    \n\
Ctrl<Key>B:		gdb-backward-character()    \n\
Ctrl<Key>C:		gdb-control(^C)		    \n\
Ctrl<Key>D:		gdb-delete-or-control(^D)   \n\
Ctrl ~Shift<Key>E:	gdb-end-of-line()	    \n\
Ctrl<Key>F:		gdb-forward-character()	    \n\
Ctrl<Key>G:		gdb-control(^C)		    \n\
Ctrl<Key>H:		delete-previous-character() \n\
Ctrl<Key>K:		delete-to-end-of-line()	    \n\
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
Ctrl<Key>S:		gdb-isearch-next()          \n\
Ctrl<Key>T:		gdb-complete-command()	    \n\
None<Key>Escape:	process-cancel() gdb-isearch-exit() \n\
None<Key>osfCancel:	process-cancel() gdb-isearch-exit() \n\
None<Key>osfUp:		gdb-previous-history()	    \n\
None<Key>osfDown:	gdb-next-history()	    \n\
None<Key>osfLeft:	gdb-backward-character()    \n\
None<Key>osfRight:	gdb-forward-character()	    \n\
None<Key>osfBeginLine:	gdb-beginning-of-line()	    \n\
None<Key>osfEndLine:	gdb-end-of-line()	    \n\
None<Key>osfBeginData:	gdb-beginning-of-line()	    \n\
None<Key>osfEndData:	gdb-end-of-line()	    \n\
None<Key>Home:		gdb-beginning-of-line()	    \n\
None<Key>End:		gdb-end-of-line()	    \n\
~Shift<Key>osfPageUp:	previous-page()		    \n\
~Shift<Key>osfPageDown:	next-page()	            \n\
~Shift<Key>Prior:	previous-page()		    \n\
~Shift<Key>Next:	next-page()	            \n\
Shift<Key>osfPageUp:	previous-page(extend)	    \n\
Shift<Key>osfPageDown:	next-page(extend)           \n\
Shift<Key>Prior:	previous-page(extend)	    \n\
Shift<Key>Next:		next-page(extend)	    \n\
None<Key>R7:		gdb-beginning-of-line()	    \n\
None<Key>R13:		gdb-end-of-line()	    \n\
None<Key>Tab:		gdb-complete-command()	    \n

dnl Basic translations for all other texts and text fields
define(EMACS_TRANSLATIONS,[#override\n\
Ctrl ~Shift<Key>A:	beginning-of-line()	    \n\
Ctrl<Key>B:		backward-character()	    \n\
Ctrl<Key>D:		delete-next-character()	    \n\
Ctrl ~Shift<Key>E:	end-of-line()		    \n\
Ctrl<Key>F:		forward-character()	    \n\
Ctrl<Key>H:		delete-previous-character() \n\
Ctrl<Key>I:		self-insert()               \n\
Ctrl<Key>K:		delete-to-end-of-line()	    \n\
Ctrl<Key>U: 		beginning-of-line()	    \
			delete-to-end-of-line()	    \n\
Ctrl<Key>W:		delete-previous-word()	    \n\
Ctrl<Key>osfBackSpace:	delete-previous-word()	    \n\
None<Key>R7:		beginning-of-line()	    \n\
None<Key>R13:		end-of-line()		    \n\
None<Key>Home:		beginning-of-line()	    \n\
None<Key>End:		end-of-line()		    \n])dnl
dnl
dnl Emacs + TAB completion.
define(COMPLETE_TRANSLATIONS, EMACS_TRANSLATIONS[\
Ctrl<Key>T:		gdb-complete-arg($1)	    \n\
None<Key>Tab:		gdb-complete-tab($1)	    \n])dnl
dnl
dnl Emacs + extra pageup/pagedown and newline capabilities
define(TEXT_TRANSLATIONS, EMACS_TRANSLATIONS[\
Ctrl<Key>J:		newline()                   \n\
Ctrl<Key>M:		newline()                   \n\
Ctrl<Key>N:		process-down()		    \n\
Ctrl<Key>P:		process-up()                \n\
Meta<Key>V:		previous-page()		    \n\
Ctrl<Key>V:		next-page()		    \n\
~Shift<Key>Prior:	previous-page()		    \n\
~Shift<Key>Next:	next-page()	            \n\
Shift<Key>osfPageUp:	previous-page(extend)	    \n\
Shift<Key>osfPageDown:	next-page(extend)           \n\
Shift<Key>Prior:	previous-page(extend)	    \n\
Shift<Key>Next:		next-page(extend)	    \n])dnl
dnl
dnl Emacs + pageup/pagedown + TAB completion.
define(COMPLETE_TEXT_TRANSLATIONS, TEXT_TRANSLATIONS[\
Ctrl<Key>T:		gdb-complete-arg($1)	    \n\
None<Key>Tab:		gdb-complete-tab($1)	    \n])dnl
dnl

! Have some of these in argument fields as well
Ddd*XmTextField.translations: COMPLETE_TRANSLATIONS(print)

Ddd*XmText.translations:      COMPLETE_TEXT_TRANSLATIONS(print)

! In breakpoint dialogs, use a `break' completion
Ddd*new_breakpoint_dialog*XmText.translations:   COMPLETE_TRANSLATIONS(break)

Ddd*new_breakpoint_dialog*XmTextField.translations: \
COMPLETE_TRANSLATIONS(break)

! In watchpoint dialogs, use a `print' completion
Ddd*new_watchpoint_dialog*XmText.translations:   COMPLETE_TRANSLATIONS(print)

Ddd*new_watchpoint_dialog*XmTextField.translations: \
COMPLETE_TRANSLATIONS(print)


! In file dialogs, use a `file' completion
Ddd*XmFileSelectionBox*XmText.translations:      COMPLETE_TRANSLATIONS(file)

Ddd*XmFileSelectionBox*XmTextField.translations: COMPLETE_TRANSLATIONS(file)


! In `print', `make', and `cd' dialogs, use a `file' completion, too.
Ddd*print_popup*XmText.translations: 		 COMPLETE_TRANSLATIONS(file)

Ddd*print_popup*XmTextField.translations:        COMPLETE_TRANSLATIONS(file)

Ddd*make_dialog*XmText.translations: 		 COMPLETE_TRANSLATIONS(file)

Ddd*make_dialog*XmTextField.translations:        COMPLETE_TRANSLATIONS(file)

Ddd*cd_dialog*XmText.translations: 		 COMPLETE_TRANSLATIONS(file)

Ddd*cd_dialog*XmTextField.translations:        COMPLETE_TRANSLATIONS(file)

! In selection boxes, use `shell' completion.
Ddd*XmSelectionBox*XmText.translations: 	 COMPLETE_TRANSLATIONS(shell)

Ddd*XmSelectionBox*XmTextField.translations:     COMPLETE_TRANSLATIONS(shell)

! In the button editor, we have no completion.
Ddd*edit_buttons*XmText.translations: 	 	 TEXT_TRANSLATIONS




! The source window is especially tuned for word selection.
define(SOURCE_TRANSLATIONS,[#override\n\
<Btn3Down>:		source-popup-menu() \n\
~Ctrl ~Shift<Btn1Down>:	source-start-select-word()  \n\
~Ctrl ~Shift<Btn1Up>:	source-end-select-word()    \n\
Ctrl ~Shift<Key>A:	beginning-of-line()	    \n\
Ctrl<Key>B:		backward-character()	    \n\
Ctrl ~Shift<Key>E:	end-of-line()		    \n\
Ctrl<Key>F:		forward-character()	    \n\
Ctrl<Key>N:		process-down()		    \n\
Ctrl<Key>P:		process-up()                \n\
Ctrl<Key>V:		next-page()		    \
			source-update-glyphs()	    \n\
Meta<Key>V:		previous-page()		    \
			source-update-glyphs()	    \n\
None<Key>R7:		beginning-of-line()	    \n\
None<Key>R13:		end-of-line()		    \n\
None<Key>Tab:		PrimitiveNextTabGroup()	    \n\
None<Key>osfPageUp:	previous-page()		    \
			source-update-glyphs()	    \n\
None<Key>osfPageDown:	next-page()		    \
			source-update-glyphs()	    \n\
Shift<Key>osfPageUp:	previous-page(extend)	    \
			source-update-glyphs()	    \n\
Shift<Key>osfPageDown:	next-page(extend)	    \
			source-update-glyphs()	    \n\
~Shift<Key>Prior:	previous-page()		    \
			source-update-glyphs()	    \n\
~Shift<Key>Next:	next-page()		    \
			source-update-glyphs()	    \n\
Shift<Key>Prior:	previous-page(extend)	    \
			source-update-glyphs()	    \n\
Shift<Key>Next:		next-page(extend)	    \
			source-update-glyphs()	    \n\
~Shift<Key>Home:	beginning-of-line()	    \n\
~Shift<Key>End:		end-of-line()		    \n])dnl

! Have all characters be processed by the GDB console.
! If you know of a better way than by using such huge translation tables,
! please let me know.
define(GDB_TRANSLATIONS,[\
~Meta<Key>osfBackSpace:		     gdb-process()\n\
~Meta<Key>osfCancel:		     gdb-process()\n\
~Ctrl ~Meta<Key>BackSpace:	     gdb-process()\n\
~Meta<Key>Tab:			     gdb-process()\n\
~Ctrl ~Meta<Key>Return:		     gdb-process()\n\
~Ctrl ~Meta<Key>Escape:		     gdb-process()\n\
~Meta<Key>Delete:		     gdb-process()\n\
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
~Ctrl ~Meta<Key>KP_0:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_1:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_2:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_3:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_4:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_5:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_6:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_7:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_8:		     gdb-process()\n\
~Ctrl ~Meta<Key>KP_9:		     gdb-process()\n\
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
Ctrl ~Meta<Key>C:		     gdb-process()\n\
Ctrl ~Meta<Key>D:		     gdb-process()\n\
Ctrl ~Meta<Key>U:		     gdb-process()])dnl

Ddd*source_text_w.translations: SOURCE_TRANSLATIONS GDB_TRANSLATIONS
Ddd*code_text_w.translations:   SOURCE_TRANSLATIONS GDB_TRANSLATIONS

! Realize glyph drag and drop as well as glyph menu popups
define(GLYPH_TRANSLATIONS,[#override\n\
<Btn1Down>:	  ArmAndActivate() source-drag-glyph()	\n\
<Btn1Motion>: 	  source-follow-glyph()                 \n\
<Btn1Up>:	  source-drop-glyph()                   \n\
<Btn2Down>:	  ArmAndActivate() source-drag-glyph()	\n\
<Btn2Motion>: 	  source-follow-glyph()			\n\
<Btn2Up>:	  source-drop-glyph()			\n\
<Btn3Down>:	  source-popup-menu()			\n])dnl

dnl This uses too much space:
dnl Ddd*source_form_w.XmPushButton.translations: \
dnl GLYPH_TRANSLATIONS GDB_TRANSLATIONS
dnl Ddd*code_form_w.XmPushButton.translations:   \
dnl GLYPH_TRANSLATIONS GDB_TRANSLATIONS
dnl
dnl Use this instead:
Ddd*source_form_w.XmPushButton.translations: GLYPH_TRANSLATIONS
Ddd*code_form_w.XmPushButton.translations:   GLYPH_TRANSLATIONS

! This defines the popup menus in the graph window.
define(GRAPH_TRANSLATIONS,[#override\n\
~Shift<Btn1Down>: graph-select-or-move()\n\
Shift<Btn1Down>:  graph-toggle-or-move()\n\
<Btn2Down>:	  graph-toggle-or-move()\n\
<Btn3Down>:	  graph-select() graph-popup-menu()\n])dnl

Ddd*graph_edit.translations: GRAPH_TRANSLATIONS GDB_TRANSLATIONS

dnl The following would forward all keyboard events to the debugger console,
dnl but it does not work for Motif 1.1
dnl Ddd*translations: \
dnl #augment\n
dnl GDB_TRANSLATIONS
dnl 
dnl An alternative would be to define translations for all labels 
dnl and several buttons.  The following might work:
dnl define(BUTTON_TRANSLATIONS,[#augment\n\
dnl ~Ctrl<Btn1Down>:	Arm()\n\
dnl ~Ctrl<Btn1Up>:        Activate() Disarm()\n])dnl
dnl
dnl Ddd*tool_shell*XmPushButton.translations: \
dnl BUTTON_TRANSLATIONS GDB_TRANSLATIONS
dnl Ddd*graph_cmd_area*XmPushButton.translations: \
dnl BUTTON_TRANSLATIONS GDB_TRANSLATIONS
dnl Ddd*arg_cmd_area*XmPushButton.translations: \
dnl BUTTON_TRANSLATIONS GDB_TRANSLATIONS
dnl
dnl Ddd*XmLabel.translations: \
dnl #augment\n\
dnl GDB_TRANSLATIONS


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
Ddd*command_shell.iconName:		DDD: Debugger Console

Ddd*source_view_shell.title:		DDD: Program Source
Ddd*source_view_shell.iconName:		DDD: Program Source

Ddd*data_disp_shell.title:		DDD: Program Data
Ddd*data_disp_shell.iconName:		DDD: Program Data

! Command tool
Ddd*tool_shell.title:			DDD
Ddd*tool_shell.iconName:		DDD: Command Tool



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

! Set this to `true' to view widget names in help texts.
Ddd*helpShowTitle: false

! Help is never acknowledged, always closed.
Ddd*help_popup*okLabelString: 		Close
Ddd*license_popup*okLabelString: 	Close
Ddd*news_popup*okLabelString: 		Close
Ddd*manual_help_popup*okLabelString: 	Close

! Titles
Ddd*help_popup.title:           DDD Help
Ddd*manual_help_popup.title:    DDD Reference
Ddd*manual_help_popup.iconName: DDD Reference
Ddd*license_popup.title:        DDD License
Ddd*license_popup.iconName:     DDD License
Ddd*news_popup.title:           DDD News
Ddd*news_popup.iconName:     	DDD News

! Help on help.
Ddd*help_popup.help*helpString: \
@rm Welcome to DDD Help\!\n\
\n\
To get help about an item, just point on it and press KEY(F1).\n\
The item's functionality will be explained here.\n\
\n\
To find out what you can do next, press KEY(Ctrl+F1).  Depending on\n\
the current DDD state, you will be given some general advice.\n\
\n\
Have fun with DDD!


! The official announcement.
! Build information (like `ddd --configuration') is appended to this text.
Ddd*helpOnVersionString:	\
@Logo DDD@logo: \
The @Logo D@logo ata @Logo D@logo isplay @Logo D@logo ebugger\n\
@rm by Dorothea L\374tkehaus and Andreas Zeller.\n\n

! The default Help Text.
Ddd*helpString:	\
@rm This item has no function, or is not documented.

! Some trival help
Ddd*XmScrollBar.helpString: \
@rm This is a EMPH(Scroll Bar).\n\
\n\
Click or hold BUTTON(1) on an arrow button\n\
to scroll the window in the arrow direction.\n\
\n\
Drag the slider with BUTTON(1) to select the\n\
visible part of the window.
! Ddd*XmScrollBar.tipString: \
! @rm Scroll window

Ddd*XmSash.helpString: \
@rm This is a EMPH(Paned Window Sash).\n\
\n\
Drag the sash with BUTTON(1) to resize window parts.
Ddd*XmSash.tipString: \
@rm Resize window

Ddd*Separator.helpString: \
@rm This is a EMPH(Paned Window Sash).\n\
\n\
Drag the sash with BUTTON(1) to resize window parts.
Ddd*Separator.tipString: \
@rm Resize window

Ddd*arg_label.helpString: \
@rm This is an EMPH(Argument Prompt).\n\
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

Ddd*arg_label.traversalOn:		false
Ddd*status_form*traversalOn:		false
Ddd*XmSash.traversalOn:			false


!-----------------------------------------------------------------------------
! DDD manual
!-----------------------------------------------------------------------------

Ddd*manual_help*form.borderWidth:	0

Ddd*manual_help*title.labelString:	DDD Reference
Ddd*manual_help*title.alignment:	XmALIGNMENT_BEGINNING

Ddd*manual_help*area.borderWidth:	0
Ddd*manual_help*area.marginWidth:	0
Ddd*manual_help*area.marginHeight:	0
Ddd*manual_help*area.allowResize:	true

Ddd*manual_help*index.selectionPolicy:	XmSINGLE_SELECT
Ddd*manual_help*index.visibleItemCount:	5

Ddd*manual_help*indexSW.height:		90

Ddd*manual_help*text.columns:		80
Ddd*manual_help*text.rows:		25
Ddd*manual_help*text.wordWrap:		true
Ddd*manual_help*text.scrollHorizontal:	false
Ddd*manual_help*scrollBarDisplayPolicy: XmAS_NEEDED

Ddd*manual_help*textSW.width:		550
Ddd*manual_help*textSW.height:		342

Ddd*manual_help*search.orientation:	XmHORIZONTAL
Ddd*manual_help*search.borderWidth:	0
Ddd*manual_help*search.spacing:		0
Ddd*manual_help*search.packing:		XmPACK_TIGHT
Ddd*manual_help*search.entryAlignment:	XmALIGNMENT_CENTER
Ddd*manual_help*search*alignment:	XmALIGNMENT_CENTER

Ddd*manual_help*arg_label.labelString:	():

Ddd*manual_help*key.columns:		58
Ddd*manual_help*findBackward.labelString: LBL_FIND_PREV
Ddd*manual_help*findBackward.tipString:	  \
@rm Find previous LBL(()) in manual
Ddd*manual_help*findBackward.documentationString:	\
@rm Search the previous occurrence of LBL(()) in the manual
Ddd*manual_help*findForward.labelString:  LBL_FIND_NEXT
Ddd*manual_help*findForward.tipString:	  \
@rm Find next LBL(()) in manual
Ddd*manual_help*findForward.documentationString:	\
@rm Search the next occurrence of LBL(()) in the manual

Ddd*manual_help*helpString: \
@rm This is the EMPH([DDD] Manual Browser).\n\
\n\
To view a specific section, select its title from the index at the top.\n\
\n\
To find a string in the manual, enter it in the LBL(()) argument field \
and press KEY_RETURN.\n\
Click on LBL_FIND_PREV to search the next occurrence of LBL(()).\n\
Click on LBL_FIND_NEXT to search the previous occurrence of LBL(()).


!-----------------------------------------------------------------------------
! DDD license and news
!-----------------------------------------------------------------------------

Ddd*license*form.borderWidth:		0

Ddd*license*title.labelString:      	DDD License
Ddd*license*title.alignment:	        XmALIGNMENT_BEGINNING

Ddd*license*text.columns:	        80
Ddd*license*text.rows:	      		25
Ddd*license*text.wordWrap:	        true
Ddd*license*text.scrollHorizontal:    	false
Ddd*license*scrollBarDisplayPolicy:   	XmAS_NEEDED

Ddd*license*textSW.width:		600
Ddd*license*textSW.height:		320

Ddd*license*helpString: \
@rm This is the EMPH([DDD] License), a GNU General Public License.\n\
\n\
It describes the terms and conditions for copying, distributing\n\
and modifying DDD.\n\
\n\
Please see section 11, TEXT(NO WARRANTY), for the various kinds of warranty\n\
you do not have.


Ddd*news*form.borderWidth:		0

Ddd*news*title.labelString:      	DDD News
Ddd*news*title.alignment:	        XmALIGNMENT_BEGINNING

Ddd*news*text.columns:	        	80
Ddd*news*text.rows:	      		25
Ddd*news*text.wordWrap:	        	true
Ddd*news*text.scrollHorizontal:    	false
Ddd*news*scrollBarDisplayPolicy:   	XmAS_NEEDED

Ddd*news*textSW.width:			600
Ddd*news*textSW.height:			320

Ddd*news*helpString: \
@rm This is the EMPH([DDD] News).\n\
\n\
It shows what's new in this DDD release relative to previous releases.



!-----------------------------------------------------------------------------
! Toolbar tips
!-----------------------------------------------------------------------------

! These colors are taken from Netscape.  The basic idea is that you might
! have a Netscape already running, so we won't use up another color cell.

Ddd*tipLabel.background:  #ffffcc
Ddd*tipLabel.foreground:  Black
Ddd*tipRow.background:    Black
Ddd*tipShell.borderColor: Black
Ddd*tipShell.borderWidth: 1


!-----------------------------------------------------------------------------
! Menu Bar
!-----------------------------------------------------------------------------

Ddd*menubar.helpString:		\
@rm This is the EMPH(Menu Bar).\n\
\n\
DESC(File, [select files and exit DDD])\n\
DESC(Edit, [cut, copy, and paste text, and change DDD options])\n\
DESC(View, [view DDD windows])\n\
DESC(Program, [run the debugged program])\n\
DESC(Commands, [view command history])\n\
DESC(Stack, [view backtrace and select the current stack frame])\n\
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
@rm This is the EMPH(File Menu).\n\
\n\
DESC(Open Program..., [open the program or class to be debugged])\n\
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
DESC(Make..., [run the CODE(make) program])\n\
\n\
DESC(Close, [close this window])\n\
DESC(Restart, [restart DDD])\n\
DESC(Exit, [exit DDD])
])dnl

Ddd*menubar.file*helpString: FILE_HELP
Ddd*fileMenu*helpString:     FILE_HELP

Ddd*menubar*fileMenu.open_file.labelString: Open Program...
Ddd*menubar*fileMenu.open_file.mnemonic:    O
Ddd*menubar*fileMenu.open_file.accelerator:      ~Shift Meta<Key>O
Ddd*menubar*fileMenu.open_file.acceleratorText:  Alt+O
Ddd*menubar*fileMenu.open_file.documentationString:   \
@rm Open a program to be debugged

Ddd*menubar*fileMenu.open_class.labelString: Open Class...
Ddd*menubar*fileMenu.open_class.mnemonic:    O
Ddd*menubar*fileMenu.open_class.accelerator:      ~Shift Meta<Key>O
Ddd*menubar*fileMenu.open_class.acceleratorText:  Alt+O
Ddd*menubar*fileMenu.open_class.documentationString:   \
@rm Open a class to be debugged

Ddd*menubar*fileMenu.open_core.labelString: Open Core Dump...
Ddd*menubar*fileMenu.open_core.mnemonic:    D
Ddd*menubar*fileMenu.open_core.documentationString:   \
@rm Open a file as core dump for examining memory and registers

Ddd*menubar*fileMenu.open_source.labelString: Open Source...
Ddd*menubar*fileMenu.open_source.mnemonic:    S
Ddd*menubar*fileMenu.open_source.documentationString:   \
@rm Open a source file in the source window

Ddd*menubar*fileMenu.open_session.labelString:	        Open Session...
Ddd*menubar*fileMenu.open_session.mnemonic:	        e
! Ddd*menubar*fileMenu.open_session.accelerator:          ~Shift Meta<Key>O
! Ddd*menubar*fileMenu.open_session.acceleratorText:      Alt+Shift+O
Ddd*menubar*fileMenu.open_session.documentationString:  \
@rm Restore a previously saved DDD session

Ddd*menubar*fileMenu.save_session.labelString:	        Save Session As...
Ddd*menubar*fileMenu.save_session.mnemonic:	        v
Ddd*menubar*fileMenu.save_session.documentationString:  \
@rm Save this DDD session for resuming later

Ddd*menubar*fileMenu.attach.labelString: Attach to Process...
Ddd*menubar*fileMenu.attach.mnemonic:    A
Ddd*menubar*fileMenu.attach.accelerator:      ~Shift Ctrl<Key>Z
Ddd*menubar*fileMenu.attach.acceleratorText:  Ctrl+Z
Ddd*menubar*fileMenu.attach.documentationString:   \
@rm Attach to a process outside of DDD

Ddd*menubar*fileMenu.detach.labelString:       Detach Process
Ddd*menubar*fileMenu.detach.mnemonic:	       t
Ddd*menubar*fileMenu.detach.accelerator:       Shift Ctrl<Key>Z
Ddd*menubar*fileMenu.detach.acceleratorText:   Ctrl+Shift+Z
Ddd*menubar*fileMenu.detach.documentationString:

Ddd*menubar*fileMenu.print.labelString:	 	Print Graph...
Ddd*menubar*fileMenu.print.mnemonic:	 	P
! Ddd*menubar*fileMenu.print.accelerator:       ~Shift Meta<Key>P
! Ddd*menubar*fileMenu.print.acceleratorText:   Alt+P
Ddd*menubar*fileMenu.print.documentationString:   \
@rm Print the contents of the data display

Ddd*menubar*fileMenu.printAgain.labelString: 	    Print Again
Ddd*menubar*fileMenu.printAgain.mnemonic: 	    g
! Ddd*menubar*fileMenu.printAgain.accelerator:      Shift Meta<Key>P
! Ddd*menubar*fileMenu.printAgain.acceleratorText:  Alt+Shift+P
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
@rm Run the ``make'' program

Ddd*menubar*fileMenu.makeAgain.labelString:	 Make Again
Ddd*menubar*fileMenu.makeAgain.mnemonic:	 i
Ddd*menubar*fileMenu.makeAgain.accelerator:      Shift Ctrl<Key>M
Ddd*menubar*fileMenu.makeAgain.acceleratorText:  Ctrl+Shift+M
Ddd*menubar*fileMenu.makeAgain.documentationString:  \
@rm Run the ``make'' program, using previous settings

Ddd*menubar*fileMenu.close.labelString:	 Close
Ddd*menubar*fileMenu.close.mnemonic:	 C
! Ddd*menubar*fileMenu.close.accelerator:      Meta<Key>W
! Ddd*menubar*fileMenu.close.acceleratorText:  Alt+W
Ddd*menubar*fileMenu.close.documentationString:   \
@rm Close this window

Ddd*menubar*fileMenu.restart.labelString: Restart
Ddd*menubar*fileMenu.restart.mnemonic:	 R
Ddd*menubar*fileMenu.restart.accelerator:      Shift Ctrl<Key>Q
Ddd*menubar*fileMenu.restart.acceleratorText:  Ctrl+Shift+Q
Ddd*menubar*fileMenu.restart.documentationString:   \
@rm Close all windows and restart DDD from scratch

Ddd*menubar*fileMenu.exit.labelString:	 Exit
Ddd*menubar*fileMenu.exit.mnemonic:	 x
Ddd*menubar*fileMenu.exit.accelerator:	    ~Shift Ctrl<Key>Q
Ddd*menubar*fileMenu.exit.acceleratorText:  Ctrl+Q
Ddd*menubar*fileMenu.exit.documentationString:   \
@rm Close all windows and exit DDD


! Edit menu
Ddd*menubar.edit.labelString:		Edit
Ddd*menubar.edit.mnemonic:		E
Ddd*menubar.edit*documentationString:	\
@rm Cut, copy, paste from and to the clipboard

define(EDIT_HELP, [\
@rm This is the EMPH(Edit Menu).\n\
\n\
DESC(Cut, [remove the selected text to the clipboard])\n\
DESC(Copy, [copy the selected text to the clipboard\n\
    without removing the original])\n\
DESC(Paste, [paste the clipboard contents into the text])\n\
DESC(Clear, [clear argument fields and @GDB@ command line])\n\
DESC(Delete, [delete the selected text])\n\
\n\
DESC(Preferences..., [invokes a panel for setting DDD options])\n\
DESC(Settings..., [invokes a panel for setting @GDB@ options])\n\
\n\
DESC(Save Options, [saves options, preferences, and @GDB@ settings\n\
    for the next DDD invocation.])
])dnl

Ddd*menubar.edit*helpString: EDIT_HELP
Ddd*editMenu*helpString:     EDIT_HELP

Ddd*editMenu.cut.labelString:				Cut
Ddd*editMenu.cut.mnemonic:				t
Ddd*editMenu.cut.accelerator:				Shift<Key>Delete
! *editMenu.cut.accelerator:				<Key>osfCut
Ddd*editMenu.cut.acceleratorText:			Shift+Del
Ddd*editMenu.cut.documentationString: \
@rm Cut the selection and put it on the clipboard

Ddd*editMenu.copy.labelString:				Copy
Ddd*editMenu.copy.mnemonic:				C
Ddd*editMenu.copy.accelerator:				Ctrl<Key>Insert
! *editMenu.copy.accelerator:				<Key>osfPrimaryCopy
Ddd*editMenu.copy.acceleratorText:			Ctrl+Ins
Ddd*editMenu.copy.documentationString: \
@rm Copy the selection and put it on the clipboard

Ddd*editMenu.paste.labelString:				Paste
Ddd*editMenu.paste.mnemonic:				P
Ddd*editMenu.paste.accelerator:				Shift<Key>Insert
! *editMenu.paste.accelerator:				<Key>osfPaste
Ddd*editMenu.paste.acceleratorText:			Shift+Ins
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
Ddd*editMenu.selectAll.documentationString: \
@rm Select all items

Ddd*editMenu.unselectAll.labelString:			Unselect All
Ddd*editMenu.unselectAll.mnemonic:			U
Ddd*editMenu.unselectAll.documentationString: \
@rm Unselect all items

Ddd*editMenu.preferences.labelString:			Preferences...
Ddd*editMenu.preferences.mnemonic:			r
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
@rm Save preferences and settings for the next DDD invocation


! View menu (for single-window DDD)
Ddd*menubar.views.labelString:		View
Ddd*menubar.views.mnemonic:		V
Ddd*menubar.view*documentationString:	\
@rm Open and close DDD windows

define(VIEWS_HELP, [\
@rm This is the EMPH(View Menu).\n\
\n\
DESC(Command Tool..., [open and recenter the command tool])\n\
DESC(Execution Window..., [open the execution window])\n\
\n\
DESC(Data Window, [toggle display of program data])\n\
DESC(Source Window, [toggle display of program source])\n\
DESC(@GDB@ Console, [toggle display of debugger console])
])dnl

Ddd*menubar.views.helpString:	VIEWS_HELP
Ddd*viewsMenu*helpString:	VIEWS_HELP

Ddd*viewsMenu.tool.labelString:			Command Tool...
Ddd*viewsMenu.tool.mnemonic:			T
! Ddd*viewsMenu.tool.acceleratorText:		Alt+Shift+T
! Ddd*viewsMenu.tool.accelerator:		Shift Meta<Key>T
Ddd*viewsMenu.tool.documentationString: \
@rm Open the command tool and move it to its original position

Ddd*viewsMenu.exec.labelString:			Execution Window...
Ddd*viewsMenu.exec.mnemonic:			x
! Ddd*viewsMenu.exec.acceleratorText:		Alt+Shift+X
! Ddd*viewsMenu.exec.accelerator:		Shift Meta<Key>X
Ddd*viewsMenu.exec.documentationString: \
@rm Open the program execution window

Ddd*viewsMenu.data.labelString:			Data Window
Ddd*viewsMenu.data.mnemonic:			D
! Ddd*viewsMenu.data.acceleratorText:		Alt+Shift+D
! Ddd*viewsMenu.data.accelerator:		Shift Meta<Key>D
Ddd*viewsMenu.data.documentationString: \
@rm Toggle showing the data window

Ddd*viewsMenu.source.labelString:		Source Window
Ddd*viewsMenu.source.mnemonic:			S
! Ddd*viewsMenu.source.acceleratorText:		Alt+Shift+S
! Ddd*viewsMenu.source.accelerator:		Shift Meta<Key>S
Ddd*viewsMenu.source.documentationString: \
@rm Toggle showing the source window

Ddd*viewsMenu.console.labelString:		@GDB@ Console
Ddd*viewsMenu.console.mnemonic:			C
! Ddd*viewsMenu.console.acceleratorText:	Alt+Shift+C
! Ddd*viewsMenu.console.accelerator:		Shift Meta<Key>C
Ddd*viewsMenu.console.documentationString: \
@rm Toggle showing the @GDB@ console


! View menu (for multi-window DDD)
Ddd*menubar.view.labelString:		View
Ddd*menubar.view.mnemonic:		V
Ddd*menubar.view*documentationString:	\
@rm Open DDD windows

define(VIEW_HELP, [\
\
@rm This is the EMPH(View Menu).\n\
\n\
DESC(Command Tool..., [open and recenter the command tool])\n\
DESC(Execution Window..., [open the execution window])\n\
\n\
DESC(Data Window..., [open the program data window])\n\
DESC(Source Window..., [open the program source window])\n\
DESC(@GDB@ Console..., [open the debugger console window])
])dnl

Ddd*menubar.view.helpString:	VIEW_HELP
Ddd*viewMenu*helpString:	VIEW_HELP

Ddd*viewMenu.tool.labelString:			Command Tool...
Ddd*viewMenu.tool.mnemonic:			T
! Ddd*viewMenu.tool.acceleratorText:		Alt+Shift+T
! Ddd*viewMenu.tool.accelerator:		Shift Meta<Key>T
Ddd*viewMenu.tool.documentationString: \
@rm Open the command tool and move it to its original position

Ddd*viewMenu.exec.labelString:			Execution Window...
Ddd*viewMenu.exec.mnemonic:			x
! Ddd*viewMenu.exec.acceleratorText:		Alt+Shift+X
! Ddd*viewMenu.exec.accelerator:		Shift Meta<Key>X
Ddd*viewMenu.exec.documentationString: \
@rm Open the program execution window

Ddd*viewMenu.data.labelString:			Data Window...
Ddd*viewMenu.data.mnemonic:			D
! Ddd*viewMenu.data.acceleratorText:		Alt+Shift+D
! Ddd*viewMenu.data.accelerator:		Shift Meta<Key>D
Ddd*viewMenu.data.documentationString: \
@rm Open the data window

Ddd*viewMenu.source.labelString:		Source Window...
Ddd*viewMenu.source.mnemonic:			S
! Ddd*viewMenu.source.acceleratorText:		Alt+Shift+S
! Ddd*viewMenu.source.accelerator:		Shift Meta<Key>S
Ddd*viewMenu.source.documentationString: \
@rm Open the source window

Ddd*viewMenu.console.labelString:		@GDB@ Console...
Ddd*viewMenu.console.mnemonic:			C
! Ddd*viewMenu.console.acceleratorText:		Alt+Shift+C
! Ddd*viewMenu.console.accelerator:		Shift Meta<Key>C
Ddd*viewMenu.console.documentationString: \
@rm Open the @GDB@ console


! Program menu
Ddd*menubar.program.labelString:		Program
Ddd*menubar.program.mnemonic:			P
Ddd*menubar.program*documentationString:	\
@rm Control the execution of the debugged program

define(PROGRAM_HELP, [\
\
@rm This is the EMPH(Program Menu).\n\
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
\n\
DESC(Continue, [continue program after signal or breakpoint])\n\
DESC(Finish, [execute until function returns])\n\
\n\
DESC(Kill, [kill execution of program being debugged])\n\
DESC(Interrupt, [interrupt program (or current @GDB@ command)])\n\
DESC(Abort, [abort program (or current @GDB@ command)])
])dnl

Ddd*menubar.program*helpString:	PROGRAM_HELP
Ddd*programMenu*helpString:	PROGRAM_HELP

Ddd*menubar*programMenu.run.labelString:	   Run...
Ddd*menubar*programMenu.run.mnemonic:		   R
Ddd*menubar*programMenu.run.accelerator:	   Ctrl ~Shift<Key>X
Ddd*menubar*programMenu.run.acceleratorText:	   Ctrl+X
Ddd*menubar*programMenu.run.documentationString: \
@rm Start debugged program.  You may specify arguments to give it.

Ddd*menubar*programMenu.run_again.labelString:	   Run Again
Ddd*menubar*programMenu.run_again.mnemonic:	   g
Ddd*menubar*programMenu.run_again.accelerator:	   Ctrl Shift<Key>X
Ddd*menubar*programMenu.run_again.acceleratorText: Ctrl+Shift+X
Ddd*menubar*programMenu.run_again.documentationString: \
@rm Restart debugged program, using the previous arguments.

Ddd*menubar*programMenu.separateExecWindow.labelString:	Run in Execution Window
Ddd*menubar*programMenu.separateExecWindow.mnemonic:	x
Ddd*menubar*programMenu*separateExecWindow.accelerator:	~Shift Meta<Key>X
Ddd*menubar*programMenu*separateExecWindow.acceleratorText: Alt+X
Ddd*menubar*programMenu.separateExecWindow.documentationString:\
@rm Toggle execution of debugged program in separate window

Ddd*menubar*programMenu.step.labelString:	   Step
Ddd*menubar*programMenu.step.mnemonic:		   S
Ddd*menubar*programMenu.step.accelerator:	   Ctrl ~Shift<Key>I
Ddd*menubar*programMenu.step.acceleratorText:	   Ctrl+I
Ddd*menubar*programMenu.step.documentationString:

Ddd*menubar*programMenu.stepi.labelString:	   Step Instruction
Ddd*menubar*programMenu.stepi.mnemonic:		   I
Ddd*menubar*programMenu.stepi.accelerator:	   Ctrl Shift<Key>I
Ddd*menubar*programMenu.stepi.acceleratorText:	   Ctrl+Shift+I
Ddd*menubar*programMenu.stepi.documentationString:

Ddd*menubar*programMenu.next.labelString:	   Next
Ddd*menubar*programMenu.next.mnemonic:		   N
Ddd*menubar*programMenu.next.accelerator:	   Ctrl ~Shift<Key>J
Ddd*menubar*programMenu.next.acceleratorText:	   Ctrl+J
Ddd*menubar*programMenu.next.documentationString:

Ddd*menubar*programMenu.nexti.labelString:	   Next Instruction
Ddd*menubar*programMenu.nexti.mnemonic:		   e
Ddd*menubar*programMenu.nexti.accelerator:	   Ctrl Shift<Key>J
Ddd*menubar*programMenu.nexti.acceleratorText:	   Ctrl+Shift+J
Ddd*menubar*programMenu.nexti.documentationString:

Ddd*menubar*programMenu.cont.labelString:	   Continue
Ddd*menubar*programMenu.cont.mnemonic:		   C
Ddd*menubar*programMenu.cont.accelerator:	   Ctrl<Key>O
Ddd*menubar*programMenu.cont.acceleratorText:	   Ctrl+O
Ddd*menubar*programMenu.cont.documentationString:

Ddd*menubar*programMenu.finish.labelString:	   Finish
Ddd*menubar*programMenu.finish.mnemonic:	   F
Ddd*menubar*programMenu.finish.accelerator:	   Ctrl<Key>Return
Ddd*menubar*programMenu.finish.acceleratorText:	   Ctrl+Return
Ddd*menubar*programMenu.finish.documentationString:

Ddd*menubar*programMenu.kill.labelString:	   Kill
Ddd*menubar*programMenu.kill.mnemonic:		   K
Ddd*menubar*programMenu.kill.accelerator:	   Ctrl Shift<Key>K
Ddd*menubar*programMenu.kill.acceleratorText:	   Ctrl+Shift+K
Ddd*menubar*programMenu.kill.documentationString:

Ddd*menubar*programMenu.break.labelString:	   Interrupt
Ddd*menubar*programMenu.break.mnemonic:		   I
Ddd*menubar*programMenu.break.accelerator:	   Ctrl<Key>C
Ddd*menubar*programMenu.break.acceleratorText:	   Ctrl+C
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
@rm This is the EMPH(Commands Menu).\n\
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
DESC(Edit Buttons..., [edit command buttons])
])dnl

Ddd*menubar.commands*helpString:	COMMANDS_HELP
Ddd*commandsMenu*helpString:		COMMANDS_HELP

Ddd*commandsMenu.history.labelString:	        Command History...
Ddd*commandsMenu.history.mnemonic:	        H
Ddd*commandsMenu.history.documentationString: \
@rm Show previous commands

Ddd*commandsMenu.prev.labelString:	        Previous
Ddd*commandsMenu.prev.mnemonic:		        P
! *commandsMenu.prev.accelerator:	        Ctrl<Key>P
Ddd*commandsMenu.prev.acceleratorText:	        Up
Ddd*commandsMenu.prev.documentationString: \
@rm Get previous command

Ddd*commandsMenu.next.labelString:	        Next
Ddd*commandsMenu.next.mnemonic:		        N
! *commandsMenu.next.accelerator:	        Ctrl<Key>N
Ddd*commandsMenu.next.acceleratorText:	        Down
Ddd*commandsMenu.next.documentationString: \
@rm Get next command

Ddd*commandsMenu.complete.labelString:	        Complete
Ddd*commandsMenu.complete.mnemonic:	        C
! *commandsMenu.complete.accelerator:	        None<Key>Tab
! *commandsMenu.complete.accelerator:	        Ctrl<Key>I
Ddd*commandsMenu.complete.acceleratorText:	Tab
Ddd*commandsMenu.complete.documentationString: \
@rm Complete current command

Ddd*commandsMenu.apply.labelString:	        Apply
Ddd*commandsMenu.apply.mnemonic:	        A
! *commandsMenu.apply.accelerator:	        None<Key>Return
Ddd*commandsMenu.apply.acceleratorText:         Return
Ddd*commandsMenu.apply.documentationString: \
@rm Execute current command

Ddd*commandsMenu.isearch_prev.labelString:	Search Previous
Ddd*commandsMenu.isearch_prev.mnemonic:	        r
Ddd*commandsMenu.isearch_prev.accelerator:	Ctrl<Key>R
Ddd*commandsMenu.isearch_prev.acceleratorText:  Ctrl+R
Ddd*commandsMenu.isearch_prev.documentationString: \
@rm Do incremental search forward in command history

Ddd*commandsMenu.isearch_next.labelString:	Search Next
Ddd*commandsMenu.isearch_next.mnemonic:	        S
Ddd*commandsMenu.isearch_next.accelerator:	Ctrl<Key>S
Ddd*commandsMenu.isearch_next.acceleratorText:  Ctrl+S
Ddd*commandsMenu.isearch_next.documentationString: \
@rm Do incremental search backward in command history

Ddd*commandsMenu.isearch_exit.labelString:	Quit Search
Ddd*commandsMenu.isearch_exit.mnemonic:	        Q
Ddd*commandsMenu.isearch_exit.acceleratorText:  Esc
Ddd*commandsMenu.isearch_exit.documentationString: \
@rm Quit incremental search

Ddd*commandsMenu.clear_line.labelString:        Clear Line
Ddd*commandsMenu.clear_line.mnemonic:	        L
! *commandsMenu.clear_line.accelerator:	        Ctrl ~Shift ~Meta<Key>U
Ddd*commandsMenu.clear_line.acceleratorText:    Ctrl+U
Ddd*commandsMenu.clear_line.documentationString: \
@rm Clear @GDB@ command line

Ddd*commandsMenu.clear_window.labelString:      Clear Window
Ddd*commandsMenu.clear_window.mnemonic:	        W
Ddd*commandsMenu.clear_window.accelerator:      Ctrl Shift ~Meta<Key>U
Ddd*commandsMenu.clear_window.acceleratorText:  Ctrl+Shift+U
Ddd*commandsMenu.clear_window.documentationString: \
@rm Clear all before last @GDB@ prompt

Ddd*commandsMenu.buttons.labelString:	        Edit Buttons...
Ddd*commandsMenu.buttons.mnemonic:	        B
Ddd*commandsMenu.buttons.documentationString: \
@rm Edit command buttons


! Status
Ddd*menubar.stack.labelString:		Status
Ddd*menubar.stack.mnemonic:		t
Ddd*menubar.stack*documentationString:  \
@rm Show the current program state

define(STACK_HELP, [\
\
@rm This is the EMPH(Status Menu).\n\
\n\
DESC(Backtrace..., [give a summary of how your program got where it is])\n\
DESC(Registers..., [show current processor registers])\n\
DESC(Threads..., [show current program threads])\n\
\n\
DESC(Up, [show the function that called the current one])\n\
DESC(Down, [show the function that was called by the current one])
])dnl

Ddd*menubar.stack.helpString:	STACK_HELP
Ddd*stackMenu*helpString:	STACK_HELP

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
@rm This is the EMPH(Source Menu).\n\
\n\
DESC(Edit Breakpoints..., [set, view, and edit breakpoints])\n\
\n\
ITEM If LBL(Find Words Only) is set, \
only complete words are found.\n\
    Otherwise, arbitrary occurrences are found.\n\
ITEM If LBL(Find Case Sensitive) is set, \
search is case-sensitive.\n\
    Otherwise, occurrences are found regardless of case.\n\
ITEM If LBL(Display Machine Code) is set, \
the current function is automatically disassembled.\n\
\n\
DESC(Edit Source..., [invoke text editor for current source])\n\
DESC(Reload Source , [reload current source file])\n\
\n\
DESC(Back, [return to previous source position])\n\
DESC(Forward, [move forward to next source position])
])dnl

Ddd*menubar.source.helpString:	SOURCE_HELP
Ddd*sourceMenu.helpString:	SOURCE_HELP

Ddd*sourceMenu.breakpoints.labelString:	Edit Breakpoints...
Ddd*sourceMenu.breakpoints.mnemonic:	d
Ddd*sourceMenu.breakpoints.documentationString: \
@rm Set, view, and edit breakpoints

Ddd*sourceMenu.findWordsOnly.labelString:	Find Words Only
Ddd*sourceMenu.findWordsOnly.mnemonic:		W
Ddd*sourceMenu*findWordsOnly.accelerator:	Meta<Key>W
Ddd*sourceMenu*findWordsOnly.acceleratorText:	Alt+W
Ddd*sourceMenu.findWordsOnly.documentationString:\
@rm Switch between finding complete words and finding arbitrary occurrences

Ddd*sourceMenu.findCaseSensitive.labelString:	Find Case Sensitive
Ddd*sourceMenu.findCaseSensitive.mnemonic:	i
Ddd*sourceMenu*findCaseSensitive.accelerator:	Meta<Key>I
Ddd*sourceMenu*findCaseSensitive.acceleratorText: Alt+I
Ddd*sourceMenu.findCaseSensitive.documentationString:\
@rm Toggle case-sensitive search

Ddd*sourceMenu.disassemble.labelString:		Display Machine Code
Ddd*sourceMenu.disassemble.mnemonic:		M
Ddd*sourceMenu*disassemble.accelerator:		Meta<Key>M
Ddd*sourceMenu*disassemble.acceleratorText:	Alt+M
Ddd*sourceMenu.disassemble.documentationString:\
@rm Toggle display of machine code window

Ddd*sourceMenu.edit.labelString:	Edit Source...
Ddd*sourceMenu.edit.mnemonic:		E
Ddd*sourceMenu.edit.accelerator:	Ctrl Shift ~Meta<Key>E
Ddd*sourceMenu.edit.acceleratorText:	Ctrl+Shift+E
Ddd*sourceMenu.edit.documentationString: \
@rm Invoke text editor for the current source

Ddd*sourceMenu.reload.labelString:	Reload Source
Ddd*sourceMenu.reload.mnemonic:		R
Ddd*sourceMenu.reload.accelerator:	Ctrl Shift ~Meta<Key>L
Ddd*sourceMenu.reload.acceleratorText:	Ctrl+Shift+L
Ddd*sourceMenu.reload.documentationString: \
@rm Reload current source file

Ddd*sourceMenu.back.labelString:	Back
Ddd*sourceMenu.back.mnemonic:		B
Ddd*sourceMenu.back.accelerator:	Ctrl<Key>Left
Ddd*sourceMenu.back.acceleratorText:	Ctrl+Left
Ddd*sourceMenu.back.documentationString: \
@rm Return to previous source position

Ddd*sourceMenu.forward.labelString:	Forward
Ddd*sourceMenu.forward.mnemonic:	F
Ddd*sourceMenu.forward.accelerator:	Ctrl<Key>Right
Ddd*sourceMenu.forward.acceleratorText:	Ctrl+Right
Ddd*sourceMenu.forward.documentationString: \
@rm Move forward to next source position


! Data menu
Ddd*menubar.data.labelString:		Data
Ddd*menubar.data.mnemonic:		D
Ddd*menubar.data*documentationString:   \
@rm Create and modify data displays

define(DATA_HELP, [\
\
@rm This is the EMPH(Data Menu).\n\
\n\
DESC(Edit Displays..., [select, enable and delete displays])\n\
DESC(Edit Watchpoints..., [set, view, and edit watchpoints])\n\
\n\
DESC(Detect Aliases, [toggle alias detection])\n\
\n\
DESC(Display Local Variables, [show current local variables])\n\
DESC(Display Arguments, [show current argument variables])\n\
DESC(More Status Displays..., [show other things about the \
debugged program])\n\
\n\
DESC(Align on Grid, [align all displays on the next grid point])\n\
DESC(Rotate Graph, [rotate the graph clockwise by 90 degrees])\n\
DESC(Layout Graph, [layout the graph])\n\
\n\
DESC(Select All, [select all displays])\n\
DESC(Refresh, [update all displays])
])dnl

Ddd*menubar.data*helpString:	DATA_HELP
Ddd*dataMenu*helpString:	DATA_HELP

Ddd*dataMenu.displays.labelString:	Edit Displays...
Ddd*dataMenu.displays.mnemonic:		D
Ddd*dataMenu.displays.documentationString: \
@rm Select, enable and delete displays

Ddd*dataMenu.watchpoints.labelString:	Edit Watchpoints...
Ddd*dataMenu.watchpoints.mnemonic:	W
Ddd*dataMenu.watchpoints.documentationString: \
@rm Set, view, and edit watchpoints

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
@rm Display the local variables of the current stack frame.

! This item is visible in GDB only.
Ddd*dataMenu.info args.labelString:	Display Arguments
Ddd*dataMenu.info args.mnemonic:	u
Ddd*dataMenu.info args.accelerator:	Meta<Key>U
Ddd*dataMenu.info args.acceleratorText: Alt+U
Ddd*dataMenu.info args.documentationString: \
@rm Display the argument variables of the current stack frame.

! This item is visible in DBX only.
Ddd*dataMenu.dump.labelString:		Display Local Variables
Ddd*dataMenu.dump.mnemonic:		L
Ddd*dataMenu.dump.accelerator:		Meta<Key>L
Ddd*dataMenu.dump.acceleratorText:	Alt+L
Ddd*dataMenu.dump.documentationString: \
@rm Display all local variables of the current stack frame.

! This item is visible in XDB only.
Ddd*dataMenu.l.labelString:		Display Local Variables
Ddd*dataMenu.l.mnemonic:		L
Ddd*dataMenu.l.accelerator:		Meta<Key>L
Ddd*dataMenu.l.acceleratorText:		Alt+L
Ddd*dataMenu.l.documentationString: \
@rm Display all local variables of the current stack frame.

! This item is visible in JDB only.
Ddd*dataMenu.locals.labelString:	Display Local Variables
Ddd*dataMenu.locals.mnemonic:		L
Ddd*dataMenu.locals.accelerator:	Meta<Key>L
Ddd*dataMenu.locals.acceleratorText:	Alt+L
Ddd*dataMenu.locals.documentationString: \
@rm Display all local variables of the current stack frame.

Ddd*dataMenu.infos.labelString:		More Status Displays...
Ddd*dataMenu.infos.mnemonic:		M
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

Ddd*dataMenu.selectAll.labelString:	Select All
Ddd*dataMenu.selectAll.mnemonic:	S
Ddd*dataMenu.selectAll.accelerator:	Meta<Key>Z
Ddd*dataMenu.selectAll.acceleratorText:	Alt+Z
Ddd*dataMenu.selectAll.documentationString:	\
@rm Select all displays

Ddd*dataMenu.refresh.labelString:	Refresh
Ddd*dataMenu.refresh.mnemonic:		f
Ddd*dataMenu.refresh.accelerator:	Ctrl ~Shift ~Meta<Key>L
Ddd*dataMenu.refresh.acceleratorText:	Ctrl+L
Ddd*dataMenu.refresh.documentationString:	\
@rm Redraw all displays, using current values


! Help menu
Ddd*menubar.help.labelString:		Help
Ddd*menubar.help.mnemonic:		H
Ddd*menubar.help*documentationString:   \
@rm Get more information

define(HELP_HELP, [\
\
@rm This is the EMPH(Help Menu).\n\
DESC(Overview, [help on how to use the help facility])\n\
\n\
DESC(On Item...,   [give help on the current item])\n\
DESC(On Context...,[click on an item and get help on it])\n\
DESC(On Window..., [help on the application itself])\n\
\n\
DESC(What Now?..., [give help on what to do now])\n\
\n\
DESC([[DDD]] Reference..., [the DDD on-line manual])\n\
DESC([[DDD]] News..., [what's new in DDD])\n\
DESC(@GDB@ Reference..., [the @GDB@ on-line manual])\n\
\n\
DESC([[DDD]] License..., [copying, distributing, and modifying DDD])\n\
DESC([[DDD]] WWW Page..., [the DDD WWW page])\n\
\n\
DESC(About [[DDD]]..., [some general information about DDD])
])dnl

Ddd*menubar.help*helpString: HELP_HELP
Ddd*helpMenu*helpString:     HELP_HELP

Ddd*helpMenu.onHelp.labelString:		Overview...
Ddd*helpMenu.onHelp.mnemonic:			v
Ddd*helpMenu.onHelp.documentationString:	\
@rm How to use the help facility

Ddd*helpMenu.onItem.labelString:		On Item...
Ddd*helpMenu.onItem.mnemonic:			O
Ddd*helpMenu.onItem.accelerator:		~Shift ~Ctrl<Key>F1
! *helpMenu.onItem.accelerator:		        <Key>osfHelp
Ddd*helpMenu.onItem.acceleratorText:	        F1
Ddd*helpMenu.onItem.documentationString:	\
@rm Get help on the current item

Ddd*helpMenu.onContext.labelString:		On Context...
Ddd*helpMenu.onContext.mnemonic:		C
Ddd*helpMenu.onContext.accelerator:		Shift ~Ctrl<Key>F1
! *helpMenu.onContext.accelerator:		<Key>osfHelp
Ddd*helpMenu.onContext.acceleratorText:	        Shift+F1
Ddd*helpMenu.onContext.documentationString:	\
@rm Click on an item and get help on it

Ddd*helpMenu.onWindow.labelString:		On Window...
Ddd*helpMenu.onWindow.mnemonic:			W
Ddd*helpMenu.onWindow.documentationString:	\
@rm Information about the current window

Ddd*helpMenu.whatNext.labelString:		What Now?...
Ddd*helpMenu.whatNext.mnemonic:			N
Ddd*helpMenu.whatNext.accelerator:		~Shift Ctrl<Key>F1
Ddd*helpMenu.whatNext.acceleratorText:		Ctrl+F1
Ddd*helpMenu.whatNext.documentationString:	\
@rm Explain the current situation and suggest what to do next

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
! Preferences
!-----------------------------------------------------------------------------

! Ddd*preferences_popup*okLabelString: 		Close
Ddd*preferences_popup*cancelLabelString:	Reset
Ddd*preferences_popup.title:			DDD Preferences

Ddd*preferences*buttons.orientation: 		XmHORIZONTAL
Ddd*preferences*buttons.borderWidth:  		0
Ddd*preferences*buttons.marginWidth:  		0
Ddd*preferences*buttons.marginHeight:		0
Ddd*preferences*buttons.entryAlignment:		XmALIGNMENT_CENTER
Ddd*preferences*buttons*indicatorOn:		False
Ddd*preferences*buttons*shadowThickness:	2
Ddd*preferences*buttons*alignment:		XmALIGNMENT_CENTER
Ddd*preferences*panel*XmRowColumn.marginHeight: 0

Ddd*preferences*buttons*general.labelString:	\ \ \ \ \ \ General\ \ \ \ \ \ 
Ddd*preferences*buttons*source.labelString:	Source
Ddd*preferences*buttons*data.labelString:	Data
Ddd*preferences*buttons*startup.labelString:	Startup
Ddd*preferences*buttons*helpers.labelString:	Helpers

Ddd*preferences*general*helpString:	\
@rm These are the EMPH(General Preferences).\n\
\n\
ITEM When you move the mouse pointer over a button, \
DDD can automatically\n\
    give a hint on its usage.\n\
    SUBITEM Select LBL(as popup tips) if you want the hint to be shown \
in a popup window.\n\
    SUBITEM Select LBL(in the status line) if you want it to be shown \
in the status line.\n\
    You can also select both options, or none at all.\n\
ITEM When you move the mouse pointer over a variable in the source window,\n\
    DDD can automatically display the variable value.\n\
    SUBITEM Select LBL(as popup tips) if you want the value to be shown \
in a popup window.\n\
    SUBITEM Select LBL(in the status line) if you want it to be shown \
in the status line.\n\
    You can also select both options, or none at all.\n\
ITEM If LBL(Iconify all windows at once) is set, all DDD windows are \
iconified as a group.\n\
    Otherwise, windows are iconified separately.\n\
ITEM If LBL(Uniconify when ready) is set, the DDD windows are \
automatically uniconified\n\
    when DDD becomes ready (e.g. after reaching a breakpoint).\n\
    Otherwise, DDD windows remain iconified.\n\
ITEM If LBL(TAB key completes in all windows) is set, the KEY(TAB) key\n\
    completes arguments in all DDD windows.\n\
    Otherwise, the KEY(TAB) key completes in the @GDB@ console only.\n\
ITEM If LBL(Suppress X warnings) is set, X warnings are silently ignored.\n\
ITEM If LBL(Ungrab mouse pointer) is set, DDD will automatically\n\
    ungrab the mouse pointer when interrupting a modal X application.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.


Ddd*preferences*buttonHints.labelString:	 \
Automatic display of button hints
Ddd*preferences*valueHints.labelString:	 \
Automatic display of variable values

Ddd*preferences*tips.labelString: as popup tips
Ddd*preferences*docs.labelString: in the status line

Ddd*preferences*tabCompletion.labelString: \
TAB key completes
Ddd*preferences*inAllWindows.labelString: in all windows
Ddd*preferences*inConsole.labelString:    in console only

Ddd*preferences*buttonHints.width:      	240
Ddd*preferences*buttonHints.recomputeSize:	false

Ddd*preferences*valueHints.width:      		240
Ddd*preferences*valueHints.recomputeSize:	false

Ddd*preferences*tabCompletion.width:      	240
Ddd*preferences*tabCompletion.recomputeSize:	false

Ddd*preferences*groupIconify.labelString:	 \
Iconify all windows at once
Ddd*preferences*uniconifyWhenReady.labelString:	 \
Uniconify when ready
Ddd*preferences*suppressWarnings.labelString:	 \
Suppress X warnings
Ddd*preferences*ungrabMousePointer.labelString:	 \
Ungrab mouse pointer when interrupting modal X application


Ddd*preferences*source*helpString:	\
@rm These are the EMPH(Source Preferences).\n\
\n\
ITEM If LBL(Show position and breakpoints as glyphs) is set,\n\
    the current execution position and breakpoints are shown as glyphs.\n\
    Otherwise, they are shown as characters in the text.\n\
ITEM If LBL(Refer to program sources by full path name) is set, \n\
    source code locations are referred by full source file paths.\n\
    Otherwise, only the base name is used.\n\
ITEM If LBL(Find words only) is set, \
only complete words are found.\n\
    Otherwise, arbitrary occurrences are found.\n\
ITEM If LBL(Find case sensitive) is set, \
search is case-sensitive.\n\
    Otherwise, occurrences are found regardless of case.\n\
ITEM If LBL(Cache source files) is set, \
source texts are cached in memory.\n\
    Otherwise, sources are read from disk upon each source change.\n\
ITEM If LBL(Cache machine code) is set, \
disassembled code is cached in memory.\n\
    Otherwise, code is re-disassembled upon each function change.\n\
ITEM If LBL(Display source line numbers) is set, \n\
    each source line is prefixed by its number.\n\
ITEM In LBL(Tab width), you can set the spacing of tab stops.\n\
    Setting the tab width to 8 sets a tab stop every 8 characters.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.

Ddd*preferences*showExecPos.labelString:	Show position and breakpoints
Ddd*preferences*asGlyphs.labelString:		as glyphs
Ddd*preferences*asText.labelString:		as text characters

Ddd*preferences*showExecPos.width:		200
Ddd*preferences*showExecPos.recomputeSize:	false

Ddd*preferences*referSources.labelString:	Refer to program sources
Ddd*preferences*byPath.labelString:		by full path name
Ddd*preferences*byBase.labelString:		by base name only

Ddd*preferences*referSources.width:		200
Ddd*preferences*referSources.recomputeSize:	false

Ddd*preferences*cache.labelString:	        Cache
Ddd*preferences*cacheSource.labelString:	source files
Ddd*preferences*cacheCode.labelString:		machine code

Ddd*preferences*cache.width:		        200
Ddd*preferences*cache.recomputeSize:	        false

Ddd*preferences*find.labelString:	        Find
Ddd*preferences*wordsOnly.labelString:		words only
Ddd*preferences*caseSensitive.labelString:	case sensitive

Ddd*preferences*find.width:		        200
Ddd*preferences*find.recomputeSize:	        false

Ddd*preferences*displayLineNumbers.labelString:	 \
Display source line numbers

Ddd*preferences*tabWidth.orientation:		XmHORIZONTAL
Ddd*preferences*tabWidth.minimum:		1
Ddd*preferences*tabWidth.maximum:		32
Ddd*preferences*tabWidth.showValue:		true
Ddd*preferences*tabWidth.titleString:		Tab width



Ddd*preferences*data*helpString:	\
@rm These are the EMPH(Data Preferences).\n\
\n\
ITEM If LBL(Detect aliases) is set, DDD detects displays with the same\n\
    physical address and suppresses all aliases except the one that was\
 least recently changed.\n\
    This is useful for examining shared data structures.\n\
ITEM If LBL(Display two-dimensional arrays as tables) is set, the elements\
 of two-dimensional\n\
    arrays are aligned in a table.\n\
    Otherwise, two-dimensional arrays are displayed as nested one-dimensional\
 arrays.\n\
ITEM If LBL(Auto-align displays) is set,\n\
    each display is aligned on the nearest grid point.\n\
ITEM If LBL(Use compact layout) is set, the graph is layouted \
in a compact fashion.\n\
    This is suitable for homogeneous structures only.\n\
    Otherwise, the standard layout algorithm is used.\n\
ITEM If LBL(Automatic layout) is set, the graph is layouted
 after each change.\n\
    Otherwise, layout can be invoked manually.\n\
ITEM In LBL(Grid size), you can change the spacing of grid points.\n\
    A spacing of 0 disables the grid.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.

Ddd*preferences*detectAliases.labelString:	\
Detect aliases (shared data structures)
Ddd*preferences*align2dArrays.labelString:	\
Display two-dimensional arrays as tables
Ddd*preferences*showHints.labelString:	        \
Show edge hints
Ddd*preferences*snapToGrid.labelString:	        \
Auto-align displays on nearest grid point
Ddd*preferences*compactLayout.labelString:      \
Use compact layout (homogeneous structures only)
Ddd*preferences*autoLayout.labelString:	        \
Re-layout graph automatically after each change

Ddd*preferences*gridSize.orientation:		XmHORIZONTAL
Ddd*preferences*gridSize.minimum:		0
Ddd*preferences*gridSize.maximum:		64
Ddd*preferences*gridSize.showValue:		true
Ddd*preferences*gridSize.titleString:		Grid size

Ddd*preferences*startup*helpString:   \
@rm These are the EMPH(Startup Preferences).\n\
\n\
ITEM LBL(Windows) sets the window layout.\n\
    SUBITEM LBL(separate windows) means to use one source window,\n\
        one data window, and one @GDB@ console window.\n\
    SUBITEM LBL(one single window) means to use one single window\n\
        for source, data, and the @GDB@ console. (*)\n\
\n\
ITEM The LBL(Status line) can be placed\n\
    SUBITEM LBL(at bottom of source), in order to display \
menu descriptions, or\n\
    SUBITEM LBL(at top of source), as in DDD 2.0 and earlier. (*)\n\
\n\
ITEM The LBL(Tool buttons) can be placed\n\
    SUBITEM in the LBL(command tool) which can be moved around DDD, or\n\
    SUBITEM in the LBL(source window), as in DDD 1.4 and earlier.\n\
\n\
ITEM LBL(Keyboard focus) sets the keyboard focus policy.\n\
    SUBITEM LBL(click-to-type) means that you must click on a window\n\
        to direct the keyboard focus to it.\n\
    SUBITEM LBL(point-to-type) means that pointing to the window suffices.\n\
\n\
ITEM LBL(Data Scrolling) sets the data window scrolling mode.\n\
    SUBITEM LBL(Panner) means to use a two-dimensional scrollbar\n\
        (not available in all DDD configurations).\n\
    SUBITEM LBL(Scrollbars) means to use two scrollbars. (*)\n\
\n\
ITEM LBL(Debugger Type) sets the type of inferior debugger to use. (*)\n\
\n\
ITEM LBL(Splash Screen) lets you choose among different DDD splash screens,\n\
    or none at all.\n\
\n\
(*) Note: these startup preferences take effect only after saving options\n\
and restarting DDD.\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.

Ddd*preferences*startup*adjustMargin:		false

Ddd*preferences*windows.labelString:		DDD comes in
Ddd*preferences*windows.width:			200
Ddd*preferences*windows.recomputeSize:		false
Ddd*windowsMenu*separate.labelString:		separate windows
Ddd*windowsMenu*attached.labelString:		one single window

Ddd*preferences*statusPosition.labelString:	Status line is located
Ddd*preferences*statusPosition.width:		200
Ddd*preferences*statusPosition.recomputeSize:	false
Ddd*statusPositionMenu*bottom.labelString:	at bottom of source
Ddd*statusPositionMenu*top.labelString:		at top of source

Ddd*preferences*toolButtons.labelString:	Tool buttons are located
Ddd*preferences*toolButtons.width:		200
Ddd*preferences*toolButtons.recomputeSize:	false
Ddd*toolButtonsMenu*commandTool.labelString:	in command tool
Ddd*toolButtonsMenu*sourceWindow.labelString:   in source window

Ddd*preferences*keyboardFocus.labelString:	Keyboard focus is
Ddd*preferences*keyboardFocus.width:		200
Ddd*preferences*keyboardFocus.recomputeSize:	false
Ddd*keyboardFocusMenu*explicit.labelString:	click-to-type
Ddd*keyboardFocusMenu*pointer.labelString:	point-to-type

Ddd*preferences*dataScrolling.labelString:	Data is scrolled using
Ddd*preferences*dataScrolling.width:		200
Ddd*preferences*dataScrolling.recomputeSize:	false
Ddd*dataScrollingMenu*panner.labelString:	a panner
Ddd*dataScrollingMenu*scrollbars.labelString:	two scrollbars

Ddd*preferences*debugger.labelString:		Debugger type is
Ddd*preferences*debugger.width:			200
Ddd*preferences*debugger.recomputeSize:		false
Ddd*debuggerMenu*gdb.labelString:		GDB
Ddd*debuggerMenu*dbx.labelString:		DBX
Ddd*debuggerMenu*xdb.labelString:		XDB
Ddd*debuggerMenu*jdb.labelString:		JDB

Ddd*preferences*showStartupLogo.labelString:	Show DDD splash screen
Ddd*preferences*showStartupLogo.width:		200
Ddd*preferences*showStartupLogo.recomputeSize:	false

Ddd*showStartupLogoMenu*color.labelString:	in color
Ddd*showStartupLogoMenu*grey.labelString:	in grey
Ddd*showStartupLogoMenu*grey4.labelString:	in grey4
Ddd*showStartupLogoMenu*mono.labelString:	in monochrome
Ddd*showStartupLogoMenu*none.labelString:	not at all

Ddd*preferences*helpers*helpString: \
@rm These are the EMPH([DDD] Helpers), external applications used by DDD.\n\
\n\
ITEM LBL(Edit Sources) invokes an X editor for the current source file.\n\
    SAMP(@ FILE@ ) is replaced by the current file name.\n\
    SAMP(@ LINE@ ) is replaced by the current line.\n\
    Example: SAMP(xedit @ FILE@ )\n\
\n\
ITEM LBL(Get Core File) is a command to get a core file \
from a running process.\n\
    SAMP(@ FILE@ ) is replaced by the base name of the target core file.\n\
    SAMP(@ PID@ ) is replaced by the process ID.\n\
    The output must be written to SAMP(@ FILE@ .@ PID@ ).\n\
    Example: SAMP(gcore -o @ FILE@  @ PID@ )\n\
    Leave this empty if you have no SAMP(gcore) or similar command.\n\
\n\
ITEM LBL(List Processes) is a command to get a list of processes.\n\
    Example: SAMP(ps)\n\
\n\
ITEM LBL(Execution Window) is a command to start a terminal emulator.\n\
    To this command, DDD appends bourne shell commands to be executed\n\
    within the execution window.\n\
    Example: SAMP(xterm -e /bin/sh -c)\n\
\n\
ITEM LBL(Uncompress) is an uncompression command.\n\
    The command reads from standard input and writes to \
standard output.\n\
    Example: SAMP(gunzip -c)\n\
\n\
ITEM LBL(Web Browser) invokes a WWW browser.\n\
    SAMP(@ URL@ ) is replaced by the URL to be shown.\n\
    Example: SAMP(netscape @ URL@ )\n\
\n\
Use the buttons above to view and change other preferences.\n\
Click on LBL(Reset) to restore the saved preferences.

Ddd*preferences*edit*labelString:		Edit Sources
Ddd*preferences*get_core*labelString:		Get Core File
Ddd*preferences*ps*labelString:			List Processes
Ddd*preferences*term*labelString:		Execution Window
Ddd*preferences*uncompress*labelString:		Uncompress
Ddd*preferences*www*labelString:		Web Browser
Ddd*preferences*helpers*label.width:		150
Ddd*preferences*helpers*label.recomputeSize:	false
Ddd*preferences*text.columns:			50



!-----------------------------------------------------------------------------
! Settings
!-----------------------------------------------------------------------------

! Ddd*settings_popup*okLabelString: Close
Ddd*settings_popup*cancelLabelString: Reset
Ddd*settings_popup.title: DDD: Debugger Settings
Ddd*settings_popup.autoUnmanage: false

Ddd*settings*scroll.scrollingPolicy:		XmAUTOMATIC
Ddd*settings*scroll.height:			293
Ddd*settings*scroll.width:			528
Ddd*settings*form.width:			500
Ddd*settings*help.labelString:			?
Ddd*settings*XmTextField.columns:		10
Ddd*settings*sep.bottomOffset:			10
Ddd*settings*leader.bottomOffset:		4
Ddd*settings*leader.separatorType:		XmSINGLE_DASHED_LINE

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
@rm These are the EMPH(@GDB@ Settings).\n\
\n\
All settings (except source and object paths) can be saved\n\
using LBL(Edit) | LBL(Save Options).\n\
\n\
Click on LBL(?) near an item to get further information.\n\
In a text field, press the KEY_RETURN key to commit the changes.\n\
\n\
Click on LBL(Reset) to restore the saved settings.\n\
Click on LBL(OK) to close this window.


!-----------------------------------------------------------------------------
! Button and Shortcut Editor
!-----------------------------------------------------------------------------

Ddd*edit_buttons_popup.title: DDD: Button Editor

Ddd*edit_buttons*buttons.entryAlignment:	XmALIGNMENT_CENTER
Ddd*edit_buttons*buttons*indicatorOn:		False
Ddd*edit_buttons*buttons*shadowThickness:	2
Ddd*edit_buttons*buttons*alignment:		XmALIGNMENT_CENTER

Ddd*edit_buttons*console.labelString:		Console Buttons
Ddd*edit_buttons*source.labelString:		Source Buttons
Ddd*edit_buttons*data.labelString:		Data Buttons
Ddd*edit_buttons*shortcuts.labelString:		Shortcuts

Ddd*edit_buttons*text.rows: 			12
Ddd*edit_buttons*text.wordWrap:	        	true
Ddd*edit_buttons*text.scrollHorizontal:    	false

Ddd*edit_buttons*verify.labelString: 		Enable supported buttons only

Ddd*edit_buttons*helpString: \
@rm This is the EMPH(Button Editor).\n\
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
Click on LBL(Apply) to apply the changes.\n\
Click on LBL(OK) to apply the changes and close this window.\n\
Use LBL(Edit) | LBL(Save Options) to save all button specifications.

Ddd*edit_buttons*shortcuts*helpString: \
@rm This is the EMPH(Shortcut Editor).\n\
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
Click on LBL(Apply) to apply the changes.\n\
Click on LBL(OK) to apply the changes and close this window.\n\
Use LBL(Edit) | LBL(Save Options) to save all shortcuts.



!-----------------------------------------------------------------------------
! Status displays
!-----------------------------------------------------------------------------

! Ddd*infos_popup*okLabelString: Close
Ddd*infos_popup*cancelLabelString: Reset
Ddd*infos_popup.title: DDD: Status Displays
Ddd*infos_popup.autoUnmanage: false

Ddd*infos*scroll.scrollingPolicy:		XmAUTOMATIC
Ddd*infos*scroll.height:			293
Ddd*infos*scroll.width:				428
Ddd*infos*form.width:				400
Ddd*infos*help.labelString:			?
Ddd*infos*sep.bottomOffset:			10
Ddd*infos*leader.bottomOffset:			4
Ddd*infos*leader.separatorType:			XmSINGLE_DASHED_LINE

Ddd*infos*help.tipString:\
@rm Get help on this status display

! An empty help string means that the help text for this command
! is to be requested from GDB (via the GDB `help' command).
Ddd*infos*scroll*helpString:

! The panel itself has a help button, too.
Ddd*infos*helpString: \
@rm This is the list of EMPH(Status Displays).\n\
\n\
A EMPH(status display) shows things about the program being debugged\n\
in the data window.  Please pick your choice from the list.\n\
\n\
Click on LBL(?) near an item to get further information.\n\
\n\
Click on LBL(Reset) to delete all status displays.\n\
Click on LBL(OK) to close this window.



!-----------------------------------------------------------------------------
! Start-up logo
!-----------------------------------------------------------------------------

Ddd*logo_shell.logo.highlightThickness:	0
Ddd*logo_shell.logo.background:	  white
Ddd*logo_shell.logo.foreground:	  black
Ddd*logo_shell.logo.marginHeight: 0
Ddd*logo_shell.logo.marginWidth:  0
Ddd*logo_shell.logo.borderWidth:  0
Ddd*logo_shell.borderWidth:       0



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

! Width and height for several Motif & Athena variants

! Motif 1.x, using ScrolledWindow
Ddd*graph_edit_swindow.width:		622
Ddd*graph_edit_swindow.height:		180

! Motif 2.0, using ScrolledWindow
Ddd*graph_editSW.width:			622
Ddd*graph_editSW.height:		180

! Motif 1.x, using Porthole
Ddd*graph_edit_porthole.width:		622
Ddd*graph_edit_porthole.height:		180

! Motif 2.0, using Porthole
Ddd*graph_edit_form.width:		622
Ddd*graph_edit_form.height:		180

Ddd*graph_edit_swindow.spacing:		0
Ddd*graph_edit_swindow.borderWidth:	0
Ddd*graph_edit_swindow.shadowThickness:	0
Ddd*graph_editSW.spacing:		0
Ddd*graph_editSW.borderWidth:		0
Ddd*graph_editSW.shadowThickness:	0


Ddd*graph_edit_panner.backgroundPixmap:	25_foreground
Ddd*graph_edit_panner.defaultScale:	10
Ddd*graph_edit_panner.translations: \
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

Ddd*graph_edit.highlightOnEnter:	True
Ddd*graph_edit.navigationType:		TAB_GROUP

define(GRAPH_EDIT_HELP,[\
@rm This is the EMPH(Data Window).\n\
It shows the data displays of the debugged program.\n\
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
@rm This is a EMPH(panner).\n\
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

Ddd*node_popup.detail.labelString:	Show Detail
Ddd*node_popup.detail.documentationString: \
@rm Show/Hide details of the selected display

Ddd*node_popup.rotate.labelString:	Rotate
Ddd*node_popup.rotate.documentationString: \
@rm Rotate the selected display

Ddd*node_popup.set.labelString:		Set Value...
Ddd*node_popup.set.documentationString: \
@rm Change the selected display value

Ddd*node_popup.delete.labelString:	Delete Display
Ddd*node_popup.delete.documentationString: \
@rm Delete the selected display

Ddd*newMenu.other.labelString:		Other...
Ddd*newMenu.other.documentationString: \
@rm Modify the selected display expression to create a new display

Ddd*newMenu.edit.labelString:		Edit Menu...
Ddd*newMenu.edit.documentationString: \
@rm Edit this menu

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
! Graph Buttons
!-----------------------------------------------------------------------------

! Ddd*graph_cmd_w*helpString: \
! @rm Commands related to the data window.

Ddd*graph_cmd_w.marginWidth:	 0
Ddd*graph_cmd_w.marginHeight:	 0
Ddd*graph_cmd_area.marginWidth:	 0

Ddd*graph_cmd_w.arg_label.labelString:	():

Ddd*graph_cmd_w.graph_arg.columns:		22
Ddd*graph_cmd_w*graph_arg.editable:		false
Ddd*graph_cmd_w*graph_arg.helpString:	\
@rm This is the argument LBL(()) for the command buttons on the right.\n\
\n\
Set LBL(()) by selecting data displays.\n\
Clear LBL(()) by clicking on the prompt LBL(():).

Ddd*graph_cmd_w.orientation:			XmHORIZONTAL
Ddd*graph_cmd_w.graph_cmd_area.orientation:	XmHORIZONTAL
Ddd*graph_cmd_w.graph_cmd_area.spacing:		0
Ddd*graph_cmd_w.spacing:			0
Ddd*graph_cmd_w.graph_cmd_area.packing:	  	XmPACK_TIGHT
Ddd*graph_cmd_w.graph_cmd_area.entryAlignment:	XmALIGNMENT_CENTER
Ddd*graph_cmd_w.graph_cmd_area.alignment:	XmALIGNMENT_CENTER

Ddd*graph_cmd_w.graph_cmd_area*new.labelString:		New Display
Ddd*graph_cmd_w.graph_cmd_area*dereference.labelString: Display *()
Ddd*graph_cmd_w.graph_cmd_area*detail.labelString:	Show ()
Ddd*graph_cmd_w.graph_cmd_area*show_more.labelString:   Show More ()
Ddd*graph_cmd_w.graph_cmd_area*show_just.labelString:   Show Just ()
Ddd*graph_cmd_w.graph_cmd_area*show_detail.labelString: Show All ()
Ddd*graph_cmd_w.graph_cmd_area*hide_detail.labelString: Hide ()
Ddd*graph_cmd_w.graph_cmd_area*rotate.labelString:	Rotate ()
Ddd*graph_cmd_w.graph_cmd_area*rotateAll.labelString:	Rotate All ()
Ddd*graph_cmd_w.graph_cmd_area*set.labelString:		Set ()
Ddd*graph_cmd_w.graph_cmd_area*delete.labelString:	Delete ()
! Ddd*graph_cmd_w.graph_cmd_area*marginTop:	  1
! Ddd*graph_cmd_w.graph_cmd_area*marginBottom:	  1

define(ANNOUNCE_PULLDOWN,[\
Pulldown menu functions (press and hold BUTTON(1)):\
])dnl

define(MORE_PULLDOWN,[(hold for menu)])dnl

define(NEW_DISPLAY_HELP, [\
DESC(Shortcut 1, [User-defined shortcut 1])\n\
DESC(Shortcut 2, [User-defined shortcut 2]) ...\n\
DESC(Other..., [Enter new shortcut])\n\
DESC(Edit..., [Edit shortcuts])
])dnl

Ddd*graph_cmd_w.graph_cmd_area*new.helpString:\
LBL(New Display)\n\
Create a new display dependent on the selected display part.\n\
\n\
ANNOUNCE_PULLDOWN\n\
NEW_DISPLAY_HELP
Ddd*newMenu*helpString: NEW_DISPLAY_HELP

Ddd*graph_cmd_w.graph_cmd_area*new.tipString:\
@rm Create new display
Ddd*graph_cmd_w.graph_cmd_area*new.documentationString:\
@rm Create a new (dependent) display MORE_PULLDOWN()

Ddd*graph_cmd_w.graph_cmd_area*dereference.helpString:\
LBL(Display *())\n\
Dereference the selected display.
Ddd*graph_cmd_w.graph_cmd_area*dereference.tipString:\
@rm Dereference selected display
Ddd*graph_cmd_w.graph_cmd_area*dereference.documentationString:\
@rm Dereference the selected display

define(DETAIL_HELP, [\
DESC(Show More, [Show more details])\n\
DESC(Show Just, [Show details, excluding substructures]) ...\n\
DESC(Show All, [Show all details, including substructures])\n\
DESC(Hide, [Hide Details])
])dnl

Ddd*graph_cmd_w.graph_cmd_area*detail.helpString:	\
LBL(Show ()) / LBL(Hide ())\n\
Show/Hide details of the selected displays.\n\
\n\
ANNOUNCE_PULLDOWN\n\
DETAIL_HELP

Ddd*detailMenu*helpString:	DETAIL_HELP

Ddd*graph_cmd_w.graph_cmd_area*detail.tipString:	\
@rm Show/Hide selected details
Ddd*graph_cmd_w.graph_cmd_area*detail.documentationString: \
@rm Show/Hide details of the selected displays MORE_PULLDOWN()

Ddd*graph_cmd_w.graph_cmd_area*show_more.documentationString: \
@rm Show more details of the selected displays
Ddd*graph_cmd_w.graph_cmd_area*show_just.documentationString: \
@rm Show details of the selected displays, hiding substructures
Ddd*graph_cmd_w.graph_cmd_area*show_detail.documentationString: \
@rm Show all details of the selected displays, including substructures
Ddd*graph_cmd_w.graph_cmd_area*hide_detail.documentationString: \
@rm Hide details of the selected displays

define(ROTATE_HELP, [\
DESC(Rotate All, [Rotate substructures too])
])dnl

Ddd*graph_cmd_w.graph_cmd_area*rotate.helpString:	\
LBL(Rotate ())\n\
Rotate the selected displays.\n\
\n\
ANNOUNCE_PULLDOWN\n\
ROTATE_HELP

Ddd*rotateMenu*helpString:	ROTATE_HELP

Ddd*graph_cmd_w.graph_cmd_area*rotate.tipString:	\
@rm Rotate selected displays
Ddd*graph_cmd_w.graph_cmd_area*rotate.documentationString: \
@rm Rotate the selected displays MORE_PULLDOWN()
Ddd*graph_cmd_w.graph_cmd_area*rotateAll.documentationString: \
@rm Rotate the selected displays, including substructures

Ddd*graph_cmd_w.graph_cmd_area*set.helpString:	\
LBL(Set ())\n\
Change the selected display value.
Ddd*graph_cmd_w.graph_cmd_area*set.tipString:	\
@rm Set display value
Ddd*graph_cmd_w.graph_cmd_area*set.documentationString:	\
@rm Change the selected display value

Ddd*graph_cmd_w.graph_cmd_area*delete.helpString:	\
LBL(Delete ())\n\
Delete the selected displays.
Ddd*graph_cmd_w.graph_cmd_area*delete.tipString:	\
@rm Delete selected displays
Ddd*graph_cmd_w.graph_cmd_area*delete.documentationString:	\
@rm Delete the selected displays


!-----------------------------------------------------------------------------
! Status line
!-----------------------------------------------------------------------------

Ddd*status_form*helpString: \
@rm This is the EMPH(Status Line).\n\
\n\
It shows the last @GDB@ message as well as short DDD messages.\n\
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
Ddd*status_form.status.shadowThickness: 0
Ddd*status_form.status.fillOnArm:	false

Ddd*status_form.led.highlightThickness: 0
Ddd*status_form.led.labelString:
Ddd*status_form.led.visibleWhenOff:	false
Ddd*status_form.led.selectColor:	DarkGreen
Ddd*status_form.led.tipString:	\
@rm @GDB@ status
Ddd*status_form.led.documentationString: \
@rm @GDB@ status indicator: blinks while @GDB@ is busy \
(click to disable or enable)

Ddd*status_form.arrow.shadowThickness:  0
Ddd*status_form.arrow.foreground:  	grey

Ddd*status_lines.marginWidth:		0
Ddd*status_lines.marginHeight:		0
Ddd*status_lines.borderWidth:		0
Ddd*status_lines.spacing:		0

Ddd*status_history.row.background:	Black

!-----------------------------------------------------------------------------
! Source view
!-----------------------------------------------------------------------------

Ddd*source_text_w.columns:		80
Ddd*source_text_w.rows:			24

Ddd*code_text_w.columns:		80
Ddd*code_text_w.rows:			2

Ddd*source_form_w.width:		622
Ddd*source_form_w.height:		280

Ddd*code_form_w.width:			622
Ddd*code_form_w.height:			50

Ddd*source_text_w_swindow.spacing:		0
Ddd*source_text_w_swindow.borderWidth:		0
Ddd*source_text_w_swindow.shadowThickness:	0
Ddd*source_text_wSW.spacing:			0
Ddd*source_text_wSW.borderWidth:		0
Ddd*source_text_wSW.shadowThickness:		0

Ddd*code_text_w_swindow.spacing:		0
Ddd*code_text_w_swindow.borderWidth:		0
Ddd*code_text_w_swindow.shadowThickness:	0
Ddd*code_text_wSW.spacing:			0
Ddd*code_text_wSW.borderWidth:			0
Ddd*code_text_wSW.shadowThickness:		0

Ddd*source_text_w.scrollHorizontal:	false
Ddd*source_text_w.wordWrap:		true

Ddd*code_text_w.scrollHorizontal:	false
Ddd*code_text_w.wordWrap:		true

Ddd*source_text_w.helpString:	\
@rm This is the EMPH(Source Text Window).\n\
It shows the source text of the debugged program.\n\
\n\
STRONG(Left side)\n\
The EMPH(left side) shows line numbers and breakpoints:\n\
ITEM A plain stop sign or TEXT([#]VAR(n)[#]) indicates \
an enabled breakpoint.\n\
ITEM A grey stop sign or TEXT([_]VAR(n)[_]) indicates \
a disabled breakpoint.\n\
ITEM The next executed line is indicated by an arrow or TEXT(>).\n\
\n\
Click BUTTON(1) to select positions.\n\
Click BUTTON(3) to set and modify breakpoints.\n\
\n\
STRONG(Right side)\n\
The EMPH(right side) contains the source code.\n\
\n\
Click BUTTON(1) to select words.\n\
Drag with BUTTON(1) to extend your selection.\n\
Click BUTTON(3) to show values and lookup places.\n\
\n\
To view a specific source, use the LBL(Lookup ()) button below.

Ddd*code_text_w.helpString:	\
@rm This is the EMPH(Machine Code Window).\n\
It shows the machine code of the debugged program.\n\
\n\
STRONG(Left side)\n\
The EMPH(left side) shows addresses and breakpoints:\n\
ITEM A plain stop sign or TEXT([#]VAR(n)[#]) indicates \
an enabled breakpoint.\n\
ITEM A grey stop sign or TEXT([_]VAR(n)[_]) indicates \
a disabled breakpoint.\n\
ITEM The next executed location is indicated by an arrow or TEXT(>).\n\
\n\
Click BUTTON(1) to select addresses.\n\
Click BUTTON(3) to set and modify breakpoints.\n\
\n\
STRONG(Right side)\n\
The EMPH(right side) contains the machine code.\n\
\n\
Click BUTTON(1) to select words.\n\
Drag with BUTTON(1) to extend your selection.\n\
Click BUTTON(3) to show values and lookup places.\n\
\n\
To disassemble a specific function or address,\n\
use the LBL(Lookup ()) button below.

Ddd*source_view_shell.helpString:	\
@rm This is the EMPH(Source Window).\n\
The source window displays the source and machine code of the \
debugged program.\n\
\n\
The upper area shows the source code.\n\
The lower area shows the machine code.\n\
\n\
Use LBL(Help) | LBL(On Item) to get\n\
detailed help on each of these areas.


!-----------------------------------------------------------------------------
! Source Popup Menus
!-----------------------------------------------------------------------------

Ddd*line_popup.set.labelString:			Set Breakpoint
Ddd*line_popup.set.documentationString:	\
@rm Set a breakpoint at the selected position

Ddd*line_popup.set_temp.labelString:		Set Temporary Breakpoint
Ddd*line_popup.set_temp.documentationString:	\
@rm Set a temporary breakpoint which will be deleted when reached

Ddd*line_popup.temp_n_cont.labelString:		Continue Until Here
Ddd*line_popup.temp_n_cont.documentationString:	\
@rm Set a temporary breakpoint and resume execution

Ddd*line_popup.set_pc.labelString:		Set Execution Position
Ddd*line_popup.set_pc.documentationString:	\
@rm Set the current execution position to the selected position

Ddd*bp_popup.disable.labelString:		Disable Breakpoint
Ddd*bp_popup.disable.documentationString:	\
@rm Disable the selected breakpoint

Ddd*bp_popup.condition.labelString:		Set Condition...
Ddd*bp_popup.condition.documentationString:	\
@rm Specify a condition for the selected breakpoint

Ddd*bp_popup.ignore_count.labelString:		Set Ignore Count...
Ddd*bp_popup.ignore_count.documentationString:	\
@rm Specify how many crossings of the selected breakpoint are to be ignored

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
! Source Text Commands
!-----------------------------------------------------------------------------

Ddd*arg_cmd_w.marginWidth:     0
Ddd*arg_cmd_w.marginHeight:    0
Ddd*arg_cmd_area.marginWidth:  0

Ddd*arg_cmd_w.arg_label.labelString:		():

Ddd*arg_cmd_w.source_arg.helpString:	\
@rm This is the argument LBL(()) for the command buttons on the right.\n\
\n\
Edit LBL(()) using the usual editing functions.\n\
Set LBL(()) by selecting items from the source window or the data window.\n\
Clear LBL(()) by clicking on the prompt LBL(():).

Ddd*arg_cmd_w.source_arg.value:			 	main
Ddd*arg_cmd_w.source_arg.columns:		 	22

Ddd*arg_cmd_w.orientation:				XmHORIZONTAL
Ddd*arg_cmd_w.arg_cmd_area.orientation:			XmHORIZONTAL
Ddd*arg_cmd_w.arg_cmd_area.spacing:			0
Ddd*arg_cmd_w*spacing:					0
Ddd*arg_cmd_w.arg_cmd_area.packing:			XmPACK_TIGHT
Ddd*arg_cmd_w.arg_cmd_area.entryAlignment:		XmALIGNMENT_CENTER
Ddd*arg_cmd_w.arg_cmd_area*alignment:			XmALIGNMENT_CENTER

Ddd*arg_cmd_w.arg_cmd_area*lookup.labelString:		Lookup ()
Ddd*arg_cmd_w.arg_cmd_area*breakAt.labelString:		Break at ()
Ddd*arg_cmd_w.arg_cmd_area*tempBreakAt.labelString:	\
Set Temporary Breakpoint at ()
Ddd*arg_cmd_w.arg_cmd_area*contUntil.labelString:	Continue Until ()
Ddd*arg_cmd_w.arg_cmd_area*enable.labelString:		Enable Breakpoint at ()
Ddd*arg_cmd_w.arg_cmd_area*condition.labelString:	\
Set Condition at ()...
Ddd*arg_cmd_w.arg_cmd_area*ignore_count.labelString:	\
Set Ignore Count at ()...
Ddd*arg_cmd_w.arg_cmd_area*setPC.labelString:		\
Set Execution Position to ()
Ddd*arg_cmd_w.arg_cmd_area*watch.labelString:		Unwatch ()
Ddd*arg_cmd_w.arg_cmd_area*cwatch.labelString:	\
Set watchpoint on ()
Ddd*arg_cmd_w.arg_cmd_area*rwatch.labelString:	\
Set read watchpoint on ()
Ddd*arg_cmd_w.arg_cmd_area*awatch.labelString:	\
Set access watchpoint on ()
Ddd*arg_cmd_w.arg_cmd_area*print.labelString:		Print ()
Ddd*arg_cmd_w.arg_cmd_area*printRef.labelString:	Print *()
Ddd*arg_cmd_w.arg_cmd_area*whatis.labelString:		Whatis ()
Ddd*arg_cmd_w.arg_cmd_area*display.labelString:		Display ()
Ddd*arg_cmd_w.arg_cmd_area*dispRef.labelString:		Display *()
Ddd*arg_cmd_w.arg_cmd_area*find.labelString:		LBL_FIND_NEXT
Ddd*arg_cmd_w.arg_cmd_area*findBackward.labelString:	LBL_FIND_PREV
Ddd*arg_cmd_w.arg_cmd_area*findForward.labelString:	LBL_FIND_NEXT

Ddd*arg_cmd_w.arg_cmd_area*lookup.helpString:	\
LBL(Lookup ())\n\
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

Ddd*arg_cmd_w.arg_cmd_area*lookup.tipString:	\
@rm Lookup LBL(()) in the source
Ddd*arg_cmd_w.arg_cmd_area*lookup.documentationString:	\
@rm Lookup a line, file, function, variable or breakpoint in the source

define(BREAK_HELP, [\
DESC(Set Temporary Breakpoint at (), [set temporary breakpoint])\n\
DESC(Continue Until (), [set temporary breakpoint and resume execution])\n\
DESC(Enable / Disable Breakpoint at (), [enable or disable breakpoint])\n\
DESC(Set Condition at ()..., [set breakpoint condition])\n\
DESC(Set Ignore Count at ()..., [set breakpoint ignore count])\n\
DESC(Set Execution Position to (), [move execution position])
])dnl

Ddd*arg_cmd_w.arg_cmd_area*breakAt.helpString:	\
LBL(Break at ()) / LBL(Clear at ())\n\
Set or delete a breakpoint at the argument LBL(()).\n\
\n\
ANNOUNCE_PULLDOWN\n\
BREAK_HELP

Ddd*breakAtMenu*helpString:	BREAK_HELP

Ddd*arg_cmd_w.arg_cmd_area*breakAt.tipString:	\
@rm Set/Delete breakpoint at LBL(())
Ddd*arg_cmd_w.arg_cmd_area*breakAt.documentationString:	\
@rm Set or delete a breakpoint at the argument LBL(()) MORE_PULLDOWN()

Ddd*arg_cmd_w.arg_cmd_area*tempBreakAt.documentationString: \
@rm Set a temporary breakpoint at the argument LBL(())
Ddd*arg_cmd_w.arg_cmd_area*contUntil.documentationString: \
@rm Set a temporary breakpoint at LBL(()) and resume execution
Ddd*arg_cmd_w.arg_cmd_area*enable.documentationString: \
@rm Enable or disable the breakpoint at the argument LBL(())
Ddd*arg_cmd_w.arg_cmd_area*condition.documentationString: \
@rm Specify a condition for the breakpoint at LBL(())
Ddd*arg_cmd_w.arg_cmd_area*ignore_count.documentationString: \
@rm Specify how many crossings of the breakpoint at LBL(()) are to be ignored
Ddd*arg_cmd_w.arg_cmd_area*setPC.documentationString: \
@rm Set the current execution position to LBL(())

define(PRINT_HELP, [\
DESC(Print *(), [print dereferenced argument])\n\
DESC(Whatis (), [print type of argument])
])dnl

Ddd*arg_cmd_w.arg_cmd_area*print.helpString:	\
LBL(Print ())\n\
Print the argument LBL(()) in the @GDB@ console.\n\
\n\
ANNOUNCE_PULLDOWN\n\
PRINT_HELP
Ddd*printMenu*helpString:	PRINT_HELP

Ddd*arg_cmd_w.arg_cmd_area*print.tipString:	\
@rm Print LBL(()) in the debugger console
Ddd*arg_cmd_w.arg_cmd_area*print.documentationString:	\
@rm Print the argument LBL(()) in the @GDB@ console MORE_PULLDOWN()
Ddd*arg_cmd_w.arg_cmd_area*printRef.documentationString:	\
@rm Print the dereferenced argument LBL(()) in the @GDB@ console
Ddd*arg_cmd_w.arg_cmd_area*whatis.documentationString:	\
@rm Print the type of the argument LBL(()) in the @GDB@ console

define(DISPLAY_HELP, [\
DESC(Display *(), [display dereferenced argument])
])dnl

Ddd*arg_cmd_w.arg_cmd_area*display.helpString:	\
LBL(Display ())\n\
Display the argument LBL(()) in the data window.\n\
\n\
ANNOUNCE_PULLDOWN\n\
DISPLAY_HELP
Ddd*displayMenu*helpString:	DISPLAY_HELP


Ddd*arg_cmd_w.arg_cmd_area*display.tipString:	\
@rm Display LBL(()) in the data window
Ddd*arg_cmd_w.arg_cmd_area*display.documentationString:	\
@rm Display the argument LBL(()) in the data window MORE_PULLDOWN()
Ddd*arg_cmd_w.arg_cmd_area*dispRef.documentationString:	\
@rm Display the dereferenced argument LBL(()) in the data window

define(WATCH_HELP, [\
DESC(Set watchpoint on (), [stop whenever LBL(()) changes; same as LBL(Watch())])\n\
DESC(Set read watchpoint on (), [stop whenever LBL(()) is read])\n\
DESC(Set access watchpoint on (), [stop whenever LBL(()) is either read or written])\n\
\n\
Unless you have special hardware support, watchpoints slow down the\n\
debugged program by about two orders of magnitude.
])dnl

Ddd*arg_cmd_w.arg_cmd_area*watch.helpString:	\
LBL(Watch ()) / LBL(Unwatch())\n\
Stop whenever the value of LBL(()) changes.\n\
\n\
LBL(Watch ()) sets a EMPH(watchpoint) on LBL(()) - a special breakpoint that\n\
stops your program whenever the value of LBL(()) changes.  A watchpoint is\n\
managed like any other breakpoint, via LBL(Source) | LBL(Edit Breakpoints).\n\
\n\
LBL(Unwatch()) deletes the EMPH(watchpoint) associated with LBL(()).\n\
\n\
ANNOUNCE_PULLDOWN\n\
WATCH_HELP
Ddd*watchMenu*helpString:	WATCH_HELP

Ddd*arg_cmd_w.arg_cmd_area*watch.tipString:	\
@rm Stop whenever LBL(()) changes
Ddd*arg_cmd_w.arg_cmd_area*watch.documentationString:	\
@rm Stop whenever the value of LBL(()) changes MORE_PULLDOWN()
Ddd*arg_cmd_w.arg_cmd_area*cwatch.documentationString:	\
@rm Stop whenever the value of LBL(()) changes
Ddd*arg_cmd_w.arg_cmd_area*rwatch.documentationString:	\
@rm Stop whenever LBL(()) is read (requires hardware support)
Ddd*arg_cmd_w.arg_cmd_area*awatch.documentationString:	\
@rm Stop whenever LBL(()) is either read or written (requires hardware support)

define(FIND_HELP, [\
DESC(LBL_FIND_PREV, [find backwards])\n\
DESC(LBL_FIND_NEXT, [find forwards])
])dnl

Ddd*arg_cmd_w.arg_cmd_area*find.helpString:	\
LBL(LBL_FIND_PREV / LBL_FIND_NEXT)\n\
Search an occurrence of LBL(()) in the current source text.\n\
\n\
See LBL(Edit) | LBL(Preferences) | LBL(Source) for search settings.\n\
\n\
ANNOUNCE_PULLDOWN\n\
FIND_HELP
Ddd*findMenu*helpString:	FIND_HELP

Ddd*arg_cmd_w.arg_cmd_area*find.tipString:	\
@rm Find LBL(()) in source
Ddd*arg_cmd_w.arg_cmd_area*find.documentationString:	\
@rm Search LBL(()) in the current source MORE_PULLDOWN()
Ddd*arg_cmd_w.arg_cmd_area*findBackward.documentationString:	\
@rm Search the previous occurrence of LBL(()) in the source
Ddd*arg_cmd_w.arg_cmd_area*findForward.documentationString:	\
@rm Search the next occurrence of LBL(()) in the source



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

Ddd*plain_stop.helpString: \
STRONG(Breakpoint (enabled))\n\
The debugged program will stop when reaching this location.\n\
\n\
To change the breakpoint properties, press BUTTON(3).\n\
To move it to another location, drag it using BUTTON(1).
Ddd*plain_stop.tipString: 
Ddd*plain_stop.documentationString: 

Ddd*grey_stop.helpString: \
STRONG(Breakpoint (disabled))\n\
A disabled breakpoint has no effect until reenabled.\n\
\n\
To change the breakpoint properties, press BUTTON(3).\n\
To move it to another location, drag it using BUTTON(1).
Ddd*grey_stop.tipString: 
Ddd*grey_stop.documentationString: 

Ddd*plain_cond.helpString: \
STRONG(Conditional breakpoint (enabled))\n\
The debugged program will stop when reaching this location\n\
if the associated condition is true.\n\
\n\
To change the breakpoint properties, press BUTTON(3).\n\
To move it to another location, drag it using BUTTON(1).
Ddd*plain_cond.tipString: 
Ddd*plain_cond.documentationString: 

Ddd*grey_cond.helpString: \
STRONG(Conditional breakpoint (disabled))\n\
A disabled breakpoint has no effect until reenabled.\n\
\n\
To change the breakpoint properties, press BUTTON(3).\n\
To move it to another location, drag it using BUTTON(1).
Ddd*grey_cond.tipString: 
Ddd*grey_cond.documentationString: 



!-----------------------------------------------------------------------------
! Main Window
!-----------------------------------------------------------------------------

define(MAIN_WINDOW_HELP, [\
@rm This is the EMPH([DDD] Main Window).\n\
\n\
From top to bottom, it shows the following areas:\n\
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
Use LBL(Help) | LBL(On Item) to get detailed \
help on any of these areas.])dnl

Ddd.helpString:	            MAIN_WINDOW_HELP
Ddd.main_window.helpString: MAIN_WINDOW_HELP



!-----------------------------------------------------------------------------
! Debugger Console
!-----------------------------------------------------------------------------

Ddd*gdb_w.value: \
DDD @VERSION@ (@host@), by Dorothea L\374tkehaus and Andreas Zeller.\n\
Copyright \251 1998 Technische Universit\344t Braunschweig, Germany.\n

Ddd*gdb_w.columns:		80
Ddd*gdb_w.rows:			5

Ddd*gdb_wSW.width:		600
Ddd*gdb_wSW.height:		82

Ddd*gdb_w_swindow.spacing:		0
Ddd*gdb_w_swindow.borderWidth:		0
Ddd*gdb_w_swindow.shadowThickness:	0
Ddd*gdb_wSW.spacing:			0
Ddd*gdb_wSW.borderWidth:		0
Ddd*gdb_wSW.shadowThickness:		0

Ddd*gdb_w.editable:		true
Ddd*gdb_w.allowResize:		true
Ddd*gdb_w.editMode:		XmMULTI_LINE_EDIT
Ddd*gdb_w.scrollHorizontal:	false
Ddd*gdb_w.wordWrap:		true
Ddd*gdb_w.autoShowCursorPosition:	true
Ddd*gdb_w.cursorPositionVisible:	true

define(CONSOLE_HELP,[\
@rm This is the EMPH(@GDB@ Console).\n\
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
Ddd*tool_shell*saveUnder: true

Ddd*tool_shell.helpString:		\
@rm This is the EMPH(Command Tool).\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
The context sensitive help for the buttons gives a short command description.

Ddd*tool_buttons.helpString:\
@rm This is the EMPH(Command Tool).\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
The context sensitive help for the buttons gives a short command description.

!-----------------------------------------------------------------------------
! Debugger Buttons
!-----------------------------------------------------------------------------

Ddd*source_buttons.helpString:\
@rm This is a EMPH(command area).\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
The context sensitive help for the buttons gives a short command description.

Ddd*command_buttons.helpString:\
@rm This is a EMPH(command area).\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
The context sensitive help for the buttons gives a short command description.

Ddd*data_buttons.helpString:\
@rm This is a EMPH(command area).\n\
\n\
By clicking on one of the buttons, \
the corresponding command is sent to @GDB@.\n\
The context sensitive help for the buttons gives a short command description.

! An empty help string means that the help text for this button
! is to be requested from GDB (via the GDB `help' command).
Ddd*source_buttons*XmPushButton.helpString:
Ddd*console_buttons*XmPushButton.helpString:
Ddd*data_buttons*XmPushButton.helpString:
Ddd*tool_buttons*XmPushButton.helpString:
Ddd*tool_bar*XmPushButton.helpString:

! Same applies for button tips.
Ddd*source_buttons*XmPushButton.tipString:
Ddd*console_buttons*XmPushButton.tipString:
Ddd*data_buttons*XmPushButton.tipString:
Ddd*tool_buttons*XmPushButton.tipString:
Ddd*tool_bar*XmPushButton.tipString:

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
Interrupt the debugged process (or current @GDB@ command).\n\
(Equivalent to KEY(Ctrl+C)).
Ddd*source_buttons*break.tipString: \
@rm Interrupt debugged program
Ddd*data_buttons*break.tipString: \
@rm Interrupt debugged program
Ddd*console_buttons*break.tipString: \
@rm Interrupt debugged program
Ddd*tool_buttons*break.tipString: \
@rm Interrupt debugged program
Ddd*tool_bar*break.tipString: \
@rm Interrupt debugged program
Ddd*?*break.documentationString: \
@rm Interrupt the debugged process (or current @GDB@ command)

Ddd*?*Yes.helpString:	\
LBL(Yes)\n\
Confirm the @GDB@ question.
Ddd*?*Yes.tipString: \
@rm Just say yes

Ddd*?*No.helpString:	\
LBL(No)\n\
Do EMPH(not) confirm the @GDB@ question.
Ddd*?*No.tipString: \
@rm Just say no

Ddd*?*Complete.helpString:	\
LBL(Complete)\n\
Complete the current debugging command.  (Equivalent to KEY(TAB)).
Ddd*?*Complete.tipString: \
@rm Complete current command

Ddd*?*Clear.helpString:	\
LBL(Clear)\n\
Clear the current debugging command.  (Equivalent to KEY(Ctrl+U)).
Ddd*?*Clear.tipString: \
@rm Clear current command

Ddd*?*Prev.helpString:	\
LBL(Prev)\n\
Get the previous debugging command \
(Equivalent to KEY(Ctrl+P) or KEY(Up)).
Ddd*?*Prev.tipString: \
@rm Get previous command

Ddd*?*Next.helpString:	\
LBL(Next)\n\
Get the next debugging command \
(Equivalent to KEY(Ctrl+N) or KEY(Down)).
Ddd*?*Next.tipString: \
@rm Get next command

Ddd*?*Apply.helpString:	\
LBL(Apply)\n\
Execute the current debugging command.  (Equivalent to KEY_RETURN).
Ddd*?*Apply.tipString: \
@rm Execute current command

Ddd*?*Back.helpString:	\
LBL(Back)\n\
Return to the previous source position.
Ddd*?*Back.tipString: \
@rm Previous source position

Ddd*?*Forward.helpString:	\
LBL(Forward)\n\
Move forward to the next source position.
Ddd*?*Forward.tipString: \
@rm Next source position

Ddd*?*Edit.helpString:	\
LBL(Edit)\n\
Invoke an editor for the current source file.
Ddd*?*Edit.tipString: \
@rm Edit source file
Ddd*?*Edit.documentationString: \
@rm Invoke text editor for the current source

Ddd*?*Reload.helpString:	\
LBL(Reload)\n\
Reload the source from file.
Ddd*?*Reload.tipString: \
@rm Reload source file
Ddd*?*Reload.documentationString: \
@rm Reload current source file




!-----------------------------------------------------------------------------
! Print Dialog
!-----------------------------------------------------------------------------

Ddd*print_popup.title: DDD: Print Graph

Ddd*print*options.orientation:		      XmVERTICAL
Ddd*print*options.marginWidth:		      0
Ddd*print*options.marginHeight:		      0
Ddd*print*options.spacing:		      0
Ddd*print*options.?.orientation:	      XmHORIZONTAL
Ddd*print*options.?.marginWidth:	      0
Ddd*print*options.?.marginHeight:	      0
Ddd*print*options.?.?.orientation:	      XmHORIZONTAL
Ddd*print*options*isAligned:		      false
Ddd*print*options*alignment:		      XmALIGNMENT_BEGINNING

Ddd*print*print_to.labelString:		      Print To:
Ddd*print*print_to.width:		      120
Ddd*print*print_to.recomputeSize:	      false
Ddd*print*print_to_field*printer.labelString: Printer
Ddd*print*print_to_field*file.labelString:    File

Ddd*print*print_command.labelString:	      Print Command:
Ddd*print*print_command.width:		      120
Ddd*print*print_command.recomputeSize:	      false
Ddd*print*print_command_field.columns:	      48

Ddd*print*file_name.labelString:	      File Name:
Ddd*print*file_name.width:		      120
Ddd*print*file_name.recomputeSize:	      false
Ddd*print*file_name_field.columns:	      32
Ddd*print*file_name_field.value:	      ddd()graph.ps
Ddd*print*postscript.labelString:	      PostScript
Ddd*print*xfig.labelString:		      FIG

Ddd*print*print_what.labelString:	      Print:
Ddd*print*print_what.width:		      120
Ddd*print*print_what.recomputeSize:	      false
Ddd*print*all.labelString:		      All Displays
Ddd*print*selected.labelString:		      Selected Displays

Ddd*print*print_orientation.labelString:      Orientation:
Ddd*print*print_orientation.width:	      120
Ddd*print*print_orientation.recomputeSize:    false
Ddd*print*portrait.labelString:		      Portrait
Ddd*print*landscape.labelString:	      Landscape

Ddd*print*paper_size.labelString:	      Paper Size:
Ddd*print*paper_size.width:		      120
Ddd*print*paper_size.recomputeSize:	      false
Ddd*print*paper_size_field.orientation:	      XmVERTICAL
Ddd*print*paper_size_field.numColumns:	      3
Ddd*print*a4.labelString:		      A4 (210mm TIMES 297mm)
Ddd*print*a3.labelString:		      A3 (297mm TIMES 420mm)
Ddd*print*letter.labelString:		      Letter (8ONE_HALF" TIMES 11")
Ddd*print*legal.labelString:		      Legal (8ONE_HALF" TIMES 14")
Ddd*print*executive.labelString:	      Executive (7ONE_HALF" TIMES 10")
Ddd*print*custom.labelString:		      Other...

Ddd*print.autoUnmanage:	 false

Ddd*print*helpString: \
@rm You can print the graph on a POSTSCRIPT printer \
or print the graph in a file.\n\
Enter the print command or the file name in the appropriate fields.\n\
\n\
Files can be created in the following formats:\n\
DESC(PostScript, POSTSCRIPT format)\n\
DESC(XFIG, [XFIG format (from the CODE(xfig) program)])\n\
\n\
If you wish to print all displays, select LBL(All Displays).\n\
If you wish to print selected displays only, select\
 LBL(Selected Displays).\n\
For POSTSCRIPT format only, you can also specify \
orientation and paper size.\n\
\n\
To print, click on LBL(OK) or LBL(Apply).

Ddd*paper_size_dialog_popup.title:  		DDD: Paper Size
Ddd*paper_size_dialog.autoUnmanage:  		false
Ddd*paper_size_dialog.selectionLabelString: 	Paper size
Ddd*paper_size_dialog*helpString: \
@rm Please enter the paper size in the format \
KBD(VAR(width) CODE(x) VAR(height)).\n\
\n\
Examples:\n\
KBD(42cm x 59.4cm) - A2 paper\n\
KBD(7.5in x 10in) - Executive paper\n\
\n\
Recognized units include:\n\
KBD(pt) (points), KBD(in) (inches), KBD(mm) (millimeters),\n\
KBD(cm) (centimeters), KBD(parsec) (parsecs).



!-----------------------------------------------------------------------------
! File Selection Dialogs
!-----------------------------------------------------------------------------

Ddd*exec_files_popup.title:		   DDD: Open Program
Ddd*exec_files_popup*okLabelString:	   Open
Ddd*exec_files_popup*selectionLabelString: Program
Ddd*exec_files_popup*helpString:	   \
@rm Enter the name of the program to be debugged in the argument field.\n\
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

Ddd*core_files_popup.title:		   DDD: Open Core Dump
Ddd*core_files_popup*okLabelString:	   Open
Ddd*core_files_popup*selectionLabelString: Core Dump
Ddd*core_files_popup*helpString:	   \
@rm Enter the name of a core dump in the argument field.\n\
\n\
The core dump is used for examining memory and registers.\n\
No argument means to have no core file.\n\
\n\
Before using LBL(Open Core), you should use LBL(File) | LBL(Open Program)\n\
to specify the program causing the core dump, and to load its symbol table.\n\
\n\
Click on LBL(Open) to open the selected core dump.\n\
Click on LBL(Filter) to apply the given filter.

Ddd*source_files_popup.title:		   DDD: Open Source
Ddd*source_files_popup*okLabelString:	   Open
Ddd*source_files_popup*selectionLabelString: Source File
Ddd*source_files_popup*helpString:    \
@rm Enter the name of a source file in the argument field.\n\
\n\
The source file is shown in the source window and may be used\n\
for setting or clearing breakpoints.\n\
\n\
Before using LBL(Open Core), you should use LBL(File) | LBL(Open Program)\n\
to specify the executable program, and to load its symbol table.\n\
\n\
Click on LBL(Open) to open the selected source file.\n\
Click on LBL(Filter) to apply the given filter.

Ddd*processes_popup.title: DDD: Attach to Process
Ddd*processes.listLabelString:	Processes
Ddd*processes*selectionPolicy:	XmSINGLE_SELECT

Ddd*processes*okLabelString:	Attach
Ddd*processes*applyLabelString:	Update

Ddd*processes*helpString:	\
@rm Select a running process of the program to be debugged.\n\
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
Before using LBL(Attach), you should use LBL(File) | LBL(Open Program)\n\
to specify the program running in the process, and to load its symbol table.\n\
\n\
Click on LBL(Attach) to attach to the selected process.\n\
Click on LBL(Update) to update the list of processes.


Ddd*classes_popup.title: 		DDD: Open Class
Ddd*classes.listLabelString:		Classes
Ddd*classes.selectionLabelString: 	Open Class
Ddd*classes*selectionPolicy:		XmSINGLE_SELECT
Ddd*classes*okLabelString:	 	Open
Ddd*classes*applyLabelString:	 	Update
Ddd*classes*helpString:	\
@rm Please select a class to be debugged.\n\
\n\
These classes all have corresponding CODE(.java) sources in the class path.\n\
To search for other classes, select LBL(Edit) | LBL(JDB Settings)\n\
and add their directories to the class path.\n\
\n\
Click on LBL(Open) to open the selected class.\n\
Click on LBL(Update) to scan the class path once more.



Ddd*sessions_to_open_popup.title: 		DDD: Open Session
Ddd*sessions_to_open.listLabelString:		Sessions
Ddd*sessions_to_open.selectionLabelString: 	Open Session
Ddd*sessions_to_open*selectionPolicy:		XmSINGLE_SELECT
Ddd*sessions_to_open*okLabelString:	 	Open
Ddd*sessions_to_open*applyLabelString:	 	Delete
Ddd*sessions_to_open*helpString:	\
@rm Enter the name of a DDD session in the argument field.\n\
\n\
A DDD session records the state of a debugging session,\n\
such that you can exit debugging sessions and resume later.\n\
\n\
Click on LBL(Open) to open the selected session.\n\
Click on LBL(Delete) to delete the selected session.


Ddd*sessions_to_save_popup.title: 		DDD: Save Session
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
@rm Enter the name of a DDD session in the argument field.\n\
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


!-----------------------------------------------------------------------------
! Breakpoint Editor
!-----------------------------------------------------------------------------

Ddd*edit_breakpoints_dialog_popup.title: DDD: Breakpoint and Watchpoint Editor
Ddd*edit_breakpoints_dialog*breakpoints.labelString: \
Breakpoints and Watchpoints
Ddd*edit_breakpoints_dialog*form1.orientation:	 XmVERTICAL
Ddd*edit_breakpoints_dialog*form1.marginWidth:	 0
Ddd*edit_breakpoints_dialog*form1.marginHeight:	 0
Ddd*edit_breakpoints_dialog*form2.orientation:	 XmHORIZONTAL
Ddd*edit_breakpoints_dialog*form2.marginWidth:	 0
Ddd*edit_breakpoints_dialog*form2.marginHeight:	 0
Ddd*edit_breakpoints_dialog*buttons.orientation: XmVERTICAL
Ddd*edit_breakpoints_dialog*buttons.marginWidth:   0
Ddd*edit_breakpoints_dialog*buttons.marginHeight:  0
Ddd*edit_breakpoints_dialog.okLabelString:	 Close

Ddd*edit_breakpoints_dialog*helpString:	     \
@rm This is the EMPH(Breakpoint and Watchpoint Editor).\n\
\n\
Select breakpoints and watchpoints on the left; operations on the right.\n\
\n\
DESC(New Breakpoint..., [create a new breakpoint])\n\
DESC(New Watchpoint..., [create a new watchpoint])\n\
DESC(Lookup, [lookup selected item])\n\
DESC(Print, [print selected item])\n\
DESC(Enable, [enable all selected items])\n\
DESC(Disable, [disable all selected items])\n\
DESC(Condition..., [set or modify an item's condition])\n\
DESC(Ignore Count..., [set or modify an item's ignore count])\n\
DESC(Delete, [delete all selected items])

Ddd*new_breakpoint_dialog_popup.title: DDD: New Breakpoint
Ddd*new_breakpoint_dialog.selectionLabelString:	New Breakpoint

Ddd*new_breakpoint_dialog*helpString:	\
\
@rm Please enter a breakpoint you want to set.\n\
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
Multiple breakpoints at one place are permitted, and useful if conditional.


Ddd*new_watchpoint_dialog_popup.title: DDD: New Watchpoint
Ddd*new_watchpoint_dialog*set.labelString:	Set
Ddd*new_watchpoint_dialog*cwatch.labelString:	Watchpoint
Ddd*new_watchpoint_dialog*rwatch.labelString:	Read Watchpoint
Ddd*new_watchpoint_dialog*awatch.labelString:	Access Watchpoint
Ddd*new_watchpoint_dialog*on.labelString:	on

Ddd*new_watchpoint_dialog*helpString:	\
@rm Please enter a variable whose value you want to watch.\n\
Enter its name in the argument field.\n\
\n\
DESC(Set Watchpoint, [stop whenever the variable value changes])\n\
DESC(Set Read Watchpoint, [stop whenever the variable is read])\n\
DESC(Set Access Watchpoint, \
[stop whenever the variable is either read or written])


Ddd*edit_breakpoint_condition_dialog_popup.title: DDD: Condition
Ddd*edit_breakpoint_condition_dialog.selectionLabelString: Condition

Ddd*edit_breakpoint_condition_dialog*helpString:	       \
@rm Specify a condition VAR(cond) for the selected items.\n\
The selected items break only if VAR(cond) evaluates to a non-zero value.


Ddd*edit_breakpoint_ignore_count_dialog_popup.title: DDD: Ignore Count
Ddd*edit_breakpoint_ignore_count_dialog.selectionLabelString: \
Ignore Count

Ddd*edit_breakpoint_ignore_count_dialog*helpString:	  \
@rm Set the ignore count VAR(count) for the selected items.\n\
Next VAR(count) hits of the selected items will be ignored.


Ddd*edit_breakpoints_dialog*buttons*new_bp.labelString:	      New Breakpoint...
Ddd*edit_breakpoints_dialog*buttons*new_bp.tipString:     \
@rm Set new breakpoint
Ddd*edit_breakpoints_dialog*buttons*new_bp.documentationString: \
@rm Set breakpoint at specified line or function

Ddd*edit_breakpoints_dialog*buttons*new_wp.labelString:	      New Watchpoint...
Ddd*edit_breakpoints_dialog*buttons*new_wp.tipString:     \
@rm Set new watchpoint
Ddd*edit_breakpoints_dialog*buttons*new_wp.documentationString: \
@rm Set watchpoint on specified variable

Ddd*edit_breakpoints_dialog*buttons*lookup.labelString:	       Lookup
Ddd*edit_breakpoints_dialog*buttons*lookup.tipString:     \
@rm Lookup or print selected item
Ddd*edit_breakpoints_dialog*buttons*lookup.documentationString: \
@rm Lookup selected item in the source (breakpoint) or print value (watchpoint)

Ddd*edit_breakpoints_dialog*buttons*enable.labelString:	       Enable
Ddd*edit_breakpoints_dialog*buttons*enable.tipString:     \
@rm Enable selected items
Ddd*edit_breakpoints_dialog*buttons*enable.documentationString: \
@rm Enable the selected items

Ddd*edit_breakpoints_dialog*buttons*disable.labelString:       Disable
Ddd*edit_breakpoints_dialog*buttons*disable.tipString:     \
@rm Disable selected items
Ddd*edit_breakpoints_dialog*buttons*disable.documentationString: \
@rm Disable the selected items

Ddd*edit_breakpoints_dialog*buttons*condition.labelString:     Condition...
Ddd*edit_breakpoints_dialog*buttons*condition.tipString:     \
@rm Set condition
Ddd*edit_breakpoints_dialog*buttons*condition.documentationString:     \
@rm Specify a condition for the selected items

Ddd*edit_breakpoints_dialog*buttons*ignore_count.labelString:  Ignore Count...
Ddd*edit_breakpoints_dialog*buttons*ignore_count.tipString:     \
@rm Set ignore count
Ddd*edit_breakpoints_dialog*buttons*ignore_count.documentationString:     \
@rm Specify how many crossings of the selected items are to be ignored

Ddd*edit_breakpoints_dialog*buttons*delete.labelString:	       Delete
Ddd*edit_breakpoints_dialog*buttons*delete.tipString:     \
@rm Delete selected items
Ddd*edit_breakpoints_dialog*buttons*delete.documentationString:     \
@rm Delete the selected items


!-----------------------------------------------------------------------------
! Show Backtrace
!-----------------------------------------------------------------------------

Ddd*stack_dialog_popup.title:		  DDD: Backtrace
Ddd*stack_dialog.listLabelString:	  Backtrace
Ddd*stack_dialog.okLabelString:		  Up
Ddd*stack_dialog.applyLabelString:	  Down
Ddd*stack_dialog.cancelLabelString:	  Close
Ddd*stack_dialog*visibleItemCount:	  10

Ddd*stack_dialog*helpString:	  \
@rm This window shows the current EMPH(Backtrace).\n\
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

Ddd*register_dialog_popup.title:	     DDD: Registers
Ddd*register_dialog.listLabelString:	     Registers
Ddd*register_dialog.okLabelString:	     Close
Ddd*register_dialog.listVisibleItemCount:    12

Ddd*register_dialog*box.orientation: XmHORIZONTAL
Ddd*register_dialog*box.marginHeight: 0
Ddd*register_dialog*box.marginWidth:  0

Ddd*register_dialog*all_registers.labelString: All registers
Ddd*register_dialog*int_registers.labelString: Integer registers

Ddd*register_dialog*helpString:	     \
@rm This window shows the EMPH(Machine Registers).\n\
Select any register to have its name copied to the argument LBL(()).\n\
\n\
If LBL(Integer registers) is set, integer registers are shown.\n\
If LBL(All registers) is set, all registers are shown.


!-----------------------------------------------------------------------------
! Threads
!-----------------------------------------------------------------------------

Ddd*thread_dialog_popup.title:	     	     DDD: Threads
Ddd*thread_dialog.listLabelString:	     Threads
Ddd*thread_dialog.cancelLabelString:	     Close
Ddd*thread_dialog.okLabelString:	     Suspend
Ddd*thread_dialog.applyLabelString:	     Resume
Ddd*thread_dialog.listVisibleItemCount:      10

Ddd*thread_dialog*box.orientation:  XmHORIZONTAL
Ddd*thread_dialog*box.marginHeight: 0
Ddd*thread_dialog*box.marginWidth:  0

Ddd*thread_dialog*helpString:	     \
@rm This window shows the current EMPH(Execution Threads).\n\
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
ITEM click on LBL(Suspend) to suspend the selected threads.\n\
ITEM click on LBL(Resume) to resume the selected threads.





!-----------------------------------------------------------------------------
! Command History
!-----------------------------------------------------------------------------

Ddd*history_dialog_popup.title:		    DDD: Command History
Ddd*history_dialog.listLabelString:	    Command History
Ddd*history_dialog.cancelLabelString:	    Close
Ddd*history_dialog*visibleItemCount:	    10

Ddd*history_dialog*helpString:	    \
@rm This window shows the EMPH(Command History).\n\
\n\
DDD keeps track of the commands you type during your debugging\n\
sessions, so that you can be certain of precisely what happened.\n\
Use this window to manage the DDD command history facility.\n\
\n\
Select a specific command to have it copied at the @GDB@ prompt.\n\
Click on LBL(OK) or LBL(Apply) to execute the current command.


!-----------------------------------------------------------------------------
! Display Editor
!-----------------------------------------------------------------------------

Ddd*edit_displays_dialog_popup.title: DDD: Display Editor
Ddd*edit_displays_dialog*displays.labelString: Displays
Ddd*edit_displays_dialog*form1.orientation:   XmVERTICAL
Ddd*edit_displays_dialog*form1.marginWidth:   0
Ddd*edit_displays_dialog*form1.marginHeight:  0
Ddd*edit_displays_dialog*form2.orientation:   XmHORIZONTAL
Ddd*edit_displays_dialog*form2.marginWidth:   0
Ddd*edit_displays_dialog*form2.marginHeight:  0
Ddd*edit_displays_dialog*buttons.orientation: XmVERTICAL
Ddd*edit_displays_dialog*buttons.marginWidth:	0
Ddd*edit_displays_dialog*buttons.marginHeight:	0
Ddd*edit_displays_dialog.okLabelString:	      Close

Ddd*edit_displays_dialog*helpString:	  \
@rm This is the EMPH(Display Editor).\n\
\n\
Select displays on the left, operations on the right.\n\
\n\
Columns:\n\
DESC(Num, [the display number])\n\
DESC(Expression, [the display expression])\n\
DESC(State, [the display state (enabled, disabled, or alias)])\n\
DESC(Address, [the location of the display expression in memory])\n\
\n\
Buttons:\n\
DESC(New Display, [create a new display])\n\
DESC(Display *, [dereference the selected display])\n\
DESC(Show Detail, [show the details of all selected displays])\n\
DESC(Hide Detail, [hide the details of all selected displays])\n\
DESC(Set Value, [change a value in the selected display])\n\
DESC(Delete, [delete all selected displays])

define(NEW_DISPLAY_HELP,
[@rm Please enter an expression in the argument field.\n\
The value of the expression is displayed each time the program stops.\n\
\n\
Variables accessible are those of the lexical environment of the current\n\
stack frame, plus all those whose scope is global or an entire file.\n\
\n\
If LBL(Include in `New Display' Menu) is set, the new display expression\n\
becomes an item in the LBL(New Display) menu.])dnl

Ddd*new_display_dialog_popup.title:          	   DDD: New Display
Ddd*new_display_dialog.okLabelString:              Display
Ddd*new_display_dialog*label.labelString:    	   Display Expression
Ddd*new_display_dialog*shortcut.labelString:   	   \
Include in `New Display' Menu
Ddd*new_display_dialog*helpString:	           NEW_DISPLAY_HELP

Ddd*dependent_display_dialog_popup.title:          DDD: New Dependent Display
Ddd*dependent_display_dialog.okLabelString:        Display
Ddd*dependent_display_dialog*label.labelString:    Display Expression
Ddd*dependent_display_dialog*shortcut.labelString: \
Include in `New Display' Menu
Ddd*dependent_display_dialog*helpString: 	   NEW_DISPLAY_HELP\n\
\n\
The new display will be made dependent on the currently selected display.

Ddd*edit_displays_dialog*buttons*new.labelString:	    New...
Ddd*edit_displays_dialog*buttons*new.tipString:  \
@rm Create new display
Ddd*edit_displays_dialog*buttons*new.documentationString:  \
@rm Create new data display (possibly depending on the selected data display)

Ddd*edit_displays_dialog*buttons*dereference.labelString:   Display *
Ddd*edit_displays_dialog*buttons*dereference.tipString:  \
@rm Dereference selected display
Ddd*edit_displays_dialog*buttons*dereference.documentationString: \
@rm Dereference the selected data display

Ddd*edit_displays_dialog*buttons*show_detail.labelString:   Show Detail
Ddd*edit_displays_dialog*buttons*show_detail.tipString:  \
@rm Show selected details
Ddd*edit_displays_dialog*buttons*show_detail.documentationString: \
@rm Show detail on the selected data displays

Ddd*edit_displays_dialog*buttons*hide_detail.labelString:   Hide Detail
Ddd*edit_displays_dialog*buttons*hide_detail.tipString:  \
@rm Hide selected details
Ddd*edit_displays_dialog*buttons*hide_detail.documentationString: \
@rm Hide detail on the selected data displays

Ddd*edit_displays_dialog*buttons*set.labelString:	    Set Value
Ddd*edit_displays_dialog*buttons*set.tipString:  \
@rm Set display value
Ddd*edit_displays_dialog*buttons*set.documentationString: \
@rm Change a value in the selected data display

Ddd*edit_displays_dialog*buttons*delete.labelString:	    Delete
Ddd*edit_displays_dialog*buttons*delete.tipString:  \
@rm Delete selected displays
Ddd*edit_displays_dialog*buttons*delete.documentationString:  \
@rm Delete the selected data displays


Ddd*run_dialog_popup.title: DDD: Run Program
Ddd*run_dialog.listLabelString:		Arguments
Ddd*run_dialog.selectionLabelString:	Run with Arguments

Ddd*run_dialog*selectionPolicy:		XmSINGLE_SELECT

Ddd*run_dialog*helpString:	\
@rm You can now run the debugged program.\n\
\n\
DDD keeps track of the arguments you gave to the debugged program,\n\
such that you can re-use them at a later time.\n\
Select specific arguments to have them copied to the argument prompt.\n\
\n\
Click on LBL(OK) or LBL(Apply) to start the debugged program \
with the selected arguments.


Ddd*make_dialog_popup.title: DDD: Make
Ddd*make_dialog.listLabelString:	Targets
Ddd*make_dialog.selectionLabelString:	Make

Ddd*make_dialog*selectionPolicy:	XmSINGLE_SELECT

Ddd*make_dialog*helpString:	\
@rm You can now start the make program.\n\
\n\
DDD keeps track of the arguments you gave to the make program,\n\
such that you can re-use them at a later time.\n\
Select specific arguments to have them copied to the argument prompt.\n\
\n\
Click on LBL(OK) or LBL(Apply) to start the make program \
with the selected arguments.


Ddd*cd_dialog_popup.title: DDD: Change Directory
Ddd*cd_dialog.listLabelString:	Directories
Ddd*cd_dialog.selectionLabelString:	Change Directory

Ddd*cd_dialog*selectionPolicy:	XmSINGLE_SELECT

Ddd*cd_dialog*helpString:	\
@rm You can now change the current directory.\n\
\n\
DDD keeps track of the directories you gave to the cd command,\n\
such that you can re-use them at a later time.\n\
Select specific directories to have them copied to the argument prompt.\n\
\n\
Click on LBL(OK) or LBL(Apply) to change the current directory.


!-----------------------------------------------------------------------------
! Debugger State
!-----------------------------------------------------------------------------

Ddd*gdb_status_dialog_popup.title: DDD: Debugger State
Ddd*gdb_status_dialog*okLabelString: Close
Ddd*gdb_status_dialog*cancelLabelString: Update

Ddd*gdb_status_dialog*helpString: \
@rm This dialog shows the @GDB@ state and capabilities.\n\
Click on LBL(Update) to update its contents.


!-----------------------------------------------------------------------------
! Other Dialogs
!-----------------------------------------------------------------------------

Ddd*busy_dialog_popup.title: DDD: Debugger Busy
Ddd*busy_dialog.messageString:		\
@rm @GDB@ is busy!
Ddd*busy_dialog*helpString:	\
@rm @GDB@ is busy on some debugger action.\n\
\n\
You may wish to wait until the @GDB@ prompt appears,\n\
or interrupt the current debugging command by selecting\n\
LBL(Program) | LBL(Interrupt) (or typing KEY(Ctrl+C)).\n\
\n\
If @GDB@ is busy running your debugged program, interrupting\n\
@GDB@ means to interrupt program execution, such that you can\n\
examine the current program state.

Ddd*quit_dialog_popup.title: DDD: Debugger Still Busy
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
(by selecting LBL(Program) | LBL(Interrupt)).

Ddd*shutdown_dialog_popup.title: DDD: Program Running
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

Ddd*terminated_dialog_popup.title: DDD: Hasta la Vista
! Ddd*terminated_dialog.messageString:   \
! @rm @GDB@ terminated abnormally.
Ddd*terminated_dialog*okLabelString:	   Exit
Ddd*terminated_dialog*cancelLabelString:   Restart
Ddd*terminated_dialog*helpString:	\
@rm @GDB@ terminated abnormally.  Without @GDB@, DDD cannot run.\n\
\n\
You may wish to examine the final @GDB@ output before exiting\n\
or restarting DDD.\n\
\n\
Click on LBL(Exit) to exit DDD.\n\
Click on LBL(Restart) to restart DDD (and @GDB@) from scratch.

Ddd*exited_dialog_popup.title: DDD: Death on Arrival
! Ddd*exited_dialog.messageString:   \
! @rm @GDB@ exited.
Ddd*exited_dialog*okLabelString:	   Exit
Ddd*exited_dialog*cancelLabelString:   Restart
Ddd*exited_dialog*helpString:	\
@rm @GDB@ has exited.  Without @GDB@, DDD cannot run.\n\
\n\
You may wish to examine the final @GDB@ output before exiting\n\
or restarting DDD.\n\
\n\
Click on LBL(Exit) to exit DDD.\n\
Click on LBL(Restart) to restart DDD (and @GDB@) from scratch.

Ddd*no_debugger_dialog_popup.title: DDD: Losing my Religion
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
    but use one of the SAMP(--gdb), SAMP(--dbx), SAMP(--xdb), \
or SAMP(--jdb) options to specify\n\
    the inferior debugger.\n\
ITEM If @GDB@ is the correct inferior debugger, \
but could not be found,\n\
    re-invoke DDD, but use the SAMP(--debugger VAR(path)) option to \
specify\n\
    the full @GDB@ path VAR(path).\n\
ITEM If you used the SAMP(--debugger VAR(path)) option, be sure to specify \
one of\n\
    the SAMP(--gdb), SAMP(--dbx), SAMP(--xdb), or SAMP(--jdb) options \
as well.\n\
ITEM If you started DDD without arguments, re-invoke DDD,\n\
    but give it an executable as argument.  XDB and some DBX versions\n\
    require being invoked with an executable.\n\
ITEM DDD passes all unknown options to @GDB@.  If @GDB@ choked on\n\
    some options intended for DDD, use the DDD SAMP(--help) option to\n\
    get a list of valid DDD options.\n\
ITEM If @GDB@ could not be invoked due to some communication failure,\n\
    try changing the SAMP(.blockTTYInput) resource.\n\
For details on options and resources, see the DDD manual: select\n\
LBL(Help) | LBL([DDD] Reference) \
or invoke DDD with the SAMP(--manual) option.\n\
\n\
Click on LBL(Exit) to exit DDD.

Ddd*fatal_dialog_popup.title: DDD: Oops
Ddd*fatal_dialog.messageString:	  \
@rm Internal error
Ddd*fatal_dialog*okLabelString:	       Exit
Ddd*fatal_dialog*restart.labelString:  Restart
Ddd*fatal_dialog*cancelLabelString:    Ignore
Ddd*fatal_dialog*helpString: \
@rm Oops!  You have found a bug in DDD.\n\
You should better save your work immediately; DDD may fail again soon.\n\
\n\
If you can reproduce this bug, please send a bug report\n\
to EMAIL([ddd]-bugs@ ips.cs.tu-bs.de), giving a subject like\n\
\n\
    DDD @VERSION@ (@host@) gets @CAUSE@\n\
\n\
To enable us to fix the bug, you should include the following information:\n\
ITEM What you were doing to get this message.  Report all the facts.\n\
ITEM The contents of the FILE(~/.[ddd]/log) file \
as generated by this session.\n\
ITEM If a core file was generated in your directory, please run\n\
    KBD(gdb [ddd] core), and type KBD(where) at the SAMP((gdb)) prompt.\n\
    (Include this output only.  Do not include the core file itself.)\n\
Please read also the section ``Reporting Bugs'' in the DDD manual.\n\
\n\
We thank you for your support.\n\
\n\
Click on LBL(Exit) to exit DDD.\n\
Click on LBL(Restart) to restart DDD from scratch.\n\
Click on LBL(Ignore) to resume your DDD session.


Ddd*yn_dialog_popup.title: DDD: Debugger Question
Ddd*yn_dialog*okLabelString:	   Yes
Ddd*yn_dialog*cancelLabelString:   No
Ddd*yn_dialog*defaultButtonType:   XmDIALOG_OK_BUTTON
Ddd*yn_dialog*helpString:	\
@rm This is a @GDB@ question.\n\
Click on LBL(Yes) or LBL(No), or enter KBD(yes) or KBD(no) \
in the @GDB@ console.\n\
For more details, consult the @GDB@ documentation.

Ddd*gdb_message_dialog_popup.title: DDD: Debugger Message
Ddd*gdb_message_dialog*helpString:	\
@rm This is a @GDB@ message.\n\
For more details, consult the @GDB@ documentation.

Ddd*gdb_selection_dialog_popup.title: DDD: Selection
Ddd*gdb_selection_dialog*listLabelString:  Selection
Ddd*gdb_selection_dialog*helpString:	\
@rm This is a @GDB@ selection menu.\n\
Please select an item from the list and click on LBL(OK).\n\
For more details, consult the @GDB@ documentation.

Ddd*confirm_overwrite_dialog_popup.title: DDD: File Exists
Ddd*confirm_overwrite_dialog*messageString:	  \
@rm Overwrite existing file?
Ddd*confirm_overwrite_dialog*okLabelString:	  Yes
Ddd*confirm_overwrite_dialog*cancelLabelString:	  No
Ddd*confirm_overwrite_dialog*defaultButtonType:	  XmDIALOG_CANCEL_BUTTON
Ddd*confirm_overwrite_dialog*helpString:	\
@rm The file already exists.\n\
Click on LBL(Yes) to overwrite the existing file.

Ddd*save_options_dialog_popup.title: DDD: Save Options
Ddd*save_options_dialog.messageString:	 \
@rm DDD startup preferences were modified.\n\
Save them for the next DDD invocation?
Ddd*save_options_dialog*okLabelString:	     Yes
Ddd*save_options_dialog*cancelLabelString:   No
Ddd*save_options_dialog*defaultButtonType:   XmDIALOG_OK_BUTTON
Ddd*save_options_dialog*helpString:	\
@rm You have changed some DDD startup preferences\n\
(using LBL(Edit) | LBL(Preferences)).\n\
To save them for the next DDD invocation, click on LBL(Yes).\n\
To discard the changes, click on LBL(No).

Ddd*set_dialog_popup.title: DDD: Set Value
Ddd*set_dialog*helpString:	\
@rm You can now change a value in the selected display.\n\
If you want to keep it unchanged, click on LBL(Cancel).

Ddd*kill_to_save_dialog_popup.title: DDD: Save Session
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

Ddd*confirm_restart_dialog_popup.title: DDD: Restart Session
Ddd*confirm_restart_dialog.messageString: \
@rm The program is running.  Restart anyway (and kill it)?
Ddd*confirm_restart_dialog.okLabelString:     Yes
Ddd*confirm_restart_dialog.cancelLabelString: No
Ddd*confirm_restart_dialog*helpString: \
@rm This is a @GDB@ message.\n\
For more details, consult the @GDB@ documentation.

Ddd*data_not_saved_dialog_popup.title: DDD: Save Session
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

Ddd*lock_dialog_popup.title: DDD: There can be only one
Ddd*lock_dialog*helpString:	\
@rm Another DDD is running, sharing state with this DDD instance.\n\
\n\
To resume execution of this DDD, click on LBL(Continue).\n\
Otherwise, click on LBL(Exit).
Ddd*lock_dialog.okLabelString:		Continue
Ddd*lock_dialog.cancelLabelString: 	Exit


!-----------------------------------------------------------------------------
! Messages
!-----------------------------------------------------------------------------

! If we have an old app-defaults file installed, we may see an old
! version of this warning.  Gee, app-defaults files are plain just stupid.
Ddd*bad_version_warning_popup.title: DDD: Version Mismatch
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
    use LBL(Edit) | LBL(Save Options) to update it.

Ddd*class_error_popup.title: DDD: No Class
Ddd*class_error*helpString:	\
@rm The given class could not be loaded.\n\
Please check the value of the class search path (KBD(use) command).

Ddd*source_file_error_popup.title: DDD: No Source
Ddd*source_file_error*helpString:	\
@rm The given source file could not be loaded.

Ddd*remote_file_error_popup.title: DDD: No Remote Source
Ddd*remote_file_error*helpString:	\
@rm The given source file could not be loaded from the remote host.\n\
This message is also caused by attempting to load an empty source file.

Ddd*source_file_from_gdb_warning_popup.title: DDD: Source From Debugger
Ddd*source_file_from_gdb_warning*helpString:	\
@rm The given source file could not be loaded directly.\n\
Instead, the source was obtained from @GDB@ via the KBD(list) command.

Ddd*source_trunc_error_popup.title: DDD: Source Truncated
Ddd*source_trunc_error*helpString:	\
@rm The given source file could not be loaded entirely.

Ddd*source_empty_warning_popup.title: DDD: Empty Source
Ddd*source_empty_warning*helpString:	\
@rm The given source file contains no data.\n\
This is very unusual for source files.\n\
Please verify your file name and try again.

Ddd*source_binary_warning_popup.title: DDD: Binary Source
Ddd*source_binary_warning*helpString:	\
@rm The given source file contains NULL characters. \n\
This is very unusual for source files. \n\
You probably gave the name of a binary file. \n\
Please verify the file name and try again.

Ddd*source_find_error_popup.title: DDD: Not Found
Ddd*source_find_error*helpString:	\
@rm The given argument LBL(()) can not be found in the source text.\n\
\n\
See LBL(Edit) | LBL(Preferences) | LBL(Source) for search settings.

Ddd*manual_find_error_popup.title: DDD: Not Found
Ddd*manual_find_error*helpString:	\
@rm The given argument LBL(()) can not be found in the on-line manual.

Ddd*no_source_error_popup.title: DDD: No Source
Ddd*no_source_error*helpString:	\
@rm There is no current source text in which to search.

Ddd*no_source_edit_error_popup.title: DDD: No Source
Ddd*no_source_edit_error*helpString:	\
@rm There is no current source text to edit.

Ddd*no_such_line_error_popup.title: DDD: No such line
Ddd*no_such_line_error*helpString:	\
@rm There is no such line in the current source text.

Ddd*no_such_breakpoint_error_popup.title: DDD: No Such Breakpoint
Ddd*no_such_breakpoint_error*helpString:	\
@rm There is no such breakpoint in the current program.

Ddd*no_completion_error_popup.title: DDD: No Completion
Ddd*no_completion_error*helpString:	\
@rm @GDB@ does not support command and argument completion.\n\
\n\
Command completion requires the GDB KBD(complete) command,\n\
which is present in GDB 4.13 and later.

Ddd*no_program_popup.title: DDD: No Program
Ddd*no_program*helpString: \
@rm You have not opened any program yet.  @GDB@ needs a debuggee\n\
to access core dumps, source files, or attaching to processes.\n\
\n\
You can ignore this warning and proceed, but @GDB@ may be unable\n\
to display symbolic values and determine locations.\n\
\n\
To open a program, use LBL(File) | LBL(Open Program).


Ddd*print_empty_graph_error_popup.title: DDD: Nothing to Print
Ddd*print_empty_graph_error*helpString:	\
@rm There is nothing to print.\n\
The printing has been cancelled.

Ddd*print_failed_error_popup.title: DDD: Printing Failed
Ddd*print_failed_error*helpString:	\
@rm The file holding the graph picture could not be written.\n\
The printing has been cancelled.

Ddd*tty_exec_error_popup.title: DDD: No Execution Window
Ddd*tty_exec_error*helpString:	\
@rm The terminal emulator for the execution window\n\
could not be invoked.  The debugged process will execute in the\n\
@GDB@ console instead.\n\
\n\
Please verify the contents of the LBL(Edit) | LBL(Preferences) | \n\
LBL(Helpers) | LBL(Execution Window) resource and make sure that the given\n\
terminal program is within your executable path.

Ddd*tty_command_error_popup.title: DDD: TTY Failed
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
CODE([Ddd]*useTTYCommand: false)\n\
\n\
in your FILE(~/.[ddd]/init) file and restart DDD.

Ddd*tty_type_error_popup.title: DDD: Term Failed
Ddd*tty_type_error*helpString:	\
@rm DDD could not set the TERM environment variable\n\
to the type of the execution window.  This means that\n\
your program will execute with a terminal type\n\
set to SAMP(dumb) instead of SAMP(xterm).

Ddd*history_save_error_popup.title: DDD: Save History Failed
Ddd*history_save_error*helpString:	\
@rm The command history could not be saved.

Ddd*options_save_error_popup.title: DDD: Save Options Failed
Ddd*options_save_error*helpString:	\
@rm The DDD options could not be saved.

define(COULD_NOT_BE_UNCOMPRESSED,
[could not be uncompressed.\n\
\n\
Please verify the contents of the \
LBL(Edit) | LBL(Preferences) | LBL(Helpers) |\n\
LBL(Uncompress) resource and make sure that the given\n\
uncompressing program is within your executable path.])dnl

Ddd*no_license_error_popup.title: DDD: No License
Ddd*no_license_error*helpString:	\
@rm The DDD license COULD_NOT_BE_UNCOMPRESSED

Ddd*no_news_error_popup.title: DDD: No news is good news
Ddd*no_news_error*helpString:	\
@rm The DDD news COULD_NOT_BE_UNCOMPRESSED

Ddd*no_ddd_manual_error_popup.title: DDD: No Manual
Ddd*no_ddd_manual_error*helpString:	\
@rm The DDD manual COULD_NOT_BE_UNCOMPRESSED

Ddd*startup_warning_popup.title: DDD: New Startup Required
Ddd*startup_warning*helpString: \
@rm This setting cannot be changed while DDD is running.\n\
Please save options and restart DDD to see the effects.

Ddd*print_warning_popup.title: DDD: Print Command Output
Ddd*print_warning*helpString:	\
@rm This is the output of your printing command.\n\
For more details, consult your system documentation.

Ddd*no_display_number_warning_popup.title: DDD: No Display Number
Ddd*no_display_number_warning*helpString: \
@rm DDD could not determine the number of the new display\n\
as assigned by @GDB@.  DDD will use a default number instead.

Ddd*invalid_range_error_popup.title: DDD: Invalid Range
Ddd*invalid_range_error*helpString: \
@rm You entered an invalid range VAR(x)..VAR(y), with VAR(x) > VAR(y).

Ddd*paper_size_value_error_popup.title: DDD: Unrecognized Paper size
Ddd*paper_size_value_error*helpString:	\
@rm DDD could not parse your paper size specification.\n\
Please verify the paper size and try again.

Ddd*paper_size_unit_error_popup.title: DDD: Unrecognized Paper size unit
Ddd*paper_size_unit_error*helpString:	\
@rm DDD did not recognize your paper size unit.\n\
Recognized units include:\n\
KBD(pt) (points), KBD(in) (inches), \n\
KBD(mm) (millimeters), KBD(cm) (centimeters).
Please verify the paper size unit and try again.

Ddd*paper_size_x_error_popup.title: DDD: Unrecognized Paper Size
Ddd*paper_size_x_error*helpString:	\
@rm DDD could not find an SAMP(x) in your paper size specification.\n\
Please verify the paper size and try again.

Ddd*expired_warning_popup.title: DDD: DDD Expired
Ddd*expired_warning*helpString: \
@rm DDD @VERSION@ has been superseded by a newer DDD version.\n\
DDD @VERSION@ should no longer be used.\n\
To get the most recent DDD version, see the LBL(Help) | LBL([DDD] WWW Page).

Ddd*core_missing_warning_popup.title: DDD: Core Missing
Ddd*core_missing_warning*helpString: \
@rm DDD could not get a core dump of the debugged program.\n\
This means that the current memory contents and execution position\n\
will be lost when restoring the session.\n\
\n\
Please verify the LBL(Edit) | LBL(Preferences) | LBL(Helpers) | \
LBL(Get Core File) settings;\n\
try again without helper if appropriate.\n\
\n\
Also be sure that you have not disabled core dumps\n\
(hint: look for SAMP(limit) and SAMP(ulimit) commands \
in your shell startup file)

Ddd*program_name_missing_warning_popup.title: DDD: Program Name Missing
Ddd*program_name_missing_warning*helpString: \
@rm DDD could not determine the name of the current debuggee.\n\
Please try again when @GDB@ is ready.

Ddd*breakpoint_missing_warning_popup.title: DDD: Breakpoints Missing
Ddd*breakpoint_missing_warning*helpString: \
@rm DDD could not save the current breakpoint state.\n\
Please try again when @GDB@ is ready.

Ddd*displays_missing_warning_popup.title: DDD: Data Displays Missing
Ddd*displays_missing_warning*helpString: \
@rm DDD could not save the entire data display state.\n\
Please try again when @GDB@ is ready.

Ddd*no_sessions_error_popup.title: DDD: No Sessions
Ddd*no_sessions_error*helpString: \
@rm DDD could not access the session repository.

Ddd*delete_session_error_popup.title: DDD: Delete Session Failed
Ddd*delete_session_error*helpString: \
@rm DDD could not entirely remove the session directory from the repository.

Ddd*too_many_shortcuts_warning_popup.title: DDD: Too many Shortcuts
Ddd*too_many_shortcuts_warning*helpString:	\
@rm DDD has not enough room for all display shortcuts and will\n\
only display the most recently created shortcuts.\n\
To edit the list of display shortcuts, use LBL(New Display) | LBL(Edit).

Ddd*gdb_io_error_popup.title: DDD: I/O Error
Ddd*gdb_io_error*helpString: \
@rm The communication between GDB and DDD does not work correctly.\n\
This is probably due to a bad DDD configuration.\n\
\n\
DDD cannot work around this problem, so you may not be able to continue.

Ddd*gdb_io_warning_popup.title: DDD: I/O Warning
Ddd*gdb_io_warning*helpString: \
@rm The communication between GDB and DDD does not work correctly.\n\
This is probably due to a bad DDD configuration.\n\
\n\
DDD can work around this problem, so you may continue working.

Ddd*gdb_echo_warning_popup.title: DDD: Echo mode Active
Ddd*gdb_echo_warning*helpString: \
@rm The communication between GDB and DDD does not work correctly - \n\
GDB echoes back every character typed by DDD.\n\
This is probably due to a bad DDD configuration.\n\
\n\
DDD can work around this problem, so you can continue working.

Ddd*out_of_glyphs_warning_popup.title: DDD: Out of Glyphs
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


!-----------------------------------------------------------------------------
! Hints on specific DDD states (`What now')
!-----------------------------------------------------------------------------

Ddd*no_program.helpString: \
@rm You have not opened any program yet.  @GDB@ needs a debuggee\n\
to access core dumps, source files, or attaching to processes.\n\
\n\
To open a program, use LBL(File) | LBL(Open Program).

define(NO_SOURCE_HELP,
[ITEM In order to debug a program effectively, you need to generate\n\
    debugging information when you compile it.  Without debugging \
information,\n\
    @GDB@ will be unable to locate the source code.  To request debugging\n\
    information, specify the SAMP(-g) option when you run the compiler.\n\
ITEM You may need to tell @GDB@ where the source code files are.\n\
    Invoke LBL(Edit) | LBL(@GDB@ Settings) and look for appropriate entries.\n\
ITEM Using GDB, you can continue at machine code level \
by enabling the\n\
    Machine Code Window.  Use LBL(Source) | LBL(Display Machine Code).])dnl

Ddd*no_source_and_no_code.helpString: \
@rm @GDB@ cannot find the source code of your program.\n\
\n\
Here are some hints that may help you out:\n\
NO_SOURCE_HELP

Ddd*program_not_running.helpString: \
@rm Your program is not running.\n\
\n\
You may now choose between the following:\n\
ITEM Start the program, using LBL(Program) | LBL(Run). \
You may wish to \n\
    set some breakpoints beforehand, using LBL(Break at ()) and \n\
    LBL(Source) | LBL(Edit Breakpoints).\n\
    To find specific functions, use LBL(Lookup ()).\n\
ITEM If your program is already running outside of DDD, you can\n\
    attach to its running process, using LBL(File) | LBL(Attach to Process).\n\
ITEM If some previous invocation of your program has failed, and\n\
    you want to know why, use LBL(File) | LBL(Open Core Dump) to open\n\
    its core dump.

Ddd*item_selected.helpString: \
@rm You have selected some item from the program source code.\n\
\n\
ITEM To look up further occurences in the source code, use LBL_FIND_PREV\n\
    and LBL_FIND_NEXT.  LBL(Lookup ()) leads you to the item's definition.\n\
ITEM To display the value, use LBL(Print ()) (for simple values)\n\
    and LBL(Display ()) (for complex values and dynamic displays).

Ddd*display_selected.helpString: \
@rm You have selected some data display.\n\
\n\
ITEM To examine further members of this data structure, use \
LBL(New Display ())\n\
    and LBL(Display *()).\n\
ITEM To modify the appearance of the selected display, use \
LBL(Hide ()) and LBL(Rotate ()).\n\
ITEM To change the value of the selected display, use LBL(Set ()).\n\
ITEM To delete the selected displays, use LBL(Delete ()).\n\
ITEM Use LBL(Data) | LBL(Edit Displays) for further editing.

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
ITEM Use LBL(Status) | LBL(Up) to step out of libraries \
and to enter your own functions.\n\
NO_SOURCE_HELP

define(STOPPED_HELP,
[ITEM To find out how you got here, use LBL(Status) | LBL(Backtrace).\n\
ITEM To examine a variable value, select it first (click on an \
occurrence).\n\
    Then use LBL(Print ()) (for simple values) and LBL(Display ()) \
(for complex\n\
    values).  As a shortcut, you may also press BUTTON(3)\n\
    to get a popup menu.\n\
ITEM You can display all local variables at once using \
LBL(Data) | LBL(Display\n\
    Local Variables).\n\
ITEM To set and clear further breakpoints, use \
LBL(Source) | LBL(Edit Breakpoints).\n\
    As a shortcut, you may also press BUTTON(3) on a\n\
    location or breakpoint to get a popup menu.])dnl

Ddd*stopped_at_passed_signal.helpString: \
@rm Your program @PROGRAM_STATE@.\n\
\n\
STOPPED_HELP\n\
\n\
When resuming execution, the @SIGNAL_DESCRIPTION@ signal\n\
will be passed to the program.\n\
ITEM To continue execution without giving a @SIGNAL@ signal,\n\
    use the @GDB@ command KBD(signal 0).\n\
ITEM To see the current signal handling, \n\
    use the @GDB@ command KBD(info handle).

Ddd*stopped_at_ignored_signal.helpString: \
@rm Your program @PROGRAM_STATE@.\n\
\n\
STOPPED_HELP\n\
\n\
When resuming execution, the @SIGNAL_DESCRIPTION@ signal \
will EMPH(not) be passed to the program.\n\
ITEM To pass this signal to the program, \
use the @GDB@ command KBD(signal @SIGNAL@).\n\
ITEM To see the current signal handling, \
use the @GDB@ command KBD(info handle).

Ddd*stopped.helpString: \
@rm Your program @PROGRAM_STATE@.\n\
\n\
STOPPED_HELP\n\
ITEM Use the functions of the LBL(Program) menu to resume execution\n\
    and step through your program.
