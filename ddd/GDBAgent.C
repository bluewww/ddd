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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

#include <stdlib.h> // exit
#include <string.h> // strdup
#include <stdio.h>  // tmpnam
#include <iostream.h>
#include <fstream.h>
#include <ctype.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

DEFINE_TYPE_INFO_1(GDBAgent, TTYAgent);

//-----------------------------------------------------------------------------
// Determine debugger type from TYPE
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
// Constructor
GDBAgent::GDBAgent (XtAppContext app_context,
		    const string& gdb_call,
		    DebuggerType tp,
		    unsigned int nTypes)
    : TTYAgent (app_context, gdb_call, nTypes),
      state(BusyOnInitialCmds),
      _type(tp),
      _user_data(0),
      _has_frame_command(tp == GDB || tp == XDB),
      _has_run_io_command(false),
      _has_print_r_option(false),
      _has_output_command(false),
      _has_where_h_option(false),
      _has_display_command(tp == GDB || tp == DBX),
      _has_clear_command(tp == GDB || tp == DBX),
      _has_pwd_command(tp == GDB || tp == DBX),
      _has_setenv_command(tp == DBX),
      _has_edit_command(tp == DBX),
      _has_named_values(tp == GDB || tp == DBX),
      _has_when_command(tp == DBX),
      _has_when_semicolon(tp == DBX),
      _has_delete_comma(false),
      _has_err_redirection(true),
      _program_language(LANGUAGE_C),
      _trace_dialog(false),
      _verbatim(false),
      last_prompt("(???) "),
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
    // Set default prompt
    switch (type())
    {
    case GDB:
	last_prompt = "(gdb) ";
	break;
    case DBX:
	last_prompt = "(dbx) ";
	break;
    case XDB:
	last_prompt = ">";
	break;
    }

    // unerwuenschte Fehlermeldungen unterdruecken
    removeAllHandlers(Panic);
    removeAllHandlers(Strange);
    addHandler(Panic,   PanicHP);
    addHandler(Strange, PanicHP);
    addHandler(Input,   InputHP); // GDB-Ausgaben
}

// Copy constructor
GDBAgent::GDBAgent(const GDBAgent& gdb)
    : TTYAgent(gdb),
      state(gdb.state),
      _type(gdb.type()),
      _user_data(0),
      _has_frame_command(gdb.has_frame_command()),
      _has_run_io_command(gdb.has_run_io_command()),
      _has_print_r_option(gdb.has_print_r_option()),
      _has_output_command(gdb.has_output_command()),
      _has_where_h_option(gdb.has_where_h_option()),
      _has_display_command(gdb.has_display_command()),
      _has_clear_command(gdb.has_clear_command()),
      _has_pwd_command(gdb.has_pwd_command()),
      _has_setenv_command(gdb.has_setenv_command()),
      _has_edit_command(gdb.has_edit_command()),
      _has_named_values(gdb.has_named_values()),
      _has_when_command(gdb.has_when_command()),
      _has_when_semicolon(gdb.has_when_semicolon()),
      _has_delete_comma(gdb.has_delete_comma()),
      _has_err_redirection(gdb.has_err_redirection()),
      _program_language(gdb.program_language()),
      _trace_dialog(gdb.trace_dialog()),
      _verbatim(gdb.verbatim()),
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

// Return default title
string GDBAgent::title() const
{
    switch (type())
    {
    case GDB:
	return "GDB";
    case DBX:
	return "DBX";
    case XDB:
	return "XDB";
    }

    return "debugger";
}


// ***************************************************************************
// Trace communication
static void trace(char *prefix, void *call_data)
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
    string nl = string("\\n\"\n") + replicate(' ', strlen(prefix)) + "\"";
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
// Start GDBAgent
void GDBAgent::do_start (OAProc  on_answer,
			 OACProc on_answer_completion,
			 void*   user_data)
{
    assert (state == BusyOnInitialCmds);

    _on_answer = on_answer;
    _on_answer_completion = on_answer_completion;
    _user_data = user_data;
    TTYAgent::start();
    callHandlers(ReadyForQuestion, (void *)false);
    callHandlers(ReadyForCmd, (void *)false);
    callHandlers(LanguageChanged, (void *)this);
}

