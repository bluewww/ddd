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

#ifndef GTKX_FONT_H
#define GTKX_FONT_H

#include <pangomm/fontdescription.h>
#include <pangomm/font.h>

#include <GtkX/RefPtr.h>

namespace GtkX {

    class FontDescription: public Pango::FontDescription {
    public:
	FontDescription(void);
	FontDescription(const Pango::FontDescription &);
    };

    class Font {
	int nrefs_;
	Glib::RefPtr<Pango::Font> font_;
    public:
	Font(Glib::RefPtr<Pango::Font> f0);
	static RefPtr<Font> wrap(Glib::RefPtr<Pango::Font> f0);
	static RefPtr<const Font> wrap(Glib::RefPtr<const Pango::Font> f0);
	Glib::RefPtr<Pango::Font> internal(void);
	Glib::RefPtr<const Pango::Font> internal(void) const;
	int ref(void) {return nrefs_++;}
	int unref(void) {if (!--nrefs_) delete this;}
	FontDescription describe(void) const;
    };

}

#endif
