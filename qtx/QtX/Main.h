// -*- C++ -*-

// High-level GUI wrapper for Qt.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of QtX.
// 
// QtX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// QtX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with QtX -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef QTX_MAIN_H
#define QTX_MAIN_H

#include <qapplication.h>

namespace QtX {

    class Window;

    class Main {
	QApplication *main_;
	QtX::Window *toplevel_;
	int argc_;
	char **argv_;
    public:
	Main(QtX::Window *&toplevel, const char *classname, const char *sessid,
	     const char *const *fallback_rsc, int argc, char **argv);
	~Main(void);
	QtX::Window *get_toplevel(void) const;
	int get_argc(void) const;
	char *get_argv(int i) const;
    };

    extern Main *default_main_loop;
}

#endif // QTX_MAIN_H
