// $Id$ -*- C++ -*-
// Synchronous GDB questions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_question_h
#define _DDD_question_h

#include "strclass.h"
#include "bool.h"

// Send COMMAND to GDB; return answer (NO_GDB_ANSWER if none)
// TIMEOUT is either 0 (= use default timeout), -1 (= no timeout)
// or maximal time in seconds
string gdb_question(const string& command, int timeout = 0, 
		    bool verbatim = false);

const string NO_GDB_ANSWER(char(-1));

extern bool gdb_question_running; // Is gdb_question running?

// Helper: weed out GDB `verbose' stuff.
void filter_junk(string& answer);

#endif // _DDD_question_h
// DON'T ADD ANYTHING BEHIND THIS #endif
