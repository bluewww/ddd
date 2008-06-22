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

#ifndef GTKX_FILESELECTION_H
#define GTKX_FILESELECTION_H

#include <vector>

#include <gtkmm/filechooserwidget.h>

#include <GtkX/Container.h>

namespace GtkX {

    enum FileAction {
	FileActionOpen,
	FileActionSave,
	FileActionSelectFolder,
	FileActionCreateFolder
    };

    class FileSelection: public Widget {
	Gtk::FileChooserWidget *fs_;
    protected:
	sigc::signal<void> signal_selection_changed_;
	static void selection_changed_callback(FileSelection *lv);
    public:
	void init_signals(void);
	FileSelection(GtkX::Container &parent, PackOptions po=PACK_SHRINK,
		      const GtkX::String &name="", FileAction action=FileActionOpen);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	~FileSelection(void);
	GtkX::String get_selected(void);
	void clear(void);
	void append(const GtkX::String &item);
	int get_selected_pos(void);
	int count(void) const;
	sigc::signal<void> &signal_selection_changed(void);
    };

}

#endif // GTKX_FILESELECTION_H
