/* $Id$ -*- C++ -*- */
/* DDD regexp scanner */

%{
// Copyright (C) 1997-1998 Technische Universitaet Braunschweig, Germany.
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

static const char *the_text;	// Pointer to next text character
static int the_length;		// Number of characters to read

#define YY_SKIP_YYWRAP
extern "C" int yywrap()
{
    return 1;
}

// Input routine for FLEX
#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) \
{\
    int len = min(max_size, the_length); \
    if (len == 0) \
        result = YY_NULL; \
    else \
    { \
	for (int k = 0; k < len; k++) \
	    buf[k] = *the_text++; \
	result = len; \
	the_length -= len; \
    } \
}

#define YY_NO_UNPUT

// yylex() declaration
#define YY_DECL static const regex *yylex YY_PROTO(( void ))
%}

/* Addresses -- in various formats */
A	(0x[0-9a-fA-F]+|0[0-9a-fA-F]+[hH]|H\'[0-9a-fA-F]+|00+|[(]nil[)]|NIL|null|@[0-9a-fA-F]+|16_[0-9a-f]+)

/* Simple prefix of address */
a	[0H@]

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
x	(0(0|x)[0-9a-f]+|[(]nil[)]|NIL|null)

/* An arbitrary character */
c	(.|\n)

/* A sequence of arbitrary characters */
C	(.|\n)*

/* `virtual table', in all variants */
V	v(irtual{W})?t(a)?bl(e)?

/* A Java identifier */
J       [A-Za-z][A-Za-z0-9.]*


/* This lexer is quite huge.  Sun LEX wants some extra sizes. */
%e 5000
%k 10000
%p 12000
%a 20000
%n 5000
%o 30000

/* We have 111 start conditions, one for each regexp */
%s S001
%s S002
%s S003
%s S004
%s S005
%s S006
%s S007
%s S008
%s S009
%s S010
%s S011
%s S012
%s S013
%s S014
%s S015
%s S016
%s S017
%s S018
%s S019
%s S020
%s S021
%s S022
%s S023
%s S024
%s S025
%s S026
%s S027
%s S028
%s S029
%s S030
%s S031
%s S032
%s S033
%s S034
%s S035
%s S036
%s S037
%s S038
%s S039
%s S040
%s S041
%s S042
%s S043
%s S044
%s S045
%s S046
%s S047
%s S048
%s S049
%s S050
%s S051
%s S052
%s S053
%s S054
%s S055
%s S056
%s S057
%s S058
%s S059
%s S060
%s S061
%s S062
%s S063
%s S064
%s S065
%s S066
%s S067
%s S068
%s S069
%s S070
%s S071
%s S072
%s S073
%s S074
%s S075
%s S076
%s S077
%s S078
%s S079
%s S080
%s S081
%s S082
%s S083
%s S084
%s S085
%s S086
%s S087
%s S088
%s S089
%s S090
%s S091
%s S092
%s S093
%s S094
%s S095
%s S096
%s S097
%s S098
%s S099
%s S100
%s S101
%s S102
%s S103
%s S104
%s S105
%s S106
%s S107
%s S108
%s S109
%s S110
%s S111
%s S112
%s S113

%%
<S001>address{W}{A}			return &rxaddr;
<S002>{A}		 		return &rxaddress;
<S003>{A}{w}in{w}			return &rxaddress_in;
<S004>{a}		 		return &rxaddress_start;
<S005>[A-Za-z]+				return &rxalpha;
<S006>[0-9A-Za-z]+			return &rxalphanum;
<S007>[(][^0-9][^)]*[)]			return &rxarglist;
<S008>[)]?{d}*[1-9]-?{_}\,{_}{d}*[1-9]-?[(]?{W}ta{W}{C}	return &rxat;
<S009>{w}?				return &rxblank;
<S010>[ ]+		 		return &rxblanks;
<S011>{W}				return &rxblanks_or_tabs;
<S012>{_}([th]*(b|bre|brea|break|b[a-z])|cl|cle|clea|clear|d[a-z]|info{W}(li|lin|line)|stop)({W}{C})?			return &rxbreak_cmd;
<S013>{_}cd({W}{C})?			return &rxcd_cmd;
<S014>[^:]*:[^:]*:{_}{D}{C}		return &rxcolons;
<S015>{C}:{C}core{C}		 	return &rxcore;
<S016>{_}(attach|core(-file)?)({W}{C})?	return &rxcore_cmd;
<S017>\r\r*\n				return &rxcrlf;
<S018>{_}[a-z ]*display({W}-?{D}{C})?	return &rxdata;
<S019>[^ \t\n)}][^=\n]*{w}={w}		return &rxdbx_begin_of_display;
<S020>[(]{P}[)]{w}			return &rxdbx_begin_of_display_info;
<S021>[a-zA-Z_$0-9]*`			return &rxdbx_scope;
<S022>[a-zA-Z_$][a-zA-Z_$0-9]*[(]{C}[)]{C}([[]{C}[]]|\,{w}line{w}{C})	return &rxdbxframe;
<S023>[a-zA-Z_$][^:]*:{_}{P}{W}{C}	return &rxdbxfunc;
<S024>{C}line{W}{P}{W}in{W}(file{W})?\"[^\"]*\"\n{C}	return &rxdbxfunc2;
<S025>[[][^]]*:{P}[^]]*]{C}		return &rxdbxpos;
<S026>dbx:{W}warning:.*option{W}only{W}recognized{W}for.*\n	return &rxdbxwarn1;
<S027>dbx:{W}warning:.*unknown{W}language.*\n	return &rxdbxwarn2;
<S028>{_}(debug|givenfile)({W}{C})?     return &rxdebug_cmd;
<S029>{W}no{W}tnedneped{W}		return &rxdep;
<S030>{_}disable{W}display({W}{C})?	return &rxdisable;
<S031>{_}disp(lay)?({W}{C})?		return &rxdisplay;
<S032>{_}disp(lay)?{W}			return &rxdisplay_cmd;
<S033>{_}disp(lay)?{W}{C} 		return &rxdisplay_cmd_and_args;
<S034>Do{_}n['o]t			return &rxdont;
<S035>\/[^/]*[^/.]\/[.][.]\/		return &rxdotdot;
<S036>-?(({D}\.{d}*)|({D})|(\.{D}))([eE][+-]?{D})?	return &rxdouble;
<S037>{_}do(wn)?({W}{C})?		return &rxdown_cmd;
<S038>{_}enable{W}display({W}{C})?	return &rxenable;
<S039>[^{};,\n= ]+{W}={W}[^{}();,\n= ]+{W}={W}{C} return &rxeqeq;
<S040>{_}file({W}{C})?			return &rxfile_cmd;
<S041>[^ "'/]*\/			return &rxfilepath;
<S042>{_}use{W}{C}			return &rxuse_cmd;
<S043>#{D}{W}{A}			return &rxframe;
<S044>{_}(up|do|down|f|fra|fram|frame|top|V|suspend|resume)({W}{C})? return &rxframe_cmd;
<S045>[a-zA-Z0-9_$][(]			return &rxfunction_call;
<S046>{P}:{W}[^ ]			return &rxgdb_begin_of_display;
<S047>{P}:{w}{w}{w}			return &rxgdb_begin_of_display_info;
<S048>{P}		 		return &rxgdb_disp_nr;
<S049>{C}(class[(]{J}[)]|[(]{J}:{P}[)]) return &rxjdbpos;
<S050>{_}graph{W}{C}			return &rxgraph_cmd;
<S051>[A-Za-z_$][A-Za-z0-9_$]*		return &rxidentifier;
<S052>[[]-?{D}]{C}			return &rxindex;
<S053>-?{D}		 		return &rxint;
<S054>([a-zA-Z]+{W}[a-zA-Z]+{C}|{C}[a-zA-Z]+{W}[a-zA-Z]+(\.|>)?)\n?  return &rxinvalid_value;
<S055>{_}(func|v){W}{C}	 		return &rxlookup_cmd;
<S056>[a-z]+		 		return &rxlowercase;
<S057>\(\*{C}\*\){C}			return &rxm3comment;
<S058>{_}(sh{W}|!)?make({W}{C})?	return &rxmake_cmd;
<S059>members{w}of{w}.+:{w}?\n		return &rxmembers_of_nl;
<S060>\[-?{D}\.\.-?{D}\]		return &rxmore_than_one;
<S061>[^ ]+:{D}\n			return &rxname_colon_int_nl;
<S062>{W}\n{W}				return &rxnl;
<S063>\n[1-9]		 		return &rxnl_int;
<S064>\n{_}{A}	 			return &rxnladdress;
<S065>\n{_}{A}{w}in{w} 			return &rxnladdress_in;
<S066>\n{c}[*]		 		return &rxnlstar;
<S067>no[nt][ -]?(0|zero|null)		return &rxnonzero1;
<S068>!={_}(0|zero|null)		return &rxnonzero2;
<S069>{_}(echo|help|show|info|where|shell|sh)({W}{C})?	return &rxnop_cmd;
<S070>@{D}@				return &rxnum;
<S071>{_}(info{W}line)({W}{C})?		return &rxop_cmd;
<S072>-[bcdeiIopPrRsx]			return &rxoptions;
<S073>Line{W}number{W}{D}{W}is{W}out{W}of{W}range{W}for{W} return &rxout_of_range;
<S074>{_}(dir|directory|path)({W}{C})?	return &rxpath_cmd;
<S075>\$pc{_}={_}{A}			return &rxpc;
<S076>[a-zA-Z][a-zA-Z0-9 ]*[a-zA-Z0-9][[][1-9][0-9]*[]][ ] return &rxjdbprompt;
<S077>[ ][]][0-9]*[1-9][[][a-zA-Z0-9][a-zA-Z0-9 ]*[a-zA-Z] return &rxjdbprompt_reverse;
<S078>Process{W}{D}:{_}			return &rxprocess1;
<S079>Process{W}{D}:			return &rxprocess2;
<S080>[(][^ )]*db[^ )]*[)]{w}		return &rxprompt;
<S081>{C}[(]END[)].*			return &rxq;
<S082>@{_}{x}{_}:{C} 			return &rxreference;
<S083>{_}refresh({W}{C})?		return &rxrefresh_cmd;
<S084>{C}([(]press{W}RETURN[)]|Hit{W}RETURN{W}to{W}continue|Type{W}<return>{W}to{W}continue|More{W}[(]n{W}if{W}no[)][?]).* return &rxreturn;
<S085>{_}(r|rer|rerun|ru|run|R)({W}{C})? return &rxrun_cmd;
<S086>{_}(r|ru|run|rer|rerun|c|cont|contin|continu|continue|u|unt|unti|until|s|si|step|stepi|n|ni|next|nexti|j|ju|jump|k|ki|kill|fin|fini|finis|finish|R|S)({W}{C})?	return &rxrunning_cmd;
<S087>Select{w}one{w}of{w}\[{D}{w}-{w}{D}\]:{w}	return &rxselect;
<S088>;{_}[}]				return &rxsemicolon_and_brace;
<S089>[^-_a-zA-Z0-9]			return &rxsep;
<S090>{_}(set{W}var[a-z]*|assign|pq)({W}{C})?	return &rxset1_cmd;
<S091>{_}(set|p|print|output){W}[^=]+=[^=]{C}	return &rxset2_cmd;
<S092>{_}set{W}args({W}{C})?		return &rxset_args_cmd;
<S093>{_}(set|dbxenv){W}{C}		return &rxsetting_cmd;
<S094>([][a-zA-Z0-9_$().]|->)*		return &rxsimple;
<S095>{_}disp(lay){W}[^ ]+		return &rxsingle_display_cmd;
<S096>{C}(--More--|line{w}{d}).* 	return &rxspace;
<S097>Breakpoint{W}{P}\,{W}{A}	return &rxstopped;
<S098>{x}?{_}([(]|[{]|record\n|RECORD\n|RECORD{w}|OBJECT{w}|struct|class|union){C}	return &rxstr_or_cl_begin;
<S099>(>|[[][1-9][0-9]*[]])[ ]          return &rxjdbprompt_nothread;
<S100>([)]|[}]|end\n|END\n|end;|END;)	return &rxstr_or_cl_end;
<S101>{x}?{_}(record\n|RECORD\n|RECORD{w}|OBJECT{w}|struct|class|union){C} return &rxstruct_keyword_begin;
<S102>([Tt]he{W})?[Pp]rogram{W}(exited|terminated|is{W}not{W}being{W}run|no{W}longer{W}exists){C} return &rxterminated;
<S103>{_}thread({W}{C})?		return &rxthread_cmd;
<S104>{_}(delete{W}|un)display({W}{C})?	return &rxundisplay;
<S105>{_}up({W}{C})?			return &rxup_cmd;
<S106>[A-Z]+				return &rxuppercase;
<S107>{A}{W}<[^ \n>]*{W}{V}>[^{},]*[{]{C} return &rxvtable;
<S108>{D}{W}{V}{W}entries\,	        return &rxvtable_entries;
<S109>[ \n\t\r\v\f]+		 	return &rxwhite;
<S110>{C}Standard{W}input:{W}END{C} 	return &rxxdb;
<S111>[^ \t]*:.*:{w}{P}[: ]{C} 	        return &rxxdbpos;
<S112>[<]repeats{W}{P}{W}times[>]       return &rxrepeats;
<S113>[-a-zA-Z0-9_>.]+                  return &rxchain;
<*>.|\n				        return 0;  // Anything else
%%
