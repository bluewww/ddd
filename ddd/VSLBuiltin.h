// $Id$
// Deklaration builtin()

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef _Nora_VSLBuiltin_h
#define _Nora_VSLBuiltin_h

#ifdef __GNUG__
#pragma interface
#endif


#include "strclass.h"
#include "Range.h"
#include "bool.h"
#include "ListBox.h"

typedef Box * (*BuiltinFunc)(ListBox *);

struct VSLBuiltin {
    static int resolve(const string& func_name);
    // Index zu gegebener Funktion zurueckgeben

    static BuiltinFunc func(int index);
    // Funktionsaufruf zu gegebenem Index zurueckgeben

    static bool isAssoc(int functionIndex);
    // Flag: ist Funktion assoziativ?

    static bool hasSideEffects(int functionIndex);
    // Flag: hat Funktion Seiteneffekte?

    static char *func_name(int functionIndex);
    // Name fuer interne Repraesentation ("__op_halign")

    static char *ext_name(int functionIndex);
    // Name fuer externe Repraesentation ("(&)")

    static bool isInfix(int functionIndex);
    // Flag: ext_name infix ausgeben?
};

#endif
