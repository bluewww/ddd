// $Id$
// Box class

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_Box_h
#define _DDD_Box_h

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
#include "PrintGC.h"
#include "Widget.h"
#include "bool.h"
#include "UniqueId.h"
#include "DataLink.h"
#include "TypeInfo.h"

class TagBox;

// Box class
class Box {
public:
    DECLARE_TYPE_INFO

    friend class BoxMemInit;

private:
    UniqueId _id;		// Identifier
    BoxSize _size;		// Size
    BoxExtend _extend;		// Extensibility
    const char *_type;		// type

    static void epsHeader (std::ostream& os, 
			   const BoxRegion& region, 
			   const PostScriptPrintGC& gc);

    Box& operator = (const Box&);

protected:
    int _links;			// #references (>= 1)

    BoxSize& thesize()     { return _size; }
    BoxExtend& theextend() { return _extend; }
    const BoxSize& thesize() const     { return _size; }
    const BoxExtend& theextend() const { return _extend; }

    // Copy constructor
    Box(const Box& box):
	_id(), _size(box._size), _extend(box._extend), 
	_type(box._type), _links(1)
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
    virtual void dump(std::ostream& s) const = 0;

    // Constructor
    Box(BoxSize s = BoxSize(0, 0), BoxExtend e = BoxExtend(0, 0), 
	const char *t = "Box"):
	_id(), _size(s), _extend(e), _type(t), _links(1)
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

    // Same, but only one level deep (if possible)
    virtual Box *dup0() const { return dup(); }

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
	// Default: space of zero width on the right of box
	return BoxSize(0, _size[Y]);
    }

    // Re-calculate box size
    virtual Box* resize() { return this; }

    // Propagate new font
    virtual void newFont(const string&) { resize(); }

    // Draw
    void draw(Widget w, 
	      const BoxRegion& region, 
	      const BoxRegion& exposed = BoxRegion(BoxPoint(0,0),
						   BoxSize(INT_MAX, INT_MAX)), 
	      GC gc = 0, 
	      bool context_selected = false) const;

    // Print box; Header/trailer must be pre-/postfixed
    virtual void _print(std::ostream& os, 
			const BoxRegion& region, 
			const PrintGC& gc) const = 0;

    // Print Header/trailer
    static void _printHeader(std::ostream& os, 
			     const BoxRegion& region, 
			     const PrintGC& gc);
    static void _printTrailer(std::ostream& os, 
			      const BoxRegion& region, 
			      const PrintGC& gc);

    // Custom function: print box with header and trailer
    void print(std::ostream& os = std::cout,
	       BoxRegion region = 
	           BoxRegion(BoxPoint(0,0), BoxSize(0,0)),
	       const PrintGC& gc = PostScriptPrintGC()) const
    {
	region.space(X) = max(region.space(X), size(X));
	region.space(Y) = max(region.space(Y), size(Y));

	_printHeader(os, region, gc);
	_print(os, region, gc);
	_printTrailer(os, region, gc);
    }

    // Check for equality
    bool operator == (const Box& b) const;
    bool operator != (const Box& b) const { return !(operator == (b)); }

    // Check class
    virtual bool isStringBox() const  { return false; }
    virtual bool isListBox() const    { return false; }
    virtual bool isDummyBox() const   { return false; }

    // Tag a node
    // Usage: box = box->tag(...)
    virtual Box *tag(Data *data, DataLink *dl = 0);

    // Return tag attributes for a point (No point: outermost)
    BoxRegion region(const BoxPoint& p = BoxPoint(-1,-1)) const;   // current region
    Data *data(const BoxPoint& p = BoxPoint(-1,-1)) const;         // current data
    string name(const BoxPoint& p = BoxPoint(-1,-1)) const;        // current name
    bool selected(const BoxPoint& p = BoxPoint(-1,-1)) const;      // Flag: selected?
    string info(const BoxPoint& p = BoxPoint(-1,-1)) const;        // Debugging info

    // Return the box that is to be matched
    virtual const Box& matchMe() const { return *this; }

    // Search functions

    // Count MatchBoxes
    virtual void countMatchBoxes(int[]) const {}

    // Return TagBox for a point (No Punkt: outermost)
    virtual const TagBox *findTag(const BoxPoint&) const 
    { 
	return 0;
    }
    const TagBox *findTag() const
    {
	return findTag(BoxPoint(-1, -1));
    }

    // Debugging
    // Send box to stream
    friend std::ostream& operator << (std::ostream& s, const Box& b);

    // Invariant check
    virtual bool OK() const
    {
	assert (_links > 0);
	assert (_type != 0);
	return true;
    }
};

#endif // _DDD_Box_h
// DON'T ADD ANYTHING BEHIND THIS #endif
