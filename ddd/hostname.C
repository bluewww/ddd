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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char hostname_rcsid[] = 
    "$Id$";

#include "hostname.h"
#include "config.h"

#ifndef NO_UNAME_AGENTS
#include "Agent.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern "C" {
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

// Get hostname
#if defined(HAVE_GETHOSTNAME)
#if !defined(HAVE_GETHOSTNAME_DECL)
    int gethostname(char *name, size_t size);
#endif
#elif defined(HAVE_UNAME)
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#if !defined(HAVE_UNAME_DECL)
    int uname(struct utsname *name);
#endif
#endif // defined(HAVE_UNAME)

// Get host aliases
#if defined(HAVE_GETHOSTBYNAME) && defined(HAVE_NETDB_H)
#include <netdb.h>

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef AF_INET
#define AF_INET 2		// internetwork: UDP, TCP, etc.
#endif

#endif // defined(HAVE_GETHOSTBYNAME) && defined(HAVE_NETDB_H)

}

// Return the host name
char *hostname()
{
    static char *name = 0;
    if (name)
	return name;

    char buffer[BUFSIZ];

    bool okay = false;

#if defined(HAVE_GETHOSTNAME)
    if (!okay && gethostname(buffer, BUFSIZ) == 0)
    {
	okay = true;
    }
#elif defined(HAVE_UNAME)
    struct utsname un;
    if (!okay && uname(&un) >= 0)
    {
	strcpy(buffer, un.nodename);
	okay = true;
    }
#endif

#ifndef NO_UNAME_AGENTS
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
#endif

    if (okay)
	return name = strcpy(new char[strlen(buffer) + 1], buffer);
    else
	return name = "unknown";
}

// Return the number of '.' in STR
inline int dots(const char *str)
{
    int dots = 0;
    while (*str != '\0')
	if (*str++ == '.')
	    dots++;

    return dots;
}

// Return most qualified name for the current host
static char *_fullhostname(char *most_qualified_host)
{
    // Try local name
    if (most_qualified_host == 0)
	most_qualified_host = hostname();

#ifdef HAVE_GETHOSTBYNAME
    struct hostent *h = gethostbyname(most_qualified_host);
    if (h)
    {
	// Try official name
	if (dots(h->h_name) > dots(most_qualified_host))
	    most_qualified_host = (char *)h->h_name;

	// Try aliases
	for (int i = 0; h->h_aliases[i] != 0; i++)
	    if (dots(h->h_aliases[i]) > dots(most_qualified_host))
		most_qualified_host = h->h_aliases[i];

	// Try network addresses
	if (h->h_addrtype == AF_INET)
	{
	    for (int j = 0; h->h_addr_list[j] != 0; j++)
	    {
		static char num_host[128];
		num_host[0] = '\0';
		for (int i = 0; i < h->h_length; i++)
		    sprintf(num_host + strlen(num_host), i ? ".%d" : "%d",
			    int((unsigned char)(h->h_addr_list[j][i])));

		if (dots(num_host) > dots(most_qualified_host))
		    most_qualified_host = num_host;
	    }
	}
    }
#endif

    // Return most qualified host name
    return most_qualified_host;
}


// Return and cache a fully qualified name for the current host
char *fullhostname(char *host)
{
    // Buffer for local host name
    static char *name = 0;

    if (name && host == 0)
	return name;

    char *n = _fullhostname(host);
    if (host == 0)
	return name = strcpy(new char[strlen(n) + 1], n);
    else
	return n;
}
