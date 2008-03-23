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

    class HScale: public Scale, public Gtk::HScale {
    public:
	HScale(Container &parent, const String &name="");
	HScale(Container &parent, const String &name,
	       double min, double max, double step=1.0);
	~HScale(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
#include <GtkX/redirect.h>
	void set_value(double x);
	double get_value(void) const;
	sigc::signal<void> signal_value_changed(void);
    };

    class VScale: public Scale, public Gtk::VScale {
    public:
	VScale(Container &parent, const String &name="");
	VScale(Container &parent, const String &name,
	       double min, double max, double step=1.0);
	~VScale(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
#include <GtkX/redirect.h>
	void set_value(double x);
	double get_value(void) const;
	sigc::signal<void> signal_value_changed(void);
    };

}

#endif // GTKX_SCALE_H
