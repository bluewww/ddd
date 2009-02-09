// $Id$ -*- C++ -*-
// Provide macros for casting types.

// Copyright (C) 2001 Universitaet Passau, Germany.
// Copyright (C) 2004 Free Software Foundation, Inc.
// Written by Arnaud Desitter <arnaud@nag.co.uk>.
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

#ifndef _DDD_casts_h
#define _DDD_casts_h

#include "config.h"

#if HAVE_NEW_CASTS
#  define CONST_CAST(TYPE,OBJ) (const_cast<TYPE>(OBJ))
#  define STATIC_CAST(TYPE,OBJ) (static_cast<TYPE>(OBJ))
#  define REINTERPRET_CAST(TYPE,OBJ) (reinterpret_cast<TYPE>(OBJ))
#else
#  define CONST_CAST(TYPE,OBJ) ((TYPE)(OBJ))
#  define STATIC_CAST(TYPE,OBJ) ((TYPE)(OBJ))
#  define REINTERPRET_CAST(TYPE,OBJ) ((TYPE)(OBJ))
#endif

// Motif does not use "const _XtString" and this leads to many const_cast.
#define XMST(OBJ) CONST_CAST(char *,OBJ)

// first three fields of XtResource are char*.
#define XTRESSTR(OBJ) CONST_CAST(char *,OBJ)

// first field of XtActionsRec is char*.
#define XTARECSTR(OBJ) CONST_CAST(char *,OBJ)

// first two fields of XrmOptionDescRec are char*.
#define XRMOPTSTR(OBJ) CONST_CAST(char *,OBJ)

// Arg.name is char*.
#define ARGSTR(OBJ) CONST_CAST(char *,OBJ)

#endif // _DDD_casts_h
// DON'T ADD ANYTHING BEHIND THIS #endif
