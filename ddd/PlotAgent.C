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

char PlotAgent_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PlotAgent.h"
#include "cook.h"

DEFINE_TYPE_INFO_1(PlotAgent, LiterateAgent)

string PlotAgent::plot_2d_settings = "";
string PlotAgent::plot_3d_settings = "";

// Start and initialize
void PlotAgent::start(const string& init)
{
    LiterateAgent::start();
    write(init.chars(), init.length());
}

// Start a new plot
void PlotAgent::start_plot(const string& title, int n)
{
    titles += title;
    values += "";
    ndim = n;

    while (files.size() < titles.size())
    {
	// Open a new temporary file
	files += tmpnam(0);
    }

    // Open plot stream
    plot_os.open(files[titles.size() - 1]);
}

// End a new plot
void PlotAgent::end_plot()
{
    // Close plot stream
    plot_os.close();
}

// Flush it all
int PlotAgent::flush()
{
    if (ndim == 0 || titles.size() == 0)
    {
	// No data - ignore
    }
    else
    {
	string cmd;
	switch (ndim)
	{
	case 0:
	    break;

	case 1:
	case 2:
	    cmd = "set noparametric\nplot ";
	    if (plot_2d_settings != "")
		cmd.prepend(plot_2d_settings + "\n");
	    break;

	case 3:
	    cmd = "set parametric\nsplot ";
	    if (plot_3d_settings != "")
		cmd.prepend(plot_3d_settings + "\n");
	    break;
	}

	for (int i = 0; i < titles.size(); i++)
	{
	    if (i > 0)
		cmd += ", ";

	    const string& v = values[i];
	    if (v != "")
	    {
		// Plot atomic value
		if (ndim == 3)
		    cmd += "0,0," + v;
		else
		    cmd += v;
	    }
	    else
	    {
		// Plot a file
		cmd += quote(files[i]);
	    }
	    cmd += " title " + quote(titles[i]);

	    if (v != "" && ndim == 3)
		cmd += " with points";
	}
	cmd += "\n";

	write(cmd.chars(), cmd.length());
    }

    // Clear titles
    static StringArray empty;
    titles = empty;

    return LiterateAgent::flush();
}

// Done
void PlotAgent::abort()
{
    // Unlink temporary files
    for (int i = 0; i < files.size(); i++)
	unlink(files[i]);

    // We're done
    LiterateAgent::abort();
}


// Add plot point
void PlotAgent::add_point(const string& v)
{
    if (ndim > 1)
	add_point(0, v);
    else
	values[values.size() - 1] = v;
}

void PlotAgent::add_point(int x, const string& v)
{
    if (ndim > 2)
	add_point(x, 0, v);
    else
	plot_os << x << '\t' << v << '\n';
}

void PlotAgent::add_point(int x, int y, const string& v)
{
    assert(ndim == 3);

    plot_os << x << '\t' << y << '\t' << v << '\n';
}

void PlotAgent::add_break()
{
    plot_os << '\n';
}
