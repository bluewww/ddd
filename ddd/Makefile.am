## $Id$
## DDD Makefile template.
## Process this file with `automake' to produce a `Makefile.in'.

# $Id$
# DDD Makefile.

# Copyright (C) 2000-2001 Universitaet Passau, Germany.
# Written by Andreas Zeller <zeller@gnu.org>.
#
# Copyright (C) 2001-2004, 2006 Free Software Foundation, USA.
# 
# This file is part of DDD.
# 
# DDD is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
# 
# DDD is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with DDD -- see the file COPYING.
# If not, see <http://www.gnu.org/licenses/>.
# 
# DDD is the data display debugger.
# For details, see the DDD World-Wide-Web page, 
# `http://www.gnu.org/software/ddd/',
# or send a mail to the DDD developers <ddd@gnu.org>.

# -----------------------------------------------------------------------------
# General Settings.
# -----------------------------------------------------------------------------

# Various utilities.
CAT  = cat
MV   = mv
ECHO = echo
GREP = grep
DATE = date
CMP  = cmp
RM   = rm -f

# Suffixes used in this file.
SUFFIXES = .C .c .Y .L .txt .xbm .xbmxx .xpm .xpmxx \
	   .xface .fig .eps .jpg .png .gif .pdf $(IMGEXT) $(IMGEXT2)

# Shared data installation
dddrootdir    = $(pkgdatadir)-$(VERSION)
dddappdir     = $(pkgdatadir)-$(VERSION)/$(ddd)


# -----------------------------------------------------------------------------
# DDD Sources.
# -----------------------------------------------------------------------------

# General-purpose utilities: strings, regexps, run-type type info...
BASE_SRC = \
	assert.h     \
	assert.C     \
	attribute.h  \
	basename.C   \
	basename.h   \
	bool.h       \
	casts.h      \
	compare.C    \
	compare.h    \
	cook.C       \
	cook.h       \
	cwd.C        \
	cwd.h        \
	ddd.h        \
	explicit.h   \
	getrlimit.h  \
	glob.C       \
	glob.h       \
	hash.C       \
	hash.h       \
	home.C       \
	home.h       \
	hypot.h      \
	isid.C       \
	isid.h       \
	misc.h       \
	mutable.h    \
	my-alloca.h  \
	my-dirent.h  \
	oldcxx.h     \
	pi.h         \
	rxclass.C    \
	rxclass.h    \
	sigName.C    \
	sigName.h    \
	strclass.C   \
	strclass.h   \
	streampos.h  \
	strtoul.h    \
	tabs.C       \
	tabs.h       \
	uniquify.C   \
	uniquify.h   \
	Logger.C     \
	Logger.h     \
	MemCheck.C   \
	MemCheck.h   \
	MemCheckD.h  \
	PrintGC.C    \
	PrintGC.h    \
	SignalB.C    \
	SignalB.h    \
	TypeInfo.C   \
	TypeInfo.h   \
	UniqueId.C   \
	UniqueId.h

# Instantiated templates
TEMPLATE_SRC = \
	Assoc.h      \
	IntIntAA.h   \
	Queue.h      \
	StatArray.h  \
	StringA.h    \
	StringMap.h  \
	StringSA.h   \
	StringSAA.h  \
	TextPosA.h   \
	VarArray.h   \
	VoidArray.h  \
	WidgetSA.h

# Interprocess communication
# Top-level class is `Agent', declared in `Agent.h'
AGENT_SRC = \
	Agent.C      \
	Agent.h      \
	AgentM.C     \
	AgentM.h     \
	AsyncAgent.C \
	AsyncAgent.h \
	ChunkQueue.C \
	ChunkQueue.h \
	DataLength.C \
	DataLength.h \
	HandlerL.C   \
	HandlerL.h   \
	LiterateA.C  \
	LiterateA.h  \
	ReadLineA.C  \
	ReadLineA.h  \
	TimeOut.C    \
	TimeOut.h    \
	TTYAgent.C   \
	TTYAgent.h

# Miscellaneous utilities for X programs: Motif strings, hourglasses,
# context-sensitive help...
XSTUFF_SRC = \
	charsets.C   \
	charsets.h   \
	converters.C \
	converters.h \
	events.C     \
	events.h     \
	findParent.C \
	findParent.h \
	findWindow.C \
	findWindow.h \
	fileSBF.C    \
	fileSBF.h    \
	frame.C      \
	frame.h      \
	longName.C   \
	longName.h   \
	selection.C  \
	selection.h  \
	verify.C     \
	verify.h     \
	AutoRaise.C  \
	AutoRaise.h  \
	Delay.C      \
	Delay.h      \
	DeleteWCB.C  \
	DeleteWCB.h  \
	DestroyCB.C  \
	DestroyCB.h  \
	ExitCB.C     \
	ExitCB.h     \
	InitImage.C  \
	InitImage.h  \
	OnOff.h      \
	Sash.h       \
	Swallower.C  \
	Swallower.h  \
	SwallowerP.h \
	Widget.h     \
	Xpm.h        \
	XErrorB.C    \
	XErrorB.h

# Same, but for Motif
MSTUFF_SRC = \
	freeXmST.C   \
	freeXmST.h   \
	LabelH.C     \
	LabelH.h     \
	LabelHP.h    \
	LessTifH.C   \
	LessTifH.h   \
	MakeMenu.C   \
	MakeMenu.h   \
	MString.C    \
	MString.h    \
	ComboBox.C   \
	ComboBox.h   \
	SpinBox.C    \
	SpinBox.h    \
	TextSetS.C   \
	TextSetS.h

# Box library - rectangular composible graphical items
# Top-level class is `Box', declared in `Box.h'
BOX_SRC = \
	eps-code.h   \
	printBox.C   \
	printBox.h   \
	AlignBox.C   \
	AlignBox.h   \
	ArcBox.C     \
	ArcBox.h     \
	BinBox.C     \
	BinBox.h     \
	Box.C        \
	Box.h        \
	BoxC.h       \
	BoxD.C       \
	BoxD.h       \
	BoxExtend.C  \
	BoxExtend.h  \
	BoxPoint.C   \
	BoxPoint.h   \
	BoxRegion.C  \
	BoxRegion.h  \
	BoxSize.C    \
	BoxSize.h    \
	BoxWeight.h  \
	ColorBox.C   \
	ColorBox.h   \
	CompositeB.C \
	CompositeB.h \
	DataLink.h   \
	DiagBox.C    \
	DiagBox.h    \
	DummyBox.C   \
	DummyBox.h   \
	FixBox.C     \
	FixBox.h     \
	FontFixBox.C \
	FontFixBox.h \
	FontTable.C  \
	FontTable.h  \
	HatBox.C     \
	HatBox.h     \
	LineBox.C    \
	LineBox.h    \
	ListBox.C    \
	ListBox.h    \
	MarkBox.C    \
	MarkBox.h    \
	MatchBox.C   \
	MatchBox.h   \
	PrimitiveB.C \
	PrimitiveB.h \
	SlopeBox.C   \
	SlopeBox.h   \
	StringBox.C  \
	StringBox.h  \
	THatBox.C    \
	THatBox.h    \
	TagBox.C     \
	TagBox.h     \
	TrueBox.C    \
	TrueBox.h

# VSL library - building boxes from structured data
# Top-level class is `VSLLib', declared in `VSLLib.h'
VSL_SRC = \
	ArgNode.C    \
	ArgNode.h    \
	BuiltinCN.C  \
	BuiltinCN.h  \
	CallNode.C   \
	CallNode.h   \
	ConstNode.C  \
	ConstNode.h  \
	DefCallN.C   \
	DefCallN.h   \
	DummyNode.C  \
	DummyNode.h  \
	LetNode.C    \
	LetNode.h    \
	NameNode.C   \
	NameNode.h   \
	ListNode.C   \
	ListNode.h   \
	TestNode.C   \
	TestNode.h   \
	ThemeVSLL.C  \
	ThemeVSLL.h  \
	TrueNode.C   \
	TrueNode.h   \
	VSEFlags.C   \
	VSEFlags.h   \
	VSLArgList.C \
	VSLArgList.h \
	VSLBuiltin.C \
	VSLBuiltin.h \
	VSLDef.C     \
	VSLDef.h     \
	VSLDefList.C \
	VSLDefList.h \
        VSLErr.C     \
	VSLLib.C     \
	VSLLib.h     \
	VSLNode.C    \
	VSLNode.h    \
        VSLRead.C

# Graph structure, graph layout and graph editor widget
# Top-level classes are `Graph', declared in `Graph.h'
# and the `GraphEdit' widget, declared in `GraphEdit.h'.
GRAPH_SRC = \
	ArcGraphE.C  \
	ArcGraphE.h  \
	BoxEdgeA.C   \
	BoxEdgeA.h   \
	BoxGraphN.C  \
	BoxGraphN.h  \
	EdgeA.C      \
	EdgeA.h      \
	EdgeAPA.h    \
	Graph.C      \
	Graph.h      \
	GraphEdge.C  \
	GraphEdge.h  \
	GraphEdit.C  \
	GraphEdit.h  \
	GraphEditP.h \
	GraphGC.C    \
	GraphGC.h    \
	GraphNode.C  \
	GraphNode.h  \
	GraphNodeI.h \
	GraphNPA.h   \
	HintGraphN.C \
	HintGraphN.h \
	LineGraphE.C \
	LineGraphE.h \
	LineGESI.C   \
	LineGESI.h   \
	PannedGE.C   \
	PannedGE.h   \
	PannedGEP.h  \
	PosGraphN.C  \
	PosGraphN.h  \
	RegionGN.C   \
	RegionGN.h   \
	ScrolledGE.C \
	ScrolledGE.h \
	ScrolleGEP.h \
	layout.C     \
	layout.h

# The DDD application itself.  `ddd.C' is the main program, `SourceView.C'
# is the source window, `DataDisp.C' is the data window.
DDD_SRC = \
	annotation.C \
	annotation.h \
	args.C       \
	args.h       \
	buttons.C    \
	buttons.h    \
	cmdtty.C     \
	cmdtty.h     \
	comm-manag.C \
	comm-manag.h \
	complete.C   \
	complete.h   \
	configinfo.h \
	ctrl.C       \
	ctrl.h       \
	data.C       \
	data.h       \
	dbx-lookup.C \
	dbx-lookup.h \
	ddd.vsl.h    \
	decoration.C \
	decoration.h \
	deref.C      \
	deref.h      \
	disp-read.C  \
	disp-read.h  \
	editing.C    \
	editing.h    \
	environ.C    \
	environ.h    \
	examine.C    \
	examine.h    \
	exectty.C    \
	exectty.h    \
	exit.C       \
	exit.h       \
	expired.C    \
	expired.h    \
	file.C       \
	file.h       \
	filetype.C   \
	filetype.h   \
	fonts.C      \
	fonts.h      \
	fortranize.C \
	fortranize.h \
	gdbinit.C    \
	gdbinit.h    \
	history.C    \
	history.h    \
	hostname.C   \
	hostname.h   \
	index.C      \
	index.h      \
	java.C 	     \
	java.h 	     \
	logo.C       \
	logo.h       \
	logplayer.C  \
	logplayer.h  \
	mainloop.C   \
	mainloop.h   \
	mydialogs.C  \
	mydialogs.h  \
	options.C    \
	options.h    \
	plotter.C    \
	plotter.h    \
	post.C       \
	post.h       \
	print.C      \
	print.h      \
	question.C   \
	question.h   \
	regexps.C    \
	regexps.h    \
	resolveP.C   \
	resolveP.h   \
	resources.C  \
	resources.h  \
	sashes.C     \
	sashes.h     \
	select.C     \
	select.h     \
	session.C    \
	session.h    \
	settings.C   \
	settings.h   \
	shell.C      \
	shell.h      \
	shorten.C    \
	shorten.h    \
	show.C       \
	show.h       \
	simpleMenu.C \
	simpleMenu.h \
	source.C     \
	source.h     \
	status.C     \
	status.h     \
	string-fun.C \
	string-fun.h \
	stty.h       \
	tempfile.h   \
	tempfile.C   \
	tictactoe.C  \
	tictactoe.h  \
	tips.C       \
	tips.h       \
	toolbar.C    \
	toolbar.h    \
	ungrab.C     \
	ungrab.h     \
	value-read.C \
	value-read.h \
	vsldoc.C     \
	vsldoc.h     \
	windows.C    \
	windows.h    \
	wm.C         \
	wm.h         \
	xconfig.C    \
	xconfig.h    \
	AliasGE.C    \
	AliasGE.h    \
	AppData.h    \
	ArgField.C   \
	ArgField.h   \
	BindingS.h   \
	BreakPoint.C \
	BreakPoint.h \
	BreakPM.h    \
	CodeCache.C  \
	CodeCache.h  \
	Command.C    \
	Command.h    \
	DataDisp.C   \
	DataDisp.h   \
	DispBox.C    \
	DispBox.h    \
	DispBuffer.C \
	DispBuffer.h \
	DispGraph.C  \
	DispGraph.h  \
	DispNode.C   \
	DispNode.h   \
	DispNodeM.h  \
	DispValue.C  \
	DispValue.h  \
	DispValueA.h \
	DispValueT.C \
	DispValueT.h \
	DynArray.h   \
	EntryType.h  \
	GDBAgent.C   \
	GDBAgent.h   \
	HelpCB.C     \
	HelpCB.h     \
	HistoryD.C   \
	HistoryD.h   \
	HistoryF.h   \
	IntArray.h   \
	Map.h        \
	MinMaxA.h    \
	PlotAgent.C  \
	PlotAgent.h  \
	PlotArea.C   \
	PlotArea.h   \
	PosBuffer.C  \
	PosBuffer.h  \
	ProgressM.C  \
	ProgressM.h  \
	RefreshDI.C  \
	RefreshDI.h  \
	SmartC.C     \
	SmartC.h     \
	SourceO.h    \
	SourceView.C \
	SourceView.h \
	StringTPA.h  \
	ThemeM.C     \
	ThemeM.h     \
	ThemeP.C     \
	ThemeP.h     \
	Tool.C       \
	Tool.h       \
	UndoBuffer.C \
	UndoBuffer.h \
	UndoBE.C     \
	UndoBE.h     \
	WhatNextCB.C \
	WhatNextCB.h

