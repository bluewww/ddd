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

GtkX::Container *get_parent(void)
{
    return GtkX::Widget::get_parent();
}

GtkX::String get_name(void)
{
    return GtkX::Widget::get_name();
}

GtkX::RefPtr<GtkX::Display> get_display(void)
{
    return GtkX::Widget::get_display();
}

GtkX::RefPtr<GtkX::XWindow> get_window(void)
{
    return GtkX::Widget::get_window();
}

void show(void)
{
    GtkX::Widget::show();
}

void hide(void)
{
    GtkX::Widget::hide();
}

bool is_visible(void)
{
    return GtkX::Widget::is_visible();
}

bool is_realized(void)
{
    return GtkX::Widget::is_realized();
}

void set_name(const GtkX::String &s)
{
    GtkX::Widget::set_name(s);
}

void set_sensitive(bool b)
{
    GtkX::Widget::set_sensitive(b);
}

sigc::signal<bool, GtkX::EventButton *> &signal_button_release_event()
{
    return GtkX::Widget::signal_button_release_event();
}

sigc::signal<bool, GtkX::EventButton *> &signal_button_press_event()
{
    return GtkX::Widget::signal_button_press_event();
}

sigc::signal<void> &signal_map()
{
    return GtkX::Widget::signal_map();
}

sigc::signal<void> &signal_unmap()
{
    return GtkX::Widget::signal_unmap();
}

void add_destroy_notify_callback(void *data, void *(*f)(void *))
{
    return GtkX::Widget::add_destroy_notify_callback(data, f);
}

void remove_destroy_notify_callback(void *data)
{
    return GtkX::Widget::remove_destroy_notify_callback(data);
}

bool
translate_coordinates(GtkX::Widget &dest_w, int x1, int y1, int &x2, int &y2)
{
    return GtkX::Widget::translate_coordinates(dest_w, x1, y1, x2, y2);
}

GtkX::PropertyProxy<void *>
property_user_data(void)
{
    return GtkX::Widget::property_user_data();
}

GtkX::PropertyProxy_RO<void *>
property_user_data(void) const
{
    return GtkX::Widget::property_user_data();
}

GtkX::Requisition
size_request(void) const
{
    return GtkX::Widget::size_request();
}

