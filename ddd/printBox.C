// $Id$  -*- C++ -*-
// utilities for printing boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Christian Lindig <lindig@ips.cs.tu-bs.de>.
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

char printBox_rcsid[] = 
    "$Id$";


#include "Box.h"
#include "printBox.h"


// some constants...
static const char EPSHEAD[] = "%!PS-Adobe-2.0 EPSF-2.0\n";
static const char CREATOR[] = "%%Creator: Box::print() by Christian Lindig\n";
static const char PAGES[] =   "%%Pages: 1\n";
static const char ENDC[] =    "%%EndComments\n";
static const char BOUND[] =   "%%BoundingBox: ";


//
// include the big strings for prolog and trailer
//

#include "eps-code.h"


//
// epsHeader
// write out the header of the eps-file, calculate the bounding box
// and make the graph fit to a DIN-A4 paper.
//
// Remember: the origin of the PS-system is lower left, the internal
// origin of the graph is upper left!
//

void Box::epsHeader (ostream& os, 
		     const BoxRegion& region, 
		     const BoxPostScriptGC& gc)
{
    // check size of graph

    BoxPoint space  = region.space();
    BoxPoint origin = region.origin();

    BoxPoint size;
    switch (gc.orientation)
    {
    case BoxPostScriptGC::PORTRAIT:
	size = BoxPoint(gc.hsize, gc.vsize);
	break;

    case BoxPostScriptGC::LANDSCAPE:
	size = BoxPoint(gc.vsize, gc.hsize);
	break;
    }

    double scale = 1.0;
    if (space > size)
    {
	// Scale down ...
	double hscale = double(size[X]) / region.space(X);
	double vscale = double(size[Y]) / region.space(Y);
	scale = (hscale < vscale ? hscale : vscale);
	
	space[X] = int(double(space[X]) * scale + 0.5);
	space[Y] = int(double(space[Y]) * scale + 0.5);

	origin[X] = int(double(origin[X]) * scale + 0.5);
	origin[Y] = int(double(origin[Y]) * scale + 0.5);
    }

    // Determine bounding box
    BoxPoint llcorner, urcorner;
    switch (gc.orientation)
    {
    case BoxPostScriptGC::PORTRAIT:
	llcorner = BoxPoint(gc.hoffset,
			    gc.voffset);
	urcorner = BoxPoint(gc.hoffset + space[X], 
			    gc.voffset + space[Y]);
	break;

    case BoxPostScriptGC::LANDSCAPE:
        llcorner = BoxPoint(gc.hsize - space[Y] + gc.hoffset - gc.voffset, 
			    gc.hoffset);
	urcorner = BoxPoint(gc.hsize + gc.hoffset - gc.voffset,
			    gc.hoffset + space[X]);
	break;
    }

    os << EPSHEAD
       << CREATOR
       << BOUND 
       << llcorner[X] << " " << llcorner[Y] << " "
       << urcorner[X] << " " << urcorner[Y] << "\n"
       << PAGES << ENDC
       << "\ngsave\n";

    // Write rotation
    if (gc.orientation == BoxPostScriptGC::LANDSCAPE)
	os << gc.hsize + gc.hoffset << " 0 translate 90 rotate\n";

    // Write scaling
    int hmove = gc.hoffset - origin[X];
    int vmove = gc.voffset + space[Y] + origin[Y];

    os << hmove << " " << vmove << " translate\n"
       << scale << " " << -scale << " scale\n";
}


// public interface

void Box::_printHeader(ostream& os, 
		       const BoxRegion& region, 
		       const BoxPrintGC& gc)
{
    if (gc.isPostScript())
    {
	epsHeader(os, region, (BoxPostScriptGC &)gc);
	os << prolog;
    }
    else if (gc.isFig())
    {
	os << FIGHEAD;
    }
}

void Box::_printTrailer(ostream& os, const BoxRegion&, const BoxPrintGC& gc)
{
    if (gc.isPostScript())
    {
	os << trailer;
    }
}
