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
#include "assert.h"


//-----------------------------------------------------------------------------
// Return font attributes
//-----------------------------------------------------------------------------

//  1     2    3    4     5     6  7     8    9    10   11  12   13     14
// -fndry-fmly-wght-slant-sWdth-ad-pxlsz-ptSz-resx-resy-spc-avgW-rgstry-encdng


const int Foundry       = 1;
const int Family        = 2;
const int Weight        = 3;
const int Slant         = 4;
const int sWidth        = 5;
const int Ad            = 6;
const int PixelSize     = 7;
const int PointSize     = 8;
const int ResX          = 9;
const int ResY          = 10;
const int Spacing       = 11;
const int AvgWidth      = 12;
const int Registry      = 13;
const int Encoding      = 14;

const int AllComponents = 14;

typedef int FontComponent;

// Return the Nth component from NAME, or DEFAULT_VALUE if none
static string component(string name, FontComponent n)
{
    // If name does not begin with `-', assume it's a font family
    if (!name.contains('-', 0))
	name.prepend("-*-");

    // Let I point to the Nth occurrence of `-'
    int i = -1;
    while (n >= Foundry && (i = name.index('-', i + 1)) >= 0)
	n--;

    string w = "";
    if (i >= 0)
    {
	w = name.after(i);
	if (w.contains('-'))
	    w = w.before('-');
    }

    return w;
}


//-----------------------------------------------------------------------------
// Access font resources
//-----------------------------------------------------------------------------

// User-specified values
static string userfont(DDDFont font)
{
    switch (font) 
    {
    case DefaultDDDFont:
	return app_data.default_font;
    case VariableWidthDDDFont:
	return app_data.variable_width_font;
    case FixedWidthDDDFont:
	return app_data.fixed_width_font;
    case SymbolDDDFont:
	return "";
    }

    assert(0);
    return "";			// Never reached
}

// defaults to use if nothing is specified
static string fallbackfont(DDDFont font)
{
    switch (font) 
    {
    case DefaultDDDFont:
 	return "-*-helvetica-bold-r-*-*-*-90-*-*-*-*-iso8859-*";
    case VariableWidthDDDFont:
 	return "-*-helvetica-medium-r-*-*-*-90-*-*-*-*-iso8859-*";
    case FixedWidthDDDFont:
 	return "-*-lucidatypewriter-medium-r-*-*-*-90-*-*-*-*-iso8859-*";
    case SymbolDDDFont:
 	return "-*-symbol-*-*-*-*-*-90-*-*-*-*-adobe-*";
    }

    assert(0);
    return "";
}

// Fetch a component
static string component(DDDFont font, FontComponent n)
{
    if (n == PointSize)
    {
	int sz = 0;
	switch(font)
	{
	case DefaultDDDFont:
	    sz = app_data.default_font_size;
	    break;

	case VariableWidthDDDFont:
	case SymbolDDDFont:
	    sz = app_data.variable_width_font_size;
	    break;

	case FixedWidthDDDFont:
	    sz = app_data.fixed_width_font_size;
	    break;
	}

	return itostring(sz);
    }

    string w = component(userfont(font), n);
    if (w == "")		// nothing specified
	w = component(fallbackfont(font), n);
    return w;
}



//-----------------------------------------------------------------------------
// Create a font name
//-----------------------------------------------------------------------------

static string override(FontComponent new_n, 
		       const string& new_value, const string& font = "")
{
    string new_font = "";
    for (FontComponent n = Foundry; n <= AllComponents; n++)
    {
	string w;
	if (n == new_n)
	    w = new_value;
	else
	    w = component(font, n);
	new_font += "-" + w;
    }

    return new_font;
}

string make_font(DDDFont base, const string& override = "")
{
    string font = "";
    for (FontComponent n = Foundry; n <= AllComponents; n++)
    {
	string w = component(override, n);
	if (w == "" || w == " ")
	    w = component(base, n);
	font += "-" + w;
    }

    return font;
}

static void define_font(const string& name, DDDFont base, 
			const string& override = "")
{
    string font = make_font(base, override);
    defineConversionMacro(name, font);

    if (app_data.show_fonts)
	cout << "@" << name << "@    \t" << font << "\n";
}


static void setup_x_fonts()
{
    Dimension small_size = 
	((app_data.default_font_size * 8) / 90) * 10;
    Dimension llogo_size =
	((app_data.default_font_size * 3) / 20) * 10;

    if (small_size < 80)
	small_size = app_data.default_font_size;

    string small_size_s = itostring(small_size);
    string llogo_size_s = itostring(llogo_size);

	
    // Default font
    define_font("CHARSET", DefaultDDDFont);

    define_font("SMALL", DefaultDDDFont,
		override(PointSize, small_size_s));

    define_font("LIGHT", DefaultDDDFont,
		override(Weight, "medium",
			 override(PointSize, small_size_s)));

    // Text fonts
    define_font("TEXT", FixedWidthDDDFont);

    // Text fonts
    define_font("LOGO", VariableWidthDDDFont,
		override(Weight, "bold"));

    define_font("LLOGO", VariableWidthDDDFont,
		override(Weight, "bold",
			 override(PointSize, llogo_size_s)));

    define_font("RM", VariableWidthDDDFont,
		override(Slant, "r"));

    define_font("SL", VariableWidthDDDFont,
		override(Slant, "*")); // matches `i' and `o'

    define_font("BF", VariableWidthDDDFont,
		override(Weight, "bold",
			 override(Slant, "r")));

    define_font("BS", VariableWidthDDDFont,
		override(Weight, "bold",
			 override(Slant, "*")));

    define_font("TT", FixedWidthDDDFont);

    define_font("TB", FixedWidthDDDFont,
		override(Weight, "bold"));

    define_font("KEY", VariableWidthDDDFont,
		override(Weight, "bold"));

    define_font("SYMBOL", SymbolDDDFont);
}

static void replace_vsl_def(string& s, const string& func, const string& val)
{
    s += "#pragma replace " + func + "\n" + func + "() = " + val + ";\n";
}

static void setup_vsl_fonts()
{
    static string defs;

    replace_vsl_def(defs, "stdfontsize", "0");
    replace_vsl_def(defs, "stdfontpoints",
		    component(FixedWidthDDDFont, PointSize));
    replace_vsl_def(defs, "stdfontfamily", 
		    quote(component(FixedWidthDDDFont, Family)));
    replace_vsl_def(defs, "stdfontweight", 
		    quote(component(FixedWidthDDDFont, Weight)));

    if (app_data.show_fonts)
	cout << defs;

    defs += app_data.vsl_base_defs;
    app_data.vsl_base_defs = defs;
}

void setup_fonts()
{
    setup_x_fonts();
    setup_vsl_fonts();
}
