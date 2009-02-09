// $Id$ -*- C++ -*-
// An area to plot upon

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Free Software Foundation, Inc.
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

#include "PlotArea.h"

#include "cook.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
	XTRESSTR("font"),
	XTRESSTR("Font"),
	XtRString,
	sizeof(String),
	XtOffsetOf(plot_resource_values, font),
	XtRImmediate,
	XtPointer("fixed")
    },
    {
	XTRESSTR("pointsize"),
	XTRESSTR("Pointsize"),
	XtRInt,
	sizeof(int),
	XtOffsetOf(plot_resource_values, pointsize),
	XtRImmediate,
	XtPointer(1)
    }
};


#if 0
static const char * const color_keys[Ncolors] =   { 
   "background", "bordercolor", "text", "border", "axis", 
   "line1", "line2", "line3",  "line4", 
   "line5", "line6", "line7",  "line8" 
   };
#endif

static const char * const color_values[Ncolors] = { 
    "white", "black",  "black",  "black",  "black", 
    "red",   "green",  "blue",   "magenta", 
    "cyan",  "sienna", "orange", "coral" 
};

static const char * const gray_values[Ncolors] = { 
    "black",   "white",  "white",  "gray50", "gray50",
    "gray100", "gray60", "gray80", "gray40", 
    "gray90",  "gray50", "gray70", "gray30" 
};

#if 0
static const char * const dash_keys[Ndashes] =   { 
   "border", "axis",
   "line1", "line2", "line3",  "line4", "line5", "line6", "line7",  "line8" 
};
#endif

static const char * const dash_mono[Ndashes] =   { 
   "0", "16",
   "0", "42", "13",  "44", "15", "4441", "42",  "13" 
};

static const char * const dash_color[Ndashes] =   { 
   "0", "16",
   "0", "0", "0", "0", "0", "0", "0", "0" 
};

