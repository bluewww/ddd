// $Id$ -*- C++ -*-
// Character sets used in DDD

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_charsets_h
#define _DDD_charsets_h

#ifdef __GNUG__
#pragma interface
#endif

#include "MString.h"

// Character sets

// The fixed-width font used in texts.
#define CHARSET_TT "tt"

// The bold variant of TT.
#define CHARSET_TB "tb"

// Roman text.
#define CHARSET_RM "rm"

// Slanted text
#define CHARSET_SL "sl"

// Bold face text
#define CHARSET_BF "bf"

// Bold slanted text
#define CHARSET_BS "bs"


// Convenience functions
inline MString tt(const string& text) { return MString(text, CHARSET_TT); }
inline MString tb(const string& text) { return MString(text, CHARSET_TB); }
inline MString rm(const string& text) { return MString(text, CHARSET_RM); }
inline MString sl(const string& text) { return MString(text, CHARSET_SL); }
inline MString bf(const string& text) { return MString(text, CHARSET_BF); }
inline MString bs(const string& text) { return MString(text, CHARSET_BS); }

// Newline function
#if XmVERSION == 1 && XmREVISION <= 1
inline MString cr() { return rm("\n"); }
#else
inline MString cr() { return MString(XmStringSeparatorCreate(), true); }
#endif

#endif // _DDD_charsets_h
// DON'T ADD ANYTHING BEHIND THIS #endif
