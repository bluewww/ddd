// $Id$ -*- C++ -*-
// Include XPM decls

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_xpm_h
#define _DDD_xpm_h

#ifdef __GNUG__
#pragma interface
#endif

#include "config.h"

#if HAVE_XPM

// The `xpm.h' header file is either in <X11/xpm.h> or <xpm.h>.
// Try <X11/xpm.h> first since this is the default in X11R6.
#if HAVE_X11_XPM_H
#include <X11/xpm.h>

#elif HAVE_XPM_H
#include <xpm.h>

#else // !HAVE_X11_XPM_H && !HAVE_XPM_H

// Hmm.  `configure' says we have XPM but no header file?  Try `xpm.h'.
#include <xpm.h>

#endif // !HAVE_X11_XPM_H && !HAVE_XPM_H
#endif // HAVE_XPM


#ifdef XpmVersion

// Make sure the codes are all defined
#ifndef XpmSuccess
#define XpmSuccess       0
#endif
#ifndef XpmColorError
#define XpmColorError    1
#endif
#ifndef XpmOpenFailed
#define XpmOpenFailed   -1
#endif
#ifndef XpmFileInvalid
#define XpmFileInvalid  -2
#endif
#ifndef XpmNoMemory
#define XpmNoMemory     -3
#endif
#ifndef XpmColorFailed
#define XpmColorFailed  -4
#endif

#endif // XpmVersion

#endif // _DDD_xpm_h
// DON'T ADD ANYTHING BEHIND THIS #endif
