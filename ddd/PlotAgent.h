// $Id$ -*- C++ -*-
// Gnuplot interface

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

#ifndef _DDD_PlotAgent_h
#define _DDD_PlotAgent_h

#ifdef __GNUG__
#pragma interface
#endif

#include "LiterateA.h"
#include "assert.h"
#include "StringA.h"

#include <fstream.h>

class PlotAgent: public LiterateAgent {

    enum PlotMode { NoPlotMode = 0, TwoD = 2, ThreeD = 3 };

public:
    DECLARE_TYPE_INFO

private:
    StringArray files;		// Temporary files allocated by this Agent
    StringArray titles;		// Titles currently plotted
    ofstream plot_os;		// File stream used for adding data
    PlotMode mode;

public:
    static string plot_2d_settings;
    static string plot_3d_settings;

    // Constructor for Agent users
    PlotAgent(XtAppContext app_context, const string& pth,
	      unsigned nTypes = LiterateAgent_NTypes)
	: LiterateAgent(app_context, pth, nTypes),
	  files(), titles(), plot_os(), mode(NoPlotMode)
    {}

    // Start and initialize
    void start(const string& init);

    // Kill
    void abort();

    // Start plotting new data
    void start_plot(const string& title);

    // Add plot point
    void add_point(int x, const string& y)
    {
	assert(mode == NoPlotMode || mode == TwoD);
	plot_os << x << '\t' << y << '\n';
	mode = TwoD;
    }

    void add_point(int x, int y, const string& z)
    {
	assert(mode == NoPlotMode || mode == ThreeD);
	plot_os << x << '\t' << y << '\t' << z << '\n';
	mode = ThreeD;
    }

    // End plot
    void end_plot();

    // Flush accumulated data
    int flush();
};

#endif // _DDD_PlotAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