# Automatically generated source files which should not be distributed.
nodist_ddd_SOURCES = \
	configinfo.C \
	build.h      \
	host.h       \
	root.h       \
	version.h    \
	Ddd.ad.h

# Make sure needed headers are built before trying to compile these files
# (Assume ddd.C is the first source file to be built.)
ddd.o: version.h host.h root.h
resources.o: Ddd.ad.h
show.o: build.h

# -----------------------------------------------------------------------------
# Local Libraries.
# -----------------------------------------------------------------------------

# POSIX regex routines (from system)
# LIBRX      = @LIBRX@
LIBRX      =
# termcap routines (from system)
LIBTERMCAP = @LIBTERMCAP@
# -lreadline library (from system)
LIBREADLINE = @LIBREADLINE@
# Yacc library (-ly)
LIBY       = @LIBY@
# Math library (-lm)
LIBM       = @LIBM@
# C library (-lc)
LIBC       = @LIBC@
# Socket library (-lsocket)
LIBSOCKET  = @LIBSOCKET@
# ELF library (-lelf)
LIBELF     = @LIBELF@
# NSL library (-lnsl)
LIBNSL     = @LIBNSL@
# Additional libraries required for non-X programs (-lsocket -lnsl)
NON_X_LIBS = @LIBS@

# All libraries shown above
ALL_LIBS = $(CXXLIBS) $(LIBREADLINE) $(LIBSOCKET) $(LIBNSL) $(LIBRX) \
	$(LIBTERMCAP) $(LIBY) $(LIBM) $(LIBELF)



# -----------------------------------------------------------------------------
# X Libraries.
# -----------------------------------------------------------------------------

# Special flags for linking with X.  (-L/usr/X11R5/lib)
X_LDFLAGS = @X_LIBS@

# Special flags to set the run-time library search path.
RPATH = @RPATH@

# Motif library.  (-lXm)
LIBXM = @XM_LIBS@

# Use this alternative for building `semistatic' executables
# where Motif libraries are statically linked in.
# LIBXM = -Wl,-Bstatic @XM_LIBS@ -Wl,-Bdynamic
# Libtool has a bug and moves -Wl,xxx before any other link flags.
# The following workaround should work:
# LIBXM = -Wl,$(X_LDFLAGS) -Wl,-Bstatic -Wl,@XM_LIBS@ -Wl,-Bdynamic
# Alternatively, just define (with the proper path)
# LIBXM = /path/to/libXm.a
# This can be done interactively: "make LIBXM=/path/to/libXm.a"

# Athena libraries.  (-lXaw)
LIBXAW = @XAW_LIBS@

# X Miscellaneous  Utilities needed for Athena and Motif >= 2.0 (lXmu)
LIBXMU = @XMU_LIBS@

# X extension library; needed for Athena and Motif >= 2.0.  (-lXext)
LIBXEXT = @XEXT_LIBS@

# Xpm pixmap library; needed for DDD and sometimes for Motif >= 2.0.  (-lXpm)
LIBXPM = @XPM_LIBS@

# Xp printing extension library; sometimes needed for Motif >= 2.1.  (-lXp)
LIBXP = @XP_LIBS@

# gen library; sometimes needed for Motif >= 2.0.  (-lgen)
LIBGEN = @LIBGEN@

# X toolkit library.  (-lXt)
LIBXT = @XT_LIBS@

# X library.  (-lX11 -lSM -lICE -lnsl -lsocket)
# libSM: Session Management extension
# libICE: Inter-Client Exchange extension
LIBX11 = @X_PRE_LIBS@ @X11_LIBS@ @X_EXTRA_LIBS@

# All libraries shown above
ALL_X_LIBS = $(X_LDFLAGS) $(LIBXM) $(LIBXAW) $(LIBXMU) $(LIBXT) $(LIBXPM) \
             $(LIBXP) $(LIBXEXT) $(LIBX11) $(LIBGEN)


# -----------------------------------------------------------------------------
# Building DDD (and others).
# -----------------------------------------------------------------------------

# The flags we use to compile C++ files.
# Optimizing flags. (-DNDEBUG -O2)
CXXOPT = @CXXOPT@
# Debugging flags.  (-g)
CXXDEBUG = @CXXDEBUG@
# Warnings.  (-W -Wall)
CXXWARNINGS = @CXXWARNINGS@
# Extras.  (-trigraphs)
CXXSTUFF = @CXXSTUFF@ @X_PERMISSIVE@
CXXFLAGS = $(CXXOPT) $(CXXDEBUG) $(CXXWARNINGS) $(CXXSTUFF)

bin_PROGRAMS = ddd
noinst_PROGRAMS = stringify userinfo ctest cxxtest vsl
ddd_SOURCES = ddd.C \
	$(BASE_SRC) \
	$(TEMPLATE_SRC) \
	$(AGENT_SRC) \
	$(XSTUFF_SRC) \
	$(MSTUFF_SRC) \
	$(BOX_SRC) \
	$(VSL_SRC) \
	$(GRAPH_SRC) \
	$(DDD_SRC)

stringify_SOURCES = stringify.C
userinfo_SOURCES  = userinfo.C
ctest_SOURCES     = ctest.c
cxxtest_SOURCES   = cxxtest.C
vsl_SOURCES       = vsl.C \
	DocSpace.c \
	DocSpace.h \
	DocSpaceP.h \
	$(BASE_SRC) \
	$(TEMPLATE_SRC) \
	$(AGENT_SRC) \
	$(XSTUFF_SRC) \
	$(MSTUFF_SRC) \
	$(BOX_SRC) \
	$(VSL_SRC)

cxxtest_LDADD = $(LIBM)
ddd_LDADD = $(RPATH) $(ALL_X_LIBS) $(ALL_LIBS)
vsl_LDADD = $(RPATH) $(ALL_X_LIBS) $(ALL_LIBS)
X_INCLUDE = @X_CFLAGS@
AM_CPPFLAGS = -I$(srcdir)/.. $(X_INCLUDE)

# -----------------------------------------------------------------------------
# DDD Application Defaults.
# -----------------------------------------------------------------------------

# The resource header and app-default files.

Ddd.ad.h: Ddd $(srcdir)/ad2c.sed
	$(CAT) Ddd | $(DELETE_CR) | \
	$(SED) -n -f $(srcdir)/ad2c.sed > $@~ && $(MV) $@~ $@

$(srcdir)/Ddd.in: $(srcdir)/Ddd.in.m4 $(srcdir)/tips.m4 \
	$(srcdir)/macros.m4 $(srcdir)/colors.m4 
	( \
	( \
		echo "define(srcdir, \`$(srcdir)')dnl"; \
		echo "include(srcdir()/Ddd.in.m4)dnl" \
	) | $(M4); \
        ( \
		echo "define(srcdir, \`$(srcdir)')dnl"; \
		echo "include(srcdir()/tips.m4)dnl" \
	) | $(M4); \
	) | $(DELETE_CR) > $@~ && $(MV) $@~ $@

$(srcdir)/tips.m4: $(srcdir)/../TIPS $(srcdir)/tips2m4.awk
	$(AWK) -f $(srcdir)/tips2m4.awk $(srcdir)/../TIPS | $(DELETE_CR) \
	    > $@~ && $(MV) $@~ $@

$(srcdir)/tips.html: $(srcdir)/../TIPS
	( \
		echo "define(srcdir, \`$(srcdir)')dnl"; \
		echo "define(macros, \`$(srcdir)/html.m4')dnl"; \
		echo "include(srcdir()/../TIPS)dnl" \
	) | $(M4) | $(AWK) -f $(srcdir)/tips2html.awk | \
	$(SED) 's/\\133/\[/g' | $(SED) 's/\\135/\]/g' | \
	$(SED) 's/@\([A-Z][A-Z]*\)@/\1/g' | $(SED) 's/@ /@/g' \
	| $(DELETE_CR) > $@~ && $(MV) $@~ $@



# -----------------------------------------------------------------------------
# VSL.
# -----------------------------------------------------------------------------

# The VSL header files.

vsldir  = $(srcdir)/../vsllib

VSL_CPP = $(CPP) -I$(vsldir) -I$(prefix)/lib/vsl-include

$(vsldir)/ddd.vsl: $(srcdir)/ddd.vsl.m4 $(srcdir)/colors.m4
	( \
		echo "define(srcdir, \`$(srcdir)')dnl"; \
		echo "include(srcdir()/ddd.vsl.m4)dnl" \
	) | $(M4) | $(DELETE_CR) > $@~ && $(MV) $@~ $@

VSLSRC = \
	$(vsldir)/std.vsl \
	$(vsldir)/builtin.vsl \
	$(vsldir)/tab.vsl \
	$(vsldir)/fonts.vsl \
	$(vsldir)/colors.vsl \
	$(vsldir)/list.vsl

$(srcdir)/ddd.vsl.h: $(vsldir)/ddd.vsl $(srcdir)/vsl2c.sed $(VSLSRC)
	$(CAT) $(vsldir)/ddd.vsl > tmp.c
	$(VSL_CPP) tmp.c | $(SED) -f $(srcdir)/vsl2c.sed | $(DELETE_CR) > $@
	$(RM) tmp.c

.PHONY: vsllib
vsllib: $(vsldir)/ddd.vsl $(VSLSRC)


# -----------------------------------------------------------------------------
# Lex & Yacc.
# -----------------------------------------------------------------------------

# YACC commands.
YY = yy
YACCTOCXX = $(srcdir)/yacctoC

# LEX commands.
LEXTOCXX = $(srcdir)/lextoC

# The lex output root (lex.yy)
LEX_OUTPUT_ROOT = @LEX_OUTPUT_ROOT@

# VSL parsers.
$(srcdir)/vsl-lex.C: $(srcdir)/vsl-lex.L
	$(LEX) $(LFLAGS) $(srcdir)/vsl-lex.L
	$(SHELL) $(LEXTOCXX) -prefix vsl
	$(MV) $(LEX_OUTPUT_ROOT).cc $@
	$(RM) $(LEX_OUTPUT_ROOT).c

$(srcdir)/vsl-gramma.h $(srcdir)/vsl-gramma.C: $(srcdir)/vsl-gramma.Y \
	$(YACCTOCXX) $(YACCTOCXX).bison $(YACCTOCXX).h $(YACCTOCXX).sun
	$(YACC) $(YFLAGS) -d $(srcdir)/vsl-gramma.Y
	$(SHELL) $(YACCTOCXX) -prefix vsl
	$(MV) y.tab.hh $(srcdir)/vsl-gramma.h
	$(MV) y.tab.cc $(srcdir)/vsl-gramma.C
	$(RM) y.tab.c y.tab.h