// ***************************************************************************
// Start with some extra commands
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
// Destructor
GDBAgent::~GDBAgent ()
{
    shutdown();
}


// ***************************************************************************
// Send CMD to GDB, associated with USER_DATA.  Return false iff busy.
bool GDBAgent::send_user_cmd(string cmd, void *user_data)  // without `\n'
{
    if (user_data)
	_user_data = user_data;

    switch (state) {
    case ReadyWithPrompt:
    case BusyOnInitialCmds:

	// Process CMD
	state = BusyOnCmd;
	callHandlers(ReadyForQuestion, (void *)false);
	cmd += '\n';
	write((const char *)cmd, cmd.length());
	flush();

	return true;

    case BusyOnQuestion:
    case BusyOnQuArray:
    case BusyOnCmd:
	break;
    }

    return false;
}

// ***************************************************************************
// Send CMD to GDB (unconditionally), associated with USER_DATA.
bool GDBAgent::send_user_ctrl_cmd(string cmd, void *user_data)
{
    if (user_data)
	_user_data = user_data;

    // Upon ^D, GDB is no more in state to receive commands.
    // Expect a new prompt to appear.
    if (cmd == '\004' && state == ReadyWithPrompt)
	state = BusyOnCmd;

    write((const char *)cmd, cmd.length());
    flush();
    return true;
}

// ***************************************************************************
// Send command array CMDS to GDB, associated with QU_DATAS.
bool GDBAgent::send_user_cmd_plus (string   cmds[],
				   void*    qu_datas[],
				   int      qu_count,
				   OQACProc on_qu_array_completion,
				   void*    qa_data,
				   string   user_cmd,
				   void* user_data)
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

    // Process command
    state = BusyOnCmd;
    callHandlers(ReadyForQuestion, (void *)false);
    user_cmd += '\n';
    write((const char *)user_cmd, user_cmd.length());
    flush();

    return true;
}


// ***************************************************************************
// Send CMD to GDB; upon completion, call ON_QUESTION_COMPLETION with QU_DATA
bool GDBAgent::send_question (string  cmd,
			      OQCProc on_question_completion,
			      void*   qu_data)
{
    if (state != ReadyWithPrompt) 
	return false;

    state = BusyOnQuestion;
    callHandlers(ReadyForQuestion, (void *)false);
    callHandlers(ReadyForCmd, (void *)false);

    _on_question_completion = on_question_completion;
    _qu_data = qu_data;
    complete_answer = "";

    cmd += '\n';
    write((const char *)cmd, cmd.length());
    flush();

    return true;
}

// ***************************************************************************
// Send CMDS to GDB; upon completion, call ON_QU_ARRAY_COMPLETION with QU_DATAS
bool GDBAgent::send_qu_array (string   cmds [],
			      void*    qu_datas [],
			      int      qu_count,
			      OQACProc on_qu_array_completion,
			      void*    qa_data)
{
    if (qu_count == 0)
	return true;
    if (state != ReadyWithPrompt)
	return false;

    state = BusyOnQuArray;
    callHandlers(ReadyForQuestion, (void *)false);
    callHandlers(ReadyForCmd, (void *)false);

    init_qu_array (cmds, qu_datas, qu_count, on_qu_array_completion, qa_data);
    
    // Send first question
    write((const char *)cmd_array[0], cmd_array[0].length());
    flush();

    return true;
}

