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

#ifndef GTKX_WINDOW_H
#define GTKX_WINDOW_H

#include <gtkmm/window.h>

#include <GtkX/Container.h>
#include <GtkX/Main.h>

namespace GtkX {

    class Window: public Shell {
	Gtk::Window *win_;
	void init_signals(void);
    public:
	Window(Main &main, const String &name="", const String &title="");
	Window(const String &name="", const String &title="");
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	Gtk::Container *gtk_container(void);
	~Window(void);
	void set_title(const String &);
	void set_default_size(int width, int height);
	void get_position(int &x, int &y);
	void get_size(int &width, int &height);
	void move(int x, int y);
    };

}

#endif // GTKX_WINDOW_H
