// $Id$ -*- C++ -*-
// Determine file type

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

// $Log$
// Revision 1.1  1995/05/01 15:47:32  zeller
// Initial revision
//
// Revision 1.2  1995/04/21  16:35:50  zeller
// New: check for picture files
//
// Revision 1.1  1995/03/22  19:26:18  zeller
// Initial revision
//

#ifndef _DDD_filetype_h
#define _DDD_filetype_h

#include "bool.h"
#include "strclass.h"

// True if FILE_NAME is an executable file
bool is_exec_file(const string& file_name);

// True if FILE_NAME is a core file
bool is_core_file(const string& file_name);

// True if FILE_NAME is a source file
bool is_source_file(const string& file_name);

// True if FILE_NAME is a PostScript file
bool is_postscript_file(const string& file_name);

// True if FILE_NAME is a FIG file
bool is_fig_file(const string& file_name);

// True if FILE_NAME is a X Window Dump file
bool is_xwd_file(const string& file_name);

// True if FILE_NAME is a directory
bool is_directory(const string& file_name);

#endif // _DDD_filetype_h
// DON'T ADD ANYTHING BEHIND THIS #endif
