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

#include <gtkmm/treemodel.h>

#include <GtkX/ListView.h>

using namespace GtkX;

class ModelColumns: public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<Glib::ustring> c1;
  Gtk::TreeModelColumn<Glib::ustring> c2;
  Gtk::TreeModelColumn<Glib::ustring> c3;
  Gtk::TreeModelColumn<Glib::ustring> c4;
  ModelColumns()
  {
    add(c1);
    add(c2);
    add(c3);
    add(c4);
  }
};

static ModelColumns model;

ListView::ListView(GtkX::Container &parent,
		   const String &name,
		   const std::vector<String> &headers)
{
    set_name(name.s());
    store_ = Gtk::ListStore::create(model);
    set_model(store_);

    set_size_request(-1, 100);

    static Gtk::TreeModelColumn<Glib::ustring> *cols[]
	= {&model.c1, &model.c2, &model.c3, &model.c4};
    assert(headers.size() <= 4);
    for (int i = 0; i < headers.size(); i++) {
	append_column(headers[i].s(), *cols[i]);
    }
    Gtk::TreeModel::Row row = *store_->append();
    row[model.c1] = "test 1";
    row[model.c2] = "test 2";
    row[model.c3] = "test 3";
    row[model.c4] = "test 4";
}

ListView::~ListView(void)
{
}

Gtk::Widget *
ListView::gtk_widget(void)
{
    return this;
}

std::string
ListView::get_selected(void)
{
    // FIXME: This returns the whole line!
    Gtk::TreeIter iter = get_selection()->get_selected();
    Gtk::TreeRow row = *iter;
    Glib::ustring tab("\t");
    return row[model.c1] + tab
	+ row[model.c2] + tab
	+ row[model.c3] + tab
	+ row[model.c4];
}

