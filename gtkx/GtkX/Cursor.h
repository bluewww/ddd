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

#ifndef GTKX_CURSOR_H
#define GTKX_CURSOR_H

#include <gdkmm/cursor.h>
#include <gdkmm/pixmap.h>
#include <GtkX/RefPtr.h>

namespace GtkX {

    class Pixmap;
    class Color;

    enum CursorType
    {
	X_CURSOR = 0,
	ARROW = 2,
	BASED_ARROW_DOWN = 4,
	BASED_ARROW_UP = 6,
	BOAT = 8,
	BOGOSITY = 10,
	BOTTOM_LEFT_CORNER = 12,
	BOTTOM_RIGHT_CORNER = 14,
	BOTTOM_SIDE = 16,
	BOTTOM_TEE = 18,
	BOX_SPIRAL = 20,
	CENTER_PTR = 22,
	CIRCLE = 24,
	CLOCK = 26,
	COFFEE_MUG = 28,
	CROSS = 30,
	CROSS_REVERSE = 32,
	CROSSHAIR = 34,
	DIAMOND_CROSS = 36,
	DOT = 38,
	DOTBOX = 40,
	DOUBLE_ARROW = 42,
	DRAFT_LARGE = 44,
	DRAFT_SMALL = 46,
	DRAPED_BOX = 48,
	EXCHANGE = 50,
	FLEUR = 52,
	GOBBLER = 54,
	GUMBY = 56,
	HAND1 = 58,
	HAND2 = 60,
	HEART = 62,
	ICON = 64,
	IRON_CROSS = 66,
	LEFT_PTR = 68,
	LEFT_SIDE = 70,
	LEFT_TEE = 72,
	LEFTBUTTON = 74,
	LL_ANGLE = 76,
	LR_ANGLE = 78,
	MAN = 80,
	MIDDLEBUTTON = 82,
	MOUSE = 84,
	PENCIL = 86,
	PIRATE = 88,
	PLUS = 90,
	QUESTION_ARROW = 92,
	RIGHT_PTR = 94,
	RIGHT_SIDE = 96,
	RIGHT_TEE = 98,
	RIGHTBUTTON = 100,
	RTL_LOGO = 102,
	SAILBOAT = 104,
	SB_DOWN_ARROW = 106,
	SB_H_DOUBLE_ARROW = 108,
	SB_LEFT_ARROW = 110,
	SB_RIGHT_ARROW = 112,
	SB_UP_ARROW = 114,
	SB_V_DOUBLE_ARROW = 116,
	SHUTTLE = 118,
	SIZING = 120,
	SPIDER = 122,
	SPRAYCAN = 124,
	STAR = 126,
	TARGET = 128,
	TCROSS = 130,
	TOP_LEFT_ARROW = 132,
	TOP_LEFT_CORNER = 134,
	TOP_RIGHT_CORNER = 136,
	TOP_SIDE = 138,
	TOP_TEE = 140,
	TREK = 142,
	UL_ANGLE = 144,
	UMBRELLA = 146,
	UR_ANGLE = 148,
	WATCH = 150,
	XTERM = 152,
	LAST_CURSOR = 153,
	CURSOR_IS_PIXMAP = -1
    };

    class Cursor: public Gdk::Cursor {
    public:
	Cursor(const RefPtr<Pixmap> &source, const RefPtr<Pixmap> &mask,
	       const Color &fg, const Color &bg,
	       int x, int y);
	Cursor(CursorType cursor_type);
    };

}

#endif
