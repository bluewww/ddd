// $Id$
// Klasse Box (Deklaration)

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _Nora_Box_h
#define _Nora_Box_h

#ifdef __GNUG__
#pragma interface
#endif


// A Box is the base class for all graphical objects. It essentially
// consists of a rectangle with a certain size and a certain
// extensibility. The contents of the box are determined by the
// drawing function _draw().

#include "assert.h"
#include <string.h>
#include "strclass.h"

#include "BoxPoint.h"
#include "BoxSize.h"
#include "BoxD.h"
#include "BoxC.h"
#include "BoxWeight.h"
#include "BoxRegion.h"
#include "BoxExtend.h"
#include "Widget.h"
#include "bool.h"
#include "UniqueId.h"
#include "DataLink.h"
#include "TypeInfo.h"

class TagBox;

// Printing environment
struct BoxPrintGC {
    DECLARE_TYPE_INFO

    virtual bool isFig() const        { return false; }
    virtual bool isPostScript() const { return false; }
};

struct BoxFigGC: public BoxPrintGC {
    DECLARE_TYPE_INFO

    virtual bool isFig() const        { return true; }

    BoxFigGC() {}
};

struct BoxPostScriptGC: public BoxPrintGC {
    DECLARE_TYPE_INFO

    virtual bool isPostScript() const { return true; }

    int hsize;   // Maximum size of graph in 1/72"
    int vsize;

    int hoffset; // lower left corner of graph on page in 1/72"
    int voffset;

    // Default: let params fit to an A4-sized page
    BoxPostScriptGC():
        hsize(510), vsize(794), hoffset(42), voffset(35) 
    {}
};



// Box class
class Box {
public:
    DECLARE_TYPE_INFO

    friend class BoxMemInit;

private:
    UniqueId _id;		// Identifier
    BoxSize _size;		// Size
    BoxExtend _extend;		// Extensibility
    char *_type;		// type

    static void epsHeader (ostream& os, 
			   const BoxRegion& region, 
			   const BoxPostScriptGC& gc);

protected:
    int _links;			// #references (>= 1)

    BoxSize& thesize()     { return _size; }
    BoxExtend& theextend() { return _extend; }
    const BoxSize& thesize() const     { return _size; }
    const BoxExtend& theextend() const { return _extend; }

    // Copy constructor
    Box(const Box& box):
	_size(box._size), _extend(box._extend), _type(box._type), _links(1)
    {}
	
    // Equality
    virtual bool matches(const Box& b, const Box *callbackArg = 0) const;

    // Draw box
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, 
		       GC gc, bool context_selected) const = 0;

public:
    // Save box to stream
    virtual void dump(ostream& s) const = 0;

    // Constructor
    Box(BoxSize s = BoxSize(0, 0), BoxExtend e = BoxExtend(0, 0), 
	char *t = "Box"):
	_size(s), _extend(e), _type(t), _links(1)
    {}

    // Destructor
    virtual ~Box()
    {
	assert (_links == 0);
	_type = 0;
    }

    // Create new reference
    virtual Box* link()
    {
	assert(_links > 0);
	_links++;
	return this;
    }

    // Copy box
    virtual Box *dup() const = 0;

    // Kill reference
    void unlink()
    {
	assert(_links > 0);
	if (--_links == 0)
	    delete this;
    }

    // Resources
    unsigned long id() const                   { return (unsigned long)_id; }
    const char *type() const                   { return _type; }
    BoxExtend extend() const                   { return _extend; }
    BoxSize size() const                       { return _size; }

    BoxWeight extend(BoxDimension dimension) const
    { 
	return _extend[dimension]; 
    }
    BoxCoordinate size(BoxDimension dimension) const 
    { 
	return _size[dimension]; 
    }

    // Build string from string components
    virtual string str() const { return ""; }

    // Return free space in lower right corner
    virtual BoxSize corner() const
    {
	// Default: Platz der Breite 0 rechts von Box
	return BoxSize(0, _size[Y]);
    }

    // Re-calculate box size
    virtual Box* resize() { return this; }

    // Propagate new font
    virtual void newFont(const string& font) { resize(); }

    // Draw
    void draw(Widget w, 
	      const BoxRegion& region, 
	      const BoxRegion& exposed = BoxRegion(BoxPoint(0,0),
						   BoxSize(MAXINT, MAXINT)), 
	      GC gc = 0, 
	      bool context_selected = false) const;


    // Print box; Header/trailer must be pre-/postfixed
    virtual void _print(ostream& os, 
			const BoxRegion& region, 
			const BoxPrintGC& gc) const = 0;

    // Print Header/trailer
    static void _printHeader(ostream& os, 
			     const BoxRegion& region, 
			     const BoxPrintGC& gc);
    static void _printTrailer(ostream& os, 
			      const BoxRegion& region, 
			      const BoxPrintGC& gc);

    // Custom function: print box with header and trailer
    void print(ostream& os = cout,
	       BoxRegion region = 
	           BoxRegion(BoxPoint(0,0), BoxSize(0,0)),
	       const BoxPrintGC& gc = BoxPostScriptGC()) const
    {
	region.space(X) = max(region.space(X), size(X));
	region.space(Y) = max(region.space(Y), size(Y));

	_printHeader(os, region, gc);
	_print(os, region, gc);
	_printTrailer(os, region, gc);
    }

    // Check for equality
    bool operator == (const Box& b) const;
    bool operator != (const Box& b) const { return !(operator== (b)); }

    // Check class
    virtual bool isStringBox() const  { return false; }
    virtual bool isListBox() const    { return false; }
    virtual bool isDummyBox() const   { return false; }

    // Tag a node
    // Usage: box = box->tag(...)
    virtual Box *tag(Data *data, DataLink *dl = 0);

    // Return tag attributes for a point (No point: outermost)
    BoxRegion region(BoxPoint p = BoxPoint(-1,-1)) const;   // current region
    Data *data(BoxPoint p = BoxPoint(-1,-1)) const;         // current data
    string name(BoxPoint p = BoxPoint(-1,-1)) const;        // current name
    bool selected(BoxPoint p = BoxPoint(-1,-1)) const;   // Flag: selected?
    string info(BoxPoint p = BoxPoint(-1,-1)) const;        // Debugging info

    // return the box that is to be matched
    virtual const Box& matchMe() const { return *this; }

    // Search functions

    // Count MatchBoxes
    virtual void countMatchBoxes(int instances[]) const {}

    // Return TagBox for a point (No Punkt: outermost)
    virtual const TagBox *findTag(BoxPoint p = BoxPoint(-1, -1)) const 
    { 
	return 0;
    }

    // Debugging
    // Send box to stream
    friend ostream& operator << (ostream& s, const Box& b);

    // Invariant check
    virtual bool OK() const
    {
	assert (_links > 0);
	assert (_type != 0);
	return true;
    }
};

#endif // _Nora_Box_h
// DON'T ADD ANYTHING BEHIND THIS #endif
