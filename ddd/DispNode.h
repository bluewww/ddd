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
#include "DispValue.h"

class DispBox;

// Event types
const unsigned DispNode_Disabled   = 0;

const unsigned DispNode_NTypes  = DispNode_Disabled + 1;

//-----------------------------------------------------------------------------
// Non-class functions
//-----------------------------------------------------------------------------

// Return true iff S is a user command display (`status display')
inline bool is_user_command(const string& s)
{
    return s.length() >= 2 && s[0] == '`' && s[s.length() - 1] == '`';
}

// Return user command from S
string user_command(const string& s);

//-----------------------------------------------------------------------------
// The DispNode class
//-----------------------------------------------------------------------------

class DispNode: public BoxGraphNode {
public:
    DECLARE_TYPE_INFO

private:
    int           mydisp_nr;	      // Display number
    string        myname;	      // Display expression
    string        myaddr;	      // Location of expression
    string        myscope;	      // Program location where created
    string        mydepends_on;	      // Display we depend upon (when deferred)
    bool          myactive;	      // Flag: is display active (in scope)?
    bool          saved_node_hidden;  // Saved `hidden' flag of node
    bool          mydeferred;	      // Flag: is display deferred?
    int           myclustered;	      // Flag: is display clustered?
    bool          myplotted;	      // Flag: is display plotted?
    bool          myconstant;	      // Flag: is display constant?
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
    
    virtual string str() const { return myname; }

    DispNode(const DispNode& node);

private:
    // Prohibit assignment
    DispNode& operator = (const DispNode&) { assert(0); return *this; }

public:
    // Create a new display numbered DISP_NR, named NAME, created at
    // SCOPE (a function name or "") with a value of VALUE.
    DispNode(int disp_nr,
	     const string& name,
	     const string& scope,
	     const string& value,
	     bool plotted);

    // Destructor
    ~DispNode();

    // Duplication
    GraphNode *dup() const
    {
	return new DispNode(*this);
    }

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

    bool enabled()  const { return value() != 0 && value()->enabled(); }
    bool disabled() const { return !enabled(); }
    bool active() const   { return myactive; }
    bool deferred() const { return mydeferred; }
    bool& deferred()      { return mydeferred; }
    int clustered() const { return myclustered; }
    bool plotted() const  { return myplotted; }
    bool& plotted()       { return myplotted; }
    bool constant() const { return myconstant; }
    bool& constant()      { return myconstant; }

    int last_change() const { return mylast_change; }

    bool is_user_command() const { return ::is_user_command(name()); }
    string user_command() const  { return ::user_command(name()); }

    // Return `true' if this expression can be aliased
    bool alias_ok() const;

    DispValue* value()          const { return disp_value; }
    DispValue* selected_value() const { return myselected_value; }

    // Handlers
    static void addHandler (unsigned    type,
			    HandlerProc proc,
			    void*       client_data = 0);

    static void removeHandler (unsigned    type,
			       HandlerProc proc,
			       void        *client_data = 0);



    // Update with NEW_VALUE; return false if value is unchanged
    bool update (string& new_value);

    // Update address with NEW_ADDR
    void set_addr(const string& new_addr);

    // Re-create the box from old DISP_VALUE
    void refresh ();

    // Highlights the box related to the display value DV
    void select (DispValue *dv = 0);

    // Copy selection state from SRC
    void copy_selection_state(const DispNode& src);

    // Disable and enable manually
    void disable();
    void enable();

    // Show and hide manually
    void make_active();
    void make_inactive();

    // Cluster display into TARGET
    void cluster(int target = 0);

    // Toggle titles.  Return true iff changed.
    bool set_title(bool set);
};

// Clustering stuff

// The command to use for clusters
#define CLUSTER_COMMAND "displays"

inline bool is_cluster(DispNode *dn)
{
    return dn->is_user_command() && dn->user_command() == CLUSTER_COMMAND;
}

#endif // _DDD_DispNode_h
// DON'T ADD ANYTHING BEHIND THIS #endif
