// $Id$
// GDB communication manager.

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
// Kommunikation mit dem gdb.
// Namenskonventionen: 
// ...SUC : ruft send_user_cmd() des gdb auf.
// ...OA  : fuer on_answer, Typ: OAProc, siehe gdbAgent.
// ...OAC : fuer on_answer_completion, Typ: OACProc, siehe gdbAgent.
//-----------------------------------------------------------------------------

#ifndef _comm_manag_h
#define _comm_manag_h

#ifdef __GNUG__
#pragma interface
#endif

// includes von Nora-Zeugs
#include "strclass.h"

// includes eigener Sachen
#include "GDBAgent.h"
#include "DataDisp.h"
#include "SourceView.h"

//-----------------------------------------------------------------------------
// Deklarationen
//-----------------------------------------------------------------------------

extern GDBAgent*   gdb;
extern DataDisp*   data_disp;
extern SourceView* source_view;

//-----------------------------------------------------------------------------
// Ruft gdb->start mit den noetigen Parametern.
//
void start_gdb ();

//-----------------------------------------------------------------------------
// Behandlung der Benutzer-Befehle (am Prompt oder mit Knopf eingegeben)
// Schickt den Benutzter-Befehl cmd an den gdb.
// Setzt Filtering , ruft ggf. handle_graph_cmd, setzt insensitiv und delay,
// schreibt Befehlstext ins gdb_w und schickt Befehl an gdb.
//
void user_cmdSUC (string cmd, Widget origin = 0);


// ***************************************************************************
// Behandlung von Benutzer-Eingaben
// Schickt die Benutzer-Eingabe CMD unveraendert an den gdb.
//
void user_rawSUC (string cmd, Widget origin = 0);

#endif



