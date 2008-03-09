// $Id$ -*- C++ -*-
// Passing arguments to VSL

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

#ifndef _DDD_VSLArgList_h
#define _DDD_VSLArgList_h

#include "ListBox.h"
#include "assert.h"

// A simple wrapper around a ListBox used for arguments
class VSLArgList {
    ListBox *_list;

    VSLArgList& operator=(const VSLArgList&);

    VSLArgList(const VSLArgList&);

public:
    VSLArgList()
	: _list(new ListBox)
    {}

    ~VSLArgList()
    {
	_list->unlink();
    }

    VSLArgList& operator += (Box *box)
    {
	*_list += box;
	box->unlink();
	return *this;
    }

    ListBox *list() const
    {
	return _list;
    }

#if 0
    operator ListBox *() const
    {
	return list();
    }
#endif
};

#endif // _DDD_VSLArgList_h
// DON'T ADD ANYTHING BEHIND THIS #endif
