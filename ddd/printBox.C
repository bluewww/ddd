// $Id$  -*- C++ -*-
// utilities for printing boxes

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Christian Lindig (lindig@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

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
// orgin of the graph is upper left!
//

void Box::epsHeader (ostream& os, 
		     const BoxRegion& region, 
		     const BoxPostScriptGC& gc)
{
    int hmove, vmove ;
    float hscale, vscale;
    float scale = 1.0 ;
    BoxPoint llcorner;
    BoxPoint urcorner;
    
    urcorner = region.space() ;
    
    // check size of graph

    if (region.space() > BoxPoint (gc.hsize,gc.vsize)) {
	// scale down ...
	hscale = (float)gc.hsize / region.space(X) ;
	vscale = (float)gc.vsize / region.space(Y) ;
	scale = ( hscale < vscale ? hscale : vscale );
	
	urcorner[X] = (int)((float)urcorner[X] * scale);
	urcorner[Y] = (int)((float)urcorner[Y] * scale);
    }

    llcorner[X] = gc.hoffset ;
    llcorner[Y] = gc.voffset ;

    hmove = gc.hoffset - region.origin(X);
    vmove = gc.voffset + urcorner[Y] + region.origin(Y) ;
    urcorner += llcorner ;

    os << EPSHEAD << CREATOR ;
    os << BOUND ;
    os << llcorner[X] << " " << llcorner[Y] << " ";
    os << urcorner[X] << " " << urcorner[Y] << "\n" ;
    os << PAGES << ENDC ;

    // write os transformation 
    
    os << "\ngsave\n" << hmove << " " << vmove << " translate\n";
    os << scale << " " << -scale << " scale\n" ;
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
