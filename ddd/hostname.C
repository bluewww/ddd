// $Id$ -*- C++ -*-
// Return `official' name of host

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

char hostname_rcsid[] = 
    "$Id$";

#include "hostname.h"
#include "config.h"
#include "Agent.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern "C" {
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#if defined(HAVE_GETHOSTNAME) && !defined(HAVE_GETHOSTNAME_DECL)
    int gethostname(char *name, int namelen);
#endif
#if defined(HAVE_UNAME) && !defined(HAVE_UNAME_DECL)
    int uname(struct utsname *name);
#endif
}

char *hostname()
{
    static char *name = 0;
    if (name)
	return name;

    char buffer[BUFSIZ];

    bool okay = false;

#ifdef HAVE_GETHOSTNAME
    if (gethostname(buffer, BUFSIZ) == 0)
    {
	okay = true;
    }
#endif
#ifdef HAVE_UNAME
    struct utsname un;
    if (!okay && uname(&un) == 0)
    {
	strcpy(buffer, un.nodename);
	okay = true;
    }
#endif
    if (!okay)
    {
	Agent agent("uname -n");
	agent.start();

	if (agent.inputfp())
	{
	    buffer[0] = '\0';
	    fscanf(agent.inputfp(), "%s", buffer);
	}
    }

    if (okay)
	return name = strcpy(new char[strlen(buffer) + 1], buffer);
    else
	return name = "unknown";
}

static char *_fullhostname()
{
    char *name = hostname();

#ifdef HAVE_GETHOSTBYNAME
    struct hostent *h = gethostbyname(name);
    if (h)
    {
	int i = 0;

	// Use first network address
	if (h->h_addrtype == AF_INET && h->h_addr_list[0] != 0)
	{
	    static char buffer[128];
	    buffer[0] = '\0';
	    for (int i = 0; i < h->h_length; i++)
		sprintf(buffer + strlen(buffer), i ? ".%d" : "%d",
			int((unsigned char)(h->h_addr_list[0][i])));

	    return buffer;
	}

	// Try qualified alias
	while (h->h_aliases[i] != 0)
	    if (strchr(h->h_aliases[i], '.'))
		return h->h_aliases[i];
    }
#endif

    return name;
}


char *fullhostname()
{
    static char *name = 0;
    if (name)
	return name;

    name = _fullhostname();
    return name = strcpy(new char[strlen(name) + 1], name);
}
