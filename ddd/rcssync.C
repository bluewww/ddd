// $Id$ -*- C++ -*-
// List recently changed RCS files

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the ICE Library.
// 
// The ICE Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The ICE Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the ICE Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// ICE is the incremental configuration engine.
// Contact ice@ips.cs.tu-bs.de for details.

char rcssync_rcsid[] =
    "$Id$";

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
}

// For all files in args, check if the corresponding RCS file is newer.
// If so, print the name of the RCS file on standard output.
// Thus, you may use co `rcssync *.C` to check out all recent .C files.
int main(int argc, char *argv[])
{
    int errors = 0;

    for (int i = 1; i < argc; i++)
    {
	int checkout = 0;

	char file[BUFSIZ];
	char rcsfile[BUFSIZ];
	strcpy(file, argv[i]);
	strcpy(rcsfile, argv[i]);

	int j = strlen(rcsfile);
	while (j >= 0 && rcsfile[j] == '/')
	    j--;
	while (j >= 0 && rcsfile[j] != '/')
	    j--;
	if (j < 0)
	{
	    char buf[BUFSIZ];
	    strcpy(buf, "RCS/");
	    strcat(buf, rcsfile);
	    strcpy(rcsfile, buf);
	}
	else
	{
	    char buf[BUFSIZ];
	    strncpy(buf, rcsfile, j + 1);
	    buf[j + 1] = '\0';
	    strcat(buf, "RCS");
	    strcat(buf, rcsfile + j);
	    strcpy(rcsfile, buf);
	}
	strcat(rcsfile, ",v");

	// printf("Checking: %s vs. %s\n", file, rcsfile);

	struct stat filestat;
	int ret = stat(file, &filestat);
	if (ret != 0)
	{
	    perror(file);
	    checkout = 1;
	}
	else
	{
	    struct stat rcsstat;
	    ret = stat(rcsfile, &rcsstat);
	    if (ret != 0)
	    {
		perror(rcsfile);
		errors++;
		continue;
	    }

	    checkout = rcsstat.st_mtime > filestat.st_mtime;
	}

	if (checkout)
	    printf("%s\n", rcsfile);
    }

    return errors > 0;
}
