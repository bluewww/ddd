## $Id$
## DDD Makefile template.
## Process this file with `automake' to produce a `Makefile.in'.

# $Id$
# DDD Makefile.

# Copyright (C) 2000 Universitaet Passau, Germany.
# Written by Andreas Zeller <zeller@gnu.org>.
# 
# This file is part of DDD.
# 
# DDD is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# DDD is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with DDD -- see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
SUFFIXES = .C .cc .c .mod .p .Y .YY .L .txt .i .xbm .xbmxx .xpm .xpmxx \
	   .xface .C~ .c~ .f .f~ .sh .sh~ .fig .eps .jpg .png .gif .pdf \
	   $(IMGEXT)

# Shared data installation
dddrootdir    = $(pkgdatadir)-$(VERSION)
dddappdir     = $(pkgdatadir)-$(VERSION)/$(ddd)


# -----------------------------------------------------------------------------
# DDD Sources.
# -----------------------------------------------------------------------------

# General-purpose utilities: strings, regexps, run-type type info...
BASE_SOURCES = \
	assert.h     \
	basename.C   \
	basename.h   \
	bool.h       \
	compare.C    \
	compare.h    \
	cook.C       \
	cook.h       \
	cwd.C        \
	cwd.h        \
	explicit.h   \
	getrlimit.h  \
	glob.C       \
	glob.h       \
	hash.C       \
	hash.h       \
	home.C       \
	home.h       \
	hypot.h      \
	misc.h       \
	mutable.h    \
	my-alloca.h  \
	my-dirent.h  \
	oldcxx.h     \
	pi.h         \
	return.h     \
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
TEMPLATE_SOURCES = \
	Assoc.h      \
	IntIntAA.C   \
	IntIntAA.h   \
	Queue.h      \
	StatArray.C  \
	StatArray.h  \
	StringA.C    \
	StringA.h    \
	StringMap.C  \
	StringMap.h  \
	StringSA.C   \
	StringSA.h   \
	StringSAA.C  \
	StringSAA.h  \
	TextPosA.h   \
	VarArray.h   \
	VoidArray.C  \
	VoidArray.h  \
	WidgetSA.C   \
	WidgetSA.h

# Interprocess communication
# Top-level class is `Agent', declared in `Agent.h'
AGENT_SOURCES = \
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
XSTUFF_SOURCES = \
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
MSTUFF_SOURCES = \
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
BOX_SOURCES = \
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
VSL_SOURCES = \
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
GRAPH_SOURCES = \
	ArcGraphE.C  \
	ArcGraphE.h  \
	BoxEdgeA.C   \
	BoxEdgeA.h   \
	BoxGraphN.C  \
	BoxGraphN.h  \
	EdgeA.C      \
	EdgeA.h      \
	EdgeAPA.C    \
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
	GraphNPA.C   \
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
DDD_SOURCES = \
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
	isid.C       \
	isid.h       \
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
	configinfo.C \
	configinfo.h \
	source.C     \
	source.h     \
	status.C     \
	status.h     \
	string-fun.C \
	string-fun.h \
	stty.h       \
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
	BreakPM.C    \
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
	DispNodeM.C  \
	DispNodeM.h  \
	DispValue.C  \
	DispValue.h  \
	DispValueA.C \
	DispValueA.h \
	DispValueT.C \
	DispValueT.h \
	EntryType.C  \
	EntryType.h  \
	GDBAgent.C   \
	GDBAgent.h   \
	HelpCB.C     \
	HelpCB.h     \
	HistoryD.C   \
	HistoryD.h   \
	HistoryF.C   \
	HistoryF.h   \
	MinMaxA.C    \
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
	StringTPA.C  \
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


# -----------------------------------------------------------------------------
# Local Libraries.
# -----------------------------------------------------------------------------

# POSIX regex routines (from system or ../librx/librx.a)
# LIBRX      = @LIBRX@
LIBRX      =
# termcap routines (from system or ../termcap/libtermcap.a)
LIBTERMCAP = @LIBTERMCAP@
# -liberty library (../libiberty/libiberty.a)
LIBIBERTY  = @LIBIBERTY@
# -lreadline library (../readline/libreadline.a)
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
	$(LIBTERMCAP) $(LIBY) $(LIBIBERTY) $(LIBM) $(LIBELF)



# -----------------------------------------------------------------------------
# X Libraries.
# -----------------------------------------------------------------------------

# Special flags for linking with X.  (-L/usr/X11R5/lib)
X_LDFLAGS = @X_LIBS@

# Motif library.  (-lXm)
LIBXM = @XM_LIBS@

# Use this alternative for building `semistatic' executables
# where Motif libraries are statically linked in.
# LIBXM = -Wl,-Bstatic @XM_LIBS@ -Wl,-Bdynamic

# Athena libraries.  (-lXaw -lXmu)
LIBXAW = @XAW_LIBS@

# X extension library; needed for Athena and Motif >= 2.0.  (-lXext)
LIBXEXT = @XEXT_LIBS@

# Xpm library; needed for DDD and sometimes for Motif >= 2.0.  (-lXpm)
LIBXPM = @XPM_LIBS@

# Xp library; sometimes needed for Motif >= 2.1.  (-lXp)
LIBXP = @XP_LIBS@

# gen library; sometimes needed for Motif >= 2.0.  (-lgen)
LIBGEN = @LIBGEN@

# X toolkit library.  (-lXt)
LIBXT = @XT_LIBS@

# X library.  (-lSM -lICE -lX11 -lnsl -lsocket)
LIBX11 = @X_PRE_LIBS@ @X11_LIBS@ @X_EXTRA_LIBS@

# All libraries shown above
ALL_X_LIBS = $(X_LDFLAGS) $(LIBXM) $(LIBXP) $(LIBXPM) $(LIBXAW) $(LIBXEXT) \
	     $(LIBXT) $(LIBX11) $(LIBGEN)


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
# Extras.  (-fexternal-templates -felide-constructors)
CXXSTUFF = @CXXSTUFF@ @X_PERMISSIVE@
CXXFLAGS = $(CXXOPT) $(CXXDEBUG) $(CXXWARNINGS) $(CXXSTUFF)

bin_PROGRAMS = ddd
noinst_PROGRAMS = stringify userinfo ctest cxxtest vsl
ddd_SOURCES = ddd.C \
	$(BASE_SOURCES) \
	$(TEMPLATE_SOURCES) \
	$(AGENT_SOURCES) \
	$(XSTUFF_SOURCES) \
	$(MSTUFF_SOURCES) \
	$(BOX_SOURCES) \
	$(VSL_SOURCES) \
	$(GRAPH_SOURCES) \
	$(DDD_SOURCES)

stringify_SOURCES = stringify.C
userinfo_SOURCES  = userinfo.C
ctest_SOURCES     = ctest.c
cxxtest_SOURCES   = cxxtest.C
vsl_SOURCES       = vsl.C \
	DocSpace.c \
	DocSpace.h \
	$(BASE_SOURCES) \
	$(TEMPLATE_SOURCES) \
	$(AGENT_SOURCES) \
	$(XSTUFF_SOURCES) \
	$(MSTUFF_SOURCES) \
	$(BOX_SOURCES) \
	$(VSL_SOURCES)

cxxtest_LDADD = $(LIBM)
ddd_LDADD = $(ALL_X_LIBS) $(ALL_LIBS) 
vsl_LDADD = $(ALL_X_LIBS) $(ALL_LIBS)
X_INCLUDE = @X_CFLAGS@
INCLUDES = -I$(srcdir)/.. $(X_INCLUDE)


# -----------------------------------------------------------------------------
# DDD Applocation Defaults.
# -----------------------------------------------------------------------------

# The resource header and app-default files.

