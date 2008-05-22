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

#include <Xmmm/Menu.h>

using namespace Xmmm;

void
Menu::init_signals(void)
{
}

Menu::~Menu(void)
{
    XtDestroyWidget(menu_);
}

::Widget Menu::internal(void)
{
    return menu_;
}



PopupMenu::PopupMenu(::Widget parent, const Xmmm::String &name)
{
    menu_ = XmCreatePopupMenu(parent, (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

PopupMenu::PopupMenu(Xmmm::Widget &parent, const Xmmm::String &name)
{
    menu_ = XmCreatePopupMenu(parent.xt_container(), (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}



PulldownMenu::PulldownMenu(::Widget parent, const Xmmm::String &name)
{
    menu_ = XmCreatePulldownMenu(parent, (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

PulldownMenu::PulldownMenu(Xmmm::Container &parent, const Xmmm::String &name)
{
    menu_ = XmCreatePulldownMenu(parent.xt_container(), (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

