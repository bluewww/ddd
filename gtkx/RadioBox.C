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

#include <gtkmm/radiobutton.h>
#include <GtkX/RadioBox.h>

using namespace GtkX;

void
RadioBox::create_box(GtkX::Orientation orientation)
{
    switch(orientation) {
    case Gtk::ORIENTATION_VERTICAL:
	box_ = new Gtk::VBox();
	break;
    case Gtk::ORIENTATION_HORIZONTAL:
	box_ = new Gtk::HBox();
	break;
    }
    box_->show();
    // FIXME: Do not allow the virtual on_add to be called here.
    Gtk::VBox::pack_start(*box_, Gtk::PACK_SHRINK);
}

RadioBox::RadioBox(GtkX::Container &parent, PackOptions po,
		   const GtkX::String &name,
		   GtkX::Orientation orientation)
{
    create_box(orientation);
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

RadioBox::~RadioBox(void)
{
    delete box_;
}

Gtk::Widget *
RadioBox::internal(void)
{
    return this;
}

const Gtk::Widget *
RadioBox::internal(void) const
{
    return this;
}

Gtk::Container *
RadioBox::gtk_container(void)
{
    return box_;
}

void
RadioBox::add_child(GtkX::Widget &child, PackOptions options, int padding)
{
    add(*child.internal());
}

void
RadioBox::on_add(Gtk::Widget *child)
{
    box_->add(*child);
    Gtk::RadioButton *rb = dynamic_cast<Gtk::RadioButton *>(child);
    if (rb) {
	rb->set_group(group_);
    }
}

void
RadioBox::pack_start(Gtk::Widget &child, PackOptions options, int padding)
{
    box_->pack_start(child, (Gtk::PackOptions)(int)options, (guint)padding);
    Gtk::RadioButton *rb = dynamic_cast<Gtk::RadioButton *>(&child);
    if (rb) {
	rb->set_group(group_);
    }
}

