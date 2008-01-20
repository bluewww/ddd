// High-level GUI wrapper for Xmmm.

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

#include <Xm/Scale.h>

#include <Xmmm/Scale.h>

using namespace Xmmm;

void
Scale::init_signals(void)
{
}

Scale::~Scale(void)
{
    XtDestroyWidget(scale_);
}

::Widget Scale::internal(void)
{
    return scale_;
}



HScale::HScale(::Widget parent, const Xmmm::String &name)
{
    Arg args[1];
    XtSetArg(args[0], XmNorientation, XmHORIZONTAL);
    scale_ = XmCreateScale(parent, (char *)name.c(), args, 1);
    init_signals();
    postinit();
}

HScale::HScale(Xmmm::Container &parent, const Xmmm::String &name)
{
    Arg args[1];
    XtSetArg(args[0], XmNorientation, XmHORIZONTAL);
    scale_ = XmCreateScale(parent.xt_container(), (char *)name.c(), args, 1);
    init_signals();
    postinit();
}



VScale::VScale(::Widget parent, const Xmmm::String &name)
{
    Arg args[1];
    XtSetArg(args[0], XmNorientation, XmVERTICAL);
    scale_ = XmCreateScale(parent, (char *)name.c(), args, 1);
    init_signals();
    postinit();
}

VScale::VScale(Xmmm::Container &parent, const Xmmm::String &name)
{
    Arg args[1];
    XtSetArg(args[0], XmNorientation, XmVERTICAL);
    scale_ = XmCreateScale(parent.xt_container(), (char *)name.c(), args, 1);
    init_signals();
    postinit();
}

