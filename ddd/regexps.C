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

// The first DATA_LEN characters are used to choose the actual regexp.
#define DATA_LEN 2

#include "rxscan.C"

// Initial state
#ifndef INTIIAL
#define INITIAL 0
#endif

static int rx_matcher(void *data, const char *s, int len, int pos)
{
#ifndef FLEX_SCANNER
    int required_len = len - pos + DATA_LEN;
    if (required_len > MAX_LEX_TOKEN_SIZE)
    {
	static int max_reported_len = 0;
	if (required_len > max_reported_len)
	{
	    cerr << "Warning: LEX buffer overflow.\n";
	    cerr << "Please raise the value of "
		"MAX_LEX_TOKEN_SIZE in `ddd/rxscan.L'\n"
		"from " << MAX_LEX_TOKEN_SIZE << " to " 
		 << required_len << " or more and recompile.  "
		"Better yet, use FLEX instead of LEX.\n";
	    max_reported_len = required_len;
	}

	// Only look at the first MAX_LEX_TOKEN_SIZE characters
	len = pos + MAX_LEX_TOKEN_SIZE - DATA_LEN;
	assert(len - pos + DATA_LEN == MAX_LEX_TOKEN_SIZE);
    }
#endif

    the_prefix = (char *)data;
    the_text   = s + pos;
    the_length = len - pos;

    assert(strlen(the_prefix) == DATA_LEN);

    // Restart the scanner
    reset_scanner();
    BEGIN(INITIAL);

    int ret;
    if (dddlex() == 0)
	ret = -1;		// not matched
    else
	ret = dddleng - DATA_LEN; // # of characters matched

    return ret;
}


// All the regexps used in DDD

const regex rxaddr			(rx_matcher, (void *)"01");
const regex rxaddress			(rx_matcher, (void *)"02");
const regex rxaddress_in		(rx_matcher, (void *)"03");
const regex rxaddress_start		(rx_matcher, (void *)"04");

#if !WITH_RUNTIME_REGEX
const regex rxalpha			(rx_matcher, (void *)"05");
const regex rxalphanum			(rx_matcher, (void *)"06");
#endif

const regex rxarglist			(rx_matcher, (void *)"07");
const regex rxat			(rx_matcher, (void *)"08");
const regex rxblank			(rx_matcher, (void *)"09");
const regex rxblanks			(rx_matcher, (void *)"10");
const regex rxblanks_or_tabs		(rx_matcher, (void *)"11");
const regex rxbreak_cmd			(rx_matcher, (void *)"12");
const regex rxcd_cmd			(rx_matcher, (void *)"13");
const regex rxcolons			(rx_matcher, (void *)"14");
const regex rxcore			(rx_matcher, (void *)"15");
const regex rxcore_cmd			(rx_matcher, (void *)"16");
const regex rxcrlf			(rx_matcher, (void *)"17");
const regex rxdata			(rx_matcher, (void *)"18");
const regex rxdbx_begin_of_display	(rx_matcher, (void *)"19");
const regex rxdbx_begin_of_display_info	(rx_matcher, (void *)"20");
const regex rxdbx_scope			(rx_matcher, (void *)"21");
const regex rxdbxframe			(rx_matcher, (void *)"22");
const regex rxdbxfunc			(rx_matcher, (void *)"23");
const regex rxdbxfunc2			(rx_matcher, (void *)"24");
const regex rxdbxpos			(rx_matcher, (void *)"25");
const regex rxdbxwarn1			(rx_matcher, (void *)"26");
const regex rxdbxwarn2			(rx_matcher, (void *)"27");
const regex rxdebug_cmd			(rx_matcher, (void *)"28");
const regex rxdep			(rx_matcher, (void *)"29");
const regex rxdisable			(rx_matcher, (void *)"30");
const regex rxdisplay			(rx_matcher, (void *)"31");
const regex rxdisplay_cmd		(rx_matcher, (void *)"32");
const regex rxdisplay_cmd_and_args	(rx_matcher, (void *)"33");
const regex rxdont			(rx_matcher, (void *)"34");
const regex rxdotdot			(rx_matcher, (void *)"35");

#if !WITH_RUNTIME_REGEX
const regex rxdouble			(rx_matcher, (void *)"36");
#endif

const regex rxdown_cmd			(rx_matcher, (void *)"37");
const regex rxenable			(rx_matcher, (void *)"38");
const regex rxeqeq			(rx_matcher, (void *)"39");
const regex rxfile_cmd			(rx_matcher, (void *)"40");
const regex rxfilepath			(rx_matcher, (void *)"41");
const regex rxuse_cmd			(rx_matcher, (void *)"42");
const regex rxframe_addr		(rx_matcher, (void *)"43");
const regex rxframe_cmd			(rx_matcher, (void *)"44");
const regex rxfunction_call		(rx_matcher, (void *)"45");
const regex rxgdb_begin_of_display	(rx_matcher, (void *)"46");
const regex rxgdb_begin_of_display_info	(rx_matcher, (void *)"47");
const regex rxgdb_disp_nr		(rx_matcher, (void *)"48");
const regex rxjdbpos                	(rx_matcher, (void *)"49");
const regex rxgraph_cmd			(rx_matcher, (void *)"50");
const regex rxidentifier		(rx_matcher, (void *)"51");
const regex rxindex			(rx_matcher, (void *)"52");

