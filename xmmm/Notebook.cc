// High-level GUI wrapper for Motif.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <iostream>

#include <Xmmm/Notebook.h>
#include <Xmmm/XmmmNotebook.h>
#include <Xmmm/XmmmNotebookP.h>

using namespace Xmmm;

Notebook::Notebook(::Widget parent, const Xmmm::String &name)
{
    nb_ = XmmmCreateNotebook(parent, (char *)name.c(), NULL, 0);
    postinit();
}

Notebook::Notebook(Xmmm::Container &parent, const Xmmm::String &name)
{
    nb_ = XmmmCreateNotebook(parent.xt_container(), (char *)name.c(), NULL, 0);
    postinit();
}

Notebook::~Notebook(void)
{
    XtDestroyWidget(nb_);
}

::Widget
Notebook::internal(void)
{
    return nb_;
}

::Widget
Notebook::xt_container(void)
{
    return internal();
}

int
Notebook::get_n_pages(void) const
{
    return XmmmNotebookNumPages(nb_);
}

void
Notebook::set_current_page(int n)
{
    std::cerr << "SET CURRENT PAGE " << n << "? - not implemented\n";
}

Xmmm::Widget *
Notebook::get_current_child(void)
{
    std::cerr << "GET CURRENT CHILD - not fully implemented?\n";
    ::Widget w = XmmmNotebookCurrentPage(nb_);
    if (w) {
	std::cerr << "UNderlying widget: " << w << "\n";
	Xmmm::Widget *wpp = Xmmm::Widget::lookup(w);
	if (wpp) {
	    std::cerr << "Widget wrapper: " << w << "\n";
	    return wpp;
	}
    }
    return NULL;
}

