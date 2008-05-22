 // -*- C++ -*-

// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.
// A brief look at QT indicates that this will be required there as
// well.

#ifndef GTKX_MENU_H
#define GTKX_MENU_H

#include <GtkX/MenuShell.h>
#include <gtkmm/menu.h>

// Template for a widget taking a single string constructor argument.

namespace GtkX {

    class Menu: public GtkX::MenuShell, public Gtk::Menu {
    public:
	Menu(GtkX::Widget &parent, const String &name="");
	Menu(Gtk::Widget *parent, const String &name="");
	~Menu(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
#include <GtkX/redirect.h>
	ChildList get_children(void);
    };

    // These are distinct for Motif, identical for Gtk.
    class PopupMenu: public Menu
    {
    public:
	PopupMenu(GtkX::Widget &parent, const String &name=""):
	    Menu(parent, name) {}
	PopupMenu(Gtk::Widget *parent, const String &name=""):
	    Menu(parent, name) {}
	void menu_position(GtkX::EventButton *event) {}
    };
    class PulldownMenu: public Menu
    {
    public:
	PulldownMenu(GtkX::Widget &parent, const String &name=""):
	    Menu(parent, name) {}
	PulldownMenu(Gtk::Widget *parent, const String &name=""):
	    Menu(parent, name) {}
    };
}

#endif // GTKX_MENU_H