// ***************************************************************************
// Add handlers for tracing GDB I/O
bool GDBAgent::trace_dialog (bool val)
{
    if (val && !trace_dialog())
    {
 	addHandler(Input,  traceInputHP);     // GDB => DDD
 	addHandler(Output, traceOutputHP);    // DDD => GDB
 	addHandler(Error,  traceErrorHP);     // GDB Errors => DDD
    }
    else if (!val && trace_dialog())
    {
 	removeHandler(Input,  traceInputHP);  // GDB => DDD
 	removeHandler(Output, traceOutputHP); // DDD => GDB
 	removeHandler(Error,  traceErrorHP);  // GDB Errors => DDD
    }

    return _trace_dialog = val;
}

// ***************************************************************************
// Initialize GDB question array
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
// Return true iff ANSWER ends with primary prompt.
bool GDBAgent::ends_with_prompt (const string& answer)
{
    switch (type())
    {
    case DBX:
    case GDB:
	{
	    // Any line ending in `(gdb) ' or `(dbx) ' is a prompt.
	    static regex rxprompt(".*[(][^ )]*db[^ )]*[)] ");
	    if (answer.matches(rxprompt))
	    {
		last_prompt = answer;
		last_prompt = last_prompt.from('(', -1);
		return true;
	    }
	    return false;
	}

    case XDB:
	{
	    // Any line equal to `>' is a prompt.
	    unsigned beginning_of_line = answer.index('\n', -1) + 1;
	    return beginning_of_line < answer.length()
		&& answer.length() > 0
		&& answer[beginning_of_line] == '>';
	}
    }

    return false;		// Never reached
}

// ***************************************************************************

static bool ends_in(const string& answer, const string& prompt)
{
    return answer.contains(prompt, answer.length() - prompt.length());
}

// Return true iff ANSWER ends with secondary prompt.
bool GDBAgent::ends_with_secondary_prompt (const string& answer)
{
    switch (type())
    {
    case DBX:
	if (ends_in(answer, "]: "))
	{
	    // AIX DBX issues `Select one of [FROM - TO]: ' in the last line
	    // Reported by Jonathan Edwards <edwards@intranet.com>
	    static regex rxselect("Select one of \\[[0-9]+ - [0-9]+\\]: ");
	    int index = answer.index(rxselect, -1);
	    if (index >= 0 && answer.index('\n', index) < 0)
		return true;
	}

	// Prompt is `> ' at beginning of line
	return answer == "> " || ends_in(answer, "\n> ");

    case GDB:
	// Prompt is `> ' at beginning of line
	return answer == "> " || ends_in(answer, "\n> ");
	
    case XDB:
	// Is there any secondary prompt in XDB? (FIXME)
	return false;
    }

    return false;		// Never reached
}


// ***************************************************************************
// Check if ANSWER requires an immediate reply; return it.
string GDBAgent::requires_reply (const string& answer)
{
    // GDB says: `---Type <return> to continue, or q <return> to quit---'
    // DBX 3.0 says: `line N', `(END)', `(press RETURN)', and `More (n if no)?'
    // XDB says: `--More--' and `Hit RETURN to continue'.
    // Escape sequences may also be embedded, but the prompt never
    // ends in a newline.

    if (answer.contains('\n', -1) || ends_with_prompt(answer))
	return "";

    static regex RXq(".*[(]END[)][^\n]*");
    if (answer.matches(RXq))
	return "q";		// Stop this

    static regex RXspace(
	 ".*(--More--|line [0-9])[^\n]*");
    if (answer.matches(RXspace))
	return " ";		// Keep on scrolling

    static regex 
	RXreturn(".*([(]press RETURN[)]"
		 "|Hit RETURN to continue"
		 "|Type <return> to continue"
		 "|More [(]n if no[)][?])[^\n]*");
    if (answer.matches(RXreturn))
	return "\n";		// Keep on scrolling

    return "";
}


// ***************************************************************************
// Normalize answer - handle control characters, remove comments and prompt
void GDBAgent::normalize(string& answer)
{
    strip_control(answer);
    strip_comments(answer);
    cut_off_prompt(answer);

#if 0
    if (trace_dialog())
    {
	DataLength dl(answer, answer.length());
	trace("<< ", &dl);
    }
#endif
}

