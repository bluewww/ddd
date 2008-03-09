// $Id$
// Tagged boxes

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

#ifndef _DDD_TagBox_h
#define _DDD_TagBox_h

#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "Widget.h"
#include "MarkBox.h"

// TagBox

class TagBox: public MarkBox {
public:
    DECLARE_TYPE_INFO

private:
    DataLink *datalink;         // Access functions
    Data *_data;                // Tag Data 

    // Default access functions
    static DataLink default_datalink;

    TagBox& operator = (const TagBox&);

protected:
    // Draw
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, 
		       GC gc,
		       bool context_selected) const;

    // Copy
    TagBox(const TagBox& tagbox):
	MarkBox(tagbox._box),
	datalink(tagbox.datalink),
	_data((*(tagbox.datalink->link))(tagbox._data))
    {}

    void dump(std::ostream& s) const;

    // Find TagBox within P
    const TagBox *findTag(const BoxPoint&) const;

public:
    // Constructor
    TagBox(Box *box, Data *dta = 0, DataLink *dl = 0):
	MarkBox(box),
	datalink(dl == 0 ? &default_datalink : dl),
	_data(dta)
    {}

    Box *dup() const { return new TagBox(*this); }

    Box *link()
    {
	// When a tag box is drawn, it may be drawn in several regions.
	// To make sure all these are properly recorded, a tag box is
        // always duplicated.

	return dup();
    }

    // Destructor
    ~TagBox() { (*(datalink->unlink))(_data); }

    // Resources
    Data *__data() const        { return _data; }
    bool __selected() const  { return (*(datalink->selected))(_data); }
    string __name() const       { return (*(datalink->name))(_data); }
    string __info() const       { return (*(datalink->info))(_data); }
};

#endif
