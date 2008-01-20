// -*- C++ -*-

// High-level GUI wrapper for Xm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#ifndef XMMM_WINDOW_H
#define XMMM_WINDOW_H

#include <sigc++/signal.h>

#include <Xmmm/Container.h>

namespace Xmmm {

    class Window: public Container {
	::Widget win_;
	void init_signals(void);
    protected:
	sigc::signal<bool,XEvent*> signal_delete_;
	static void delete_callback(::Widget widget, XtPointer data);
    public:
	void init(::Widget parent, const Xmmm::String &name="",
		  int argc=0, char **argv=NULL); // TEMPORARY
	Window(::Widget parent, const Xmmm::String &name="",
	       int argc=0, char **argv=NULL); // TEMPORARY
	Window(Xmmm::Widget &parent, const Xmmm::String &name="",
	       int argc=0, char **argv=NULL);
	~Window(void);
	::Widget internal(void);
	sigc::signal<bool,XEvent*> &signal_delete_event(void);
    };

}

#endif // XMMM_WINDOW_H
