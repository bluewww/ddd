// $Id$ -*- C++ -*-
// Determine file type

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_filetype_h
#define _DDD_filetype_h

#include "bool.h"
#include "strclass.h"
#include <time.h>		// time_t

// True if FILE_NAME is a regular file
bool is_regular_file(const string& file_name);

// True if FILE_NAME contains non-TEXT characters
bool is_binary_file(const string& file_name);

// True if FILE_NAME is an executable binary file
bool is_exec_file(const string& file_name);

// True if FILE_NAME is an executable file or script
bool is_cmd_file(const string& file_name);

// True if FILE_NAME is a core file
bool is_core_file(const string& file_name);

// True if FILE_NAME is a source file
bool is_source_file(const string& file_name);

// True if FILE_NAME is a PostScript file
bool is_postscript_file(const string& file_name);

// True if FILE_NAME is a FIG file
bool is_fig_file(const string& file_name);

// True if FILE_NAME is a PERL file
bool is_perl_file(const string& file_name);

// True if FILE_NAME is a PYTHON file
bool is_python_file(const string& file_name);

// True if FILE_NAME is a directory
bool is_directory(const string& file_name);

// Return full file name of COMMAND
string cmd_file(const string& command);

// Return last modification time of FILE_NAME
time_t last_modification_time(const string& file_name);

#endif // _DDD_filetype_h
// DON'T ADD ANYTHING BEHIND THIS #endif
