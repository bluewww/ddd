// $Id$
// Filter position information from GDB output.

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
// Diese Klasse filtert aus den gdb-Ausgaben eine Positionsangabe heraus.
// Voraussetzung: Aufruf des gdb mit Option -f.
//-----------------------------------------------------------------------------

#ifndef _PosBuffer_h
#define _PosBuffer_h

#ifdef __GNUG__
#pragma interface
#endif

// includes von Nora-Zeugs
#include "bool.h"
#include "strclass.h"

class PosBuffer {
    // Was stand in der bisherigen Antwort ?
    //
    enum ReadState {Null, PosPart, PosComplete};
    
    string pos_buffer;
    string func_buffer;
    string answer_buffer;      // fuer Vielleicht-Positionsangaben-Teile
    ReadState already_read;

public:
    // Constructor
    PosBuffer () :
	pos_buffer(""),
	func_buffer(""),
	answer_buffer(""),
	already_read(Null)
    {}

    // Filtert Positionsangabe aus answer (ein Antwortteil) heraus und
    // puffert sie;
    // answer enthaelt anschliessend nur noch andere Ausgaben;
    // ggf. werden Teile der Antwort 'auf Verdacht' zurueckgehalten.
    //
    void filter (string& answer);

    // Die Komplettierung der gdb-Ausgabe hiermit bekanntgeben!
    // ggf. werden die 'auf Verdacht' zurueckgehaltenen Teile der Antwort
    // zurueckgegeben.
    //
    string answer_ended ();

    // enthielt die letzte Ausgabe Positionsangaben?
    //
    bool pos_found () const { return already_read == PosComplete; }

    // Gibt die gepufferte Positionsangabe zurueck;
    //
    string get_position () const { return pos_buffer; }
    string get_function () const { return func_buffer; }

    // Aufrufen bevor neue Antwort gefiltert wird!
    //
    void clear () {
	pos_buffer    = "";
	func_buffer   = "";
	answer_buffer = "";
	already_read  = Null;
    }

};

#endif
