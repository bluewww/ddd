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
#include <Xmmm/Notebook3.h>
#include <Xmmm/Notebook3P.h>

using namespace Xmmm;

Notebook::Notebook(::Widget parent, const Xmmm::String &name)
{
    nb_ = XmmmCreateNotebook3(parent, (char *)name.c(), NULL, 0);
}

Notebook::Notebook(Xmmm::Container &parent, const Xmmm::String &name)
{
    nb_ = XmmmCreateNotebook3(parent.xt_container(), (char *)name.c(), NULL, 0);
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
    // return ((XmmmNotebookWidget)nb_)->notebook2.change;
    return internal();
}

int
Notebook::get_n_pages(void) const
{
#if 0
    XmmmNotebook2Widget nbw = (XmmmNotebook2Widget)nb_;
    CompositeWidget change = (CompositeWidget)nbw->notebook2.change;
    return change->composite.num_children;
#else
    XmmmNotebook3Widget nbw = (XmmmNotebook3Widget)nb_;
    return nbw->composite.num_children;
#endif
}

void
Notebook::set_current_page(int n)
{
    std::cerr << "SET CURRENT PAGE " << n << "\n";
}

