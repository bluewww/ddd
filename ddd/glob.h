// $Id$ -*- C++ -*-
// glob function decls

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_glob_h
#define _DDD_glob_h

// These functions have "C" bindings such that they be compatible with
// GNU's libglob.a.

extern "C" {
extern int noglob_dot_filenames;
extern int glob_pattern_p (const char *pattern);
extern int glob_match (const char *pattern, const char *text, int dot_special);
extern char **glob_vector (const char *pat, const char *dir);
extern char **glob_filename(const char *pathname);
}

#endif // _DDD_glob_h
// DON'T ADD ANYTHING BEHIND THIS #endif
