// $Id$  -*- C++ -*-
// get filename from file selection box

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char fileSelectionBoxFilename_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "fileSBF.h"
#include "MString.h"

#include <Xm/FileSB.h>

string fileSelectionBoxFilename(XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct *info =
	(XmFileSelectionBoxCallbackStruct *)call_data;

    char *filename = NULL;

    if (XmStringGetLtoR(info->value, MSTRING_DEFAULT_CHARSET, &filename)
	&& filename != NULL)
    {
	string s = filename;
	XtFree(filename);
	return s;
    }

    return "";
}
