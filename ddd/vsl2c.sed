# $Id$ -*- text -*-
#
# vsl2c: Convert VSL file to C strings declarations.

# Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
# Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
# `http://www.cs.tu-bs.de/softech/ddd/',
# or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

# Strip C++ comments
s,//[^"]*$,,
s,[^"]*//.*$,,

# Add real line info
s/^# *\([1-9][0-9]*\)[^"]*\("[^"]*\)tmp.c".*/#line \1 \2ddd.vsl"/
s/^# *\([1-9][0-9]*\)[^"]*\("[^"]*"\).*/#line \1 \2/
s/^# *\([1-9][0-9]*\)[^"]*$/#line \1/

# Remove leading and trailing whitespace
s/^\([^" 	]*\)[ 	]*/\1 /g
s/^[ 	]*//
s/[ 	]*$//

# Delete empty lines
# /^[ 	]*$/d

# Quote strings
s/\\/\\&/g
s/"/\\"/g

# Add newlines
s/$/&\\n/

# Enclose in double quotes
s/.*/"&"/
