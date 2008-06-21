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

#include <cassert>
#include <iostream>

#include <gtkmm/treemodel.h>

#include <GtkX/FileSelectionDialog.h>

#include <cassert>

using namespace GtkX;

void
FileSelectionDialog::init(GtkX::Shell &parent,
			  const String &name,
			  FileAction action)
{
    set_name(name.s());
    postinit();
    fs_ = new FileSelection(*this, PACK_EXPAND_WIDGET,
			    name+String("_fs"), action);
    fs_->set_size_request(600, 400);
    fs_->show();
}

FileSelectionDialog::FileSelectionDialog(GtkX::Shell &parent, const String &name,
					 FileAction action):
    Dialog(parent, name)
{
    init(parent, name, action);
}

FileSelectionDialog::~FileSelectionDialog(void)
{
    delete fs_;
}

std::string
FileSelectionDialog::get_selected(void)
{
    return fs_->get_selected();
}

