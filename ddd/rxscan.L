/* $Id$ -*- C++ -*- */
/* DDD regexp scanner */

%{
// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char rxscan_rcsid[] = 
    "$Id$";

static const char *the_prefix;	// Pointer to next prefix character
static const char *the_text;	// Pointer to next text character
static int the_length;		// Number of characters to read

inline int do_input()
{
    if (*the_prefix != '\0')
	return *the_prefix++;

    if (the_length-- > 0)
	return *the_text++;

    return '\0';
}

#define YY_SKIP_YYWRAP
extern "C" int yywrap()
{
    return 1;
}

// Input routine for FLEX
#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) \
{\
    int c = do_input(); \
    result = (c == '\0') ? YY_NULL : (buf[0] = c, 1); \
}

#define YY_NO_UNPUT

// yylex() declaration
#define YY_DECL static const regex *yylex YY_PROTO(( void ))
%}

/* Address */
A	(0x[0-9a-fA-F]+|0[0-9a-fA-F]+[hH]|H'[0-9a-fA-F]+|00+|[(]nil[)]|NIL|16_[0-9a-f]+)

/* Simple prefix of address */
a	[0H]

/* Whitespace */
w	[ \f\t]

/* Optional Whitespace */
_	[ \f\t]*

/* Non-empty sequence of Whitespace */
W	[ \f\t]+

/* Digit */
d	[0-9]

/* Non-empty sequence of digits */
D	[0-9]+

/* Positive number */
P	[1-9][0-9]*

/* Hex address */
x	(0(0|x)[0-9a-f]+|[(]nil[)])

/* An arbitrary character */
c	(.|\n)

/* A sequence of arbitrary characters */
C	(.|\n)*


/* This lexer is quite huge.  Sun LEX wants some extra sizes. */
%e 5000
%k 10000
%p 12000
%a 20000
%n 5000
%o 30000

%%
001address{W}{A}			return &rxaddr;
002{A}		 			return &rxaddress;
003{A}{w}in{w}				return &rxaddress_in;
004{a}		 			return &rxaddress_start;
005[A-Za-z]+				return &rxalpha;
006[0-9A-Za-z]+				return &rxalphanum;
007[(][^0-9][^)]*[)]			return &rxarglist;
008[)]?{d}*[1-9]-?{_}\,{_}{d}*[1-9]-?[(]?{W}ta{W}{C}	return &rxat;
009{w}?					return &rxblank;
010[ ]+		 			return &rxblanks;
011{W}					return &rxblanks_or_tabs;
012{_}([th]*(b|bre|brea|break|b[a-z])|cl|cle|clea|clear|d[a-z]|info{W}(li|lin|line)|stop)({W}{C})?			return &rxbreak_cmd;
013{_}cd({W}{C})?			return &rxcd_cmd;
014[^:]*:[^:]*:{_}{D}{C}		return &rxcolons;
015{C}:{C}core{C}		 	return &rxcore;
016{_}(core|core-file)({W}{C})?		return &rxcore_cmd;
017\r\r*\n				return &rxcrlf;
018{_}[a-z ]*display({W}-?{D}{C})?	return &rxdata;
019[^ \t\n)}][^=\n]*{w}={w}		return &rxdbx_begin_of_display;
020[(]{P}[)]{w}				return &rxdbx_begin_of_display_info;
021[a-zA-Z_0-9]*`			return &rxdbx_scope;
022[a-zA-Z_$][a-zA-Z_$0-9]*[(]{C}[)]{C}([[]{C}[]]|\,{w}line{w}{C})	return &rxdbxframe;
023[a-zA-Z_][^:]*:{_}{P}{W}{C}		return &rxdbxfunc;
024{C}line{W}{P}{W}in{W}\"[^\"]*\"\n{C}	return &rxdbxfunc2;
025[[][^]]*:{P}[^]]*]{C}		return &rxdbxpos;
026dbx:{W}warning:.option{W}only{W}recognized{W}for.*\n	return &rxdbxwarn1;
027dbx:{W}warning:.unknown{W}language.*\n	return &rxdbxwarn2;
028{_}debug({W}{C})?			return &rxdebug_cmd;
029{W}no{W}tnedneped{W}			return &rxdep;
030{_}disable{W}display({W}{C})?	return &rxdisable;
031{_}(disp|displ|displa|display)({W}{C})?	return &rxdisplay;
032{_}(disp|displ|displa|display){W}	return &rxdisplay_cmd;
033{_}(disp|displ|displa|display){W}{C} return &rxdisplay_cmd_and_args;
034Do{_}n['o]t				return &rxdont;
035\/[^/]*[^/.]\/[.][.]\/		return &rxdotdot;
036-?(({D}\.{d}*)|({D})|(\.{D}))([eE][+-]?{D})?	return &rxdouble;
037{_}(do|down)({W}{C})?			return &rxdown_cmd;
038{_}enable{W}display({W}{C})?		return &rxenable;
039[^{};,\n= ]+{W}={W}[^{}();,\n= ]+{W}={W}{C} return &rxeqeq;
040{_}file({W}{C})?			return &rxfile_cmd;
041[^ /]*\/				return &rxfilepath;
042[_A-Za-z][-_A-Za-z0-9]*		return &rxfont_id;
043#{D}{W}{A}				return &rxframe;
044{_}(up|do|down|f|fra|fram|frame|top|V)({W}{C})? return &rxframe_cmd;
045[a-zA-Z0-9_$][(]			return &rxfunction_call;
046{P}:{W}[^ ]				return &rxgdb_begin_of_display;
047{P}:{w}{w}{w}			return &rxgdb_begin_of_display_info;
048{P}		 			return &rxgdb_disp_nr;
049\n{P}:{w}{w}{w}			return &rxgdb_next_display_info;
050{_}graph{W}{C}			return &rxgraph_cmd;
051[A-Za-z_][A-Za-z0-9_]*		return &rxidentifier;
052[[]-?{D}]{C}				return &rxindex;
053-?{D}		 		return &rxint;
054([a-zA-Z]+{W}[a-zA-Z]+{C}|{C}[a-zA-Z]+{W}[a-zA-Z]+(\.|>)?)\n?  return &rxinvalid_value;
055{_}(func|v){W}{C}	 		return &rxlookup_cmd;
056[a-z]+		 		return &rxlowercase;
057\(\*{C}\*\){C}			return &rxm3comment;
058{_}(sh{W}|!)?make({W}{C})?		return &rxmake_cmd;
059members{w}of{w}.+:{w}?\n		return &rxmembers_of_nl;
060\[-?{D}\.\.-?{D}\]			return &rxmore_than_one;
061[^ ]+:{D}\n				return &rxname_colon_int_nl;
062{W}\n{W}				return &rxnl;
063\n[1-9]		 		return &rxnl_int;
064\n{_}{A}	 			return &rxnladdress;
065\n{_}{A}{w}in{w} 			return &rxnladdress_in;
066\n{c}[*]		 		return &rxnlstar;
067non-?(0|zero|null)			return &rxnonzero1;
068!={_}(0|zero|null)			return &rxnonzero2;
069{_}(echo|help|show|info|where|shell|sh)({W}{C})?	return &rxnop_cmd;
070@{D}@				return &rxnum;
071{_}(info{W}line)({W}{C})?		return &rxop_cmd;
072-[bcdeiIopPrRsx]			return &rxoptions;
073Line{W}number{W}{D}{W}is{W}out{W}of{W}range{W}for{W} return &rxout_of_range;
074{_}(dir|directory|path)({W}{C})?	return &rxpath_cmd;
075\$pc{_}={_}{A}			return &rxpc;
076([(][^)]+[)]{W})?{A}{C}		return &rxpointer1_value;
077[{][^{}]+[}]{W}{a}{C}		return &rxpointer2_value;
078Process{W}{D}:{_}			return &rxprocess1;
079Process{W}{D}:			return &rxprocess2;
080[(][^ )]*db[^ )]*[)]{w}		return &rxprompt;
081{C}[(]END[)].*			return &rxq;
082@{W}{x}{W}:{C} 			return &rxreference;
083{_}refresh({W}{C})?			return &rxrefresh_cmd;
084{C}([(]press{W}RETURN[)]|Hit{W}RETURN{W}to{W}continue|Type{W}<return>{W}to{W}continue|More{W}[(]n{W}if{W}no[)][?]).* return &rxreturn;
085{_}(r|rer|rerun|ru|run|R)({W}{C})? return &rxrun_cmd;
086{_}(r|ru|run|rer|rerun|c|cont|contin|continu|continue|u|unt|unti|until|s|si|step|stepi|n|ni|next|nexti|j|ju|jump|k|ki|kill|fin|fini|finis|finish|R|S)({W}{C})?	return &rxrunning_cmd;
087Select{w}one{w}of{w}\[{D}{w}-{w}{D}\]:{w}	return &rxselect;
088;{_}[}]				return &rxsemicolon_and_brace;
089[^-_a-zA-Z0-9]			return &rxsep;
090{_}(set{W}var[a-z]*|assign|pq)({W}{C})?	return &rxset1_cmd;
091{_}(set|p|print|output){W}[^=]+=[^=]{C}	return &rxset2_cmd;
092{_}set{W}args({W}{C})?		return &rxset_args_cmd;
093{_}(set|dbxenv){W}{C}		return &rxsetting_cmd;
094([][a-zA-Z0-9_().]|->)*		return &rxsimple;
095{_}(disp|displ|displa|display){W}[^ ]+	return &rxsingle_display_cmd;
096{C}(--More--|line{w}{d}).* 	return &rxspace;
097Breakpoint{W}{P}\,{W}{A}	return &rxstopped;
098{x}?{_}([(]|[{]|record\n|RECORD\n|RECORD{w}|OBJECT{w}|struct|class|union){C}	return &rxstr_or_cl_begin;
099([(]|[{])				return &rxstr_or_cl_begin_s;
100([)]|[}]|end\n|END\n|end;|END;)	return &rxstr_or_cl_end;
101{x}?{_}(record\n|RECORD\n|RECORD{w}|OBJECT{w}|struct|class|union){C} return &rxstruct_keyword_begin;
102([Tt]he{W})?[Pp]rogram{W}(exited|terminated|is{W}not{W}being{W}run|no{W}longer{W}exists){C} return &rxterminated;
103{_}thread({W}{C})?		 return &rxthread_cmd;
104{_}(delete{W}|un)display({W}{C})?	return &rxundisplay;
105{_}up({W}{C})?			return &rxup_cmd;
106[A-Z]+				return &rxuppercase;
107{A}{W}<[^ \n>]*{W}v(irtual{W})?t(a)?bl(e)?>[^{},]*[{]{C}	return &rxvtable;
108[{]{D}{W}vtable{W}entries\,{C}	return &rxvtable_entries;
109[ \n\t\r\v\f]+		 	return &rxwhite;
110{C}Standard{W}input:{W}END{C} 	return &rxxdb;
111[^: \t]*:[^:]*:{w}{P}[: ]{C} 	return &rxxdbpos;
.					return 0;  // Anything else
%%
