// $Id$
// Own converters

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char converters_rcsid[] = 
    "$Id$";

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#include <X11/CoreP.h>

#include "bool.h"
#include "strclass.h"
#include "charsets.h"
#include "StringSA.h"

#include <Xm/Xm.h>

#define new new_w
#define class class_w

#if XmVersion < 1002
#include <Xm/bitmaps.h>
#else

// Some reasonable defaults...
static unsigned char bitmaps [3][32] =
{
    {  0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88,
       0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88,
       0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88,
       0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88  },

    {  0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55,
       0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55,
       0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55,
       0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55  },

    {  0xFF, 0xFF, 0xAA, 0xAA, 0xFF, 0xFF, 0x55, 0x55,
       0xFF, 0xFF, 0xAA, 0xAA, 0xFF, 0xFF, 0x55, 0x55,
       0xFF, 0xFF, 0xAA, 0xAA, 0xFF, 0xFF, 0x55, 0x55,
       0xFF, 0xFF, 0xAA, 0xAA, 0xFF, 0xFF, 0x55, 0x55  }
};

static String bitmap_name_set[] =
{
   "25_foreground",
   "50_foreground",
   "75_foreground"
};

#endif

// Decl of XmIsSlowSubclass in Motif 1.1 is not C++-aware, hence extern "C"
extern "C" {
#include <Xm/XmP.h>
}

// <Xm/PrimitiveP.h> only exists in Motif 1.2 and later
#if XmVersion >= 1002
#include <Xm/PrimitiveP.h>
#endif

#undef new
#undef class

#include "converters.h"
#include "MString.h"


// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

// Return a value of given type
#define done(type, value) \
    do {                                                \
        if (toVal->addr != NULL) {                      \
            if (toVal->size < sizeof(type)) {           \
                toVal->size = sizeof(type);             \
                return False;                           \
            }                                           \
            *(type *)(toVal->addr) = (value);           \
        }                                               \
        else {                                          \
            static type static_val;                     \
            static_val = (value);                       \
            toVal->addr = (caddr_t)&static_val;         \
        }                                               \
                                                        \
        toVal->size = sizeof(type);                     \
        return True;                                    \
    } while(0)


// Convert String to Widget
// This is based on Asente/Swick: The X Window System Toolkit,
// Digital Press, Example 3.9

Boolean CvtStringToWidget(Display *display, 
			  XrmValue *args, Cardinal *num_args, 
			  XrmValue *fromVal, XrmValue *toVal,
			  XtPointer *)
{
    // convert first arg into parent    
    if (*num_args != 1) 
    {
	XtAppErrorMsg(XtDisplayToApplicationContext(display),
	    "wrongParameters", "CvtStringToWidget",
	    "XtToolkitError",
	    "String to Widget conversion needs parent arg",
	    (String *)NULL, (Cardinal *)NULL);
    }
    Widget parent = *(Widget *) args[0].addr;

    // get widget
    Widget w = XtNameToWidget(parent, (String)fromVal->addr);
    if (w == NULL)
    {
	XtDisplayStringConversionWarning(display, fromVal->addr, XtRWidget);
	return False;
    }

    done(Widget, w);
}


// Convert String to Pixmap (using XmGetPixmap)
// A Pixmap will be read in as bitmap file
// 1 and 0 values are set according to the widget's 
// foreground/background colors.
Boolean CvtStringToPixmap(Display *display, 
			  XrmValue *args, Cardinal *num_args, 
			  XrmValue *fromVal, XrmValue *toVal,
			  XtPointer *)
{
    // Default parameters
    Screen *screen   = DefaultScreenOfDisplay(display);
    Pixel background = WhitePixelOfScreen(screen);
    Pixel foreground = BlackPixelOfScreen(screen);

    if (*num_args >= 1)
    {
	// convert first arg into widget
	Widget w = *(Widget *) args[0].addr;
	background = w->core.background_pixel;
	
	screen = XtScreen(w);
	
	if (XtIsWidget(w) && XmIsPrimitive(w))
	{
	    // Get foreground color from widget
	    foreground = XmPrimitiveWidget(w)->primitive.foreground;
	}
	else
	{
	    // Ask Motif for a default foreground color
	    Pixel newfg, newtop, newbot, newselect;
	    XmGetColors(screen, w->core.colormap, background,
			&newfg, &newtop, &newbot, &newselect);
	    foreground = newfg;
	}
	    
    }

    // get pixmap
    Pixmap p = XmGetPixmap (screen, (String)fromVal->addr,
			    foreground, background);

    if (p == XmUNSPECIFIED_PIXMAP)
    {
	XtDisplayStringConversionWarning(display, fromVal->addr, XtRPixmap);
	return False;
    }

    done(Pixmap, p);
}



