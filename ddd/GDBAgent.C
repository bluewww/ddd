// $Id$
// Communicate with separate GDB process

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char GDBAgent_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Implementierung der Klasse GDBAgent
//-----------------------------------------------------------------------------

#include "GDBAgent.h"
#include "cook.h"
#include "ddd.h"
#include "post.h"

#include <stdlib.h> // exit
#include <string.h> // strdup
#include <stdio.h>  // tmpnam
#include <iostream.h>
#include <fstream.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

DEFINE_TYPE_INFO_1(GDBAgent, TTYAgent);

//-----------------------------------------------------------------------------

DebuggerType debugger_type(const string& type)
{
    if (type.contains("gdb"))
	return GDB;
    if (type.contains("dbx"))
	return DBX;
    if (type.contains("xdb"))
	return XDB;

    cerr << "Unknown debugger type " << quote(type) << "\n";
    exit(EXIT_FAILURE);

    // Never reached
    return GDB;
}


// ***************************************************************************
GDBAgent::GDBAgent (XtAppContext app_context,
		    const string& gdb_call,
		    DebuggerType tp)
    : TTYAgent (app_context, gdb_call),
      state(BusyOnInitialCmds),
      _type(tp),
      _user_data(0),
      busy_handlers (BusyNTypes),
      _has_frame_command(tp == GDB || tp == XDB),
      _has_line_command(false),
      _has_run_io_command(false),
      _has_print_r_command(false),
      _has_where_h_command(false),
      _has_display_command(tp == GDB || tp == DBX),
      _has_clear_command(tp == GDB || tp == DBX),
      _has_pwd_command(tp == GDB || tp == DBX),
      _has_named_values(tp == GDB || tp == DBX),
      _has_func_pos(false),
      _has_when_semicolon(tp == DBX),
      _has_err_redirection(true),
      trace_dialog(false),
      questions_waiting(false),
      _qu_data(0),
      qu_index(0),
      _qu_count(0),
      cmd_array(0),
      complete_answers(0),
      _qu_datas(0),
      _qa_data(0),
      _on_answer(0),
      _on_answer_completion(0),
      _on_question_completion(0),
      _on_qu_array_completion(0),
      complete_answer("")
{
    removeAllHandlers(Died);
    addHandler(Input, InputHP, this); // GDB-Ausgaben
    addHandler(Died,  DiedHP, this);  // GDB beendet

    // unerwuenschte Fehlermeldungen unterdruecken
    removeAllHandlers(Panic);
    removeAllHandlers(Strange);
    addHandler(Panic, PanicHP, this);
    addHandler(Strange, PanicHP, this);
}

GDBAgent::GDBAgent(const GDBAgent& gdb)
    : TTYAgent(gdb),
      state(gdb.state),
      _type(gdb.type()),
      _user_data(0),
      busy_handlers(gdb.busy_handlers),
      _has_frame_command(gdb.has_frame_command()),
      _has_line_command(gdb.has_line_command()),
      _has_run_io_command(gdb.has_run_io_command()),
      _has_print_r_command(gdb.has_print_r_command()),
      _has_where_h_command(gdb.has_where_h_command()),
      _has_display_command(gdb.has_display_command()),
      _has_clear_command(gdb.has_clear_command()),
      _has_pwd_command(gdb.has_pwd_command()),
      _has_named_values(gdb.has_named_values()),
      _has_func_pos(gdb.has_func_pos()),
      _has_when_semicolon(gdb.has_when_semicolon()),
      trace_dialog(false),
      questions_waiting(false),
      _qu_data(0),
      qu_index(0),
      _qu_count(0),
      cmd_array(0),
      complete_answers(0),
      _qu_datas(0),
      _qa_data(0),
      _on_answer(0),
      _on_answer_completion(0),
      _on_question_completion(0),
      _on_qu_array_completion(0),
      complete_answer("")
{}


string GDBAgent::default_prompt() const
{
    switch (type())
    {
    case GDB:
	return "(gdb) ";
    case DBX:
	return "(dbx) ";
    case XDB:
	return ">";
    }

    return "(???) ";
}
    

// ***************************************************************************
void GDBAgent::do_start (OAProc  on_answer,
			 OACProc on_answer_completion,
			 void*   user_data)
{
    assert (state == BusyOnInitialCmds);

    _on_answer = on_answer;
    _on_answer_completion = on_answer_completion;
    _user_data = user_data;
    TTYAgent::start();
    busy_handlers.call(ReadyForQuestion, 0, (void*)false);
    busy_handlers.call(ReadyForCmd, 0, (void*)false);
}

