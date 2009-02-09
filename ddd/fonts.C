// $Id$ -*- C++ -*-
// Setup DDD fonts

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2003 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char fonts_rcsid[] = 
    "$Id$";

#include "fonts.h"
#include "charsets.h"

#include "AppData.h"
#include "DestroyCB.h"
#include "LessTifH.h"
#include "LiterateA.h"
#include "StringSA.h"
#include "TextSetS.h"
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
#include "TimeOut.h"

#include <stdlib.h>		// atoi()
#include <ctype.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/PushB.h>
#include <X11/Xatom.h>		// XA_...


//-----------------------------------------------------------------------------
// Return X font attributes
//-----------------------------------------------------------------------------

//  1     2    3    4     5     6  7     8    9    10   11  12   13     14
// -fndry-fmly-wght-slant-sWdth-ad-pxlsz-ptSz-resx-resy-spc-avgW-rgstry-encdng

const int Foundry       = 1;
const int Family        = 2;
const int Weight        = 3;
const int Slant         = 4;
// const int sWidth     = 5;
// const int Ad         = 6;
// const int PixelSize  = 7;
const int PointSize     = 8;
// const int ResX       = 9;
// const int ResY       = 10;
// const int Spacing    = 11;
// const int AvgWidth   = 12;
// const int Registry   = 13;
// const int Encoding   = 14;

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

    string w;
    if (i >= 0)
    {
	w = name.after(i);
	if (w.contains('-'))
	    w = w.before('-');
    }

    return w;
}


//-----------------------------------------------------------------------------
// Access X font resources
//-----------------------------------------------------------------------------

// User-specified values
static string userfont(const AppData& ad, DDDFont font)
{
    switch (font) 
    {
    case DefaultDDDFont:
	return ad.default_font;
    case VariableWidthDDDFont:
	return ad.variable_width_font;
    case FixedWidthDDDFont:
	return ad.fixed_width_font;
    case DataDDDFont:
	return ad.data_font;
    case SymbolDDDFont:
	return "";
    }

    assert(0);
    ::abort();
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
    case DataDDDFont:
 	return "data font";
    case SymbolDDDFont:
 	return "symbol font";
    }

    assert(0);
    ::abort();
    return "";			// Never reached
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
    case DataDDDFont:
 	return "-*-lucidatypewriter-medium-r-*-*-*-120-*-*-*-*-iso8859-*";
    case SymbolDDDFont:
 	return "-*-symbol-*-*-*-*-*-120-*-*-*-*-adobe-*";
    }

    assert(0);
    ::abort();
    return "";			// Never reached
}

// Fetch a component
static string component(const AppData& ad, DDDFont font, FontComponent n)
{
    if (n == PointSize)
    {
	int sz = 0;
	switch(font)
	{
	case DefaultDDDFont:
	    sz = ad.default_font_size;
	    break;

	case VariableWidthDDDFont:
	case SymbolDDDFont:
	    sz = ad.variable_width_font_size;
	    break;

	case FixedWidthDDDFont:
	    sz = ad.fixed_width_font_size;
	    break;

	case DataDDDFont:
	    sz = ad.data_font_size;
	    break;
	}

	return itostring(sz);
    }

    string w = component(userfont(ad, font), n);
    if (w.empty())		// nothing specified
	w = component(fallbackfont(font), n);
    return w;
}



//-----------------------------------------------------------------------------
// Create an X font name
//-----------------------------------------------------------------------------

static string override(FontComponent new_n, 
		       const string& new_value, const string& font = "")
{
    string new_font;
    for (FontComponent n = Foundry; n <= AllComponents; n++)
    {
	new_font += '-';
	new_font +=
	  (n == new_n) ?
	  new_value :
	  component(font, n);
    }

    return new_font;
}

string make_font(const AppData& ad, DDDFont base, const string& override)
{
    string font;
    for (FontComponent n = Foundry; n <= AllComponents; n++)
    {
	font += '-';
	string w = component(override, n);
	if (w.empty() || w == " ")
	    w = component(ad, base, n);
	font += w;
    }

#if 0
    std::clog << "make_font(" << font_type(base) << ", " << quote(override) 
	      << ") = " << quote(font) << "\n";
#endif

    return font;
}



//-----------------------------------------------------------------------------
// Setup X fonts
//-----------------------------------------------------------------------------

static StringStringAssoc font_defs;

