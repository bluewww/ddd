// $Id$ 
// Test program

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

char bug_rcsid[] = 
    "$Id$";

// includes von Motif-Zeugs
#include <Xm/Xm.h>

// includes von Nora-Zeugs
#include "strclass.h"
#include "assert.h"

// sonstige includes
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h> // fuer atoi
#include <string.h> // fuer strdup, strchr

const int BPIndent = 5;
const char *BPIndentText = "-----";

void read_file (const string file_name)
{
    if (file_name == "")
	return;

    char* filename = strdup ((const char*) file_name);

    /* make sure the file is a regular text file and open it */
    FILE *fp = 0;
    struct stat statb;

    if (stat(filename, &statb) == -1 ||
            (statb.st_mode & S_IFMT) != S_IFREG ||
            !(fp = fopen(filename, "r"))) {

        if ((statb.st_mode & S_IFMT) == S_IFREG) {
	    perror (filename);
	}
        else {
	    cerr << file_name << ": not a regular file" << '\n';
	}
        XtFree(filename);
        return;
    }

    /* put the contents of the file in the Text widget by allocating
     * enough space for the entire file, reading the file into the
     * allocated space, and using XmTextFieldSetString() to show the file.
     */
    char* text;
    if (!(text = XtMalloc((unsigned)(statb.st_size+1)))) {

	cerr << "Can't alloc enough space for " << file_name<< '\n';
        XtFree(filename);
        fclose(fp);
        return;
    }

    if (!fread(text, sizeof(char), statb.st_size+1, fp))
	cerr << "Warning: may not have read entire file!\n";

    text[statb.st_size] = 0; /* be sure to NULL-terminate */

    // Im Text Platz fuer bpoint-Informationen schaffen.
    int line_count = 0;
    char* index1 = text;
    char* index2 = 0;
    while (index2 = strchr (index1+1, '\n')) {
	assert (index1 < index2);
	index1 = index2;
	line_count++;
    }

    char* indented_text;
    if (!(indented_text =
	  XtMalloc((unsigned)(statb.st_size + 1 + BPIndent*line_count)))) {
	
	cerr << "Can't alloc enough space for " << file_name << '\n';
        XtFree(filename);
        fclose(fp);
        return;
    }
    int end_pos = 0;
    indented_text[end_pos] = 0;
     index1 = text;
    assert (index1 >= text);
    index2 = strchr (index1, '\n');
    assert (index2 >= text);
    assert (index1 <= index2);
    while (index2 != 0) {
	strcat (indented_text, BPIndentText);
 	end_pos += BPIndent;
	assert (indented_text[end_pos] == 0);
	strncat (indented_text, index1, index2 - index1);
	end_pos += (index2 - index1);
	indented_text[end_pos] = 0;
	index1 = index2;
	index2 = strchr (index1, '\n');
	assert (index2 >= text || (index2 == 0));
	assert ((index1 <= index2) || (index2 == 0));
    }
    assert (end_pos == statb.st_size + BPIndent*line_count);
    indented_text[statb.st_size + BPIndent*line_count] = 0;
    
    
    /* insert file contents in Text widget */
    cout << "Jetzt kommt Text\n";
    cout << indented_text;
    
    
    /* free all allocated space and we're outta here. */
    XtFree(text);
    XtFree(indented_text);
    XtFree(filename);
    fclose(fp);
    
}



int main (int argc, char* argv[])
{
    read_file (argv[2]);
    
}

