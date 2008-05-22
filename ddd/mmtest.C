// $Id$ -*- C++ -*-
//

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char mmtest_rcsid[] = 
    "$Id$";

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
    MMadjustPanel(items);
    (void) panel;

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    return EXIT_SUCCESS;
}