static void define_font(const AppData& ad,
			const string& name, DDDFont base, 
			const string& override = "")
{
    string font = make_font(ad, base, override);
    const string s1 = upcase(name);
    defineConversionMacro(s1.chars(), font.chars());
    font_defs[name] = font;

    if (ad.show_fonts)
    {
	const string sym =
	  (name == MSTRING_DEFAULT_CHARSET) ?
	  "default" :
	  "@" + name;
	std::cout << sym << "\t" << font << "\n";
    }
}

static void set_db_font(const AppData& ad, XrmDatabase& db,
			const string& line)
{
    XrmPutLineResource(&db, line.chars());

    if (ad.show_fonts)
    {
	string s = line;
	s.gsub(":", ": \\\n   ");
	s.gsub(",", ",\\\n    ");
	std::cout << s << "\n\n";
    }
}

static string define_default_font(const string& font_def)
{
    string s;

    // The canonical way of doing it.
    s += font_def + "=" + MSTRING_DEFAULT_CHARSET + ",";

    // Special cases
    if (string(MSTRING_DEFAULT_CHARSET) != "charset")
    {
	// This happens in Motif 1.1 and LessTif.  Ensure
	// compatibility with other Motif versions.
	s += font_def;
	s += "=charset,";
    }

    return s;
}
	    

static void setup_font_db(const AppData& ad, XrmDatabase& db)
{
    // Default fonts
    string special_fontlist = 
	font_defs[CHARSET_SMALL] + "=" + CHARSET_SMALL + "," +
	font_defs[CHARSET_TT] + "=" + CHARSET_TT + "," +
	font_defs[CHARSET_TB] + "=" + CHARSET_TB + "," +
	font_defs[CHARSET_KEY] + "=" + CHARSET_KEY + "," +
	font_defs[CHARSET_RM] + "=" + CHARSET_RM + "," +
	font_defs[CHARSET_SL] + "=" + CHARSET_SL + "," +
	font_defs[CHARSET_BF] + "=" + CHARSET_BF + "," +
	font_defs[CHARSET_BS] + "=" + CHARSET_BS + "," +
	font_defs[CHARSET_LOGO] + "=" + CHARSET_LOGO + "," +
	font_defs[CHARSET_LLOGO] + "=" + CHARSET_LLOGO + "," +
	font_defs[CHARSET_SYMBOL] + "=" + CHARSET_SYMBOL;


    string default_fontlist = 
	define_default_font(font_defs[CHARSET_DEFAULT]) + special_fontlist;

    set_db_font(ad, db, string(DDD_CLASS_NAME "*") + 
		XmCFontList + ": " + default_fontlist);

    // Text fonts
    string text_fontlist = 
	define_default_font(font_defs[CHARSET_TEXT]) + special_fontlist;

    set_db_font(ad, db, string(DDD_CLASS_NAME "*XmTextField.") + 
		XmCFontList + ": " + text_fontlist);
    set_db_font(ad, db, string(DDD_CLASS_NAME "*XmText.") + 
		XmCFontList + ": " + text_fontlist);
    set_db_font(ad, db, string(DDD_CLASS_NAME "*XmCommand*XmList.") + 
		XmCFontList + ": " + text_fontlist);

    // Command tool fonts
    string tool_fontlist = 
	define_default_font(font_defs[CHARSET_LIGHT]) + special_fontlist;

    set_db_font(ad, db, 
		string(DDD_CLASS_NAME "*tool_buttons.run.") + 
		XmCFontList + ": " + default_fontlist);
    set_db_font(ad, db, 
		string(DDD_CLASS_NAME "*tool_buttons.break.") +
		XmCFontList + ": " + default_fontlist);
    set_db_font(ad, db, 
		string(DDD_CLASS_NAME "*tool_buttons*") +
		XmCFontList + ": " + tool_fontlist);
}

static void title(const AppData& ad, const string& s)
{
    if (!ad.show_fonts)
	return;

    static bool title_seen = false;

    if (title_seen)
	std::cout << "\n\n";

    std::cout << s << "\n" << replicate('-', s.length()) << "\n\n";

    title_seen = true;
}

static void get_derived_sizes(Dimension size,
			      Dimension& small_size,
			      Dimension& tiny_size,
			      Dimension& llogo_size)
{
    small_size = ((size * 8) / 90) * 10;
    tiny_size  = ((size * 6) / 90) * 10;
    llogo_size = ((size * 3) / 20) * 10;
}

