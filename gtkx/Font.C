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

#include <iostream>

#include <GtkX/Font.h>

using namespace GtkX;

FontDescription::FontDescription(void)
{
}

FontDescription::FontDescription(const Pango::FontDescription &src):
    Pango::FontDescription(src)
{
}

FontDescription::~FontDescription()
{
}

typedef std::map<PangoFont *, Font *> FontMap;
FontMap font_map;

Font::Font(Glib::RefPtr<Pango::Font> f0)
{
    nrefs_ = 0;
    font_ = f0;
}

Font::~Font()
{
    font_map.erase(font_->gobj());
    std::cerr << "font_map[-] size now " << font_map.size() << "\n";
}

RefPtr<Font>
Font::wrap(Glib::RefPtr<Pango::Font> f0)
{
    if (!f0) return NULL;
    FontMap::iterator iter = font_map.find(f0->gobj());
    if (iter != font_map.end()) {
	return iter->second;
    }
    Font *f = new Font(f0);
    font_map.insert(std::pair<PangoFont *, Font *>(f0->gobj(), f));
    std::cerr << "font_map[+] size now " << font_map.size() << "\n";
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


typedef std::map<PangoContext *, Context *> ContextMap;
ContextMap context_map;

Context::Context(Glib::RefPtr<Pango::Context> f0)
{
    nrefs_ = 0;
    context_ = f0;
}

Context::~Context()
{
    context_map.erase(context_->gobj());
    std::cerr << "context_map[-] size now " << context_map.size() << "\n";
}

RefPtr<Context>
Context::wrap(Glib::RefPtr<Pango::Context> f0)
{
    if (!f0) return NULL;
    ContextMap::iterator iter = context_map.find(f0->gobj());
    if (iter != context_map.end()) {
	return iter->second;
    }
    Context *f = new Context(f0);
    context_map.insert(std::pair<PangoContext *, Context *>(f0->gobj(), f));
    std::cerr << "context_map[+] size now " << context_map.size() << "\n";
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

void
Context::set_font_description(const FontDescription& desc)
{
    context_->set_font_description(desc);
}

////////////////////

typedef std::map<PangoLayout *, Layout *> LayoutMap;
LayoutMap layout_map;

Layout::Layout(Glib::RefPtr<Pango::Layout> f0)
{
    nrefs_ = 0;
    layout_ = f0;
}

Layout::~Layout()
{
    layout_map.erase(layout_->gobj());
    std::cerr << "layout_map[-] size now " << layout_map.size() << "\n";
}

RefPtr<Layout>
Layout::wrap(Glib::RefPtr<Pango::Layout> f0)
{
    if (!f0) return NULL;
    LayoutMap::iterator iter = layout_map.find(f0->gobj());
    if (iter != layout_map.end()) {
	return iter->second;
    }
    Layout *f = new Layout(f0);
    layout_map.insert(std::pair<PangoLayout *, Layout *>(f0->gobj(), f));
    std::cerr << "layout_map[+] size now " << layout_map.size() << "\n";
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

RefPtr<Layout>
Layout::create(const RefPtr<Context> &context)
{
    return Layout::wrap(Pango::Layout::create(context->internal()));
}

void
Layout::set_text(const String &text)
{
    layout_->set_text(text.s());
}

String
Layout::get_text() const
{
    return layout_->get_text();
}
