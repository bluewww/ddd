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

#ifndef GTKX_CONTAINER_H
#define GTKX_CONTAINER_H

#include <gtkmm/container.h>
#include <gtkmm/radiobuttongroup.h>
#include <GtkX/Widget.h>

namespace GtkX {

    // Compatibility with Gtkmm:
    enum PackOptions
    {
	PACK_SHRINK,
	PACK_EXPAND_PADDING,
	PACK_EXPAND_WIDGET
    };

    class RadioButton;

    class ChildIterator {
	Glib::ListHandle<Gtk::Widget*>::iterator iter;
    public:
	ChildIterator(void):
	    iter(0) {
	}
	ChildIterator(const ChildIterator &src):
	    iter(src.iter) {
	}
	ChildIterator(const Glib::ListHandle<Gtk::Widget*>::iterator &src):
	    iter(src) {
	}
	ChildIterator &operator=(const ChildIterator &src) {
	    iter = src.iter;
	    return *this;
	}
	bool operator==(const ChildIterator &src) {
	    return (iter == src.iter);
	}
	bool operator!=(const ChildIterator &src) {
	    return (iter != src.iter);
	}
	Widget *operator*(void) {
	    return (Widget *)(*iter)->get_data(gtkx_super_quark);
	}
	ChildIterator &operator++(void) {
	    ++iter;
	    return *this;
	}
	const ChildIterator operator++(int) {
	    const ChildIterator tmp(*this);
	    iter++;
	    return tmp;
	}
    };

    class ChildList {
	Glib::ListHandle<Gtk::Widget*> list;
	// No copy assignment
	ChildList &operator=(const ChildList &src);
    public:
	ChildList(const ChildList &src):
	    list(src.list) {
	}
	ChildList(const Glib::ListHandle<Gtk::Widget*> &src):
	    list(src) {
	}
	ChildIterator begin(void) {
	    return ChildIterator(list.begin());
	}
	ChildIterator end(void) {
	    return ChildIterator(list.end());
	}
    };

    class Container: public GtkX::Widget {
    protected:
	Gtk::RadioButtonGroup grp;
    public:
	Container(void);
	~Container(void);
	virtual Gtk::Container *gtk_container(void);
	virtual void add_child(GtkX::Widget &child,
			       PackOptions options=PACK_EXPAND_WIDGET,
			       int padding=0);
	ChildList get_children(void);
	void remove(Widget &w);
	friend class GtkX::RadioButton;
	Gtk::RadioButtonGroup &button_group(void);
    };

    class Shell: public Container {
    };
}

#endif // GTKX_CONTAINER_H