Ddd.ad.h: Ddd $(srcdir)/ad2c.sed
	$(SED) -n -f $(srcdir)/ad2c.sed Ddd > $@~ && $(MV) $@~ $@

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
	) > $@~ && $(MV) $@~ $@

$(srcdir)/tips.m4: $(srcdir)/../TIPS $(srcdir)/tips2m4.awk
	$(AWK) -f $(srcdir)/tips2m4.awk $(srcdir)/../TIPS > $@~ && $(MV) $@~ $@

$(srcdir)/tips.html: $(srcdir)/../TIPS
	( \
		echo "define(srcdir, \`$(srcdir)')dnl"; \
		echo "define(macros, \`$(srcdir)/html.m4')dnl"; \
		echo "include(srcdir()/../TIPS)dnl" \
	) | $(M4) | $(AWK) -f $(srcdir)/tips2html.awk | \
	$(SED) 's/\\133/\[/g' | $(SED) 's/\\135/\]/g' | \
	$(SED) 's/@\([A-Z][A-Z]*\)@/\1/g' | $(SED) 's/@ /@/g' \
	> $@~ && $(MV) $@~ $@



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
	) | $(M4) > $@~ && $(MV) $@~ $@

VSLSOURCES = \
	$(vsldir)/std.vsl \
	$(vsldir)/builtin.vsl \
	$(vsldir)/tab.vsl \
	$(vsldir)/fonts.vsl \
	$(vsldir)/colors.vsl \
	$(vsldir)/list.vsl

$(srcdir)/ddd.vsl.h: $(vsldir)/ddd.vsl $(srcdir)/vsl2c.sed $(VSLSOURCES)
	$(CAT) $(vsldir)/ddd.vsl > tmp.c
	$(VSL_CPP) tmp.c | $(SED) -f $(srcdir)/vsl2c.sed > $@
	$(RM) tmp.c

.PHONY: vsllib
vsllib: $(vsldir)/ddd.vsl $(VSLSOURCES)


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
	$(MV) $(LEX_OUTPUT_ROOT).C $@
	$(RM) $(LEX_OUTPUT_ROOT).c

$(srcdir)/vsl-gramma.h $(srcdir)/vsl-gramma.C: $(srcdir)/vsl-gramma.Y \
	$(YACCTOCXX) $(YACCTOCXX).bison $(YACCTOCXX).h $(YACCTOCXX).sun
	$(YACC) $(YFLAGS) -d $(srcdir)/vsl-gramma.Y
	$(SHELL) $(YACCTOCXX) -prefix vsl
	$(MV) y.tab.H $(srcdir)/vsl-gramma.h
	$(MV) y.tab.C $(srcdir)/vsl-gramma.C
	$(RM) y.tab.c y.tab.h

VSLRead.$(OBJEXT): \
	$(srcdir)/vsl-lex.C \
	$(srcdir)/vsl-gramma.h \
	$(srcdir)/vsl-gramma.C

# DDD regexp parsers.
rxscan.C: $(srcdir)/rxscan.L
	$(LEX) $(LFLAGS) $(srcdir)/rxscan.L
	$(SHELL) $(LEXTOCXX) -prefix "ddd"
	$(MV) $(LEX_OUTPUT_ROOT).C $@
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
	$(CPP) -C -P -undef $(CPPFLAGS) $(DEFS) $(INCLUDE) tmp.C > $@; \
	$(RM) tmp.C

# Reference.
# ddd.info.txt does NOT depend on ddd.info since we don't want it to get
# re-created after each configuration.
$(srcdir)/ddd.info.txt: $(srcdir)/ddd.texi configure.in
	$(MAKE) ddd.info
	info --subnodes -o - -f ddd.info > $@~ && $(MV) $@~ $@

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
configinfo.C: USERINFO $(srcdir)/config-info
	$(SHELL) $(srcdir)/config-info $(srcdir)/USERINFO USERINFO > $@~ && \
	$(MV) $@~ $@


# If `~/.ddd_userinfo' exists, use this file as personal identification.
USERINFO: userinfo$(EXEEXT)
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
	$(SED) 's/^.*$$/"&\\n"/' $(srcdir)/header.ps > $@~ && $(MV) $@~ $@

$(srcdir)/trailer.str: $(srcdir)/trailer.ps
	$(SED) 's/^.*$$/"&\\n"/' $(srcdir)/trailer.ps > $@~ && $(MV) $@~ $@

$(srcdir)/eps-code.h: \
	$(srcdir)/header.str $(srcdir)/trailer.str $(srcdir)/eps-code.x
	$(SED) -e '/prolog/r $(srcdir)/header.str' \
	       -e '/trailer/r $(srcdir)/trailer.str' $(srcdir)/eps-code.x \
	> $@~ && $(MV) $@~ $@


# -----------------------------------------------------------------------------
# Demos.
# -----------------------------------------------------------------------------

# `make check' commands.

# A debugger for debugging DDD.  (gdb)
DEBUGGER = @DEBUGGER@

# The options for DDD when invoking `make check'.  ()
DDD_OPTIONS = @DEBUGGER_OPTION@

# For demonstration and debugging purposes
.PHONY: check
check: Ddd ./$(ddd)$(EXEEXT) cxxtest$(EXEEXT)
	DDD_HOME=$(srcdir)/.. XUSERFILESEARCHPATH=%N XAPPLRESDIR=. \
	./$(ddd)$(EXEEXT) $(DDD_OPTIONS) cxxtest$(EXEEXT)

.PHONY: debug
debug: Ddd ./$(ddd)$(EXEEXT) cxxtest$(EXEEXT) .gdbinit
	DDD_HOME=$(srcdir)/.. XUSERFILESEARCHPATH=%N XAPPLRESDIR=. \
$(DEBUGGER) ./$(ddd)$(EXEEXT)

# We don't want no optimizing, but debugging info in our test files
MAKE_DEBUG = \
$(MAKE) CXXFLAGS="`$(ECHO) '$(CXXFLAGS)' | \
	$(SED) 's/-O[^ ]*//' | $(SED) 's/-DNDEBUG//'`" \
	CFLAGS="`$(ECHO) '$(CFLAGS)' | \
	$(SED) 's/-O[^ ]*//' | $(SED) 's/-DNDEBUG//'`"

cxxtest.$(OBJEXT): $(srcdir)/cxxtest.C
	@case '$(CXXFLAGS)' in \
	*-O*) $(MAKE_DEBUG) $@;; \
	*) set -x; \
	   $(CXXCOMPILE) -c @CXXDEBUG@ $(srcdir)/cxxtest.C -o $@;; \
	esac

ctest.$(OBJEXT): $(srcdir)/ctest.c
	@case '$(CFLAGS)' in \
	*-O*) $(MAKE_DEBUG) $@;; \
	*) set -x; \
	   $(COMPILE) -c -g $(srcdir)/ctest.c -o $@;; \
	esac

sample.$(OBJEXT): $(srcdir)/sample.c
	@case '$(CFLAGS)' in \
	*-O*) $(MAKE_DEBUG) $@;; \
	*) set -x; \
	   $(COMPILE) -c -g $(srcdir)/sample.c -o $@;; \
	esac

grabtest.$(OBJEXT): $(srcdir)/grabtest.C
	@case '$(CXXFLAGS)' in \
	*-O*) $(MAKE_DEBUG) $@;; \
	*) set -x; \
	   $(CXXCOMPILE) -c @CXXDEBUG@ $(srcdir)/grabtest.C -o $@;; \
	esac

bug.$(OBJEXT):	$(srcdir)/bug.C
	@case '$(CXXFLAGS)' in \
	*-O*) $(MAKE_DEBUG) $@;; \
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
	$(SED) "s/static char/static unsigned char/g" > $@~
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)

.xbm.xface:
	@$(ECHO) "Creating $@"
	@$(ECHO) "X-Face:`$(XBM2XFACE) $<`" > $@~
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)


