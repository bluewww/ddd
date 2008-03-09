// $Id$ -*- C++ -*-
// Agent interface on GNU readline basis

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_ReadLineAgent_h
#define _DDD_ReadLineAgent_h

#include "LiterateA.h"

class ReadLineAgent: public LiterateAgent {
public:
    DECLARE_TYPE_INFO

private:
    static ReadLineAgent *current_prompter;
    static void readline_handler(char *line);

protected:
    // Input data handling
    virtual int _readInput(const char *& data);

    // Clear prompt mode
    static void make_sane();

public:
    // Prompt with PROMPT_STRING
    virtual void prompt(const string& prompt_string);

    // The only constructor available: reading from stdin
    ReadLineAgent(XtAppContext app_context, FILE *in = stdin,
		  FILE *out = stdout, FILE *err = 0,
		  unsigned nTypes = LiterateAgent_NTypes)
	: LiterateAgent(app_context, in, out, err, nTypes)
    {
	assert(in == stdin);
    }

    // Duplicator
    ReadLineAgent(const ReadLineAgent& agent)
	: LiterateAgent(agent)
    {}

    virtual Agent *dup() const { return new ReadLineAgent(*this); }

    virtual void abort();
    virtual ~ReadLineAgent() { make_sane(); }
};

#endif // _DDD_ReadLineAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
