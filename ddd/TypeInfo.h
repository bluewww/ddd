// $Id$ -*- C++ -*-
// Run-time type information

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_TypeInfo_h
#define _DDD_TypeInfo_h

#include "config.h"

#if HAVE_TYPEINFO

#include <typeinfo>

// Public interface.  Defined according to: Stroustrup, the C++
// Programming Language, 2nd Ed., Section 13.5

// Use ISO C++ run-time type information
#define static_type_info(T)  typeid(T)
#define ptr_type_info(p)     typeid(p)
#define ref_type_info(r)     typeid(&(r))
#define ptr_cast(T, p)       dynamic_cast<T *>(p)
#define ref_cast(T, r)       dynamic_cast<T &>(r)
#define const_ptr_cast(T, p) dynamic_cast<const T *>(p)
#define const_ref_cast(T, r) dynamic_cast<const T &>(r)


// Old-style definition macros are no more required
#define DECLARE_TYPE_INFO
#define DEFINE_TYPE_INFO_0(T)
#define DEFINE_TYPE_INFO_1(T, B1)
#define DEFINE_TYPE_INFO_2(T, B1, B2)
#define DEFINE_TYPE_INFO_3(T, B1, B2, B3)
#define DEFINE_TYPE_INFO_4(T, B1, B2, B3, B4)
#define DEFINE_TYPE_INFO_5(T, B1, B2, B3, B4, B5)
#define DEFINE_TYPE_INFO_6(T, B1, B2, B3, B4, B5, B6)
#define DEFINE_TYPE_INFO_7(T, B1, B2, B3, B4, B5, B6, B7)
#define DEFINE_TYPE_INFO_8(T, B1, B2, B3, B4, B5, B6, B7, B8)
#define DEFINE_TYPE_INFO_9(T, B1, B2, B3, B4, B5, B6, B7, B8, B9)

#else // !HAVE_TYPEINFO

// No C++ run-time type information available - realize it manually

#include <iostream>
#include <stdlib.h>
#include "assert.h"

#ifdef __GNUC__
// Set this to 1 to enable TypeInfo table construction at link time
// instead of run-time (GCC 2.7.0 or lower only)
// #define IMMEDIATE_TYPE_INFO 1
#endif

// Use these macros to declare and define type information
#define DECLARE_TYPE_INFO \
    static TypeInfo type_info; \
    virtual const TypeInfo& get_type_info() const { return type_info; } \
    static BaseList _type_info_all_bases; \
    static const TypeInfo *_type_info_direct_bases[];

#if IMMEDIATE_TYPE_INFO
#define _DEFINE_TYPE_INFO(T) \
    TypeInfo T::type_info = \
        { #T, T::_type_info_direct_bases, &T::_type_info_all_bases }; \
    BaseList T::_type_info_all_bases = 0; \
    const TypeInfo *T::_type_info_direct_bases[] = 
#else
#define _DEFINE_TYPE_INFO(T) \
    TypeInfo T::type_info(#T, T::_type_info_direct_bases, \
                          &T::_type_info_all_bases); \
    BaseList T::_type_info_all_bases = 0; \
    const TypeInfo *T::_type_info_direct_bases[] = 
#endif

#define DEFINE_TYPE_INFO_0(T) \
    _DEFINE_TYPE_INFO(T) { 0 };

#define DEFINE_TYPE_INFO_1(T, B1) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, 0 };

#define DEFINE_TYPE_INFO_2(T, B1, B2) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, &B2::type_info, 0 };

#define DEFINE_TYPE_INFO_3(T, B1, B2, B3) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, &B2::type_info, &B3::type_info, \
			   0 };

#define DEFINE_TYPE_INFO_4(T, B1, B2, B3, B4) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, &B2::type_info, &B3::type_info, \
			   &B4::type_info, 0 };

#define DEFINE_TYPE_INFO_5(T, B1, B2, B3, B4, B5) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, &B2::type_info, &B3::type_info, \
			   &B4::type_info, &B5::type_info, 0 };

#define DEFINE_TYPE_INFO_6(T, B1, B2, B3, B4, B5, B6) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, &B2::type_info, &B3::type_info, \
			   &B4::type_info, &B5::type_info, &B6::type_info, \
			   0 };

#define DEFINE_TYPE_INFO_7(T, B1, B2, B3, B4, B5, B6, B7) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, &B2::type_info, &B3::type_info, \
			   &B4::type_info, &B5::type_info, &B6::type_info, \
			   &B7::type_info, 0 };

