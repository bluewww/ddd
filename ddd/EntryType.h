// $Id$ -*- C++ -*-
// Entry types in `Settings' panel

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

#ifndef _DDD_EntryType_h
#define _DDD_EntryType_h

#ifdef __GNUG__
#pragma interface
#endif

#include "VarArray.h"

enum EntryType
{
    OnOffToggleButtonEntry,	// Create on/off toggle button
    TrueFalseToggleButtonEntry,	// Create true/false toggle button
    NumToggleButtonEntry,	// Create 1/0 toggle button
    NoNumToggleButtonEntry,	// Create 0/1 toggle button
    SensitiveToggleButtonEntry,	// Create sensitive/insensitive toggle button
    DisplayToggleButtonEntry,	// Create display toggle button
    TargetOptionMenuEntry,	// Create `target' option menu
    CheckOptionMenuEntry,       // Create `check' option menu
    OtherOptionMenuEntry,       // Create other option menu
    TextFieldEntry,		// Create text field
    SignalEntry		        // Create signal handler
};

typedef VarArray<EntryType> EntryTypeArray;

#endif // _DDD_EntryType_h
// DON'T ADD ANYTHING BEHIND THIS #endif
