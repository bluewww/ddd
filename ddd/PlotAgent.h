// $Id$ -*- C++ -*-
// Gnuplot interface

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

#ifndef _DDD_PlotAgent_h
#define _DDD_PlotAgent_h

#include "LiterateA.h"
#include "assert.h"
#include "StringA.h"
#include "IntArray.h"
#include "PrintGC.h"
#include "ChunkQueue.h"

#include <fstream>

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
    std::ofstream plot_os;		// Stream used for adding data
    int ndim;			// Number of dimensions used so far

    double x_min, x_max;	// Minimum and maximum values
    double y_min, y_max;
    double v_min, v_max;

    string init_commands;	// Initialization commands
    bool need_reset;		// Reset with next plot
    int last_ndim;		// Last number of dimensions

    bool getting_plot_data;	// True if getting plot data

protected:
    void add_v(double v);
    void add_x(double x);
    void add_y(double y);

    void reset();
    string var(const char *name, double min, double max) const;

    virtual void dispatch(int type, const char *data, int length);

public:
    static string plot_2d_settings;
    static string plot_3d_settings;

    // Constructor for Agent users
    PlotAgent(XtAppContext app_context, const string& pth,
	      unsigned nTypes = PlotAgent_NTypes)
	: LiterateAgent(app_context, pth, nTypes),
	  files(), titles(), values(), dims(),
	  plot_os(), ndim(0), 
	  x_min(0.0), x_max(0.0),
	  y_min(0.0), y_max(0.0),
	  v_min(0.0), v_max(0.0),
	  init_commands(""),
	  need_reset(false),
	  last_ndim(0),
	  getting_plot_data(false)
    {
	reset();
    }

    // Start and initialize
    void start_with(const string& init);

    // Kill
    void abort();

    // Start plotting new data with TITLE in NDIM dimensions
    void start_plot(const string& title, int ndim);

    // Add plot point
    void add_point(const string& v, int dim);
    void add_point(int x, const string& v);
    void add_point(double x, const string& v);
    void add_point(int x, int y, const string& v);
    void add_point(double x, double y, const string& v);

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

    // Show plot state
    void set_state(const string& state);
};

#endif // _DDD_PlotAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