// Initialize
PlotArea::PlotArea(Widget w, const string& fontname)
    : area(w), dpy(XtDisplay(w)), win(XtWindow(w)),
      cx(0), cy(0), px(1), py(1), xscale(0.0), yscale(0.0),
      gc(0), font(0), vchar(0), jmode(0), line_type(0), width(0),
      type(LineSolid), pointsize(1), pending_plots(0), last_commands()
{
    plot_resource_values values;
    XtGetApplicationResources(area, &values,
			      plot_subresources, XtNumber(plot_subresources),
			      ArgList(0), 0);

    // Init font
    font = XLoadQueryFont(dpy, fontname.chars());
    if (font == 0)
	font = XLoadQueryFont(dpy, values.font);
    if (font == 0)
	font = XLoadQueryFont(dpy, "fixed");
    if (font == 0)
    {
	std::cerr << "Cannot load font\n";
	exit(1);
    }
    
    vchar = font->ascent + font->descent;

    // Init point size
    pointsize = values.pointsize;
    if (pointsize <= 0 || pointsize > 10) 
    {
	std::cerr << "Invalid point size " << pointsize << "\n";
	pointsize = 1;
    }

    // Init colors
    Pixel black = BlackPixelOfScreen(XtScreen(area));
    Pixel white = WhitePixelOfScreen(XtScreen(area));
    int depth;
    XtVaGetValues(area, XmNdepth, &depth, XtPointer(0));
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
	XtVaGetValues(area, XmNcolormap, &cmap, XtPointer(0));

	for (int i = 0; i < Ncolors; i++)
	{
	    string color = gray ? gray_values[i] : color_values[i];
	    XColor xcolor;
	    if (!XParseColor(dpy, cmap, color.chars(), &xcolor))
	    {
		std::cerr << "Unable to parse " << quote(color) 
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
		    std::cerr << "Cannot allocate " << quote(color)
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

void PlotArea::plot_nop(const char *)
{
    // Ignore command
}

// Unknown command
void PlotArea::plot_unknown(const char *command)
{
    std::cerr << "PlotArea: unknown plot command " << quote(command) << "\n";
}

void PlotArea::plot_vector(const char *buf)
{
    int x, y;
    int assignments = sscanf(buf, "V%4d%4d", &x, &y);
    if (assignments != 2)
    {
	plot_unknown(buf);
	return;
    }

    XDrawLine(dpy, win, gc, X(cx), Y(cy), X(x), Y(y));
    cx = x; cy = y;
}

void PlotArea::plot_move(const char *buf)
{
    int assignments = sscanf(buf, "M%4d%4d", &cx, &cy);
    if (assignments != 2)
    {
	plot_unknown(buf);
	return;
    }
}

void PlotArea::plot_text(const char *buf)
{
    int x, y;
    int assignments = sscanf(buf, "T%4d%4d", &x, &y);  
    if (assignments != 2)
    {
	plot_unknown(buf);
	return;
    }

    const char *str = buf + 9;
    int sl = 0;
    while (str[sl] != '\n' && str[sl] != '\0')
	sl++;

    int sw = XTextWidth(font, str, sl);
    switch (jmode) 
    {
    case 0: sw = 0;     break;	// left
    case 1: sw = -sw/2; break;	// center
    case 2: sw = -sw;   break;	// right
    }

    XSetForeground(dpy, gc, colors[2]);
    XDrawString(dpy, win, gc, X(x)+sw, Y(y) + vchar / 3, str, sl);
    XSetForeground(dpy, gc, colors[line_type + 3]);
}

void PlotArea::plot_justify(const char *buf)
{
    int assignments = sscanf(buf, "J%4d", &jmode);
    if (assignments != 1)
    {
	plot_unknown(buf);
	return;
    }
}

void PlotArea::plot_linetype(const char *buf)
{
    int assignments = sscanf(buf, "L%4d", &line_type);
    if (assignments != 1)
    {
	plot_unknown(buf);
	return;
    }

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

void PlotArea::plot_point(const char *buf)
{
    int point, x, y;
    int assignments = sscanf(buf, "P%1d%4d%4d", &point, &x, &y);  
    if (assignments != 3)
    {
	plot_unknown(buf);
	return;
    }

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

void PlotArea::plot_clear(const char *)
{
    // Clear window
    Dimension area_width, area_height;
    Pixel area_background;
    XtVaGetValues(area, 
		  XmNwidth, &area_width, 
		  XmNheight, &area_height, 
		  XmNbackground, &area_background,
		  XtPointer(0));

    XSetForeground(dpy, gc, area_background);
    XFillRectangle(dpy, win, gc, 0, 0, area_width, area_height);
    XSetBackground(dpy, gc, area_background);
}

// Reset all values
void PlotArea::plot_reset(const char *)
{
    if (!XtIsRealized(area))
    {
	win = 0;
	return;
    }

    win = XtWindow(area);

    // Set scaling factor between internal driver & window geometry
    Dimension area_width, area_height;
    XtVaGetValues(area, 
		  XmNwidth, &area_width, 
		  XmNheight, &area_height, 
		  XtPointer(0));
    xscale = (double)area_width  / 4096.0;
    yscale = (double)area_height / 4096.0;

    // Reset center
    cx = 0;
    cy = 0;

    // Reset line type
    type = LineSolid;

    // Create new GC
    if (gc != 0)
	XFreeGC(dpy, gc);
    gc = XCreateGC(dpy, win, 0, (XGCValues *)0);
    XSetFont(dpy, gc, font->fid);
}

void PlotArea::plot(const char *commands, int length, bool clear)
{
    if (last_commands.length() > 0 && 
	last_commands.data()[last_commands.length() - 1] != '\n')
    {
	// Last command was incomplete - complete it
	const char *s = last_commands.data();
	int line = last_commands.length() - 1;
	while (line > 0 && s[line - 1] != '\n')
	    line--;
	assert(line == 0 || s[line - 1] == '\n');

	const char *tail = commands;
	while (length > 0 && *commands != '\n')
	    commands++, length--;

	if (length > 0 && *commands == '\n')
	{
	    commands++, length--;
	    string command = string(s + line) + string(tail, commands - tail);

	    assert(isalpha(command[0]));
	    assert(command.contains('\n', -1));

	    do_plot(command.chars(), clear);

	    last_commands.append(tail, commands - tail);
	}
    }

    int discard = do_plot(commands, clear);

    if (discard >= 0)
    {
	// `G' command found - forget about old commands
	last_commands.discard();
	last_commands.append(commands + discard, length - discard);
    }
    else
    {
	// No `G' command found - append to previous commands
	last_commands.append(commands, length);
    }

    assert(last_commands.length() == 0 || last_commands.data()[0] == 'G');
}

void PlotArea::replot(bool clear)
{
    plot_reset("");

    do_plot(last_commands.data(), clear);
}

int PlotArea::do_plot(const char *commands, bool clear)
{
    // Discard all commands up to `G' command, if any
    int discard = -1;

    {
      const char *cmds = commands;
      while (*cmds != '\0')
	{
	  if (cmds[0] == 'G' && cmds[1] == '\n')
	    {
	      if (pending_plots > 0)
		pending_plots--;
	      
	      discard = (cmds - commands);
	    }
	  
	  while (*cmds != '\n' && *cmds != '\0')
	    cmds++;
	  if (*cmds != '\0')
	    cmds++;
	}
    }

    // Process commands
    const char *cmds = commands;
    if (discard >= 0)
    {
	cmds += discard;
	assert(cmds[0] == 'G');
	assert(cmds[1] == '\n');
    }

#if 0				// FIXME: Not thoroughly tested yet  -AZ
    if (discard < 0 && pending_plots > 0)
	return discard;
#endif

    while (cmds[0] != '\0')
    {
	const char *command_begin = cmds;

	// Move CMDS to the next line
	while (*cmds != '\0' && *cmds != '\n')
	    cmds++;
	if (*cmds == '\0')
	    break;		// Command is incomplete - don't do it
	assert(*cmds == '\n');
	cmds++;

	// Copy current command to a NULL-terminated string. Otherwise, 
        // sscanf() takes far too much time.
	const int len_ = cmds-command_begin-1;
	assert(len_ >= 0);
	const string command_s(command_begin,len_);

	const char *command = command_s.chars();
	switch (command[0])
	{
	case 'V':
	    if (win)
		plot_vector(command);
	    break;

	case 'M':
	    if (win)
		plot_move(command);
	    break;

	case 'T':
	    if (win)
		plot_text(command);
	    break;

	case 'J':
	    if (win)
		plot_justify(command);
	    break;

	case 'L':
	    if (win)
		plot_linetype(command);
	    break;

	case 'P':
	    if (win)
		plot_point(command);
	    break;

	case 'G':
	    plot_reset(command);
	    if (win && clear)
		plot_clear(command);
	    break;

	case 'E':
	case 'R':
	    if (win)
		plot_nop(command);
	    break;

	default:
	    plot_unknown(command);
	    break;
	}
    }

    return discard;
}
