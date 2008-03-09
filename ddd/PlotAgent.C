// $Id$ -*- C++ -*-
// Gnuplot interface

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Universitaet Passau, Germany.
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

char PlotAgent_rcsid[] = 
    "$Id$";

#include "PlotAgent.h"
#include "cook.h"
#include "version.h"
#include "tempfile.h"

#include <float.h>
#include <stdlib.h>		// atof()

DEFINE_TYPE_INFO_1(PlotAgent, LiterateAgent)

string PlotAgent::plot_2d_settings = "";
string PlotAgent::plot_3d_settings = "";

// Start and initialize
void PlotAgent::start_with(const string& init)
{
    LiterateAgent::start();
    write(init.chars(), init.length());
    init_commands = init;
}

// Reset for next plot
void PlotAgent::reset()
{
    // Clear storage
    titles = StringArray();
    values = StringArray();
    dims   = IntArray();
    ndim   = 0;

    // Clear range
    x_min = y_min = v_min = +DBL_MAX;
    x_max = y_max = v_max = -DBL_MAX;

    need_reset = false;
}

// Start a new plot
void PlotAgent::start_plot(const string& title, int n)
{
    if (need_reset)
	reset();

    titles += title;
    values += "";
    dims   += 0;

    ndim = n;

    while (files.size() < titles.size())
    {
	// Open a new temporary file
	files += tempfile();
    }

    // Open plot stream
    plot_os.open(files[titles.size() - 1].chars());

    // Issue initial line
    plot_os << "# " DDD_NAME ": " << title << "\n"
	    << "# Use `set parametric' and `"
	    << (ndim <= 2 ? "plot" : "splot")
	    << "' to plot this data.\n"
	    << "# " 
	    << (ndim <= 2 ? "X\tVALUE" : "X\tY\tVALUE")
	    << "\n";
}

// End a new plot
void PlotAgent::end_plot()
{
    // Close plot stream
    plot_os.close();
}

string PlotAgent::var(const char *name, double min, double max) const
{
    std::ostringstream os;

    if (min < +DBL_MAX && max > -DBL_MAX)
    {
	if (min != 0.0)
	    os << min << " + ";
	os << name << " * " << (max - min);
    }
    else
    {
	// Stick to default range
	os << name;
    }

    return string(os);
}

// Flush it all
int PlotAgent::flush()
{
    if (ndim == 0 || titles.size() == 0)
    {
	// No data - ignore
	return -1;
    }
    else
    {
	// Issue plot command
	std::ostringstream cmd;
	switch (ndim)
	{
	case 0:
	    break;

	case 1:
	case 2:
	    if (ndim != last_ndim && !plot_2d_settings.empty())
	    {
		cmd << plot_2d_settings << "\n";
		last_ndim = ndim;
	    }
	    cmd << "plot ";
	    break;

	case 3:
	    if (ndim != last_ndim && !plot_3d_settings.empty())
	    {
		cmd << plot_3d_settings << "\n";
		last_ndim = ndim;
	    }
	    cmd << "splot ";
	    break;
	}

	// Issue functions
	for (int i = 0; i < titles.size(); i++)
	{
	    if (i > 0)
		cmd << ", ";

	    const string& v = values[i];
	    int dim = dims[i];
	    if (!v.empty())
	    {
		// Plot scalar value
		if (ndim == 3)
		{
		    switch (dim)
		    {
		    case 0:	// u, v, VALUE - a horizontal plain
			cmd << var("u", x_min, x_max) << ", "
			    << var("v", y_min, y_max) << ", " 
			    << v;
			break;

		    case 1:	// u, VALUE, v - a vertical plain
			cmd << var("u", x_min, x_max) << ", "
			    << v << ", "
			    << var("v", v_min, v_max);
			break;

 		    case 2:	// VALUE, u, v - a vertical plain
			cmd << v << ", "
			    << var("u", y_min, y_max) << ", "
			    << var("v", v_min, v_max);
			break;
		    }
		}
		else
		{
		    switch (dim)
		    {
		    case 0:	// t, VALUE - a horizontal line
			cmd << var("t", x_min, x_max) << ", "
			    << v;
			break;

		    case 1:	// VALUE, t - a vertical line
		    case 2:
			cmd << v << ", "
			    << var("t", v_min, v_max);
			break;
		    }
		}
	    }
	    else
	    {
		// Plot a file
		cmd << quote(files[i]);
	    }

	    // Add title
	    cmd << " title " << quote(titles[i]);
	}
	cmd << "\n";

	// That's all, folks!
	string c(cmd);
	write(c.chars(), c.length());
    }

    need_reset = true;

    return LiterateAgent::flush();
}

// Done
void PlotAgent::abort()
{
    // Unlink temporary files
    for (int i = 0; i < files.size(); i++)
	unlink(files[i].chars());

    // We're done
    LiterateAgent::abort();
}

