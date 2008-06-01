// $Id$ -*- C++ -*-
// DDD info functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

#ifndef _DDD_show_h
#define _DDD_show_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "ddd.h"
#include "GDBAgent.h"
#if defined(IF_XM)
#include <X11/Intrinsic.h>
#include <X11/Xresource.h>
#else
#include <GUI/Widget.h>
#endif
#include <iostream>

// Help texts
void show_invocation(const string& gdb_command, std::ostream& os);
void show_version(std::ostream& os);
void show_configuration(std::ostream& os);
void show(int (*formatter)(std::ostream& os));

#if defined(IF_XM)
// WWW page
void DDDWWWPageCB(Widget, XtPointer, XtPointer);

// Manual
int ddd_man(std::ostream& os);
void DDDManualCB(Widget, XtPointer, XtPointer);
void GDBManualCB(Widget, XtPointer, XtPointer);

// License
int ddd_license(std::ostream& os);
void DDDLicenseCB(Widget, XtPointer, XtPointer);

// News
int ddd_news(std::ostream& os);
void DDDNewsCB(Widget, XtPointer, XtPointer);
#else
// WWW page
void DDDWWWPageCB(GUI::Widget *);

// Manual
int ddd_man(std::ostream& os);
void DDDManualCB(GUI::Widget *);
void GDBManualCB(GUI::Widget *);

// License
int ddd_license(std::ostream& os);
void DDDLicenseCB(GUI::Widget *);

// News
int ddd_news(std::ostream& os);
void DDDNewsCB(GUI::Widget *);
#endif

#endif // _DDD_show_h
// DON'T ADD ANYTHING BEHIND THIS #endif
