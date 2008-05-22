// $Id$ -*- C++ -*-
// Log method entries and exits

// Copyright (C) 2000 Universitaet Passau, Germany.
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

#ifndef _DDD_Logger_h
#define _DDD_Logger_h

#include <iostream>
#include "strclass.h"

#if HAVE_CONFIG_H
#include "config.h"		// HAVE_PRETTY_FUNCTION
#endif

class _Logger {
    string _function;
    string _args;
    static int depth;

public:
    _Logger(const string& function, const string& args)
	: _function(function), _args(args)
    {
	std::clog << ++depth
	     << " " << _function 
	     << "(" << _args << ")"
	     << "...\n";
    }
    ~_Logger()
    {
	std::clog << depth--
	     << " " << _function 
	     << "(" << _args << ")"
	     << "...done.\n";
    }
};

#if HAVE_PRETTY_FUNCTION
#define LOG(args) _Logger lOgGeR(__PRETTY_FUNCTION__, args)
#elif HAVE_FUNCTION
#define LOG(args) _Logger lOgGeR(__FUNCTION__, args)
#else
#define LOG(args) _Logger lOgGeR("function call", args)
#endif

#endif // _DDD_Logger_h
// DON'T ADD ANYTHING BEHIND THIS #endif
