// $Id$
// NORA Exit Callbacks

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

char ExitCB_rcsid[] = 
    "$Id$";

#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>

#include "findParent.h"
#include "ExitCB.h"

// callbacks

// leave program
void ExitCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    exit(0);
}

// same, but with interaction
void ExitAfterConfirmationCB(Widget widget, XtPointer client_data,
			     XtPointer call_data)
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget confirmExit = 
	XmCreateQuestionDialog(findTopLevelShellParent(widget),
			       "confirmExit", args, arg);

    XtAddCallback(confirmExit, XmNokCallback, ExitCB, NULL);

    XtManageChild(confirmExit);
}
