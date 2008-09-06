// -*- C++ -*-

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
// A brief look at QT indicates that this will be required there as
// well.

#ifndef GTKX_IMAGEW_H
#define GTKX_IMAGEW_H

#include <GtkX/Container.h>
#include <GtkX/Image.h>
#include <gtkmm/image.h>

namespace GtkX {

    class Image: public Widget {
	Gtk::Image *image_;
    public:
	Image(Container &parent, PackOptions po=PACK_SHRINK,
	      const String &name="");
	Image(Container &parent, PackOptions po,
	      const String &name, const ImageHandle &im);
	~Image(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	void set(const ImageHandle &);
    };

}

#endif // GTKX_IMAGEW_H
