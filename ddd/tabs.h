// $Id$ -*- C++ -*-
// Tabulator functions

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_tabs_h
#define _DDD_tabs_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"

// Replace first '\t' by filling up spaces until POS is reached
void tabto(string& s, int pos);

// Replace all '\t' by filling up spaces until multiple of TAB_WIDTH
// is reached; ignore first INDENT characters.
void untabify(string& str, int tab_width = 8, int indent = 0);

// Same, but untabify only for non-standard settings
inline void untabify_if_needed(string& str, int tab_width = 8, int indent = 0)
{
    if (tab_width != 8 || indent % tab_width != 0)
	untabify(str, tab_width, indent);
}

#endif // _DDD_tabs_h
// DON'T ADD ANYTHING BEHIND THIS #endif
