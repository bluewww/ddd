// $Id$ -*- C++ -*-
// An area to plot upon

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_PlotArea_h
#define _DDD_PlotArea_h

#include "strclass.h"
#include <X11/Intrinsic.h>
#include "ChunkQueue.h"
#include "assert.h"

class PlotArea;

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

    int pending_plots;		// Number of plots pending

    ChunkQueue last_commands;	// Command buffer
    
    // Plotting commands
    void plot_unknown (const char *command);
    void plot_nop     (const char *command);
    void plot_vector  (const char *command);
    void plot_move    (const char *command);
    void plot_text    (const char *command);
    void plot_justify (const char *command);
    void plot_linetype(const char *command);
    void plot_point   (const char *command);
    void plot_clear   (const char *command);
    void plot_reset   (const char *command);

    // Don't use these
    PlotArea(const PlotArea&);

    PlotArea& operator=(const PlotArea&);

protected:
    virtual int do_plot(const char *commands, bool clear);

public:
    // Constructor
    PlotArea(Widget w, const string& fontname);

    // Destructor
    virtual ~PlotArea() {}

    // Plot
    void plot(const char *commands, int length, bool clear = true);
    void replot(bool clear = false);

    // Set if plot is pending
    void plot_pending() { pending_plots++; }

    Widget widget() const { return area; }
};

#endif // _DDD_PlotArea_h
// DON'T ADD ANYTHING BEHIND THIS #endif
