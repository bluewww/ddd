// $Id$
// Deklaration Klasse TagBox

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_TagBox_h
#define _DDD_TagBox_h

#ifdef __GNUG__
#pragma interface
#endif


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

    void dump(ostream& s) const;

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
