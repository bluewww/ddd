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

#ifndef GTKX_MAIN_H
#define GTKX_MAIN_H

#include <gtkmm/main.h>

namespace GtkX {

    class Window;

    class Main {
	Gtk::Main *main_;
	GtkX::Window *toplevel_;
	int argc_;
	char **argv_;
    public:
	Main(GtkX::Window *&toplevel, const char *classname, const char *sessid,
	     const char *const *fallback_rsc, int argc, char **argv);
	~Main(void);
	GtkX::Window *get_toplevel(void) const;
	int get_argc(void) const;
	char *get_argv(int i) const;
    };

    extern Main *default_main_loop;
}

#endif // GTKX_MAIN_H
