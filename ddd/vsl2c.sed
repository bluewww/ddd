# $Id$
#
# vsl2c: Convert VSL file to C strings declarations.

# Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
# Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
# 675 Mass Ave, Cambridge, MA 02139, USA.
# 
# DDD is the GDB-based data display debugger.
# Contact ddd@ips.cs.tu-bs.de for details.

# Strip C++ comments
s,//[^"]*$,,
s,[^"]*//.*$,,

# Remove leading and trailing whitespace
s/^\([^" 	]*\)[ 	]*/\1 /g
s/^[ 	]*//
s/[ 	]*$//

# Delete empty lines
/^[ 	]*$/d

# Quote strings
s/\\/\\&/g
s/"/\\"/g

# Add newlines
s/[^;=>]$/&\\n/
s/^#/\\n&/

# Enclose in double quotes
s/.*/"&"/
