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
#include "IntArray.h"
#include "PrintGC.h"

#include <fstream.h>

// Event types
const unsigned Plot = LiterateAgent_NTypes;   // Plot data received

const unsigned PlotAgent_NTypes = Plot + 1;   // number of events

class PlotAgent: public LiterateAgent {

public:
    DECLARE_TYPE_INFO

private:
    StringArray files;		// Temporary files allocated by this Agent
    StringArray titles;		// Titles currently plotted
    StringArray values;		// Scalars
    IntArray dims;		// Dimensions of scalars
    ofstream plot_os;		// Stream used for adding data
    int ndim;			// Number of dimensions used so far
    double x_min, x_max;	// Minimum and maximum values
    double y_min, y_max;
    double v_min, v_max;
    string plot_commands;	// Plotting commands received so far
    string init_commands;	// Initialization commands
    bool need_reset;		// Reset with next plot
    int last_ndim;		// Last number of dimensions

protected:
    void add_v(double v);
    void add_x(double x);
    void add_y(double y);

    void reset();
    string var(char *name, double min, double max);

    virtual void dispatch(int type, char *data, int length);

public:
    static string plot_2d_settings;
    static string plot_3d_settings;

    // Constructor for Agent users
    PlotAgent(XtAppContext app_context, const string& pth,
	      unsigned nTypes = PlotAgent_NTypes)
	: LiterateAgent(app_context, pth, nTypes),
	  files(), titles(), plot_os(), ndim(0), 
	  x_min(0.0), x_max(0.0),
	  y_min(0.0), y_max(0.0),
	  v_min(0.0), v_max(0.0),
	  plot_commands(""),
	  init_commands(""),
	  need_reset(false),
	  last_ndim(0)
    {
	reset();
    }

    // Start and initialize
    void start(const string& init);

    // Kill
    void abort();

    // Start plotting new data with TITLE in NDIM dimensions
    void start_plot(const string& title, int ndim);

    // Add plot point
    void add_point(const string& v, int dim);
    void add_point(int x, const string& v);
    void add_point(int x, int y, const string& v);

    // Add a break
    void add_break();

    // End plot
    void end_plot();

    // Flush accumulated data
    int flush();

    // Return number of dimensions
    int dimensions() const { return ndim; }

    // Return max and min values
    double min_x() const { return x_min; }
    double max_x() const { return x_max; }
    double min_y() const { return y_min; }
    double max_y() const { return y_max; }
    double min_v() const { return v_min; }
    double max_v() const { return v_max; }

    // Get data titles and files
    const StringArray& data_titles() const { return titles; }
    const StringArray& data_files()  const { return files;  }

    // Print plot to FILENAME
    void print(const string& filename, 
	       const PrintGC& gc = PostScriptPrintGC());
};

#endif // _DDD_PlotAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