// ***************************************************************************
void GDBAgent::start_plus (OAProc   on_answer,
			   OACProc  on_answer_completion,
			   void*    user_data,
			   string   cmds [],
			   void*    qu_datas [],
			   int      qu_count,
			   OQACProc on_qu_array_completion,
			   void*    qa_data)
{
    if (qu_count > 0) {
	questions_waiting = true;
	init_qu_array
	    (cmds, qu_datas, qu_count, on_qu_array_completion, qa_data);
    }

    do_start(on_answer, on_answer_completion, user_data);
}



// ***************************************************************************
GDBAgent::~GDBAgent ()
{
    shutdown();
}


// ***************************************************************************
bool GDBAgent::send_user_cmd(string cmd, void *user_data)  //ohne \n
{
    if (user_data)
	_user_data = user_data;

    switch (state) {
    case ReadyWithPrompt:
    case BusyOnInitialCmds:

	// Befehl bearbeiten
	state = BusyOnCmd;
	busy_handlers.call(ReadyForQuestion, 0, (void *)false);
	cmd += '\n';
	write((const char *)cmd, cmd.length());
	flush();

	return true;
	break;

    default: // BusyOnQuestion || BusyOnQuArray || BusyOnCmd:
	return false;
	break;
    }

    // Never reached
    return false;
}

// ***************************************************************************
bool GDBAgent::send_user_ctrl_cmd(string cmd, void *user_data)
{
    if (user_data)
	_user_data = user_data;

    write((const char *)cmd, cmd.length());
    flush();
    return true;
}

// ***************************************************************************
bool GDBAgent::send_user_cmd_plus (string   cmds [],
				      void*    qu_datas [],
				      int      qu_count,
				      OQACProc on_qu_array_completion,
				      void*    qa_data,
				      string user_cmd, void* user_data)
{
    if (state != ReadyWithPrompt) 
	return false;

    if (user_data)
	_user_data = user_data;
    if (qu_count > 0) {
	questions_waiting = true;
	init_qu_array
	    (cmds, qu_datas, qu_count, on_qu_array_completion, qa_data);
    }

    // Befehl bearbeiten
    state = BusyOnCmd;
    busy_handlers.call(ReadyForQuestion, 0, (void*)false);
    user_cmd += '\n';
    write((const char *)user_cmd, user_cmd.length());
    flush();

    return true;
}


// ***************************************************************************
bool GDBAgent::send_question (string  cmd,
			      OQCProc on_question_completion,
			      void*   qu_data)
{
    if (state != ReadyWithPrompt) 
	return false;

    state = BusyOnQuestion;
    busy_handlers.call(ReadyForQuestion, this, (void *)false);
    busy_handlers.call(ReadyForCmd, this, (void *)false);

    _on_question_completion = on_question_completion;
    _qu_data = qu_data;
    complete_answer = "";

    cmd += '\n';
    write((const char *)cmd, cmd.length());
    flush();

    return true;
}

// ***************************************************************************
bool GDBAgent::send_qu_array (string   cmds [],
			      void*    qu_datas [],
			      int      qu_count,
			      OQACProc on_qu_array_completion,
			      void*    qa_data)
{
    if (state != ReadyWithPrompt || qu_count == 0)
	return false;

    state = BusyOnQuArray;
    busy_handlers.call(ReadyForQuestion, this, (void *)false);
    busy_handlers.call(ReadyForCmd, this, (void *)false);

    init_qu_array (cmds, qu_datas, qu_count, on_qu_array_completion, qa_data);
    
    // erste Frage losschicken
    write((const char *)cmd_array[0], cmd_array[0].length());
    flush();

    return true;
}

// ***************************************************************************
void GDBAgent::addBusyHandler (unsigned    type,
			       HandlerProc proc,
			       void*       client_data)
{
    busy_handlers.add(type, proc, client_data);
}

// ***************************************************************************
void GDBAgent::removeBusyHandler (unsigned    type,
				  HandlerProc proc,
				  void        *client_data)
{
    busy_handlers.remove(type, proc, client_data);
}

// ***************************************************************************
void GDBAgent::callBusyHandlers ()
{
    busy_handlers.call(ReadyForQuestion,
		       this,
		       (void*)(state == ReadyWithPrompt));

    busy_handlers.call(ReadyForCmd,
		       this,
		       (void*)(state == ReadyWithPrompt	|| isBusyOnCmd()));
}


