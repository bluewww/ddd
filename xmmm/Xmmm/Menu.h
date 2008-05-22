// -*- C++ -*-

// High-level GUI wrapper for Xm.

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

#ifndef XMMM_MENU_H
#define XMMM_MENU_H

#include <Xmmm/Container.h>
#include <Xm/RowColumn.h>
#include <X11/Xlib.h>

namespace Xmmm {

    class Menu: public Container {
    protected:
	::Widget menu_;
	void init_signals(void);
    protected:
	sigc::signal<void> signal_clicked_;
	static void clicked_callback(::Widget widget, XtPointer data);
	sigc::signal<void> signal_unmap_;
	static void unmap_callback(::Widget widget, XtPointer data);
    public:
	~Menu(void);
	::Widget internal(void); // TEMPORARY
	sigc::signal<void> &signal_clicked(void);
	sigc::signal<void> &signal_unmap(void);
    };

    class PopupMenu: public Menu {
    public:
	PopupMenu(Xmmm::Widget &parent, const Xmmm::String &name="");
	PopupMenu(::Widget parent, const Xmmm::String &name=""); // TEMPORARY
	void menu_position(XButtonEvent *e);
	void popup(int button, int time);
    };

    class PulldownMenu: public Menu {
    public:
	PulldownMenu(Xmmm::Container &parent, const Xmmm::String &name="");
	PulldownMenu(::Widget parent, const Xmmm::String &name=""); // TEMPORARY
    };

}

#endif // XMMM_MENU_H
