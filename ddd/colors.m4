dnl $Id$
dnl 
dnl Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
dnl Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
dnl 
dnl This file is part of DDD.
dnl 
dnl DDD is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU General Public
dnl License as published by the Free Software Foundation; either
dnl version 2 of the License, or (at your option) any later version.
dnl 
dnl DDD is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
dnl See the GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public
dnl License along with DDD -- see the file COPYING.
dnl If not, write to the Free Software Foundation, Inc.,
dnl 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
dnl 
dnl DDD is the data display debugger.
dnl For details, see the DDD World-Wide-Web page, 
dnl `http://www.cs.tu-bs.de/softech/ddd/',
dnl or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.
dnl
dnl
dnl M4 setup
dnl --------
dnl
dnl Use `[' and `]' for quoting; the default quotes ``' and `'' 
dnl are used too often within DDD resources.
changequote()changequote([,])dnl
dnl
dnl Disable comment characters
ifdef([changecom],changecom)dnl
dnl
dnl
dnl Undefine some disturbing macros.
dnl This works for BSD M4, SYSV M4, and GNU M4.
undefine([builtin])dnl
undefine([eval])dnl
undefine([file])dnl
undefine([format])dnl
undefine([gnu])dnl
undefine([include])dnl
undefine([index])dnl
undefine([line])dnl
undefine([shift])dnl
undefine([unix])dnl
dnl
dnl Colors
dnl ------
dnl
dnl FOREGROUND_COLOR is the default foreground color.
define([FOREGROUND_COLOR], [black])dnl
dnl
dnl BACKGROUND_COLOR is the default background color.
define([BACKGROUND_COLOR], [grey])dnl
dnl
dnl TEXT_BACKGROUND_COLOR is the background color for text.
define([TEXT_BACKGROUND_COLOR], [grey92])dnl
dnl
dnl RUN_COLOR is the default color for `running' actions.
define([RUN_COLOR], [DarkGreen])dnl
dnl
dnl STOP_COLOR is the default color for `stopping' actions.
define([STOP_COLOR], [red4])dnl
dnl
dnl DATA_COLOR is the default color for `data' actions.
define([DATA_COLOR], [blue4])dnl
dnl
dnl DISABLED_COLOR is the default color for `disabled' items.
define([DISABLED_COLOR], [grey50])dnl
dnl
dnl DISPLAY_COLOR is the default background color for data displays.
define([DISPLAY_COLOR], [white])dnl
dnl
dnl CHANGED_COLOR is the color for changed data displays.
define([CHANGED_COLOR], [#ffffcc])dnl
dnl
dnl TIP_COLOR is the color for toolbar and value tips.
define([TIP_COLOR], [#ffffcc])dnl
