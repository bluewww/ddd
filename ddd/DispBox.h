// $Id$
// Display boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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

//-----------------------------------------------------------------------------
// Klasse zur Erzeugung von DispBoxen mit VSL.
//-----------------------------------------------------------------------------

#ifndef _DispBox_h
#define _DispBox_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
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
    static string  vsllib_name;
    static string  vsllib_path;
    static string  vsllib_defs;
    static int     max_display_title_length;

    // bei dv == 0 disabled-box erzeugen
    DispBox (string disp_nr,
	     string name,
	     const DispValue* dv = 0);

    ~DispBox ();

    Box* box () const { return mybox; }

    // bei dv == 0 disabled-box erzeugen
    void set_value (const DispValue* dv = 0);

private:
    Box*   create_value_box (const DispValue* dv, int member_name_width = 0);

    static VSLLib* vsllib_ptr;
    static void init_vsllib();
    static VSLLib *vsllib()
    {
	if (vsllib_ptr == 0)
	    init_vsllib();
	return vsllib_ptr;
    }
    

protected:
    // Evaluation functions
    static Box *dup(const string& func_name, const Box *box);
    static Box *eval(const string& func_name, ListBox *arg)
    {
	return dup(func_name, vsllib()->eval(func_name, arg));
    }
    static Box *eval(const string& func_name, VSLArg args[])
    {
	return dup(func_name, vsllib()->eval(func_name, args));
    }
    static Box *eval(const string& func_name, 
		     VSLArg arg1 = (Box *)0,
		     VSLArg arg2 = (Box *)0,
		     VSLArg arg3 = (Box *)0)
    {
	return dup(func_name, vsllib()->eval(func_name, arg1, arg2, arg3));
    }
};


#endif
