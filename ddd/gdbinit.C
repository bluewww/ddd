// $Id$ -*- C++ -*-
// GDB initialization

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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

const char gdbinit_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "gdbinit.h"
#include "shell.h"
#include "string-fun.h"

#include <iostream.h>
#include <fstream.h>

// GDB initialization file (may be remote)
static string gdb_init_file;


//-----------------------------------------------------------------------------
// Create new GDB interface
//-----------------------------------------------------------------------------

GDBAgent *new_gdb(DebuggerType type,
		  const AppData& app_data,
		  XtAppContext app_context,
		  int argc, char *argv[])
{
    char *initial_cmds = 0;
    switch (type)
    {
    case GDB:
	initial_cmds = app_data.gdb_initial_cmds;
	break;

    case DBX:
	initial_cmds = app_data.dbx_initial_cmds;
	break;

    case XDB:
	initial_cmds = app_data.xdb_initial_cmds;
	break;
    }

    if (initial_cmds == 0 || initial_cmds[0] == '\0')
	gdb_init_file = "";
    else
    {
	// Set initial commands
	if (remote_gdb())
	{
	    gdb_init_file = "${TMPDIR=/tmp}/ddd" + itostring(getpid());
	    string cmd = sh_command("cat > " + gdb_init_file);
	    FILE *fp = popen(cmd, "w");
	    if (fp == 0)
		perror(gdb_init_file);
	    else
	    {
		fputs(initial_cmds, fp);
		pclose(fp);
	    }
	}
	else
	{
	    gdb_init_file = tmpnam(0);
	    ofstream os(gdb_init_file);
	    os << initial_cmds << "\n";
	}
    }

    // Build call
    string gdb_call = app_data.debugger_command;

    switch(type)
    {
    case GDB:
	gdb_call += " -q -fullname";
	if (gdb_init_file != "")
	    gdb_call += " -x " + gdb_init_file;
	break;

    case DBX:
	if (gdb_init_file != "")
	{
	    // When we specify a DBX init file, all default init
	    // files are overridden.  Specify them explicitly.
	    gdb_call += " -s .dbxrc -s $HOME/.dbxrc";
	    gdb_call += " -s .dbxinit -s $HOME/.dbxinit";
	    gdb_call += " -s " + gdb_init_file;
	}
	break;

    case XDB:
	gdb_call += " -L ";
	if (gdb_init_file != "")
	    gdb_call += " -p " + gdb_init_file;
	break;
    }

    for (int i = 1; i < argc; i++) {
	string arg = argv[i];
	gdb_call += " " + sh_quote(arg);
    }

    gdb_call = sh_command("exec " + gdb_call);

    return new GDBAgent(app_context, gdb_call, type);
}

// Cleanup
void remove_init_file()
{
    if (gdb_init_file != "")
    {
	if (remote_gdb())
	{
	    string cmd = sh_command(
		"rm -f " + gdb_init_file + " >/dev/null </dev/null 2>&1 &");
	    system(cmd);
	}
	else
	{
	    unlink((String)gdb_init_file);
	}
	gdb_init_file = "";
    }
}
