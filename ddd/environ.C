// $Id$ -*- C++ -*-
// Environment commands

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char environ_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "environ.h"
#include "StringA.h"
#include "bool.h"
#include "shell.h"

#include <stdlib.h>		// getenv, putenv
#include <string.h>

static StringArray environment_names;

// Put NAME=VALUE into the environment
extern void put_environment(string name, string value)
{
    string env_s = name + "=" + value;
    char *env = new char[env_s.length() + 1];
    strcpy(env, env_s);
    putenv(env);

    bool found = false;
    for (int i = 0; i < environment_names.size() && !found; i++)
	found = found || (environment_names[i] == name);

    if (!found)
	environment_names += name;
}

// Return NAME1=VALUE1 NAME2=VALUE2 for each name defined by PUT_ENVIRONMENT
extern string set_environment_command()
{
    string cmd;

    for (int i = 0; i < environment_names.size(); i++)
    {
	const string& name = environment_names[i];
	char *env = getenv((char *)name);
	if (env != 0)
	    cmd += name + "=" + sh_quote(env) + "; export " + name + "; ";
    }

    return cmd;
}
