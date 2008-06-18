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

#include <GtkX/Cursor.h>
#include <GtkX/Widget.h>

using namespace GtkX;

static Gdk::Color make_color(const Color &rgb)
{
    Gdk::Color c;
    c.set_rgb_p(rgb.r, rgb.g, rgb.b);
    return c;
}

Cursor::Cursor(const RefPtr<Pixmap> &source, const RefPtr<Pixmap> &mask,
	       const Color &fg, const Color &bg,
	       int x, int y):
    Gdk::Cursor(source->gdk_pixmap(), mask->gdk_pixmap(),
		make_color(fg), make_color(bg), x, y)
{
}


