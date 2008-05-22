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

    class ListView;

    class ListView: public Widget, public Gtk::TreeView {
    private:
	Glib::RefPtr<Gtk::ListStore> store_;
    protected:
	sigc::signal<void> signal_selection_changed_;
	static void selection_changed_callback(ListView *lv);
    public:
	void init_signals(void);
	ListView(GtkX::Container &parent, PackOptions po,
		 const GtkX::String &name,
		 const std::vector<GtkX::String> &headers);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	~ListView(void);
	String get_selected(int row=-1, int col=-1);
	void clear(void);
	void append(const GtkX::String &item);
	int get_selected_pos(void);
	int count(void) const;
	sigc::signal<void> &signal_selection_changed(void);
	int n_rows(void) const;
	int n_selected_rows(void) const;
	String get_at(int row, int col=-1);
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
#include <GtkX/redirect.h>
    };

}

#endif // GTKX_LISTVIEW_H
