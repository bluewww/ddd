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

static Gdk::CursorType
translate_cursor_type(CursorType cursor_type)
{
    Gdk::CursorType ct;
    switch(cursor_type) {
    case X_CURSOR: ct = Gdk::X_CURSOR; break;
    case ARROW: ct = Gdk::ARROW; break;
    case BASED_ARROW_DOWN: ct = Gdk::BASED_ARROW_DOWN; break;
    case BASED_ARROW_UP: ct = Gdk::BASED_ARROW_UP; break;
    case BOAT: ct = Gdk::BOAT; break;
    case BOGOSITY: ct = Gdk::BOGOSITY; break;
    case BOTTOM_LEFT_CORNER: ct = Gdk::BOTTOM_LEFT_CORNER; break;
    case BOTTOM_RIGHT_CORNER: ct = Gdk::BOTTOM_RIGHT_CORNER; break;
    case BOTTOM_SIDE: ct = Gdk::BOTTOM_SIDE; break;
    case BOTTOM_TEE: ct = Gdk::BOTTOM_TEE; break;
    case BOX_SPIRAL: ct = Gdk::BOX_SPIRAL; break;
    case CENTER_PTR: ct = Gdk::CENTER_PTR; break;
    case CIRCLE: ct = Gdk::CIRCLE; break;
    case CLOCK: ct = Gdk::CLOCK; break;
    case COFFEE_MUG: ct = Gdk::COFFEE_MUG; break;
    case CROSS: ct = Gdk::CROSS; break;
    case CROSS_REVERSE: ct = Gdk::CROSS_REVERSE; break;
    case CROSSHAIR: ct = Gdk::CROSSHAIR; break;
    case DIAMOND_CROSS: ct = Gdk::DIAMOND_CROSS; break;
    case DOT: ct = Gdk::DOT; break;
    case DOTBOX: ct = Gdk::DOTBOX; break;
    case DOUBLE_ARROW: ct = Gdk::DOUBLE_ARROW; break;
    case DRAFT_LARGE: ct = Gdk::DRAFT_LARGE; break;
    case DRAFT_SMALL: ct = Gdk::DRAFT_SMALL; break;
    case DRAPED_BOX: ct = Gdk::DRAPED_BOX; break;
    case EXCHANGE: ct = Gdk::EXCHANGE; break;
    case FLEUR: ct = Gdk::FLEUR; break;
    case GOBBLER: ct = Gdk::GOBBLER; break;
    case GUMBY: ct = Gdk::GUMBY; break;
    case HAND1: ct = Gdk::HAND1; break;
    case HAND2: ct = Gdk::HAND2; break;
    case HEART: ct = Gdk::HEART; break;
    case ICON: ct = Gdk::ICON; break;
    case IRON_CROSS: ct = Gdk::IRON_CROSS; break;
    case LEFT_PTR: ct = Gdk::LEFT_PTR; break;
    case LEFT_SIDE: ct = Gdk::LEFT_SIDE; break;
    case LEFT_TEE: ct = Gdk::LEFT_TEE; break;
    case LEFTBUTTON: ct = Gdk::LEFTBUTTON; break;
    case LL_ANGLE: ct = Gdk::LL_ANGLE; break;
    case LR_ANGLE: ct = Gdk::LR_ANGLE; break;
    case MAN: ct = Gdk::MAN; break;
    case MIDDLEBUTTON: ct = Gdk::MIDDLEBUTTON; break;
    case MOUSE: ct = Gdk::MOUSE; break;
    case PENCIL: ct = Gdk::PENCIL; break;
    case PIRATE: ct = Gdk::PIRATE; break;
    case PLUS: ct = Gdk::PLUS; break;
    case QUESTION_ARROW: ct = Gdk::QUESTION_ARROW; break;
    case RIGHT_PTR: ct = Gdk::RIGHT_PTR; break;
    case RIGHT_SIDE: ct = Gdk::RIGHT_SIDE; break;
    case RIGHT_TEE: ct = Gdk::RIGHT_TEE; break;
    case RIGHTBUTTON: ct = Gdk::RIGHTBUTTON; break;
    case RTL_LOGO: ct = Gdk::RTL_LOGO; break;
    case SAILBOAT: ct = Gdk::SAILBOAT; break;
    case SB_DOWN_ARROW: ct = Gdk::SB_DOWN_ARROW; break;
    case SB_H_DOUBLE_ARROW: ct = Gdk::SB_H_DOUBLE_ARROW; break;
    case SB_LEFT_ARROW: ct = Gdk::SB_LEFT_ARROW; break;
    case SB_RIGHT_ARROW: ct = Gdk::SB_RIGHT_ARROW; break;
    case SB_UP_ARROW: ct = Gdk::SB_UP_ARROW; break;
    case SB_V_DOUBLE_ARROW: ct = Gdk::SB_V_DOUBLE_ARROW; break;
    case SHUTTLE: ct = Gdk::SHUTTLE; break;
    case SIZING: ct = Gdk::SIZING; break;
    case SPIDER: ct = Gdk::SPIDER; break;
    case SPRAYCAN: ct = Gdk::SPRAYCAN; break;
    case STAR: ct = Gdk::STAR; break;
    case TARGET: ct = Gdk::TARGET; break;
    case TCROSS: ct = Gdk::TCROSS; break;
    case TOP_LEFT_ARROW: ct = Gdk::TOP_LEFT_ARROW; break;
    case TOP_LEFT_CORNER: ct = Gdk::TOP_LEFT_CORNER; break;
    case TOP_RIGHT_CORNER: ct = Gdk::TOP_RIGHT_CORNER; break;
    case TOP_SIDE: ct = Gdk::TOP_SIDE; break;
    case TOP_TEE: ct = Gdk::TOP_TEE; break;
    case TREK: ct = Gdk::TREK; break;
    case UL_ANGLE: ct = Gdk::UL_ANGLE; break;
    case UMBRELLA: ct = Gdk::UMBRELLA; break;
    case UR_ANGLE: ct = Gdk::UR_ANGLE; break;
    case WATCH: ct = Gdk::WATCH; break;
    case XTERM: ct = Gdk::XTERM; break;
    case LAST_CURSOR: ct = Gdk::LAST_CURSOR; break;
    default: ct = Gdk::X_CURSOR; break;
    }
    return ct;
}

Cursor::Cursor(CursorType cursor_type):
    Gdk::Cursor(translate_cursor_type(cursor_type))
{
}


