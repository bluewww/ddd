# $Id$
# Java test program makefile

# Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
# Written by Andreas Zeller <zeller@gnu.org>.
# 
# This file is part of the DDD Library.
# 
# The DDD Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# The DDD Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU Library General Public License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with the DDD Library -- see the file COPYING.LIB.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
# DDD is the data display debugger.
# For details, see the DDD World-Wide-Web page, 
# `http://www.gnu.org/software/ddd/',
# or send a mail to the DDD developers <ddd@gnu.org>.

# What we use to compile Java files.
JAVAC = javac

# The flags we use to compile Java files.
JFLAGS = -g

# Native-code compiler
GCJ = gcj

# Its flags
GCJFLAGS = -g

# Default rule
all:	jtest-java
	@:
.PHONY: all

# Implicit rules
.SUFFIXES: .java .class

.java.class:
	$(JAVAC) $(JFLAGS) $<

.java.o:
	$(GCJ) -c $(GCJFLAGS) $<

# Our app
.PHONY: jtest-java
jtest-java: jtest.class Date.class Holiday.class List.class Tree.class
	@:

# The binary
JTEST_OBJECTS = jtest.o	Date.o Holiday.o List.o Tree.o
jtest:	$(JTEST_OBJECTS)
	$(GCJ) -o $@ --main=jtest $(JTEST_OBJECTS)
