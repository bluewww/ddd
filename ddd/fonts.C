// $Id$ -*- C++ -*-
// Setup DDD fonts

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char fonts_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "fonts.h"

#include "converters.h"
#include "strclass.h"
#include "AppData.h"
#include "string-fun.h"
#include "cook.h"

//  1     2    3    4     5     6  7     8    9    10   11  12   13     14
// -fndry-fmly-wght-slant-sWdth-ad-pxlsz-ptSz-resx-resy-spc-avgW-rgstry-encdng

// Return the Nth component from NAME, or DEFAULT_VALUE if none
static string word(string name, int n, const string& default_value)
{
    // If name does not begin with `-', assume it's a font family
    if (!name.contains('-', 0))
	name.prepend("-*-");

    // Let I point to the Nth occurrence of `-'
    int i = -1;
    while (n > 0 && (i = name.index('-', i + 1)) >= 0)
	n--;

    string w = "";
    if (i >= 0)
    {
	w = name.after(i);
	if (w.contains('-'))
	    w = w.before('-');
    }

    if (w == "")
	w = default_value;

    return w;
}

inline string family(const string& font)
{
    return word(font, 2, "*");
}

inline string weight(const string& font)
{
    return word(font, 3, "*");
}

static string slant(const string& font)
{
    return word(font, 4, "*");
}

static string registry(const string& font)
{
    return word(font, 13, "iso8859");
}

static string make_font(const string& fam, const string& weight,
			const string& slant, int size, 
			const string& registry)
{
    return "-*-" + fam + "-" + weight + "-" + slant + "-*-*-*-" + 
	itostring(size) + "-*-*-*-*-" + registry + "-*";
}

static void define_font(const string& name,
			const string& fam, const string& weight,
			const string& slant, int size,
			const string& registry, bool show)
{
    string font = make_font(fam, weight, slant, size, registry);
    defineConversionMacro(name, font);

    if (show)
	cout << "@" << name << "@ =\t" << font << "\n";
}

static void setup_x_fonts(bool show)
{
    Dimension small_size = 
	((app_data.default_font_size * 8) / 90) * 10;
    Dimension llogo_size =
	((app_data.default_font_size * 3) / 20) * 10;

    if (small_size < 80)
	small_size = app_data.default_font_size;

	
    // Default font
    define_font("CHARSET",
		family(app_data.default_font),
		weight(app_data.default_font),
		slant(app_data.default_font),
		app_data.default_font_size,
		registry(app_data.default_font), show);

    define_font("SMALL",
		family(app_data.default_font),
		weight(app_data.default_font),
		slant(app_data.default_font),
		small_size,
		registry(app_data.default_font), show);

    define_font("LIGHT",
		family(app_data.default_font),
		"medium",
		slant(app_data.default_font),
		small_size,
		registry(app_data.default_font), show);

    define_font("LOGO",
		family(app_data.default_font),
		"bold",
		slant(app_data.default_font),
		app_data.default_font_size,
		registry(app_data.default_font), show);

    define_font("LLOGO",
		family(app_data.default_font),
		"bold",
		slant(app_data.default_font),
		llogo_size,
		registry(app_data.default_font), show);

    define_font("KEY",
		family(app_data.default_font),
		weight(app_data.default_font),
		slant(app_data.default_font),
		app_data.default_font_size,
		registry(app_data.default_font), show);

    // Text fonts
    define_font("TEXT",
		family(app_data.fixed_width_font),
		weight(app_data.fixed_width_font),
		slant(app_data.fixed_width_font),
		app_data.fixed_width_font_size,
		registry(app_data.fixed_width_font), show);

    // Text fonts
    define_font("RM",
		family(app_data.variable_width_font),
		weight(app_data.variable_width_font),
		"r",
		app_data.variable_width_font_size,
		registry(app_data.variable_width_font), show);

    define_font("SL",
		family(app_data.variable_width_font),
		weight(app_data.variable_width_font),
		"*",	// matches both "i" and "o"
		app_data.variable_width_font_size,
		registry(app_data.variable_width_font), show);

    define_font("BF",
		family(app_data.variable_width_font),
		"bold",
		"r",
		app_data.variable_width_font_size,
		registry(app_data.variable_width_font), show);

    define_font("BS",
		family(app_data.variable_width_font),
		"bold",
		"*",	// matches both "i" and "o"
		app_data.variable_width_font_size,
		registry(app_data.variable_width_font), show);

    define_font("TT",
		family(app_data.fixed_width_font),
		weight(app_data.fixed_width_font),
		slant(app_data.fixed_width_font),
		app_data.variable_width_font_size,
		registry(app_data.fixed_width_font), show);

    define_font("TB",
		family(app_data.fixed_width_font),
		"bold",
		slant(app_data.fixed_width_font),
		app_data.variable_width_font_size,
		registry(app_data.fixed_width_font), show);

    define_font("SYMBOL",
		"symbol",
		"*",
		"*",
		app_data.variable_width_font_size,
		"adobe", show);
}

static void _replace_vsl_def(string& s, const string& func, const string& val)
{
    s += "#pragma replace " + func + "\n" + func + "() = " + val + ";\n";
}

static void replace_vsl_def(string& s, const string& func, int val)
{
    _replace_vsl_def(s, func, itostring(val));
}

static void replace_vsl_def(string& s, const string& func, const string& val)
{
    _replace_vsl_def(s, func, quote(val));
}

static void setup_vsl_fonts(bool show)
{
    static string defs;

    replace_vsl_def(defs, "stdfontsize", 0);
    replace_vsl_def(defs, "stdfontpoints", app_data.fixed_width_font_size);
    replace_vsl_def(defs, "stdfontfamily", family(app_data.fixed_width_font));
    replace_vsl_def(defs, "stdfontweight", weight(app_data.fixed_width_font));

    if (show)
	cout << defs;

    defs += app_data.vsl_base_defs;
    app_data.vsl_base_defs = defs;
}

void setup_fonts(bool show)
{
    setup_x_fonts(show);
    setup_vsl_fonts(show);
}
