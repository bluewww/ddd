// includes von Motif-Zeugs
extern "C" {
#include <Xm/Xm.h>
}

// includes von Nora-Zeugs
#include "strclass.h"
#include "assert.h"

// sonstige includes
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h> // fuer atoi
extern "C" {
#include <string.h> // fuer strdup, strchr
}

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