// ***************************************************************************
void GDBAgent::set_trace_dialog (bool trace)
{
    if (trace && !trace_dialog) {
 	addHandler (Input,  traceInputHP); //gdb-Ausgaben
 	addHandler (Output, traceOutputHP); //Eingabe an gdb
 	addHandler (Error,  traceErrorHP); //gdb-FehlerAusgaben
    }
    else if (!trace && trace_dialog) {
 	removeHandler (Input,  traceInputHP); //gdb-Ausgaben
 	removeHandler (Output, traceOutputHP); //Eingabe an gdb
 	removeHandler (Error,  traceErrorHP); //gdb-FehlerAusgaben
    }
    trace_dialog = trace;
}

// ***************************************************************************
void GDBAgent::init_qu_array (string   cmds [],
			      void*    qu_datas [],
			      int      qu_count,
			      OQACProc on_qu_array_completion,
			      void*    qa_data)
{
    _on_qu_array_completion = on_qu_array_completion;
    qu_index = 0;
    _qu_count = qu_count;
    cmd_array = new string [qu_count];
    _qu_datas = new (void* [qu_count]);
    complete_answers = new string [qu_count];
    _qa_data = qa_data;
    for (int i = 0; i < qu_count; i++)
    {
	cmd_array[i] = cmds[i] + '\n';
	_qu_datas[i] = qu_datas[i];
    }
}

// ***************************************************************************
bool GDBAgent::ends_with_prompt (const string& answer)
{
    unsigned beginning_of_line = answer.index('\n', -1) + 1;

    switch (type())
    {
    case GDB:
    case DBX:
	return beginning_of_line < answer.length()
	    && answer.length() > 3
	    && answer[beginning_of_line] == '(' 
	    && answer[answer.length() - 2] == ')'
	    && answer[answer.length() - 1] == ' '
	    && (answer.contains("gdb") || answer.contains("dbx"));

    case XDB:
	return beginning_of_line < answer.length()
	    && answer.length() > 0
	    && answer[beginning_of_line] == '>';
    }

    return false;		// Never reached
}

// ***************************************************************************
bool GDBAgent::ends_with_secondary_prompt (const string& answer)
{
    switch (type())
    {
    case GDB:
    case DBX:
	return answer.length() > 3
	    && answer[answer.length() - 3] == '\n'
	    && answer[answer.length() - 2] == '>'
	    && answer[answer.length() - 1] == ' ';

    case XDB:
	return answer.matches("[Hit RETURN to continue]", -1) 
		|| answer.matches("--More--", -1);
    }

    return false;		// Never reached
}

// ***************************************************************************
void GDBAgent::cut_off_prompt (string& answer)
{
    switch (type())
    {
    case GDB:
    case DBX:
	answer = answer.before('(', -1);
	break;

    case XDB:
	answer = answer.before('>', -1);
	break;
    }
}



// ***************************************************************************
//
void GDBAgent::strip_comments(string& s)
{
    if (!has_print_r_command())
	return;

    if (s.contains('/'))
    {
	// Check for C and C++ comments
	char quoted = '\0';

	unsigned int i = 0;
	while (i < s.length())
	{
	    char c = s[i++];
	    switch (c)
	    {
	    case '\\':
		if (i < s.length())
		    i++;
		break;

	    case '\'':
	    case '\"':
		if (c == quoted)
		    quoted = '\0';
		else if (!quoted)
		    quoted = c;
		break;

	    case '/':
		if (i < s.length() && !quoted)
		{
		    if (s[i] == '*')
		    {
			/* C-style comment */
			int end = s.index("*/", i + 1);
			if (end == -1)
			{
			    // unterminated comment -- keep it now
			    break;
			}

			// Remove comment
			i--;
			s.at(int(i), int(end - i + 2)) = "";
		    }
		    else if (s[i] == '/')
		    {
			// C++-style comment
			int end = s.index('\n', i + 1);
			i--;

			// Remove comment
			if (end == -1)
			    s.from(int(i)) = "";
			else
			    s.at(int(i), int(end - i + 1)) = "";
		    }
		}
	    }
	}
    }

    if (s.contains("dbx: warning:"))
    {
	// Weed out annoying DBX warnings like
	// `dbx: warning: -r option only recognized for C++' and
	// `dbx: warning: unknown language, 'c' assumed'

	static regex RXdbxwarn1("dbx: warning:[^\n]*"
				"option only recognized for[^\n]*\n");
	static regex RXdbxwarn2("dbx: warning:[^\n]*"
				"unknown language[^\n]*\n");
	s.gsub(RXdbxwarn1, "");
	s.gsub(RXdbxwarn2, "");
    }
}

//-----------------------------------------------------------------------------
// Event handlers
//-----------------------------------------------------------------------------

