// $Id$ -*- C++ -*-
// DDD file functions

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char file_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "DynArray"
#pragma implementation "VarArray"
#endif

#include "file.h"

#include "AppData.h"
#include "GDBAgent.h"
#include "MString.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "SourceView.h"
#include "VarArray.h"
#include "commandQ.h"
#include "ddd.h"
#include "filetype.h"
#include "glob.h"
#include "post.h"
#include "question.h"
#include "shell.h"
#include "strclass.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

//-----------------------------------------------------------------------------
// Opening files
//-----------------------------------------------------------------------------

typedef void (*FileSearchProc)(Widget fs, 
			       XmFileSelectionBoxCallbackStruct *cbs);

static WidgetArray file_filters;
static WidgetArray file_dialogs;

static string current_file_filter = "";

// Make sure that every change in one filter is reflected in all others
static void SyncFiltersCB(Widget dialog, XtPointer, XtPointer)
{
    static bool entered = false;

    if (entered)
	return;

    entered = true;

    // clog << "widget = " << longName(text) << "\n";

    while (dialog != 0 && !XmIsFileSelectionBox(dialog))
	dialog = XtParent(dialog);
	
    // clog << "dialog = " << longName(dialog) << "\n";

    Widget text = XmFileSelectionBoxGetChild(dialog, XmDIALOG_FILTER_TEXT);
    String _current_file_filter = XmTextGetString(text);
    current_file_filter = _current_file_filter;
    XtFree(_current_file_filter);

    for (int i = 0; i < file_filters.size(); i++)
    {
	if (file_dialogs[i] != dialog)
	{
	    // clog << "other dialog = " << longName(file_dialogs[i]) << "\n";
	    XmTextSetString(file_filters[i], current_file_filter);
	}
    }

    entered = false;
}

// Make sure that every new filter call is performed in all other
// dialogs as well
static void FilterAllCB(Widget dialog, XtPointer client_data, 
			XtPointer call_data)
{
    SyncFiltersCB(dialog, client_data, call_data);

    // clog << "widget = " << longName(dialog) << "\n";

    while (dialog != 0 && !XmIsFileSelectionBox(dialog))
	dialog = XtParent(dialog);
	
    // clog << "dialog = " << longName(dialog) << "\n";

    for (int i = 0; i < file_dialogs.size(); i++)
    {
	if (file_dialogs[i] != dialog)
	{
	    // clog << "other dialog = " << longName(file_dialogs[i]) << "\n";
	    XmFileSelectionDoSearch(file_dialogs[i], NULL);
	}
    }
}

// Create a file dialog NAME with DO_SEARCH_FILES and DO_SEARCH_DIRS
// as search procedures for files and directories, respectively, and
// OK_CALLBACK as the procedure called when a file is selected.
static Widget file_dialog(Widget w, const string& name,
			  FileSearchProc do_search_files,
			  FileSearchProc do_search_dirs,
			  XtCallbackProc ok_callback)
{
    Delay delay(w);

    Arg args[10];
    int arg = 0;

    string pwd;

    arg = 0;
    if (do_search_files)
    {
	XtSetArg(args[arg], XmNfileSearchProc, do_search_files); arg++;
    }
    if (do_search_dirs)
    {
	XtSetArg(args[arg], XmNdirSearchProc, do_search_dirs); arg++;
    }

    if (remote_gdb())
    {
	static MString xmpwd;
	xmpwd = SourceView::pwd();
	XtSetArg(args[arg], XmNdirectory, xmpwd.xmstring()); arg++;
    }

    Widget dialog = 
	verify(XmCreateFileSelectionDialog(w, name, args, arg));
    Delay::register_shell(dialog);
    XtAddCallback(dialog, XmNokCallback,     ok_callback, 0);
    XtAddCallback(dialog, XmNcancelCallback, UnmanageThisCB, 
		  XtPointer(dialog));
    XtAddCallback(dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    Widget filter = XmFileSelectionBoxGetChild(dialog, XmDIALOG_FILTER_TEXT);
    file_filters += filter;
    if (current_file_filter != "")
	XmTextSetString(filter, current_file_filter);
    XtAddCallback(filter, XmNvalueChangedCallback, SyncFiltersCB, 0);

    Widget filter_button = 
	XmFileSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON);
    XtAddCallback(filter_button, XmNactivateCallback, FilterAllCB, 0);

    file_dialogs += dialog;

    return dialog;
}

static char delay_message[] = "Filtering files";

