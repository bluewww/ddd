// $Id$
// Display boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

//-----------------------------------------------------------------------------
// Klasse zur Erzeugung von DispBoxen mit VSL.
//-----------------------------------------------------------------------------

#ifndef _DispBox_h
#define _DispBox_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Box.h"
#include "VSLLib.h"
#include "DispValue.h"

//-----------------------------------------------------------------------------

class DispBox {
private:
    Box* mybox;
    Box* title_box;
public:
    // Must be initialized from outside!
    static VSLLib* vsllibptr;
    static int     max_name_length;

    // bei dv == 0 disabled-box erzeugen
    DispBox (string disp_nr,
	     string name,
	     const DispValue* dv = 0);

    ~DispBox ();

    Box* box () const { return mybox; }

    // bei dv == 0 disabled-box erzeugen
    void set_value (const DispValue* dv = 0);

private:
    string short_name (string name, int length = max_name_length);
    Box*   create_value_box (const DispValue* dv, int member_name_width = 0);

protected:
    // Evaluation functions
    static Box *dup(const string& func_name, const Box *box);
    static Box *eval(const string& func_name, ListBox *arg)
    {
	return dup(func_name, vsllibptr->eval(func_name, arg));
    }
    static Box *eval(const string& func_name, VSLArg args[])
    {
	return dup(func_name, vsllibptr->eval(func_name, args));
    }
    static Box *eval(const string& func_name, 
		     VSLArg arg1 = (Box *)0,
		     VSLArg arg2 = (Box *)0,
		     VSLArg arg3 = (Box *)0)
    {
	return dup(func_name, vsllibptr->eval(func_name, arg1, arg2, arg3));
    }
};


#endif
