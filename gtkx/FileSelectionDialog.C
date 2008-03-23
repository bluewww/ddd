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
#include <iostream>

#include <gtkmm/treemodel.h>

#include <GtkX/FileSelectionDialog.h>

#include <cassert>

using namespace GtkX;

void
FileSelectionDialog::init(Gtk::Window &parent,
			  const String &name,
			  FileAction action)
{
    set_name(name.s());
    add(fs_);
    postinit();
}

FileSelectionDialog::FileSelectionDialog(Gtk::Window &parent,
					 const String &name,
					 FileAction action):
    Dialog(&parent, name),
    fs_(*this, name+String("_fs"), action)
{
    init(parent, name, action);
}

static Gtk::Window &
check_shell(GtkX::Shell &parent)
{
    Gtk::Widget *w = parent.internal();
    Gtk::Window *win = dynamic_cast<Gtk::Window *>(w);
    assert(win);
    return *win;
}

FileSelectionDialog::FileSelectionDialog(GtkX::Shell &parent, const String &name,
					 FileAction action):
    Dialog(&check_shell(parent), name.s()),
    fs_(*this, name+String("_fs"), action)
{
    init(check_shell(parent), name, action);
}

FileSelectionDialog::~FileSelectionDialog(void)
{
}

Gtk::Widget *
FileSelectionDialog::internal(void)
{
    return this;
}

const Gtk::Widget *
FileSelectionDialog::internal(void) const
{
    return this;
}

std::string
FileSelectionDialog::get_selected(void)
{
    return fs_.get_selected();
}

