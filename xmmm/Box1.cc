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

#include <Xmmm/Box1.h>

using namespace Xmmm;

Box1::Box1(Xmmm::Widget &parent, const Xmmm::String &name)
{
    b2_ = XmmmCreateBox2(parent.xt_container(), (char *)name.c(), NULL, 0);
    postinit();
}

Box1::Box1(::Widget parent, const Xmmm::String &name)
{
    b2_ = XmmmCreateBox2(parent, (char *)name.c(), NULL, 0);
    postinit();
}

Box1::~Box1(void)
{
    XtDestroyWidget(b2_);
}

::Widget Box1::internal(void)
{
    return b2_;
}

