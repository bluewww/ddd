// $Id$ -*- C++ -*-
// Regular expressions used in DDD

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
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
#include "misc.h"		// min()
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

// Anything not in the list is `not matched'
#define ECHO                 return 0

#include "rxscan.C"

static int rx_matcher(void *data, const char *s, int len, int pos)
{
    int state  = int(data);
    the_text   = s + pos;
    the_length = len - pos;

    // Restart the scanner
    zzrestart(zzin);
    BEGIN(state);

    int ret;
    if (zzlex() == 0)
	ret = -1;		// not matched
    else
	ret = yyleng;		// # of characters matched

    return ret;
}


// All the regexps used in DDD

const regex rxaddr			(rx_matcher, (void *)S001);
const regex rxaddress			(rx_matcher, (void *)S002);
const regex rxaddress_in		(rx_matcher, (void *)S003);
const regex rxaddress_start		(rx_matcher, (void *)S004);

#if !WITH_RUNTIME_REGEX
const regex rxalpha			(rx_matcher, (void *)S005);
const regex rxalphanum			(rx_matcher, (void *)S006);
#endif

const regex rxarglist			(rx_matcher, (void *)S007);
const regex rxat			(rx_matcher, (void *)S008);
const regex rxblank			(rx_matcher, (void *)S009);
const regex rxblanks			(rx_matcher, (void *)S010);
const regex rxblanks_or_tabs		(rx_matcher, (void *)S011);
const regex rxbreak_cmd			(rx_matcher, (void *)S012);
const regex rxcd_cmd			(rx_matcher, (void *)S013);
const regex rxcolons			(rx_matcher, (void *)S014);
const regex rxcore			(rx_matcher, (void *)S015);
const regex rxcore_cmd			(rx_matcher, (void *)S016);
const regex rxcrlf			(rx_matcher, (void *)S017);
const regex rxdata			(rx_matcher, (void *)S018);
const regex rxdbx_begin_of_display	(rx_matcher, (void *)S019);
const regex rxdbx_begin_of_display_info	(rx_matcher, (void *)S020);
const regex rxdbx_scope			(rx_matcher, (void *)S021);
const regex rxdbxframe			(rx_matcher, (void *)S022);
const regex rxdbxfunc			(rx_matcher, (void *)S023);
const regex rxdbxfunc2			(rx_matcher, (void *)S024);
const regex rxdbxpos			(rx_matcher, (void *)S025);
const regex rxdbxwarn1			(rx_matcher, (void *)S026);
const regex rxdbxwarn2			(rx_matcher, (void *)S027);
const regex rxdebug_cmd			(rx_matcher, (void *)S028);
const regex rxdep			(rx_matcher, (void *)S029);
const regex rxdisable			(rx_matcher, (void *)S030);
const regex rxdisplay			(rx_matcher, (void *)S031);
const regex rxdisplay_cmd		(rx_matcher, (void *)S032);
const regex rxdisplay_cmd_and_args	(rx_matcher, (void *)S033);
const regex rxdont			(rx_matcher, (void *)S034);
const regex rxdotdot			(rx_matcher, (void *)S035);

#if !WITH_RUNTIME_REGEX
const regex rxdouble			(rx_matcher, (void *)S036);
#endif

const regex rxdown_cmd			(rx_matcher, (void *)S037);
const regex rxenable			(rx_matcher, (void *)S038);
const regex rxeqeq			(rx_matcher, (void *)S039);
const regex rxfile_cmd			(rx_matcher, (void *)S040);
const regex rxfilepath			(rx_matcher, (void *)S041);
const regex rxuse_cmd			(rx_matcher, (void *)S042);
const regex rxframe			(rx_matcher, (void *)S043);
const regex rxframe_cmd			(rx_matcher, (void *)S044);
const regex rxfunction_call		(rx_matcher, (void *)S045);
const regex rxgdb_begin_of_display	(rx_matcher, (void *)S046);
const regex rxgdb_begin_of_display_info	(rx_matcher, (void *)S047);
const regex rxgdb_disp_nr		(rx_matcher, (void *)S048);
const regex rxjdbpos                	(rx_matcher, (void *)S049);
const regex rxgraph_cmd			(rx_matcher, (void *)S050);
const regex rxidentifier		(rx_matcher, (void *)S051);
const regex rxindex			(rx_matcher, (void *)S052);

