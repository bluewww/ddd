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
extern const regex rxcolons;
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
extern const regex rxeqeq;
extern const regex rxfile_cmd;
extern const regex rxfilepath;
extern const regex rxframe;
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
extern const regex rxset_args_cmd;
extern const regex rxsetting_cmd;
extern const regex rxsimple;
extern const regex rxsingle_display_cmd;
extern const regex rxspace;
extern const regex rxstopped;
extern const regex rxstr_or_cl_begin;
extern const regex rxstr_or_cl_end;
extern const regex rxstruct_keyword_begin;
extern const regex rxterminated;
extern const regex rxthread_cmd;
extern const regex rxundisplay;
extern const regex rxup_cmd;
extern const regex rxuppercase;
extern const regex rxvtable;
extern const regex rxvtable_entries;
extern const regex rxwhite;
extern const regex rxxdb;
extern const regex rxxdbpos;

#endif // !RUNTIME_REGEX

#endif // _DDD_regexps_h
// DON'T ADD ANYTHING BEHIND THIS #endif
