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

#include <GtkX/MultiPaned.h>
#include <cassert>

using namespace GtkX;

#if 0
MultiPaned::MultiPaned()
{
    Gtk::VPaned *paned = new Gtk::VPaned();
    xchild_.push_back(paned);
}
#endif

MultiPaned::MultiPaned(GtkX::Container &parent, PackOptions po,
		       const GtkX::String &name, const GtkX::String &label)
{
    Gtk::VPaned *paned = new Gtk::VPaned();
    xchild_.push_back(paned);
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

MultiPaned::~MultiPaned()
{
    std::list<Gtk::VPaned *>::iterator iter;
    for (iter = xchild_.begin(); iter != xchild_.end(); iter++) {
	delete *iter;
    }
}

Gtk::Widget *
MultiPaned::internal(void)
{
    return *xchild_.begin();
}

const Gtk::Widget *
MultiPaned::internal(void) const
{
    return *xchild_.begin();
}

void
MultiPaned::add_child(GtkX::Widget &child, PackOptions options, int padding)
{
    int resize = 0;
    if (options & (PACK_EXPAND_PADDING|PACK_EXPAND_WIDGET))
	resize = 1;
    Gtk::VPaned *top = *xchild_.begin();
    int n = top->get_children().size();
    if (n == 0) {
	top->pack1(*child.internal(), resize, false);
    }
    else if (n == 1) {
	Gtk::VPaned *paned = new Gtk::VPaned();
	paned->set_name("MultiPaned");
	top->add(*paned);
	xchild_.push_back(paned);
	paned->show();
	paned->pack1(*child.internal(), resize, false);
    }
    else {
	Gtk::VPaned *paned = new Gtk::VPaned();
	Gtk::VPaned *parent = *xchild_.rbegin();
	paned->set_name(parent->get_name()+Glib::ustring("+"));
	xchild_.push_back(paned);
	parent->add(*paned);
	paned->show();
	paned->pack1(*child.internal(), resize, false);
    }
}

void
MultiPaned::debug(void)
{
    Gtk::VPaned *paned = *xchild_.begin();
    while (paned) {
	Gtk::Widget *c1 = paned->get_child1();
	Gtk::Allocation all = c1->get_allocation();
	std::cerr << c1 << " \"" << c1->get_name() << "\" "
		  << c1->is_visible() << " " << c1->is_mapped() << " ("
		  << all.get_x() << " " << all.get_y() << " "
		  << all.get_width() << " " << all.get_height() << ") "
		  << "\n";
	Gtk::Widget *c2 = paned->get_child2();
	if (c2)
	    paned = dynamic_cast<Gtk::VPaned *>(c2);
	else
	    break;
    }
}

void
MultiPaned::show_child(Gtk::Widget* w)
{
    w->show();
    Gtk::Widget *paned = w->get_parent();
    if (paned)
	paned->show();
}

void
MultiPaned::hide_child(Gtk::Widget *w)
{
    w->hide();
    Gtk::Widget *parent = w->get_parent();
    if (!parent) return;
    Gtk::Paned *paned = dynamic_cast<Gtk::Paned *>(parent);
    if (paned) {
	Gtk::Widget *w1 = paned->get_child1();
	Gtk::Widget *w2 = paned->get_child2();
	// A MultiPaned child is always the first child of a
	// Gtk::Paned widget.
	assert (w == w1);
	if (!(w2 && w2->is_visible()))
	    paned->hide();
    }
}

void
MultiPaned::show_child(GtkX::Widget* widget)
{
    MultiPaned::show_child(widget->internal());
}

void
MultiPaned::hide_child(GtkX::Widget* widget)
{
    MultiPaned::hide_child(widget->internal());
}