#if !WITH_RUNTIME_REGEX
const regex rxint			(rx_matcher, (void *)S053);
#endif

const regex rxinvalid_value		(rx_matcher, (void *)S054);
const regex rxlookup_cmd		(rx_matcher, (void *)S055);

#if !WITH_RUNTIME_REGEX
const regex rxlowercase			(rx_matcher, (void *)S056);
#endif

const regex rxm3comment			(rx_matcher, (void *)S057);
const regex rxmake_cmd			(rx_matcher, (void *)S058);
const regex rxmembers_of_nl		(rx_matcher, (void *)S059);
const regex rxmore_than_one		(rx_matcher, (void *)S060);
const regex rxname_colon_int_nl		(rx_matcher, (void *)S061);
const regex rxnl			(rx_matcher, (void *)S062);
const regex rxnl_int			(rx_matcher, (void *)S063);
const regex rxnladdress			(rx_matcher, (void *)S064);
const regex rxnladdress_in		(rx_matcher, (void *)S065);
const regex rxnlstar			(rx_matcher, (void *)S066);
const regex rxnonzero1			(rx_matcher, (void *)S067);
const regex rxnonzero2			(rx_matcher, (void *)S068);
const regex rxnop_cmd			(rx_matcher, (void *)S069);
const regex rxnum			(rx_matcher, (void *)S070);
const regex rxop_cmd			(rx_matcher, (void *)S071);
const regex rxoptions			(rx_matcher, (void *)S072);
const regex rxout_of_range		(rx_matcher, (void *)S073);
const regex rxpath_cmd			(rx_matcher, (void *)S074);
const regex rxpc			(rx_matcher, (void *)S075);
const regex rxjdbprompt			(rx_matcher, (void *)S076);
const regex rxjdbprompt_reverse         (rx_matcher, (void *)S077);
const regex rxprocess1			(rx_matcher, (void *)S078);
const regex rxprocess2			(rx_matcher, (void *)S079);
const regex rxprompt			(rx_matcher, (void *)S080);
const regex rxq				(rx_matcher, (void *)S081);
const regex rxreference			(rx_matcher, (void *)S082);
const regex rxrefresh_cmd		(rx_matcher, (void *)S083);
const regex rxreturn			(rx_matcher, (void *)S084);
const regex rxrun_cmd			(rx_matcher, (void *)S085);
const regex rxrunning_cmd		(rx_matcher, (void *)S086);
const regex rxselect			(rx_matcher, (void *)S087);
const regex rxsemicolon_and_brace	(rx_matcher, (void *)S088);
const regex rxsep			(rx_matcher, (void *)S089);
const regex rxset1_cmd			(rx_matcher, (void *)S090);
const regex rxset2_cmd			(rx_matcher, (void *)S091);
const regex rxset_args_cmd		(rx_matcher, (void *)S092);
const regex rxsetting_cmd		(rx_matcher, (void *)S093);
const regex rxsimple			(rx_matcher, (void *)S094);
const regex rxsingle_display_cmd	(rx_matcher, (void *)S095);
const regex rxspace			(rx_matcher, (void *)S096);
const regex rxstopped			(rx_matcher, (void *)S097);
const regex rxstr_or_cl_begin		(rx_matcher, (void *)S098);
const regex rxjdbprompt_nothread        (rx_matcher, (void *)S099);
const regex rxstr_or_cl_end		(rx_matcher, (void *)S100);
const regex rxstruct_keyword_begin	(rx_matcher, (void *)S101);
const regex rxterminated		(rx_matcher, (void *)S102);
const regex rxthread_cmd		(rx_matcher, (void *)S103);
const regex rxundisplay			(rx_matcher, (void *)S104);
const regex rxup_cmd			(rx_matcher, (void *)S105);

#if !WITH_RUNTIME_REGEX
const regex rxuppercase			(rx_matcher, (void *)S106);
#endif

const regex rxvtable			(rx_matcher, (void *)S107);
const regex rxvtable_entries		(rx_matcher, (void *)S108);

#if !WITH_RUNTIME_REGEX
const regex rxwhite			(rx_matcher, (void *)S109);
#endif

const regex rxxdb			(rx_matcher, (void *)S110);
const regex rxxdbpos			(rx_matcher, (void *)S111);

const regex rxrepeats                   (rx_matcher, (void *)S112);
const regex rxchain                     (rx_matcher, (void *)S113);
const regex rxwhen                      (rx_matcher, (void *)S114);
