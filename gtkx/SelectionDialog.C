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
#include <iostream>

#include <gtkmm/treemodel.h>

#include <GtkX/SelectionDialog.h>

#include <cassert>

using namespace GtkX;

void
SelectionDialog::init(Gtk::Window &parent,
		      const String &name,
		      const std::vector<String> &headers)
{
    set_name(name.s());

    sw_ = new GtkX::ScrolledWindow(*this, name+String("_sw"), GtkX::PACK_EXPAND_WIDGET);
    sw_->set_size_request(-1, 100);
    sw_->show();

    listview_ = new GtkX::ListView(*sw_, name+String("_list"), headers);
    listview_->show();

    buttons_ = new GtkX::HBox(*this, name+String("_buttons"));
    buttons_->show();
    postinit();
}

SelectionDialog::SelectionDialog(Gtk::Window &parent,
				 const String &name,
				 const std::vector<String> &headers):
    Dialog(name.s(), parent)
{
    init(parent, name, headers);
}

static Gtk::Window &
check_shell(GtkX::Shell &parent)
{
    Gtk::Widget *w = parent.internal();
    Gtk::Window *win = dynamic_cast<Gtk::Window *>(w);
    assert(win);
    return *win;
}

SelectionDialog::SelectionDialog(GtkX::Shell &parent, const String &name,
				 const std::vector<String> &headers):
    Dialog(name.s(), check_shell(parent))
{
    init(check_shell(parent), name, headers);
}

SelectionDialog::~SelectionDialog(void)
{
    delete listview_;
    delete sw_;
    delete buttons_;
}

Gtk::Widget *
SelectionDialog::internal(void)
{
    return this;
}

// In the case of Dialogs, we almost always want to add to the VBox
// returned by get_vbox().
Gtk::Container *
SelectionDialog::gtk_container(void)
{
    return get_vbox();
}

GtkX::ListView *
SelectionDialog::list(void)
{
    return listview_;
}

std::string
SelectionDialog::get_selected(void)
{
    return listview_->get_selected();
}

std::string
SelectionDialog::get_text(void)
{
    std::cerr << "SelectionDialog::get_text\n";
    return std::string("ERROR");
}

void
SelectionDialog::set_text(const std::string &s)
{
    std::cerr << "SelectionDialog::set_text\n";
}

void
SelectionDialog::pack_start(Gtk::Widget &child, PackOptions options, int padding)
{
    get_vbox()->pack_start(child, (Gtk::PackOptions)(int)options, (guint)padding);
}

Button *
SelectionDialog::add_button(const String &name)
{
    return new Button(*buttons_, name);
}

