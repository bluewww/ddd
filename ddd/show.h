// $Id$ -*- C++ -*-
// DDD info functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_show_h
#define _DDD_show_h

#ifdef __GNUG__
#pragma interface
#endif

#include "ddd.h"
#include "GDBAgent.h"
#include <X11/Intrinsic.h>
#include <X11/Xresource.h>
#include <iostream.h>

// Help texts
void show_invocation(DebuggerType type, ostream& os);
void show_version(ostream& os);
void show_configuration(ostream& os);
void show(int (*formatter)(ostream& os));

// WWW page
void DDDWWWPageCB(Widget, XtPointer, XtPointer);

// Manual
int ddd_man(ostream& os);
void DDDManualCB(Widget, XtPointer, XtPointer);
void GDBManualCB(Widget, XtPointer, XtPointer);

// License
int ddd_license(ostream& os);
void DDDLicenseCB(Widget, XtPointer, XtPointer);

// News
int ddd_news(ostream& os);
void DDDNewsCB(Widget, XtPointer, XtPointer);

// Russian Roulette
void russian_roulette();

#endif // _DDD_show_h
// DON'T ADD ANYTHING BEHIND THIS #endif
