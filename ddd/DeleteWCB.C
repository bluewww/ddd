// $Id: DeleteWCB.C,v 1.1.1.1 2003/03/03 00:44:05 rockyb Exp $ -*- C++ -*-
// Add deletion callback to window

// Copyright (C) 2000 Universitaet Passau, Germany.
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

char DeleteWindowCallBack_rcsid[] = 
    "$Id: DeleteWCB.C,v 1.1.1.1 2003/03/03 00:44:05 rockyb Exp $";

#include "DeleteWCB.h"
#include "casts.h"

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

void AddDeleteWindowCallback(Widget shell,
			     XtCallbackProc callback, 
			     XtPointer closure)
{
    static Atom WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(shell), XMST("WM_DELETE_WINDOW"), False);

    // We don't invoke XmAddWMProtocols directly, because this LessTif macro
    // causes a compilation warning with `gcc -Wwrite-strings'.

    static Atom WM_PROTOCOLS = 
	XmInternAtom(XtDisplay(shell), XMST("WM_PROTOCOLS"), FALSE);
    
    XmAddProtocolCallback(shell, WM_PROTOCOLS, WM_DELETE_WINDOW, 
			  callback, closure);
}
