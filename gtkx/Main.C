// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

#include <gtkmm/main.h>
#include <GtkX/Main.h>
#include <GtkX/Window.h>

using namespace GtkX;

Main *GtkX::default_main_loop = NULL;

Main::Main(GtkX::Window *&toplevel, const char *classname, const char *sessid,
	   const char *const *fallback_rsc, int argc, char **argv)
{
    argc_ = argc;
    argv_ = (char **)calloc((argc+1), sizeof(char *));
    int i;
    for (i = 0; i < argc; i++) {
	argv_[i] = strdup(argv[i]);
    }
    argv_[argc] = NULL;

    // Note: the cast on ddd_fallback_resources is safe.
    main_ = new Gtk::Main(argc, argv);
    toplevel_ = toplevel = new Window(*this, "main", "main");
    default_main_loop = this;
}

Main::~Main(void)
{
    delete main_;
    int i;
    for (i = 0; i < argc_; i++) {
	free(argv_[i]);
    }
    free(argv_);
}

GtkX::Window *
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

