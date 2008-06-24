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

#ifndef GTKX_SCALE_H
#define GTKX_SCALE_H

#include <GtkX/Container.h>
#include <gtkmm/scale.h>

namespace GtkX {

    class Scale: public Widget {
    protected:
	sigc::signal<void> signal_value_changed_;
	void value_changed_callback(void);
    public:
	void init_signals(void);
	void set_value(double);
	double get_value(void) const;
	sigc::signal<void> signal_value_changed(void);
    };

    class HScale: public Scale {
	Gtk::HScale *scale_;
    public:
	HScale(Container &parent, PackOptions po=PACK_SHRINK,
	       const String &name="");
	HScale(Container &parent, PackOptions po,
	       const String &name,
	       double min, double max, double step=1.0);
	~HScale(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
    };

    class VScale: public Scale {
	Gtk::VScale *scale_;
    public:
	VScale(Container &parent, PackOptions po=PACK_SHRINK,
	       const String &name="");
	VScale(Container &parent, PackOptions po,
	       const String &name,
	       double min, double max, double step=1.0);
	~VScale(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
    };

}

#endif // GTKX_SCALE_H