VSLRead.$(OBJEXT): \
	$(srcdir)/vsl-lex.C \
	$(srcdir)/vsl-gramma.h \
	$(srcdir)/vsl-gramma.C

# DDD regexp parsers.
rxscan.C: $(srcdir)/rxscan.L
	$(LEX) $(LFLAGS) $(srcdir)/rxscan.L
	$(SHELL) $(LEXTOCXX) -prefix "ddd"
	$(MV) $(LEX_OUTPUT_ROOT).cc $@
	$(RM) $(LEX_OUTPUT_ROOT).c

regexps.$(OBJEXT): rxscan.C



# -----------------------------------------------------------------------------
# Text files compiled into DDD.
# -----------------------------------------------------------------------------

# Script rules
.txt.C:
	@$(ECHO) "Creating $@"
	@( \
	$(ECHO) '// $@ -- generated automatically from `$<'"'."; \
	$(SED) '/^ *#/d' $< | \
	$(SED) 's/^ *include \([^"][^.]*\)\..*$$/#include "\1.C"/' | \
	$(SED) 's/^ *include "\([^"][^.]*\)[^"]*"/#include "\1.C"/' | \
	$(SED) '/^[^#]/s/\(["'"'"'\\]\)/\\\1/g' | \
	$(SED) '/^[^#]/s/.*/"&\\n"/' | \
	$(SED) 's//\\b/g' | \
	$(SED) 's/^$$/"\\n"/' \
	) > tmp.C; \
	$(CPP) -C -P -undef $(CPPFLAGS) $(DEFS) $(INCLUDE) tmp.C | \
	    $(DELETE_CR) > $@; \
	$(RM) tmp.C

# Reference.
# ddd.info.txt does NOT depend on ddd.info since we don't want it to get
# re-created after each configuration.
$(srcdir)/ddd.info.txt: $(srcdir)/ddd.texi $(srcdir)/../configure.ac
	$(MAKE) ddd.info
	info --subnodes -o - -f $(srcdir)/ddd.info | $(DELETE_CR) > $@~ \
	    && $(MV) $@~ $@

$(srcdir)/ddd.info.txt.gz: $(srcdir)/ddd.info.txt
	$(COMPRESS) $? > $@~ && $(MV) $@~ $@

$(srcdir)/ddd.info.txt.gz.C: $(srcdir)/ddd.info.txt.gz
	$(MAKE) stringify$(EXEEXT)
	-./stringify < $? > $@~ && $(MV) $@~ $@

# License.
$(srcdir)/COPYING.gz: $(srcdir)/../COPYING
	$(COMPRESS) $? > $@

$(srcdir)/COPYING.gz.C: $(srcdir)/COPYING.gz
	$(MAKE) stringify$(EXEEXT)
	-./stringify < $? > $@~ && $(MV) $@~ $@

# News.
$(srcdir)/NEWS.gz: $(srcdir)/../NEWS
	$(COMPRESS) $? > $@~ && $(MV) $@~ $@

$(srcdir)/NEWS.gz.C: $(srcdir)/NEWS.gz
	$(MAKE) stringify$(EXEEXT)
	-./stringify < $? > $@~ && $(MV) $@~ $@

show.$(OBJEXT): COPYING.gz.C NEWS.gz.C ddd.info.txt.gz.C


# Configuration info stuff
configinfo.C: USERINFO.txt $(srcdir)/config-info
	$(SHELL) $(srcdir)/config-info $(srcdir)/USERINFO.txt USERINFO.txt > $@~ && \
	$(MV) $@~ $@


# If `~/.ddd_userinfo' exists, use this file as personal identification.
USERINFO.txt: userinfo$(EXEEXT)
	-(cat $$HOME/.ddd_userinfo 2> /dev/null || \
	./userinfo || echo '(unknown)') > $@~ && $(MV) $@~ $@
	@$(ECHO) ""
	@$(ECHO) "Please check your name and e-mail address:"
	@$(ECHO) ""
	@$(CAT) $@
	@$(ECHO) ""
	@$(ECHO) "This information will be compiled into $(DDD)."
	@$(ECHO) "If it is not correct, please edit $@ and re-compile."
	@$(ECHO) ""


# EPS code stuff.
$(srcdir)/header.str: $(srcdir)/header.ps
	$(SED) 's/^.*$$/"&\\n"/' $(srcdir)/header.ps | $(DELETE_CR) \
	    > $@~ && $(MV) $@~ $@

$(srcdir)/trailer.str: $(srcdir)/trailer.ps
	$(SED) 's/^.*$$/"&\\n"/' $(srcdir)/trailer.ps | $(DELETE_CR) \
	    > $@~ && $(MV) $@~ $@

$(srcdir)/eps-code.h: \
	$(srcdir)/header.str $(srcdir)/trailer.str $(srcdir)/eps-code.x
	$(SED) -e '/prolog/r $(srcdir)/header.str' \
	       -e '/trailer/r $(srcdir)/trailer.str' $(srcdir)/eps-code.x \
	    | $(DELETE_CR) > $@~ && $(MV) $@~ $@


# -----------------------------------------------------------------------------
# Demos.
# -----------------------------------------------------------------------------

# `make check' commands.

# A debugger for debugging DDD.  (gdb)
DEBUGGER = @DEBUGGER@

# The options for DDD when invoking `make check'.  ()
DDDFLAGS = @DEBUGGER_OPTION@

# The program to debug
DDDTARGET = cxxtest$(EXEEXT)

# For demonstration and debugging purposes
.PHONY: check
check: Ddd ./$(ddd)$(EXEEXT) cxxtest$(EXEEXT) check-now

.PHONY: check-now
check-now:
	MALLOC_CHECK_=2 \
	DDD_HOME=$(srcdir)/.. \
	XUSERFILESEARCHPATH=%N XAPPLRESDIR=. \
	./$(ddd)$(EXEEXT) $(DDDFLAGS) $(DDDTARGET)

.PHONY: debug
debug: Ddd ./$(ddd)$(EXEEXT) cxxtest$(EXEEXT) .gdbinit debug-now

.PHONY: debug-now
debug-now:
	MALLOC_CHECK_=2 \
	DDD_HOME=$(srcdir)/.. \
	XUSERFILESEARCHPATH=%N XAPPLRESDIR=. \
	$(DEBUGGER) ./$(ddd)$(EXEEXT)

# We don't want no optimizing, but debugging info in our test files
MAKE_DEBUG_FLAGS = \
	CXXFLAGS="`$(ECHO) '$(CXXFLAGS)' | \
	$(SED) 's/-O[^ ]*//' | $(SED) 's/-DNDEBUG//'`" \
	CFLAGS="`$(ECHO) '$(CFLAGS)' | \
	$(SED) 's/-O[^ ]*//' | $(SED) 's/-DNDEBUG//'`"

cxxtest.$(OBJEXT): $(srcdir)/cxxtest.C
	@case '$(CXXFLAGS)' in \
	*-O*) $(MAKE) $(MAKE_DEBUG_FLAGS) $@;; \
	*) set -x; \
	   $(CXXCOMPILE) -c @CXXDEBUG@ $(srcdir)/cxxtest.C -o $@;; \
	esac

ctest.$(OBJEXT): $(srcdir)/ctest.c
	@case '$(CFLAGS)' in \
	*-O*) $(MAKE) $(MAKE_DEBUG_FLAGS) $@;; \
	*) set -x; \
	   $(COMPILE) -c -g $(srcdir)/ctest.c -o $@;; \
	esac

sample.$(OBJEXT): $(srcdir)/sample.c
	@case '$(CFLAGS)' in \
	*-O*) $(MAKE) $(MAKE_DEBUG_FLAGS) $@;; \
	*) set -x; \
	   $(COMPILE) -c -g $(srcdir)/sample.c -o $@;; \
	esac

grabtest.$(OBJEXT): $(srcdir)/grabtest.C
	@case '$(CXXFLAGS)' in \
	*-O*) $(MAKE) $(MAKE_DEBUG_FLAGS) $@;; \
	*) set -x; \
	   $(CXXCOMPILE) -c @CXXDEBUG@ $(srcdir)/grabtest.C -o $@;; \
	esac

bug.$(OBJEXT):	$(srcdir)/bug.C
	@case '$(CXXFLAGS)' in \
	*-O*) $(MAKE) $(MAKE_DEBUG_FLAGS) $@;; \
	*) set -x; \
	   $(CXXCOMPILE) -c @CXXDEBUG@ $(srcdir)/bug.C -o $@;; \
	esac



# -----------------------------------------------------------------------------
# Icons.
# -----------------------------------------------------------------------------

# Icon rules
.xpm.xbm:
	@$(ECHO) "Creating $@"
	@-base=`$(ECHO) $@ | $(SED) 's,.*/,,' | $(SED) 's/[.]x[bp]m.*//'`; \
	$(SED) 's/[	 ][cg]4*[ 	][#a-zA-Z0-9][#a-zA-Z0-9]*//g' $< | \
	$(SED) 's/[nN]one/white/g' | \
	$(XPMTOPPM) | $(PNMDEPTH) 255 | $(PPMTOPGM) | $(PGMTOPBM) | \
	$(PBMTOXBM) | $(SED) "s/noname/$${base}/g" | \
	$(SED) "s/static char/static unsigned char/g" | $(DELETE_CR) > $@~
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)

.xbm.xface:
	@$(ECHO) "Creating $@"
	@$(ECHO) "X-Face:`$(XBM2XFACE) $<`" | $(DELETE_CR) > $@~
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)


# This jewel makes an `emboss' effect in two steps:
# 1. Create a displaced mask in FILE-mask.pbm, using `pnmcut' and `pnmpaste'
# 2. Add the mask to FILE.xbm, using `pamarith'.
.xbm.xpmxx:
	@$(ECHO) "Creating $@"
	@-base=`$(ECHO) $@ | $(SED) 's,.*/,,' | $(SED) 's/[.]x[bp]m.*//'`; \
	$(SED) 's/unsigned //' $< | \
	$(XBMTOPBM) > $${base}-object.pbm; \
	$(PNMMARGIN) 1 $${base}-object.pbm | \
	$(PNMCUT) 0 0 29 25 | \
	$(PNMINVERT) | \
	$(PNMPASTE) -and $${base}-object.pbm 0 0 > $${base}-mask.pbm; \
	$(PPMTOXPM) $${base}-object.pbm | \
	$(SED) -e 's/#00*/grey50/' -e 's/#FF*/grey/' \
	-e 's/\<black\>/grey50/' -e 's/\<white\>/grey/' | \
	$(XPMTOPPM) > $${base}-button.ppm; \
	$(PAMARITH) -add $${base}-button.ppm $${base}-mask.pbm | \
	$(PPMTOXPM) | \
	$(SED) 's/c #BE[BE]*/c none   g none   m none  s Background/' | \
	$(SED) 's/c #7F[7F]*/c grey50 g grey50 m black/' | \
	$(SED) 's/c #FF*/c white  g white  m white/' | \
	$(SED) "s/noname/$${base}_xx_xpm/" | $(DELETE_CR) > $@~; \
	$(RM) $${base}-object.pbm $${base}-mask.pbm $${base}-button.ppm
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)

.xpm.xbmxx:
	@$(ECHO) "Creating $@"
	@-base=`$(ECHO) $@ | $(SED) 's,.*/,,' | $(SED) 's/[.]x[bp]m.*//'`; \
	$(SED) 's/[ 	]c[	 ][#a-zA-Z0-9][#a-zA-Z0-9]*//' $< | \
	$(SED) 's/ g black  / g grey50 /' | \
	$(SED) 's/ g white  / g grey75 /' | \
	$(SED) 's/g \([a-zA-Z0-9]*\) /c \1 g \1 /' | \
	$(SED) 's/[nN]one/white/g' | \
	$(XPMTOPPM) | $(PNMDEPTH) 255 | $(PPMTOPGM) | $(PGMTOPBM) | \
	$(PBMTOXBM) | $(SED) "s/noname/$${base}_xx/" | \
	$(SED) "s/static char/static unsigned char/g" | $(DELETE_CR) > $@~
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)

# Icons and logos.

# The (derived) icons.
# Icon directories
icondir = $(top_srcdir)/icons
buildicondir = $(top_builddir)/icons
cursorsdir = $(icondir)/cursors
glyphsdir = $(icondir)/glyphs
tictactoedir = $(icondir)/tictactoe
toolbardir = $(buildicondir)/toolbar


.PHONY: icons
icons:	$(ICONS)
	@:

# All X bitmaps are derived from the XPM sources.

$(buildicondir)/dddmask.xpm: $(icondir)/ddd.xpm
	@test -d $(buildicondir) || mkdir $(buildicondir)
	$(SED) 's/[wW]hite/black/g' $(icondir)/ddd.xpm | $(DELETE_CR) > $@

$(buildicondir)/ddd.xbm: $(icondir)/ddd.xpm

$(buildicondir)/ddd.xface: $(buildicondir)/ddd.xbm

$(buildicondir)/dddmask.xbm: $(icondir)/dddmask.xpm

# Original filter for dddsplash to extract g4 info
# $(SED) 's/[	 ][cmg][ 	][#a-zA-Z0-9][#a-zA-Z0-9]*//g' | \
#

$(buildicondir)/dddsplash.xbm: $(icondir)/dddsplash.xpm
	@test -d $(buildicondir) || mkdir $(buildicondir)
	@$(ECHO) "Creating $@"
	@-$(CAT) $(icondir)/dddsplash.xpm | \
	$(XPMTOPPM) | $(PNMDEPTH) 255 | $(PNMGAMMA) 0.4 | $(PPMTOPGM) | \
	$(PGMENHANCE) -7 | \
	$(PGMTOPBM) | $(PBMTOXBM) | $(SED) 's/noname/dddsplash/g' | \
	$(SED) "s/static char/static unsigned char/g" | $(DELETE_CR) > $@~
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)

$(buildicondir)/dddlogo.xpm: $(icondir)/dddsplash.xpm
	@test -d $(buildicondir) || mkdir $(buildicondir)
	$(CONVERT) -fill white \
	-draw 'rectangle 0,138 409,139' -draw 'rectangle 408,0 409,139' $< $@


XBM_GLYPH_ICONS = \
	$(glyphsdir)/arrow.xbm \
	$(glyphsdir)/cond.xbm \
	$(glyphsdir)/dragarrow.xbm \
	$(glyphsdir)/dragcond.xbm \
	$(glyphsdir)/dragstop.xbm \
	$(glyphsdir)/dragtemp.xbm \
	$(glyphsdir)/greyarrow.xbm \
	$(glyphsdir)/greycond.xbm \
	$(glyphsdir)/greysignal.xbm \
	$(glyphsdir)/greystop.xbm \
	$(glyphsdir)/greytemp.xbm \
	$(glyphsdir)/pastarrow.xbm \
	$(glyphsdir)/signalarrow.xbm \
	$(glyphsdir)/stop.xbm \
	$(glyphsdir)/temp.xbm

XBM_CURSORS_ICONS = \
	$(cursorsdir)/time16.xbm \
	$(cursorsdir)/time16m.xbm \
	$(cursorsdir)/time32.xbm \
	$(cursorsdir)/time32m.xbm

XBM_TICTACTOE_ICONS = \
	$(tictactoedir)/circle.xbm \
	$(tictactoedir)/cross.xbm \
	$(tictactoedir)/empty.xbm

# XBM icons are derived from XPM sources
XPM_ICONS = \
	$(toolbardir)/breakat.xpm \
	$(toolbardir)/clearat.xpm \
	$(toolbardir)/cluster.xpm \
	$(toolbardir)/delete.xpm \
	$(toolbardir)/deref.xpm \
	$(toolbardir)/disable.xpm \
	$(toolbardir)/display.xpm \
	$(toolbardir)/findbwd.xpm \
	$(toolbardir)/findfwd.xpm \
	$(toolbardir)/enable.xpm \
	$(toolbardir)/hide.xpm \
	$(toolbardir)/lookup.xpm \
	$(toolbardir)/maketemp.xpm \
	$(toolbardir)/newbreak.xpm \
	$(toolbardir)/newdisplay.xpm \
	$(toolbardir)/newwatch.xpm \
	$(toolbardir)/plot.xpm \
	$(toolbardir)/print.xpm \
	$(toolbardir)/properties.xpm \
	$(toolbardir)/rotate.xpm \
	$(toolbardir)/set.xpm \
	$(toolbardir)/show.xpm \
	$(toolbardir)/uncluster.xpm \
	$(toolbardir)/undisplay.xpm \
	$(toolbardir)/unwatch.xpm \
	$(toolbardir)/watch.xpm 

# XBM icons are derived from XPM sources
XBM_ICONS = \
	$(toolbardir)/breakat.xbm \
	$(toolbardir)/clearat.xbm \
	$(toolbardir)/cluster.xbm \
	$(toolbardir)/delete.xbm \
	$(toolbardir)/deref.xbm \
	$(toolbardir)/disable.xbm \
	$(toolbardir)/display.xbm \
	$(toolbardir)/findbwd.xbm \
	$(toolbardir)/findfwd.xbm \
	$(toolbardir)/enable.xbm \
	$(toolbardir)/hide.xbm \
	$(toolbardir)/lookup.xbm \
	$(toolbardir)/maketemp.xbm \
	$(toolbardir)/newbreak.xbm \
	$(toolbardir)/newdisplay.xbm \
	$(toolbardir)/newwatch.xbm \
	$(toolbardir)/plot.xbm \
	$(toolbardir)/print.xbm \
	$(toolbardir)/properties.xbm \
	$(toolbardir)/rotate.xbm \
	$(toolbardir)/set.xbm \
	$(toolbardir)/show.xbm \
	$(toolbardir)/uncluster.xbm \
	$(toolbardir)/undisplay.xbm \
	$(toolbardir)/unwatch.xbm \
	$(toolbardir)/watch.xbm 

# XPMXX icons (insensitive XPM icons) are derived from XBM sources
XPM_XX_ICONS = \
	$(toolbardir)/breakat.xpmxx \
	$(toolbardir)/clearat.xpmxx \
	$(toolbardir)/cluster.xpmxx \
	$(toolbardir)/delete.xpmxx \
	$(toolbardir)/deref.xpmxx \
	$(toolbardir)/disable.xpmxx \
	$(toolbardir)/display.xpmxx \
	$(toolbardir)/findbwd.xpmxx \
	$(toolbardir)/findfwd.xpmxx \
	$(toolbardir)/enable.xpmxx \
	$(toolbardir)/hide.xpmxx \
	$(toolbardir)/lookup.xpmxx \
	$(toolbardir)/maketemp.xpmxx \
	$(toolbardir)/newbreak.xpmxx \
	$(toolbardir)/newdisplay.xpmxx \
	$(toolbardir)/newwatch.xpmxx \
	$(toolbardir)/plot.xpmxx \
	$(toolbardir)/print.xpmxx \
	$(toolbardir)/properties.xpmxx \
	$(toolbardir)/rotate.xpmxx \
	$(toolbardir)/set.xpmxx \
	$(toolbardir)/show.xpmxx \
	$(toolbardir)/uncluster.xpmxx \
	$(toolbardir)/undisplay.xpmxx \
	$(toolbardir)/unwatch.xpmxx \
	$(toolbardir)/watch.xpmxx 

# XBMXX icons (insensitive XBM icons) are derived from XPM sources
XBM_XX_ICONS = \
	$(toolbardir)/breakat.xbmxx \
	$(toolbardir)/clearat.xbmxx \
	$(toolbardir)/cluster.xbmxx \
	$(toolbardir)/delete.xbmxx \
	$(toolbardir)/deref.xbmxx \
	$(toolbardir)/disable.xbmxx \
	$(toolbardir)/display.xbmxx \
	$(toolbardir)/findbwd.xbmxx \
	$(toolbardir)/findfwd.xbmxx \
	$(toolbardir)/enable.xbmxx \
	$(toolbardir)/hide.xbmxx \
	$(toolbardir)/lookup.xbmxx \
	$(toolbardir)/maketemp.xbmxx \
	$(toolbardir)/newbreak.xbmxx \
	$(toolbardir)/newdisplay.xbmxx \
	$(toolbardir)/newwatch.xbmxx \
	$(toolbardir)/plot.xbmxx \
	$(toolbardir)/print.xbmxx \
	$(toolbardir)/properties.xbmxx \
	$(toolbardir)/rotate.xbmxx \
	$(toolbardir)/set.xbmxx \
	$(toolbardir)/show.xbmxx \
	$(toolbardir)/uncluster.xbmxx \
	$(toolbardir)/undisplay.xbmxx \
	$(toolbardir)/unwatch.xbmxx \
	$(toolbardir)/watch.xbmxx

# All icons
# Order is significant here!  First sources, then derived files.
ICONS = \
	$(icondir)/ddd.xpm \
	$(icondir)/ddd.xbm \
	$(icondir)/dddback.xpm \
	$(icondir)/dddmask.xpm \
	$(icondir)/dddmask.xbm \
	$(icondir)/dddsplash.xpm \
	$(buildicondir)/dddsplash.xbm \
	$(buildicondir)/ddd.xface \
	$(XPM_ICONS) \
	$(XBM_CURSORS_ICONS) \
	$(XBM_GLYPH_ICONS) \
	$(XBM_TICTACTOE_ICONS) \
	$(XBM_ICONS) \
	$(XPM_XX_ICONS) \
	$(XBM_XX_ICONS)

# -----------------------------------------------------------------------------
# DDD Manual.
# -----------------------------------------------------------------------------

info_TEXINFOS = ddd.texi ddd-themes.texi
ddd_TEXINFOS = \
	$(srcdir)/config.texi \
	$(srcdir)/macros.texi \
	$(srcdir)/gpl.texinfo \
	$(srcdir)/gfdl.texinfo
ddd_themes_TEXINFOS = \
	$(srcdir)/config.texi \
	$(srcdir)/macros.texi \
	$(srcdir)/gpl.texinfo \
	$(srcdir)/gfdl.texinfo
texidir = $(srcdir)/../texinfo
TEXINFO_TEX = $(texidir)

$(srcdir)/config.texi: cfg.texi
	@$(CMP) cfg.texi $@ 2> /dev/null || \
	( $(CAT) cfg.texi > $@~ && $(MV) $@~ $@ )

$(srcdir)/gpl.texinfo: $(texidir)/gpl.texinfo
	$(RM) $@; cp $(texidir)/gpl.texinfo $@

$(srcdir)/gfdl.texinfo: $(texidir)/gfdl.texinfo
	$(RM) $@; cp $(texidir)/gfdl.texinfo $@

ddd.dvi: $(PDF_IMAGES) $(ddd_TEXINFOS)

ddd-themes.dvi: $(ddd_themes_TEXINFOS)


# The DDD manual in PostScript format, including all pictures.

docdir = $(srcdir)/../doc
builddocdir = $(top_builddir)/doc

.PHONY: ps
ps: $(builddocdir)/ddd.ps $(builddocdir)/ddd-themes.ps

# The DDD manual in PDF format, including all pictures.

.PHONY: pdf
pdf: $(builddocdir)/ddd.pdf $(builddocdir)/ddd-themes.pdf

$(builddocdir)/ddd.pdf: ddd.pdf
	@-test -d $(builddocdir) || mkdir $(builddocdir)
	$(INSTALL_DATA) ddd.pdf $@

$(builddocdir)/ddd-themes.pdf: ddd-themes.pdf
	@-test -d $(builddocdir) || mkdir $(builddocdir)
	$(INSTALL_DATA) ddd-themes.pdf $@

ddd.pdf: $(PDF_IMAGES) $(ddd_TEXINFOS)

ddd-themes.pdf: $(ddd_themes_TEXINFOS)



# The DDD manual in HTML format.

htmldir = $(builddocdir)/html

.PHONY: html
html: $(htmldir)/ddd.html $(htmldir)/ddd-themes.html