static String locateBitmap(Display *display, String basename);

// Convert String to Bitmap
// A Bitmap will be read in as bitmap file -- 1 and 0 values remain unchanged.
Boolean CvtStringToBitmap(Display *display, 
			  XrmValue *, Cardinal *, 
			  XrmValue *fromVal, XrmValue *toVal,
			  XtPointer *)
{
    // Fetch a drawable
    Window window = None;

#if 0
    if (*num_args >= 1)
    {
	// convert first arg into widget
	Widget w = *(Widget *) args[0].addr;
	if (XtIsRealized(w))
	    window = XtWindow(w);
    }
#endif

    if (window == None)
	window = DefaultRootWindow(display);

    // Locate file
    string basename = (String)fromVal->addr;
    String filename = locateBitmap(display, basename);
    if (filename == NULL)
    {
	// Cannot find file -- check for predefined motif bitmaps
	for (Cardinal i = 0; i < XtNumber(bitmap_name_set); i++)
	{
	    if (basename == bitmap_name_set[i])
	    {
		Pixmap bitmap = XCreateBitmapFromData(display, window,
						      (char *)(bitmaps[i]),
						      16, 16);
		if (bitmap == None)
		    break;

		done(Pixmap, bitmap);
	    }
	}

	// Cannot find file and no predefined bitmap found
	XtDisplayStringConversionWarning(display, fromVal->addr, XtRBitmap);
	return False;
    }


    // create pixmap
    unsigned int width, height;
    Pixmap bitmap;
    int x_hot, y_hot;
    int success = XReadBitmapFile(display, window, filename, 
				  &width, &height, &bitmap, &x_hot, &y_hot);
    if (success != BitmapSuccess)
    {
	XtDisplayStringConversionWarning(display, filename, XtRBitmap);
	XtFree(filename);
	return False;
    }

    done(Pixmap, bitmap);
}

// return home directory
static string getHome()
{
    static string home = "";

    // try using $HOME
    if (home == "")
    {
	char *h = getenv("HOME");
	if (h != NULL)
	    home = h;
    }

    // try using passwd entry
    if (home == "")
    {
	struct passwd *pw = NULL;

	char *user = getenv("USER");
	if (user == NULL)
	    user = getenv("LOGNAME");

	if (user != NULL)
	    pw = getpwnam(user);
	else
	    pw = getpwuid(getuid());

	if (pw != NULL)
	    home = pw->pw_dir;
    }

    return home;
}

// Note: <percent>B<percent> is expanded by SCCS -- thus inserting ""
static string BASENAME = "%B""%S";
static string DELIMITER = ":";

// add default search paths to path
static void addDefaultPaths(string& path, string root)
{
    path += DELIMITER + root + "/%L/%T/%N/" + BASENAME;
    path += DELIMITER + root + "/%l/%T/%N/" + BASENAME;
    path += DELIMITER + root + "/%T/%N/"    + BASENAME;
    path += DELIMITER + root + "/%L/%T/"    + BASENAME;
    path += DELIMITER + root + "/%l/%T/"    + BASENAME;
    path += DELIMITER + root + "/%T/"       + BASENAME;
}

// locate path
// this mimics XmGetPixmap's efforts to build a path
static string bitmapPath()
{
    static string path = "";

    if (path != "")
	return path;

    path = BASENAME;
    char *xbmlangpath = getenv("XBMLANGPATH");
    if (xbmlangpath == NULL)
    {
	char *xapplresdir = getenv("XAPPLRESDIR");
	string home = getHome();

	if (xapplresdir != NULL)
	    addDefaultPaths(path, xapplresdir);
	else
	    addDefaultPaths(path, home);

	path += DELIMITER + home + BASENAME;
	addDefaultPaths(path, "/usr/lib/X11");
	path += DELIMITER + "/usr/include/X11/%T/" + BASENAME;
    }
    else
	path = xbmlangpath;

    return path;
}

static string PATH = bitmapPath();

// locate bitmap
// this mimics XmGetPixmap's efforts to locate a path
static String locateBitmap(Display *display, String basename)
{
    SubstitutionRec subst;
    subst.match        = 'B';
    subst.substitution = basename;

    return XtResolvePathname(display,      // the display we use
			     "bitmaps",    // %T = bitmaps
			     NULL,         // %N = application class name
			     "",           // %S = "" (suffix)
			     (String)PATH, // path to use
			     &subst, 1,    // %B = basename
			     NULL);        // no checking for valid bitmap
}

