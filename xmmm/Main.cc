// High-level GUI wrapper for Xm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

#include <malloc.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xmmm/Main.h>
#include <Xmmm/Window.h>

using namespace Xmmm;

Main::Main(Xmmm::Window *&toplevel, const char *classname, const char *sessid,
	   const char *const *fallback_rsc, int argc, char **argv)
{
    argc_ = argc;
    argv_ = (char **)calloc((argc+1), sizeof(char *));
    int i;
    for (i = 0; i < argc; i++) {
	argv_[i] = strdup(argv[i]);
    }
    argv_[argc] = NULL;

    Arg args[1];
    XtSetArg(args[0], XtNsessionID, sessid);

    // Note: the cast on ddd_fallback_resources is safe.
    ::Widget top =
	  XtOpenApplication(&appctx_, classname,
			    XrmOptionDescList(0), 0,
			    &argc, argv, (::String *)fallback_rsc,
			    sessionShellWidgetClass,
			    args, 1);
    toplevel_ = toplevel = new Xmmm::Window(top);
}

Main::~Main(void)
{
    int i;
    for (i = 0; i < argc_; i++) {
	free(argv_[i]);
    }
    free(argv_);
}

Xmmm::Window *
Main::get_toplevel(void) const
{
    return toplevel_;
}

int
Main::get_argc(void) const
{
}

char *
Main::get_argv(int i) const
{
}

