// $Id$
// Display boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_DispBox_h
#define _DDD_DispBox_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// Create display boxes via the VSL library
//-----------------------------------------------------------------------------

#include "strclass.h"
#include "Box.h"
#include "VSLLib.h"
#include "VSLArgList.h"
#include "DispValue.h"
#include "assert.h"

//-----------------------------------------------------------------------------

class DispBox {
private:
    Box *mybox;
    Box *title_box;

protected:
    DispBox(const DispBox &node)
	: mybox(node.mybox ? node.mybox->dup() : 0),
	  title_box(node.title_box ? node.title_box->dup() : 0)
    {}

private:
    DispBox& operator = (const DispBox&) { assert(0); return *this; }

    static bool is_numeric(const DispValue *dv, const DispValue *parent);

public:
    // Must be initialized from outside!
    static string  vsllib_name;
    static string  vsllib_path;
    static string  vsllib_defs;
    static string  vsllib_base_defs;
    static int     max_display_title_length;
    static bool    align_2d_arrays;

    // True if the VSL library has been initialized
    static bool    vsllib_initialized;

    // Initialize VSL library, using BACKGROUND as work proc
    static void init_vsllib(void (*background)() = 0);

    // Create a new box.  If DV == 0, create a disabled box.
    DispBox (int disp_nr, const string& title, 
	     const DispValue *dv = 0, const DispValue *parent = 0);

    ~DispBox ();

    Box *box () const { return mybox; }

    DispBox *dup() const { return new DispBox(*this); }

    // Set new value to DV.  If DV == 0, make it disabled.
    void set_value (const DispValue *dv, const DispValue *parent = 0);

    // Set title to NAME; if NAME == "", disable it
    void set_title(int disp_nr, string name);

    bool have_title() const { return title_box != 0; }

private:
    Box *create_value_box(const DispValue *dv,
			  const DispValue *parent,
			  int member_name_width = 0);

    Box *_create_value_box(const DispValue *dv,
			   const DispValue *parent);

    static VSLLib dummylib;
    static VSLLib *vsllib_ptr;
    static VSLLib *vsllib()
    {
	if (!vsllib_initialized)
	    init_vsllib();

	return vsllib_ptr;
    }
    

protected:
    // Evaluation functions
    static Box *check(const string& func_name, const Box *box);

    static void shorten_title(string& title);

public:
    static Box *eval(const string& func_name, const VSLArgList& args)
    {
	return check(func_name, vsllib()->eval(func_name, args.list()));
    }
    static Box *eval(const string& func_name, VSLArg args[])
    {
	return check(func_name, vsllib()->eval(func_name, args));
    }
    static Box *eval(const string& func_name, 
		     VSLArg arg1 = (Box *)0,
		     VSLArg arg2 = (Box *)0,
		     VSLArg arg3 = (Box *)0,
		     VSLArg arg4 = (Box *)0)
    {
	return check(func_name, 
		     vsllib()->eval(func_name, arg1, arg2, arg3, arg4));
    }
};

#endif // _DDD_DispBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