# This jewel makes an `emboss' effect in two steps:
# 1. Create a displaced mask in FILE-mask.pbm, using `pnmcut' and `pnmpaste'
# 2. Add the mask to FILE.xbm, using `pnmarith'.
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
	$(SED) 's/#00*/grey50/' | \
	$(SED) 's/#FF*/grey/' | \
	$(XPMTOPPM) > $${base}-button.ppm; \
	$(PNMARITH) -add $${base}-button.ppm $${base}-mask.pbm | \
	$(PPMTOXPM) | \
	$(SED) 's/c #BE[BE]*/c none   g none   m none  s Background/' | \
	$(SED) 's/c #7F[7F]*/c grey50 g grey50 m black/' | \
	$(SED) 's/c #FF*/c white  g white  m white/' | \
	$(SED) "s/noname/$${base}_xx_xpm/" > $@~; \
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
	$(SED) "s/static char/static unsigned char/g" > $@~
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)

# Icons and logos.

.PHONY: icons
icons:	$(ICONS)
	@:

# All X bitmaps are derived from the XPM sources.

$(icondir)/dddmask.xpm: $(icondir)/ddd.xpm
	$(SED) 's/[wW]hite/black/g' $(icondir)/ddd.xpm > $@

$(icondir)/ddd.xbm: $(icondir)/ddd.xpm

$(icondir)/ddd.xface: $(icondir)/ddd.xbm

$(icondir)/dddmask.xbm: $(icondir)/dddmask.xpm

# Original filter for dddsplash to extract g4 info
# $(SED) 's/[	 ][cmg][ 	][#a-zA-Z0-9][#a-zA-Z0-9]*//g' | \
#

$(icondir)/dddsplash.xbm: $(icondir)/dddsplash.xpm
	@$(ECHO) "Creating $@"
	@-$(CAT) $(icondir)/dddsplash.xpm | \
	$(XPMTOPPM) | $(PNMDEPTH) 255 | $(PNMGAMMA) 0.4 | $(PPMTOPGM) | \
	$(PGMENHANCE) -7 | \
	$(PGMTOPBM) | $(PBMTOXBM) | $(SED) 's/noname/dddsplash/g' | \
	$(SED) "s/static char/static unsigned char/g" > $@~
	@test -s $@~ && $(MV) $@~ $@ || ($(RM) $@~; test -s $@)

$(icondir)/dddlogo.xpm: $(icondir)/dddsplash.xpm
	$(CONVERT) -pen white \
	-draw 'rectangle 0,0 409,139' -draw 'rectangle 1,1 408,138' $< $@


# The (derived) icons.
# Icon directories
icondir = $(srcdir)/../icons
toolbardir = $(icondir)/toolbar

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

# All derived icons
# Order is significant here!  First sources, then derived files.
ICONS = \
	$(icondir)/ddd.xbm \
	$(icondir)/dddmask.xbm \
	$(icondir)/dddsplash.xbm \
	$(icondir)/ddd.xface \
	$(XBM_ICONS) \
	$(XPM_XX_ICONS) \
	$(XBM_XX_ICONS)



# -----------------------------------------------------------------------------
# DDD Manual.
# -----------------------------------------------------------------------------

info_TEXINFOS = ddd.texi
ddd_TEXINFOS = $(srcdir)/config.texi $(srcdir)/gpl.texinfo
texidir = $(srcdir)/../texinfo
TEXINFO_TEX = $(texidir)

$(srcdir)/config.texi: cfg.texi
	@$(CMP) cfg.texi $@ 2> /dev/null || \
	( $(CAT) cfg.texi > $@~ && $(MV) $@~ $@ )

$(srcdir)/gpl.texinfo: $(texidir)/gpl.texinfo
	$(RM) $@; $(LN_S) $(texidir)/gpl.texinfo $@

ddd.dvi: $(PS_IMAGES)


# The DDD manual in PostScript format, including all pictures.

$(docdir)/ddd.ps: ddd.dvi
	$(DVIPS) ddd.dvi -o $@


# The DDD manual in PDF format, including all pictures.

.PHONY: pdf
pdf: $(docdir)/ddd.pdf

$(docdir)/ddd.pdf: ddd.pdf
	$(INSTALL_DATA) ddd.pdf $@

ddd.pdf: $(PDF_IMAGES)


# The DDD manual in HTML format.

docdir = $(srcdir)/../doc
htmldir = $(docdir)/html

.PHONY: html
html: $(htmldir)/index.html

