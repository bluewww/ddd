// $Id$ -*- C++ -*-
// Add deletion callback to window

// Copyright (C) 2000 Universitaet Passau, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char DeleteWindowCallBack_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "DeleteWCB.h"

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

void AddDeleteWindowCallback(Widget shell,
			     XtCallbackProc callback, 
			     XtPointer closure)
{
    static Atom WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(shell), (char *)"WM_DELETE_WINDOW", False);

    // We don't invoke XmAddWMProtocols directly, because this LessTif macro
    // causes a compilation warning with `gcc -Wwrite-strings'.

    static Atom WM_PROTOCOLS = 
	XmInternAtom(XtDisplay(shell), (char *)"WM_PROTOCOLS", FALSE);
    
    XmAddProtocolCallback(shell, WM_PROTOCOLS, WM_DELETE_WINDOW, 
			  callback, closure);
}
