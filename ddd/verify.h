// $Id$ -*- C++ -*-
// Verify widget creation

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_verify_h
#define _DDD_verify_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>

extern void widget_creation_error();

// Some Motif versions return 0 if widget creation fails.
// Rather than crashing, we prefer exiting with a diagnostic message.
inline Widget verify(Widget w)
{
    if (w == 0)
	widget_creation_error();
    return w;
}



#endif // _DDD_verify_h
// DON'T ADD ANYTHING BEHIND THIS #endif