inline double min(double a, double b)
{
    return a < b ? a : b;
}

inline double max(double a, double b)
{
    return a > b ? a : b;
}

// Check value
void PlotAgent::add_v(double v)
{
    v_min = min(v_min, v);
    v_max = max(v_max, v);
}

void PlotAgent::add_x(double x)
{
    x_min = min(x_min, x);
    x_max = max(x_max, x);
}

void PlotAgent::add_y(double y)
{
    y_min = min(y_min, y);
    y_max = max(y_max, y);
}

// Add plot point
void PlotAgent::add_point(const string& v, int dim)
{
    values[values.size() - 1] = v;
    dims[dims.size() - 1] = dim;

    add_v(atof(v.chars()));
}

void PlotAgent::add_point(int x, const string& v)
{
    if (ndim > 2)
	add_point(x, 0, v);
    else
    {
	plot_os << x << '\t' << v << '\n';
	add_x(x);
	add_v(atof(v.chars()));
    }
}

void PlotAgent::add_point(double x, const string& v)
{
    if (ndim > 2)
	add_point(x, 0.0, v);
    else
    {
	plot_os << x << '\t' << v << '\n';
	add_x(x);
	add_v(atof(v.chars()));
    }
}

void PlotAgent::add_point(int x, int y, const string& v)
{
    assert(ndim == 3);

    plot_os << x << '\t' << y << '\t' << v << '\n';
    add_x(x);
    add_y(y);
    add_v(atof(v.chars()));
}

void PlotAgent::add_point(double x, double y, const string& v)
{
    assert(ndim == 3);

    plot_os << x << '\t' << y << '\t' << v << '\n';
    add_x(x);
    add_y(y);
    add_v(atof(v.chars()));
}

void PlotAgent::add_break()
{
    plot_os << '\n';
}


// Handle plot commands
void PlotAgent::dispatch(int type, const char *data, int length)
{
    if (type != int(Input) || length < 2)
    {
	LiterateAgent::dispatch(type, data, length);
	return;
    }

    if (data[0] == 'G' && data[1] == '\n')
    {
	// Enter graphics mode
	getting_plot_data = true;
    }

    if (!getting_plot_data)
    {
	LiterateAgent::dispatch(type, data, length);
	return;
    }

    // Call handlers
    DataLength dl(data, length);
    callHandlers(Plot, &dl);

    if (length >= 2 && data[length - 1] == '\n')
    {
	char last_cmd = data[length - 2];

	if (last_cmd == 'E' || last_cmd == 'R')
	{
	    // Leave graphics mode
	    getting_plot_data = false;
	}
    }
}

// Show whether plot is active
void PlotAgent::set_state(const string& state)
{
    string title;
    if (!state.empty())
	title = " " + quote('(' + state + ')');

    const string c = "set title" + title + "\nreplot\n";
    write(c.chars(), c.length());
}

// Print plot to FILENAME
void PlotAgent::print(const string& filename, const PrintGC& gc)
{
    std::ostringstream cmd;

    if (gc.isFig())
    {
	cmd << "set term fig\n";
    }
    else if (gc.isPostScript())
    {
	const PostScriptPrintGC& ps = const_ref_cast(PostScriptPrintGC, gc);

	cmd << "set term postscript";

	switch (ps.orientation)
	{
	case PostScriptPrintGC::PORTRAIT:
	{
	    // Portrait plotting doesn't make too much sense, so we
	    // use the more useful EPS plotting instead.
	    cmd << " eps";
	    break;
	}

	case PostScriptPrintGC::LANDSCAPE:
	{
	    cmd << " landscape";
	    break;
	}
	}

	if (ps.color)
	{
	    cmd << " color";
	}
	else
	{
	    cmd << " monochrome";
	}

	cmd << "\n";


	switch (ps.orientation)
	{
	case PostScriptPrintGC::PORTRAIT:
	{
	    // Use the default size for EPS.
	    cmd << "set size\n";
	    break;
	}
	case PostScriptPrintGC::LANDSCAPE:
	{
	    // Postscript defaults are: landscape 10" wide and 7" high.
	    // Our `vsize' and `hsize' members assume portrait, so they
	    // are just reversed.
	    const int default_hsize =  7 * 72; // Default width in 1/72"
	    const int default_vsize = 10 * 72; // Default height in 1/72"

	    // Leave 1" extra vertical space
	    const int hoffset = 0;
	    const int voffset = 1 * 72;

	    // Set size
	    double xscale = double(ps.hsize - hoffset) / default_hsize;
	    double yscale = double(ps.vsize - voffset) / default_vsize;

	    cmd << "set size " << xscale << ", " << yscale << "\n";
	}
	}
    }

    cmd << "set output " << quote(filename) << "\n"
	<< "replot\n"
	<< "set output\n"
	<< "set size\n"
	<< init_commands << "\n"
	<< "replot\n";

    string c(cmd);
    write(c.chars(), c.length());
}
