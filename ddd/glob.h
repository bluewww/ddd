// $Id$ -*- C++ -*-
// glob function decls

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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

#ifndef _DDD_glob_h
#define _DDD_glob_h

// These functions have "C" bindings such that they be compatible with
// GNU's libglob.a.

extern "C" {
extern int noglob_dot_filenames;
extern int glob_pattern_p (char *pattern);
extern int glob_match (char *pattern, char *text, int dot_special);
extern char **glob_vector (char *pat, char *dir);
extern char **glob_filename(char *pathname);
}

#endif // _DDD_glob_h
// DON'T ADD ANYTHING BEHIND THIS #endif
