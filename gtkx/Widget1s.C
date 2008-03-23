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

#include <GtkX/Widget.h>
#include <GtkX/Widget1s.h>

using namespace GtkX;

template <class T>
Widget1s<T>::Widget1s(GtkX::Container &parent, const GtkX::String &name):
    T(name.s())
{
    T::set_name(name.s());
    // We cannot use this:
    // parent.gtk_container()->add(*this);
    // If we always had parent.gtk_container() == &parent we could just
    // override the on_add() method to do what we want.  However,
    // sometimes parent.gtk_container() is a standard Gtk widget.
    // In such a case (e.g. RadioBox) we need to override add_child()
    // instead.
    parent.add_child(*this);
    postinit();
}

// TEMPORARY
template <class T>
Widget1s<T>::Widget1s(Gtk::Container *parent, const GtkX::String &name):
    T(name.s())
{
    T::set_name(name.s());
    parent->add(*internal());
    postinit();
}

template <class T>
Widget1s<T>::~Widget1s(void)
{
}

template <class T>
Gtk::Widget *
Widget1s<T>::internal(void)
{
    return this;
}

template <class T>
const Gtk::Widget *
Widget1s<T>::internal(void) const
{
    return this;
}

#include <gtkmm/radiobutton.h>
#include <gtkmm/button.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>

template class Widget1s<Gtk::RadioButton>;
template class Widget1s<Gtk::Button>;
template class Widget1s<Gtk::MenuItem>;
template class Widget1s<Gtk::CheckMenuItem>;
template class Widget1s<Gtk::CheckButton>;
template class Widget1s<Gtk::Label>;
