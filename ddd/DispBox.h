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

#ifndef _DDD_DispBox_h
#define _DDD_DispBox_h

//-----------------------------------------------------------------------------
// Create display boxes via the VSL library
//-----------------------------------------------------------------------------

#include "strclass.h"
#include "Box.h"
#include "ThemeVSLL.h"
#include "VSLArgList.h"
#include "DataLink.h"
#include "DispValue.h"
#include "ThemeM.h"
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
    DispBox& operator = (const DispBox&);

    static bool is_numeric(const DispValue *dv, const DispValue *parent);

    static struct VSLLibCache *vsllib_cache;

protected:
    static DataLink data_link;
    static Data *link_link(Data *);
    static void link_unlink(Data *);
    static bool link_selected(Data *);
    static string link_info(Data *);
    static string link_name(Data *);

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

    // Mapping of expressions to themes
    static ThemeManager theme_manager;

    // Clear cache
    static void clear_vsllib_cache();

    // Create a new box.  If DV == 0, create a disabled box.
    DispBox (int disp_nr, const string& title, 
	     const DispValue *dv = 0, const DispValue *parent = 0);

    ~DispBox ();

    Box *box () const { return mybox; }

    DispBox *dup() const { return new DispBox(*this); }

    // Set new value to DV.  If DV == 0, make it disabled.
    void set_value (const DispValue *dv, const DispValue *parent = 0);

    // Set title to NAME; if NAME == "", disable it
    void set_title(const DispValue *dv, int disp_nr, const string& name);

    bool have_title() const { return title_box != 0; }

private:
    Box *create_value_box(const DispValue *dv,
			  const DispValue *parent,
			  int member_name_width = 0);

    Box *_create_value_box(const DispValue *dv,
			   const DispValue *parent);

    static ThemedVSLLib dummylib;
    static ThemedVSLLib *vsllib_ptr;
    static VSLLib *vsllib(const DispValue *dv);

protected:
    // Evaluation functions
    static Box *check(const string& func_name, const Box *box);

    static void shorten_title(string& title);

public:
    static Box *eval(const DispValue *dv,
		     const string& func_name, const VSLArgList& args)
    {
	return check(func_name, vsllib(dv)->eval(func_name, args.list()));
    }
    static Box *eval(const DispValue *dv,
		     const string& func_name, VSLArg args[])
    {
	return check(func_name, vsllib(dv)->eval(func_name, args));
    }
    static Box *eval(const DispValue *dv,
		     const string& func_name, 
		     VSLArg arg1 = (Box *)0,
		     VSLArg arg2 = (Box *)0,
		     VSLArg arg3 = (Box *)0,
		     VSLArg arg4 = (Box *)0)
    {
	return check(func_name, 
		     vsllib(dv)->eval(func_name, arg1, arg2, arg3, arg4));
    }
};

#endif // _DDD_DispBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
