// $Id$ 
// Read and evaluate VSL library

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2003 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char vsl_rcsid[] = 
    "$Id$";

#include "config.h"
// #include "MemCheckD.h"

#include "assert.h"
#include <iostream>
#include "strclass.h"
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#if !HAVE_ATHENA
int main(void)
{
    std::cerr << "This program requires the Athena widget library.\n";
    return EXIT_FAILURE;
}
#else

#include <X11/Intrinsic.h>      // X Toolkit
#include <X11/Xlib.h>           // X Library
#include <X11/Xaw/Viewport.h>   // Viewport Widget
#include <X11/StringDefs.h>     // String Definitions
#include <X11/Xaw/Cardinals.h>  // Definition ZERO
#include "DocSpace.h"           // DocSpace Widget

#include "bool.h"
#include "ThemeVSLL.h"
#include "VSLDef.h"
#include "VSEFlags.h"

#include "Box.h"
#include "StringBox.h"
#include "ListBox.h"



// Graphics functions
static void Flush(Widget w)
{
    Display *display = XtDisplay(w);
    
    XFlush(display);
}

// Standard resources
static const _XtString fallback_resources[] = 
    { "*Viewport.width:    400", "*Viewport.height: 400", 0 };

// These should be part of ExposeCB...
static Box *thebox = 0;

// clock() function
#if !HAVE_CLOCK_DECL
extern "C" clock_t clock();
#endif

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif

// Redraw
static void redraw(Widget w, BoxRegion r, BoxRegion exposed)
{
    if (VSEFlags::verbose)
    {
	std::cout << "Redraw: " << exposed;
	std::cout.flush();
    }

    clock_t starttime = clock();
    thebox->draw(w, r, exposed);
    clock_t endtime = clock();

    if (VSEFlags::show_eval_time)
	std::cout << " (" << (endtime - starttime) * 1000 / CLOCKS_PER_SEC
	     << " ms)\n";

    Flush(w);
}

// Expose Callback
static void ExposeCB(Widget w, XtPointer, XtPointer call_data)
{
    // Set size
    Arg arglist[10];
    int a = 0;
    XtSetArg(arglist[a], XtNwidth,  thebox->size(X)); a++;
    XtSetArg(arglist[a], XtNheight, thebox->size(Y)); a++;
    XtSetValues(w, arglist, a);

    // Get region
    XExposeEvent* event = (XExposeEvent *)call_data;
    BoxPoint expose_origin = BoxPoint(event->x, event->y);
    BoxSize  expose_space  = BoxSize(event->width, event->height);
    BoxRegion exposed(expose_origin, expose_space);

    redraw(w, BoxRegion(BoxPoint(0,0), thebox->size()), exposed);
}

// When selecting: redraw all (for statistics)
static void SelectCB(Widget w, XtPointer, XtPointer)
{
    XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, false);

    redraw(w, 
	BoxRegion(BoxPoint(0,0), thebox->size()), 
	BoxRegion(BoxPoint(0,0), BoxSize(1000000, 1000000)));
}


static XtAppContext app_con;

// Callback when done
static void QuitCB(Widget, XtPointer, XtPointer)
{
    if (VSEFlags::verbose)
    {
	std::cout << "Quit!\n";
	std::cout.flush();
    }
    XtDestroyApplicationContext(app_con);

    // Clear box
    if (thebox) 
	thebox->unlink();

    // Check if we really deleted everything
    if (UniqueId::inUse() > 0)
	std::cerr << "Warning: " << UniqueId::inUse() << " IDs still in use\n";

    exit(EXIT_SUCCESS);
}

// Create argument list
static ListBox *vsl_args(int argc, char *argv[])
{
    ListBox *args = new ListBox;
    for (int i = 0; i < argc; i++)
    {
	Box *a = new StringBox(argv[i]);
	*args += a;
	a->unlink();
    }

    return args;
}

