// $Id$ -*- C++ -*-
// VSL library with themes

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

char ThemedVSLLib_rcsid[] = 
    "$Id$";

#include "ThemeVSLL.h"

ThemedVSLLib::ThemedVSLLib()
    : VSLLib(),
      _original_lib(0),
      _optimizeMode(0),
      _theme_list(0)
{}

ThemedVSLLib::ThemedVSLLib(const string& lib_name, 
			   unsigned optimizeMode)
    : VSLLib(lib_name, 0),	// Do not optimize
      _original_lib(dup()),	// Make a copy of unoptimized lib
      _optimizeMode(optimizeMode),
      _theme_list()
{
    optimize(optimizeMode);	// Optimize now
}

ThemedVSLLib::ThemedVSLLib(std::istream& is, unsigned optimizeMode)
    : VSLLib(is, 0),		// Do not optimize
      _original_lib(dup()),	// Make a copy of unoptimized lib
      _optimizeMode(optimizeMode),
      _theme_list()
{
    optimize(optimizeMode);	// Optimize now
}

void ThemedVSLLib::optimize(unsigned optimizeMode)
{
    delete _original_lib;
    _original_lib = dup();

    _optimizeMode = optimizeMode;
    VSLLib::optimize(optimizeMode);
}

void ThemedVSLLib::set_theme_list(const StringArray& themes)
{
    if (_theme_list == themes)
	return;			// Everything stays the same

    // Assign from original library
    assert (_original_lib != 0);
    (void) VSLLib::operator = (*_original_lib);

    // Read in themes
    for (int i = 0; i < themes.size(); i++)
	update(themes[i]);

    // Optimize according to saved mode
    VSLLib::optimize(_optimizeMode);

    // Save list
    _theme_list = themes;
}

// Destructor
ThemedVSLLib::~ThemedVSLLib()
{
    delete _original_lib;
}

// Representation invariant
bool ThemedVSLLib::OK() const
{
    assert (VSLLib::OK());
    assert (_original_lib == 0 || _original_lib->OK());

    return true;
}