// Search for remote files and directories, using the command CMD
static void searchRemote(Widget fs,
			 XmFileSelectionBoxCallbackStruct *cbs,
			 String cmd,
			 bool search_dirs)
{
    StatusDelay delay(delay_message);

    int nitems = 0;
    int size = 256;
    XmStringTable items = 
	XmStringTable(XtMalloc(size * sizeof(XmString)));

    String mask;
    if (!XmStringGetLtoR(cbs->mask, MSTRING_DEFAULT_CHARSET, &mask))
	return;
    String dir;
    if (!XmStringGetLtoR(cbs->dir, MSTRING_DEFAULT_CHARSET, &dir))
	return;

    if (search_dirs)
    {
	string extra_dir = string(dir) + ".";
	items[nitems++] = 
	    XmStringCreateLtoR(extra_dir, MSTRING_DEFAULT_CHARSET);
	extra_dir = string(dir) + "..";
	items[nitems++] = 
	    XmStringCreateLtoR(extra_dir, MSTRING_DEFAULT_CHARSET);
    }

    string command = cmd;
    command.gsub("@MASK@", mask);
    command = sh_command(command);

    Agent search(command);
    search.start();

    FILE *fp = search.inputfp();
    if (fp == 0)
    {
	perror(command);
	return;
    }

    char buf[BUFSIZ];
    while (fgets(buf, sizeof(buf), fp))
    {
	if (buf[0] && buf[strlen(buf) - 1] == '\n')
	    buf[strlen(buf) - 1] = '\0';

	if (nitems >= size)
	{
	    size += 256;
	    items = XmStringTable(XtRealloc((char *)items,
					    size * sizeof(XmString)));
	}
	    
	items[nitems++] = XmStringCreateLtoR(buf, MSTRING_DEFAULT_CHARSET);

	if (nitems == 1 || nitems % 10 == 0)
	{
	    ostrstream status;
	    status << delay_message << "... ("
		   << nitems << " processed)";
	    string s(status);
	    set_status(s);
	}
    }

    if (search_dirs)
    {
	XtVaSetValues(fs,
		      XmNdirListItems,     items,
		      XmNdirListItemCount, nitems,
		      XmNdirectoryValid,   True,
		      XmNlistUpdated,      True,
		      NULL);
    }
    else
    {
	if (nitems > 0)
	{
	    XtVaSetValues(fs,
			  XmNfileListItems,     items,
			  XmNfileListItemCount, nitems,
			  XmNdirSpec,           items[0],
			  XmNlistUpdated,       True,
			  NULL);
	}
	else
	{
	    XtVaSetValues(fs,
			  XmNfileListItems,     0,
			  XmNfileListItemCount, 0,
			  XmNlistUpdated,       True,
			  NULL);
	}
    }
}


static void searchRemoteExecFiles(Widget fs,
				  XmFileSelectionBoxCallbackStruct *cbs)
{
    searchRemote(fs, cbs, app_data.list_exec_command, false);
}

static void searchRemoteCoreFiles(Widget fs,
				  XmFileSelectionBoxCallbackStruct *cbs)
{
    searchRemote(fs, cbs, app_data.list_core_command, false);
}

static void searchRemoteSourceFiles(Widget fs,
				    XmFileSelectionBoxCallbackStruct *cbs)
{
    searchRemote(fs, cbs, app_data.list_source_command, false);
}

static void searchRemoteDirectories(Widget fs,
				    XmFileSelectionBoxCallbackStruct *cbs)
{
    searchRemote(fs, cbs, app_data.list_dir_command, true);
}

static void sort(char *a[], int size)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= size);
    do {
	h /= 3;
	for (int i = h; i < size; i++)
	{
	    char *v = a[i];
	    int j;
	    for (j = i; j >= h && strcmp(a[j - h], v) > 0; j -= h)
		a[j] = a[j - h];
	    if (i != j)
		a[j] = v;
	}
    } while (h != 1);
}

// Search for local files and directories, using the predicate IS_OKAY
static void searchLocal(Widget fs,
			XmFileSelectionBoxCallbackStruct *cbs,
			bool is_okay(const string& file_name))
{
    String mask;
    if (!XmStringGetLtoR(cbs->mask, MSTRING_DEFAULT_CHARSET, &mask))
	return;

    char **files = glob_filename(mask);
    if (files == (char **)0)
    {
	cerr << mask << ": glob failed\n";
    }
    else if (files == (char **)-1)
    {
	post_error(string(mask) + ": " + strerror(errno));
    }
    else
    {
	StatusDelay delay(delay_message);

	int count;
	for (count = 0; files[count] != 0; count++)
	    ;
	sort(files, count);

	XmStringTable items = 
	    XmStringTable(XtMalloc(count * sizeof(XmString)));

	int nitems = 0;
	for (int i = 0; files[i] != 0; i++)
	{
	    if (is_okay(files[i]))
		items[nitems++] = XmStringCreateLtoR(files[i], 
						     MSTRING_DEFAULT_CHARSET);
	    free(files[i]);

	    int percent     = i * 100 / count;
	    int old_percent = (i - 1) * 100 / count;
	    if (nitems == 1 || (percent % 10 == 0 && old_percent % 10 != 0))
	    {
		ostrstream status;
		status << delay_message << "... ("
		       << percent << "% processed)";
		string s(status);
		set_status(s);
	    }
	}
	free((char *)files);

	if (nitems > 0)
	{
	    XtVaSetValues(fs,
			  XmNfileListItems,     items,
			  XmNfileListItemCount, nitems,
			  XmNdirSpec,           items[0],
			  XmNlistUpdated,       True,
			  NULL);

	    XtFree((char *)items);
	    return;
	}
    }

    // Error or nothing found 
    XtVaSetValues(fs,
		  XmNfileListItems,     0,
		  XmNfileListItemCount, 0,
		  XmNlistUpdated,       True,
		  NULL);
}

