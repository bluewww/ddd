// $Id$ -*- C++ -*-
// Synchronous GDB questions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2001 Free Software Foundation, Inc.
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

char question_rcsid[] = 
    "$Id$";

#include "question.h"

#include "assert.h"
#include "cook.h"
#include "ddd.h"
#include "AppData.h"
#include "Command.h"
#include "GDBAgent.h"
#include "Delay.h"
#include "TimeOut.h"
#include "disp-read.h"

#include <X11/Intrinsic.h>
#include <iostream>

#ifndef LOG_GDB_QUESTION
#define LOG_GDB_QUESTION 0
#endif

//-----------------------------------------------------------------------------
// Synchronized questions
//-----------------------------------------------------------------------------

bool gdb_question_running = false; // Flag: is gdb_question() running?

struct GDBReply {
    string answer;		// The answer text (NO_GDB_ANSWER if timeout)
    bool received;		// True iff we found an answer
    bool answered;		// True if we got an answer from GDB
    bool killme;		// True if this is to be deleted

    GDBReply()
	: answer(NO_GDB_ANSWER), 
	  received(false), answered(false), killme(false)
    {}
};

// Timeout proc - called from XtAppAddTimeOut()
static void gdb_reply_timeout(XtPointer client_data, XtIntervalId *)
{
#if LOG_GDB_QUESTION
    std::clog << "gdb_question: TimeOut\n";
#endif

    assert(gdb_question_running);

    GDBReply *reply = (GDBReply *)client_data;
    assert(!reply->received);

    reply->answer   = NO_GDB_ANSWER;
    reply->received = true;
    reply->answered = false;
}

// GDB sent a reply - Called from GDBAgent::send_question()
static void gdb_reply(const string& complete_answer, void *qu_data)
{
#if LOG_GDB_QUESTION
    std::clog << "gdb_question: reply " << quote(complete_answer) << "\n";
#endif

    GDBReply *reply = (GDBReply *)qu_data;
    assert(!reply->received || reply->killme);

    reply->answer   = complete_answer;
    reply->received = true;
    reply->answered = true;

    // Weed out GDB junk
    filter_junk(reply->answer);

    if (reply->killme)
    {
	// Reply arrived too late
	delete reply;
    }
}

// Weed out GDB `verbose' junk from ANSWER
void filter_junk(string& answer)
{
    if (gdb->type() == GDB)
    {
	while (answer.contains("Reading in symbols ", 0) ||
	       answer.contains("Mapping symbols ", 0))
	{
	    if (answer.contains("done.\n"))
		answer = answer.after("done.\n");
	    else
		break;
	}
    }
}

// Wait for GDB reply
static void wait_for_gdb_reply(GDBReply *reply, int timeout)
{
    reply->received = false;
    reply->answer   = NO_GDB_ANSWER;

    if (timeout == 0)
	timeout = app_data.question_timeout;

    XtIntervalId timer = 0;
    if (timeout > 0)
    {
	timer = XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
				timeout * 1000,
				gdb_reply_timeout, (void *)reply);
    }

    // Process all GDB input and timer events
    while (!reply->received && gdb->running())
	XtAppProcessEvent(XtWidgetToApplicationContext(gdb_w), 
			  XtIMTimer | XtIMAlternateInput);

    if (reply->answered || !reply->received)
    {
	// Reply has been answered or will not be answered any more:
	// Remove timeout
	if (timeout > 0)
	    XtRemoveTimeOut(timer);
    }
}

// Send COMMAND to GDB; return answer (NO_GDB_ANSWER if none)
// TIMEOUT is either 0 (= use default timeout), -1 (= no timeout)
// or maximal time in seconds
string gdb_question(const string& command, int timeout, bool verbatim)
{
    if (command.empty())
	return "";

    if (gdb_question_running || !can_do_gdb_command() || gdb->recording())
	return NO_GDB_ANSWER;

#if LOG_GDB_QUESTION
    std::clog << "gdb_question(" << quote(command) << ")...\n";
#endif

    // Block against reentrant calls
    gdb_question_running = true;

    // Set verbatim mode if needed
    bool old_verbatim = gdb->verbatim();
    gdb->verbatim(verbatim);

    // Set delay (unless this is a trivial question)
    Delay *delay = 0;
    if (!command.contains("help", 0) && !is_print_cmd(command, gdb))
	delay = new Delay;

    // Send question to GDB
    GDBReply *reply = new GDBReply;
    gdb_command(command, 0, gdb_reply, (void *)reply);

    // GDB received question - set timeout
    wait_for_gdb_reply(reply, timeout);

    // Clear delay again
    delete delay;

    // Restore old verbatim mode
    gdb->verbatim(old_verbatim);

    // Unblock against reentrant calls
    gdb_question_running = false;

    string answer = reply->answer;

    if (reply->answered)
    {
	// Reply is no longer needed
	delete reply;
    }
    else
    {
	// Answer may still arrive (or be canceled): delete reply at this point
	reply->killme = true;
    }
#if LOG_GDB_QUESTION
    std::clog << "gdb_question(" << quote(command) << ") = " 
	      << quote(answer) << "\n";
#endif

    // Return answer
    return answer;
}
