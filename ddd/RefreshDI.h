// $Id$ -*- C++ -*-
// Refresh Disassemble Info

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

#ifndef _DDD_RefreshDisassembleInfo_h
#define _DDD_RefreshDisassembleInfo_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "strclass.h"
#include "status.h"
#if defined(IF_XM)
#include <Xm/Text.h>
#else
#include <GUI/ScrolledText.h>
#endif


// This is used in SourceView.

struct RefreshDisassembleInfo {
    string pc;
#if defined(IF_XM)
    XmHighlightMode mode;
#else
    GUI::HighlightMode mode;
#endif
    StatusDelay delay;

private:
    // Don't use assignment or copy constructors
    RefreshDisassembleInfo(const RefreshDisassembleInfo&);
    RefreshDisassembleInfo& operator = (const RefreshDisassembleInfo&);

public:
#if defined(IF_XM)
    RefreshDisassembleInfo(const string& p, XmHighlightMode m,
			   const string& msg)
        : pc(p), mode(m), delay(msg)
    {}
#else
    RefreshDisassembleInfo(const string& p, GUI::HighlightMode m,
			   const string& msg)
        : pc(p), mode(m), delay(msg)
    {}
#endif

    ~RefreshDisassembleInfo()
    {}
};

#endif // _DDD_RefreshDisassembleInfo_h
// DON'T ADD ANYTHING BEHIND THIS #endif
