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

#include <GtkX/Font.h>

using namespace GtkX;

FontDescription::FontDescription(void)
{
}

FontDescription::FontDescription(const Pango::FontDescription &src):
    Pango::FontDescription(src)
{
}

typedef std::map<PangoFont *, RefPtr<Font> > FontMap;
FontMap font_map;

Font::Font(Glib::RefPtr<Pango::Font> f0)
{
    nrefs_ = 0;
    font_ = f0;
}

RefPtr<Font>
Font::wrap(Glib::RefPtr<Pango::Font> f0)
{
    if (!f0) return NULL;
    FontMap::iterator iter = font_map.find(f0->gobj());
    if (iter != font_map.end()) {
	return iter->second;
    }
    RefPtr<Font> f = new Font(f0);
    font_map.insert(std::pair<PangoFont *, RefPtr<Font> >(f0->gobj(), f));
    return f;
}

Glib::RefPtr<Pango::Font>
Font::internal(void)
{
    return font_;
}

Glib::RefPtr<const Pango::Font>
Font::internal(void) const
{
    return font_;
}

FontDescription
Font::describe(void) const
{
    return FontDescription(font_->describe());
}

