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

#include <iostream>
#include <GtkX/Notebook.h>
#include <GtkX/Box.h>

using namespace GtkX;

Notebook::Notebook(GtkX::Container &parent, PackOptions po,
		   const GtkX::String &name)
{
    nb_ = new Gtk::Notebook();
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

Notebook::~Notebook(void)
{
    delete nb_;
}

Gtk::Widget *
Notebook::internal(void)
{
    return nb_;
}

const Gtk::Widget *
Notebook::internal(void) const
{
    return nb_;
}

int
Notebook::append_page(GtkX::Widget &child, const String &tab_label, bool use_mnemonic)
{
    nb_->append_page(*child.internal(), tab_label.s(), use_mnemonic);
}

GtkX::Container *
Notebook::append_page(const String &tab_label, bool use_mnemonic)
{
    HBox *box = new HBox(*this, PACK_EXPAND_WIDGET, get_name()+String("_page"), tab_label);
    box->show();
    return box;
}

GtkX::Widget *
Notebook::get_current_child(void)
{
    Gtk::Notebook_Helpers::PageList::iterator iter = nb_->get_current();
    Gtk::Widget *child = iter->get_child();
    if (!child) return NULL;
    GtkX::Widget *super = (GtkX::Widget *)child->get_data(gtkx_super_quark);
    if (super) {
	std::cerr << "super=" << super->get_name().s() << "\n";
	return super;
    }
    return NULL;
}

int
Notebook::get_n_pages()
{
    return nb_->get_n_pages();
}

void
Notebook::set_current_page(int page_num)
{
    nb_->set_current_page(page_num);
}

