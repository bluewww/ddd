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

#ifndef MULTIPANED_H
#define MULTIPANED_H

#include <gtkmm/paned.h>
#include <list>

#include <GtkX/Container.h>

namespace GtkX {

    class MultiPaned: public GtkX::Container, public Gtk::VPaned
    {
    private:
	std::list<Gtk::VPaned *> xchild_;
    public:
	MultiPaned(void);
	MultiPaned(GtkX::Container &parent, PackOptions po=PACK_SHRINK, 
		   const GtkX::String &name="", const GtkX::String &label="");
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	void on_add(Gtk::Widget* widget);
	bool on_expose_event(GdkEventExpose* event);
	void debug(void);
	static void show_child(Gtk::Widget* widget);
	static void hide_child(Gtk::Widget* widget);
	static void show_child(GtkX::Widget* widget);
	static void hide_child(GtkX::Widget* widget);
#include <GtkX/redirect.h>
    };

}

#endif