$(htmldir)/ddd.html $(htmldir)/ddd-themes.html: \
	$(srcdir)/ddd.texi $(srcdir)/ddd-themes.texi $(HTML_IMAGES) \
	style.css $(srcdir)/fix-html.sed
	@-test -d $(htmldir) || mkdir $(htmldir)
	@-test -d $(htmldir)/PICS || mkdir $(htmldir)/PICS
	@-test -d PICS || mkdir PICS
	$(RM) ddd*.html
	$(MAKEINFOHTML) -I $(srcdir) -I $(texidir) $(srcdir)/ddd-themes.texi --no-split
	$(MAKEINFOHTML) -I $(srcdir) -I $(texidir) $(srcdir)/ddd.texi --no-split
	for file in ddd*.html; do \
		$(SED) -f $(srcdir)/fix-html.sed $$file | \
		$(SED) 's/$(IMGEXT)/\$(IMGEXT2)/g' > $$file~ && \
		$(MV) $$file~ $$file; \
	done
	cd $(htmldir) && $(RM) ddd*.html style.css PICS/*
	$(INSTALL_DATA) ddd*.html $(srcdir)/style.css $(htmldir)
	($(ECHO) 'index.html:'; $(ECHO) '	$(LN_S) ddd.html index.html') \
	 > $(htmldir)/Makefile
	@$(ECHO) Installing images...
	@$(INSTALL_DATA) $(HTML_IMAGES) $(htmldir)/PICS
	@$(ECHO) Installing images...done.

# Manual page
man_MANS = ddd.1

# `ddd.man' has 8-bit format, `ddd.1' has 7-bit format
ddd.1:   ddd.man $(srcdir)/unumlaut.sed
	-$(SED) -f $(srcdir)/unumlaut.sed ddd.man | $(DELETE_CR) > $@~ \
	    && $(MV) $@~ $@



# -----------------------------------------------------------------------------
# DDD Manual Pictures.
# -----------------------------------------------------------------------------

# The image extension (.jpg, .png, or .gif)
IMGEXT     = .png
IMGEXT2    = .jpg

# Width of image in HTML manual (pixels)
IMAGE_WIDTH = 800

# Some XPM/PPM/PostScript converters, from the netpbm and pbmplus packages.
PBMTOXBM   = pbmtoxbm
PGMENHANCE = pgmenhance
PGMTOPBM   = pgmtopbm -threshold
PAMARITH   = pamarith
PNMCUT     = pnmcut
PNMDEPTH   = pnmdepth
PNMGAMMA   = pnmgamma
PNMINVERT  = pnminvert
PNMMARGIN  = pnmmargin
PNMPASTE   = pnmpaste
PNMTOPS    = pnmtops
PPMTOPGM   = ppmtopgm
PPMTOXPM   = ppmtoxpm
XBMTOPBM   = xbmtopbm
XPMTOPPM   = xpmtoppm

# The ImageMagick `convert' program (convert)
CONVERT    = convert

# The epstopdf program.  (epstopdf)
# Available from http://www.tug.org/applications/pdftex/epstopdf
EPSTOPDF = epstopdf

# The fig2dev command.  (fig2dev)
FIG2DEV = fig2dev
FIG2PS  = $(FIG2DEV) -L eps
FIG2PDF = $(FIG2DEV) -L pdf


# We use giftoppm/ppmtogif to get optimal GIF compression.
.eps.gif:
	@-test -d $(buildpicdir) || mkdir $(buildpicdir)
	$(CONVERT) -geometry $(IMAGE_WIDTH) $< $@

.eps.jpg:
	@-test -d $(buildpicdir) || mkdir $(buildpicdir)
	$(CONVERT) -geometry $(IMAGE_WIDTH) -quality 100 $< $@

##.eps.png:
##	$(CONVERT) -geometry $(IMAGE_WIDTH) -quality 100 $< $@

.eps.pdf:
	@-test -d $(buildpicdir) || mkdir $(buildpicdir)
	$(EPSTOPDF) $< --outfile=$@

.fig.eps:
	@-test -d $(buildpicdir) || mkdir $(buildpicdir)
	(cd `dirname $<` && $(FIG2PS) `basename $<` ) > $@

.fig.pdf:
	@-test -d $(buildpicdir) || mkdir $(buildpicdir)
	(cd `dirname $<` && $(FIG2PDF) `basename $<` ) > $@

# Our `texinfo.tex' must reside in the current directory, otherwise
# texi2dvi will use the default.
.texi.pdf:
	@-test -d $(buildpicdir) || mkdir $(buildpicdir)
	@-test -f texinfo.tex || $(LN_S) $(texidir)/texinfo.tex .
	$(TEXI2PDF) -I $(srcdir) -I $(texidir) $<

.texi.dvi:
	@-test -d $(buildpicdir) || mkdir $(buildpicdir)
	@-test -f texinfo.tex || $(LN_S) $(texidir)/texinfo.tex .
	$(TEXI2DVI) -I $(srcdir) -I $(texidir) $<

.pdf.ps:
	test -d $(docdir) || mkdir $(docdir)
	pdf2ps $< $@


# PostScript pictures in the DDD manual.
# All of these are generated from FIG and XPM files.
picdir = $(srcdir)/PICS
buildpicdir = $(top_builddir)/ddd/PICS
PS_IMAGES = \
	$(buildpicdir)/disclaimer.eps           \
	$(buildpicdir)/dddback.eps		\
	$(buildpicdir)/dddlogo.eps		\
	$(buildpicdir)/dddsplash.eps            \
	$(buildpicdir)/ddd-aliases.eps          \
	$(buildpicdir)/ddd-all.eps              \
	$(buildpicdir)/ddd-arguments.eps        \
	$(buildpicdir)/ddd-attach.eps           \
	$(buildpicdir)/ddd-backtrace.eps	\
	$(buildpicdir)/ddd-bp-popup.eps         \
	$(buildpicdir)/ddd-bp-properties.eps    \
	$(buildpicdir)/ddd-button-editor.eps    \
	$(buildpicdir)/ddd-clusters.eps         \
	$(buildpicdir)/ddd-code.eps             \
	$(buildpicdir)/ddd-console.eps          \
	$(buildpicdir)/ddd-data-prefs.eps       \
	$(buildpicdir)/ddd-define-command.eps   \
	$(buildpicdir)/ddd-display.eps          \
	$(buildpicdir)/ddd-drag.eps             \
	$(buildpicdir)/ddd-edit-breakpoints.eps \
	$(buildpicdir)/ddd-edit-displays.eps    \
	$(buildpicdir)/ddd-examine.eps          \
	$(buildpicdir)/ddd-font-prefs.eps       \
	$(buildpicdir)/ddd-general-prefs.eps    \
	$(buildpicdir)/ddd-helpers.eps  	\
	$(buildpicdir)/ddd-hide-display.eps     \
	$(buildpicdir)/ddd-history.eps          \
	$(buildpicdir)/ddd-layout.eps           \
	$(buildpicdir)/ddd-line-popup.eps       \
	$(buildpicdir)/ddd-locals.eps	        \
	$(buildpicdir)/ddd-open.eps             \
	$(buildpicdir)/ddd-open-session.eps     \
	$(buildpicdir)/ddd-plots.eps            \
	$(buildpicdir)/ddd-print-graph.eps      \
	$(buildpicdir)/ddd-print-output.eps     \
	$(buildpicdir)/ddd-print-popup.eps      \
	$(buildpicdir)/ddd-registers.eps        \
	$(buildpicdir)/ddd-repeats.eps          \
	$(buildpicdir)/ddd-rotate-display.eps   \
	$(buildpicdir)/ddd-save-session.eps     \
	$(buildpicdir)/ddd-select-display.eps   \
	$(buildpicdir)/ddd-separate.eps         \
	$(buildpicdir)/ddd-set.eps	        \
	$(buildpicdir)/ddd-settings.eps         \
	$(buildpicdir)/ddd-shortcuts.eps        \
	$(buildpicdir)/ddd-shortcut-editor.eps  \
	$(buildpicdir)/ddd-signals.eps          \
	$(buildpicdir)/ddd-source-popup.eps     \
	$(buildpicdir)/ddd-source-prefs.eps     \
	$(buildpicdir)/ddd-source.eps           \
	$(buildpicdir)/ddd-startup-prefs.eps    \
	$(buildpicdir)/ddd-status-displays.eps  \
	$(buildpicdir)/ddd-suppress.eps         \
	$(buildpicdir)/ddd-themes.eps           \
	$(buildpicdir)/ddd-threads.eps          \
	$(buildpicdir)/ddd-tool.eps             \
	$(buildpicdir)/ddd-toolbar.eps          \
	$(buildpicdir)/ddd-toolbars.eps         \
	$(buildpicdir)/ddd-value-tip.eps        \
	$(buildpicdir)/tut-backtrace.eps        \
	$(buildpicdir)/tut-display.eps          \
	$(buildpicdir)/tut-finish.eps           \
	$(buildpicdir)/tut-invoke.eps           \
	$(buildpicdir)/tut-run.eps              \
	$(buildpicdir)/tut-set.eps              \
	$(buildpicdir)/tut-value.eps

PDF_IMAGES = \
	$(buildpicdir)/disclaimer.pdf           \
	$(buildpicdir)/dddback.pdf		\
	$(buildpicdir)/dddlogo.pdf              \
	$(buildpicdir)/dddsplash.pdf            \
	$(buildpicdir)/ddd-aliases.pdf          \
	$(buildpicdir)/ddd-all.pdf              \
	$(buildpicdir)/ddd-arguments.pdf        \
	$(buildpicdir)/ddd-attach.pdf           \
	$(buildpicdir)/ddd-backtrace.pdf	\
	$(buildpicdir)/ddd-bp-popup.pdf         \
	$(buildpicdir)/ddd-bp-properties.pdf    \
	$(buildpicdir)/ddd-button-editor.pdf    \
	$(buildpicdir)/ddd-clusters.pdf         \
	$(buildpicdir)/ddd-code.pdf             \
	$(buildpicdir)/ddd-console.pdf          \
	$(buildpicdir)/ddd-data-prefs.pdf       \
	$(buildpicdir)/ddd-define-command.pdf   \
	$(buildpicdir)/ddd-display.pdf          \
	$(buildpicdir)/ddd-drag.pdf             \
	$(buildpicdir)/ddd-edit-breakpoints.pdf \
	$(buildpicdir)/ddd-edit-displays.pdf    \
	$(buildpicdir)/ddd-examine.pdf          \
	$(buildpicdir)/ddd-font-prefs.pdf       \
	$(buildpicdir)/ddd-general-prefs.pdf    \
	$(buildpicdir)/ddd-helpers.pdf  	\
	$(buildpicdir)/ddd-hide-display.pdf     \
	$(buildpicdir)/ddd-history.pdf          \
	$(buildpicdir)/ddd-layout.pdf           \
	$(buildpicdir)/ddd-line-popup.pdf       \
	$(buildpicdir)/ddd-locals.pdf	        \
	$(buildpicdir)/ddd-open.pdf             \
	$(buildpicdir)/ddd-open-session.pdf     \
	$(buildpicdir)/ddd-plots.pdf            \
	$(buildpicdir)/ddd-print-graph.pdf      \
	$(buildpicdir)/ddd-print-output.pdf     \
	$(buildpicdir)/ddd-print-popup.pdf      \
	$(buildpicdir)/ddd-registers.pdf        \
	$(buildpicdir)/ddd-repeats.pdf          \
	$(buildpicdir)/ddd-rotate-display.pdf   \
	$(buildpicdir)/ddd-save-session.pdf     \
	$(buildpicdir)/ddd-select-display.pdf   \
	$(buildpicdir)/ddd-separate.pdf         \
	$(buildpicdir)/ddd-set.pdf	        \
	$(buildpicdir)/ddd-settings.pdf         \
	$(buildpicdir)/ddd-shortcuts.pdf        \
	$(buildpicdir)/ddd-shortcut-editor.pdf  \
	$(buildpicdir)/ddd-signals.pdf          \
	$(buildpicdir)/ddd-source-popup.pdf     \
	$(buildpicdir)/ddd-source-prefs.pdf     \
	$(buildpicdir)/ddd-source.pdf           \
	$(buildpicdir)/ddd-startup-prefs.pdf    \
	$(buildpicdir)/ddd-status-displays.pdf  \
	$(buildpicdir)/ddd-suppress.pdf         \
	$(buildpicdir)/ddd-themes.pdf           \
	$(buildpicdir)/ddd-threads.pdf          \
	$(buildpicdir)/ddd-tool.pdf             \
	$(buildpicdir)/ddd-toolbar.pdf          \
	$(buildpicdir)/ddd-toolbars.pdf         \
	$(buildpicdir)/ddd-value-tip.pdf        \
	$(buildpicdir)/tut-backtrace.pdf        \
	$(buildpicdir)/tut-display.pdf          \
	$(buildpicdir)/tut-finish.pdf           \
	$(buildpicdir)/tut-invoke.pdf           \
	$(buildpicdir)/tut-run.pdf              \
	$(buildpicdir)/tut-set.pdf              \
	$(buildpicdir)/tut-value.pdf

HTML_IMAGES = \
	$(buildpicdir)/disclaimer$(IMGEXT2)           \
	$(buildpicdir)/dddback$(IMGEXT2)	      \
	$(buildpicdir)/dddlogo$(IMGEXT2)              \
	$(buildpicdir)/dddsplash$(IMGEXT2)            \
	$(buildpicdir)/ddd-aliases$(IMGEXT2)          \
	$(buildpicdir)/ddd-all$(IMGEXT2)              \
	$(buildpicdir)/ddd-arguments$(IMGEXT2)        \
	$(buildpicdir)/ddd-attach$(IMGEXT2)           \
	$(buildpicdir)/ddd-backtrace$(IMGEXT2)	      \
	$(buildpicdir)/ddd-bp-popup$(IMGEXT2)         \
	$(buildpicdir)/ddd-bp-properties$(IMGEXT2)    \
	$(buildpicdir)/ddd-button-editor$(IMGEXT2)    \
	$(buildpicdir)/ddd-clusters$(IMGEXT2)         \
	$(buildpicdir)/ddd-code$(IMGEXT2)             \
	$(buildpicdir)/ddd-console$(IMGEXT2)          \
	$(buildpicdir)/ddd-data-prefs$(IMGEXT2)       \
	$(buildpicdir)/ddd-define-command$(IMGEXT2)   \
	$(buildpicdir)/ddd-display$(IMGEXT2)          \
	$(buildpicdir)/ddd-drag$(IMGEXT2)             \
	$(buildpicdir)/ddd-edit-breakpoints$(IMGEXT2) \
	$(buildpicdir)/ddd-edit-displays$(IMGEXT2)    \
	$(buildpicdir)/ddd-examine$(IMGEXT2)          \
	$(buildpicdir)/ddd-font-prefs$(IMGEXT2)       \
	$(buildpicdir)/ddd-general-prefs$(IMGEXT2)    \
	$(buildpicdir)/ddd-helpers$(IMGEXT2)  	     \
	$(buildpicdir)/ddd-hide-display$(IMGEXT2)     \
	$(buildpicdir)/ddd-history$(IMGEXT2)          \
	$(buildpicdir)/ddd-layout$(IMGEXT2)           \
	$(buildpicdir)/ddd-line-popup$(IMGEXT2)       \
	$(buildpicdir)/ddd-locals$(IMGEXT2)	     \
	$(buildpicdir)/ddd-open$(IMGEXT2)             \
	$(buildpicdir)/ddd-open-session$(IMGEXT2)     \
	$(buildpicdir)/ddd-plots$(IMGEXT2)            \
	$(buildpicdir)/ddd-print-graph$(IMGEXT2)      \
	$(buildpicdir)/ddd-print-output$(IMGEXT2)     \
	$(buildpicdir)/ddd-print-popup$(IMGEXT2)      \
	$(buildpicdir)/ddd-registers$(IMGEXT2)        \
	$(buildpicdir)/ddd-repeats$(IMGEXT2)          \
	$(buildpicdir)/ddd-rotate-display$(IMGEXT2)   \
	$(buildpicdir)/ddd-save-session$(IMGEXT2)     \
	$(buildpicdir)/ddd-select-display$(IMGEXT2)   \
	$(buildpicdir)/ddd-separate$(IMGEXT2)         \
	$(buildpicdir)/ddd-set$(IMGEXT2)	             \
	$(buildpicdir)/ddd-settings$(IMGEXT2)         \
	$(buildpicdir)/ddd-shortcuts$(IMGEXT2)        \
	$(buildpicdir)/ddd-shortcut-editor$(IMGEXT2)  \
	$(buildpicdir)/ddd-signals$(IMGEXT2)          \
	$(buildpicdir)/ddd-source-popup$(IMGEXT2)     \
	$(buildpicdir)/ddd-source-prefs$(IMGEXT2)     \
	$(buildpicdir)/ddd-source$(IMGEXT2)           \
	$(buildpicdir)/ddd-startup-prefs$(IMGEXT2)    \
	$(buildpicdir)/ddd-status-displays$(IMGEXT2)  \
	$(buildpicdir)/ddd-suppress$(IMGEXT2)         \
	$(buildpicdir)/ddd-tool$(IMGEXT2)             \
	$(buildpicdir)/ddd-toolbar$(IMGEXT2)          \
	$(buildpicdir)/ddd-toolbars$(IMGEXT2)         \
	$(buildpicdir)/ddd-themes$(IMGEXT2)           \
	$(buildpicdir)/ddd-threads$(IMGEXT2)          \
	$(buildpicdir)/ddd-value-tip$(IMGEXT2)        \
	$(buildpicdir)/tut-backtrace$(IMGEXT2)        \
	$(buildpicdir)/tut-display$(IMGEXT2)          \
	$(buildpicdir)/tut-finish$(IMGEXT2)           \
	$(buildpicdir)/tut-invoke$(IMGEXT2)           \
	$(buildpicdir)/tut-run$(IMGEXT2)              \
	$(buildpicdir)/tut-set$(IMGEXT2)              \
	$(buildpicdir)/tut-value$(IMGEXT2)

# All derived pictures
IMAGES = $(PS_IMAGES) $(PDF_IMAGES) $(HTML_IMAGES)

# Included pictures

$(buildpicdir)/ddd-all.eps: \
	$(picdir)/ddd-all.fig $(picdir)/ddd-all.png
$(buildpicdir)/ddd-clusters.eps: \
	$(picdir)/ddd-clusters.fig $(picdir)/ddd-clusters.png
$(buildpicdir)/ddd-console.eps: \
	$(picdir)/ddd-console.fig $(picdir)/ddd-console.png
$(buildpicdir)/ddd-history.eps: \
	$(picdir)/ddd-history.fig $(picdir)/ddd-history.png
$(buildpicdir)/ddd-open.eps: \
	$(picdir)/ddd-open.fig $(picdir)/ddd-open.png
$(buildpicdir)/ddd-source.eps: \
	$(picdir)/ddd-source.fig $(picdir)/ddd-source.png
$(buildpicdir)/ddd-line-popup.eps: \
	$(picdir)/ddd-line-popup.fig $(picdir)/ddd-line-popup.png
$(buildpicdir)/ddd-bp-popup.eps: \
	$(picdir)/ddd-bp-popup.fig $(picdir)/ddd-bp-popup.png
$(buildpicdir)/ddd-source-popup.eps: \
	$(picdir)/ddd-source-popup.fig $(picdir)/ddd-source-popup.png
$(buildpicdir)/ddd-arguments.eps: \
	$(picdir)/ddd-arguments.fig $(picdir)/ddd-arguments.png
$(buildpicdir)/ddd-tool.eps: \
	$(picdir)/ddd-tool.fig $(picdir)/ddd-tool.png
$(buildpicdir)/ddd-backtrace.eps: \
	$(picdir)/ddd-backtrace.fig $(picdir)/ddd-backtrace.png
$(buildpicdir)/ddd-value-tip.eps: \
	$(picdir)/ddd-value-tip.fig $(picdir)/ddd-value-tip.png
$(buildpicdir)/ddd-print-popup.eps: \
	$(picdir)/ddd-print-popup.fig $(picdir)/ddd-print-popup.png
$(buildpicdir)/ddd-select-display.eps: \
	$(picdir)/ddd-select-display.fig $(picdir)/ddd-select-display.png
$(buildpicdir)/ddd-hide-display.eps: \
	$(picdir)/ddd-hide-display.fig $(picdir)/ddd-hide-display.png
$(buildpicdir)/ddd-rotate-display.eps: \
	$(picdir)/ddd-rotate-display.fig $(picdir)/ddd-rotate-display.png
$(buildpicdir)/ddd-locals.eps: \
	$(picdir)/ddd-locals.fig $(picdir)/ddd-locals.png
$(buildpicdir)/ddd-status-displays.eps: \
	$(picdir)/ddd-status-displays.fig $(picdir)/ddd-status-displays.png
$(buildpicdir)/ddd-set.eps: \
	$(picdir)/ddd-set.fig $(picdir)/ddd-set.png
$(buildpicdir)/ddd-layout.eps: \
	$(picdir)/ddd-layout.fig $(picdir)/ddd-layout.png
$(buildpicdir)/ddd-print-graph.eps: \
	$(picdir)/ddd-print-graph.fig $(picdir)/ddd-print-graph.png
$(buildpicdir)/ddd-print-output.eps: \
	$(picdir)/ddd-print-output.fig $(picdir)/ddd-graph.ps
$(buildpicdir)/ddd-code.eps: \
	$(picdir)/ddd-code.fig $(picdir)/ddd-code.png
$(buildpicdir)/ddd-registers.eps: \
	$(picdir)/ddd-registers.fig $(picdir)/ddd-registers.png
$(buildpicdir)/ddd-general-prefs.eps: \
	$(picdir)/ddd-general-prefs.fig $(picdir)/ddd-general-prefs.png 
$(buildpicdir)/ddd-helpers.eps: \
	$(picdir)/ddd-helpers.fig $(picdir)/ddd-helpers.png 
$(buildpicdir)/ddd-source-prefs.eps: \
	$(picdir)/ddd-source-prefs.fig $(picdir)/ddd-source-prefs.png
$(buildpicdir)/ddd-data-prefs.eps: \
	$(picdir)/ddd-data-prefs.fig $(picdir)/ddd-data-prefs.png
$(buildpicdir)/ddd-startup-prefs.eps: \
	$(picdir)/ddd-startup-prefs.fig $(picdir)/ddd-startup-prefs.png
$(buildpicdir)/ddd-settings.eps: \
	$(picdir)/ddd-settings.fig $(picdir)/ddd-settings.png
$(buildpicdir)/ddd-threads.eps: \
	$(picdir)/ddd-threads.fig $(picdir)/ddd-threads.png
$(buildpicdir)/ddd-aliases.eps: \
	$(picdir)/ddd-aliases.fig $(picdir)/ddd-aliases.png
$(buildpicdir)/ddd-edit-displays.eps: \
	$(picdir)/ddd-edit-displays.fig $(picdir)/ddd-edit-displays.png
$(buildpicdir)/ddd-attach.eps: \
	$(picdir)/ddd-attach.fig $(picdir)/ddd-attach.png
$(buildpicdir)/ddd-save-session.eps: \
	$(picdir)/ddd-save-session.fig $(picdir)/ddd-save-session.png
$(buildpicdir)/ddd-open-session.eps: \
	$(picdir)/ddd-open-session.fig $(picdir)/ddd-open-session.png
$(buildpicdir)/ddd-button-editor.eps: \
	$(picdir)/ddd-button-editor.fig $(picdir)/ddd-button-editor.png
$(buildpicdir)/ddd-shortcuts.eps: \
	$(picdir)/ddd-shortcuts.fig $(picdir)/ddd-shortcuts.png
$(buildpicdir)/ddd-shortcut-editor.eps: \
	$(picdir)/ddd-shortcut-editor.fig $(picdir)/ddd-shortcut-editor.png
$(buildpicdir)/ddd-drag.eps: \
	$(picdir)/ddd-drag.fig \
	$(picdir)/ddd-drag1.png \
	$(picdir)/ddd-drag2.png \
	$(picdir)/ddd-drag3.png
$(buildpicdir)/disclaimer.eps: \
	$(picdir)/disclaimer.fig
$(buildpicdir)/ddd-separate.eps: \
	$(picdir)/ddd-separate.fig $(picdir)/ddd-separate.png
$(buildpicdir)/ddd-bp-properties.eps: \
	$(picdir)/ddd-bp-properties.fig $(picdir)/ddd-bp-properties.png
$(buildpicdir)/ddd-edit-breakpoints.eps: \
	$(picdir)/ddd-edit-breakpoints.fig $(picdir)/ddd-edit-breakpoints.png
$(buildpicdir)/ddd-display.eps: \
	$(picdir)/ddd-display.fig $(picdir)/ddd-display.png
$(buildpicdir)/ddd-repeats.eps: \
	$(picdir)/ddd-repeats.fig $(picdir)/ddd-repeats.png
$(buildpicdir)/ddd-toolbar.eps: \
	$(picdir)/ddd-toolbar.fig $(picdir)/ddd-toolbar.png
$(buildpicdir)/ddd-font-prefs.eps: \
	$(picdir)/ddd-font-prefs.fig $(picdir)/ddd-font-prefs.png
$(buildpicdir)/ddd-define-command.eps: \
	$(picdir)/ddd-define-command.fig $(picdir)/ddd-define-command.png
$(buildpicdir)/ddd-signals.eps: \
	$(picdir)/ddd-signals.fig $(picdir)/ddd-signals.png
$(buildpicdir)/ddd-examine.eps: \
	$(picdir)/ddd-examine.fig $(picdir)/ddd-examine.png
$(buildpicdir)/ddd-clusters.eps: \
	$(picdir)/ddd-clusters.fig \
	$(picdir)/clustered.ps $(picdir)/unclustered.ps
$(buildpicdir)/ddd-plots.eps: \
	$(picdir)/ddd-plots.fig $(picdir)/ddd-plots.png
$(buildpicdir)/ddd-toolbars.eps: \
	$(picdir)/ddd-toolbars.fig \
	$(picdir)/ddd-toolbar-1.png \
	$(picdir)/ddd-toolbar-2.png \
	$(picdir)/ddd-toolbar-3.png \
	$(picdir)/ddd-toolbar-4.png
PICS/Make$(buildpicdir)/ddd-themes.eps: \
	$(picdir)/ddd-themes.fig \
	$(picdir)/theme-none.ps \
	$(picdir)/theme-backgrounds.ps \
	$(picdir)/theme-small-titles.ps \
	$(picdir)/theme-small-values.ps \
	$(picdir)/theme-tiny-values.ps \
	$(picdir)/theme-suppress.ps
$(buildpicdir)/ddd-suppress.eps: \
	$(picdir)/ddd-suppress.fig \
	$(picdir)/theme-tree-full.ps \
	$(picdir)/theme-tree-part.ps
$(buildpicdir)/tut-backtrace.eps: \
	$(picdir)/tut-backtrace.fig $(picdir)/tut-backtrace.png
$(buildpicdir)/tut-display.eps: \
	$(picdir)/tut-display.fig $(picdir)/tut-display.png
$(buildpicdir)/tut-finish.eps: \
	$(picdir)/tut-finish.fig $(picdir)/tut-finish.png
$(buildpicdir)/tut-invoke.eps: \
	$(picdir)/tut-invoke.fig $(picdir)/tut-invoke.png
$(buildpicdir)/tut-run.eps: \
	$(picdir)/tut-run.fig $(picdir)/tut-run.png
$(buildpicdir)/tut-set.eps: \
	$(picdir)/tut-set.fig $(picdir)/tut-set.png
$(buildpicdir)/tut-value.eps: \
	$(picdir)/tut-value.fig $(picdir)/tut-value.png


# JPEGs are derived from EPS files

$(buildpicdir)/ddd-all$(IMGEXT2):             $(buildpicdir)/ddd-all.eps
$(buildpicdir)/ddd-console$(IMGEXT2):         $(buildpicdir)/ddd-console.eps
$(buildpicdir)/ddd-history$(IMGEXT2):         $(buildpicdir)/ddd-history.eps
$(buildpicdir)/ddd-open$(IMGEXT2):            $(buildpicdir)/ddd-open.eps
$(buildpicdir)/ddd-source$(IMGEXT2):          $(buildpicdir)/ddd-source.eps
$(buildpicdir)/ddd-line-popup$(IMGEXT2):      $(buildpicdir)/ddd-line-popup.eps
$(buildpicdir)/ddd-bp-popup$(IMGEXT2):        $(buildpicdir)/ddd-bp-popup.eps
$(buildpicdir)/ddd-source-popup$(IMGEXT2):    $(buildpicdir)/ddd-source-popup.eps
$(buildpicdir)/ddd-arguments$(IMGEXT2):       $(buildpicdir)/ddd-arguments.eps
$(buildpicdir)/ddd-tool$(IMGEXT2):            $(buildpicdir)/ddd-tool.eps
$(buildpicdir)/ddd-backtrace$(IMGEXT2):       $(buildpicdir)/ddd-backtrace.eps
$(buildpicdir)/ddd-value-tip$(IMGEXT2):       $(buildpicdir)/ddd-value-tip.eps
$(buildpicdir)/ddd-print-popup$(IMGEXT2):     $(buildpicdir)/ddd-print-popup.eps
$(buildpicdir)/ddd-select-display$(IMGEXT2):  $(buildpicdir)/ddd-select-display.eps
$(buildpicdir)/ddd-hide-display$(IMGEXT2):    $(buildpicdir)/ddd-hide-display.eps
$(buildpicdir)/ddd-rotate-display$(IMGEXT2):  $(buildpicdir)/ddd-rotate-display.eps
$(buildpicdir)/ddd-locals$(IMGEXT2):          $(buildpicdir)/ddd-locals.eps
$(buildpicdir)/ddd-status-displays$(IMGEXT2): $(buildpicdir)/ddd-status-displays.eps
$(buildpicdir)/ddd-set$(IMGEXT2):             $(buildpicdir)/ddd-set.eps
$(buildpicdir)/ddd-layout$(IMGEXT2):          $(buildpicdir)/ddd-layout.eps
$(buildpicdir)/ddd-print-graph$(IMGEXT2):     $(buildpicdir)/ddd-print-graph.eps
$(buildpicdir)/ddd-print-output$(IMGEXT2):    $(buildpicdir)/ddd-print-output.eps
$(buildpicdir)/ddd-code$(IMGEXT2):            $(buildpicdir)/ddd-code.eps
$(buildpicdir)/ddd-registers$(IMGEXT2):       $(buildpicdir)/ddd-registers.eps
$(buildpicdir)/ddd-general-prefs$(IMGEXT2):   $(buildpicdir)/ddd-general-prefs.eps
$(buildpicdir)/ddd-helpers$(IMGEXT2):         $(buildpicdir)/ddd-helpers.eps
$(buildpicdir)/ddd-source-prefs$(IMGEXT2):    $(buildpicdir)/ddd-source-prefs.eps
$(buildpicdir)/ddd-data-prefs$(IMGEXT2):      $(buildpicdir)/ddd-data-prefs.eps
$(buildpicdir)/ddd-startup-prefs$(IMGEXT2):   $(buildpicdir)/ddd-startup-prefs.eps
$(buildpicdir)/ddd-settings$(IMGEXT2):        $(buildpicdir)/ddd-settings.eps
$(buildpicdir)/ddd-threads$(IMGEXT2):         $(buildpicdir)/ddd-threads.eps
$(buildpicdir)/ddd-aliases$(IMGEXT2):         $(buildpicdir)/ddd-aliases.eps
$(buildpicdir)/ddd-edit-displays$(IMGEXT2):   $(buildpicdir)/ddd-edit-displays.eps
$(buildpicdir)/ddd-attach$(IMGEXT2):          $(buildpicdir)/ddd-attach.eps
$(buildpicdir)/ddd-save-session$(IMGEXT2):    $(buildpicdir)/ddd-save-session.eps
$(buildpicdir)/ddd-open-session$(IMGEXT2):    $(buildpicdir)/ddd-open-session.eps
$(buildpicdir)/ddd-button-editor$(IMGEXT2):   $(buildpicdir)/ddd-button-editor.eps
$(buildpicdir)/ddd-shortcuts$(IMGEXT2):       $(buildpicdir)/ddd-shortcuts.eps
$(buildpicdir)/ddd-shortcut-editor$(IMGEXT2): $(buildpicdir)/ddd-shortcut-editor.eps
$(buildpicdir)/ddd-drag$(IMGEXT2):            $(buildpicdir)/ddd-drag.eps
$(buildpicdir)/disclaimer$(IMGEXT2):          $(buildpicdir)/disclaimer.eps
$(buildpicdir)/ddd-separate$(IMGEXT2):        $(buildpicdir)/ddd-separate.eps
$(buildpicdir)/ddd-bp-properties$(IMGEXT2):   $(buildpicdir)/ddd-bp-properties.eps
$(buildpicdir)/ddd-edit-breakpoints$(IMGEXT2): $(buildpicdir)/ddd-edit-breakpoints.eps
$(buildpicdir)/ddd-display$(IMGEXT2):         $(buildpicdir)/ddd-display.eps
$(buildpicdir)/ddd-repeats$(IMGEXT2):         $(buildpicdir)/ddd-repeats.eps
$(buildpicdir)/ddd-toolbar$(IMGEXT2):         $(buildpicdir)/ddd-toolbar.eps
$(buildpicdir)/ddd-font-prefs$(IMGEXT2):      $(buildpicdir)/ddd-font-prefs.eps
$(buildpicdir)/ddd-define-command$(IMGEXT2):  $(buildpicdir)/ddd-define-command.eps
$(buildpicdir)/ddd-signals$(IMGEXT2):         $(buildpicdir)/ddd-signals.eps
$(buildpicdir)/ddd-examine$(IMGEXT2):         $(buildpicdir)/ddd-examine.eps
$(buildpicdir)/ddd-clusters$(IMGEXT2):        $(buildpicdir)/ddd-clusters.eps
$(buildpicdir)/ddd-plots$(IMGEXT2):           $(buildpicdir)/ddd-plots.eps
$(buildpicdir)/ddd-suppress$(IMGEXT2):        $(buildpicdir)/ddd-suppress.eps
$(buildpicdir)/ddd-toolbars$(IMGEXT2):        $(buildpicdir)/ddd-toolbars.eps
$(buildpicdir)/ddd-themes$(IMGEXT2):          $(buildpicdir)/ddd-themes.eps
$(buildpicdir)/tut-backtrace$(IMGEXT2):	      $(buildpicdir)/tut-backtrace.eps
$(buildpicdir)/tut-display$(IMGEXT2):	      $(buildpicdir)/tut-display.eps
$(buildpicdir)/tut-finish$(IMGEXT2):	      $(buildpicdir)/tut-finish.eps
$(buildpicdir)/tut-invoke$(IMGEXT2):	      $(buildpicdir)/tut-invoke.eps
$(buildpicdir)/tut-run$(IMGEXT2):	      $(buildpicdir)/tut-run.eps
$(buildpicdir)/tut-set$(IMGEXT2):	      $(buildpicdir)/tut-set.eps
$(buildpicdir)/tut-value$(IMGEXT2):	      $(buildpicdir)/tut-value.eps


# PDFs are also derived from EPS files.
# (We could also generate them directly from FIG files, but this
# would mean to duplicate all dependencies as above.)

$(buildpicdir)/ddd-all.pdf:              $(buildpicdir)/ddd-all.eps
$(buildpicdir)/ddd-console.pdf:          $(buildpicdir)/ddd-console.eps
$(buildpicdir)/ddd-history.pdf:          $(buildpicdir)/ddd-history.eps
$(buildpicdir)/ddd-open.pdf:             $(buildpicdir)/ddd-open.eps
$(buildpicdir)/ddd-source.pdf:           $(buildpicdir)/ddd-source.eps
$(buildpicdir)/ddd-line-popup.pdf:       $(buildpicdir)/ddd-line-popup.eps
$(buildpicdir)/ddd-bp-popup.pdf:         $(buildpicdir)/ddd-bp-popup.eps
$(buildpicdir)/ddd-source-popup.pdf:     $(buildpicdir)/ddd-source-popup.eps
$(buildpicdir)/ddd-arguments.pdf:        $(buildpicdir)/ddd-arguments.eps
$(buildpicdir)/ddd-tool.pdf:             $(buildpicdir)/ddd-tool.eps
$(buildpicdir)/ddd-backtrace.pdf:        $(buildpicdir)/ddd-backtrace.eps
$(buildpicdir)/ddd-value-tip.pdf:        $(buildpicdir)/ddd-value-tip.eps
$(buildpicdir)/ddd-print-popup.pdf:      $(buildpicdir)/ddd-print-popup.eps
$(buildpicdir)/ddd-select-display.pdf:   $(buildpicdir)/ddd-select-display.eps
$(buildpicdir)/ddd-hide-display.pdf:     $(buildpicdir)/ddd-hide-display.eps
$(buildpicdir)/ddd-rotate-display.pdf:   $(buildpicdir)/ddd-rotate-display.eps
$(buildpicdir)/ddd-locals.pdf:           $(buildpicdir)/ddd-locals.eps
$(buildpicdir)/ddd-status-displays.pdf:  $(buildpicdir)/ddd-status-displays.eps
$(buildpicdir)/ddd-set.pdf:              $(buildpicdir)/ddd-set.eps
$(buildpicdir)/ddd-layout.pdf:           $(buildpicdir)/ddd-layout.eps
$(buildpicdir)/ddd-print-graph.pdf:      $(buildpicdir)/ddd-print-graph.eps
$(buildpicdir)/ddd-print-output.pdf:     $(buildpicdir)/ddd-print-output.eps
$(buildpicdir)/ddd-code.pdf:             $(buildpicdir)/ddd-code.eps
$(buildpicdir)/ddd-registers.pdf:        $(buildpicdir)/ddd-registers.eps
$(buildpicdir)/ddd-general-prefs.pdf:    $(buildpicdir)/ddd-general-prefs.eps
$(buildpicdir)/ddd-helpers.pdf:          $(buildpicdir)/ddd-helpers.eps
$(buildpicdir)/ddd-source-prefs.pdf:     $(buildpicdir)/ddd-source-prefs.eps
$(buildpicdir)/ddd-data-prefs.pdf:       $(buildpicdir)/ddd-data-prefs.eps
$(buildpicdir)/ddd-startup-prefs.pdf:    $(buildpicdir)/ddd-startup-prefs.eps
$(buildpicdir)/ddd-settings.pdf:         $(buildpicdir)/ddd-settings.eps
$(buildpicdir)/ddd-threads.pdf:          $(buildpicdir)/ddd-threads.eps
$(buildpicdir)/ddd-aliases.pdf:          $(buildpicdir)/ddd-aliases.eps
$(buildpicdir)/ddd-edit-displays.pdf:    $(buildpicdir)/ddd-edit-displays.eps
$(buildpicdir)/ddd-attach.pdf:           $(buildpicdir)/ddd-attach.eps
$(buildpicdir)/ddd-save-session.pdf:     $(buildpicdir)/ddd-save-session.eps
$(buildpicdir)/ddd-open-session.pdf:     $(buildpicdir)/ddd-open-session.eps
$(buildpicdir)/ddd-button-editor.pdf:    $(buildpicdir)/ddd-button-editor.eps
$(buildpicdir)/ddd-shortcuts.pdf:        $(buildpicdir)/ddd-shortcuts.eps
$(buildpicdir)/ddd-shortcut-editor.pdf:  $(buildpicdir)/ddd-shortcut-editor.eps
$(buildpicdir)/ddd-drag.pdf:             $(buildpicdir)/ddd-drag.eps
$(buildpicdir)/disclaimer.pdf:           $(buildpicdir)/disclaimer.eps
$(buildpicdir)/ddd-separate.pdf:         $(buildpicdir)/ddd-separate.eps
$(buildpicdir)/ddd-bp-properties.pdf:    $(buildpicdir)/ddd-bp-properties.eps
$(buildpicdir)/ddd-edit-breakpoints.pdf: $(buildpicdir)/ddd-edit-breakpoints.eps
$(buildpicdir)/ddd-display.pdf:          $(buildpicdir)/ddd-display.eps
$(buildpicdir)/ddd-repeats.pdf:          $(buildpicdir)/ddd-repeats.eps
$(buildpicdir)/ddd-toolbar.pdf:          $(buildpicdir)/ddd-toolbar.eps
$(buildpicdir)/ddd-font-prefs.pdf:       $(buildpicdir)/ddd-font-prefs.eps
$(buildpicdir)/ddd-define-command.pdf:   $(buildpicdir)/ddd-define-command.eps
$(buildpicdir)/ddd-signals.pdf:          $(buildpicdir)/ddd-signals.eps
$(buildpicdir)/ddd-examine.pdf:          $(buildpicdir)/ddd-examine.eps
$(buildpicdir)/ddd-clusters.pdf:         $(buildpicdir)/ddd-clusters.eps
$(buildpicdir)/ddd-plots.pdf:            $(buildpicdir)/ddd-plots.eps
$(buildpicdir)/ddd-suppress.pdf:         $(buildpicdir)/ddd-suppress.eps
$(buildpicdir)/ddd-toolbars.pdf:         $(buildpicdir)/ddd-toolbars.eps
$(buildpicdir)/ddd-themes.pdf:           $(buildpicdir)/ddd-themes.eps
$(buildpicdir)/tut-backtrace.pdf:	 $(buildpicdir)/tut-backtrace.eps
$(buildpicdir)/tut-display.pdf:          $(buildpicdir)/tut-display.eps
$(buildpicdir)/tut-finish.pdf:           $(buildpicdir)/tut-finish.eps
$(buildpicdir)/tut-invoke.pdf:	    	 $(buildpicdir)/tut-invoke.eps
$(buildpicdir)/tut-run.pdf:	         $(buildpicdir)/tut-run.eps
$(buildpicdir)/tut-set.pdf:	         $(buildpicdir)/tut-set.eps
$(buildpicdir)/tut-value.pdf:            $(buildpicdir)/tut-value.eps


# The logo comes in XPM source
$(buildpicdir)/dddsplash.eps: $(icondir)/dddsplash.xpm
	$(CONVERT) $(icondir)/dddsplash.xpm $@

$(buildpicdir)/dddsplash.gif: $(icondir)/dddsplash.xpm
	$(CONVERT) $(icondir)/dddsplash.xpm $@

$(buildpicdir)/dddsplash.jpg: $(icondir)/dddsplash.xpm
	$(CONVERT) -quality 100 $(icondir)/dddsplash.xpm $@

$(buildpicdir)/dddsplash.png: $(icondir)/dddsplash.xpm
	$(CONVERT) -quality 100 $(icondir)/dddsplash.xpm $@

$(buildpicdir)/dddlogo.eps: $(buildicondir)/dddlogo.xpm
	$(CONVERT) $(buildicondir)/dddlogo.xpm $@

$(buildpicdir)/dddlogo.gif: $(buildicondir)/dddlogo.xpm
	$(CONVERT) $(buildicondir)/dddlogo.xpm $@

$(buildpicdir)/dddlogo.jpg: $(buildicondir)/dddlogo.xpm
	$(CONVERT) -quality 100 $(buildicondir)/dddlogo.xpm $@

$(buildpicdir)/dddlogo.png: $(buildicondir)/dddlogo.xpm
	$(CONVERT) -quality 100 $(buildicondir)/dddlogo.xpm $@

$(buildpicdir)/dddback.eps: $(icondir)/dddback.xpm
	$(CONVERT) $(icondir)/dddback.xpm $@

$(buildpicdir)/dddback.gif: $(icondir)/dddback.xpm
	$(CONVERT) $(icondir)/dddback.xpm $@

$(buildpicdir)/dddback.jpg: $(icondir)/dddback.xpm
	$(CONVERT) -quality 100 $(icondir)/dddback.xpm $@

$(buildpicdir)/dddback.png: $(icondir)/dddback.xpm
	$(CONVERT) -quality 100 $(icondir)/dddback.xpm $@



# -----------------------------------------------------------------------------
# Configuration.
# -----------------------------------------------------------------------------

# Architecture (usually overridden by the top-level Makefile).

# The host we were compiled on.  (CPU-COMPANY-SYSTEM)
host_canonical = @host@

# The target we were compiled for.  (CPU-COMPANY-SYSTEM)
target_canonical = @host@


# Special rules for configuration.
# autoheader might not change config.h.in
stamp-h: config.h.in $(top_builddir)/config.status
	CONFIG_FILES= CONFIG_HEADERS=config.h $(top_builddir)/config.status
	$(DATE) | $(DELETE_CR) > $@~ && $(MV) $@~ $@

$(srcdir)/config.h.in:
	cd $(srcdir)/.. && $(AUTOHEADER)

# Special program includes
# Note: we use `build.h' instead of `./build.h', because `./build.h'
# breaks SGI make.
version.h: vr.h
	@$(CMP) vr.h $@ 2> /dev/null || \
	    ( $(CAT) vr.h | $(DELETE_CR) > $@~ && $(MV) $@~ $@ )
build.h: bld.h
	@$(CMP) bld.h $@ 2> /dev/null || \
	    ( $(CAT) bld.h | $(DELETE_CR) > $@~ && $(MV) $@~ $@ )
host.h: $(srcdir)/host.h.in $(top_builddir)/config.status
	$(CAT) $(srcdir)/host.h.in | \
	$(SED) "s%@HOST_CANONICAL@%$(host_canonical)%" | \
	$(SED) "s%@TARGET_CANONICAL@%$(target_canonical)%" | \
	    $(DELETE_CR) > $@~ && $(MV) $@~ $@
root.h: $(srcdir)/root.h.in $(top_builddir)/config.status
	$(CAT) $(srcdir)/root.h.in | \
	$(SED) "s%@ROOT@%$(dddrootdir)%" | \
	$(SED) "s%@ALT_ROOT@%$(pkgdatadir)%" | \
	    $(DELETE_CR) > $@~ && $(MV) $@~ $@


# -----------------------------------------------------------------------------
# Installation.
# -----------------------------------------------------------------------------

dddapp_DATA = Ddd


# -----------------------------------------------------------------------------
# Distribution.
# -----------------------------------------------------------------------------

# Architecture-independent files required for building DDD.
# In a distribution, these should be already built.
#
# Order is significant here!  First source files, then derived files.
PROG_STUFF = $(LEXTOCXX).flex $(LEXTOCXX).sun \
	     vsl-lex.L $(LEXTOCXX) vsl-lex.C \
	     $(YACCTOCXX).bison $(YACCTOCXX).sun \
	     vsl-gramma.Y $(YACCTOCXX) yacctoC.h vsl-gramma.h vsl-gramma.C \
	     rxscan.L rxscan.C \
             $(srcdir)/../TIPS $(srcdir)/tips.m4 \
	     $(srcdir)/tips2m4.awk $(srcdir)/tips.html \
	     $(srcdir)/macros.m4 $(srcdir)/colors.m4 \
             $(srcdir)/Ddd.in.m4 $(srcdir)/Ddd.in \
	     host.h.in $(srcdir)/vsl2c.sed \
             $(srcdir)/ddd.vsl.m4 $(vsldir)/ddd.vsl $(srcdir)/ddd.vsl.h \
             root.h.in $(srcdir)/COPYING.gz $(srcdir)/NEWS.gz \
             $(srcdir)/COPYING.gz.C $(srcdir)/NEWS.gz.C \
	     $(srcdir)/ddd.info.txt.gz.C $(srcdir)/ddd.info \
             xlibdir.txt xlibdir.C $(srcdir)/header.str \
	     $(srcdir)/trailer.str $(srcdir)/eps-code.x eps-code.h \
	     .gdbinit $(ICONS)

# Additional files to be included in the distribution.
DIST_STUFF = $(srcdir)/header.ps $(srcdir)/trailer.ps \
	     $(docdir)/ddd.pdf \
	     $(docdir)/ddd-themes.pdf \
	     $(docdir)/ddd-paper.ps \
	     $(htmldir)/ddd.html $(srcdir)/ad2c.sed $(srcdir)/config-info \
	     style.css $(srcdir)/fix-html.sed \
	     $(srcdir)/unumlaut.sed ddd.desktop

# Let automake know what we need.
EXTRA_DIST = $(PROG_STUFF) $(DIST_STUFF) $(IMAGES)

# Make sure all this is built before DDD.
ddd.$(OBJEXT): $(PROG_STUFF)

# These should not go into the distribution.
DISTCLEANFILES = build.h host.h root.h ../config.cache USERINFO.txt \
	$(top_builddir)/ddd/stamp-h $(man_MANS) configinfo.C \
	version.h Ddd.ad.h

# `make mostlyclean' should also remove these temporary Texinfo files.
MOSTLYCLEANFILES = ddd.au ddd.cm ddd.cms ddd.en ddd.fl ddd.fls ddd.lb ddd.lbs \
	ddd.tmp ddd.html ddd.pdf ddd.ps \
	ddd-themes.au ddd-themes.fl ddd-themes.fls \
	ddd-themes.tmp ddd-themes.html ddd-themes.pdf ddd-themes.ps

# Have `make maintainer-clean' remove _all_ derived files.
MAINTAINERCLEANFILES = Ddd.in ddd.vsl.h ddd.info.txt.gz gfdl.texinfo \
	NEWS.gz.C ddd.info.txt.gz.C ddd.info \
	NEWS.gz ddd.info.txt tips.m4 gpl.texinfo vsl-lex.C \
	COPYING.gz config.h.in config.texi vsl-gramma.C \
	vsl-gramma.h xlibdir.C trailer.str tips.html \
	COPYING.gz.C header.str version.h rxscan.C \
	ddd/PICS/*.gif \
	ddd/PICS/*.jpg \
	ddd/PICS/*.pdf \
	$(buildicondir)/ddd.xface $(buildicondir)/dddsplash.xbm \
	$(vsldir)/ddd.vsl

# `make diststuff' creates all files required for a distribution.
diststuff: $(EXTRA_DIST)

# `make progstuff' creates all files required for building DDD.
progstuff: $(PROG_STUFF)

desktopdir = $(datadir)/applications

desktop_DATA = ddd.desktop
