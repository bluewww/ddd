// $Id$ -*- C++ -*-
// Regular expressions used in DDD

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

char regexps_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "regexps.h"
#include "assert.h"
#include <string.h>

#define yy_create_buffer     zz_create_buffer
#define yy_delete_buffer     zz_delete_buffer
#define yy_flex_debug        zz_flex_debug
#define yy_init_buffer       zz_init_buffer
#define yy_flush_buffer      zz_flush_buffer
#define yy_scan_buffer       zz_scan_buffer
#define yy_scan_bytes        zz_scan_bytes
#define yy_scan_string       zz_scan_string
#define yy_load_buffer_state zz_load_buffer_state
#define yy_switch_to_buffer  zz_switch_to_buffer
#define yyin                 zzin
#define yyleng               zzleng
#define yylex                zzlex
#define yylineno             zzlineno
#define yyout                zzout
#define yyrestart            zzrestart
#define yytext               zztext
#define yywrap               zzwrap

#include "rxscan.C"

#define PREFIX_LENGTH 3

static int rx_matcher(void *data, const char *s, int len, int pos)
{
    char *prefix = (char *)data;
    assert(strlen(prefix) == PREFIX_LENGTH);

    the_prefix   = prefix;
    the_text     = s + pos;
    the_length   = len - pos;

    // Restart the scanner
    zzrestart(zzin);

    int ret;
    if (zzlex() == 0)
	ret = -1;		         // not matched
    else
	ret = yyleng - PREFIX_LENGTH;    // # of characters matched

    return ret;
}


// The 111 regexps used in DDD

const regex rxaddr			(rx_matcher, "001");
const regex rxaddress			(rx_matcher, "002");
const regex rxaddress_in		(rx_matcher, "003");
const regex rxaddress_start		(rx_matcher, "004");

#if !WITH_FULL_RX
const regex rxalpha			(rx_matcher, "005");
const regex rxalphanum			(rx_matcher, "006");
#endif

const regex rxarglist			(rx_matcher, "007");
const regex rxat			(rx_matcher, "008");
const regex rxblank			(rx_matcher, "009");
const regex rxblanks			(rx_matcher, "010");
const regex rxblanks_or_tabs		(rx_matcher, "011");
const regex rxbreak_cmd			(rx_matcher, "012");
const regex rxcd_cmd			(rx_matcher, "013");
const regex rxcolons			(rx_matcher, "014");
const regex rxcore			(rx_matcher, "015");
const regex rxcore_cmd			(rx_matcher, "016");
const regex rxcrlf			(rx_matcher, "017");
const regex rxdata			(rx_matcher, "018");
const regex rxdbx_begin_of_display	(rx_matcher, "019");
const regex rxdbx_begin_of_display_info	(rx_matcher, "020");
const regex rxdbx_scope			(rx_matcher, "021");
const regex rxdbxframe			(rx_matcher, "022");
const regex rxdbxfunc			(rx_matcher, "023");
const regex rxdbxfunc2			(rx_matcher, "024");
const regex rxdbxpos			(rx_matcher, "025");
const regex rxdbxwarn1			(rx_matcher, "026");
const regex rxdbxwarn2			(rx_matcher, "027");
const regex rxdebug_cmd			(rx_matcher, "028");
const regex rxdep			(rx_matcher, "029");
const regex rxdisable			(rx_matcher, "030");
const regex rxdisplay			(rx_matcher, "031");
const regex rxdisplay_cmd		(rx_matcher, "032");
const regex rxdisplay_cmd_and_args	(rx_matcher, "033");
const regex rxdont			(rx_matcher, "034");
const regex rxdotdot			(rx_matcher, "035");

#if !WITH_FULL_RX
const regex rxdouble			(rx_matcher, "036");
#endif

