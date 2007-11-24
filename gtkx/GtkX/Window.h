// -*- C++ -*-

// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef GTKX_WINDOW_H
#define GTKX_WINDOW_H

#include <gtkmm/window.h>

#include <GtkX/Container.h>

namespace GtkX {

    class Window: public Gtk::Window, public Container {
	void init_signals(void);
    protected:
	sigc::signal<bool, GdkEvent *> signal_delete_;
	bool delete_callback(GdkEventAny *data);
    public:
	Window(Gtk::Window *parent, const String &name,
	       int argc=0, char **argv=NULL); // TEMPORARY
	Window(const String &name);
	Gtk::Widget *internal(void);
	Gtk::Container *gtk_container(void);
	~Window(void);
	sigc::signal<bool, GdkEvent *> &signal_delete_event(void);
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
#include <GtkX/redirect.h>
    };

}

#endif // GTKX_WINDOW_H
