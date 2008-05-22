// High-level GUI wrapper for Xmmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <Xm/RowColumn.h>

#include <Xmmm/MenuBar.h>

using namespace Xmmm;

void
MenuBar::init_signals(void)
{
}

MenuBar::MenuBar(::Widget parent, const Xmmm::String &name)
{
    menubar_ = XmCreateMenuBar(parent, (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

MenuBar::MenuBar(Xmmm::Container &parent, const Xmmm::String &name)
{
    menubar_ = XmCreateMenuBar(parent.xt_container(), (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

MenuBar::~MenuBar(void)
{
    XtDestroyWidget(menubar_);
}

::Widget MenuBar::internal(void)
{
    return menubar_;
}

