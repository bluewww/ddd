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

#ifndef XMMM_OPTIONMENU_H
#define XMMM_OPTIONMENU_H

#include <Xmmm/Container.h>
#include <Xm/RowColumn.h>

namespace Xmmm {

    class OptionMenu: public Widget {
    private:
	::Widget optionmenu_;
	void init_signals(void);
    protected:
	sigc::signal<void> signal_clicked_;
	static void activate_callback(::Widget widget, XtPointer data);
    public:
	OptionMenu(Xmmm::Container &parent, const Xmmm::String &name="");
	OptionMenu(::Widget parent, const Xmmm::String &name=""); // TEMPORARY
	~OptionMenu(void);
	::Widget internal(void); // TEMPORARY
	void set_menu(Xmmm::Widget &sub);
    };

}

#endif // XMMM_OPTIONMENU_H
