// $Id$ -*- C++ -*-
// DDD manual functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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

char manual_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "show.h"

#include "AppData.h"
#include "LiterateA.h"
#include "configinfo.h"
#include "cook.h"
#include "ddd.h"
#include "host.h"
#include "shell.h"
#include "status.h"
#include "version.h"
#include "filetype.h"
#include "HelpCB.h"

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

//-----------------------------------------------------------------------------
// Manual
//-----------------------------------------------------------------------------

#define HUFFTEXT "ddd.man.huff.C"
#include "huffdecode.C"

void ddd_man(ostream& os)
{
    huffdecode(os);
}

void show_manual()
{
    FILE *pager = 0;
    if (isatty(fileno(stdout)))
    {
	// Try, in that order:
	// 1. The pager specified in the $PAGER environment variable
	// 2. less
	// 3. more
	// 4. cat  (I wonder if this can ever happen)
	string cmd = "less || more || cat";

	char *env_pager = getenv("PAGER");
	if (env_pager != 0)
	    cmd = string(env_pager) + " || " + cmd;
	cmd = string("( ") + cmd + " )";
	pager = popen(cmd, "w");
    }

    if (pager == 0)
    {
	ddd_man(cout);
	cout << flush;
    }
    else
    {
	ostrstream man;
	ddd_man(man);
	string s(man);

	fputs((char *)s, pager);
	pclose(pager);
    }
}


//-----------------------------------------------------------------------------
// Show Manual Page
//-----------------------------------------------------------------------------

void DDDManualCB(Widget w, XtPointer, XtPointer call_data)
{
    StatusDelay delay("Formatting manual page");

    ostrstream man;
    ddd_man(man);
    string s(man);
    ManualStringHelpCB(w, XtPointer((char *)s), call_data);
}
