dnl $Id$
dnl 
dnl Copyright (C) 1997-1998 Technische Universitaet Braunschweig, Germany.
dnl Written by Andreas Zeller <zeller@gnu.org>.
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
dnl `http://www.gnu.org/software/ddd/',
dnl or send a mail to the DDD developers <ddd@gnu.org>.
dnl
dnl
dnl Include a note that we're generated automatically
dnl
dnl
dnl
dnl
dnl M4 setup
dnl --------
dnl
include(ifdef(`srcdir',srcdir()/colors.m4,colors.m4))dnl
dnl
dnl
dnl Typesetting
dnl -----------
dnl
dnl Note: all macros are recognized only with parameters; i.e.
dnl if a macro F(X) is defined, F and F() expand to F.
dnl
dnl DASH makes a dash.
define([DASH], [@tt -@rm ])dnl
dnl
dnl SEP is the postscript arrow symbol.
define([SEP], [@symbol \256])dnl
dnl define([SEP], [@rm  | @bf ])dnl
dnl
dnl DESC(ITEM, DESC) issues `* ITEM - DESC.', with ITEM in bold.
define([DESC], [ifelse([$1], , [[DESC]], [@tt \267 LBL($1) DASH $2.])])dnl
dnl
dnl LBL(FOO) is used for describing the label titled FOO.
dnl LBL(F1, F2, ..., Fn) describes the sequence F1->F2->...->Fn.
dnl Not sure whether we should use @charset (the default charset used 
dnl for buttons) or @bf (for emphasis) here...
define([_LBL], [ifelse([$1], , [[_LBL]], [@bf $1[]])dnl
ifelse([$2], ,,[SEP()_LBL($2)])dnl
ifelse([$3], ,,[SEP()_LBL($3)])dnl
ifelse([$4], ,,[SEP()_LBL($4)])dnl
ifelse([$5], ,,[SEP()_LBL($5)])dnl
ifelse([$6], ,,[SEP()_LBL($6)])dnl
ifelse([$7], ,,[SEP()_LBL($7)])dnl
ifelse([$8], ,,[SEP()_LBL($8)])dnl
ifelse([$9], ,,[SEP()_LBL($9)])dnl
])dnl
define([LBL], [ifelse([$1], , [[LBL]], _LBL($1,$2,$3,$4,$5,$6,$7,$8,$9)@rm )])dnl
dnl
dnl WIDGET(FOO) is used for describing the item with the symbolic name FOO.
define([WIDGET], [ifelse([$1], , [[WIDGET]], [@bs $1@rm ])])dnl
dnl
dnl TEXT(k) is used for describing visible text
define([TEXT], [ifelse([$1], , [[TEXT]], [`@tt $1@rm'])])dnl
dnl
dnl CODE(k) indicates code.
define([CODE], [ifelse([$1], , [[CODE]], [@tt $1@rm ])])dnl
dnl
dnl KBD(k) is used for commands at the GDB prompt
define([KBD], [ifelse([$1], , [[KBD]], [`@tt $1@rm'])])dnl
dnl
dnl KEY(k) is used for keyboard keys
define([KEY], [ifelse([$1], , [[KEY]], [@key $1@rm ])])dnl
dnl
dnl BUTTON(n) sets mouse button N.
define([BUTTON], [ifelse([$1], , [[BUTTON]], [@bs mouse button $1@rm ])])dnl
dnl
dnl SAMP(k) shows a literal sequence of characters.
define([SAMP], [ifelse([$1], , [[SAMP]], [`@tt $1@rm'])])dnl
dnl 
dnl VAR(v) shows a metasyntactic variable.
define([VAR], [ifelse([$1], , [[VAR]], [@sl $1@rm ])])dnl
dnl
dnl FILE(f) shows the name of a file
define([FILE], [ifelse([$1], , [[FILE]], [`@tt $1@rm'])])dnl
dnl
dnl DFN(f) gives a definition
define([DFN], [ifelse([$1], , [[DFN]], [@sl $1@rm ])])dnl
dnl
dnl CITE(c) refers to some external work
define([CITE], [ifelse([$1], , [[CITE]], [@sl $1@rm ])])dnl
dnl
dnl URL(u) refers to an URL
define([URL], [ifelse([$1], , [[URL]], [@tt $1@rm ])])dnl
dnl
dnl EMAIL(e) refers to an e-mail address
define([EMAIL], [ifelse([$1], , [[EMAIL]], [@tt<$1>@rm ])])dnl
dnl
dnl EMPH(t) emphasizes text
define([EMPH], [ifelse([$1], , [[EMPH]], [@sl $1@rm ])])dnl
dnl
dnl STRONG(t) places strong emphasis
define([STRONG], [ifelse([$1], , [[STRONG]], [@bf $1@rm ])])dnl
dnl
dnl ITEM issues a simple centered dot.
define([ITEM], [@tt \267 @rm])dnl
dnl
dnl SUBITEM issues a dash.
define([SUBITEM], DASH)dnl
dnl
dnl QUOTE(AUTHOR, WORK) quotes a tip
define([QUOTE], [\n        - EMPH($1), $2])dnl
dnl
dnl BY(AUTHOR, E-MAIL) is an alternative
define([BY],    [\n        - EMPH($1) EMAIL($2)])dnl
dnl
dnl Constants
dnl ---------
dnl
dnl LBL_FIND_,,, are the labels used for the Find buttons
define([LBL_FIND_BACKWARD], [@charset Find@small<< @charset()@rm ])dnl
define([LBL_FIND_FORWARD],  [@charset Find@small>> @charset()@rm ])dnl
dnl
dnl POSTSCRIPT includes a (TM) symbol
define([POSTSCRIPT], [PostScript@symbol \344 @rm ])dnl
dnl
dnl KEY_RETURN is the symbol of the return key
define([KEY_RETURN], [KEY(RETURN @symbol \277)])dnl
dnl
dnl HELP_KEY is the name of the help key
define([HELP_KEY], [F1])dnl
dnl
dnl KEY_HELP is the symbol of the help key
define([KEY_HELP], [KEY(HELP_KEY)])dnl
dnl
dnl ONE_HALF is the 1/2 symbol.
define([ONE_HALF], [\275])dnl
dnl
dnl TIMES is the x symbol.
define([TIMES], [\327])dnl
dnl
dnl DDD is defined as `@DDD@', which `configure' replaces by `DDD' again
dnl Same applies to `Ddd' and `ddd'.  This allows `configure' to set up
dnl an alternate name for DDD.
define([DDD], [[@DDD@]])dnl
define([Ddd], [[@Ddd@]])dnl
define([ddd], [[@ddd@]])dnl
