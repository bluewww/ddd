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

#include "AppData.h"
#include "DestroyCB.h"
#include "LiterateA.h"
#include "assert.h"
#include "converters.h"
#include "cook.h"
#include "ddd.h"
#include "events.h"
#include "shell.h"
#include "status.h"
#include "strclass.h"
#include "string-fun.h"
#include "post.h"

#include <ctype.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/PushB.h>


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

// Return a symbolic name for FONT
static string font_type(DDDFont font)
{
    switch (font)
    {
    case DefaultDDDFont:
	return "default font";
    case VariableWidthDDDFont:
 	return "variable width font";
    case FixedWidthDDDFont:
 	return "fixed width font";
    case SymbolDDDFont:
 	return "symbol font";
    default:
	assert(0);
    }
}

// defaults to use if nothing is specified
static string fallbackfont(DDDFont font)
{
    switch (font) 
    {
    case DefaultDDDFont:
 	return "-*-helvetica-bold-r-*-*-*-120-*-*-*-*-iso8859-*";
    case VariableWidthDDDFont:
 	return "-*-helvetica-medium-r-*-*-*-120-*-*-*-*-iso8859-*";
    case FixedWidthDDDFont:
 	return "-*-lucidatypewriter-medium-r-*-*-*-120-*-*-*-*-iso8859-*";
    case SymbolDDDFont:
 	return "-*-symbol-*-*-*-*-*-120-*-*-*-*-adobe-*";
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

string make_font(DDDFont base, const string& override)
{
    string font = "";
    for (FontComponent n = Foundry; n <= AllComponents; n++)
    {
	string w = component(override, n);
	if (w == "" || w == " ")
	    w = component(base, n);
	font += "-" + w;
    }

#if 0
    clog << "make_font(" << font_type(base) << ", " << quote(override) 
	 << ") = " << quote(font) << "\n";
#endif

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



//-----------------------------------------------------------------------------
// Set font resources
//-----------------------------------------------------------------------------

// Simplify font specs
static string simplify_font(DDDFont font, const string& source)
{
    string s = "";

    for (FontComponent n = AllComponents; n >= Foundry; n--)
    {
	string c = component(source, n);
	if (s == "" && c == component(font, n))
	{
	    // Default setting -- ignore
	}
	else
	{
	    s.prepend("-" + c);
	}
    }

    if (s.contains("-*-", 0))
	s = s.after("-*-");

    if (s == "")
	s = component(font, Family);
    if (!s.contains('-'))
	s += "-" + component(font, Weight);

#if 0
    clog << "simplify_font(" << font_type(font) << ", " 
	 << quote(source) << ") = " << quote(s) << "\n";
#endif

    return s;
}

// Set a new font resource
void set_font(DDDFont font, const string& name)
{
    switch (font)
    {
    case DefaultDDDFont:
    {
	static string s;
	s = name;
	app_data.default_font = s;
	break;
    }
    case VariableWidthDDDFont:
    {
	static string s;
	s = name;
	app_data.variable_width_font = s;
	break;
    }
    case FixedWidthDDDFont:
    {
	static string s;
	s = name;
	app_data.fixed_width_font = s;
	break;
    }
    default:
	assert(0);
    }
}

// Set a new font resource
void set_font_size(DDDFont font, int size)
{
    switch (font)
    {
    case DefaultDDDFont:
	app_data.default_font_size = size;
	break;
    case VariableWidthDDDFont:
	app_data.variable_width_font_size = size;
	break;
    case FixedWidthDDDFont:
	app_data.fixed_width_font_size = size;
	break;
    default:
	assert(0);
    }
}


void SetFontNameCB(Widget w, XtPointer client_data, XtPointer)
{
    DDDFont font = (DDDFont)client_data;
    String s = XmTextFieldGetString(w);
    set_font(font, s);
    XtFree(s);

    update_reset_preferences();
}

void SetFontSizeCB(Widget w, XtPointer client_data, XtPointer)
{
    DDDFont font = (DDDFont)client_data;
    String s = XmTextFieldGetString(w);
    set_font_size(font, atoi(s));
    XtFree(s);

    update_reset_preferences();
}

//-----------------------------------------------------------------------------
// Font browser
//-----------------------------------------------------------------------------

struct FontSelectInfo {
    DDDFont font;
    Widget text;
};

static void gdbDeleteFontSelectAgent(XtPointer client_data, XtIntervalId *)
{
    // Delete agent after use
    Agent *font_select_agent = (Agent *)client_data;
    delete font_select_agent;
}

static void DeleteAgentHP(Agent *agent, void *client_data, void *)
{
    FontSelectInfo *info = (FontSelectInfo *)client_data;

    // Agent has died -- delete it
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
		    gdbDeleteFontSelectAgent, 
		    XtPointer(agent));

    // Destroy the text
    DestroyWhenIdle(info->text);
}

static void FontSelectionErrorHP(Agent *, void *, void *call_data)
{
    // Fetch stderr output from font selector
    DataLength *input = (DataLength *)call_data;
    post_warning(string(input->data, input->length), "font_selector_warning");
}


static void process_font(DDDFont font, string fontspec)
{
    string sz = component(fontspec, PointSize);
    if (sz != "*")
	set_font_size(font, atoi(sz));

    fontspec.gsub('*', ' ');
    set_font(font, simplify_font(font, make_font(font, fontspec)));

    update_options();
}

// Handle `Quit' button in xfontsel
static void FontSelectionDoneHP(Agent *agent, void *client_data, 
				void *call_data)
{
    FontSelectInfo *info = (FontSelectInfo *)client_data;

    // Fetch string from font selector
    DataLength *d = (DataLength *)call_data;
    string fontspec(d->data, d->length);
    strip_space(fontspec);

    if (!fontspec.contains('-', 0))
    {
	// Treat as error
	FontSelectionErrorHP(agent, client_data, call_data);
	return;
    }

    process_font(info->font, fontspec);
}


static void GotSelectionCB(Widget w, XtPointer client_data,
			   Atom *, Atom *type, XtPointer value,
			   unsigned long *length, int *format)
{
    FontSelectInfo *info = (FontSelectInfo *)client_data;

    if (*type == None)
	return;			// Could not fetch selection

    if (*type != XA_STRING)
    {
	XtFree((char *)value);
	return;			// Not a string
    }

    if (*format != 8)
    {
	XtFree((char *)value);
	return;			// No 8-bit-string
    }

    string s((String)value, *length);
    if (s.contains('\0'))
	s = s.before('\0');
    XtFree((char *)value);

    if (s.contains("-", 0) && !s.contains('\n') && 
	s.freq('-') == AllComponents)
    {
	process_font(info->font, s);
    }

    XmTextSetString(w, (String)s);

    // Get the selection again.
    // This will fail if we have multiple font selectors (FIXME).
    XmTextSetSelection(w, 0, s.length(), 
		       XtLastTimestampProcessed(XtDisplay(w)));
}

// Handle `Select' button in xfontsel
static void SelectionLostCB(Widget w, XtPointer client_data, XtPointer)
{
    FontSelectInfo *info = (FontSelectInfo *)client_data;
    assert(info->text == w);

    XtGetSelectionValue(w, XA_PRIMARY, XA_STRING, GotSelectionCB,
			XtPointer(info), 
			XtLastTimestampProcessed(XtDisplay(w)));
}


// Browse fonts
void BrowseFontCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Time tm = CurrentTime;
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    if (cbs && cbs->event)
	tm = time(cbs->event);

    DDDFont font = (DDDFont)client_data;

    StatusDelay delay("Invoking " + font_type(font) + " selector");

    string cmd = app_data.font_select_command;
    cmd.gsub("@FONT@", make_font(DefaultDDDFont));
    string type = font_type(font);
    type[0] = toupper(type[0]);
    cmd.gsub("@TYPE@", type);
    cmd = sh_command(cmd, true);

    // Create a TextField to fetch the selection
    FontSelectInfo *info = new FontSelectInfo;
    info->text = XmCreateText(XtParent(w), "text", 0, 0);
    info->font = font;

    XtRealizeWidget(info->text);

    string text = "dummy";
    XmTextSetString(info->text, (String)text);
    XmTextSetSelection(info->text, 0, text.length(), tm);
    XtAddCallback(info->text, XmNlosePrimaryCallback, 
		  SelectionLostCB, XtPointer(info));

    // Invoke a font selector
    LiterateAgent *font_select_agent = 
	new LiterateAgent(XtWidgetToApplicationContext(w), cmd);
    font_select_agent->removeAllHandlers(Died);
    font_select_agent->addHandler(Died,
				  DeleteAgentHP, (void *)info);
    font_select_agent->addHandler(Input,
				  FontSelectionDoneHP, (void *)info);
    font_select_agent->addHandler(Error,
				  FontSelectionErrorHP, (void *)info);
    font_select_agent->start();
}
