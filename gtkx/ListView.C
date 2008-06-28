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
#include <vector>

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
ListView::init_signals()
{
    tv_->get_selection()->signal_changed().connect(sigc::mem_fun(*this, &ListView::selection_changed_callback));
}

ListView::ListView(GtkX::Container &parent, PackOptions po,
		   const String &name,
		   const std::vector<String> &headers)
{
    tv_ = new Gtk::TreeView();
    set_name(name.s());
    store_ = Gtk::ListStore::create(model);
    tv_->set_model(store_);

    set_size_request(-1, 100);

    static Gtk::TreeModelColumn<Glib::ustring> *cols[]
	= {&model.c1, &model.c2, &model.c3, &model.c4,
	   &model.c5, &model.c6, &model.c7, &model.c8};
    assert(headers.size() <= 8);
    for (int i = 0; i < headers.size(); i++) {
	tv_->append_column(headers[i].s(), *cols[i]);
    }
    Gtk::TreeModel::Row row = *store_->append();
    row[model.c1] = "test 1";
    row[model.c2] = "test 2";
    row[model.c3] = "test 3";
    row[model.c4] = "test 4";
    row[model.c5] = "test 5";
    row[model.c6] = "test 6";
    row[model.c7] = "test 7";
    row[model.c8] = "test 8";

    init_signals();
    parent.add_child(*this, po, 0);
}

ListView::~ListView()
{
    delete tv_;
}

Gtk::Widget *
ListView::internal()
{
    return tv_;
}

const Gtk::Widget *
ListView::internal() const
{
    return tv_;
}

void
ListView::postinit()
{
    Widget::postinit();
    init_signals();
}

String
ListView::get_selected(int r, int c)
{
    static Gtk::TreeModelColumn<Glib::ustring> *cols[]
	= {&model.c1, &model.c2, &model.c3, &model.c4,
	   &model.c5, &model.c6, &model.c7, &model.c8};
    Glib::ustring result;
    // Note that the ListHandle_Path returned by get_selected_rows()
    // is automatically converted to an STL type:
    std::vector<Gtk::TreePath> paths = tv_->get_selection()->get_selected_rows();
    result = "";
    if (r < 0) r = 0;
    if (r > paths.size())
	return "";
    Gtk::TreeIter iter = tv_->get_model()->get_iter(paths[r]);
    Gtk::TreeRow row = *iter;
    Glib::ustring tab("\t");
    if (c < 0) {
	result = row[model.c1] + tab
	    + row[model.c2] + tab
	    + row[model.c3] + tab
	    + row[model.c4] + tab
	    + row[model.c5] + tab
	    + row[model.c6] + tab
	    + row[model.c7] + tab
	    + row[model.c8];
    }
    else {
	result = row.get_value(*cols[c]);
    }
    return result;
}

int
ListView::n_rows() const
{
    return tv_->get_model()->children().size();
}

int
ListView::n_selected_rows() const
{
    return tv_->get_selection()->count_selected_rows();
}

String
ListView::get_at(int r, int c)
{
    static Gtk::TreeModelColumn<Glib::ustring> *cols[]
	= {&model.c1, &model.c2, &model.c3, &model.c4,
	   &model.c5, &model.c6, &model.c7, &model.c8};
    Gtk::TreePath p;
    p.push_back(r);
    Gtk::TreeIter iter = tv_->get_model()->get_iter(p);
    Gtk::TreeRow row = *iter;
    Glib::ustring tab("\t");
    Glib::ustring result;
    if (c < 0) {
	result = row[model.c1] + tab
	    + row[model.c2] + tab
	    + row[model.c3] + tab
	    + row[model.c4] + tab
	    + row[model.c5] + tab
	    + row[model.c6] + tab
	    + row[model.c7] + tab
	    + row[model.c8];
    }
    else {
	result = row.get_value(*cols[c]);
    }
    return result;
}

void
ListView::clear()
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
    row[model.c5] = "";
    row[model.c6] = "";
    row[model.c7] = "";
    row[model.c8] = "";
}

int
ListView::get_selected_pos()
{
    Gtk::TreeSelection::ListHandle_Path paths
	= tv_->get_selection()->get_selected_rows();
    if (paths.begin() == paths.end())
	return -1;
    return (*paths.begin())[0];
}

int
ListView::count() const
{
    return store_->children().size();
}

sigc::signal<void> &
ListView::signal_selection_changed()
{
    return signal_selection_changed_;
}

void
ListView::selection_changed_callback()
{
    signal_selection_changed_();
}

void
ListView::get_selected_numbers(std::vector<int> &numbers)
{
    Glib::RefPtr<Gtk::TreeModel> model = tv_->get_model();
    Glib::RefPtr<Gtk::TreeSelection> sel = tv_->get_selection();
    std::list<Gtk::TreeModel::Path> paths = sel->get_selected_rows();
    std::list<Gtk::TreeModel::Path>::iterator iter;
    for (iter = paths.begin(); iter != paths.end(); iter++) {
	numbers.push_back((*iter)[0]);
    }
}

void
ListView::set_selection_mode(SelectionMode sm)
{
    Gtk::SelectionMode gsm;
    switch (sm) {
    case Gtk::SELECTION_NONE: gsm = Gtk::SELECTION_NONE; break;
    case Gtk::SELECTION_SINGLE: gsm = Gtk::SELECTION_SINGLE; break;
    case Gtk::SELECTION_BROWSE: gsm = Gtk::SELECTION_BROWSE; break;
    case Gtk::SELECTION_MULTIPLE: gsm = Gtk::SELECTION_MULTIPLE; break;
    default: gsm = Gtk::SELECTION_MULTIPLE; break;
    }
    tv_->get_selection()->set_mode(gsm);
}

void
ListView::set_cursor(int pos)
{
    Gtk::TreeModel::Path path;
    path.push_back(pos);
    tv_->set_cursor(path);
}

void
ListView::scroll_to_row(int pos)
{
    Gtk::TreeModel::Path path;
    path.push_back(pos);
    tv_->scroll_to_row(path);
}