$(htmldir)/index.html: $(srcdir)/ddd.texi $(HTML_IMAGES) \
	style.css $(srcdir)/fix-html.sed
	@-test -d $(htmldir) || mkdir $(htmldir)
	@-test -d $(htmldir)/PICS || mkdir $(htmldir)/PICS
	@-test -d PICS || $(LN_S) $(picdir) .
	$(RM) ddd*.html
	$(MAKEINFO) --html -I $(srcdir) -I $(texidir) $(srcdir)/ddd.texi
	for file in ddd*.html; do \
		$(SED) -f $(srcdir)/fix-html.sed $$file | \
		$(SED) 's/@IMGEXT@/\$(IMGEXT)/g' > $$file~ && \
		$(MV) $$file~ $$file; \
	done
	cd $(htmldir) && $(RM) ddd*.html style.css PICS/*
	$(INSTALL_DATA) ddd*.html $(srcdir)/style.css $(htmldir)
	@$(ECHO) Installing images...
	@$(INSTALL_DATA) $(HTML_IMAGES) $(htmldir)/PICS
	@$(ECHO) Installing images...done.
	$(RM) $(htmldir)/index.html
	$(LN_S) ddd.html $@

# Manual page
man_MANS = ddd.1

# `ddd.man' has 8-bit format, `ddd.1' has 7-bit format
ddd.1:   ddd.man $(srcdir)/unumlaut.sed
	-$(SED) -f $(srcdir)/unumlaut.sed ddd.man > $@~ && $(MV) $@~ $@



# -----------------------------------------------------------------------------
# DDD Manual Pictures.
# -----------------------------------------------------------------------------

# The image extension (.jpg, .png, or .gif)
IMGEXT     = .jpg

# Width of image in HTML manual (pixels)
IMAGE_WIDTH = 600

# Some XPM/PPM/PostScript converters, from the netpbm and pbmplus packages.
PBMTOXBM   = pbmtoxbm
PGMENHANCE = pgmenhance
PGMTOPBM   = pgmtopbm
PNMARITH   = pnmarith
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

# The face-generating program from the `compface' package.  (xbm2xface.pl)
XBM2XFACE  = xbm2xface.pl

# The epstopdf program.  (epstopdf)
# Available from http://www.tug.org/applications/pdftex/epstopdf
EPSTOPDF = epstopdf

# The texi2pdf program.  (texi2pdf)
TEXI2PDF = texi2dvi --pdf

# The fig2dev command.  (fig2dev)
FIG2DEV = fig2dev
FIG2PS = $(FIG2DEV) -L ps


# We use giftoppm/ppmtogif to get optimal GIF compression.
.eps.gif:
	$(CONVERT) -geometry $(IMAGE_WIDTH) $< $@

.eps.jpg:
	$(CONVERT) -geometry $(IMAGE_WIDTH) -quality 100 $< $@

.eps.png:
	$(CONVERT) -geometry $(IMAGE_WIDTH) -quality 100 $< $@

.eps.pdf:
	$(EPSTOPDF) $< --outfile=$@

.fig.eps:
	(cd `dirname $<` && $(FIG2PS) `basename $<` ) > $@

.texi.pdf:
	@-test -d PICS || $(LN_S) $(picdir) .
	TEXINPUTS=$(top_srcdir)/../texinfo:$$TEXINPUTS \
	  MAKEINFO='$(MAKEINFO) -I $(srcdir)' $(TEXI2PDF) $<

# PostScript pictures in the DDD manual.
# All of these are generated from FIG and XPM files.
picdir = $(srcdir)/PICS
PS_IMAGES = \
	$(picdir)/disclaimer.eps           \
	$(picdir)/dddlogo.eps		   \
	$(picdir)/dddsplash.eps            \
	$(picdir)/ddd-aliases.eps          \
	$(picdir)/ddd-all.eps              \
	$(picdir)/ddd-arguments.eps        \
	$(picdir)/ddd-attach.eps           \
	$(picdir)/ddd-backtrace.eps	   \
	$(picdir)/ddd-bp-popup.eps         \
	$(picdir)/ddd-bp-properties.eps    \
	$(picdir)/ddd-button-editor.eps    \
	$(picdir)/ddd-clusters.eps         \
	$(picdir)/ddd-code.eps             \
	$(picdir)/ddd-console.eps          \
	$(picdir)/ddd-data-prefs.eps       \
	$(picdir)/ddd-define-command.eps   \
	$(picdir)/ddd-display.eps          \
	$(picdir)/ddd-drag.eps             \
	$(picdir)/ddd-edit-breakpoints.eps \
	$(picdir)/ddd-edit-displays.eps    \
	$(picdir)/ddd-examine.eps          \
	$(picdir)/ddd-font-prefs.eps       \
	$(picdir)/ddd-general-prefs.eps    \
	$(picdir)/ddd-helpers.eps  	   \
	$(picdir)/ddd-hide-display.eps     \
	$(picdir)/ddd-history.eps          \
	$(picdir)/ddd-layout.eps           \
	$(picdir)/ddd-line-popup.eps       \
	$(picdir)/ddd-locals.eps	   \
	$(picdir)/ddd-open.eps             \
	$(picdir)/ddd-open-session.eps     \
	$(picdir)/ddd-plots.eps            \
	$(picdir)/ddd-print-graph.eps      \
	$(picdir)/ddd-print-output.eps     \
	$(picdir)/ddd-print-popup.eps      \
	$(picdir)/ddd-registers.eps        \
	$(picdir)/ddd-repeats.eps          \
	$(picdir)/ddd-rotate-display.eps   \
	$(picdir)/ddd-save-session.eps     \
	$(picdir)/ddd-select-display.eps   \
	$(picdir)/ddd-separate.eps         \
	$(picdir)/ddd-set.eps	           \
	$(picdir)/ddd-settings.eps         \
	$(picdir)/ddd-shortcuts.eps        \
	$(picdir)/ddd-shortcut-editor.eps  \
	$(picdir)/ddd-signals.eps          \
	$(picdir)/ddd-source-popup.eps     \
	$(picdir)/ddd-source-prefs.eps     \
	$(picdir)/ddd-source.eps           \
	$(picdir)/ddd-startup-prefs.eps    \
	$(picdir)/ddd-status-displays.eps  \
	$(picdir)/ddd-threads.eps          \
	$(picdir)/ddd-tool.eps             \
	$(picdir)/ddd-toolbar.eps          \
	$(picdir)/ddd-toolbars.eps         \
	$(picdir)/ddd-value-tip.eps        \
	$(picdir)/tut-backtrace.eps        \
	$(picdir)/tut-display.eps          \
	$(picdir)/tut-finish.eps           \
	$(picdir)/tut-invoke.eps           \
	$(picdir)/tut-run.eps              \
	$(picdir)/tut-set.eps              \
	$(picdir)/tut-value.eps

PDF_IMAGES = \
	$(picdir)/disclaimer.pdf           \
	$(picdir)/dddback.pdf		   \
	$(picdir)/dddlogo.pdf              \
	$(picdir)/dddsplash.pdf            \
	$(picdir)/ddd-aliases.pdf          \
	$(picdir)/ddd-all.pdf              \
	$(picdir)/ddd-arguments.pdf        \
	$(picdir)/ddd-attach.pdf           \
	$(picdir)/ddd-backtrace.pdf	   \
	$(picdir)/ddd-bp-popup.pdf         \
	$(picdir)/ddd-bp-properties.pdf    \
	$(picdir)/ddd-button-editor.pdf    \
	$(picdir)/ddd-clusters.pdf         \
	$(picdir)/ddd-code.pdf             \
	$(picdir)/ddd-console.pdf          \
	$(picdir)/ddd-data-prefs.pdf       \
	$(picdir)/ddd-define-command.pdf   \
	$(picdir)/ddd-display.pdf          \
	$(picdir)/ddd-drag.pdf             \
	$(picdir)/ddd-edit-breakpoints.pdf \
	$(picdir)/ddd-edit-displays.pdf    \
	$(picdir)/ddd-examine.pdf          \
	$(picdir)/ddd-font-prefs.pdf       \
	$(picdir)/ddd-general-prefs.pdf    \
	$(picdir)/ddd-helpers.pdf  	   \
	$(picdir)/ddd-hide-display.pdf     \
	$(picdir)/ddd-history.pdf          \
	$(picdir)/ddd-layout.pdf           \
	$(picdir)/ddd-line-popup.pdf       \
	$(picdir)/ddd-locals.pdf	   \
	$(picdir)/ddd-open.pdf             \
	$(picdir)/ddd-open-session.pdf     \
	$(picdir)/ddd-plots.pdf            \
	$(picdir)/ddd-print-graph.pdf      \
	$(picdir)/ddd-print-output.pdf     \
	$(picdir)/ddd-print-popup.pdf      \
	$(picdir)/ddd-registers.pdf        \
	$(picdir)/ddd-repeats.pdf          \
	$(picdir)/ddd-rotate-display.pdf   \
	$(picdir)/ddd-save-session.pdf     \
	$(picdir)/ddd-select-display.pdf   \
	$(picdir)/ddd-separate.pdf         \
	$(picdir)/ddd-set.pdf	           \
	$(picdir)/ddd-settings.pdf         \
	$(picdir)/ddd-shortcuts.pdf        \
	$(picdir)/ddd-shortcut-editor.pdf  \
	$(picdir)/ddd-signals.pdf          \
	$(picdir)/ddd-source-popup.pdf     \
	$(picdir)/ddd-source-prefs.pdf     \
	$(picdir)/ddd-source.pdf           \
	$(picdir)/ddd-startup-prefs.pdf    \
	$(picdir)/ddd-status-displays.pdf  \
	$(picdir)/ddd-threads.pdf          \
	$(picdir)/ddd-tool.pdf             \
	$(picdir)/ddd-toolbar.pdf          \
	$(picdir)/ddd-toolbars.pdf         \
	$(picdir)/ddd-value-tip.pdf        \
	$(picdir)/tut-backtrace.pdf        \
	$(picdir)/tut-display.pdf          \
	$(picdir)/tut-finish.pdf           \
	$(picdir)/tut-invoke.pdf           \
	$(picdir)/tut-run.pdf              \
	$(picdir)/tut-set.pdf              \
	$(picdir)/tut-value.pdf

HTML_IMAGES = \
	$(picdir)/disclaimer$(IMGEXT)           \
	$(picdir)/dddback$(IMGEXT)		\
	$(picdir)/dddlogo$(IMGEXT)              \
	$(picdir)/dddsplash$(IMGEXT)            \
	$(picdir)/ddd-aliases$(IMGEXT)          \
	$(picdir)/ddd-all$(IMGEXT)              \
	$(picdir)/ddd-arguments$(IMGEXT)        \
	$(picdir)/ddd-attach$(IMGEXT)           \
	$(picdir)/ddd-backtrace$(IMGEXT)	\
	$(picdir)/ddd-bp-popup$(IMGEXT)         \
	$(picdir)/ddd-bp-properties$(IMGEXT)    \
	$(picdir)/ddd-button-editor$(IMGEXT)    \
	$(picdir)/ddd-clusters$(IMGEXT)         \
	$(picdir)/ddd-code$(IMGEXT)             \
	$(picdir)/ddd-console$(IMGEXT)          \
	$(picdir)/ddd-data-prefs$(IMGEXT)       \
	$(picdir)/ddd-define-command$(IMGEXT)   \
	$(picdir)/ddd-display$(IMGEXT)          \
	$(picdir)/ddd-drag$(IMGEXT)             \
	$(picdir)/ddd-edit-breakpoints$(IMGEXT) \
	$(picdir)/ddd-edit-displays$(IMGEXT)    \
	$(picdir)/ddd-examine$(IMGEXT)          \
	$(picdir)/ddd-font-prefs$(IMGEXT)       \
	$(picdir)/ddd-general-prefs$(IMGEXT)    \
	$(picdir)/ddd-helpers$(IMGEXT)  	\
	$(picdir)/ddd-hide-display$(IMGEXT)     \
	$(picdir)/ddd-history$(IMGEXT)          \
	$(picdir)/ddd-layout$(IMGEXT)           \
	$(picdir)/ddd-line-popup$(IMGEXT)       \
	$(picdir)/ddd-locals$(IMGEXT)	        \
	$(picdir)/ddd-open$(IMGEXT)             \
	$(picdir)/ddd-open-session$(IMGEXT)     \
	$(picdir)/ddd-plots$(IMGEXT)            \
	$(picdir)/ddd-print-graph$(IMGEXT)      \
	$(picdir)/ddd-print-output$(IMGEXT)     \
	$(picdir)/ddd-print-popup$(IMGEXT)      \
	$(picdir)/ddd-registers$(IMGEXT)        \
	$(picdir)/ddd-repeats$(IMGEXT)          \
	$(picdir)/ddd-rotate-display$(IMGEXT)   \
	$(picdir)/ddd-save-session$(IMGEXT)     \
	$(picdir)/ddd-select-display$(IMGEXT)   \
	$(picdir)/ddd-separate$(IMGEXT)         \
	$(picdir)/ddd-set$(IMGEXT)	        \
	$(picdir)/ddd-settings$(IMGEXT)         \
	$(picdir)/ddd-shortcuts$(IMGEXT)        \
	$(picdir)/ddd-shortcut-editor$(IMGEXT)  \
	$(picdir)/ddd-signals$(IMGEXT)          \
	$(picdir)/ddd-source-popup$(IMGEXT)     \
	$(picdir)/ddd-source-prefs$(IMGEXT)     \
	$(picdir)/ddd-source$(IMGEXT)           \
	$(picdir)/ddd-startup-prefs$(IMGEXT)    \
	$(picdir)/ddd-status-displays$(IMGEXT)  \
	$(picdir)/ddd-threads$(IMGEXT)          \
	$(picdir)/ddd-tool$(IMGEXT)             \
	$(picdir)/ddd-toolbar$(IMGEXT)          \
	$(picdir)/ddd-toolbars$(IMGEXT)         \
	$(picdir)/ddd-value-tip$(IMGEXT)        \
	$(picdir)/tut-backtrace$(IMGEXT)        \
	$(picdir)/tut-display$(IMGEXT)          \
	$(picdir)/tut-finish$(IMGEXT)           \
	$(picdir)/tut-invoke$(IMGEXT)           \
	$(picdir)/tut-run$(IMGEXT)              \
	$(picdir)/tut-set$(IMGEXT)              \
	$(picdir)/tut-value$(IMGEXT)

# Included pictures

$(picdir)/ddd-all.eps: \
	$(picdir)/ddd-all.fig $(picdir)/ddd-all.xpm
$(picdir)/ddd-console.eps: \
	$(picdir)/ddd-console.fig $(picdir)/ddd-console.xpm
$(picdir)/ddd-history.eps: \
	$(picdir)/ddd-history.fig $(picdir)/ddd-history.xpm
$(picdir)/ddd-open.eps: \
	$(picdir)/ddd-open.fig $(picdir)/ddd-open.xpm
$(picdir)/ddd-source.eps: \
	$(picdir)/ddd-source.fig $(picdir)/ddd-source.xpm
$(picdir)/ddd-line-popup.eps: \
	$(picdir)/ddd-line-popup.fig $(picdir)/ddd-line-popup.xpm
$(picdir)/ddd-bp-popup.eps: \
	$(picdir)/ddd-bp-popup.fig $(picdir)/ddd-bp-popup.xpm
$(picdir)/ddd-source-popup.eps: \
	$(picdir)/ddd-source-popup.fig $(picdir)/ddd-source-popup.xpm
$(picdir)/ddd-arguments.eps: \
	$(picdir)/ddd-arguments.fig $(picdir)/ddd-arguments.xpm
$(picdir)/ddd-tool.eps: \
	$(picdir)/ddd-tool.fig $(picdir)/ddd-tool.xpm
$(picdir)/ddd-backtrace.eps: \
	$(picdir)/ddd-backtrace.fig $(picdir)/ddd-backtrace.xpm
$(picdir)/ddd-value-tip.eps: \
	$(picdir)/ddd-value-tip.fig $(picdir)/ddd-value-tip.xpm
$(picdir)/ddd-print-popup.eps: \
	$(picdir)/ddd-print-popup.fig $(picdir)/ddd-print-popup.xpm
$(picdir)/ddd-select-display.eps: \
	$(picdir)/ddd-select-display.fig $(picdir)/ddd-select-display.xpm
$(picdir)/ddd-hide-display.eps: \
	$(picdir)/ddd-hide-display.fig $(picdir)/ddd-hide-display.xpm
$(picdir)/ddd-rotate-display.eps: \
	$(picdir)/ddd-rotate-display.fig $(picdir)/ddd-rotate-display.xpm
$(picdir)/ddd-locals.eps: \
	$(picdir)/ddd-locals.fig $(picdir)/ddd-locals.xpm
$(picdir)/ddd-status-displays.eps: \
	$(picdir)/ddd-status-displays.fig $(picdir)/ddd-status-displays.xpm
$(picdir)/ddd-set.eps: \
	$(picdir)/ddd-set.fig $(picdir)/ddd-set.xpm
$(picdir)/ddd-layout.eps: \
	$(picdir)/ddd-layout.fig $(picdir)/ddd-layout.xpm
$(picdir)/ddd-print-graph.eps: \
	$(picdir)/ddd-print-graph.fig $(picdir)/ddd-print-graph.xpm
$(picdir)/ddd-print-output.eps: \
	$(picdir)/ddd-print-output.fig $(picdir)/ddd-graph.ps
$(picdir)/ddd-code.eps: \
	$(picdir)/ddd-code.fig $(picdir)/ddd-code.xpm
$(picdir)/ddd-registers.eps: \
	$(picdir)/ddd-registers.fig $(picdir)/ddd-registers.xpm
$(picdir)/ddd-general-prefs.eps: \
	$(picdir)/ddd-general-prefs.fig $(picdir)/ddd-general-prefs.xpm 
$(picdir)/ddd-helpers.eps: \
	$(picdir)/ddd-helpers.fig $(picdir)/ddd-helpers.xpm 
$(picdir)/ddd-source-prefs.eps: \
	$(picdir)/ddd-source-prefs.fig $(picdir)/ddd-source-prefs.xpm
$(picdir)/ddd-data-prefs.eps: \
	$(picdir)/ddd-data-prefs.fig $(picdir)/ddd-data-prefs.xpm
$(picdir)/ddd-startup-prefs.eps: \
	$(picdir)/ddd-startup-prefs.fig $(picdir)/ddd-startup-prefs.xpm
$(picdir)/ddd-settings.eps: \
	$(picdir)/ddd-settings.fig $(picdir)/ddd-settings.xpm
$(picdir)/ddd-threads.eps: \
	$(picdir)/ddd-threads.fig $(picdir)/ddd-threads.xpm
$(picdir)/ddd-aliases.eps: \
	$(picdir)/ddd-aliases.fig $(picdir)/ddd-aliases.xpm
$(picdir)/ddd-edit-displays.eps: \
	$(picdir)/ddd-edit-displays.fig $(picdir)/ddd-edit-displays.xpm
$(picdir)/ddd-attach.eps: \
	$(picdir)/ddd-attach.fig $(picdir)/ddd-attach.xpm
$(picdir)/ddd-save-session.eps: \
	$(picdir)/ddd-save-session.fig $(picdir)/ddd-save-session.xpm
$(picdir)/ddd-open-session.eps: \
	$(picdir)/ddd-open-session.fig $(picdir)/ddd-open-session.xpm
$(picdir)/ddd-button-editor.eps: \
	$(picdir)/ddd-button-editor.fig $(picdir)/ddd-button-editor.xpm
$(picdir)/ddd-shortcuts.eps: \
	$(picdir)/ddd-shortcuts.fig $(picdir)/ddd-shortcuts.xpm
$(picdir)/ddd-shortcut-editor.eps: \
	$(picdir)/ddd-shortcut-editor.fig $(picdir)/ddd-shortcut-editor.xpm
$(picdir)/ddd-drag.eps: \
	$(picdir)/ddd-drag.fig \
	$(picdir)/ddd-drag1.xpm \
	$(picdir)/ddd-drag2.xpm \
	$(picdir)/ddd-drag3.xpm
$(picdir)/disclaimer.eps: \
	$(picdir)/disclaimer.fig
$(picdir)/ddd-separate.eps: \
	$(picdir)/ddd-separate.fig $(picdir)/ddd-separate.xpm
$(picdir)/ddd-bp-properties.eps: \
	$(picdir)/ddd-bp-properties.fig $(picdir)/ddd-bp-properties.xpm
$(picdir)/ddd-edit-breakpoints.eps: \
	$(picdir)/ddd-edit-breakpoints.fig $(picdir)/ddd-edit-breakpoints.xpm
$(picdir)/ddd-display.eps: \
	$(picdir)/ddd-display.fig $(picdir)/ddd-display.xpm
$(picdir)/ddd-repeats.eps: \
	$(picdir)/ddd-repeats.fig $(picdir)/ddd-repeats.xpm
$(picdir)/ddd-toolbar.eps: \
	$(picdir)/ddd-toolbar.fig $(picdir)/ddd-toolbar.xpm
$(picdir)/ddd-font-prefs.eps: \
	$(picdir)/ddd-font-prefs.fig $(picdir)/ddd-font-prefs.xpm
$(picdir)/ddd-define-command.eps: \
	$(picdir)/ddd-define-command.fig $(picdir)/ddd-define-command.xpm
$(picdir)/ddd-signals.eps: \
	$(picdir)/ddd-signals.fig $(picdir)/ddd-signals.xpm
$(picdir)/ddd-examine.eps: \
	$(picdir)/ddd-examine.fig $(picdir)/ddd-examine.xpm
$(picdir)/ddd-clusters.eps: \
	$(picdir)/ddd-clusters.fig \
	$(picdir)/clustered.ps $(picdir)/unclustered.ps
$(picdir)/ddd-plots.eps: \
	$(picdir)/ddd-plots.fig $(picdir)/ddd-plots.xpm
$(picdir)/ddd-toolbars.eps: \
	$(picdir)/ddd-toolbars.fig \
	$(picdir)/ddd-toolbar-1.xpm \
	$(picdir)/ddd-toolbar-2.xpm \
	$(picdir)/ddd-toolbar-3.xpm \
	$(picdir)/ddd-toolbar-4.xpm
$(picdir)/tut-backtrace.eps: \
	$(picdir)/tut-backtrace.fig $(picdir)/tut-backtrace.xpm
$(picdir)/tut-display.eps: \
	$(picdir)/tut-display.fig $(picdir)/tut-display.xpm
$(picdir)/tut-finish.eps: \
	$(picdir)/tut-finish.fig $(picdir)/tut-finish.xpm
$(picdir)/tut-invoke.eps: \
	$(picdir)/tut-invoke.fig $(picdir)/tut-invoke.xpm
$(picdir)/tut-run.eps: \
	$(picdir)/tut-run.fig $(picdir)/tut-run.xpm
$(picdir)/tut-set.eps: \
	$(picdir)/tut-set.fig $(picdir)/tut-set.xpm
$(picdir)/tut-value.eps: \
	$(picdir)/tut-value.fig $(picdir)/tut-value.xpm


# JPEGs are derived from EPS files

$(picdir)/ddd-all$(IMGEXT):              $(picdir)/ddd-all.eps
$(picdir)/ddd-console$(IMGEXT):          $(picdir)/ddd-console.eps
$(picdir)/ddd-history$(IMGEXT):          $(picdir)/ddd-history.eps
$(picdir)/ddd-open$(IMGEXT):             $(picdir)/ddd-open.eps
$(picdir)/ddd-source$(IMGEXT):           $(picdir)/ddd-source.eps
$(picdir)/ddd-line-popup$(IMGEXT):       $(picdir)/ddd-line-popup.eps
$(picdir)/ddd-bp-popup$(IMGEXT):         $(picdir)/ddd-bp-popup.eps
$(picdir)/ddd-source-popup$(IMGEXT):     $(picdir)/ddd-source-popup.eps
$(picdir)/ddd-arguments$(IMGEXT):        $(picdir)/ddd-arguments.eps
$(picdir)/ddd-tool$(IMGEXT):             $(picdir)/ddd-tool.eps
$(picdir)/ddd-backtrace$(IMGEXT):        $(picdir)/ddd-backtrace.eps
$(picdir)/ddd-value-tip$(IMGEXT):        $(picdir)/ddd-value-tip.eps
$(picdir)/ddd-print-popup$(IMGEXT):      $(picdir)/ddd-print-popup.eps
$(picdir)/ddd-select-display$(IMGEXT):   $(picdir)/ddd-select-display.eps
$(picdir)/ddd-hide-display$(IMGEXT):     $(picdir)/ddd-hide-display.eps
$(picdir)/ddd-rotate-display$(IMGEXT):   $(picdir)/ddd-rotate-display.eps
$(picdir)/ddd-locals$(IMGEXT):           $(picdir)/ddd-locals.eps
$(picdir)/ddd-status-displays$(IMGEXT):  $(picdir)/ddd-status-displays.eps
$(picdir)/ddd-set$(IMGEXT):              $(picdir)/ddd-set.eps
$(picdir)/ddd-layout$(IMGEXT):           $(picdir)/ddd-layout.eps
$(picdir)/ddd-print-graph$(IMGEXT):      $(picdir)/ddd-print-graph.eps
$(picdir)/ddd-print-output$(IMGEXT):     $(picdir)/ddd-print-output.eps
$(picdir)/ddd-code$(IMGEXT):             $(picdir)/ddd-code.eps
$(picdir)/ddd-registers$(IMGEXT):        $(picdir)/ddd-registers.eps
$(picdir)/ddd-general-prefs$(IMGEXT):    $(picdir)/ddd-general-prefs.eps
$(picdir)/ddd-helpers$(IMGEXT):          $(picdir)/ddd-helpers.eps
$(picdir)/ddd-source-prefs$(IMGEXT):     $(picdir)/ddd-source-prefs.eps
$(picdir)/ddd-data-prefs$(IMGEXT):       $(picdir)/ddd-data-prefs.eps
$(picdir)/ddd-startup-prefs$(IMGEXT):    $(picdir)/ddd-startup-prefs.eps
$(picdir)/ddd-settings$(IMGEXT):         $(picdir)/ddd-settings.eps
$(picdir)/ddd-threads$(IMGEXT):          $(picdir)/ddd-threads.eps
$(picdir)/ddd-aliases$(IMGEXT):          $(picdir)/ddd-aliases.eps
$(picdir)/ddd-edit-displays$(IMGEXT):    $(picdir)/ddd-edit-displays.eps
$(picdir)/ddd-attach$(IMGEXT):           $(picdir)/ddd-attach.eps
$(picdir)/ddd-save-session$(IMGEXT):     $(picdir)/ddd-save-session.eps
$(picdir)/ddd-open-session$(IMGEXT):     $(picdir)/ddd-open-session.eps
$(picdir)/ddd-button-editor$(IMGEXT):    $(picdir)/ddd-button-editor.eps
$(picdir)/ddd-shortcuts$(IMGEXT):        $(picdir)/ddd-shortcuts.eps
$(picdir)/ddd-shortcut-editor$(IMGEXT):  $(picdir)/ddd-shortcut-editor.eps
$(picdir)/ddd-drag$(IMGEXT):             $(picdir)/ddd-drag.eps
$(picdir)/disclaimer$(IMGEXT):           $(picdir)/disclaimer.eps
$(picdir)/ddd-separate$(IMGEXT):         $(picdir)/ddd-separate.eps
$(picdir)/ddd-bp-properties$(IMGEXT):    $(picdir)/ddd-bp-properties.eps
$(picdir)/ddd-edit-breakpoints$(IMGEXT): $(picdir)/ddd-edit-breakpoints.eps
$(picdir)/ddd-display$(IMGEXT):          $(picdir)/ddd-display.eps
$(picdir)/ddd-repeats$(IMGEXT):          $(picdir)/ddd-repeats.eps
$(picdir)/ddd-toolbar$(IMGEXT):          $(picdir)/ddd-toolbar.eps
$(picdir)/ddd-font-prefs$(IMGEXT):       $(picdir)/ddd-font-prefs.eps
$(picdir)/ddd-define-command$(IMGEXT):   $(picdir)/ddd-define-command.eps
$(picdir)/ddd-signals$(IMGEXT):          $(picdir)/ddd-signals.eps
$(picdir)/ddd-examine$(IMGEXT):          $(picdir)/ddd-examine.eps
$(picdir)/ddd-clusters$(IMGEXT):         $(picdir)/ddd-clusters.eps
$(picdir)/ddd-plots$(IMGEXT):            $(picdir)/ddd-plots.eps
$(picdir)/ddd-toolbars$(IMGEXT):         $(picdir)/ddd-toolbars.eps
$(picdir)/tut-backtrace$(IMGEXT):	 $(picdir)/tut-backtrace.eps
$(picdir)/tut-display$(IMGEXT):	         $(picdir)/tut-display.eps
$(picdir)/tut-finish$(IMGEXT):	         $(picdir)/tut-finish.eps
$(picdir)/tut-invoke$(IMGEXT):	         $(picdir)/tut-invoke.eps
$(picdir)/tut-run$(IMGEXT):	         $(picdir)/tut-run.eps
$(picdir)/tut-set$(IMGEXT):	         $(picdir)/tut-set.eps
$(picdir)/tut-value$(IMGEXT):	         $(picdir)/tut-value.eps


# PDFs are also derived from EPS files

$(picdir)/ddd-all.pdf:              $(picdir)/ddd-all.eps
$(picdir)/ddd-console.pdf:          $(picdir)/ddd-console.eps
$(picdir)/ddd-history.pdf:          $(picdir)/ddd-history.eps
$(picdir)/ddd-open.pdf:             $(picdir)/ddd-open.eps
$(picdir)/ddd-source.pdf:           $(picdir)/ddd-source.eps
$(picdir)/ddd-line-popup.pdf:       $(picdir)/ddd-line-popup.eps
$(picdir)/ddd-bp-popup.pdf:         $(picdir)/ddd-bp-popup.eps
$(picdir)/ddd-source-popup.pdf:     $(picdir)/ddd-source-popup.eps
$(picdir)/ddd-arguments.pdf:        $(picdir)/ddd-arguments.eps
$(picdir)/ddd-tool.pdf:             $(picdir)/ddd-tool.eps
$(picdir)/ddd-backtrace.pdf:        $(picdir)/ddd-backtrace.eps
$(picdir)/ddd-value-tip.pdf:        $(picdir)/ddd-value-tip.eps
$(picdir)/ddd-print-popup.pdf:      $(picdir)/ddd-print-popup.eps
$(picdir)/ddd-select-display.pdf:   $(picdir)/ddd-select-display.eps
$(picdir)/ddd-hide-display.pdf:     $(picdir)/ddd-hide-display.eps
$(picdir)/ddd-rotate-display.pdf:   $(picdir)/ddd-rotate-display.eps
$(picdir)/ddd-locals.pdf:           $(picdir)/ddd-locals.eps
$(picdir)/ddd-status-displays.pdf:  $(picdir)/ddd-status-displays.eps
$(picdir)/ddd-set.pdf:              $(picdir)/ddd-set.eps
$(picdir)/ddd-layout.pdf:           $(picdir)/ddd-layout.eps
$(picdir)/ddd-print-graph.pdf:      $(picdir)/ddd-print-graph.eps
$(picdir)/ddd-print-output.pdf:     $(picdir)/ddd-print-output.eps
$(picdir)/ddd-code.pdf:             $(picdir)/ddd-code.eps
$(picdir)/ddd-registers.pdf:        $(picdir)/ddd-registers.eps
$(picdir)/ddd-general-prefs.pdf:    $(picdir)/ddd-general-prefs.eps
$(picdir)/ddd-helpers.pdf:          $(picdir)/ddd-helpers.eps
$(picdir)/ddd-source-prefs.pdf:     $(picdir)/ddd-source-prefs.eps
$(picdir)/ddd-data-prefs.pdf:       $(picdir)/ddd-data-prefs.eps
$(picdir)/ddd-startup-prefs.pdf:    $(picdir)/ddd-startup-prefs.eps
$(picdir)/ddd-settings.pdf:         $(picdir)/ddd-settings.eps
$(picdir)/ddd-threads.pdf:          $(picdir)/ddd-threads.eps
$(picdir)/ddd-aliases.pdf:          $(picdir)/ddd-aliases.eps
$(picdir)/ddd-edit-displays.pdf:    $(picdir)/ddd-edit-displays.eps
$(picdir)/ddd-attach.pdf:           $(picdir)/ddd-attach.eps
$(picdir)/ddd-save-session.pdf:     $(picdir)/ddd-save-session.eps
$(picdir)/ddd-open-session.pdf:     $(picdir)/ddd-open-session.eps
$(picdir)/ddd-button-editor.pdf:    $(picdir)/ddd-button-editor.eps
$(picdir)/ddd-shortcuts.pdf:        $(picdir)/ddd-shortcuts.eps
$(picdir)/ddd-shortcut-editor.pdf:  $(picdir)/ddd-shortcut-editor.eps
$(picdir)/ddd-drag.pdf:             $(picdir)/ddd-drag.eps
$(picdir)/disclaimer.pdf:           $(picdir)/disclaimer.eps
$(picdir)/ddd-separate.pdf:         $(picdir)/ddd-separate.eps
$(picdir)/ddd-bp-properties.pdf:    $(picdir)/ddd-bp-properties.eps
$(picdir)/ddd-edit-breakpoints.pdf: $(picdir)/ddd-edit-breakpoints.eps
$(picdir)/ddd-display.pdf:          $(picdir)/ddd-display.eps
$(picdir)/ddd-repeats.pdf:          $(picdir)/ddd-repeats.eps
$(picdir)/ddd-toolbar.pdf:          $(picdir)/ddd-toolbar.eps
$(picdir)/ddd-font-prefs.pdf:       $(picdir)/ddd-font-prefs.eps
$(picdir)/ddd-define-command.pdf:   $(picdir)/ddd-define-command.eps
$(picdir)/ddd-signals.pdf:          $(picdir)/ddd-signals.eps
$(picdir)/ddd-examine.pdf:          $(picdir)/ddd-examine.eps
$(picdir)/ddd-clusters.pdf:         $(picdir)/ddd-clusters.eps
$(picdir)/ddd-plots.pdf:            $(picdir)/ddd-plots.eps
$(picdir)/ddd-toolbars.pdf:         $(picdir)/ddd-toolbars.eps
$(picdir)/tut-backtrace.pdf:	    $(picdir)/tut-backtrace.eps
$(picdir)/tut-display.pdf:          $(picdir)/tut-display.eps
$(picdir)/tut-finish.pdf:           $(picdir)/tut-finish.eps
$(picdir)/tut-invoke.pdf:	    $(picdir)/tut-invoke.eps
$(picdir)/tut-run.pdf:	            $(picdir)/tut-run.eps
$(picdir)/tut-set.pdf:	            $(picdir)/tut-set.eps
$(picdir)/tut-value.pdf:            $(picdir)/tut-value.eps


# The logo comes in XPM source
$(picdir)/dddsplash.eps: $(icondir)/dddsplash.xpm
	$(CONVERT) $(icondir)/dddsplash.xpm $@

$(picdir)/dddsplash.gif: $(icondir)/dddsplash.xpm
	$(CONVERT) $(icondir)/dddsplash.xpm $@

$(picdir)/dddsplash.jpg: $(icondir)/dddsplash.xpm
	$(CONVERT) -quality 100 $(icondir)/dddsplash.xpm $@

$(picdir)/dddsplash.png: $(icondir)/dddsplash.xpm
	$(CONVERT) -quality 100 $(icondir)/dddsplash.xpm $@

$(picdir)/dddlogo.eps: $(icondir)/dddlogo.xpm
	$(CONVERT) $(icondir)/dddlogo.xpm $@

$(picdir)/dddlogo.gif: $(icondir)/dddlogo.xpm
	$(CONVERT) $(icondir)/dddlogo.xpm $@

$(picdir)/dddlogo.jpg: $(icondir)/dddlogo.xpm
	$(CONVERT) -quality 100 $(icondir)/dddlogo.xpm $@

$(picdir)/dddlogo.png: $(icondir)/dddlogo.xpm
	$(CONVERT) -quality 100 $(icondir)/dddlogo.xpm $@

$(picdir)/dddback.eps: $(icondir)/dddback.xpm
	$(CONVERT) $(icondir)/dddback.xpm $@

$(picdir)/dddback.gif: $(icondir)/dddback.xpm
	$(CONVERT) $(icondir)/dddback.xpm $@

$(picdir)/dddback.jpg: $(icondir)/dddback.xpm
	$(CONVERT) -quality 100 $(icondir)/dddback.xpm $@

$(picdir)/dddback.png: $(icondir)/dddback.xpm
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
stamp-h: config.h.in config.status
	CONFIG_FILES= CONFIG_HEADERS=config.h ./config.status
	$(DATE) > $@~ && $(MV) $@~ $@

$(srcdir)/config.h.in: stamp-h.in
$(srcdir)/stamp-h.in: $(srcdir)/acconfig.h
	cd $(srcdir) && $(AUTOHEADER)
	$(DATE) > $@~ && $(MV) $@~ $@

$(srcdir)/acconfig.h: $(srcdir)/acconfig.h.in $(srcdir)/acspecial.h
	( \
		echo "define(srcdir, \`$(srcdir)')dnl"; \
		echo "include(srcdir()/acconfig.h.in)dnl" \
	) | $(M4) > $@~ && $(MV) $@~ $@

$(srcdir)/acspecial.h: $(srcdir)/configure.in
	$(GREP) '^ICE_[a-zA-Z0-9_]* *(' $(srcdir)/configure.in > $@~ && \
	$(MV) $@~ $@

# Special program includes
# Note: we use `build.h' instead of `./build.h', because `./build.h'
# breaks SGI make.
version.h: vr.h
	@$(CMP) vr.h $@ 2> /dev/null || ( $(CAT) vr.h > $@~ && $(MV) $@~ $@ )
build.h: bld.h
	@$(CMP) bld.h $@ 2> /dev/null || ( $(CAT) bld.h > $@~ && $(MV) $@~ $@ )
host.h: $(srcdir)/host.h.in config.status
	$(CAT) $(srcdir)/host.h.in | \
	$(SED) "s%@HOST_CANONICAL@%$(host_canonical)%" | \
	$(SED) "s%@TARGET_CANONICAL@%$(target_canonical)%" > $@~ && \
	$(MV) $@~ $@
root.h: $(srcdir)/root.h.in config.status
	$(CAT) $(srcdir)/root.h.in | \
	$(SED) "s%@ROOT@%$(dddrootdir)%" | \
	$(SED) "s%@ALT_ROOT@%$(pkgdatadir)%" > $@~ && \
	$(MV) $@~ $@


# -----------------------------------------------------------------------------
# Installation.
# -----------------------------------------------------------------------------

dddapp_DATA = Ddd


# -----------------------------------------------------------------------------
# Distribution.
# -----------------------------------------------------------------------------

# The build number.  (This must be set from outside)
REL = 1

# RPM spec
.PHONY:	spec
spec: ../$(ddd)-$(VERSION)-$(REL).spec
../$(ddd)-$(VERSION)-$(REL).spec: $(srcdir)/../ddd.spec.in config.status
	CONFIG_FILES=../ddd.spec CONFIG_HEADERS= $(SHELL) ./config.status
	$(SED) 's/@REL@/$(REL)/g' ../ddd.spec > $@
	$(RM) ../ddd.spec

# Architecture-independent files required for building DDD.
# In a distribution, these should be already built.
#
# Order is significant here!  First source files, then derived files.
PROG_STUFF = vsl-lex.C vsl-gramma.h vsl-gramma.C rxscan.C \
             acconfig.h acspecial.h stamp-h.in config.h \
             $(srcdir)/tips.m4 $(srcdir)/tips.html \
             $(srcdir)/Ddd.in Ddd.ad.h host.h \
             $(vsldir)/ddd.vsl $(srcdir)/ddd.vsl.h \
             vr.h version.h bld.h build.h root.h configinfo.C \
             $(srcdir)/COPYING.gz.C $(srcdir)/NEWS.gz.C \
	     $(srcdir)/ddd.info.txt.gz.C $(srcdir)/ddd.info \
             xlibdir.C eps-code.h .gdbinit

# Additional files to be included in the distribution.
DIST_STUFF = ../$(ddd)-$(VERSION)-$(REL).spec \
             $(man_MANS) $(docdir)/ddd.ps $(docdir)/ddd.pdf \
	     $(htmldir)/index.html $(ICONS)

# Let automake know what we need.
EXTRA_DIST = $(PROG_STUFF) $(DIST_STUFF)

# Make sure all this is built before DDD.
ddd.$(OBJEXT): $(PROG_STUFF)

# These should not go into the distribution.
DISTCLEANFILES = build.h host.h root.h ../config.cache USERINFO

# Have `make maintainer-clean' remove all derived files.
MAINTAINERCLEANFILES = \
	ddd.ps ddd.pdf ddd.html $(PROG_STUFF) \
	$(PS_IMAGES) $(PDF_IMAGES) $(HTML_IMAGES)

diststuff: $(EXTRA_DIST)
