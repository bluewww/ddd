// $Id$
// get long widget name

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Container.h>
#endif
#include "longName.h"

char longName_rcsid[] = 
    "$Id$";

#if defined(IF_XM)
string longName(Widget w)
{
    if (w == 0)
	return "<none>";

    string ret = XtName(w);

    Widget parent = XtParent(w);
    if (parent)
	ret = longName(parent) + "." + ret;

    return ret;
}
#else
string longName(GUI::Widget *w)
{
    if (w == 0)
	return "<none>";

    string ret = w->get_name().c_str();

    GUI::Widget *parent = w->get_parent();
    if (parent)
	ret = longName(parent) + "." + ret;

    return ret;
}
#endif
