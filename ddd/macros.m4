dnl $Id$
dnl Macros for Ddd app-defaults file
dnl
dnl Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
dnl Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
dnl 
dnl This file is part of the DDD Library.
dnl 
dnl The DDD Library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Library General Public
dnl License as published by the Free Software Foundation; either
dnl version 2 of the License, or (at your option) any later version.
dnl 
dnl The DDD Library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
dnl See the GNU Library General Public License for more details.
dnl 
dnl You should have received a copy of the GNU Library General Public
dnl License along with the DDD Library -- see the file COPYING.LIB.
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
changequote()dnl
changequote([,])dnl
dnl
dnl Disable comment characters
ifdef([changecom],changecom)dnl
dnl
dnl
dnl Undefine some disturbing macros.
dnl This works for BSD M4, SYSV M4, and GNU M4.
undefine([eval])dnl
undefine([include])dnl
undefine([shift])dnl
undefine([index])dnl
undefine([format])dnl
undefine([unix])dnl
dnl
dnl
dnl Typesetting
dnl -----------
dnl
dnl ITEM issues a simple centered dot.
define(ITEM,@tt \267 @rm)dnl
dnl
dnl SUBITEM issues a dash.
define(SUBITEM,@tt - @rm)dnl
dnl
dnl DESC(ITEM, DESC) issues `* ITEM - DESC.', with ITEM in bold.
define(DESC,@tt \267 LBL($1) - $2.)dnl
dnl
dnl LBL(foo) is used for describing labels
define(LBL,@bf $1@rm )dnl
dnl
dnl TEXT(k) is used for describing visible text
define(TEXT,`@tt $1@rm')dnl
dnl
dnl CODE(k) indicates code.
define(CODE,@tt $1@rm )dnl
dnl
dnl KBD(k) is used for commands at the GDB prompt
define(KBD,`@tt $1@rm')dnl
dnl
dnl KEY(k) is used for keyboard keys
define(KEY,@key $1@rm )dnl
dnl
dnl BUTTON(k) is used for mouse buttons
define(BUTTON,@sl $1@rm )dnl
dnl
dnl SAMP(k) shows a literal sequence of characters.
define(SAMP,`@tt $1@rm')dnl
dnl 
dnl VAR(v) shows a metasyntactic variable.
define(VAR,@sl $1@rm )dnl
dnl
dnl FILE(f) shows the name of a file
define(FILE,`@tt $1@rm')dnl
dnl
dnl DFN(f) gives a definition
define(DFN,@sl $1@rm )dnl
dnl
dnl CITE(c) refers to some external work
define(CITE,@sl $1@rm )dnl
dnl
dnl URL(u) refers to an URL
define(URL,@tt $1@rm )dnl
dnl
dnl EMAIL(e) refers to an e-mail address
define(EMAIL,@tt<$1>@rm )dnl
dnl
dnl EMPH(t) emphasizes text
define(EMPH,@sl $1@rm )dnl
dnl
dnl STRONG(t) places strong emphasis
define(STRONG,@bf $1@rm )dnl
dnl
dnl
dnl Constants
dnl ---------
dnl
dnl LBL_FIND_PREV and LBL_FIND_NEXT are the labels used for the Find buttons
define(LBL_FIND_PREV,Find@small<< @bf()@rm )dnl
define(LBL_FIND_NEXT,Find@small>> @bf()@rm )dnl
dnl
dnl POSTSCRIPT includes a (TM) symbol
define(POSTSCRIPT,PostScript@symbol \344 @rm )dnl
dnl
dnl KEY_RETURN is the symbol of the return key
define(KEY_RETURN,KEY(RETURN @symbol \277))dnl
dnl
dnl ONE_HALF is the 1/2 symbol.
define(ONE_HALF,\275)dnl
dnl
dnl TIMES is the x symbol.
define(TIMES,\327)dnl
dnl
dnl DDD is defined as `@DDD@', which `configure' replaces by `DDD' again
dnl Same applies to `Ddd' and `ddd'.  This allows `configure' to set up
dnl an alternate name for DDD.
define(DDD,[[@DDD@]])dnl
define(Ddd,[[@Ddd@]])dnl
define(ddd,[[@ddd@]])dnl
dnl
dnl
dnl Header
dnl ------
dnl
dnl Insert a header line
! [Generated automatically from Ddd.in.m4 by @M4@.]
