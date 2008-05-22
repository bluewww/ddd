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

#ifndef _DDD_ThemedVSLLib_h
#define _DDD_ThemedVSLLib_h

#include "VSLLib.h"
#include "StringA.h"

class ThemedVSLLib: public VSLLib {
private:
    VSLLib *_original_lib;
    unsigned _optimizeMode;
    StringArray _theme_list;

    // Assignment
    ThemedVSLLib& operator=(const ThemedVSLLib&);

    // Copy constructor
    ThemedVSLLib(const ThemedVSLLib&);

public:
    // Build
    ThemedVSLLib();
    ThemedVSLLib(const string& lib_name, unsigned optimizeMode = stdOpt);
    ThemedVSLLib(std::istream& s, unsigned optimizeMode = stdOpt);

    // Optimize
    virtual void optimize(unsigned mode = stdOpt);

    // Theme list
    const StringArray& theme_list() const { return _theme_list; }
    void set_theme_list(const StringArray& themes);

    // Destructor
    virtual ~ThemedVSLLib();

    // Representation invariant
    virtual bool OK() const;
};

#endif // _DDD_ThemedVSLLib_h
// DON'T ADD ANYTHING BEHIND THIS #endif