// Main VSL program
int main(int argc, char *argv[])
{
    // Set flags
    {
	// SGI CC wants this:
	const char** tmp_argv = (const char**)argv;
	VSEFlags::parse(argc, tmp_argv, "vsllib");
	argv = (char **)tmp_argv;
    }

    // Init toolkit
    Widget toplevel = XtAppInitialize(&app_con, "Vsl", 
				      (XrmOptionDescRec *)0, ZERO, 
				      &argc, argv, (char**)fallback_resources, 
				      ArgList(0), ZERO);

    // Create Viewport
    Arg arglist[10];        // Arguments
    int a = 0;              // Argument counter
    XtSetArg(arglist[a], ARGSTR(XtNallowHoriz), true); a++;
    XtSetArg(arglist[a], ARGSTR(XtNallowVert),  true); a++;
    Widget viewport = XtCreateManagedWidget("viewport", viewportWidgetClass, 
					    toplevel, arglist, a);

    // Create DocSpace
    a = 0;
    Widget docSpace = XtCreateManagedWidget("docSpace", docSpaceWidgetClass, 
					    viewport, arglist, a);
    XtAddCallback(docSpace, XtNcallback, SelectCB, 0);
    XtAddCallback(docSpace, XtNexposeCallback, ExposeCB, 0);
    XtAddCallback(docSpace, XtNquitCallback, QuitCB, 0);

    // Set font table
    StringBox::fontTable = new FontTable(XtDisplay(toplevel));

    // Fetch name
    string library_file = VSEFlags::library_file;
    if (argc > 1)
    {
	library_file = argv[1];
	if (library_file[0] == '-')
	{
	    std::cout << argv[0] << ": usage: " << argv[0] << " [options] "
		<< "VSLLIB [THEMES...]\n\n" << VSEFlags::explain();

	    exit(EXIT_FAILURE);
	}
    }

    // Create pic in THEBOX
    {
	// Read library
	long starttime = clock();
	ThemedVSLLib lib(library_file, VSEFlags::optimize_mode());
	long endtime = clock();

	assert(lib.OK());

	if (VSEFlags::show_optimizing_time)
	    std::cout << "\nRead & optimizing time: " 
		<< (endtime - starttime) / 1000 << " ms\n";

	// Build themes
	StringArray themes;
	for (int i = 2; i < argc; i++)
	    themes += argv[i];
	lib.set_theme_list(themes);
	assert(lib.OK());

	if (VSEFlags::assert_library_ok)
	    assert(lib.OK());

	if (VSEFlags::dump_library)
	    std::cout << lib;

	if (VSEFlags::dump_tree)
	    lib.dumpTree(std::cout);

	if (VSEFlags::suppress_eval)
	    return EXIT_SUCCESS;

	// Fetch last function def (typically "main")
	VSLDef *def = lib.lastdef();

	if (def == 0)
	{
	    std::cerr << argv[0] << ": cannot find last definition (sorry)\n";
	    return EXIT_FAILURE;
	}

	// Eval function
	ListBox *arg = vsl_args(argc, argv);

	starttime = clock();
	for (int loop = 1; loop < VSEFlags::loops; loop++)
	{
	    Box *result = (Box *)def->eval(arg);
	    lib.output(result);
	    result->unlink();
	}
	Box *result = (Box *)def->eval(arg);
	lib.output(result);
	endtime = clock();
	arg->unlink();

	// Show eval time
	if (VSEFlags::show_eval_time)
	    std::cout << "\nEvaluation time: " 
		<< (endtime - starttime) / 1000 << " ms\n";

	if (result && VSEFlags::dump_picture)
	    std::cout << "#!" << argv[0] << "\n#include <std.vsl>\n\nmain() -> "
		<< *result << ";\n";

	thebox = result;

	// Stack and library are destroyed upon leaving this block
    }

    if (thebox && !thebox->size().isValid())
    {
	std::cerr << argv[0] << ": result has no size (maybe list?)\n";
	thebox->unlink();
	thebox = 0;
    }

    if (thebox == 0)
    {
	std::cerr << argv[0] << ": evaluation failed (sorry)\n";
	return EXIT_FAILURE;
    }

    // Realize Widget
    XtRealizeWidget(toplevel);

    // Process events
    XtAppMainLoop(app_con);

    // Never reached...
    return EXIT_SUCCESS;
}

#endif // HAVE_ATHENA
