/* $Id$ */
/* These are required for correct profiling on SUNs */

/*
    Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
    Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).

    This file is part of the ICE Library.

    The ICE Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    The ICE Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with the ICE Library -- see the file COPYING.LIB.
    If not, write to the Free Software Foundation, Inc.,
    675 Mass Ave, Cambridge, MA 02139, USA.

    ICE is the incremental configuration engine.
    Contact ice@ips.cs.tu-bs.de for details.
*/

/*
 * $Log$
 * Revision 1.1  1995/05/01 15:47:12  zeller
 * Initial revision
 *
 * Revision 1.1  1995/03/30  11:52:59  zeller
 * Initial revision
 *
 * Revision 1.1.1.1  1995/02/09  09:12:05  zeller
 * ICE 0.5 distribution
 *
 * Revision 1.7  1995/01/14  14:48:27  zeller
 * New license for ICE 0.5
 *
*/

void *dlopen(path, mode)
    char *path;
    int mode;
{ 
    return 0; 
}

void *dlsym(handle, symbol)
    void *handle;
    char *symbol;
{ 
    return 0; 
}

int dlclose(handle)
    void *handle;
{ 
    return -1; 
}