// Remove GDB prompt
void GDBAgent::cut_off_prompt(string& answer)
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

// Strip annoying DBX comments
void GDBAgent::strip_comments(string& s)
{
    // These problems occur in Sun DBX 3.x only.
    if (!has_print_r_option() || verbatim())
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
			    s.at(int(i), int(end - i)) = "";
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

// Strip control characters
void GDBAgent::strip_control(string& answer)
{
    int source_index = 0;
    int target_index = 0;

    for (source_index = 0; source_index < int(answer.length()); source_index++)
    {
	switch (answer[source_index])
	{
	case '\b':
	    // Delete last character
	    if (target_index > 0 && answer[target_index - 1] != '\n')
		target_index--;
	    break;

	case '\r':
	    if (source_index + 1 < int(answer.length())
		&& (answer[source_index + 1] == '\n'
		    || answer[source_index + 1] == '\r'))
	    {
		// Followed by '\n' or '\r' -- ignore
		break;
	    }

	    // Return to beginning of line
	    while (target_index > 0 && answer[target_index - 1] != '\n')
		target_index--;
	    break;

	default:
	    // Leave character unchanged
	    answer[target_index++] = answer[source_index];
	    break;
	}
    }

    answer = answer.before(target_index);
}

//-----------------------------------------------------------------------------
// Event handlers
//-----------------------------------------------------------------------------

// ***************************************************************************
// Received data from GDB
//
void GDBAgent::InputHP(Agent *agent, void *, void *call_data)
{
    GDBAgent* gdb = ptr_cast(GDBAgent, agent);
    assert(gdb != 0);

    DataLength* dl = (DataLength *) call_data;
    string answer(dl->data, dl->length);

    string reply = gdb->requires_reply(answer);
    if (reply != "")
    {
	// Oops - this should not happen.  Just hit the reply key.
	gdb->write(reply, reply.length());
	gdb->flush();

	// Ignore the `more' prompt
	answer += '\r';
    }

    if (gdb->ends_with_secondary_prompt(answer))
    {
	// GDB requires more information here: probably the
	// selection of an ambiguous C++ name.
	// We simply try the first alternative here:
	// - in GDB, this means `all';
	// - in DBX and XDB, this is a non-deterministic selection.

	ReplyRequiredInfo info;
	info.question = answer;
	info.reply    = "1\n";

	// Allow handlers to override the default reply
	gdb->callHandlers(ReplyRequired, (void *)&info);

	// Send reply immediately
	gdb->write(info.reply, info.reply.length());
	gdb->flush();

	// Ignore the selection
	answer = info.question;
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
	    gdb->normalize(answer);

	    if (answer != "" && gdb->_on_answer != 0)
		gdb->_on_answer (answer, gdb->_user_data);

            // Set new state and call answer procedure
	    if (gdb->state == BusyOnInitialCmds)
	    {
		if (!gdb->questions_waiting) 
		{
		    gdb->state = ReadyWithPrompt;
		    gdb->callHandlers(ReadyForCmd, (void *)true);
		    gdb->callHandlers(ReadyForQuestion, (void *)true);

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
		gdb->callHandlers(ReadyForQuestion, (void *)true);

		if (gdb->_on_answer_completion != 0)
		    gdb->_on_answer_completion (gdb->_user_data);
	    }
	    else
	    {
		gdb->state = BusyOnQuArray;
		gdb->callHandlers(ReadyForCmd, (void *)false);

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
	    gdb->complete_answer += answer;
	    gdb->normalize(gdb->complete_answer);

            // Set new state
	    gdb->state = ReadyWithPrompt;
	    gdb->callHandlers(ReadyForQuestion, (void *)true);
	    gdb->callHandlers(ReadyForCmd, (void *)true);

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
	    gdb->complete_answers[gdb->qu_index] += answer;
	    gdb->normalize(gdb->complete_answers[gdb->qu_index]);

            // Set new state
	    gdb->qu_index++;

	    if (gdb->qu_index == gdb->_qu_count)
	    {
		// Received all answers
		delete[] gdb->cmd_array;

		// Set new state
		gdb->state = ReadyWithPrompt;
		gdb->callHandlers(ReadyForCmd, (void *)true);

		if (gdb->questions_waiting)
		{
		    // We did not call the OACProc yet.

		    if (gdb->_on_answer_completion != 0)
			gdb->_on_answer_completion (gdb->_user_data);

		    gdb->questions_waiting = false;
		}

		gdb->callHandlers(ReadyForQuestion, (void *)true);

		if (gdb->_on_qu_array_completion != 0)
		{
		    gdb->_on_qu_array_completion (gdb->complete_answers, 
						  gdb->_qu_datas,
						  gdb->_qu_count,
						  gdb->_qa_data);
		}
	    }
	    else
	    {
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
string GDBAgent::print_command(string expr, bool internal) const
{
    string cmd;

    switch (type())
    {
    case GDB:
    case DBX:
	if (internal && has_output_command())
	    cmd = "output";
	else
	    cmd = "print";
	if (has_print_r_option())
	    cmd += " -r";
	break;

    case XDB:
	cmd = "p";
	break;
    }

    if (expr != "")
    {
	if (!has_named_values())
	{
	    switch (type())
	    {
	    case DBX:
		cmd += " " + quote(expr + " =") + ",";
		break;

	    case GDB:
	    case XDB:
		cmd = echo_command(expr + " = ") + "; " + cmd;
		break;
	    }
	}

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
    if (has_print_r_option() && expr != "")
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
	if (has_where_h_option())
	    return "where -h";
	else
	    return "where";
	
    case XDB:
	return "t";
    }

    return "";			// Never reached
}

string GDBAgent::info_locals_command() const
{
    switch (type())
    {
    case GDB:
	return "info locals";

    case DBX:
	return "dump";

    case XDB:
	return "l";
    }

    return "";			// Never reached
}

string GDBAgent::info_args_command() const
{
    switch (type())
    {
    case GDB:
	return "info args";

    case DBX:
    case XDB:
	return info_locals_command();
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
	return "echo " + cook(text);

    case DBX:
	return print_command() + " " + quote(text);

    case XDB:
	return quote(text);
    }

    return "";			// Never reached
}

// Prefer `ptype' on `whatis' in GDB
string GDBAgent::whatis_command(string text) const
{
    switch (type())
    {
    case GDB:
	return "ptype " + text;

    case DBX:
	if (has_print_r_option())
	    return "whatis -r " + text;
	else
	    return "whatis " + text;

    case XDB:
	return "p " + text + "\\T";
    }

    return "";			// Never reached
}


// Return PREFIX + EXPR, parenthesizing EXPR if needed
string GDBAgent::prepend_prefix(const string& prefix, const string& expr)
{
    if (expr.matches(rxidentifier)
	|| expr.contains("(", 0) && expr.contains(")", -1))
	return prefix + expr;
    else if (expr == "")
	return prefix;
    else
	return prefix + "(" + expr + ")";
}

// Return EXPR + SUFFIX, parenthesizing EXPR if needed
string GDBAgent::append_suffix(const string& expr, const string& suffix)
{
    if (expr.matches(rxidentifier)
	|| expr.contains("(", 0) && expr.contains(")", -1))
	return expr + suffix;
    else if (expr == "")
	return suffix;
    else
	return "(" + expr + ")" + suffix;
}

// Dereference an expression.
string GDBAgent::dereferenced_expr(string expr) const
{
    switch (program_language())
    {
    case LANGUAGE_C:
	return prepend_prefix("*", expr);

    case LANGUAGE_FORTRAN:
	// The GDB source `f-lang.c' says `**' is the FORTRAN
	// indirection operator.  Well, if GDB wants it so...
	return prepend_prefix("**", expr);

    case LANGUAGE_CHILL:
	return append_suffix(expr, "->");

    case LANGUAGE_PASCAL:
	return append_suffix(expr, "^");

    case LANGUAGE_OTHER:
	return "";		// All other languages
    }

    return "";			// All other languages
}

// Give the address of an expression.
string GDBAgent::address_expr(string expr) const
{
    switch (program_language())
    {
    case LANGUAGE_C:
	return prepend_prefix("&", expr);

    case LANGUAGE_PASCAL:
	return "ADR(" + expr + ")"; // Modula-2 address operator

    case LANGUAGE_CHILL:	// FIXME: untested.
	return prepend_prefix("->", expr);

    case LANGUAGE_FORTRAN:	// FIXME
    case LANGUAGE_OTHER:
	return "";		// All other languages
    }

    return "";			// All other languages
}

// Return assignment command
string GDBAgent::assign_command(string var, string expr) const
{
    string cmd;

    switch (type())
    {
    case GDB:
	cmd = "set variable";
	break;

    case DBX:
	cmd = "assign";
	break;

    case XDB:
	cmd = "pq";
	break;
    }

    cmd += " " + var + " ";

    switch (program_language())
    {
    case LANGUAGE_C:
    case LANGUAGE_FORTRAN:
    case LANGUAGE_OTHER:
	cmd += "=";
	break;

    case LANGUAGE_PASCAL:
    case LANGUAGE_CHILL:
	cmd += ":=";
	break;
    }

    return cmd + " " + expr;
}

// Return disassemble command
string GDBAgent::disassemble_command(string pc) const
{
    if (gdb->type() != GDB)
	return "";

    // In C, hexadecimal integers are specified by a leading "0x".
    // In Modula-2, hexadecimal integers are specified by a trailing "H".
    // In Chill, hexadecimal integers are specified by a leading "H'".

    pc.downcase();
    if (pc.contains("0", 0))
	pc = pc.after("0");
    if (pc.contains("x", 0))
	pc = pc.after("x");
    if (pc.contains("h'", 0))
	pc = pc.after("h'");
    if (pc.contains("h", -1))
	pc = pc.before(int(pc.length()) - 1);

    switch (program_language())
    {
    case LANGUAGE_C:
    case LANGUAGE_FORTRAN:
    case LANGUAGE_OTHER:
	return "disassemble 0x" + pc;

    case LANGUAGE_CHILL:
	return "disassemble H'0" + upcase(pc);

    case LANGUAGE_PASCAL:
	return "disassemble 0" + upcase(pc) + "H";
    }

    return "";			// All other languages
}


// ***************************************************************************
ProgramLanguage GDBAgent::program_language(string text)
{
    text.downcase();

    if (text.contains("language"))
    {
	if (text.contains("mod")
	    || text.contains("pascal")
	    || text.contains("ada")
	    || text.contains("m2")
	    || text.contains("m3"))
	{
	    program_language(LANGUAGE_PASCAL);
	}
	else if (text.contains("fortran"))
	{
	    program_language(LANGUAGE_FORTRAN);
	}
	else if (text.contains("chill"))
	{
	    program_language(LANGUAGE_CHILL);
	}
	else if (text.contains("c"))
	{
	    program_language(LANGUAGE_C);
	}
	else if (text.contains("auto"))
	{
	    // Do nothing -- keep old setting
	}
	else
	{
	    // Default setting
	    program_language(LANGUAGE_C);
	}
    }

    return program_language();
}

// ***************************************************************************
// Handle error messages
//
void GDBAgent::PanicHP (Agent *source, void *, void *call_data)
{
    string msg = (char *)call_data;

    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    if (gdb != 0)
	cerr << gdb->prompt() << msg << "\n";
    else
	cerr << source->path() << ": " << msg << "\n";
}

// ***************************************************************************

// Make sure the GDB child does not inherit the current terminal settings.
int GDBAgent::setupChildCommunication()
{
    // Nothing special...
    return TTYAgent::setupChildCommunication();
}
