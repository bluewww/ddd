// $Id$ -*- C++ -*-
// Check for expiration date

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char expired_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "expired.h"

#include "version.h"
#include <stdio.h>
#include <time.h>
#ifndef HAVE_DIFFTIME
inline double difftime(time_t time1, time_t time0) { return time1 - time0; }
#endif


//-----------------------------------------------------------------------------
// Check expiration date
//-----------------------------------------------------------------------------

// Return expiration time.  0: no expiration time, -1: invalid expiration time.
static time_t ddd_expiration_time()
{
    string expiration_date = DDD_EXPIRES;
    if (expiration_date == "")
    {
	// No expiration date given
	return 0;
    }

    int year  = 0;
    int month = 0;
    int day   = 0;
    sscanf(expiration_date.chars(), "%d-%d-%d", &year, &month, &day);
    if (year == 0 || month == 0 || day == 0)
	return -1;

    struct tm expiration_tm;
    expiration_tm.tm_sec   = 0;
    expiration_tm.tm_min   = 0;
    expiration_tm.tm_hour  = 0;
    expiration_tm.tm_mday  = day;
    expiration_tm.tm_mon   = month - 1;
    expiration_tm.tm_year  = year - 1900;
    expiration_tm.tm_wday  = -1;
    expiration_tm.tm_yday  = -1;
    expiration_tm.tm_isdst = -1;
    
    time_t expiration_time = mktime(&expiration_tm);
    if (expiration_time < 0)
	return -1;

    return expiration_time;
}

string ddd_expiration_date()
{
    time_t expiration_time = ddd_expiration_time();
    if (expiration_time == 0)
	return "";		// No expiration
    else if (expiration_time < 0)
	return "the epoch";	// Invalid time

    string pretty_expiration_date;

#ifdef HAVE_STRFTIME
    char buffer[1024];
    strftime(buffer, sizeof buffer, "%x", localtime(&expiration_time));
    pretty_expiration_date = buffer;
#elif HAVE_ASCTIME    
    pretty_expiration_date = asctime(localtime(&expiration_time));
    pretty_expiration_date.gsub("\n", "");
#else
    pretty_expiration_date = DDD_EXPIRES;
#endif

    return pretty_expiration_date;
}

bool ddd_expired()
{
    time_t expiration_time = ddd_expiration_time();
    if (expiration_time == 0)
    {
	// No expiration time
	return false;
    }
    else if (expiration_time < 0)
    {
	// Invalid expiration time
	return true;
    }

    time_t current_time = time(NULL);
    if (current_time < 0)
    {
	// No current time -- consider as expired?
	return true;
    }

    double seconds_since_expiration = difftime(current_time, expiration_time);
    if (seconds_since_expiration <= 0)
    {
	// Still within expiration date
	return false;
    }

    return true;
}