// Macro tables
static StringStringAssoc conversionMacroTable;

// Convert String to XmString, using `@' for font specs:
// `@[font-id] TEXT' makes TEXT be displayed in font font-id
// `@[space]' displays a single `@'.
Boolean CvtStringToXmString(Display *display, 
			    XrmValue *, Cardinal *, 
			    XrmValue *fromVal, XrmValue *toVal,
			    XtPointer *)
{
    const string font_esc = "@";

    static regex font_id("[_A-Za-z][-_A-Za-z0-9]*");
    static regex blank("[ \t]*");

    // get string
    string source = (String)fromVal->addr;
    string charset = (String)MSTRING_DEFAULT_CHARSET;

    int n_segments = source.freq(font_esc) + 1;
    string *segments = new string[n_segments];
    
    split(source, segments, n_segments, font_esc);

    MString buf;
    for (int i = 0; i < n_segments && buf.xmstring() != 0; i++)
    {
	string segment;

	if (i == 0)
	{
	    // At beginning of text
	    segment = segments[i];
	}
	else
	{
	    if (segments[i].contains(font_id, 0))
	    {
		// Found @[font-id] <segment>: process it
		string c = segments[i].through(font_id);
		segment = segments[i].from(int(c.length()));
		if (segment == "")
		{
		    // Found @MACRO@: process it
		    if (conversionMacroTable.has(c))
		    {
			// Replace macro by value
			segment = conversionMacroTable[c] + segments[++i];
		    }
		    else
		    {
			// No such macro
			Cardinal num_params = 1;
			String params = (String)c.chars();
			XtAppWarningMsg(XtDisplayToApplicationContext(display),
					"noSuchMacro", "CvtStringToXmString",
					"XtToolkitError",
					"No such macro: @%s@",
					&params, &num_params);
			segment = font_esc + segment + font_esc;
		    }
		}
		else
		{
		    // Found @CHARSET: set new charset
		    charset = c;
		    segment = segment.after(blank);
		}
	    }
	    else if (segments[i].contains(' ', 0))
	    {
		// found @[space]: remove space
		segment = font_esc + segments[i].from(1);
	    }
	    else
	    {
		// found @[anything-else]: ignore @, take remainder literally
		segment = segments[i];
	    }
	}

	while (segment.contains('\n'))
	{
	    buf += MString(segment.before('\n'), charset) + cr();
	    segment = segment.after('\n');
	}

	if (segment.length() > 0)
	    buf += MString(segment, charset);
    }

    XmString target = XmStringCopy(buf.xmstring());
    delete[] segments;

    if (target == 0)
    {
	XtDisplayStringConversionWarning(display, fromVal->addr, XmRXmString);
	return False;
    }
    
    done(XmString, target);
}


// Convert the strings 'beginning', 'center' and 'end' in any case to a value
// suitable for the specification of the XmNentryAlignment-resource in 
// RowColumn-widgets (or anything else using alignment-resources)
Boolean CvtStringToAlignment(Display*   display, 
                             XrmValue*  ,
                             Cardinal*  , 
                             XrmValue*  fromVal,
                             XrmValue*  toVal,
                             XtPointer* )
{
    string theAlignment = downcase((String)fromVal->addr);
    if (theAlignment.contains("xm", 0))
	theAlignment = theAlignment.after("xm");
    if (theAlignment.contains("alignment_", 0))
	theAlignment = theAlignment.after("alignment_");

    if      (theAlignment == "beginning")
	done(unsigned char, XmALIGNMENT_BEGINNING);
    else if (theAlignment == "center")
	done(unsigned char, XmALIGNMENT_CENTER);
    else if (theAlignment == "end")
	done(unsigned char, XmALIGNMENT_END);

    XtDisplayStringConversionWarning(display, fromVal->addr, XmCAlignment);
    return False;
}


// Convert the strings 'vertical' and 'horizontal' in any case to a value
// suitable for the specification of the XmNorientation-resource in 
// RowColumn-widgets (or anything else using orientation-resources)
Boolean CvtStringToOrientation(Display*         display, 
                               XrmValue*        ,
                               Cardinal*        , 
                               XrmValue*        fromVal,
                               XrmValue*        toVal,
                               XtPointer*       )
{
    string theOrientation = downcase((String)fromVal->addr);
    if (theOrientation.contains("xm", 0))
	theOrientation = theOrientation.after("xm");
  
    if      (theOrientation == "vertical")
	done(unsigned char, XmVERTICAL);
    else if (theOrientation == "horizontal")
	done(unsigned char, XmHORIZONTAL);
    else
    {
	XtDisplayStringConversionWarning(display, fromVal->addr, 
					 XmCOrientation);
	return False;
    }
}


