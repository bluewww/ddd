// $Id$ -*- C++ -*-
// An area to plot upon

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

// Most of this code is based on `gplt_x11.c' from Gnuplot.
//
// Acknowledgements: 
//    Chris Peterson (MIT)
//    Dana Chee (Bellcore) 
//    Arthur Smith (Cornell)
//    Hendri Hondorp (University of Twente, The Netherlands)
//    Bill Kucharski (Solbourne)
//    Charlie Kline (University of Illinois)
//    Yehavi Bourvine (Hebrew University of Jerusalem, Israel)
//    Russell Lang (Monash University, Australia)
//    O'Reilly & Associates: X Window System - Volumes 1 & 2
//
// This code is provided as is and with no warranties of any kind.

char PlotArea_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PlotArea.h"

#include "cook.h"

#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <Xm/DrawingA.h>

// Data
struct plot_resource_values {
    String font;
    int pointsize;
};

static XtResource plot_subresources[] = {
    {
	"font",
	"Font",
	XtRString,
	sizeof(String),
	XtOffsetOf(plot_resource_values, font),
	XtRImmediate,
	XtPointer("fixed")
    },
    {
	"pointsize",
	"Pointsize",
	XtRInt,
	sizeof(int),
	XtOffsetOf(plot_resource_values, pointsize),
	XtRImmediate,
	XtPointer(1)
    }
};


#if 0
static char color_keys[Ncolors][30] =   { 
   "background", "bordercolor", "text", "border", "axis", 
   "line1", "line2", "line3",  "line4", 
   "line5", "line6", "line7",  "line8" 
   };
#endif

static char color_values[Ncolors][30] = { 
    "white", "black",  "black",  "black",  "black", 
    "red",   "green",  "blue",   "magenta", 
    "cyan",  "sienna", "orange", "coral" 
};

static char gray_values[Ncolors][30] = { 
    "black",   "white",  "white",  "gray50", "gray50",
    "gray100", "gray60", "gray80", "gray40", 
    "gray90",  "gray50", "gray70", "gray30" 
};

#if 0
char dash_keys[Ndashes][10] =   { 
   "border", "axis",
   "line1", "line2", "line3",  "line4", "line5", "line6", "line7",  "line8" 
};
#endif

static char dash_mono[Ndashes][10] =   { 
   "0", "16",
   "0", "42", "13",  "44", "15", "4441", "42",  "13" 
};

static char dash_color[Ndashes][10] =   { 
   "0", "16",
   "0", "0", "0", "0", "0", "0", "0", "0" 
};

// Initialize
PlotArea::PlotArea(Widget w, const string& fontname)
    : area(w), dpy(XtDisplay(w)), win(XtWindow(w)),
      cx(0), cy(0), px(1), py(1), xscale(0.0), yscale(0.0),
      gc(0), font(0), vchar(0), jmode(0), line_type(0), width(0),
      type(LineSolid), pointsize(1), commands("")
{
    plot_resource_values values;
    XtGetApplicationResources(area, &values,
			      plot_subresources, XtNumber(plot_subresources),
			      NULL, 0);

    // Init font
    font = XLoadQueryFont(dpy, fontname);
    if (font == 0)
	font = XLoadQueryFont(dpy, values.font);
    if (font == 0)
	font = XLoadQueryFont(dpy, "fixed");
    if (font == 0)
    {
	cerr << "Cannot load font\n";
	exit(1);
    }
    
    vchar = font->ascent + font->descent;

    // Init point size
    pointsize = values.pointsize;
    if (pointsize <= 0 || pointsize > 10) 
    {
	cerr << "Invalid point size " << pointsize << "\n";
	pointsize = 1;
    }

    // Init colors
    Pixel black = BlackPixelOfScreen(XtScreen(area));
    Pixel white = WhitePixelOfScreen(XtScreen(area));
    int depth;
    XtVaGetValues(area, XmNdepth, &depth, NULL);
    if (depth <= 1)
    {
	// Monochrome
	colors[0] = white;
	for (int i = 1; i < Ncolors; i++)
	    colors[i] = black;
    }
    else
    {
	// Multi-color or gray
	Visual *vis = DefaultVisualOfScreen(XtScreen(area));
	bool gray = (vis->c_class == StaticGray || vis->c_class == GrayScale);
	Colormap cmap;
	XtVaGetValues(area, XmNcolormap, &cmap, NULL);

	for (int i = 0; i < Ncolors; i++)
	{
	    string color = gray ? gray_values[i] : color_values[i];
	    XColor xcolor;
	    if (!XParseColor(dpy, cmap, color, &xcolor))
	    {
		cerr << "Unable to parse " << quote(color) 
		     << ".  Using black.\n";
		colors[i] = black;
	    }
	    else
	    {
		if (XAllocColor(dpy, cmap, &xcolor)) {
		    colors[i] = xcolor.pixel;
		}
		else
		{
		    cerr << "Cannot allocate " << quote(color)
			 << ".  Using black.\n";
		    colors[i] = black;
		}
	    }
	}
    }

    // Init dashes
    int i;
    for (i = 0; i < Ndashes; i++)
    { 
	string v;
	if (depth <= 1)
	    v = dash_mono[i];
	else
	    v = dash_color[i];

	if (v.length() == 0 || v[0] == '0')
	{
	    dashes[i][0] = (unsigned char)0;
	}
	else
	{
	    for (int j = 0; j < int(v.length()); j++)
		dashes[i][j] = (unsigned char) (v[j] - '0');
	}
	dashes[i][v.length()] = (unsigned char)0;
    }

    // Init widths
    widths[0] = 2;
    for (i = 1; i < Nwidths; i++)
	widths[i] = 0;
}

