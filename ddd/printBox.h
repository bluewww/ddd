// $Id$  -*- C++ -*-
// definitions for xfig-output - protocol version 2.1
// comments in /*...*/ taken from xfig-spec-2.1

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Christian Lindig (lindig@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_printBox_h
#define _DDD_printBox_h

/*
(1) The very first line is a comment line containing the name and version:
	#FIG 2.1

    The character # at the first column of a line indicates that the line
    is a comment line which will be ignored.

(2) The first non-comment line consists of two numbers :

	int	fig_resolution		(pixels/inch)
	int	coordinate_system	(1 : origin is at the lower left corner
					 2 : Upper left)

    Fig_resolution is the resolution fig is using for the current file.
    It must always be 80 for compatibility with this version of Fig.
    Pixels are assumed to be square.
*/

// The header.
#define FIGHEAD "#FIG 2.1\n" \
                "80 2\n"     \
                "#Creator: Box::print() by Christian Lindig\n"


/*
(3) The rest of the file contains various objects.  An object can be one
    of six classes (or types).

	  i)	Arc.
	 ii)	Ellipse which is a generalization of circle.
	iii)	Polyline which includes polygon and box.
	 iv)	Spline which includes closed/open control/interpolated spline.
	  v)	Text.
	 vi)	Compound object which is composed of one or more objects.

    In the following elaboration on object formats, every value of fig
    output are separated by blank characters or new line ('\n').  The
    value of the not-used parameters will be -1.

    Some fields are described as "enumeration type" or "bit vector"; the 
    values which these fields can take are defined in the header file object.h.
    The pen, arrow_type, and arrow_style fields are described as unused.
    These values may be defined in some future version of Fig.

    The color field is defined as follows:

	-1 = Default
	 0 = Black
	 1 = Blue
	 2 = Green
	 3 = Cyan
	 4 = Red
	 5 = Magenta
	 6 = Yellow
	 7 = White
	 values from 8 to 2^30-1 = reserved for future use

    The depth field is defined as follows:

	 0 ... 999 where larger value means object is deeper than (under)
		   objects with smaller depth

    The area fill field is defined as follows:
	
	 0 = not filled
	 1 = white
	...  values from 2 to 20 are shades of grey, from lighter to darker
	21 = black

    The line_style field is defined as follows:

	-1 = Default
	 0 = Solid
	 1 = Dashed
	 2 = Dotted

    The style_val field is defined as the length, in pixels, of the
    on/off dashes for dashed lines, and the distance between the dots, in
    pixels, for dotted lines.
*/

/*
    (3.4) POLYLINE

    First line :
	type	name			(brief description)
	----	----			-------------------
	int	object_code		(always 2)
	int	sub_type		(1 : polyline
					 2 : box
					 3 : polygon
					 4 : arc-box)
					 5 : imported-eps bounding-box)
	int	line_style		(enumeration type)
	int	thickness		(pixels)
	int	color			(enumeration type)
	int	depth			(no units)
	int	pen			(not used)
	int	area_fill		(enumeration type)
	float	style_val		(pixels)
	int	radius			(pixels)
	int	forward_arrow		(0: off, 1: on)
	int	backward_arrow		(0: off, 1: on)

    Forward arrow line (Optional; absent if forward_arrow is 0) :
	type	name			(brief description)
	----	----			-------------------
	int	arrow_type		(not used)
	int	arrow_style		(not used)
	float	arrow_thickness		(pixels)
	float 	arrow_width		(pixels)
	float	arrow_height		(pixels)

    Backward arrow line (Optional; absent if backward_arrow is 0) :
	type	name			(brief description)
	----	----			-------------------
	int	arrow_type		(not used)
	int	arrow_style		(not used)
	float	arrow_thickness		(pixels)
	float	arrow_width		(pixels)
	float	arrow_height		(pixels)

    Points line :
	type	name			(brief description)
	----	----			-------------------
	int	x1, y1			(pixels)
	int	x2, y2			(pixels)
	  .
	  .
	int	xn, yn			(this will be the same as the 1st
					point for polygon and box)
	int	x, y			(always 9999, 9999; marks the end of
					point for polygon and box)

    EPS line :
	type	name			(brief description)
	----	----			-------------------
	bool	flipped			orientation = normal (0) or flipped (1)
	char	file[]			name of eps file to import
*/ 

// A black line (for edges, depth = 0)
#define EDGEHEAD1 "2 1 0 " // +thickness
#define EDGEHEAD2 " -1 2 0 0 0.000 0 0 0\n\t"

// Same, but with arrows (for edges, depth = 2)
#define ARROWHEAD1 "2 1 0 " // +thickness
#define ARROWHEAD2 " -1 2 0 0 0.000 0 1 0\n\t -1 -1 1 4 10\n\t"

// A black line (for boxes, depth = 0)
#define LINEHEAD1 "2 1 0 " // +thickness
#define LINEHEAD2 " -1 0 0 0 0.000 0 0 0\n\t"

// A black rule (for boxes, depth = 0)
#define RULEHEAD "2 2 0 1 -1 0 0 21 0.000 0 0 0\n\t"

// A white rectangle in the background (behind boxes, depth = 1)
#define CLEANHEAD "2 2 0 0 7 1 0 21 0.000 0 0 0\n\t"


/*
    (3.2) COMPOUND

    A line with object code 6 signifies the start of a compound.
    There are four more numbers on this line which indicate the
    upper right corner and the lower left corner of the bounding
    box of this compound.  A line with object code -6 signifies
    the end of the compound.  Compound may be nested.

    First line :
	type	name			(brief description)
	----	----			-------------------
	int	object_code		(always 6)
	int	upperright_corner_x	(pixels)
	int	upperright_corner_y	(pixels)
	int	lowerleft_corner_x	(pixels)
	int	lowerleft_corner_y	(pixels)

    Subsequent lines :
	objects
	.
	.

    Last line :
	-6
*/

#define CMPHEAD "6 " // +corners
#define CMPTAIL "-6\n"


/*
    (3.6) TEXT
	type	name			(brief description)
	----	----			-------------------
	int	object 			(always 4)
	int	sub_type		(0 : Left justified
					 1 : Center justified
					 2 : Right justified)
	int	font 			(enumeration type)
	int	font_size 		(points)
	int	pen			(not used)
	int	color			(enumeration type)
	int	depth			(no units)
	float	angle			(radians, the angle of the text)
	int	font_flags		(bit vector)
	int	height			(pixels)
	int	length			(pixels)
	int	x, y			(pixels, coordinate of the origin
					 of the string.  If sub_type = 0, it is
					 the lower left corner of the string.
					 If sub_type = 1, it is the lower
					 center.  Otherwise it is the lower
					 right corner of the string.)
	char	string[]		(ascii characters; starts after a blank
					character following the last number and
					ends before the character '\1'.  This
					character is not part of the string.
					Note that the string may contain '\n'.)
*/

// Texts (depth = 0)
#define TEXTHEAD1 "4 0 " // +font +font_size
#define TEXTHEAD2 " 0 -1 0 0.000 4 "

#endif // _DDD_printBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
