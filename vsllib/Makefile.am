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


# Shared data installation
dddrootdir   = $(pkgdatadir)-@VERSION@

# -----------------------------------------------------------------------------
# VSL files.
# -----------------------------------------------------------------------------

vslrootdir = $(dddrootdir)/vsllib
vslroot_DATA = \
	arcs.vsl      \
	builtin.vsl   \
	colors.vsl    \
	ctree.vsl     \
	ddd.vsl       \
	flow.vsl      \
	flowplus.vsl  \
	fonts.vsl     \
	linebreak.vsl \
	list.vsl      \
	slopes.vsl    \
	std.vsl       \
	struct.vsl    \
	symbols.vsl   \
	tab.vsl       \
	tree.vsl      \
	verbose.vsl   \
	vsl.vsl       \
	vsldef.vsl    \
	vsllib.vsl

# Automake invokes `autoconf' without passing the `-l' option :-(
AUTOCONF = autoconf -l ../ddd
