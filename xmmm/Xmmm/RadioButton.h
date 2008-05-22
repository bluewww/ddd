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

#ifndef XMMM_RADIOBUTTON_H
#define XMMM_RADIOBUTTON_H

#include <sigc++/signal.h>
#include <Xmmm/Bipolar.h>
#include <Xmmm/Container.h>
#include <Xm/ToggleB.h>

namespace Xmmm {

    class RadioButton: public Bipolar {
    private:
	::Widget button_;
	void init_signals(void);
    protected:
	sigc::signal<void> signal_toggled_;
	static void toggled_callback(::Widget widget, XtPointer data);
    public:
	RadioButton(Xmmm::Container &parent, const Xmmm::String &name="",
		    const Xmmm::String &label="");
	RadioButton(::Widget parent, const Xmmm::String &name="",
		    const Xmmm::String &label=""); // TEMPORARY
	~RadioButton(void);
	::Widget internal(void); // TEMPORARY
	bool get_active(void);
	void set_active(bool new_state, bool notify=false);
	sigc::signal<void> &signal_toggled(void);
    };

}

#endif // XMMM_RADIOBUTTON_H
