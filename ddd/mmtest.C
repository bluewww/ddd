// $Id$ -*- C++ -*-
//

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char mmtest_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>
#include "MakeMenu.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

static MMDesc type_menu[] =
{
    { "dbx", MMPush },
    { "gdb", MMPush },
    MMEnd
};

static MMDesc onoff_menu[] =
{
    { "on",  MMToggle },
    { "off", MMToggle },
    MMEnd
};

static MMDesc items[] = 
{
    { "groupIconify",        MMToggle },
    { "debuggerType",        MMOptionMenu, MMNoCB, type_menu },
    { "globalTabCompletion", MMToggle },
    { "suppressWarnings",    MMRadioPanel, MMNoCB, onoff_menu },
    { "saveHistoryOnExit",   MMToggle },
    MMEnd
};

int main(int argc, char *argv[])
{
    XtAppContext app_context;

    // Initialize it all
    Widget toplevel = XtAppInitialize(&app_context, "Mmtest",
        XtPointer(0), 0, &argc, argv, XtPointer(0), XtPointer(0), 0);

    Widget panel = MMcreatePanel(toplevel, "panel", items);
    (void) panel;

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    return EXIT_SUCCESS;
}

