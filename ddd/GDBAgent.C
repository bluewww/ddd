// $Id$
// Communicate with separate GDB process

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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

#include <string.h> // strdup
#include <stdio.h>  // tmpnam
#include <iostream.h>
#include <fstream.h>

DEFINE_TYPE_INFO_1(GDBAgent, TTYAgent);

//-----------------------------------------------------------------------------

DebuggerType debugger_type(const string& type)
{
    if (type.contains("gdb"))
	return GDB;
    if (type.contains("dbx"))
	return DBX;

    cerr << "Unknown debugger type " << quote(type) << "\n";
    exit(1);
}


// ***************************************************************************
// Create appropriate debugger call
string build_gdb_call (DebuggerType debugger_type,
		       const string& debugger_name,
		       const string& init_file,
		       int argc, char *argv[],
		       string myArguments)
{
    string gdb_call = debugger_name;

    switch(debugger_type)
    {
    case GDB:
	gdb_call += " -q -fullname";
	if (init_file != "")
	    gdb_call += " -x " + init_file;
	break;

    case DBX:
	if (init_file != "")
	{
	    // When we specify a DBX init file, all default init
	    // files are overridden.  Specify them explicitly.
	    gdb_call += " -s .dbxrc -s $HOME/.dbxrc";
	    gdb_call += " -s .dbxinit -s $HOME/.dbxinit";
	    gdb_call += " -s " + init_file;
	}
	break;
    }

    if (myArguments != "")
	gdb_call += " " + myArguments;

    for (int i = 1; i < argc; i++) {
	string arg = argv[i];
	gdb_call += " " + sh_quote(arg);
    }

    return gdb_call;
}

// ***************************************************************************
GDBAgent::GDBAgent (XtAppContext app_context,
		    const string& gdb_call,
		    DebuggerType tp)
    : state(BusyOnInitialCmds),
      _type(tp),
      _user_data(0),
      busy_handlers (BusyNTypes),
      _prompt(tp == GDB ?  "\\(.\\|\n\\)*([^)]*gdb[^)]*) " :
	      (tp == DBX ? "\\(.\\|\n\\)*([^)]*dbx[^)]*) " : 
	       "\\(.\\|\n\\)*([^)]*) "), true),
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
      complete_answer(""),
      TTYAgent (app_context, gdb_call)
{
    removeAllHandlers(Died);
    addHandler(Input, InputHP, this); //gdb-Ausgaben
    addHandler(Died,  DiedHP, this);  //gdb beendet

    // unerwuenschte Fehlermeldungen unterdruecken
    removeAllHandlers(Panic);
    removeAllHandlers(Strange);
    addHandler(Panic, PanicHP, this);
    addHandler(Strange, PanicHP, this);
}

string GDBAgent::default_prompt() const
{
    switch (type())
    {
    case GDB:
	return "(gdb) ";
    case DBX:
	return "(dbx) ";
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
    return answer.matches(prompt());
}

// ***************************************************************************
void GDBAgent::cut_off_prompt (string& answer)
{
    answer = answer.before('(', -1);
}



//-----------------------------------------------------------------------------
// Event handlers
//-----------------------------------------------------------------------------

// ***************************************************************************
// Received data from GDB
//
void GDBAgent::InputHP (Agent* agent, void* client_data, void* call_data)
{
    GDBAgent*   gdb   = (GDBAgent *) client_data;
    DataLength* dl    = (DataLength *) call_data;
    string      answer(dl->data, dl->length);

    switch (gdb->state) {
    case ReadyWithPrompt:
	// We have a prompt, and still get input?  Maybe this is an
	// answer to a command sent before the prompt was received.
	gdb->state = BusyOnCmd;
	gdb->busy_handlers.call(ReadyForQuestion, 0, (void*)false);
	break;

    case BusyOnInitialCmds:
    case BusyOnCmd:
	if ( !gdb->ends_with_prompt (answer) ) {

            // Received only part of the answer
	    if (gdb->_on_answer != 0)
		gdb->_on_answer (answer, gdb->_user_data);
	}
	else {
            // Received complete answer (GDB issued prompt)
	    gdb->cut_off_prompt (answer);

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
	if ( !gdb->ends_with_prompt (answer) ) {

            // Received only part of the answer
	    gdb->complete_answer += answer;
	}
	else {
            // Received complete answer (GDB issued prompt)
	    gdb->cut_off_prompt (answer);

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
	if ( !gdb->ends_with_prompt (answer) ) {

            // Received only part of the answer
	    gdb->complete_answers[gdb->qu_index] += answer;
	}
	else {
            // Received complete answer (GDB issued prompt)
	    gdb->cut_off_prompt (answer);

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

void GDBAgent::DiedHP (Agent *agent, void* client_data, void* call_data)
{
    char *reason = (char *)call_data;
    post_gdb_died(reason);
}


// ***************************************************************************
// Handle error messages
//
void GDBAgent::PanicHP (Agent *source, void *client_data, void *call_data)
{
    string msg = (char *)call_data;

    if (msg.contains("I/O exception ", 0))
    {
	// Such messages occur all the time.  Suppress them.
    }
    else
    {
	GDBAgent *gdb = ptr_cast(GDBAgent, source);
	if (gdb != 0)
	    cerr << gdb->default_prompt() << msg << "\n";
	else
	    cerr << source->path() << ": " << msg << "\n";
    }
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
}
    
void GDBAgent::traceInputHP(Agent *source, 
			    void *client_data, void *call_data)
{
    trace("<- ", call_data);
}

void GDBAgent::traceOutputHP(Agent *source, 
			     void *client_data, void *call_data)
{
    trace("-> ", call_data);
}


void GDBAgent::traceErrorHP (Agent *source, void *client_data, void *call_data)
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
