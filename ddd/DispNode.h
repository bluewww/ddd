// $Id$
// Store information about a single display espression

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

#ifndef _DDD_DispNode_h
#define _DDD_DispNode_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// A DispNode keeps all information about a single data display
//-----------------------------------------------------------------------------

// Misc includes
#include "strclass.h"
#include "assert.h"
#include "bool.h"
#include "GraphNode.h"
#include "HandlerL.h"
#include "BoxGraphN.h"

class DispValue;
class DispBox;

// Event types
const unsigned DispNode_Disabled   = 0;

const unsigned DispNode_NTypes  = DispNode_Disabled + 1;

//-----------------------------------------------------------------------------
// Non-class functions
//-----------------------------------------------------------------------------

// Return true iff S is a user command display (`status display')
bool is_user_command(const string& s);

// Return user command from S
string user_command(const string& s);


//-----------------------------------------------------------------------------
// The DispNode class
//-----------------------------------------------------------------------------

class DispNode 
{
private:
    int           mydisp_nr;	      // Display number
    string        myname;	      // Display expression
    string        myaddr;	      // Location of expression
    string        myscope;	      // Program location where created
    string        mydepends_on;	      // Display we depend upon (when deferred)
    bool          myenabled;	      // Flag: is display enabled?
    bool          myactive;	      // Flag: is display active (in scope)?
    bool          saved_node_hidden;  // Saved `hidden' flag of node
    bool          mydeferred;	      // Flag: is display deferred?
    BoxGraphNode* mynodeptr;	      // Associated graph node 
    DispValue*    disp_value;	      // Associated value
    DispValue*    myselected_value;   // Selected value within DISP_VALUE
    DispBox*      disp_box;	      // Associated box within DISP_VALUE
    int           mylast_change;      // Last value or address change

    static int change_tics;           // Shared change counter

public:
    int           alias_of;	      // Alias of another display

protected:
    static HandlerList handlers;
    static class TagBox *findTagBox(const Box *box, DispValue *dv);

private:
    DispNode(const DispNode&)
	: mydisp_nr(0), myname(), myaddr(), myscope(), mydepends_on(),
	  myenabled(false), myactive(false), saved_node_hidden(false),
	  mydeferred(false), mynodeptr(0), disp_value(0), 
	  myselected_value(0), disp_box(0), mylast_change(0), alias_of(0)
    {
	assert(0);
    }
    DispNode& operator = (const DispNode&) { assert(0); return *this; }

public:
    // Create a new display numbered DISP_NR, named NAME, created at
    // SCOPE (a function name or "") with a value of VALUE.
    DispNode(int disp_nr,
	     const string& name,
	     const string& scope = "",
	     const string& value = "");

    // Destructor
    ~DispNode();

    // Resources
    int disp_nr()  const             { return mydisp_nr; }
    const string& name() const       { return myname; }
    const string& addr() const       { return myaddr; }
    const string& scope() const      { return myscope; }
    const string& depends_on() const 
    {
	assert(deferred()); 
	return mydepends_on;
    }
    string& depends_on()
    { 
	assert(deferred()); 
	return mydepends_on;
    }

    bool enabled()  const   { return myenabled; }
    bool disabled() const   { return !myenabled; }
    bool active() const     { return myactive; }
    bool deferred() const   { return mydeferred; }
    bool& deferred()        { return mydeferred; }

    int last_change() const { return mylast_change; }

    bool is_user_command() const { return ::is_user_command(name()); }
    string user_command() const  { return ::user_command(name()); }

    // Return `true' if this expression can be aliased
    bool alias_ok() const;

    BoxGraphNode* nodeptr() const { return mynodeptr; }
    const Box*    box()     const { return mynodeptr->box(); }
    DispValue*    value()   const { return disp_value; }
    DispValue*    selected_value() const { return myselected_value; }

    bool& selected()                    { return mynodeptr->selected(); }
    void moveTo(const BoxPoint& newPos) { mynodeptr->moveTo(newPos); }

    // Handlers
    static void addHandler (unsigned    type,
			    HandlerProc proc,
			    void*       client_data = 0);

    static void removeHandler (unsigned    type,
			       HandlerProc proc,
			       void        *client_data = 0);



    // Update with NEW_VALUE;  return false if value is unchanged
    bool update (string& new_value);

    // Update address with NEW_ADDR
    void set_addr(const string& new_addr);

    // Re-create the box from old DISP_VALUE
    void refresh ();

    // Highlights the box related to the display value DV
    void select (DispValue *dv = 0);

    // Copy selection state from SRC
    void copy_selection_state(DispNode *src);

    // Disable and enable manually
    void disable();
    void enable();

    // Show and hide manually
    void make_active();
    void make_inactive();

    // Toggle titles
    void refresh_title();
};

#endif // _DDD_DispNode_h
// DON'T ADD ANYTHING BEHIND THIS #endif
