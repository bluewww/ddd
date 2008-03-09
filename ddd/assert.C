// $Id$ -*- C++ -*-
// Function for custom implementation of assert.

// Copyright (C) 2004 Free Software Foundation, USA.
// Written by Arnaud Desitter.
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

#include <iostream>
#include <stdlib.h>
#include "assert.h"

void ddd_assert_fail (const char *assertion, const char *file,
		      unsigned int line, const char *function) {
  if ( function != NULL )
    std::cerr << "\n" << file << ": In function `" << function << "':";
  std::cerr << "\n" << file << ":" << line
	    << ": assertion `" << assertion << "' failed\n";
  ::abort();
}
