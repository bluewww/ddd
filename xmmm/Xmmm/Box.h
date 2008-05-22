// -*- C++ -*-

// High-level GUI wrapper for Xm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#ifndef XMMM_BOX_H
#define XMMM_BOX_H

#include <Xmmm/Container.h>
#include <Xm/RowColumn.h>

namespace Xmmm {

    class Box: public Container {
    };

    class VBox: public Box {
	::Widget rc_;
    public:
	void init(::Widget parent, const Xmmm::String &name="");
	VBox(Xmmm::Widget &parent, const Xmmm::String &name="");
	VBox(::Widget parent, const Xmmm::String &name=""); // TEMPORARY
	~VBox(void);
	::Widget internal(void); // TEMPORARY
	operator ::Widget(void); // TEMPORARY
    };

    class HBox: public Box {
	::Widget rc_;
    public:
	void init(::Widget parent, const Xmmm::String &name="");
	HBox(Xmmm::Widget &parent, const Xmmm::String &name="");
	HBox(::Widget parent, const Xmmm::String &name=""); // TEMPORARY
	~HBox(void);
	::Widget internal(void); // TEMPORARY
    };

}

#endif // XMMM_BOX_H
