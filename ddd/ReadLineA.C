// $Id$ -*- C++ -*-
// Agent interface on GNU readline basis

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char ReadLineAgent_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ReadLineA.h"
#include "config.h"

#include <iostream.h>

#if WITH_READLINE
extern "C" {
#include "readline/readline.h"
}
#endif

DEFINE_TYPE_INFO_1(ReadLineAgent, LiterateAgent)

// The currently prompting agent.  There can be only one.
ReadLineAgent *ReadLineAgent::current_prompter = 0;

// Called from readline library as soon as the line is complete
void ReadLineAgent::readline_handler(char *line)
{
    ReadLineAgent *prompter = current_prompter;
    assert (prompter != 0);

    make_sane();

    if (line == 0)
    {
	prompter->inputEOF();
    }
    else
    {
	string input = string(line) + "\n";
	prompter->dispatch(Input, input, input.length());
    }
}

void ReadLineAgent::prompt(const string& prompt_string)
{
#if WITH_READLINE
    // Prompt using PROMPT_STRING
    rl_callback_handler_remove();
    rl_callback_handler_install((char *)prompt_string, readline_handler);
#else
    write(prompt_string);
#endif

    current_prompter = this;
}

void ReadLineAgent::make_sane()
{
#if WITH_READLINE
    // No need to read any longer
    rl_callback_handler_remove();
#endif
    current_prompter = 0;
}

void ReadLineAgent::abort()
{
    make_sane();
    LiterateAgent::abort();
}

int ReadLineAgent::_readInput(char *& data)
{
    if (!running())
    {
	data = "";
	return 0;
    }

#if WITH_READLINE
    if (current_prompter == this)
    {
	rl_callback_read_char();
	data = "";
	return 0;
    }
#endif

    return LiterateAgent::_readInput(data);
}
