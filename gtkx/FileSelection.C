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

#include <cassert>

#include <gtkmm/filechooserwidget.h>

#include <GtkX/FileSelection.h>

using namespace GtkX;

void
FileSelection::init_signals(void)
{
}

static Gtk::FileChooserAction
convert_action(FileAction act)
{
    switch(act) {
    case FileActionOpen: return Gtk::FILE_CHOOSER_ACTION_OPEN;
    case FileActionSave: return Gtk::FILE_CHOOSER_ACTION_SAVE;
    case FileActionSelectFolder: return Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER;
    case FileActionCreateFolder: return Gtk::FILE_CHOOSER_ACTION_CREATE_FOLDER;
    }
    return Gtk::FILE_CHOOSER_ACTION_OPEN;
}

FileSelection::FileSelection(GtkX::Container &parent,
			     const String &name,
			     FileAction action):
    FileChooserWidget(convert_action(action))
{
    set_name(name.s());
    init_signals();
    parent.add_child(*this);
}

FileSelection::~FileSelection(void)
{
}

Gtk::Widget *
FileSelection::internal(void)
{
    return this;
}

const Gtk::Widget *
FileSelection::internal(void) const
{
    return this;
}

std::string
FileSelection::get_selected(void)
{
    // FIXME: This returns the whole line!
}

void
FileSelection::clear(void)
{
}

void
FileSelection::append(const GtkX::String &item)
{
}

int
FileSelection::get_selected_pos(void)
{
}

int
FileSelection::count(void) const
{
}

sigc::signal<void> &
FileSelection::signal_selection_changed(void)
{
    return signal_selection_changed_;
}

void
FileSelection::selection_changed_callback(FileSelection *lv)
{
    lv->signal_selection_changed_();
}