const regex rxdown_cmd			(rx_matcher, "037");
const regex rxenable			(rx_matcher, "038");
const regex rxeqeq			(rx_matcher, "039");
const regex rxfile_cmd			(rx_matcher, "040");
const regex rxfilepath			(rx_matcher, "041");
const regex rxfont_id			(rx_matcher, "042");
const regex rxframe			(rx_matcher, "043");
const regex rxframe_cmd			(rx_matcher, "044");
const regex rxfunction_call		(rx_matcher, "045");
const regex rxgdb_begin_of_display	(rx_matcher, "046");
const regex rxgdb_begin_of_display_info	(rx_matcher, "047");
const regex rxgdb_disp_nr		(rx_matcher, "048");
const regex rxgdb_next_display_info	(rx_matcher, "049");
const regex rxgraph_cmd			(rx_matcher, "050");
const regex rxidentifier		(rx_matcher, "051");
const regex rxindex			(rx_matcher, "052");

#if !WITH_FULL_RX
const regex rxint			(rx_matcher, "053");
#endif

const regex rxinvalid_value		(rx_matcher, "054");
const regex rxlookup_cmd		(rx_matcher, "055");

#if !WITH_FULL_RX
const regex rxlowercase			(rx_matcher, "056");
#endif

const regex rxm3comment			(rx_matcher, "057");
const regex rxmake_cmd			(rx_matcher, "058");
const regex rxmembers_of_nl		(rx_matcher, "059");
const regex rxmore_than_one		(rx_matcher, "060");
const regex rxname_colon_int_nl		(rx_matcher, "061");
const regex rxnl			(rx_matcher, "062");
const regex rxnl_int			(rx_matcher, "063");
const regex rxnladdress			(rx_matcher, "064");
const regex rxnladdress_in		(rx_matcher, "065");
const regex rxnlstar			(rx_matcher, "066");
const regex rxnonzero1			(rx_matcher, "067");
const regex rxnonzero2			(rx_matcher, "068");
const regex rxnop_cmd			(rx_matcher, "069");
const regex rxnum			(rx_matcher, "070");
const regex rxop_cmd			(rx_matcher, "071");
const regex rxoptions			(rx_matcher, "072");
const regex rxout_of_range		(rx_matcher, "073");
const regex rxpath_cmd			(rx_matcher, "074");
const regex rxpc			(rx_matcher, "075");
const regex rxpointer1_value		(rx_matcher, "076");
const regex rxpointer2_value		(rx_matcher, "077");
const regex rxprocess1			(rx_matcher, "078");
const regex rxprocess2			(rx_matcher, "079");
const regex rxprompt			(rx_matcher, "080");
const regex rxq				(rx_matcher, "081");
const regex rxreference			(rx_matcher, "082");
const regex rxrefresh_cmd		(rx_matcher, "083");
const regex rxreturn			(rx_matcher, "084");
const regex rxrun_cmd			(rx_matcher, "085");
const regex rxrunning_cmd		(rx_matcher, "086");
const regex rxselect			(rx_matcher, "087");
const regex rxsemicolon_and_brace	(rx_matcher, "088");
const regex rxsep			(rx_matcher, "089");
const regex rxset1_cmd			(rx_matcher, "090");
const regex rxset2_cmd			(rx_matcher, "091");
const regex rxset_args_cmd		(rx_matcher, "092");
const regex rxsetting_cmd		(rx_matcher, "093");
const regex rxsimple			(rx_matcher, "094");
const regex rxsingle_display_cmd	(rx_matcher, "095");
const regex rxspace			(rx_matcher, "096");
const regex rxstopped			(rx_matcher, "097");
const regex rxstr_or_cl_begin		(rx_matcher, "098");
const regex rxstr_or_cl_begin_s		(rx_matcher, "099");
const regex rxstr_or_cl_end		(rx_matcher, "100");
const regex rxstruct_keyword_begin	(rx_matcher, "101");
const regex rxterminated		(rx_matcher, "102");
const regex rxthread_cmd		(rx_matcher, "103");
const regex rxundisplay			(rx_matcher, "104");
const regex rxup_cmd			(rx_matcher, "105");

#if !WITH_FULL_RX
const regex rxuppercase			(rx_matcher, "106");
#endif

const regex rxvtable			(rx_matcher, "107");
const regex rxvtable_entries		(rx_matcher, "108");

#if !WITH_FULL_RX
const regex rxwhite			(rx_matcher, "109");
#endif

const regex rxxdb			(rx_matcher, "110");
const regex rxxdbpos			(rx_matcher, "111");