#if !WITH_RUNTIME_REGEX
const regex rxint			(rx_matcher, (void *)"53");
#endif

const regex rxinvalid_value		(rx_matcher, (void *)"54");
const regex rxlookup_cmd		(rx_matcher, (void *)"55");

#if !WITH_RUNTIME_REGEX
const regex rxlowercase			(rx_matcher, (void *)"56");
#endif

const regex rxm3comment			(rx_matcher, (void *)"57");
const regex rxmake_cmd			(rx_matcher, (void *)"58");
const regex rxmembers_of_nl		(rx_matcher, (void *)"59");
const regex rxmore_than_one		(rx_matcher, (void *)"60");
const regex rxname_colon_int_nl		(rx_matcher, (void *)"61");
const regex rxnl			(rx_matcher, (void *)"62");
const regex rxnl_int			(rx_matcher, (void *)"63");
const regex rxnladdress			(rx_matcher, (void *)"64");
const regex rxnladdress_in		(rx_matcher, (void *)"65");
const regex rxnlstar			(rx_matcher, (void *)"66");
const regex rxnonzero1			(rx_matcher, (void *)"67");
const regex rxnonzero2			(rx_matcher, (void *)"68");
const regex rxnop_cmd			(rx_matcher, (void *)"69");
const regex rxnum			(rx_matcher, (void *)"70");
const regex rxop_cmd			(rx_matcher, (void *)"71");
const regex rxoptions			(rx_matcher, (void *)"72");
const regex rxout_of_range		(rx_matcher, (void *)"73");
const regex rxpath_cmd			(rx_matcher, (void *)"74");
const regex rxpc			(rx_matcher, (void *)"75");
const regex rxjdbprompt			(rx_matcher, (void *)"76");
const regex rxjdbprompt_reverse         (rx_matcher, (void *)"77");
const regex rxprocess1			(rx_matcher, (void *)"78");
const regex rxprocess2			(rx_matcher, (void *)"79");
const regex rxprompt			(rx_matcher, (void *)"80");
const regex rxq				(rx_matcher, (void *)"81");
const regex rxreference			(rx_matcher, (void *)"82");
const regex rxrefresh_cmd		(rx_matcher, (void *)"83");
const regex rxreturn			(rx_matcher, (void *)"84");
const regex rxrun_cmd			(rx_matcher, (void *)"85");
const regex rxrunning_cmd		(rx_matcher, (void *)"86");
const regex rxselect			(rx_matcher, (void *)"87");
const regex rxsemicolon_and_brace	(rx_matcher, (void *)"88");
const regex rxsep			(rx_matcher, (void *)"89");
const regex rxset1_cmd			(rx_matcher, (void *)"90");
const regex rxset2_cmd			(rx_matcher, (void *)"91");
const regex rxset_args_cmd		(rx_matcher, (void *)"92");
const regex rxsetting_cmd		(rx_matcher, (void *)"93");
const regex rxsimple			(rx_matcher, (void *)"94");
const regex rxsingle_display_cmd	(rx_matcher, (void *)"95");
const regex rxspace			(rx_matcher, (void *)"96");
const regex rxstopped_addr		(rx_matcher, (void *)"97");
const regex rxstr_or_cl_begin		(rx_matcher, (void *)"98");
const regex rxjdbprompt_nothread        (rx_matcher, (void *)"99");
const regex rxstr_or_cl_end		(rx_matcher, (void *)"A0");
const regex rxstruct_keyword_begin	(rx_matcher, (void *)"A1");
const regex rxterminated		(rx_matcher, (void *)"A2");
const regex rxthread_cmd		(rx_matcher, (void *)"A3");
const regex rxundisplay			(rx_matcher, (void *)"A4");
const regex rxup_cmd			(rx_matcher, (void *)"A5");

#if !WITH_RUNTIME_REGEX
const regex rxuppercase			(rx_matcher, (void *)"A6");
#endif

const regex rxvtable			(rx_matcher, (void *)"A7");
const regex rxvtable_entries		(rx_matcher, (void *)"A8");

#if !WITH_RUNTIME_REGEX
const regex rxwhite			(rx_matcher, (void *)"A9");
#endif

const regex rxxdb			(rx_matcher, (void *)"B0");
const regex rxxdbpos			(rx_matcher, (void *)"B1");

const regex rxrepeats                   (rx_matcher, (void *)"B2");
const regex rxchain                     (rx_matcher, (void *)"B3");
const regex rxwhen                      (rx_matcher, (void *)"B4");
const regex rxstopped_func              (rx_matcher, (void *)"B5");
const regex rxframe_func                (rx_matcher, (void *)"B6");
const regex rxdefine_cmd                (rx_matcher, (void *)"B7");
const regex rxlist_cmd                  (rx_matcher, (void *)"B8");
const regex rxlist_range                (rx_matcher, (void *)"B9");
const regex rxfilepos                   (rx_matcher, (void *)"C0");