// ***************************************************************************
// Received data from GDB
//
void GDBAgent::InputHP(Agent *, void* client_data, void* call_data)
{
    GDBAgent*   gdb   = (GDBAgent *) client_data;
    DataLength* dl    = (DataLength *) call_data;
    string      answer(dl->data, dl->length);

    if (gdb->ends_with_secondary_prompt(answer))
    {
	// GDB requires more information here: probably the
	// selection of an ambiguous C++ name.
	// We simply try the first alternative here:
	// - in GDB, this means `all';
	// - in DBX and XDB, this is a non-deterministic selection,

	string answer = "1\n";
	gdb->write(answer, answer.length());
	gdb->flush();
    }

    switch (gdb->state)
    {
    case ReadyWithPrompt:
	// We have a prompt, and still get input?  Maybe this is an
	// answer to a command sent before the prompt was received - 
	// or a reply to a control character (`Quit').
	_gdb_out(answer);
	break;

    case BusyOnInitialCmds:
    case BusyOnCmd:
	if (!gdb->ends_with_prompt(answer))
	{
            // Received only part of the answer
	    if (gdb->_on_answer != 0)
		gdb->_on_answer (answer, gdb->_user_data);
	}
	else
	{
            // Received complete answer (GDB issued prompt)
	    gdb->cut_off_prompt (answer);
	    gdb->strip_comments (answer);

	    if (answer != "" && gdb->_on_answer != 0)
		gdb->_on_answer (answer, gdb->_user_data);

            // Set new state and call answer procedure
	    if (gdb->state == BusyOnInitialCmds)
	    {
		if (!gdb->questions_waiting) 
		{
		    gdb->state = ReadyWithPrompt;
		    gdb->busy_handlers.call(ReadyForCmd, 0, 
					    (void*)true);
		    gdb->busy_handlers.call(ReadyForQuestion, 0, 
					    (void*)true);

		    if (gdb->_on_answer_completion != 0)
			gdb->_on_answer_completion (gdb->_user_data);
		}
		else
		{
		    gdb->state = BusyOnQuArray;

		    // Send first question
		    gdb->write((const char *)gdb->cmd_array[0],
			       gdb->cmd_array[0].length());
		    gdb->flush();
		}
	    }
	    else if (!gdb->questions_waiting)
	    {
		gdb->state = ReadyWithPrompt;
		gdb->busy_handlers.call(ReadyForQuestion, 0, (void*)true);

		if (gdb->_on_answer_completion != 0)
		    gdb->_on_answer_completion (gdb->_user_data);
	    }
	    else
	    {
		gdb->state = BusyOnQuArray;
		gdb->busy_handlers.call(ReadyForCmd, 0, (void*)false);

		// Send first question
		gdb->write((const char *)gdb->cmd_array[0],
		      gdb->cmd_array[0].length());
		gdb->flush();
	    }
	}
	break;

    case BusyOnQuestion:
	if (!gdb->ends_with_prompt(answer))
	{
            // Received only part of the answer
	    gdb->complete_answer += answer;
	}
	else
	{
            // Received complete answer (GDB issued prompt)
	    gdb->cut_off_prompt (answer);
	    gdb->strip_comments (answer);

	    gdb->complete_answer += answer;

            // Set new state
	    gdb->state = ReadyWithPrompt;
	    gdb->busy_handlers.call(ReadyForQuestion, 0, (void*)true);
	    gdb->busy_handlers.call(ReadyForCmd, 0, (void*)true);

	    if (gdb->_on_question_completion != 0)
		gdb->_on_question_completion (gdb->complete_answer, 
					      gdb->_qu_data);
	}
	break;

    case BusyOnQuArray:
	if (!gdb->ends_with_prompt(answer))
	{
            // Received only part of the answer
	    gdb->complete_answers[gdb->qu_index] += answer;
	}
	else
	{
            // Received complete answer (GDB issued prompt)
	    gdb->cut_off_prompt (answer);
	    gdb->strip_comments (answer);

	    gdb->complete_answers[gdb->qu_index] += answer;

            // Set new state
	    gdb->qu_index++;

	    if (gdb->qu_index == gdb->_qu_count) {

		// Received all answers
		delete[] gdb->cmd_array;

		// Set new state
		gdb->state = ReadyWithPrompt;
		gdb->busy_handlers.call(ReadyForCmd, 0, (void*)true);

		if (gdb->questions_waiting) {
		    // We did not call the OACProc yet.

		    if (gdb->_on_answer_completion != 0)
			gdb->_on_answer_completion (gdb->_user_data);

		    gdb->questions_waiting = false;
		}

		gdb->busy_handlers.call(ReadyForQuestion, 0, (void*)true);

		if (gdb->_on_qu_array_completion != 0)
		{
		    gdb->_on_qu_array_completion (gdb->complete_answers, 
						  gdb->_qu_datas,
						  gdb->_qu_count,
						  gdb->_qa_data);
		}
	    }
	    else {
		// Send next question
		gdb->write((const char *)gdb->cmd_array[gdb->qu_index], 
			   gdb->cmd_array[gdb->qu_index].length());
		gdb->flush();
	    }
	}
	break;

    default:
	assert(0);
	break;
    }
}

