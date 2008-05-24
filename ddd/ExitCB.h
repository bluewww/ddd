// $Id$
// Exit Callbacks

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_ExitCB_h
#define _DDD_ExitCB_h

#include <stdlib.h>

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include "gtk_wrapper.h"
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#if defined(IF_XM)
// Exit program
extern void ExitCB(Widget, XtPointer, XtPointer);

// Restart program
extern void RestartCB(Widget, XtPointer, XtPointer);
#else
// Exit program
extern void ExitCB(int status);

// Restart program
extern void RestartCB(void);
#endif

// Save argv
extern void register_argv(const char * const argv[]);

// Save environment
extern void register_environ();

// Return saved environment
extern char **saved_argv();
extern char **saved_environ();

#endif // _DDD_ExitCB_h
// DON'T ADD ANYTHING BEHIND THIS #endif

