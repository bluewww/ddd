# $Id$
# Set up GDB to debug DDD

# Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
# Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
# `http://www.cs.tu-bs.de/softech/ddd/',
# or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

# Debug the Debugger
# ------------------
# 
# I appreciate that the April 1997 _Communications_
# covers the issue of debugging
# and hope we programmers can be relieved
# from the pain of debugging
# with the help of advanced tools.
# However, all I'm looking for today
# is a well-debugged debugger.
# I have been working
# with C/C++ debuggers from several Unix vendors
# as well as a Microsoft Windows-based debugger.
# All of them more than occasionally fail
# to perform some very basic function,
# such as setting a break point,
# displaying a data structure,
# evaluating an expression,
# and looking up a symbol.
# Some of them simply crash too often.
#
# My observation is that the debugger
# is often buggier
# than the program being debugged.
# One debugging difficulty Eisenstadt points out
# is the inability of the debugging tool.
# This is certainly true.
# I should emphasize that the difficulty
# is often a debugger-reliability problem
# rather than a capability problem.
#
# W. QUOCK, San Mateo, California
# Communications of the ACM, August 1997/Vol.40, No. 8, p. 31

# Conveniences
set verbose on
set print pretty on
set print object on
set print static-members off
set editing on
set history expansion on
set history filename .gdbhist
set history save on
set history size 1000

# String command
define string
    p *((char *)$->rep->s)@($->rep->len + 1)
end
document string
    Print value of last viewed DDD string
end

# Paths, etc
directory .

# Environment
set environment XUSERFILESEARCHPATH .
set environment XAPPLRESDIR .

# Start DDD
set args --trace cxxtest
echo Starting ddd --trace cxxtest...\n
run