// Plot functions

#define X(x) (int) (x * xscale)
#define Y(y) (int) ((4095-y) * yscale)
PlotCommand PlotArea::command_table[] =
{
    { 'V', &PlotArea::plot_vector },
    { 'M', &PlotArea::plot_move },
    { 'T', &PlotArea::plot_text },
    { 'J', &PlotArea::plot_justify },
    { 'L', &PlotArea::plot_linetype },
    { 'P', &PlotArea::plot_point },
    { 'G', &PlotArea::plot_nop },
    { 'E', &PlotArea::plot_nop },
    { 'R', &PlotArea::plot_nop },
};


void PlotArea::plot_nop(const string&)
{
    // Ignore command
}

void PlotArea::plot_vector(const string& buf)
{
    int x, y;
    sscanf(buf, "V%4d%4d", &x, &y);  
    XDrawLine(dpy, win, gc, X(cx), Y(cy), X(x), Y(y));
    cx = x; cy = y;
}

void PlotArea::plot_move(const string& buf)
{
    sscanf(buf, "M%4d%4d", &cx, &cy);
}

void PlotArea::plot_text(const string& buf)
{
    int x, y;
    sscanf(buf, "T%4d%4d", &x, &y);  
    char *str = (char *)buf + 9;
    int sl = strlen(str);
    int sw = XTextWidth(font, str, sl);
    switch (jmode) {
    case 0: sw = 0;     break;	// left
    case 1: sw = -sw/2; break;	// center
    case 2: sw = -sw;   break;	// right
    }
    XSetForeground(dpy, gc, colors[2]);
    XDrawString(dpy, win, gc, X(x)+sw, Y(y) + vchar / 3, str, sl);
    XSetForeground(dpy, gc, colors[line_type + 3]);
}

void PlotArea::plot_justify(const string& buf)
{
    sscanf(buf, "J%4d", &jmode);
}

void PlotArea::plot_linetype(const string& buf)
{
    sscanf(buf, "L%4d", &line_type);
    line_type = (line_type % 8) + 2;
    width = widths[line_type];
    if (dashes[line_type][0])
    {
	type = LineOnOffDash;
	XSetDashes(dpy, gc, 0, dashes[line_type], strlen(dashes[line_type]));
    }
    else
    {
	type = LineSolid;
    }
    XSetForeground(dpy, gc, colors[line_type + 3]);
    XSetLineAttributes(dpy, gc, width, type, CapButt, JoinBevel);
}

