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

#ifndef GTKX_SELECTIONDIALOG_H
#define GTKX_SELECTIONDIALOG_H

#include <vector>

#include <gtkmm/window.h>
#include <gtkmm/dialog.h>

#include <GtkX/Container.h>
#include <GtkX/ListView.h>
#include <GtkX/ScrolledWindow.h>
#include <GtkX/Box.h>
#include <GtkX/Button.h>

namespace GtkX {

    class SelectionDialog: public Gtk::Dialog, public Container {
	ScrolledWindow *sw_;
	ListView *listview_;
	HBox *buttons_;
    public:
	void init(Gtk::Window &parent, const String &name,
		  const std::vector<String> &headers);
	SelectionDialog(GtkX::Shell &parent, const String &name,
			const std::vector<String> &headers);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	Gtk::Container *gtk_container(void);
	~SelectionDialog(void);
	ListView *list(void);
	String get_selected(void); // DEPRECATED
	// FIXME: SelectionDialog can be List+Entry.
	// Entry can be entered by hand, not necessarily from list.
	String get_text(void);
	void set_text(const String &s);
	void pack_start(Gtk::Widget &child,
			PackOptions options = PACK_EXPAND_WIDGET,
			int padding = 0);
	Button *add_button(const String &name="", const String &label="");
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
#include <GtkX/redirect.h>
    private:
	SelectionDialog(Gtk::Window &parent, const String &name,
			const std::vector<String> &headers);
    };

}

#endif // GTKX_SELECTIONDIALOG_H
