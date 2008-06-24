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

#include <gtkmm/image.h>
#include <GtkX/Widget.h>
#include <GtkX/Button.h>

using namespace GtkX;

Button::Button(GtkX::Container &parent, PackOptions po,
	       const GtkX::String &name, const GtkX::String &label)
{
    button_ = new Gtk::Button(mklabel(name, label).s());
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

Button::~Button(void)
{
    delete button_;
}

void
Button::clicked_callback(void)
{
    signal_clicked_();
}

void
Button::init_signals(void)
{
    button_->signal_clicked().connect(sigc::mem_fun(*this, &Button::clicked_callback));
}

void
Button::postinit(void)
{
    Widget::postinit();
    init_signals();
}

Gtk::Widget *
Button::internal(void)
{
    return button_;
}

const Gtk::Widget *
Button::internal(void) const
{
    return button_;
}

void
Button::set_alignment(float xalign, float yalign)
{
    button_->set_alignment(xalign, yalign);
}

void
Button::set_label(const String &s)
{
    button_->set_label(s.s());
}

void
Button::set_image(const ImageHandle &ih)
{
    Gtk::Widget *old_img = button_->get_image();
    Gtk::Image *new_img = new Gtk::Image(ih);
    button_->set_image(*new_img);
    if (old_img)
	delete old_img;
}

void
Button::remove(void)
{
    button_->remove();
}

void
Button::add_label(const String &s)
{
    button_->add_label(s.s());
}

sigc::signal<void> &
Button::signal_clicked()
{
    return signal_clicked_;
}

