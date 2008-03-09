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

char ReadLineAgent_rcsid[] = 
    "$Id$";

#include "ReadLineA.h"
#include "config.h"

#include <iostream>

#if WITH_READLINE
extern "C" {
#include "readline/readline.h"
}
#endif // WITH_READLINE

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
	const string input = string(line) + "\n";
	prompter->dispatch(Input, input.chars(), input.length());
    }
}

void ReadLineAgent::prompt(const string& prompt_string)
{
#if WITH_READLINE
    if (inputIsTerminal())
    {
	// Prompt using PROMPT_STRING
	rl_callback_handler_remove();
	rl_callback_handler_install(prompt_string.chars(),
				    readline_handler);
	current_prompter = this;
	return;
    }
#endif

    write(prompt_string.chars(), prompt_string.length());
    current_prompter = 0;
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

int ReadLineAgent::_readInput(const char *& data)
{
    if (!running())
    {
	// Prevent against being called to drain stdin
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
