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

#ifndef GTKX_LISTVIEW_H
#define GTKX_LISTVIEW_H

#include <vector>

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

#include <GtkX/Container.h>

namespace GtkX {

    enum SelectionMode {
	SELECTION_NONE,
	SELECTION_SINGLE,
	SELECTION_BROWSE,
	SELECTION_MULTIPLE
    };

    class ListView;

    class ListView: public Widget {
	Gtk::TreeView *tv_;
	Glib::RefPtr<Gtk::ListStore> store_;
    protected:
	sigc::signal<void> signal_selection_changed_;
	void selection_changed_callback();
    public:
	ListView(GtkX::Container &parent, PackOptions po,
		 const GtkX::String &name,
		 const std::vector<GtkX::String> &headers);
	Gtk::Widget *internal();
	const Gtk::Widget *internal() const;
	~ListView();
	void init_signals();
	void postinit();
	String get_selected(int row=-1, int col=-1);
	void clear();
	void append(const GtkX::String &item);
	int get_selected_pos();
	int count() const;
	sigc::signal<void> &signal_selection_changed();
	int n_rows() const;
	int n_selected_rows() const;
	String get_at(int row, int col=-1);
	void get_selected_numbers(std::vector<int> &numbers);
	void set_selection_mode(SelectionMode);
	void set_cursor(int);
	void scroll_to_row(int);
    };

}

#endif // GTKX_LISTVIEW_H
