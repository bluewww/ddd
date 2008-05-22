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

#ifndef GTKX_WIDGETPTR_H
#define GTKX_WIDGETPTR_H

#include <GtkX/Widget.h>

namespace GtkX {

    template <class T>
    class WidgetPtr {
	T *p_;
    public:
	WidgetPtr(void) {
	    p_ = NULL;
	}
	WidgetPtr(T *p) {
	    p_ = p;
	}
	operator T *(void) {
	    return p_;
	};
	T &operator*(void) const {
	    return *p_;
	}
	T *operator->() const {
	    return p_;
	}
	operator Gtk::Widget *(void) {
	    if (!p_) return (Gtk::Widget *)0;
	    return p_->internal();
	}
    };

}

#endif // GTKX_WIDGETPTR_H
