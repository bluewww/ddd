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

#ifndef XMMM_SCALE_H
#define XMMM_SCALE_H

#include <Xmmm/Container.h>
#include <Xm/Scale.h>

namespace Xmmm {

    class Scale: public Widget {
    protected:
	::Widget scale_;
	void init_signals(void);
    protected:
	sigc::signal<void> signal_clicked_;
	static void activate_callback(::Widget widget, XtPointer data);
    public:
	~Scale(void);
	::Widget internal(void); // TEMPORARY
    };

    class HScale: public Scale {
    public:
	HScale(Xmmm::Container &parent, const Xmmm::String &name="");
	HScale(::Widget parent, const Xmmm::String &name=""); // TEMPORARY
    };

    class VScale: public Scale {
    public:
	VScale(Xmmm::Container &parent, const Xmmm::String &name="");
	VScale(::Widget parent, const Xmmm::String &name=""); // TEMPORARY
    };

}

#endif // XMMM_SCALE_H
