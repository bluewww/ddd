// $Id$ -*- C++ -*-
// Create toolbar

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_toolbar_h
#define _DDD_toolbar_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "MakeMenu.h"
#include "ArgField.h"

// Create a toolbar as child of parent, named `NAME_toolbar', having
// the buttons ITEMS.  Return LABEL and ARGFIELD.
extern Widget create_toolbar(Widget parent, const string& name, 
			     MMDesc *items1, MMDesc *items2,
			     Widget& label, ArgField*& argfield,
			     unsigned char label_type);

#endif // _DDD_toolbar_h
// DON'T ADD ANYTHING BEHIND THIS #endif
