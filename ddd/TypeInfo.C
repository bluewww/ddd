// $Id$ -*- C++ -*-
// Run-time type information

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the ICE Library.
// 
// The ICE Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The ICE Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the ICE Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// ICE is the incremental configuration engine.
// Contact ice@ips.cs.tu-bs.de for details.

char TypeInfo_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "VarArray.h"
#pragma implementation "DynArray.h"
#endif

#include "TypeInfo.h"
#include "VarArray.h"
#include "hash.h"


// TypeInfoPtrSet is a simple set with O(n^2) behaviour
typedef const TypeInfo *TypeInfoPtr;

typedef VarArray<TypeInfoPtr> TypeInfoPtrArray;

class TypeInfoPtrSet: public TypeInfoPtrArray {
public:
    virtual void add(const TypeInfoPtr& v)
    {
        for (int i = 0; i < size(); i++)
            if (operator[](i) == v)
                return;

        TypeInfoPtrArray::add(v);
    }
};


// Set this to a non-zero value to allow debugging
int TypeInfo::debug = 0;

// Setup the _all_bases entry
void TypeInfo::_setup_all_bases() const
{
    TypeInfoPtrSet set;

    // Add all base classes to set
    BaseIterator direct_base(*this, BaseIterator::direct);
    const TypeInfo *direct_info;

    while ((direct_info = direct_base()) != 0)
    {
	set += direct_info;

	BaseIterator all_base(*direct_info, BaseIterator::all);
	const TypeInfo *indirect_info;
	while ((indirect_info = all_base()) != 0)
	    set += indirect_info;
    }

    // Build new storage
    BaseList new_all_bases = new const TypeInfo *[set.size() + 1];

    for (int i = 0; i < set.size(); i++)
	new_all_bases[i] = set[i];
    new_all_bases[i] = 0;

    *_all_bases = new_all_bases;

    if (debug)
	clog << "(TypeInfo: set up " << *this << ")";
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
