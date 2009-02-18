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

#include <GtkX/Window.h>

using namespace GtkX;

void
Window::init_signals(void)
{
}

Gtk::WindowType translate_wt[] = {
    Gtk::WINDOW_TOPLEVEL,
    Gtk::WINDOW_POPUP
};

Window::Window(Main &main, const String &name, const String &title,
	       WindowType wt)
{
    win_ = new Gtk::Window(translate_wt[wt]);
    set_name(name.s());
    set_title(title.s());
    postinit();
}

Window::Window(const String &name, const String &title,
	       WindowType wt)
{
    win_ = new Gtk::Window(translate_wt[wt]);
    set_name(name.s());
    set_title(title.s());
    postinit();
}

Window::~Window(void)
{
    delete win_;
}

Gtk::Widget *
Window::internal(void)
{
    return win_;
}

const Gtk::Widget *
Window::internal(void) const
{
    return win_;
}

Gtk::Container *
Window::gtk_container(void)
{
    return win_;
}

void
Window::set_title(const String &s)
{
    win_->set_title(s.s());
}

void
Window::set_default_size(int width, int height)
{
    win_->set_default_size(width, height);
}


void
Window::get_position(int &x, int &y)
{
    win_->get_position(x, y);
}

void
Window::get_size(int &width, int &height)
{
    win_->get_size(width, height);
}

void
Window::resize(int width, int height)
{
    win_->resize(width, height);
}

void
Window::move(int x, int y)
{
    win_->move(x, y);
}

PropertyProxy<bool>
Window::property_allow_shrink()
{
    // N.B. This must be consistent with Gtk.
    return PropertyProxy<bool>(this, "allow-shrink");
}

PropertyProxy_RO<bool>
Window::property_allow_shrink() const
{
    // N.B. This must be consistent with Gtk.
    return PropertyProxy_RO<bool>(this, "allow-shrink");
}

