// $Id$ -*- C++ -*-
// Entry types in `Settings' panel

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_EntryType_h
#define _DDD_EntryType_h

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
    SignalEntry,		// Create signal handler
    ThemeEntry,			// Create theme field
    ForkOptionMenuEntry,	// Create `follow-fork-mode' option menu
    DisassemblyOptionMenuEntry,	// Create `disassembly-flavor' option menu
    SchedulerOptionMenuEntry	// Create `scheduler-locking' option menu
};

typedef VarArray<EntryType> EntryTypeArray;

#endif // _DDD_EntryType_h
// DON'T ADD ANYTHING BEHIND THIS #endif
