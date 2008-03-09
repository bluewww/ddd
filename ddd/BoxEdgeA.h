// $Id$ -*- C++ -*-
// Edge annotation using boxes

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

#ifndef _DDD_BoxEdgeAnnotation_h
#define _DDD_BoxEdgeAnnotation_h

#include "EdgeA.h"
#include "Box.h"
#include "assert.h"

class BoxEdgeAnnotation: public EdgeAnnotation {
public:
    DECLARE_TYPE_INFO

private:
    Box *_box;			// The box to use

protected:
    virtual void _draw(Widget w, const BoxPoint& p,
		       const BoxRegion& exposed, const GraphGC& gc) const;

    BoxEdgeAnnotation(const BoxEdgeAnnotation &src)
	: EdgeAnnotation(src), _box(0)
    {
	if (src._box != 0)
	    _box = src._box->dup();
    }

public:
    BoxEdgeAnnotation(Box *box)
	: EdgeAnnotation(), _box(box)
    {
	assert(box != 0);
    }

    virtual ~BoxEdgeAnnotation()
    {
	if (_box != 0)
	    _box->unlink();
    }

    BoxEdgeAnnotation& operator=(const BoxEdgeAnnotation& src)
    {
      if (this != &src) {
	Box *new_box = 0;
	if (src._box != 0)
	    new_box = src._box->dup();

	if (_box != 0)
	    _box->unlink();
	_box = new_box;

      }
      return *this;
    }

    // Duplication
    virtual EdgeAnnotation *dup() const 
    {
	return new BoxEdgeAnnotation(*this);
    }

    // Resources
    Box *box() const { return _box; }

    // Print
    virtual void _print(std::ostream& os, const BoxPoint& p, 
			const GraphGC& gc) const;

    // Size used by annotation
    virtual BoxSize size(const GraphGC&) const
    {
	return box()->size();
    }

    // Region used by annotation
    virtual BoxRegion region(const BoxPoint& p, const GraphGC& gc) const;
};

#endif // _DDD_BoxEdgeAnnotation_h
// DON'T ADD ANYTHING BEHIND THIS #endif
