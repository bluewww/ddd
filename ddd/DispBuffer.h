// $Id$
// Filter display information from GDB output

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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

//-----------------------------------------------------------------------------
// Diese Klasse filtert aus den gdb-Ausgaben die Display-Ausgben heraus.
//-----------------------------------------------------------------------------

#ifndef _DispBuffer_h
#define _DispBuffer_h

#ifdef __GNUG__
#pragma interface
#endif

// includes von Nora-Zeugs
#include "bool.h"
#include "strclass.h"

class DispBuffer {
    // Was stand in der bisherigen Antwort ?
    //
    enum ReadState {Null, DisplayPart, DisplayFound};
    
    string display_buffer;
    string answer_buffer;      // fuer Vielleicht-Display-Teile
    ReadState already_read;

public:
    // Constructor
    DispBuffer () :
	display_buffer(""),
	answer_buffer(""),
	already_read(Null)
    {}

    // Filtert Display-Ausgaben aus answer (ein Antwortteil) heraus und
    // puffert sie,
    // answer enthaelt anschliessend nur noch andere Ausgaben,
    // ggf. werden Teile der Antwort 'auf Verdacht' zurueckgehalten.
    //
    void filter (string& answer);

    // Die komplettierung der gdb-Ausgabe hiermit bekanntgeben.
    // ggf. werden die 'auf Verdacht' zurueckgehaltenen Teile der Antwort
    // zurueckgegeben.
    //
    string answer_ended ();

    // enthielt die letzte Ausgabe Display-Ausgaben?
    //
    bool displays_found () const { return display_buffer != ""; }

    // Gibt die gepufferten Display-Ausgaben zurueck.
    // evtl. stehen nach den Displays noch andere Ausgaben.
    //
    string get_displays () const { return display_buffer; }

    // Aufrufen bevor neue Antwort gefiltert wird!
    //
    void clear () {
	display_buffer = "";
	answer_buffer = "";
	already_read = Null;
    }

};

#endif
