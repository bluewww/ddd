// $Id$ -*- C++ -*-
// DDD file functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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
#include "ExitCB.h"
#include "HelpCB.h"
#include "SourceView.h"
#include "VarArray.h"
#include "Command.h"
#include "cook.h"
#include "ddd.h"
#include "filetype.h"
#include "glob.h"
#include "mydialogs.h"
#include "post.h"
#include "question.h"
#include "regexps.h"
#include "shell.h"
#include "strclass.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#include <limits.h>
#include <string.h>		// strerror()
#include <errno.h>

#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

#if !HAVE_POPEN_DECL
extern "C" FILE *popen(const char *command, const char *mode);
#endif
#if !HAVE_PCLOSE_DECL
extern "C" int pclose(FILE *stream);
#endif


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// Don't rely on libiberty basename() because we don't want to depend
// on libiberty include files
static const char *file_basename(const char *name)
{
    const char *base = name;

    while (*name)
    {
	if (*name++ == '/')
	    base = name;
    }

    return base;
}

#define basename file_basename


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
			  FileSearchProc do_search_files = 0,
			  FileSearchProc do_search_dirs  = 0,
			  XtCallbackProc ok_callback     = 0)
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

    if (ok_callback != 0)
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
    {
	delay.outcome = "failed";
	return;
    }
    String dir;
    if (!XmStringGetLtoR(cbs->dir, MSTRING_DEFAULT_CHARSET, &dir))
    {
	delay.outcome = "failed";
	return;
    }

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
	delay.outcome = strerror(errno);
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
	    set_status(s, true);
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

	    int percent     = (i * 100) / count;
	    int old_percent = ((i - 1) * 100) / count;
	    if (percent % 10 == 0 && old_percent % 10 != 0)
	    {
		ostrstream status;
		status << delay_message << "... ("
		       << percent << "% processed)";
		string s(status);
		set_status(s, true);
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

// Get information on current debuggee
ProgramInfo::ProgramInfo()
    : file(NO_GDB_ANSWER),
      core(NO_GDB_ANSWER),
      pid(0),
      attached(false),
      running(false),
      state()
{
    if (source_view->have_exec_pos())
    {
	state = "has stopped";
	running = true;
    }
    else
    {
	state = "is not being run";
	running = false;
    }

    switch(gdb->type())
    {
    case GDB:
    {
	string ans = gdb_question("info files");
	if (ans == NO_GDB_ANSWER)
	    break;

	file = "";
	if (ans.contains("Symbols from "))
	{
	    file = ans.after("Symbols from ");
	    file = file.before(".\n");
	    file = unquote(file);
	}

	core = "";
	if (ans.contains("core dump"))
	{
	    core = ans.after("core dump");
	    core = core.after('`');
	    core = core.before("',");
	}

	if (ans.contains("process "))
	{
	    string p = ans.after("process ");
	    pid = atoi(p.chars());
	}

	attached = ans.contains("attached process ");

	ans = gdb_question("info program");
	if (ans == NO_GDB_ANSWER)
	    break;

	if (ans.contains("not being run"))
	    running = false;
	else if (ans.contains("\nIt stopped "))
	{
	    state = ans.from("\nIt stopped ");
	    state = "has " + state.after("\nIt ");
	    state = state.before('.');
	    running = true;
	}
	break;
    }

    case DBX:
    {
	string ans = gdb_question(gdb->debug_command());
	if (ans != NO_GDB_ANSWER)
	{
	    if (ans.contains("Debugging: ", 0))
		ans = ans.after(": ");

	    read_leading_blanks(ans);
	    strip_final_blanks(ans);
	    if (!ans.contains(' ')) // Sanity check
		file = ans;
	}
	break;
    }

    case XDB:
	break;			// FIXME
    }

    // As a fallback, get core file and executable from argument list.
    // Works only on local file system and is more a guess.
    char **argv = saved_argv();
    int argc = 0;
    while (argv[argc] != 0)
	argc++;

    // All debuggers supported by GDB have [EXEC [CORE]] as their last 
    // arguments.
    if ((file == NO_GDB_ANSWER || core == NO_GDB_ANSWER) && argc >= 2)
    {
	string last_file = cmd_file(argv[argc - 1]);
	if (is_exec_file(last_file))
	{
	    file = last_file;
	    core = "";
	}
	else if (argc >= 3)
	{
	    string prev_file = cmd_file(argv[argc - 2]);
	    if (is_exec_file(prev_file) && is_core_file(last_file))
	    {
		file = prev_file;
		core = last_file;
	    }
	}
    }
}




// OK pressed in `Open File'
static void openFileDone(Widget w, XtPointer client_data, XtPointer call_data)
{
    string filename = get_file(w, client_data, call_data);
    if (filename == "")
	return;

    XtUnmanageChild(w);

    if (filename == NO_GDB_ANSWER)
	return;

    if (gdb->type() == GDB)
    {
	// GDB does not always detach processes upon opening new
	// files, so we do it explicitly
	ProgramInfo info;
    	if (info.attached)
	    gdb_command("detach");
    }

    gdb_command(gdb->debug_command(filename));
}

// Process selection
static int ps_pid_index = 0;

// Retrieve PID from PS output
static int ps_pid(const string& line)
{
    const char *s = line.chars() + ps_pid_index;
    while (s > line.chars() && isdigit(s[-1]))
	--s;

    return atoi(s);
}

// Fill the pids in DISP_NRS
static void getPIDs(Widget selectionList, IntArray& disp_nrs)
{
    static IntArray empty;
    disp_nrs = empty;

    XmStringTable selected_items;
    int selected_items_count = 0;

    assert(XmIsList(selectionList));

    XtVaGetValues(selectionList,
		  XmNselectedItemCount, &selected_items_count,
		  XmNselectedItems, &selected_items,
		  NULL);

    for (int i = 0; i < selected_items_count; i++)
    {
	String _item;
	XmStringGetLtoR(selected_items[i], LIST_CHARSET, &_item);
	string item(_item);
	XtFree(_item);

	int p = ps_pid(item);
	if (p > 0)
	    disp_nrs += p;
    }
}

// Get the PID from the selection list in CLIENT_DATA
static int get_pid(Widget, XtPointer client_data, XtPointer)
{
    IntArray pids;
    Widget processes = Widget(client_data);
    if (processes != 0)
	getPIDs(processes, pids);

    if (pids.size() == 1)
	return pids[0];
    else
	return 0;
}

// OK pressed in `Open Core'
static void openCoreDone(Widget w, XtPointer client_data, XtPointer call_data)
{
    string corefile = get_file(w, client_data, call_data);
    if (corefile == "")
	return;

    ProgramInfo info;

    XtUnmanageChild(w);

    if (corefile != NO_GDB_ANSWER)
    {
	switch(gdb->type())
	{
	case GDB:
	    gdb_command("core-file " + corefile);
	    break;

	case DBX:
	    if (info.file != NO_GDB_ANSWER && info.file != "")
		gdb_command("debug " + info.file + " " + info.core);
	    else
		post_error("No program.", "no_program", w);
	    break;

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
static Widget create_file_dialog(Widget w, String name,
				 FileSearchProc searchRemoteFiles       = 0,
				 FileSearchProc searchRemoteDirectories = 0,
				 FileSearchProc searchLocalFiles        = 0,
				 FileSearchProc searchLocalDirectories  = 0,
				 XtCallbackProc openDone = 0)
{			     
    if (remote_gdb())
	return file_dialog(find_shell(w), name,
			   searchRemoteFiles, searchRemoteDirectories, 
			   openDone);
    else
	return file_dialog(find_shell(w), name,
			   searchLocalFiles, searchLocalDirectories, 
			   openDone);
}

// Process selection

static void sort(StringArray& a)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= a.size());
    do {
	h /= 3;
	for (int i = h; i < a.size(); i++)
	{
	    string v = a[i];
	    int j;
	    for (j = i; j >= h && ps_pid(a[j - h]) > ps_pid(v); j -= h)
		a[j] = a[j - h];
	    if (i != j)
		a[j] = v;
	}
    } while (h != 1);
}


inline bool is_separator(char c)
{
    return c == ' ' || c == '\'' || c == '\"';
}

// Check whether LINE is a valid PS line.  Exclude occurrences of PS_COMMAND.
static bool valid_ps_line(const string& line, const string& ps_command)
{
    int pid = ps_pid(line);
    if (pid == 0)
	return false;		// No PID

    // You don't want to debug DDD, don't you?
    if (!remote_gdb() && pid == getpid())
	return false;

    // Neither should you debug GDB by itself.
    if (pid == gdb->pid())
	return false;

    // Don't issue lines containing `ps' (or whatever the first word
    // in PS_COMMAND is).
    string ps = ps_command;
    if (ps.contains(' '))
	ps = ps.before(' ');
    ps = basename(ps);
    int index = line.index(ps);
    if (index > 0
	&& (line[index - 1] == '/' || is_separator(line[index - 1]))
	&& (line.length() == index + ps.length()
	    || is_separator(line[index + ps.length()])))
	return false;

    // Okay, just leave it
    return true;
}


// Create list of processes
static void update_processes(Widget processes, bool keep_selection)
{
    StatusDelay delay("Getting list of processes");

    string cmd = sh_command(app_data.ps_command) + " 2>&1";
    FILE *fp = popen(cmd.chars(), "r");
    if (fp == 0)
    {
	delay.outcome = strerror(errno);
	return;
    }

    StringArray all_process_list;
    int c;
    string line = "";
    bool first_line = true;

    while ((c = getc(fp)) != EOF)
    {
	if (c == '\n')
	{
	    if (first_line || valid_ps_line(line, app_data.ps_command))
		all_process_list += line;
#if 0
	    else
		clog << "Excluded: " << line << "\n";
#endif

	    if (first_line)
	    {
		// Find first occurrence of `PID' title
		ps_pid_index = line.index(" PID ");
		if (ps_pid_index < 0)
		    ps_pid_index = 0;
	    }

	    line = "";
	    first_line = false;
	}
	else
	{
	    line += c;
	}
    }

    pclose(fp);
    sort(all_process_list);
    DynIntArray pids(all_process_list.size());

    // If GDB cannot send a signal to the process, we cannot debug it.
    // Try a `kill -0' (via GDB, as it may be setuid) and filter out
    // all processes in the `kill' diagnostic -- that is, all
    // processes that `kill' could not send a signal.
    string kill = "kill -0";
    int i;
    for (i = 0; i < all_process_list.size(); i++)
    {
	pids[i] = ps_pid(all_process_list[i]);
	if (pids[i])
	    kill += string(" ") + itostring(pids[i]);
    }
    string kill_result = gdb_question(gdb->shell_command(kill));
    i = 0;
    while (i >= 0)
    {
	i = kill_result.index(rxint, i);
	if (i >= 0)
	{
	    int bad_pid = atoi((char *)kill_result + i);
	    for (int k = 0; k < all_process_list.size(); k++)
	    {
		if (pids[k] != 0 && pids[k] == bad_pid)
		{
#if 0
		    clog << "Excluded: " << all_process_list[k] << "\n";
#endif
		    all_process_list[k] = NO_GDB_ANSWER;
		}
	    }
	    i++;
	}
    }

    StringArray process_list;
    for (i = 0; i < all_process_list.size(); i++)
	if (all_process_list[i] != NO_GDB_ANSWER)
	    process_list += all_process_list[i];

    // Now set the selection.
    bool *selected = new bool[process_list.size()];
    for (i = 0; i < process_list.size(); i++)
	selected[i] = false;

    int pos = -1;
    if (keep_selection)
    {
	// Preserve old selection: each PID selected before will also be
	// selected after.
	IntArray selection;
	getPIDs(processes, selection);

	for (i = 0; i < selection.size(); i++)
	{
	    for (int j = 0; j < process_list.size(); j++)
		if (selection[i] == ps_pid(process_list[j]))
		{
		    if (pos < 0)
			pos = j;
		    selected[j] = true;
		}
	}
    }

    if (pos < 0)
    {
	// Create new selection from current file and current pid.
	ProgramInfo info;

	// Check for current pid; if found, highlight it.
	for (i = 0; pos < 0 && i < process_list.size(); i++)
	{
	    if (info.pid != 0 && ps_pid(process_list[i]) == info.pid)
		pos = i;
	}

	if (pos < 0)
	{
	    // Not found? Try leftmost occurrence of process base name.
	    string current_base = basename(info.file.chars());
	    int leftmost = INT_MAX;
	    for (i = 0; i < process_list.size(); i++)
	    {
		int occurrence = process_list[i].index(current_base);
		if (occurrence >= 0 && occurrence < leftmost 
		    && ps_pid(process_list[i]) > 0)
		{
		    leftmost = occurrence;
		    pos = i;
		}
	    }
	}
    }

    if (pos >= 0)
	selected[pos] = true;

    setLabelList(processes, process_list.values(),
		 selected, process_list.size(), true, false);

    if (pos >= 0)
	ListSetAndSelectPos(processes, pos + 1);

    delete[] selected;
}


static void gdbUpdateProcessesCB(Widget, XtPointer client_data, XtPointer)
{
    Widget processes = Widget(client_data);
    update_processes(processes, true);
}

// Set program arguments from list
static void SelectProcessCB(Widget w, XtPointer client_data, 
			    XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position;
    if (pos == 1)
	XmListDeselectAllItems(w); // Title selected
    else
	ListSetAndSelectPos(w, pos);

    int pid = get_pid(w, client_data, call_data);
    if (pid <= 0)
	set_status("");
    else
	set_status("Process " + itostring(pid));
}


// OK pressed in `Open Process'
static void openProcessDone(Widget w, XtPointer client_data, 
			    XtPointer call_data)
{
    int pid = get_pid(w, client_data, call_data);
    if (pid <= 0)
    {
	gdbUpdateProcessesCB(w, client_data, call_data);	
	return;
    }

    XtUnmanageChild(w);

    ProgramInfo info;

    if (pid == info.pid)
    {
	set_status("Already attached to process " + itostring(pid) + ".");
	return;
    }

    switch(gdb->type())
    {
    case GDB:
	// GDB does not always detach processes upon opening new
	// files, so we do it explicitly
	if (info.attached)
	    gdb_command("detach");

	// Attach to new process
	gdb_command("attach " + itostring(pid));
	break;
	
    case DBX:
	// Attach to new process
	if (info.file != NO_GDB_ANSWER && info.file != "")
	    gdb_command("debug " + info.file + " " + itostring(pid));
	else
	    post_error("No program.", "no_program", w);
	break;

    case XDB:
	break;		// FIXME
    }
}

// When W is to be destroyed, remove all references in Widget(CLIENT_DATA)
static void RemoveCallbacksCB(Widget w, XtPointer client_data, XtPointer)
{
    Widget ref = Widget(client_data);
    XtRemoveCallback(ref, XmNokCallback,      UnmanageThisCB, XtPointer(w));
    XtRemoveCallback(ref, XmNcancelCallback,  UnmanageThisCB, XtPointer(w));
    XtRemoveCallback(ref, XmNdestroyCallback, RemoveCallbacksCB, XtPointer(w));
}

// If we don't have a current executable, issue a warning.
static void warn_if_no_program(Widget popdown)
{
    ProgramInfo info;

    if (info.file == "")
    {
	Widget warning = post_warning("Please open a program first.", 
				      "no_program", popdown);

	if (popdown != 0 && warning != 0)
	{
	    // Tie the warning to the dialog - if one is popped down,
	    // so is the other.
	    XtAddCallback(warning, XmNokCallback, 
			  UnmanageThisCB, XtPointer(popdown));
	    XtAddCallback(warning, XmNcancelCallback, 
			  UnmanageThisCB, XtPointer(popdown));
	    XtAddCallback(popdown, XmNdestroyCallback,
			  RemoveCallbacksCB, XtPointer(warning));

	    XtAddCallback(popdown, XmNokCallback,
			  UnmanageThisCB, XtPointer(warning));
	    XtAddCallback(popdown, XmNcancelCallback,
			  UnmanageThisCB, XtPointer(warning));
	    XtAddCallback(warning, XmNdestroyCallback,
			  RemoveCallbacksCB, XtPointer(popdown));
	}
    }
}


// Entry points

void gdbOpenFileCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 
	create_file_dialog(w, "exec_files", 
			   searchRemoteExecFiles, 
			   searchRemoteDirectories,
			   searchLocalExecFiles, 0,
			   openFileDone);
    manage_and_raise(dialog);
}

void gdbOpenCoreCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 
	create_file_dialog(w, "core_files", 
			   searchRemoteCoreFiles, searchRemoteDirectories,
			   searchLocalCoreFiles, 0,
			   openCoreDone);
    manage_and_raise(dialog);
    warn_if_no_program(dialog);
}

void gdbOpenSourceCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 
	create_file_dialog(w, "source_files", 
			   searchRemoteSourceFiles, searchRemoteDirectories,
			   searchLocalSourceFiles, 0,
			   openSourceDone);
    manage_and_raise(dialog);
    warn_if_no_program(dialog);
}

void gdbOpenProcessCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    static Widget processes = 0;

    if (dialog == 0)
    {
	Arg args[10];
	int arg = 0;
    
	XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
	dialog = verify(XmCreateSelectionDialog(find_shell(w), 
						"processes", args, arg));

	Delay::register_shell(dialog);

	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_SELECTION_LABEL));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_TEXT));

	processes = XmSelectionBoxGetChild(dialog, XmDIALOG_LIST);

	XtAddCallback(processes, XmNsingleSelectionCallback,
		      SelectProcessCB, XtPointer(processes));
	XtAddCallback(processes, XmNmultipleSelectionCallback,
		      SelectProcessCB, XtPointer(processes));
	XtAddCallback(processes, XmNextendedSelectionCallback,
		      SelectProcessCB, XtPointer(processes));
	XtAddCallback(processes, XmNbrowseSelectionCallback,
		      SelectProcessCB, XtPointer(processes));

	XtAddCallback(dialog, XmNokCallback, 
		      openProcessDone, XtPointer(processes));
	XtAddCallback(dialog, XmNapplyCallback, 
		      gdbUpdateProcessesCB, XtPointer(processes));
	XtAddCallback(dialog, XmNcancelCallback, 
		      UnmanageThisCB, XtPointer(dialog));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, 0);
    }

    update_processes(processes, false);
    manage_and_raise(dialog);
    warn_if_no_program(dialog);
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
