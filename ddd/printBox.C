// $Id$  -*- C++ -*-
// utilities for printing boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Christian Lindig (lindig@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char printBox_rcsid[] = 
    "$Id$";


#include "Box.h"
#include "printBox.h"


// some constants...
const char EPSHEAD[] = "%!PS-Adobe-2.0 EPSF-2.0\n";
const char CREATOR[] = "%%Creator: Box::print() by Christian Lindig\n";
const char PAGES[] =   "%%Pages: 1\n";
const char ENDC[] =    "%%EndComments\n";
const char BOUND[] =   "%%BoundingBox: ";


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
    float scale = 1.0;
    
    // check size of graph

    BoxPoint space(region.space());
    BoxPoint origin(region.origin());

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

    if (space > size)
    {
	// Scale down ...
	double hscale = float(size[X]) / region.space(X);
	double vscale = float(size[Y]) / region.space(Y);
	scale = (hscale < vscale ? hscale : vscale);
	
	space[X] = int(double(space[X]) * scale);
	space[Y] = int(double(space[Y]) * scale);

	origin[X] = int(double(origin[X]) * scale);
	origin[Y] = int(double(origin[Y]) * scale);
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
