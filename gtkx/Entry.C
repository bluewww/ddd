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

// ***************************************************************************

// Primitive widget creation using constructors with no arguments.

#include <GtkX/Widget.h>
#include <GtkX/Entry.h>

using namespace GtkX;

Entry::Entry(GtkX::Container &parent,
	     PackOptions po,
	     const GtkX::String &name)
{
    entry_ = new Gtk::Entry();
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

Entry::~Entry(void)
{
    delete entry_;
}

Gtk::Widget *
Entry::internal(void)
{
    return entry_;
}

const Gtk::Widget *
Entry::internal(void) const
{
    return entry_;
}

void
Entry::init_signals(void)
{
    entry_->signal_activate().connect(sigc::mem_fun(*this, &Entry::activate_callback));
    entry_->signal_changed().connect(sigc::mem_fun(*this, &Entry::changed_callback));
}

void
Entry::postinit(void)
{
    Widget::postinit();
    init_signals();
}

void
Entry::set_text(const String &text)
{
    entry_->set_text(text.s());
}

String
Entry::get_text() const
{
    return entry_->get_text();
}

void
Entry::set_editable(bool is_editable)
{
    entry_->set_editable(is_editable);
}

bool
Entry::get_editable() const
{
    return entry_->get_editable();
}

void
Entry::activate_callback()
{
    signal_activate_();
}

void
Entry::changed_callback()
{
    signal_changed_();
}

sigc::signal<void> &
Entry::signal_activate()
{
    return signal_activate_;
}

sigc::signal<void> &
Entry::signal_changed()
{
    return signal_changed_;
}

