// $Id$
// VSLLib error handling

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

char VSLErr_rcsid[] = 
    "$Id$";

#include <iostream>
#include <sstream>
#include "strclass.h"

#include "VSLLib.h"
#include "VSLDef.h"



// General error interface

// Error handling
void VSLLib::default_echo(const string& msg)
{
    std::cerr << msg << '\n';
}

// Evaluation errors
void VSLLib::eval_echo(const string& errmsg, const VSLDef *d)
{
    if (d == 0 && VSLDef::backtrace)
    {
	// Find topmost def
	for (int i = 0; VSLDef::backtrace[i] != 0; i++)
	    d = VSLDef::backtrace[i];
    }

    // Issue error message
    if (d)
	echo(d->longname() + ": " + errmsg);
    else
	echo(errmsg);


    // Issue backtrace if desired
    if (VSLDef::backtrace && VSLDef::backtrace[0])
    {
	static const char *line = "------------------------------";

	echo(line);
	echo("Current stack:");

	int i;
	for (i = 0; VSLDef::backtrace[i] != 0; i++)
	    ;

	for (i--; i >= 0; i--)
	{
	    std::ostringstream os;
	    os << i << " -- " << VSLDef::backtrace[i]->longname() 
	       << *VSLDef::backtrace_args[i];

	    echo(os);
	}

	echo(line);
    }
}

// Error during evaluation
void VSLLib::eval_error(const string& errmsg, const VSLDef *d)
{
    eval_echo(errmsg, d);
}

// Warning during evaluation
void VSLLib::eval_warning(const string& errmsg, const VSLDef *d)
{
    eval_echo("warning: " + errmsg, d);
}
