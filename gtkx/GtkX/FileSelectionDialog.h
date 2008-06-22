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

#ifndef GTKX_FILESELECTIONDIALOG_H
#define GTKX_FILESELECTIONDIALOG_H

#include <vector>

#include <gtkmm/window.h>
#include <gtkmm/dialog.h>

#include <GtkX/Dialog.h>
#include <GtkX/FileSelection.h>

namespace GtkX {

    class FileSelectionDialog: public Dialog {
	FileSelection *fs_;
    public:
	void init(GtkX::Shell &parent, const String &name,
		  FileAction action);
	FileSelectionDialog(GtkX::Shell &parent, const String &name,
			    FileAction action);
	GtkX::String get_selected(void);
	~FileSelectionDialog(void);
    };

}

#endif // GTKX_FILESELECTIONDIALOG_H
