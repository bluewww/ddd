// $Id$ -*- C++ -*-
//

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

char freeXmStringTable_rcsid[] = 
    "$Id$";

#include "freeXmST.h"
#include "LessTifH.h"
#include "config.h"

// Free the XmString table XMLIST of length LIST_LENGTH
void freeXmStringTable (XmStringTable xmlist, int list_length)
{
    for (int i = 0; i < list_length; i++)
	XmStringFree(xmlist[i]);

    // Some Motif versions want XMLIST to be freed, others (notably
    // older Lesstif versions) do not.
#if MOTIF_DIALOGS_OWN_STRING_TABLE
    // do nothing - list is owned by widget
#else
    if (lesstif_version <= 89)
    {
	// do nothing - list is owned by widget
    }
    else
    {
	XtFree((char *)xmlist);
    }
#endif
}
