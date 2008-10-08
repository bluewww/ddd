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

#include <GtkX/Notebook.h>
#include <GtkX/Box.h>

using namespace GtkX;

void
Box::add_child(Widget &child, PackOptions options, int padding)
{
    Gtk::Container *cont = gtk_container();
    Gtk::Box *box = dynamic_cast<Gtk::Box *>(cont);
    Gtk::PackOptions tr[] = {Gtk::PACK_SHRINK,
			     Gtk::PACK_EXPAND_PADDING,
			     Gtk::PACK_EXPAND_WIDGET};
    box->pack_start(*child.internal(), tr[options], padding);
}

void
Box::set_spacing(int s)
{
    Gtk::Box *box = dynamic_cast<Gtk::Box *>(internal());
    box->set_spacing(s);
}

#ifdef __GNUC__
#warning Remove this special case notebook stuff?
#endif

// Private.  API does not allow construction of unparented widgets.
VBox::VBox(const GtkX::String &name)
{
    vbox_ = new Gtk::VBox;
    set_name(name.s());
    postinit();
}

VBox::VBox(GtkX::Container &parent, GtkX::PackOptions po,
	   const GtkX::String &name, const GtkX::String &label)
{
    vbox_ = new Gtk::VBox;
    set_name(name.s());
    GtkX::Notebook *nb = dynamic_cast<GtkX::Notebook *>(&parent);
    if (nb)
	nb->append_page(*this, mklabel(name,label));
    else
	parent.add_child(*this, po, 0);
    postinit();
}

VBox::~VBox()
{
    delete vbox_;
}

Gtk::Widget *
VBox::internal(void)
{
    return vbox_;
}

const Gtk::Widget *
VBox::internal(void) const
{
    return vbox_;
}

// Private.  API does not allow construction of unparented widgets.
HBox::HBox(const GtkX::String &name)
{
    hbox_ = new Gtk::HBox;
    set_name(name.s());
    postinit();
}

HBox::HBox(GtkX::Container &parent, GtkX::PackOptions po,
	   const GtkX::String &name, const GtkX::String &label)
{
    hbox_ = new Gtk::HBox;
    set_name(name.s());
    GtkX::Notebook *nb = dynamic_cast<GtkX::Notebook *>(&parent);
    if (nb) {
	nb->append_page(*this, mklabel(name,label));
	// FIXME?
	show();
    }
    else
	parent.add_child(*this, po, 0);
    postinit();
}

HBox::~HBox()
{
    delete hbox_;
}

Gtk::Widget *
HBox::internal(void)
{
    return hbox_;
}

const Gtk::Widget *
HBox::internal(void) const
{
    return hbox_;
}

