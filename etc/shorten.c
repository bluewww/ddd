/* $Id$ */
/* Convert long filenames to short ones, according to OSF/Motif convention */

/*
    Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
    Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.

    This file is part of ICE.

    ICE is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    ICE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with ICE -- see the file COPYING.
    If not, write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    ICE is the incremental configuration environment.
    For details, see the ICE World-Wide-Web page, 
    `http://www.cs.tu-bs.de/softech/ice/',
    or send a mail to the ICE developers at `ice@ips.cs.tu-bs.de'.
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

extern int puts();

static char *shorten(s, length)
    char *s;
    int length;
{
    static char ret[BUFSIZ];

    char abbrev[BUFSIZ + 1];
    char trailer[BUFSIZ];

    int i, j;
    int a = 0;
    int t = 0;

    /* if length is enough, return */
    if (strlen(s) <= length)
	return s;

    /* find trailer */
    for (i = 0; s[i] != '.' && s[i] != '\0'; i++)
	;

    /* fetch trailer */
    strcpy(trailer, s + i);
    t = strlen(trailer);

    a = 0;
    abbrev[BUFSIZ] = '\0';

    /* fetch abbrev */
    for (; i > 0 && i + a + t >= length; i--)
	if (isupper(s[i]))
	    abbrev[BUFSIZ - ++a] = s[i];

    /* cut after upper-case-letter, if it's not the first */
    for (j = i; j > 0 && !isupper(s[j]); j--)
	;
    if (j > 0)
	i = j;

    /* echo it all */
    s[i + 1] = '\0';
    strcpy(ret, s);
    strcat(ret, abbrev + BUFSIZ - a);
	ret[length - t] = '\0';
    strcat(ret, trailer);
	ret[length] = '\0';

    return ret;
}


int main(argc, argv)
    int argc;
    char *argv[];
{
    char s[BUFSIZ];
    int length = 12; /* system V convention */

    if (argc > 1)
	length = atoi(argv[1]);

    while (gets(s) != NULL)
	puts(shorten(s, length));

    return 0;
}