static void setup_x_fonts(const AppData& ad, XrmDatabase& db)
{
    Dimension small_size, tiny_size, llogo_size;
    get_derived_sizes(ad.default_font_size, small_size, tiny_size, llogo_size);

    if (small_size < 80)
	small_size = ad.default_font_size;

    string small_size_s = itostring(small_size);
    string llogo_size_s = itostring(llogo_size);

    // Clear old font defs
    static StringStringAssoc empty;
    font_defs = empty;

    title(ad, "Symbolic font names");

    // Default font
    define_font(ad, CHARSET_DEFAULT, DefaultDDDFont);

    define_font(ad, CHARSET_SMALL, DefaultDDDFont,
		override(PointSize, small_size_s));

    define_font(ad, CHARSET_LIGHT, DefaultDDDFont,
		override(Weight, "medium",
			 override(PointSize, small_size_s)));

    // Text fonts
    define_font(ad, CHARSET_TEXT, FixedWidthDDDFont);

    // Text fonts
    define_font(ad, CHARSET_LOGO, VariableWidthDDDFont,
		override(Weight, "bold"));

    define_font(ad, CHARSET_LLOGO, VariableWidthDDDFont,
		override(Weight, "bold",
			 override(PointSize, llogo_size_s)));

    define_font(ad, CHARSET_RM, VariableWidthDDDFont,
		override(Slant, "r"));

    define_font(ad, CHARSET_SL, VariableWidthDDDFont,
		override(Slant, "*")); // matches `i' and `o'

    define_font(ad, CHARSET_BF, VariableWidthDDDFont,
		override(Weight, "bold",
			 override(Slant, "r")));

    define_font(ad, CHARSET_BS, VariableWidthDDDFont,
		override(Weight, "bold",
			 override(Slant, "*")));

    define_font(ad, CHARSET_TT, FixedWidthDDDFont);

    define_font(ad, CHARSET_TB, FixedWidthDDDFont,
		override(Weight, "bold"));

    define_font(ad, CHARSET_TS, FixedWidthDDDFont,
		override(Slant, "*"));

    define_font(ad, CHARSET_TBS, FixedWidthDDDFont,
		override(Weight, "bold",
			 override(Slant, "*")));

    define_font(ad, CHARSET_KEY, VariableWidthDDDFont,
		override(Weight, "bold"));

    define_font(ad, CHARSET_SYMBOL, SymbolDDDFont);

    title(ad, "Font resources");

    setup_font_db(ad, db);
}



//-----------------------------------------------------------------------------
// Set VSL font resources
//-----------------------------------------------------------------------------

static void replace_vsl_font(string& defs, const string& func, 
			     const AppData& ad, const string& override = "",
			     DDDFont font = DataDDDFont)
{
    string fontname = quote(make_font(ad, font, override));
    defs += "#pragma replace " + func + "\n" + 
	func + "(box) = font(box, " + fontname + ");\n";
}

static void setup_vsl_fonts(AppData& ad)
{
    Dimension small_size, tiny_size, llogo_size;
    get_derived_sizes(ad.data_font_size, small_size, tiny_size, llogo_size);

    string small_size_s = itostring(small_size);
    string tiny_size_s  = itostring(tiny_size);

    static string defs;
    defs = "";

    title(ad, "VSL defs");

    replace_vsl_font(defs, "rm", ad);
    replace_vsl_font(defs, "bf", ad, 
		     override(Weight, "bold"));
    replace_vsl_font(defs, "it", ad, 
		     override(Slant, "*"));
    replace_vsl_font(defs, "bf", ad, 
		     override(Weight, "bold",
			      override(Slant, "*")));

    replace_vsl_font(defs, "small_rm", ad, 
		     override(PointSize, small_size_s));
    replace_vsl_font(defs, "small_bf", ad, 
		     override(Weight, "bold", 
			      override(PointSize, small_size_s)));
    replace_vsl_font(defs, "small_it", ad, 
		     override(Slant, "*", 
			      override(PointSize, small_size_s)));
    replace_vsl_font(defs, "small_bf", ad, 
		     override(Weight, "bold", 
			      override(Slant, "*", 
				       override(PointSize, small_size_s))));

    replace_vsl_font(defs, "tiny_rm", ad, 
		     override(PointSize, tiny_size_s), VariableWidthDDDFont);
    replace_vsl_font(defs, "tiny_bf", ad, 
		     override(Weight, "bold", 
			      override(PointSize, tiny_size_s)), 
		     VariableWidthDDDFont);
    replace_vsl_font(defs, "tiny_it", ad, 
		     override(Slant, "*", 
			      override(PointSize, tiny_size_s)),
		     VariableWidthDDDFont);
    replace_vsl_font(defs, "tiny_bf", ad, 
		     override(Weight, "bold", 
			      override(Slant, "*", 
				       override(PointSize, tiny_size_s))),
		     VariableWidthDDDFont);

    if (ad.show_fonts)
	std::cout << defs;

    defs += ad.vsl_base_defs;
    ad.vsl_base_defs = defs.chars();
}

