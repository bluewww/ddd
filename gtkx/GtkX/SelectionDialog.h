// -*- C++ -*-

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

#ifndef GTKX_SELECTIONDIALOG_H
#define GTKX_SELECTIONDIALOG_H

#include <vector>

#include <gtkmm/window.h>
#include <gtkmm/dialog.h>

#include <GtkX/Container.h>
#include <GtkX/ListView.h>

namespace GtkX {

    class SelectionDialog: public Gtk::Dialog, public GtkX::Container {
	GtkX::ListView *listview_;
    public:
	SelectionDialog(Gtk::Window &parent, const GtkX::String &name,
			const std::vector<GtkX::String> &headers);
	Gtk::Widget *gtk_widget(void);
	Gtk::Container *gtk_container(void);
	~SelectionDialog(void);
	GtkX::ListView *list(void);
	std::string get_selected(void);
    };

}

#endif // GTKX_SELECTIONDIALOG_H
