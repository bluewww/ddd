// $Id$
// Predefined VSL functions

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

#ifndef _DDD_VSLBuiltin_h
#define _DDD_VSLBuiltin_h

#include "strclass.h"
#include "bool.h"
#include "ListBox.h"

typedef Box * (*BuiltinFunc)(ListBox *);

struct VSLBuiltin {
    // Return index of given function
    static int resolve(const string& func_name);

    // Return function call of given index
    static BuiltinFunc func(int index);

    // True if associative
    static bool isAssoc(int functionIndex);

    // True if has side effects
    static bool hasSideEffects(int functionIndex);

    // Internal representation ("__op_halign")
    static const char *func_name(int functionIndex);

    // External representation ("(&)")
    static const char *ext_name(int functionIndex);

    // Flag: dump EXT_NAME infix?
    static bool isInfix(int functionIndex);
};

#endif
