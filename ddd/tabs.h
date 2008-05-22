// $Id$ -*- C++ -*-
// Tabulator functions

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Universitaet des Saarlandes, Germany.
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

#ifndef _DDD_tabs_h
#define _DDD_tabs_h

#include "strclass.h"

// Standard tab width
const int DEFAULT_TAB_WIDTH = 8; // Eight characters

// Replace first '\t' by filling up spaces until POS is reached
void tabto(string& s, int pos);

// Replace all '\t' by filling up spaces until multiple of TAB_WIDTH
// is reached; ignore first INDENT characters.
void untabify(string& str, int tab_width = DEFAULT_TAB_WIDTH, int indent = 0);

#endif // _DDD_tabs_h
// DON'T ADD ANYTHING BEHIND THIS #endif