#define DEFINE_TYPE_INFO_8(T, B1, B2, B3, B4, B5, B6, B7, B8) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, &B2::type_info, &B3::type_info, \
			   &B4::type_info, &B5::type_info, &B6::type_info, \
			   &B7::type_info, &B8::type_info, 0 };

#define DEFINE_TYPE_INFO_9(T, B1, B2, B3, B4, B5, B6, B7, B8, B9) \
    _DEFINE_TYPE_INFO(T) { &B1::type_info, &B2::type_info, &B3::type_info, \
			   &B4::type_info, &B5::type_info, &B6::type_info, \
			   &B7::type_info, &B8::type_info, &B9::type_info, \
			   0 };


// Use these macros to get information
// These are defined according to:
// Stroustrup, the C++ Programming Language, 2nd Ed., Section 13.5

#define static_type_info(T) (T::type_info)
#define ptr_type_info(p)    ((p)->get_type_info())
#define ref_type_info(r)    ((r).get_type_info())
#define ptr_cast(T, p) \
((p) == 0 ? 0 : (ptr_type_info(p).can_cast(static_type_info(T)) ? \
 (T *)(p) : 0))
#define ref_cast(T, r) \
(ref_type_info(r).can_cast(static_type_info(T)) ? \
 (T &)(r) : (::abort(), (T &)(r)))
#define const_ptr_cast(T, p) (const T *)ptr_cast(T, p)
#define const_ref_cast(T, r) (const T &)ref_cast(T, r)


// Now for the innards.
// Usually, you should not need to access any of these.

typedef const class TypeInfo **BaseList;

class TypeInfo {
    friend class BaseIterator;

private:
    // Data
    const char *_name;		// Name of this class
    BaseList _direct_bases;	// List of direct base classes
    BaseList *_all_bases;	// Ptr to list of all base classes
    unsigned int _hash;		// 1Hash code (0 if not yet built)

    void _setup_all_bases() const;
    void setup_all_bases() const
    {
	if (*_all_bases == 0)
	    _setup_all_bases();
    }

    void _setup_hash() const;
    void setup_hash() const
    {
	if (_hash == 0)
	    _setup_hash();
    }

protected:
    // Resources
    /* const */ BaseList direct_bases() const { return _direct_bases; }
    /* const */ BaseList all_bases() const 
    {
	setup_all_bases();
	return *_all_bases; 
    }

    // Check if we can cast to info
    int _can_cast(const TypeInfo& info) const;

    // Make hash code
    void makehash();

private:
    // No X(const X&)
    TypeInfo(const TypeInfo&);
    TypeInfo& operator = (const TypeInfo&);

public:
#if !IMMEDIATE_TYPE_INFO
    // Construction
    TypeInfo(const char *name, BaseList direct_bases, BaseList *all_bases):
	_name(name), _direct_bases(direct_bases), _all_bases(all_bases),
	_hash(0)
    {}
#endif
     
    // Debugging
    // We cannot use a bool here, since it may be initialized too late
    static int debug;

    // Resources
    const char *name() const { return _name; }

    // Hash code
    unsigned int hash() const
    {
	setup_hash();
	return _hash;
    }

    // Comparing
    int operator == (const TypeInfo& info) const
    {
	return this == &info;
    }
    int operator != (const TypeInfo& info) const
    {
	return !operator == (info);
    }
	
    // Check if we can cast to info
    int can_cast(const TypeInfo& info) const
    {
	return this == &info || _can_cast(info);
    }

    // Output
    friend ostream& operator << (ostream& os, const TypeInfo& info);
};



// Iterators

class BaseIterator {
private:
    int index;
    const BaseList bases;

    BaseIterator(const BaseIterator&);
    BaseIterator& operator = (const BaseIterator&);

public:
    enum mode { all, direct };

    const TypeInfo *operator()()
    {
	const TypeInfo *info = bases[index];
	if (info != 0)
	    index++;
	return info;
    }

    void reset()
    {
	index = 0;
    }

    BaseIterator(const TypeInfo& info, mode m = all):
        index(0), bases(m == all ? info.all_bases() : info.direct_bases())
    {}
};

#endif // !HAVE_TYPEINFO

#endif // _DDD_TypeInfo_h
// DON'T ADD ANYTHING BEHIND THIS #endif