// Convert the strings 'tight', 'column' and 'none' in any case to a value
// suitable for the specification of the XmNpacking-resource in
// RowColumn-widgets (or anything else using packing-resources)
Boolean CvtStringToPacking(Display*     display, 
                           XrmValue*    ,
                           Cardinal*    , 
                           XrmValue*    fromVal,
                           XrmValue*    toVal,
                           XtPointer*   )
{
    string thePacking = downcase((String)fromVal->addr);
    if (thePacking.contains("xm", 0))
	thePacking = thePacking.after("xm");
    if (thePacking.contains("pack_", 0))
	thePacking = thePacking.after("pack_");
  
    if      (thePacking == "tight")
	done(unsigned char, XmPACK_TIGHT);
    else if (thePacking == "column")
	done(unsigned char, XmPACK_COLUMN);
    else if (thePacking == "none")
	done(unsigned char, XmPACK_NONE);


    XtDisplayStringConversionWarning(display, fromVal->addr, XmRPacking);
    return False;
}

// Convert the strings 'pixels', '100th_millimeters' and so on
// to unit types.
Boolean CvtStringToUnitType(Display*     display, 
			    XrmValue*    ,
			    Cardinal*    , 
			    XrmValue*    fromVal,
			    XrmValue*    toVal,
			    XtPointer*   )
{
    string theType = downcase((String)fromVal->addr);
    if (theType.contains("xm", 0))
	theType = theType.after("xm");
  
    if      (theType == "pixels")
	done(unsigned char, XmPIXELS);
    else if (theType == "100th_millimeters")
	done(unsigned char, Xm100TH_MILLIMETERS);
    else if (theType == "1000th_inches")
	done(unsigned char, Xm1000TH_INCHES);
    else if (theType == "100th_points")
	done(unsigned char, Xm100TH_POINTS);
    else if (theType == "100th_font_units")
	done(unsigned char, Xm100TH_FONT_UNITS);

    XtDisplayStringConversionWarning(display, fromVal->addr, XmRUnitType);
    return False;
}


// register all converters

void registerOwnConverters()
{
    static XtConvertArgRec parentCvtArgs[] = {
    { XtWidgetBaseOffset, XtPointer(XtOffsetOf(CoreRec, core.parent)),
	  sizeof(CoreWidget) }
    };

    static XtConvertArgRec thisCvtArgs[] = {
    { XtBaseOffset, XtPointer(0), 
	  sizeof(Widget) }
    };

    // string -> widget
    XtSetTypeConverter(XtRString, XtRWidget, CvtStringToWidget,
		       parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
		       XtDestructor(NULL));

    // string -> window
    // We use CvtStringToWidget for conversions to "Window" as well,
    // since Motif widgets want a widget id in their "Window" fields.
    XtSetTypeConverter(XtRString, XtRWindow, CvtStringToWidget,
		       parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
		       XtDestructor(NULL));

    // string -> pixmap
    XtSetTypeConverter(XtRString, XtRPixmap, CvtStringToPixmap,
		       thisCvtArgs, XtNumber(thisCvtArgs), XtCacheNone,
		       XtDestructor(NULL));

    // string -> bitmap
    XtSetTypeConverter(XtRString, XtRBitmap, CvtStringToBitmap,
		       thisCvtArgs, XtNumber(thisCvtArgs), XtCacheByDisplay,
		       XtDestructor(NULL));

    // string -> xmstring
    XtSetTypeConverter(XmRString, XmRXmString, CvtStringToXmString,
		       NULL, 0, XtCacheNone,
		       XtDestructor(NULL));

    // string -> unitType
    XtSetTypeConverter(XmRString, XmRUnitType, CvtStringToUnitType,
		       NULL, 0, XtCacheAll, 
		       XtDestructor(NULL));


    // The following three were contributed by Thorsten Sommer
    // (sommer@ips.cs.tu-bs.de):

    // string -> alignment
    XtSetTypeConverter(XtRString, XmNentryAlignment, CvtStringToAlignment,
		       parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
		       XtDestructor(NULL));

    // string -> orientation
    XtSetTypeConverter(XtRString, XmNorientation, CvtStringToOrientation,
		       parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
		       XtDestructor(NULL));

    // string -> packing
    XtSetTypeConverter(XtRString, XmNpacking, CvtStringToPacking,
		       parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
		       XtDestructor(NULL));
}

void defineConversionMacro(String name, String value)
{
    conversionMacroTable[name] = value;
}
