// $Id$
// Breakpoint handler

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

#ifndef _BreakPoint_h
#define _BreakPoint_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// Diese Klasse speichert Informationen zu Breakpoints des gdb.
//-----------------------------------------------------------------------------

#include <X11/Intrinsic.h>

// Misc includes
#include "strclass.h"
#include "bool.h"

enum BPType {BREAKPOINT, WATCHPOINT};
enum BPDispo {BPKEEP, BPDEL, BPDIS};

class BreakPoint {
    string  mynumber_str;
    int     mynumber;
    BPType  mytype;
    BPDispo mydispo;
    bool    myenabled;
    string  myfile_name;
    int     myline_nr;
    string  myaddress;
    string  myinfos;
    string  myignore_count;
    bool    myenabled_changed;
    bool    myfile_changed;
    bool    myposition_changed;
    bool    myaddress_changed;
    bool    myselected;
    Widget  mysource_glyph;
    Widget  mycode_glyph;

public:
    // Entfernt eigene info aus info_output.
    //
    BreakPoint (string& info_output);

    const string& number_str()   const { return mynumber_str; }
    int           number()       const { return mynumber; }
    BPType        type()         const { return mytype; }
    BPDispo       dispo()        const { return mydispo; }
    bool          enabled()      const { return myenabled; }
    const string& file_name()    const { return myfile_name; }
    int           line_nr()      const { return myline_nr; }
    const string& address()      const { return myaddress; }
    const string& infos()        const { return myinfos; }
    string        ignore_count() const { return myignore_count; }

    bool&         selected()        { return myselected; }
    Widget&       source_glyph()    { return mysource_glyph; }
    Widget&       code_glyph()      { return mycode_glyph; }

    // true wenn sich beim letzten update enabled geaendert hat.
    //
    bool enabled_changed () const { return myenabled_changed; }

    // true wenn sich beim letzten update der file_name geaendert hat.
    //
    bool file_changed () const { return myfile_changed; }

    // true wenn sich beim letzten update line_nr oder file_name geaendert hat.
    //
    bool position_changed () const { return myposition_changed; }

    // true wenn sich beim letzten update address geaendert hat.
    //
    bool address_changed () const { return myaddress_changed; }

    // liefert true, wenn sich irgendetwas geandert hat.
    // entfernt eigene info aus info_output.
    //
    bool update (string& info_output);
};


#endif