void PlotArea::plot_point(const string& buf)
{
    int point, x, y;
    sscanf(buf, "P%1d%4d%4d", &point, &x, &y);  
    if (point == 7) 
    {
	// Set point size
	px = (int) (x * xscale * pointsize);
	py = (int) (y * yscale * pointsize);
	return;
    }

    if (type != LineSolid || width != 0) 
    {
	// Select solid line
	XSetLineAttributes(dpy, gc, 0, LineSolid, CapButt, JoinBevel);
    }

    switch(point) {
    case 0: // Dot
	XDrawLine(dpy,win,gc, X(x), Y(y), X(x), Y(y));
	break;

    case 1: // Diamond
	XDrawLine(dpy,win,gc, X(x)-px, Y(y), X(x), Y(y)-py);
	XDrawLine(dpy,win,gc, X(x), Y(y)-py, X(x)+px, Y(y));
	XDrawLine(dpy,win,gc, X(x)+px, Y(y), X(x), Y(y)+py);
	XDrawLine(dpy,win,gc, X(x), Y(y)+py, X(x)-px, Y(y));
	XDrawLine(dpy,win,gc, X(x), Y(y), X(x), Y(y));
	break;

    case 2: // Plus
	XDrawLine(dpy,win,gc, X(x)-px, Y(y), X(x)+px, Y(y));
	XDrawLine(dpy,win,gc, X(x), Y(y)-py, X(x), Y(y)+py);
	break;

    case 3: // Box
	XDrawLine(dpy,win,gc, X(x)-px, Y(y)-py, X(x)+px, Y(y)-py);
	XDrawLine(dpy,win,gc, X(x)+px, Y(y)-py, X(x)+px, Y(y)+py);
	XDrawLine(dpy,win,gc, X(x)+px, Y(y)+py, X(x)-px, Y(y)+py);
	XDrawLine(dpy,win,gc, X(x)-px, Y(y)+py, X(x)-px, Y(y)-py);
	XDrawLine(dpy,win,gc, X(x), Y(y), X(x), Y(y));
	break;

    case 4: // X
	XDrawLine(dpy,win,gc, X(x)-px, Y(y)-py, X(x)+px, Y(y)+py);
	XDrawLine(dpy,win,gc, X(x)-px, Y(y)+py, X(x)+px, Y(y)-py);
	break;

    case 5: // Triangle
	XDrawLine(dpy,win,gc, X(x), Y(y)-(4*px/3), 
		  X(x)-(4*px/3), Y(y)+(2*py/3));
	XDrawLine(dpy,win,gc, X(x), Y(y)-(4*px/3), 
		  X(x)+(4*px/3), Y(y)+(2*py/3));
	XDrawLine(dpy,win,gc, X(x)-(4*px/3), Y(y)+(2*py/3), 
		  X(x)+(4*px/3), Y(y)+(2*py/3));
	XDrawLine(dpy,win,gc, X(x), Y(y), X(x), Y(y));
	break;

    case 6: // Star
	XDrawLine(dpy,win,gc, X(x)-px, Y(y), X(x)+px, Y(y));
	XDrawLine(dpy,win,gc, X(x), Y(y)-py, X(x), Y(y)+py);
	XDrawLine(dpy,win,gc, X(x)-px, Y(y)-py, X(x)+px, Y(y)+py);
	XDrawLine(dpy,win,gc, X(x)-px, Y(y)+py, X(x)+px, Y(y)-py);
	break;
    }

    if (type != LineSolid || width != 0)
    {  
	// Restore line type
	XSetLineAttributes(dpy, gc, width, type, CapButt, JoinBevel);
    }
}

void PlotArea::plot(const string& cmds, bool clear)
{
    commands = cmds;

    if (!XtIsRealized(area))
	return;

    // Initialize
    Dimension width, height;
    int depth;
    Pixel background;
    XtVaGetValues(area, 
		  XmNwidth, &width, 
		  XmNheight, &height, 
		  XmNdepth, &depth, 
		  XmNbackground, &background,
		  NULL);

    // Set scaling factor between internal driver & window geometry
    xscale = (double)width  / 4096.0;
    yscale = (double)height / 4096.0;

    cx = 0;
    cy = 0;

    type = LineSolid;
    win = XtWindow(area);

    // Create new GC
    if (gc != 0)
	XFreeGC(dpy, gc);
    gc = XCreateGC(dpy, win, 0, (XGCValues *)0);
    XSetFont(dpy, gc, font->fid);

    if (clear)
    {
	// Set win background
	XSetForeground(dpy, gc, background);
	XFillRectangle(dpy, win, gc, 0, 0, width, height);
	XSetBackground(dpy, gc, background);
    }

    int n = commands.freq('\n');
    string *lines = new string[n + 1];
    split(commands, lines, n + 1, '\n');

    for (int i = 0; i < n - 1; i++)
    {
	const string& line = lines[i];

	bool processed = false;
	for (int j = 0; !processed && j < int(XtNumber(command_table)); j++)
	{
	    if (line.contains(command_table[j].code, 0))
	    {
		PlotAreaFunc func = command_table[j].func;
		(this->*func)(lines[i]);
		processed = true;
	    }
	}

	if (!processed)
	    cerr << "Unknown command: " << quote(lines[i]) << "\n";
    }

    delete[] lines;
}

void PlotArea::replot(bool clear)
{
    plot(commands, clear);
}

