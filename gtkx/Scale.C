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

#include <GtkX/Scale.h>

using namespace GtkX;

void
Scale::value_changed_callback(void)
{
    signal_value_changed_();
}

void
Scale::init_signals(void)
{
    Gtk::Scale *sc = dynamic_cast<Gtk::Scale *>(internal());
    sc->signal_value_changed().connect(sigc::mem_fun(*this, &Scale::value_changed_callback));
}

sigc::signal<void>
Scale::signal_value_changed(void)
{
    return signal_value_changed_;
}

void
Scale::set_value(double x)
{
    Gtk::Scale *sc = dynamic_cast<Gtk::Scale *>(internal());
    sc->set_value(x);
}

double
Scale::get_value(void) const
{
    Gtk::Scale *sc = dynamic_cast<Gtk::Scale *>((const_cast<Scale *>(this))->internal());
    return sc->get_value();
}

VScale::VScale(GtkX::Container &parent, PackOptions po,
	       const GtkX::String &name)
{
    scale_ = new Gtk::VScale();
    set_name(name.s());
    parent.add_child(*this, po, 0);
    init_signals();
    postinit();
}

VScale::VScale(GtkX::Container &parent, PackOptions po,
	       const GtkX::String &name,
	       double min, double max, double step)
{
    scale_ = new Gtk::VScale(min, max, step);
    set_name(name.s());
    parent.add_child(*this, po, 0);
    init_signals();
    postinit();
}

VScale::~VScale()
{
    delete scale_;
}

Gtk::Widget *
VScale::internal(void)
{
    return scale_;
}

const Gtk::Widget *
VScale::internal(void) const
{
    return scale_;
}

HScale::HScale(GtkX::Container &parent, PackOptions po,
	       const GtkX::String &name)
{
    scale_ = new Gtk::HScale();
    set_name(name.s());
    parent.add_child(*this, po, 0);
    init_signals();
    postinit();
}

HScale::HScale(GtkX::Container &parent, PackOptions po,
	       const GtkX::String &name,
	       double min, double max, double step)
{
    scale_ = new Gtk::HScale(min, max, step);
    set_name(name.s());
    parent.add_child(*this, po, 0);
    init_signals();
    postinit();
}

HScale::~HScale()
{
    delete scale_;
}

Gtk::Widget *
HScale::internal(void)
{
    return scale_;
}

const Gtk::Widget *
HScale::internal(void) const
{
    return scale_;
}

