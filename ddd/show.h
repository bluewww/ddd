// $Id$ -*- C++ -*-
// DDD info functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_show_h
#define _DDD_show_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ddd.h"
#include "GDBAgent.h"

#if defined(IF_MOTIF)

#include <X11/Intrinsic.h>
#include <X11/Xresource.h>

#endif

#if !defined(IF_XM)
#include <GUI/Widget.h>
#endif

#include <iostream>

// Help texts
void show_invocation(const string& gdb_command, std::ostream& os);
void show_version(std::ostream& os);
void show_configuration(std::ostream& os);
void show(int (*formatter)(std::ostream& os));

// WWW page
void DDDWWWPageCB(Widget, XtPointer, XtPointer);

// Manual
int ddd_man(std::ostream& os);
#if defined(IF_XM)
void DDDManualCB(Widget, XtPointer, XtPointer);
void GDBManualCB(Widget, XtPointer, XtPointer);
#else
void DDDManualCB(GUI::Widget *);
void GDBManualCB(GUI::Widget *);
#endif

// License
int ddd_license(std::ostream& os);
#if defined(IF_XM)
void DDDLicenseCB(Widget, XtPointer, XtPointer);
#else
void DDDLicenseCB(GUI::Widget *);
#endif

// News
int ddd_news(std::ostream& os);
#if defined(IF_XM)
void DDDNewsCB(Widget, XtPointer, XtPointer);
#else
void DDDNewsCB(GUI::Widget *);
#endif

#endif // _DDD_show_h
// DON'T ADD ANYTHING BEHIND THIS #endif