void setup_fonts(AppData& ad, XrmDatabase db)
{
    XrmDatabase db2 = db;
    setup_x_fonts(ad, db2);
    assert(db == db2);

    setup_vsl_fonts(ad);
}



//-----------------------------------------------------------------------------
// Handle font resources
//-----------------------------------------------------------------------------

// Simplify font specs
static string simplify_font(const AppData& ad, DDDFont font, 
			    const string& source)
{
    string s;

    for (FontComponent n = AllComponents; n >= Foundry; n--)
    {
	string c = component(source, n);
	if (s.empty() && c == component(ad, font, n))
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

    if (s.empty())
	s = component(ad, font, Family);
    if (!s.contains('-')) {
	s += '-';
	s += component(ad, font, Weight);
    }

#if 0
    std::clog << "simplify_font(" << font_type(font) << ", " 
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
	app_data.default_font = s.chars();
	break;
    }
    case VariableWidthDDDFont:
    {
	static string s;
	s = name;
	app_data.variable_width_font = s.chars();
	break;
    }
    case FixedWidthDDDFont:
    {
	static string s;
	s = name;
	app_data.fixed_width_font = s.chars();
	break;
    }
    case DataDDDFont:
    {
	static string s;
	s = name;
	app_data.data_font = s.chars();
	break;
    }
    default:
	assert(0);
	::abort();
    }
}

// Set a new font resource
static void set_font_size(DDDFont font, int size)
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
    case DataDDDFont:
	app_data.data_font_size = size;
	break;
    default:
	assert(0);
	::abort();
    }
}


void SetFontNameCB(Widget w, XtPointer client_data, XtPointer)
{
    DDDFont font = (DDDFont) (long) client_data;
    String s = XmTextFieldGetString(w);
    set_font(font, s);
    XtFree(s);

    update_reset_preferences();
}

void SetFontSizeCB(Widget w, XtPointer client_data, XtPointer)
{
    DDDFont font = (DDDFont) (long) client_data;
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

static string output_buffer;

static void FontSelectionErrorHP(Agent *, void *, void *call_data)
{
    DataLength *input = (DataLength *)call_data;
    output_buffer += string(input->data, input->length);
    while (output_buffer.contains('\n'))
    {
	set_status(output_buffer.before('\n'));
	output_buffer = output_buffer.after('\n');
    }
    if (!output_buffer.empty())
	set_status(output_buffer);
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

    if (!output_buffer.empty())
	set_status("");
}

static void process_font(DDDFont font, string fontspec)
{
    string sz = component(fontspec, PointSize);
    if (sz != "*")
	set_font_size(font, atoi(sz.chars()));

    fontspec.gsub('*', ' ');
    set_font(font, simplify_font(app_data, font, 
				 make_font(app_data, font, fontspec)));

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

    XmTextSetString(w, XMST(s.chars()));

    // Get the selection again.
    // This will fail if we have multiple font selectors (FIXME).
    TextSetSelection(w, 0, s.length(), 
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

    DDDFont font = (DDDFont) (long) client_data;

    StatusDelay delay("Invoking " + font_type(font) + " selector");

    string cmd = app_data.font_select_command;
    cmd.gsub("@FONT@", make_font(app_data, DefaultDDDFont));
    string type = font_type(font);
    type[0] = toupper(type[0]);
    cmd.gsub("@TYPE@", type);
    cmd = sh_command(cmd, true);

    // Create a TextField to fetch the selection
    FontSelectInfo *info = new FontSelectInfo;
    info->text = XmCreateText(XtParent(w), XMST("text"), 0, 0);
    info->font = font;

    XtRealizeWidget(info->text);

    const string text = "dummy";
    XmTextSetString(info->text, XMST(text.chars()));
    TextSetSelection(info->text, 0, text.length(), tm);
    XtAddCallback(info->text, XmNlosePrimaryCallback, 
		  SelectionLostCB, XtPointer(info));

    // Invoke a font selector
    LiterateAgent *font_select_agent = 
	new LiterateAgent(XtWidgetToApplicationContext(w), cmd);

    output_buffer = "";

    font_select_agent->removeAllHandlers(Died);
    font_select_agent->addHandler(Died,
				  DeleteAgentHP, (void *)info);
    font_select_agent->addHandler(Input,
				  FontSelectionDoneHP, (void *)info);
    font_select_agent->addHandler(Error,
				  FontSelectionErrorHP, (void *)info);
    font_select_agent->start();
}
