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

#ifndef _DDD_PlotArea_h
#define _DDD_PlotArea_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include <X11/Intrinsic.h>

class PlotArea;

typedef void (PlotArea::*PlotAreaFunc)(const string& command);

struct PlotCommand {
    char code;
    PlotAreaFunc func;
};

#define Ncolors 13
#define Ndashes 10
#define Nwidths 10

class PlotArea {
    Widget area;		// Widget to draw upon
    Display *dpy;		// ... its display
    Window win;			// ... its window

    int cx, cy;			// Current position
    int px, py;			// Current point size
    double xscale, yscale;	// Current scaling
    GC gc;			// Current graphics context
    XFontStruct *font;		// Font to use
    int vchar;			// Its height
    Pixel colors[Ncolors];	// Colors to use
    char dashes[Ndashes][5];	// Dashes to use
    unsigned int widths[Nwidths]; // Widths to use
    int jmode;			// Justification
    int line_type;		// Gnuplot Line type
    int width;			// Current line width
    int type;			// Current line type
    int pointsize;		// Point size
    string commands;		// Commands drawn

    static PlotCommand command_table[]; // Commands to use

    // Plotting commands
    void plot_nop(const string& command);
    void plot_vector(const string& command);
    void plot_move(const string& command);
    void plot_text(const string& command);
    void plot_justify(const string& command);
    void plot_linetype(const string& command);
    void plot_point(const string& command);

public:
    // Constructor
    PlotArea(Widget w, const string& fontname);

    // Plot
    void plot(const string& commands, bool clear = true);
    void replot(bool clear = false);

    Widget widget() const { return area; }
};

#endif // _DDD_PlotArea_h
// DON'T ADD ANYTHING BEHIND THIS #endif
