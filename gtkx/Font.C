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

Font::~Font()
{
    font_map.erase(font_->gobj());
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

////////////////////


typedef std::map<PangoContext *, RefPtr<Context> > ContextMap;
ContextMap context_map;

Context::Context(Glib::RefPtr<Pango::Context> f0)
{
    nrefs_ = 0;
    context_ = f0;
}

Context::~Context()
{
    context_map.erase(context_->gobj());
}

RefPtr<Context>
Context::wrap(Glib::RefPtr<Pango::Context> f0)
{
    if (!f0) return NULL;
    ContextMap::iterator iter = context_map.find(f0->gobj());
    if (iter != context_map.end()) {
	return iter->second;
    }
    RefPtr<Context> f = new Context(f0);
    context_map.insert(std::pair<PangoContext *, RefPtr<Context> >(f0->gobj(), f));
    return f;
}

Glib::RefPtr<Pango::Context>
Context::internal(void)
{
    return context_;
}

Glib::RefPtr<const Pango::Context>
Context::internal(void) const
{
    return context_;
}

////////////////////

typedef std::map<PangoLayout *, RefPtr<Layout> > LayoutMap;
LayoutMap layout_map;

Layout::Layout(Glib::RefPtr<Pango::Layout> f0)
{
    nrefs_ = 0;
    layout_ = f0;
}

Layout::~Layout()
{
    layout_map.erase(layout_->gobj());
}

RefPtr<Layout>
Layout::wrap(Glib::RefPtr<Pango::Layout> f0)
{
    if (!f0) return NULL;
    LayoutMap::iterator iter = layout_map.find(f0->gobj());
    if (iter != layout_map.end()) {
	return iter->second;
    }
    RefPtr<Layout> f = new Layout(f0);
    layout_map.insert(std::pair<PangoLayout *, RefPtr<Layout> >(f0->gobj(), f));
    return f;
}

Glib::RefPtr<Pango::Layout>
Layout::internal(void)
{
    return layout_;
}

Glib::RefPtr<const Pango::Layout>
Layout::internal(void) const
{
    return layout_;
}

