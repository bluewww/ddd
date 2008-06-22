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

#include <GtkX/Label.h>

using namespace GtkX;

Label::Label(Container &parent, PackOptions po,
	     const String &name,
	     const String &label)
{
    label_ = new Gtk::Label(mklabel(name, label).s());
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

Label::~Label(void)
{
    delete label_;
}

Gtk::Widget *
Label::internal(void)
{
    return label_;
}

const Gtk::Widget *
Label::internal(void) const
{
    return label_;
}

void
Label::set_alignment(float xalign, float yalign)
{
    label_->set_alignment(xalign, yalign);
}

void
Label::set_text(const String &s)
{
    label_->set_text(s.s());
}

// Uses underlines or Pango markup
void
Label::set_label(const String &s)
{
    label_->set_label(s.s());
}
