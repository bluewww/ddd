// $Id$ -*- C++ -*-
// Some DDD strings

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@gnu.org).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with the DDD Library -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers at `ddd@gnu.org'.

char ddd_strings_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ddd_strings.h"

String ddd_fallback_resources[] = {
#include "Ddd.ad.h"
0
};

#define HUFFTEXT "ddd.man.huff.C"
#include "huffdecode.C"

void ddd_man(ostream& os)
{
    huffdecode(os);
}
    
