// $Id$ -*- C++ -*-
// Run-time type information

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

char TypeInfo_rcsid[] =
    "$Id$";

#include "TypeInfo.h"
#include "hash.h"

#if !HAVE_TYPEINFO

// Set this to a non-zero value to allow debugging
int TypeInfo::debug = 0;

// Setup the _all_bases entry
void TypeInfo::_setup_all_bases() const
{
    const TypeInfo *direct_info;
    int count = 0;

    {
	// Count max number of entries
	BaseIterator direct_bases(*this, BaseIterator::direct);
	while ((direct_info = direct_bases()) != 0)
	{
	    count++;

	    BaseIterator all_bases(*direct_info, BaseIterator::all);
	    const TypeInfo *indirect_info;
	    while ((indirect_info = all_bases()) != 0)
		count++;
	}
    }

    // Build new storage
    BaseList new_all_bases = new const TypeInfo *[count + 1];

    count = 0;

    {
	BaseIterator direct_bases(*this, BaseIterator::direct);
	while ((direct_info = direct_bases()) != 0)
	{
	    int i;
	    for (i = 0; i < count; i++)
		if (new_all_bases[i] == direct_info)
		    break;
	    if (i >= count)
		new_all_bases[count++] = direct_info;

	    BaseIterator all_bases(*direct_info, BaseIterator::all);
	    const TypeInfo *indirect_info;
	    while ((indirect_info = all_bases()) != 0)
	    {
		int i;
		for (i = 0; i < count; i++)
		    if (new_all_bases[i] == indirect_info)
			break;
		if (i >= count)
		    new_all_bases[count++] = indirect_info;
	    }
	}
    }

    new_all_bases[count] = 0;

    *_all_bases = new_all_bases;

    if (debug)
    {
	std::clog << "TypeInfo: setup done for " << *this << "\n";
	std::clog.flush();
    }
}
	
// Check if we can cast to info
int TypeInfo::_can_cast(const TypeInfo& info) const
{
    BaseIterator all_base(*this, BaseIterator::all);
    const TypeInfo *base_info;

    while ((base_info = all_base()) != 0)
	if (base_info == &info)
	    return 1;

    return 0;
}

// Rebuild hash code
void TypeInfo::_setup_hash() const
{
    int h = hashpjw(name());
    if (h == 0)
	h = 1;

    ((TypeInfo *)this)->_hash = h;
}

// Echo type info in C++ format
ostream& operator << (ostream& os, const TypeInfo& info)
{
    os << "class " << info.name();
    
    BaseIterator direct_base(info, BaseIterator::direct);
    const TypeInfo *base_info;
    int count = 0;

    while ((base_info = direct_base()) != 0)
    {
	if (count++ == 0)
	    os << ":";
	else
	    os << ",";
	os << " public " << base_info->name();
    }

    BaseIterator all_base(info, BaseIterator::all);
    count = 0;

    while ((base_info = all_base()) != 0)
    {
	if (count++ == 0)
	    os << " // all: ";
	else
	    os << ", ";
	os << base_info->name();
    }

    return os;
}

#endif // !HAVE_TYPEINFO
