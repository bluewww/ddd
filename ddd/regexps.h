// $Id$ -*- C++ -*-
// Regular expressions used in DDD

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_regexps_h
#define _DDD_regexps_h

#ifdef __GNUG__
#pragma interface
#endif

#include "config.h"
#include "rxclass.h"

#if !RUNTIME_REGEX

extern const regex rxaddr;
extern const regex rxaddress;
extern const regex rxaddress_in;
extern const regex rxaddress_start;
extern const regex rxalpha;
extern const regex rxalphanum;
extern const regex rxarglist;
extern const regex rxat;
extern const regex rxblank;
extern const regex rxblanks;
extern const regex rxblanks_or_tabs;
extern const regex rxbreak_cmd;
extern const regex rxcd_cmd;
extern const regex rxchain;
extern const regex rxcolons;
extern const regex rxcont_cmd;
extern const regex rxcore;
extern const regex rxcore_cmd;
extern const regex rxcrlf;
extern const regex rxdata;
extern const regex rxdbx_begin_of_display;
extern const regex rxdbx_begin_of_display_info;
extern const regex rxdbx_scope;
extern const regex rxdbxframe;
extern const regex rxdbxfunc;
extern const regex rxdbxfunc2;
extern const regex rxdbxpos;
extern const regex rxdbxwarn1;
extern const regex rxdbxwarn2;
extern const regex rxdebug_cmd;
extern const regex rxdefine_cmd;
extern const regex rxdep;
extern const regex rxdisable;
extern const regex rxdisplay;
extern const regex rxdisplay_cmd;
extern const regex rxdisplay_cmd_and_args;
extern const regex rxdont;
extern const regex rxdotdot;
extern const regex rxdouble;
extern const regex rxdown_cmd;
extern const regex rxenable;
extern const regex rxenter_file_name;
extern const regex rxeqeq;
extern const regex rxfile_cmd;
extern const regex rxfilepath;
extern const regex rxfilepos;
extern const regex rxframe_addr;
extern const regex rxframe_func;
extern const regex rxframe_cmd;
extern const regex rxfunction_call;
extern const regex rxgdb_begin_of_display;
extern const regex rxgdb_begin_of_display_info;
extern const regex rxgdb_disp_nr;
extern const regex rxgraph_cmd;
extern const regex rxidentifier;
extern const regex rxindex;
extern const regex rxint;
extern const regex rxinvalid_value;
extern const regex rxjdbpos;
extern const regex rxjdbprompt;
extern const regex rxjdbprompt_reverse;
extern const regex rxjdbprompt_nothread;
extern const regex rxlist_cmd;
extern const regex rxlist_range;
extern const regex rxlookup_cmd;
extern const regex rxlowercase;
extern const regex rxm3comment;
extern const regex rxmake_cmd;
extern const regex rxmembers_of_nl;
extern const regex rxmore_than_one;
extern const regex rxname_colon_int_nl;
extern const regex rxnl;
extern const regex rxnl_int;
extern const regex rxnladdress;
extern const regex rxnladdress_in;
extern const regex rxnlstar;
extern const regex rxnonzero1;
extern const regex rxnonzero2;
extern const regex rxnop_cmd;
extern const regex rxnum;
extern const regex rxop_cmd;
extern const regex rxoptions;
extern const regex rxout_of_range;
extern const regex rxpath_cmd;
extern const regex rxpc;
extern const regex rxperlpos;
extern const regex rxperlprompt;
extern const regex rxperlref;
extern const regex rxbashpos;
extern const regex rxbashprompt;
extern const regex rxbashref;
extern const regex rxprocess1;
extern const regex rxprocess2;
extern const regex rxprompt;
extern const regex rxq;
extern const regex rxreference;
extern const regex rxrefresh_cmd;
extern const regex rxrepeats;
extern const regex rxreturn;
extern const regex rxrun_cmd;
extern const regex rxrunning_cmd;
extern const regex rxselect;
extern const regex rxsemicolon_and_brace;
extern const regex rxsep;
extern const regex rxset1_cmd;
extern const regex rxset2_cmd;
extern const regex rxset3_cmd;
extern const regex rxset_args_cmd;
extern const regex rxsetting_cmd;
extern const regex rxsimple;
extern const regex rxsingle_display_cmd;
extern const regex rxspace;
extern const regex rxstopped_addr;
extern const regex rxstopped_func;
extern const regex rxstruct_begin;
extern const regex rxstruct_end;
extern const regex rxstruct_keyword_begin;
extern const regex rxterminated;
extern const regex rxthread_cmd;
extern const regex rxdbx_baseclass;
extern const regex rxundisplay;
extern const regex rxup_cmd;
extern const regex rxuppercase;
extern const regex rxuse_cmd;
extern const regex rxvtable;
extern const regex rxvtable_entries;
extern const regex rxwhen;
extern const regex rxwhite;
extern const regex rxxdb;
extern const regex rxxdbpos;
extern const regex rxdoubleindex;

#endif // !RUNTIME_REGEX

#endif // _DDD_regexps_h
// DON'T ADD ANYTHING BEHIND THIS #endif