static void searchLocalExecFiles(Widget fs,
				 XmFileSelectionBoxCallbackStruct *cbs)
{
    searchLocal(fs, cbs, is_exec_file);
}

static void searchLocalCoreFiles(Widget fs,
				 XmFileSelectionBoxCallbackStruct *cbs)
{
    searchLocal(fs, cbs, is_core_file);
}

static void searchLocalSourceFiles(Widget fs,
				   XmFileSelectionBoxCallbackStruct *cbs)
{
    searchLocal(fs, cbs, is_source_file);
}


// Get the file name from the file selection box W
static string get_file(Widget w, XtPointer, XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct *cbs = 
	(XmFileSelectionBoxCallbackStruct *)call_data;

    String s;
    if (!XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &s))
	return NO_GDB_ANSWER;

    string filename = s;
    XtFree(s);

    if (filename == "" || filename[0] != '/')
    {
	String dir;
	if (!XmStringGetLtoR(cbs->dir, MSTRING_DEFAULT_CHARSET, &dir))
	    return NO_GDB_ANSWER;

	filename = string(dir) + "/" + filename;
	XtFree(dir);
    }

    if (is_directory(filename))
    {
	MString filter(filename);
	XmFileSelectionDoSearch(w, filter.xmstring());
	return "";
    }

    return filename;
}

// OK pressed in `Open File'
static void openFileDone(Widget w, XtPointer client_data, XtPointer call_data)
{
    string filename = get_file(w, client_data, call_data);
    if (filename == "")
	return;

    XtUnmanageChild(w);

    if (filename != NO_GDB_ANSWER)
    {
	switch(gdb->type())
	{
	case GDB:
	    gdb_command("file " + filename);
	    break;

	case DBX:
	    gdb_command("debug " + filename);
	    break;

	case XDB:
	    break;		// FIXME
	}
    }
}

// OK pressed in `Open Core'
static void openCoreDone(Widget w, XtPointer client_data, XtPointer call_data)
{
    string corefile = get_file(w, client_data, call_data);
    if (corefile == "")
	return;

    XtUnmanageChild(w);

    if (corefile != NO_GDB_ANSWER)
    {
	switch(gdb->type())
	{
	case GDB:
	    gdb_command("core-file " + corefile);
	    break;

	case DBX:
	    {
		string file = gdb_question("debug");
		if (file == NO_GDB_ANSWER)
		    post_gdb_busy();
		else if (file.contains("Debugging: ", 0))
		{
		    file = file.after(": ");
		    strip_final_blanks(file);
		    gdb_command("debug " + file + " " + corefile);
		}
		else
		    post_gdb_message(file);
	    }

	case XDB:
	    break;		// FIXME
	}
    }
}

// OK pressed in `Open Source'
static void openSourceDone(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
    string filename = get_file(w, client_data, call_data);
    if (filename == "")
	return;

    XtUnmanageChild(w);

    if (filename != NO_GDB_ANSWER)
	source_view->read_file(filename);
}


// Create various file dialogs

void gdbOpenFileCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	if (remote_gdb())
	    dialog = file_dialog(w, "exec_files", searchRemoteExecFiles, 
				 searchRemoteDirectories, openFileDone);
	else
	    dialog = file_dialog(w, "exec_files", searchLocalExecFiles, 
				 0, openFileDone);
    }

    XtManageChild(dialog);
    raise_shell(dialog);
}

void gdbOpenCoreCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	if (remote_gdb())
	    dialog = file_dialog(w, "core_files", searchRemoteCoreFiles, 
				 searchRemoteDirectories, openCoreDone);
	else
	    dialog = file_dialog(w, "core_files", searchLocalCoreFiles, 
				 0, openCoreDone);
    }

    XtManageChild(dialog);
    raise_shell(dialog);
}

void gdbOpenSourceCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	if (remote_gdb())
	    dialog = file_dialog(w, "source_files", searchRemoteSourceFiles, 
				 searchRemoteDirectories, openSourceDone);
	else
	    dialog = file_dialog(w, "source_files", searchLocalSourceFiles, 
				 0, openSourceDone);
    }

    XtManageChild(dialog);
    raise_shell(dialog);
}

// Synchronize file dialogs with current directory
void process_cd(string pwd)
{
    current_file_filter = pwd + "/*";

    for (int i = 0; i < file_filters.size(); i++)
    {
	if (file_filters[i] != 0)
	{
	    XmTextSetString(file_filters[i], current_file_filter);
	    break;
	}
    }
}
