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

#include <Xmmm/ScrolledWindow.h>

using namespace Xmmm;

ScrolledWindow::ScrolledWindow(::Widget parent, const Xmmm::String &name,
			       std::vector<Arg> &args)
{
    sw_ = XmCreateScrolledWindow(parent, (char *)name.c(), &args[0], args.size());
    postinit();
}

ScrolledWindow::ScrolledWindow(Xmmm::Container &parent, const Xmmm::String &name,
			       std::vector<Arg> &args)
{
    sw_ = XmCreateScrolledWindow(parent.xt_container(), (char *)name.c(), &args[0], args.size());
    postinit();
}

ScrolledWindow::~ScrolledWindow(void)
{
    XtDestroyWidget(sw_);
}

::Widget ScrolledWindow::internal(void)
{
    return sw_;
}

