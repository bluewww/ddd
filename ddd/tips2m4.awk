# $Id$ -*- awk -*-
# Convert tips of the day to app-resource format

# Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

# This file requires GNU awk (gawk).  Feel free to rewrite it for
# a general AWK and tell us about it.

# Initialize variables.

BEGIN { 
    tips = 1;			# Next tip number
    intip = 0;			# Whether we're in a tip text or not
    seen = 0;			# Whether we've seen a non-comment yet
    }

# Handle comments
/^\#.*/  { next }
/^!.*/   { print; seen = 1; next }

# Pass `include' unchanged, for M4
/^include[(].*/ { print; next }

# Copy blank lines only when we've first seen something.
!seen && !intip && $0 ~ "^$" { next }
seen && !intip && $0 ~ "^$" { print; next }

# Start a tip
!intip && !($0 ~ "^$") {
    printf "Ddd*tips%d: \\\n%s", tips, $0;
    tips++;
    intip = 1; seen = 1;
    next }

# Continue a tip
intip && !($0 ~ "^$") {
    printf "\\n\\\n%s", $0;
    next }

# End a tip
intip && $0 ~ "^$" {
    printf "\n\n";
    intip = 0;
    next }

# Ignore everything else (is there anything else?)
/./ { next }

# Terminate with a final newline
END { print ""; }
