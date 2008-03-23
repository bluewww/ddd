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
  Gtk::TreeModelColumn<Glib::ustring> c5;
  Gtk::TreeModelColumn<Glib::ustring> c6;
  Gtk::TreeModelColumn<Glib::ustring> c7;
  Gtk::TreeModelColumn<Glib::ustring> c8;
  ModelColumns()
  {
    add(c1);
    add(c2);
    add(c3);
    add(c4);
    add(c5);
    add(c6);
    add(c7);
    add(c8);
  }
};

static ModelColumns model;

void
ListView::init_signals(void)
{
}

ListView::ListView(GtkX::Container &parent,
		   const String &name,
		   const std::vector<String> &headers,
		   PackOptions options,
		   int padding)
{
    set_name(name.s());
    store_ = Gtk::ListStore::create(model);
    set_model(store_);

    set_size_request(-1, 100);

    static Gtk::TreeModelColumn<Glib::ustring> *cols[]
	= {&model.c1, &model.c2, &model.c3, &model.c4,
	   &model.c5, &model.c6, &model.c7, &model.c8};
    assert(headers.size() <= 8);
    for (int i = 0; i < headers.size(); i++) {
	append_column(headers[i].s(), *cols[i]);
    }
    Gtk::TreeModel::Row row = *store_->append();
    row[model.c1] = "test 1";
    row[model.c2] = "test 2";
    row[model.c3] = "test 3";
    row[model.c4] = "test 4";
    row[model.c4] = "test 5";
    row[model.c4] = "test 6";
    row[model.c4] = "test 7";
    row[model.c4] = "test 8";

    init_signals();
    parent.add_child(*this);
}

ListView::~ListView(void)
{
}

Gtk::Widget *
ListView::internal(void)
{
    return this;
}

const Gtk::Widget *
ListView::internal(void) const
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

void
ListView::clear(void)
{
    store_->clear();
}

void
ListView::append(const GtkX::String &item)
{
    Gtk::TreeModel::Row row = *store_->append();
    row[model.c1] = item.s();
    row[model.c2] = "";
    row[model.c3] = "";
    row[model.c4] = "";
}

int
ListView::get_selected_pos(void)
{
    Gtk::TreeSelection::ListHandle_Path paths = get_selection()->get_selected_rows();
    if (paths.begin() == paths.end())
	return -1;
    return (*paths.begin())[0];
}

int
ListView::count(void) const
{
    return store_->children().size();
}

sigc::signal<void> &
ListView::signal_selection_changed(void)
{
    return signal_selection_changed_;
}

void
ListView::selection_changed_callback(ListView *lv)
{
    lv->signal_selection_changed_();
}

