// $Id$
// Helper functions to process GDB display output

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
// Verschiedene Routinen, die das Erkennen bzw. Lesen
// (und Erzeugen) von display-Ausgaben erleichtern.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef _disp_read_h
#define _disp_read_h

#ifdef __GNUG__
#pragma interface
#endif

// Misc includes
#include "strclass.h"
#include "bool.h"

// DDD includes
#include "GDBAgent.h"

//----------------------------------------------------------------------------
// fuer die Erkennung bestimmter Befehle
//----------------------------------------------------------------------------

// ***************************************************************************
// false, wenn cmd ein einzelnes display erzeugt.
// 
bool is_single_display_cmd(const string& cmd, GDBAgent *gdb);

// ***************************************************************************
// false, wenn cmd keine besonderen Auswirkungen hat.
// 
bool is_nop_cmd(const string& cmd);

// ***************************************************************************
// false, wenn cmd ein einzelnes display erzeugt.
// 
bool is_display_cmd(const string& cmd);

// ***************************************************************************
// true, wenn cmd Programmstop (mit display-Ausgabe) zur Folge hat.
// 
bool is_running_cmd(const string& cmd, GDBAgent *gdb);

// ***************************************************************************
// true, wenn cmd ein Programm startet.
// 
bool is_run_cmd(const string& cmd);

// ***************************************************************************
// true, wenn cmd Programm-Argumente setzt.
// 
bool is_set_args_cmd(const string& cmd);

// ***************************************************************************
// true, wenn cmd Frame-Wechsel zur Folge hat.
// 
bool is_frame_cmd(const string& cmd);
bool is_up_cmd(const string& cmd);
bool is_down_cmd(const string& cmd);

// ***************************************************************************
// true, wenn cmd Variablen-Wechsel zur Folge hat.
// 
bool is_set_cmd(const string& cmd);

// ***************************************************************************
// true, wenn cmd Einstellungs-Wechsel zur Folge hat.
// 
bool is_setting_cmd(const string& cmd);

// ***************************************************************************
// true, wenn cmd File-Wechsel zur Folge hat.
// 
bool is_file_cmd(const string& cmd, GDBAgent *gdb);

// ***************************************************************************
// true, wenn cmd Breakpoints setzt
// 
bool is_break_cmd(const string& cmd);

// ***************************************************************************
// true, wenn cmd die aktuelle Position veraendert
// 
bool is_lookup_cmd(const string& cmd);

// ***************************************************************************
// true, wenn cmd das aktuelle Verzeichnis veraendert
// 
bool is_cd_cmd(const string& cmd);


bool is_graph_cmd(const string& cmd);
bool is_refresh_cmd(const string& cmd);
string get_display_expression(const string& cmd);

//----------------------------------------------------------------------------
// fuer das Erkennen von Displayausgaben
//----------------------------------------------------------------------------

// ***************************************************************************
// -1, wenn gdb_answer kein display enthaelt, 
// sonst den index des ersten displays.
// 
int display_index (const string& gdb_answer, GDBAgent *gdb);

// ***************************************************************************
// 
bool contains_display (const string& gdb_answer, GDBAgent *gdb);

// ***************************************************************************
// gibt index zurueck, an dem ein Display anfangen koennte (d.h. index eines
// moeglichen Display-Teils
// 
int possible_begin_of_display (string gdb_answer, GDBAgent *gdb);

// ***************************************************************************
// gibt den naechsten Display zurueck falls vorhanden, und
// schneidet diesen von displays vorn ab.
// 
string read_next_display (string& displays, GDBAgent *gdb);


// ***************************************************************************
// schneidet vom display "'nr': 'name' = " vorne ab.
// 
string get_disp_value_str (/*const*/ string& display, GDBAgent *gdb);


//----------------------------------------------------------------------------
// fuer das Erkennen der Ausdruecke bei info display
//----------------------------------------------------------------------------

// ***************************************************************************
// gibt den ersten (naechsten) Display-Info aus 
// Ist kein weiteres display-info vorhanden, sind return-Wert und gdb_answer
// gleich "".
// 
string read_first_disp_info (string& gdb_answer, GDBAgent *gdb);
string read_next_disp_info (string& gdb_answer, GDBAgent *gdb);

// ***************************************************************************
// schneidet "'nr': " vorne ab
//
string get_info_disp_str (string& display, GDBAgent *gdb);

// ***************************************************************************
//
bool disp_is_disabled (const string& info_disp_str, GDBAgent *gdb);



//----------------------------------------------------------------------------
// fuer Knotenerzeugung (Lesen des Display-Anfangs)
//----------------------------------------------------------------------------

string  read_disp_nr_str (string& display, GDBAgent *gdb);
string  read_disp_name   (string& display, GDBAgent *gdb);
bool is_disabling (const string& value, GDBAgent *gdb);
bool is_not_active (const string& value, GDBAgent *gdb);

#endif