// ***************************************************************************
// Configuration

// DBX 3.0 wants `print -r' instead of `print' for C++
string GDBAgent::print_command(string expr) const
{
    string cmd;

    switch (type())
    {
    case GDB:
    case DBX:
	if (has_print_r_command())
	    cmd = "print -r";
	else
	    cmd = "print";
	break;

    case XDB:
	cmd = "p";
	break;
    }

    if (expr != "")
    {
	if (!has_named_values())
	    cmd = echo_command(expr + " = ") + "; " + cmd;
	cmd += " " + expr;
    }

    return cmd;
}

// DBX 3.0 wants `display -r' instead of `display' for C++
string GDBAgent::display_command(string expr) const
{
    if (!has_display_command())
	return "";

    string cmd;
    if (has_print_r_command())
	cmd = "display -r";
    else
	cmd = "display";

    if (expr != "")
	cmd += " " + expr;

    return cmd;
}

// DBX 3.0 wants `where -h' instead of `where'
string GDBAgent::where_command() const
{
    switch (type())
    {
    case GDB:
    case DBX:
	if (has_where_h_command())
	    return "where -h";
	else
	    return "where";
	
    case XDB:
	return "t";
    }

    return "";			// Never reached
}

// Some DBXes want `sh pwd' instead of `pwd'
string GDBAgent::pwd_command() const
{
    switch (type())
    {
    case GDB:
    case DBX:
	if (has_pwd_command())
	    return "pwd";
	else
	    return "sh pwd";

    case XDB:
	return "!pwd";
    }

    return "";			// Never reached
}

// Some DBXes want `sh pwd' instead of `pwd'
string GDBAgent::frame_command(string depth) const
{
    if (!has_frame_command())
	return "";

    switch (type())
    {
    case GDB:
    case DBX:
	if (depth == "")
	    return "frame";
	else
	    return "frame " + depth;

    case XDB:
	if (depth == "")
	    return print_command("$depth");
	else
	    return "V " + depth;
    }

    return "";			// Never reached
}

// Each debugger has its own way of echoing (sigh)
string GDBAgent::echo_command(string text) const
{
    switch (type())
    {
    case GDB:
	return "echo " + text;

    case DBX:
	return print_command() + " " + quote(text);

    case XDB:
	return quote(text);
    }

    return "";			// Never reached
}



// ***************************************************************************

void GDBAgent::DiedHP (Agent *, void *, void* call_data)
{
    char *reason = (char *)call_data;
    post_gdb_died(reason);
}


// ***************************************************************************
// Handle error messages
//
void GDBAgent::PanicHP (Agent *source, void *, void *call_data)
{
    string msg = (char *)call_data;

    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    if (gdb != 0)
	cerr << gdb->default_prompt() << msg << "\n";
    else
	cerr << source->path() << ": " << msg << "\n";
}



// ***************************************************************************

// Trace communication
static void trace(const string& prefix, void *call_data)
{
    DataLength* dl    = (DataLength *) call_data;
    string s(dl->data, dl->length);

    bool s_ends_with_nl = false;
    if (s.length() > 0 && s[s.length() - 1] == '\n')
    {
	s_ends_with_nl = true;
	s = s.before(int(s.length() - 1));
    }

    s = quote(s);
    string nl = string("\\n\"\n") + replicate(' ', prefix.length()) + "\"";
    s.gsub("\\n", nl);

    if (s_ends_with_nl)
	s(s.length() - 1, 0) = "\\n";

    clog << prefix << s << '\n';
    clog.flush();
}
    
void GDBAgent::traceInputHP(Agent *, void *, void *call_data)
{
    trace("<- ", call_data);
}

void GDBAgent::traceOutputHP(Agent *, void *, void *call_data)
{
    trace("-> ", call_data);
}

void GDBAgent::traceErrorHP (Agent *, void *, void *call_data)
{
    trace("<= ", call_data);
}

// ***************************************************************************

// Make sure the GDB child does not inherit the current terminal settings.
int GDBAgent::setupChildCommunication()
{
    // Nothing special...
    return TTYAgent::setupChildCommunication();
}
