// $Id$
// File Selection Box test

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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char test_file_rcsid[] = 
    "$Id$";

#include <stdlib.h>
#include <iostream.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifdef XmFONTLIST_DEFAULT_TAG
#define DEFAULT_CHARSET XmFONTLIST_DEFAULT_TAG
#else
#define DEFAULT_CHARSET XmSTRING_DEFAULT_CHARSET
#endif

static void echoCB(Widget, XtPointer, XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct *cbs =
	(XmFileSelectionBoxCallbackStruct *)call_data;

    char *filename = 0;

    if (XmStringGetLtoR(cbs->value, DEFAULT_CHARSET, &filename)
	&& filename != 0)
    {
	cout << filename << "\n";
	XtFree(filename);
    }
}

static void doneCB(Widget, XtPointer, XtPointer)
{
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    XtAppContext app_context;

    Widget toplevel = XtAppInitialize(&app_context, "Test-file",
        XtPointer(0), 0, &argc, argv, 0, XtPointer(0), 0);

    Widget box = XmCreateFileSelectionBox(toplevel, "open_file", 0, 0);
    XtManageChild(box);

    Widget help = XmFileSelectionBoxGetChild(box, XmDIALOG_HELP_BUTTON);
    XtSetSensitive(help, False);

    XtAddCallback(box, XmNokCallback, echoCB, 0);
    XtAddCallback(box, XmNcancelCallback, doneCB, 0);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    return EXIT_SUCCESS;
}
