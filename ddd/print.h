// $Id$ -*- C++ -*-
//

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

#ifndef _DDD_print_h
#define _DDD_print_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Button.h>
#endif

#include "ddd.h"

#if defined(IF_XM)
extern void PrintGraphCB (Widget, XtPointer, XtPointer);
extern void PrintPlotCB  (Widget, XtPointer, XtPointer);
extern void PrintAgainCB (Widget, XtPointer, XtPointer);
#else
extern void PrintGraphCB (GUI::Button *);
extern void PrintPlotCB  (GUI::Button *);
extern void PrintAgainCB(GUI::Button *w, long client_data);
#endif

#endif // _DDD_print_h
// DON'T ADD ANYTHING BEHIND THIS #endif
