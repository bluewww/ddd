// $Id$
// Communicate with separate GDB process

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char GDBAgent_rcsid[] =
    "$Id$";

// From: hobson@pipper.enet.dec.com
// Subject: And the Twelve Bugs of Christmas ....
// Keywords: topical, heard it, computers, chuckle, originally
//           appeared in fourth quarter, 1992
// Newsgroups: rec.humor.funny.reruns
// Date: Tue, 23 Dec 97 13:20:02 EST
// 
// For the first bug of Christmas, my manager said to me
//      See if they can do it again.
// 
// For the second bug of Christmas, my manager said to me
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the third bug of Christmas, my manager said to me
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the fourth bug of Christmas, my manager said to me
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the fifth bug of Christmas, my manager said to me
//      Ask for a dump
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the sixth bug of Christmas, my manager said to me
//      Reinstall the software
//      Ask for a dump
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the seventh bug of Christmas, my manager said to me
//      Say they need an upgrade
//      Reinstall the software
//      Ask for a dump
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the eighth bug of Christmas, my manager said to me
//      Find a way around it
//      Say they need an upgrade
//      Reinstall the software
//      Ask for a dump
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the ninth bug of Christmas, my manager said to me
//      Blame it on the hardware
//      Find a way around it
//      Say they need an upgrade
//      Reinstall the software
//      Ask for a dump
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the tenth bug of Christmas, my manager said to me
//      Change the documentation
//      Blame it on the hardware
//      Find a way around it
//      Say they need an upgrade
//      Reinstall the software
//      Ask for a dump
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the eleventh bug of Christmas, my manager said to me
//      Say it's not supported
//      Change the documentation
//      Blame it on the hardware
//      Find a way around it
//      Say they need an upgrade
//      Reinstall the software
//      Ask for a dump
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.
// 
// For the twelfth bug of Christmas, my manager said to me
//      Tell them it's a feature
//      Say it's not supported
//      Change the documentation
//      Blame it on the hardware
//      Find a way around it
//      Say they need an upgrade
//      Reinstall the software
//      Ask for a dump
//      Run with the debugger
//      Try to reproduce it
//      Ask them how they did it and
//      See if they can do it again.

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// GDBAgent implementation
//-----------------------------------------------------------------------------

#include "GDBAgent.h"
#include "cook.h"
#include "ddd.h"
#include "string-fun.h"
#include "regexps.h"
#include "index.h"

#include <stdlib.h>
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
//-----------------------------------------------------------------------------

DebuggerType debugger_type(const string& type)
{
    if (type.contains("gdb"))
	return GDB;
    if (type.contains("dbx"))
	return DBX;
    if (type.contains("xdb"))
	return XDB;
    if (type.contains("jdb"))
	return JDB;

    cerr << "Unknown debugger type " << quote(type) << "\n";
    exit(EXIT_FAILURE);

    // Never reached
    return GDB;
}


//-----------------------------------------------------------------------------
// Construction and setup
//-----------------------------------------------------------------------------

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
      _has_func_command(tp == DBX),
      _has_run_io_command(false),
      _has_print_r_option(false),
      _has_output_command(false),
      _has_where_h_option(false),
      _has_display_command(tp == GDB || tp == DBX),
      _has_clear_command(tp == GDB || tp == DBX || tp == JDB),
      _has_handler_command(false),
      _has_pwd_command(tp == GDB || tp == DBX),
      _has_setenv_command(tp == DBX),
      _has_edit_command(tp == DBX),
      _has_make_command(tp == GDB || tp == DBX),
      _has_jump_command(tp == GDB || tp == DBX || tp == XDB),
      _has_regs_command(tp == GDB),
      _has_watch_command(0),	// see below
      _has_named_values(tp == GDB || tp == DBX || tp == JDB),
      _has_when_command(tp == DBX),
      _has_when_semicolon(tp == DBX),
      _wants_delete_comma(false),
      _has_err_redirection(tp == GDB || tp == DBX || tp == XDB),
      _has_givenfile_command(false),
      _has_cont_sig_command(false),
      _program_language(tp == JDB ? LANGUAGE_JAVA : LANGUAGE_C),
      _verbatim(false),
      _recording(false),
      _detect_echos(true),
      last_prompt(""),
      last_written(""),
      echoed_characters(-1),
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
    // Suppress default error handlers
    removeAllHandlers(Panic);
    removeAllHandlers(Strange);
    removeAllHandlers(Died);

    // Add own handlers
    addHandler(Panic,   PanicHP);
    addHandler(Strange, StrangeHP);
    addHandler(Died,    DiedHP);
    addHandler(Input,   InputHP);

    // Add trace handlers
    addHandler(Input,  traceInputHP);     // GDB => DDD
    addHandler(Output, traceOutputHP);    // DDD => GDB
    addHandler(Error,  traceErrorHP);     // GDB Errors => DDD

    // Setup watch mode
    if (type() == GDB)
	_has_watch_command = WATCH_CHANGE | WATCH_READ | WATCH_WRITE;
    else if (type() == DBX)
	_has_watch_command = WATCH_CHANGE;
    else
	_has_watch_command = 0;
}


// Copy constructor
GDBAgent::GDBAgent(const GDBAgent& gdb)
    : TTYAgent(gdb),
      state(gdb.state),
      _type(gdb.type()),
      _user_data(0),
      _has_frame_command(gdb.has_frame_command()),
      _has_func_command(gdb.has_func_command()),
      _has_run_io_command(gdb.has_run_io_command()),
      _has_print_r_option(gdb.has_print_r_option()),
      _has_output_command(gdb.has_output_command()),
      _has_where_h_option(gdb.has_where_h_option()),
      _has_display_command(gdb.has_display_command()),
      _has_clear_command(gdb.has_clear_command()),
      _has_handler_command(gdb.has_handler_command()),
      _has_pwd_command(gdb.has_pwd_command()),
      _has_setenv_command(gdb.has_setenv_command()),
      _has_edit_command(gdb.has_edit_command()),
      _has_make_command(gdb.has_make_command()),
      _has_jump_command(gdb.has_jump_command()),
      _has_regs_command(gdb.has_regs_command()),
      _has_watch_command(gdb.has_watch_command()),
      _has_named_values(gdb.has_named_values()),
      _has_when_command(gdb.has_when_command()),
      _has_when_semicolon(gdb.has_when_semicolon()),
      _wants_delete_comma(gdb.wants_delete_comma()),
      _has_err_redirection(gdb.has_err_redirection()),
      _has_givenfile_command(gdb.has_givenfile_command()),
      _has_cont_sig_command(gdb.has_cont_sig_command()),
      _program_language(gdb.program_language()),
      _verbatim(gdb.verbatim()),
      _recording(gdb.recording()),
      _detect_echos(gdb.detect_echos()),
      last_prompt(""),
      last_written(""),
      echoed_characters(-1),
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
    case JDB:
	return "JDB";
    }

    return "debugger";
}

// Trace communication
void GDBAgent::trace(char *prefix, void *call_data) const
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

    dddlog << prefix << s << '\n';
    dddlog.flush();
}
    
void GDBAgent::traceInputHP(Agent *source, void *, void *call_data)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    if (gdb != 0)
	gdb->trace("<- ", call_data);
}

void GDBAgent::traceOutputHP(Agent *source, void *, void *call_data)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    if (gdb != 0)
	gdb->trace("-> ", call_data);
}

void GDBAgent::traceErrorHP(Agent *source, void *, void *call_data)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    if (gdb != 0)
	gdb->trace("<= ", call_data);
}

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

// Start with some extra commands
void GDBAgent::start_plus (OAProc   on_answer,
			   OACProc  on_answer_completion,
			   void*    user_data,
			   const StringArray& cmds,
			   const VoidArray& qu_datas,
			   int      qu_count,
			   OQACProc on_qu_array_completion,
			   void*    qa_data)
{
    if (qu_count > 0) {
	questions_waiting = true;
	init_qu_array(cmds, qu_datas, qu_count, 
		      on_qu_array_completion, qa_data);
    }

    do_start(on_answer, on_answer_completion, user_data);
}

// Destructor
GDBAgent::~GDBAgent ()
{
    shutdown();
}



//-----------------------------------------------------------------------------
// Command sending
//-----------------------------------------------------------------------------

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
	complete_answer = "";
	callHandlers(ReadyForQuestion, (void *)false);
	cmd += '\n';
	write(cmd);
	flush();

	return true;

    case BusyOnQuestion:
    case BusyOnQuArray:
    case BusyOnCmd:
	break;
    }

    return false;
}

// Send CMD to GDB (unconditionally), associated with USER_DATA.
bool GDBAgent::send_user_ctrl_cmd(string cmd, void *user_data)
{
    if (user_data)
	_user_data = user_data;

    // Upon ^D, GDB is no more in state to receive commands.
    // Expect a new prompt to appear.
    if (cmd == '\004' && state == ReadyWithPrompt)
    {
	state = BusyOnCmd;
	complete_answer = "";
    }

    write(cmd);
    flush();
    return true;
}

// Send command array CMDS to GDB, associated with QU_DATAS.
bool GDBAgent::send_user_cmd_plus (const StringArray& cmds,
				   const VoidArray& qu_datas,
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
    if (qu_count > 0)
    {
	questions_waiting = true;
	init_qu_array(cmds, qu_datas, qu_count,
		      on_qu_array_completion, qa_data);
    }

    // Process command
    state = BusyOnCmd;
    complete_answer = "";
    callHandlers(ReadyForQuestion, (void *)false);
    user_cmd += '\n';
    write(user_cmd);
    flush();

    return true;
}

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
    write(cmd);
    flush();

    return true;
}

// Send CMDS to GDB; upon completion, call ON_QU_ARRAY_COMPLETION with QU_DATAS
bool GDBAgent::send_qu_array (const StringArray& cmds,
			      const VoidArray& qu_datas,
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

    init_qu_array(cmds, qu_datas, qu_count, on_qu_array_completion, qa_data);
    
    // Send first question
    write(cmd_array[0]);
    flush();

    return true;
}

// Initialize GDB question array
void GDBAgent::init_qu_array (const StringArray& cmds,
			      const VoidArray& qu_datas,
			      int      qu_count,
			      OQACProc on_qu_array_completion,
			      void*    qa_data)
{
    _on_qu_array_completion = on_qu_array_completion;
    qu_index  = 0;
    _qu_count = qu_count;
    _qa_data  = qa_data;

    StringArray empty_s;
    VoidArray   empty_v;

    complete_answers = empty_s;
    cmd_array        = empty_s;
    _qu_datas        = empty_v;
    for (int i = 0; i < qu_count; i++)
    {
	complete_answers += "";
	cmd_array        += cmds[i] + '\n';
	_qu_datas        += qu_datas[i];
    }
}


//-----------------------------------------------------------------------------
// Prompt Recognition
//-----------------------------------------------------------------------------

// Return true iff ANSWER ends with primary prompt.
bool GDBAgent::ends_with_prompt (const string& ans)
{
    string answer = ans;
    strip_control(answer);

    switch (type())
    {
    case GDB:
	// GDB reads in breakpoint commands using a `>' prompt
	if (recording() && answer.contains('>', -1))
	{
	    last_prompt = ">";
	    return true;
	}

	// FALL THROUGH
    case DBX:
    {
	// Any line ending in `(gdb) ' or `(dbx) ' is a prompt.
	int i = answer.length() - 1;
	if (i < 0 || answer[i] != ' ')
	    return false;

	while (i >= 0 && answer[i] != '\n' && answer[i] != '(')
	    i--;
	if (i < 0 || answer[i] != '(')
	    return false;

	string possible_prompt = answer.from(i);
#if RUNTIME_REGEX
	static regex rxprompt("[(][^ )]*db[^ )]*[)] ");
#endif
	if (possible_prompt.matches(rxprompt))
	{
	    last_prompt = possible_prompt;
	    recording(false);
	    return true;
	}
	return false;
    }

    case XDB:
    {
	// Any line equal to `>' is a prompt.
	unsigned beginning_of_line = answer.index('\n', -1) + 1;
	if (beginning_of_line < answer.length()
	    && answer.length() > 0
	    && answer[beginning_of_line] == '>')
	{
	    last_prompt = ">";
	    return true;
	}
	return false;
    }

    case JDB:
    {
	// JDB prompts using "> " or "THREAD[DEPTH] ".  All these
	// prompts may also occur asynchronously.

#if RUNTIME_REGEX
	// Threaded prompt: "THREAD[DEPTH] "
	static regex rxjdbprompt        
	    ("[a-zA-Z][a-zA-Z0-9 ]*[a-zA-Z0-9][[][1-9][0-9]*[]] ");
	// Same, but in reverse
	static regex rxjdbprompt_reverse
	    (" []][0-9]*[1-9][[][a-zA-Z0-9][a-zA-Z0-9 ]*[a-zA-Z]");
	// Non-threaded prompt: "[DEPTH] " or "> "
	static regex rxjdbprompt_nothread
	    ("(>|[[][1-9][0-9]*[]]) ");
#endif

	// Check for threaded prompt at the end of the last line
	string reverse_answer = reverse(answer);
	int match_len = rxjdbprompt_reverse.match(reverse_answer.chars(), 
						  reverse_answer.length(), 0);
	if (match_len > 0)
	{
	    last_prompt = reverse(reverse_answer.at(0, match_len));
	    return true;
	}

	// Check for non-threaded prompt as the last line
	int beginning_of_line = answer.index('\n', -1) + 1;
	string possible_prompt = ((string &) answer).from(beginning_of_line);
	if (possible_prompt.matches(rxjdbprompt_nothread))
	{
	    last_prompt = possible_prompt;
	    return true;
	}

	// Check for threaded prompt at the beginning of each line
	int last_nl = answer.length() - 1;
	while (last_nl >= 0)
	{
	    last_nl = answer.index('\n', last_nl - answer.length());
	    int beginning_of_line = last_nl + 1;

	    match_len = rxjdbprompt.match(answer.chars(), answer.length(), 
					  beginning_of_line);
	    if (match_len > 0)
	    {
		int i = beginning_of_line + match_len;
		while (i < int(answer.length()) && isspace(answer[i]))
		    i++;
		if (i < int(answer.length()) && answer[i] == '=')
		{
		    // This is no prompt, but something like `dates[1] = 33'.
		}
		else
		{
		    last_prompt = answer.at(beginning_of_line, match_len);
		    return true;
		}
	    }

	    last_nl--;
	}

	return false;
    }
    }

    return false;		// Never reached
}


static bool ends_in(const string& answer, const string& prompt)
{
    return answer.contains(prompt, answer.length() - prompt.length());
}

// Return true iff ANSWER ends with secondary prompt.
bool GDBAgent::ends_with_secondary_prompt (const string& ans)
{
    string answer = ans;
    strip_control(answer);

    switch (type())
    {
    case DBX:
	if (ends_in(answer, "]: "))
	{
	    // AIX DBX issues `Select one of [FROM - TO]: ' in the last line
	    // Reported by Jonathan Edwards <edwards@intranet.com>
#if RUNTIME_REGEX
	    static regex rxselect("Select one of \\[[0-9]+ - [0-9]+\\]: ");
#endif
	    int idx = index(answer, rxselect, "Select one of ", -1);
	    if (idx >= 0 && answer.index('\n', idx) < 0)
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

    case JDB:
	// Is there any secondary prompt in XDB? (FIXME)
	return false;
    }

    return false;		// Never reached
}


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
    int last_line_index = answer.index('\n', -1) + 1;

    string last_line = answer.chars() + last_line_index;
    last_line.downcase();
    strip_control(last_line);

    if (last_line.contains("end") 
	|| last_line.contains("line")
	|| last_line.contains("more")
	|| last_line.contains("return"))
    {
#if RUNTIME_REGEX
	static regex rxq(".*[(]END[)][^\n]*");
#endif
	if (answer.matches(rxq, last_line_index))
	    return "q";		// Stop this

#if RUNTIME_REGEX
	static regex rxspace(".*(--More--|line [0-9])[^\n]*");
#endif
	if (answer.matches(rxspace, last_line_index))
	    return " ";		// Keep on scrolling

#if RUNTIME_REGEX
	static regex rxreturn(".*([(]press RETURN[)]"
			      "|Hit RETURN to continue"
			      "|Type <return> to continue"
			      "|More [(]n if no[)][?])[^\n]*");
#endif
	if (answer.matches(rxreturn, last_line_index))
	    return "\n";		// Keep on scrolling

	if (type() == XDB)
	{
	    // Added regular expression for "Standard input: END" to
	    // GDBAgent::requires_reply 
	    // -- wiegand@kong.gsfc.nasa.gov (Robert Wiegand)
#if RUNTIME_REGEX
	    static regex rxxdb(".*Standard input: END.*");
#endif
	    if (answer.matches(rxxdb, last_line_index))
		return "\n";	// Keep on scrolling
	}
    }

    return "";
}



//-----------------------------------------------------------------------------
// Filters
//-----------------------------------------------------------------------------

// Normalize answer - handle control characters, remove comments and prompt
void GDBAgent::normalize_answer(string& answer) const
{
    strip_control(answer);
    strip_dbx_comments(answer);
    cut_off_prompt(answer);
}

// Remove GDB prompt
void GDBAgent::cut_off_prompt(string& answer) const
{
    switch (type())
    {
    case GDB:
	if (recording() && answer.contains('>', -1))
	{
	    answer = answer.before('>', -1);
	    break;
	}

	// FALL THROUGH
    case DBX:
	answer = answer.before('(', -1);
	break;

    case XDB:
	answer = answer.before('>', -1);
	break;

    case JDB:
    {
	// Check for prompt at the end of the last line
	if (answer.contains(last_prompt, -1))
	{
	    answer = answer.before(int(answer.length()) - 
				   int(last_prompt.length()));
	}
	break;
    }
    }
}

// Strip annoying DBX comments
void GDBAgent::strip_dbx_comments(string& s) const
{
    if (type() == DBX)
    {
	// Weed out annoying DBX warnings like
	// `WARNING: terminal cannot clear to end of line'
	for (;;)
	{
	    int warning = s.index("WARNING: terminal cannot ");
	    if (warning < 0)
		break;
	    int eol = s.index('\n', warning) + 1;
	    if (eol <= 0)
		eol = s.length();
	    s(warning, eol - warning) = "";
	}
    }

    // If we're verbatim, leave output unchanged.
    if (verbatim())
	return;

    // All remaining problems occur in Sun DBX 3.x only.
    if (!has_print_r_option())
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

#if RUNTIME_REGEX
	static regex rxdbxwarn1("dbx: warning:[^\n]*"
				"option only recognized for[^\n]*\n");
	static regex rxdbxwarn2("dbx: warning:[^\n]*"
				"unknown language[^\n]*\n");
#endif
	s.gsub(rxdbxwarn1, "");
	s.gsub(rxdbxwarn2, "");
    }
}

// Strip control characters
void GDBAgent::strip_control(string& answer) const
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

	case '\033':
	    // XDB `more' sends VT100 escape sequences like `\e[m',
	    // `\e[22;1H', `\e[7m', `\e[K', regardless of TERM
	    // settings.  We simply weed out everything up to and
	    // including to the next letter.
	    while (!isalpha(answer[source_index]))
		source_index++;
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

// Received data from GDB

// Number of characters to be echoed for echo detection
const int ECHO_THRESHOLD = 4;

void GDBAgent::InputHP(Agent *agent, void *, void *call_data)
{
    GDBAgent* gdb = ptr_cast(GDBAgent, agent);
    assert(gdb != 0);

    DataLength* dl = (DataLength *) call_data;
    string answer(dl->data, dl->length);

    gdb->handle_input(answer);
}

void GDBAgent::handle_echo(string& answer)
{
    // If we don't detect echos, leave output unchanged.
    if (!detect_echos())
	return;

    // Check for echoed characters.  Every now and then, the TTY setup
    // fails such that we get characters echoed back.  This may also
    // happen with remote connections.
    if (echoed_characters >= 0)
    {
	int i = 0;
	int e = echoed_characters;
	while (i < int(answer.length()) && e < int(last_written.length()))
	{
	    if (answer[i] == '\r')
	    {
		// Ignore `\r' in comparisons
		i++;
	    }
	    else if (answer[i] == last_written[e])
	    {
		i++, e++;
	    }
	    else
	    {
		// No echo
		break;
	    }
	}

	if (e >= ECHO_THRESHOLD && e >= int(last_written.length()))
	{
	    // All characters last written have been echoed.
	    // => Remove echoed characters and keep on processing
	    callHandlers(EchoDetected);
	    answer = answer.from(i);
	    echoed_characters = -1;
	}
	else if (i >= ECHO_THRESHOLD && i >= int(answer.length()))
	{
	    // All characters received so far have been echoed.
	    // => Wait for further input
	    answer = "";
	    echoed_characters = e;
	}
	else
	{
	    // No echo.
	    // => Restore any echoed characters and keep on processing
	    answer.prepend(last_written.before(echoed_characters));
	    echoed_characters = -1;

	    // Disable echo detection until re-activated
	    detect_echos(false);
	}
    }
}

void GDBAgent::handle_more(string& answer)
{
    // Check for `More' prompt
    string reply = requires_reply(answer);
    if (reply != "")
    {
	// Oops - this should not happen.  Just hit the reply key.
	write(reply);
	flush();

	// Ignore the last line (containing the `More' prompt)
	int last_beginning_of_line = answer.index('\n', -1) + 1;
	answer.from(last_beginning_of_line) = "";
    }
}

void GDBAgent::handle_reply(string& answer)
{
    if (recording())
	return;

    // Check for secondary prompt
    if (ends_with_secondary_prompt(answer))
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
	callHandlers(ReplyRequired, (void *)&info);

	// Send reply immediately
	write(info.reply);
	flush();

	// Ignore the selection
	answer = info.question;
    }
}

void GDBAgent::handle_input(string& answer)
{
    handle_echo(answer);
    handle_more(answer);
    handle_reply(answer);

    // Handle all other GDB output, depending on current state.
    switch (state)
    {
    case ReadyWithPrompt:
	// We have a prompt, and still get input?  Maybe this is an
	// answer to a command sent before the prompt was received -
	// or a reply to a control character (`Quit').
	strip_control(answer);
	callHandlers(AsyncAnswer, (void *)&answer);
	break;

    case BusyOnInitialCmds:
    case BusyOnCmd:
	complete_answer += answer;

	if (_on_answer != 0)
	{
	    if (ends_with_prompt(complete_answer))
		normalize_answer(answer);
	    else
		strip_control(answer);
	    _on_answer(answer, _user_data);
	}

	if (ends_with_prompt(complete_answer))
	{
            // Received complete answer (GDB issued prompt)

            // Set new state and call answer procedure
	    if (state == BusyOnInitialCmds)
	    {
		if (!questions_waiting) 
		{
		    state = ReadyWithPrompt;
		    callHandlers(ReadyForCmd, (void *)true);
		    callHandlers(ReadyForQuestion, (void *)true);

		    if (_on_answer_completion != 0)
			_on_answer_completion (_user_data);
		}
		else
		{
		    state = BusyOnQuArray;

		    // Send first question
		    write(cmd_array[0]);
		    flush();
		}
	    }
	    else if (!questions_waiting)
	    {
		state = ReadyWithPrompt;
		callHandlers(ReadyForQuestion, (void *)true);

		if (_on_answer_completion != 0)
		    _on_answer_completion (_user_data);
	    }
	    else
	    {
		state = BusyOnQuArray;
		callHandlers(ReadyForCmd, (void *)false);

		// Send first question
		write(cmd_array[0]);
		flush();
	    }
	}
	break;

    case BusyOnQuestion:
	complete_answer += answer;

	if (ends_with_prompt(complete_answer))
	{
            // Received complete answer (GDB issued prompt)
	    normalize_answer(complete_answer);

            // Set new state
	    state = ReadyWithPrompt;
	    callHandlers(ReadyForQuestion, (void *)true);
	    callHandlers(ReadyForCmd, (void *)true);

	    if (_on_question_completion != 0)
	    {
		// We use a local copy of COMPLETE_ANSWER here, since
		// the callback may submit a new query, overriding
		// the original value.
		string c(complete_answer);
		_on_question_completion(c, _qu_data);
	    }
	}
	break;

    case BusyOnQuArray:
	complete_answers[qu_index] += answer;

	if (ends_with_prompt(complete_answers[qu_index]))
	{
            // Answer is complete (GDB issued prompt)
	    normalize_answer(complete_answers[qu_index]);

	    if (qu_index == _qu_count - 1)
	    {
		// Received all answers -- we're ready again
		state = ReadyWithPrompt;
		callHandlers(ReadyForQuestion, (void *)true);
		callHandlers(ReadyForCmd, (void *)true);

		if (questions_waiting || _on_qu_array_completion != 0)
		{
		    // We use a local copy of the answers and user
		    // data here, since the callback may submit a new
		    // query, overriding the original value.
		    StringArray answers(complete_answers);
		    VoidArray datas(_qu_datas);
		    OQACProc array_completion  = _on_qu_array_completion;
		    OACProc  answer_completion = _on_answer_completion;
		    void *array_data           = _qa_data;

		    if (questions_waiting)
		    {
			// We did not call the OACProc yet.
			questions_waiting = false;

			if (answer_completion != 0)
			    answer_completion(_user_data);
		    }
		    if (array_completion != 0)
			array_completion(answers, datas, array_data);
		}
	    }
	    else
	    {
		// Send next question
		write(cmd_array[++qu_index]);
		flush();
	    }
	}
	break;

    default:
	assert(0);
	break;
    }
}

// GDB died
void GDBAgent::DiedHP(Agent *agent, void *, void *)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, agent);
    gdb->handle_died();
}

void GDBAgent::handle_died()
{
    // We have no prompt
    last_prompt = "";

    // Call answer completion handlers
    switch (state)
    {
    case ReadyWithPrompt:
	break;

    case BusyOnInitialCmds:
    case BusyOnCmd:
	if (_on_answer_completion != 0)
	    _on_answer_completion (_user_data);
	break;

    case BusyOnQuestion:
	if (_on_question_completion != 0)
	    _on_question_completion (complete_answer, _qu_data);
	break;

    case BusyOnQuArray:
	if (_on_qu_array_completion != 0)
	    _on_qu_array_completion(complete_answers, _qu_datas, _qa_data);
	break;
    }

    // We're not ready anymore
    state = BusyOnCmd;
    complete_answer = "";
    callHandlers(ReadyForQuestion, (void *)false);
    callHandlers(ReadyForCmd,      (void *)false);
}


//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------

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

    case JDB:
	if (internal)
	    cmd = "dump";
	else
	    cmd = "print";
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

	    case JDB:
		// JDB has named values
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
string GDBAgent::where_command(int count) const
{
    string cmd;
    switch (type())
    {
    case GDB:
    case DBX:
    case JDB:
	if (has_where_h_option())
	    cmd = "where -h";
	else
	    cmd = "where";
	break;
	
    case XDB:
	cmd = "t";
	break;
    }

    if (count != 0)
	cmd += " " + itostring(count);

    return cmd;
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

    case JDB:
	return "locals";
    }

    return "";			// Never reached
}

string GDBAgent::info_args_command() const
{
    switch (type())
    {
    case GDB:
	return "info args";

    default:
	return info_locals_command();
    }

    return "";			// Never reached
}

string GDBAgent::info_display_command() const
{
    if (type() == GDB)
	return "info display";
    else
	return display_command();
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

    case JDB:
	return "";
    }

    return "";			// Never reached
}

// Some DBXes want `sh make' instead of `make'
string GDBAgent::make_command(string args) const
{
    string cmd;
    switch (type())
    {
    case GDB:
    case DBX:
	if (has_make_command())
	    cmd = "make";
	else
	    cmd = "sh make";
	break;

    case XDB:
	cmd = "!make";
	break;

    case JDB:
	return "";		// Not available
    }

    if (args == "")
	return cmd;
    else
	return cmd + " " + args;
}

// Move PC to POS
string GDBAgent::jump_command(string pos) const
{
    if (!has_jump_command())
	return "";

    switch (type())
    {
    case GDB:
	return "jump " + pos;
   
    case XDB:
    {
	if (pos.contains('*', 0))
	    pos = pos.after('*');
	return "g " + pos;
    }

    case DBX:
	return "cont at " + pos;

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Show registers
string GDBAgent::regs_command(bool all) const
{
    if (!has_regs_command())
	return "";

    switch (type())
    {
    case GDB:
	if (all)
	    return "info all-registers";
	else
	    return "info registers";
   
    case DBX:
	if (all)
	    return "regs -F";	// Sun DBX 4.0
	else
	    return "regs";	

    case XDB:
    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Watch expressions
string GDBAgent::watch_command(string expr, WatchMode w) const
{
    if ((has_watch_command() & w) != w)
	return "";

    switch (type())
    {
    case GDB:
	if ((w & WATCH_CHANGE) == WATCH_CHANGE)
	    return "watch " + expr;
	if ((w & WATCH_ACCESS) == WATCH_ACCESS)
	    return "awatch " + expr;
	if ((w & WATCH_READ) == WATCH_READ)
	    return "rwatch " + expr;
	return "";
   
    case DBX:
	if ((w & WATCH_CHANGE) == WATCH_CHANGE)
	    return "stop " + expr;
	return "";

    case XDB:
	// Not available.  (There is the `assertion' concept which is
	// similar but won't fit into the DDD GUI.)
	return "";

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}


string GDBAgent::kill_command() const
{
    switch (type())
    {
    case GDB:
    case DBX:
	return "kill";
   
    case XDB:
	return "k";

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

string GDBAgent::frame_command() const
{
    switch (type())
    {
    case GDB:
    case DBX:
	if (has_frame_command())
	    return "frame";
	else
	    return where_command(1);

    case XDB:
	return print_command("$depth");

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

string GDBAgent::frame_command(int num) const
{
    if (!has_frame_command())
	return "";

    switch (type())
    {
    case GDB:
    case DBX:
	return frame_command() + " " + itostring(num);

    case XDB:
	return "V " + itostring(num);

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

string GDBAgent::func_command() const
{
    switch (type())
    {
    case GDB:
    case XDB:
    case JDB:
	return frame_command();

    case DBX:
	return has_func_command() ? "func" : "";
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

    case JDB:
	return "";		// Not available
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

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Enable breakpoint BP
string GDBAgent::enable_command(string bp) const
{
    if (bp != "")
	bp.prepend(' ');

    switch (type())
    {
    case GDB:
	return "enable" + bp;

    case DBX:
	if (has_handler_command())
	    return "handler -enable" + bp;
	else
	    return "";

    case XDB:
	return "ab" + bp;

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Disable breakpoint BP
string GDBAgent::disable_command(string bp) const
{
    if (bp != "")
	bp.prepend(' ');

    switch (type())
    {
    case GDB:
	return "disable" + bp;

    case DBX:
	if (has_handler_command())
	    return "handler -disable" + bp;
	else
	    return "";

    case XDB:
	return "sb" + bp;

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Delete breakpoint BP
string GDBAgent::delete_command(string bp) const
{
    if (bp != "")
	bp.prepend(' ');

    switch (type())
    {
    case DBX:
    case GDB:
	return "delete" + bp;

    case XDB:
	return "db" + bp;

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Set ignore count of breakpoint BP to COUNT
string GDBAgent::ignore_command(string bp, int count) const
{
    switch (type())
    {
    case GDB:
	return "ignore " + bp + " " + itostring(count);

    case DBX:
	if (has_handler_command())
	    return "handler -count " + bp + " " + itostring(count);
	else
	    return "";

    case XDB:
	return "bc " + bp + " " + itostring(count);

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Set condition of breakpoint BP to EXPR
string GDBAgent::condition_command(string bp, string expr) const
{
    switch (type())
    {
    case GDB:
	return "condition " + bp + " " + expr;

    case DBX:
    case XDB:
    case JDB:
	return "";		// FIXME
    }

    return "";			// Never reached
}

// Return shell escape command
string GDBAgent::shell_command(string cmd) const
{
    switch (type())
    {
    case GDB:
	return "shell " + cmd;

    case DBX:
	return "sh " + cmd;

    case XDB:
	return "!" + cmd;

    case JDB:
	return "";		// Not available
    }
    return "";			// Never reached
}

// Return command to debug PROGRAM
string GDBAgent::debug_command(string program) const
{
    switch (type())
    {
    case GDB:
	return "file " + program;

    case DBX:
	if (has_givenfile_command())
	    return "givenfile " + program; // SGI DBX
	else
	    return "debug " + program;     // SUN DBX

    case XDB:
	return "";		// FIXME

    case JDB:
	return "load " + program;
    }
    return "";			// Never reached
}

// Return command to send signal SIG
string GDBAgent::signal_command(int sig) const
{
    string n = itostring(sig);

    switch (type())
    {
    case GDB:
	return "signal " + n;

    case DBX:
	if (has_cont_sig_command())
	    return "cont sig " + n; // SUN DBX
	else
	    return "cont " + n;     // Other DBXes

    case XDB:
	return "p $signal = " + n + "; C";

    case JDB:
	return "";		// Not available
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
	// GDB prints dereferenced pointers as `**X', but accepts them as `*X'.
	return prepend_prefix("*", expr);

    case LANGUAGE_JAVA:
	if (type() == GDB)
	{
	    // GDB dereferences JAVA references by prepending `*'
	    return prepend_prefix("*", expr);
	}
	else
	{
	    // JDB (and others?) dereference automatically
	    return expr;
	}
	break;

    case LANGUAGE_CHILL:
	return append_suffix(expr, "->");

    case LANGUAGE_PASCAL:
	return append_suffix(expr, "^");

    case LANGUAGE_OTHER:
	return expr;		// All other languages
    }

    return expr;		// All other languages
}

// Give the address of an expression.
string GDBAgent::address_expr(string expr) const
{
    if (expr.contains('/', 0))
	expr = expr.after(' ');

    switch (program_language())
    {
    case LANGUAGE_C:
	return prepend_prefix("&", expr);

    case LANGUAGE_PASCAL:
	return "ADR(" + expr + ")"; // Modula-2 address operator

    case LANGUAGE_CHILL:	// FIXME: untested.
	return prepend_prefix("->", expr);

    case LANGUAGE_FORTRAN:
	return prepend_prefix("&", expr);

    case LANGUAGE_JAVA:
	return "";		// Not supported in GDB

    case LANGUAGE_OTHER:
	return "";		// All other languages
    }

    return "";			// All other languages
}

// Give the index of an expression.
string GDBAgent::index_expr(string expr, int index) const
{
    switch (program_language())
    {
    case LANGUAGE_FORTRAN:
	return expr + "(" + itostring(index) + ")";

    default:
	return expr + "[" + itostring(index) + "]";
    }

    return "";			// All other languages
}

// Return default index base
int GDBAgent::default_index_base() const
{
    switch (program_language())
    {
    case LANGUAGE_FORTRAN:
    case LANGUAGE_PASCAL:
    case LANGUAGE_CHILL:
	return 1;

    case LANGUAGE_C:
    case LANGUAGE_JAVA:
    case LANGUAGE_OTHER:
	return 0;
    }

    return 0;			// Never reached
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

    case JDB:
	return "";		// Not available
    }

    cmd += " " + var + " ";

    switch (program_language())
    {
    case LANGUAGE_C:
    case LANGUAGE_JAVA:
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


void GDBAgent::normalize_address(string& addr) const
{
    // In C, hexadecimal integers are specified by a leading "0x".
    // In Modula-2, hexadecimal integers are specified by a trailing "H".
    // In Chill, hexadecimal integers are specified by a leading "H'".
    addr.downcase();
    if (addr.contains("0", 0))
	addr = addr.after("0");
    if (addr.contains("x", 0))
	addr = addr.after("x");
    if (addr.contains("h'", 0))
	addr = addr.after("h'");
    if (addr.contains("h", -1))
	addr = addr.before(int(addr.length()) - 1);

    if (addr != "")
    {
	switch (program_language())
	{
	case LANGUAGE_C:
	case LANGUAGE_JAVA:
	case LANGUAGE_FORTRAN:
	case LANGUAGE_OTHER:
	    addr.prepend("0x");
	    break;

	case LANGUAGE_CHILL:
	    addr = "H'0" + upcase(addr);
	    break;

	case LANGUAGE_PASCAL:
	    addr = "0" + upcase(addr) + "H";
	    break;
	}
    }
}

// Return disassemble command
string GDBAgent::disassemble_command(string start, string end) const
{
    if (type() != GDB)
	return "";

    normalize_address(start);
    string cmd = "disassemble " + start;

    if (end != "")
    {
	normalize_address(end);
	cmd += " " + end;
    }
    return cmd;
}


string GDBAgent::history_file() const
{
    char *_home = getenv("HOME");
    if (_home == 0)
	_home = ".";
    string home(_home);

    switch (type())
    {
    case GDB:
    {
	char *g = getenv("GDBHISTFILE");
	if (g != 0)
	    return g;
	else
	    return "./.gdb_history";
    }

    case DBX:
    case JDB:
	return "";		// Unknown

    case XDB:
    {
	char *g = getenv("XDBHIST");
	if (g != 0)
	    return g;
	else
	    return home + "/.xdbhist";
    }
    }

    return "";			// Unknown
}

// Determine language from TEXT
ProgramLanguage GDBAgent::program_language(string text)
{
    text.downcase();

    if (text.contains("language"))
    {
	text = text.after("language");
	text = text.before("\n");

	if (text.contains("fortran"))
	{
	    program_language(LANGUAGE_FORTRAN);
	}
	else if (text.contains("java"))
	{
	    program_language(LANGUAGE_JAVA);
	}
	else if (text.contains("chill"))
	{
	    program_language(LANGUAGE_CHILL);
	}
	else if (text.contains("pascal")
	    || text.contains("ada"))
	{
	    program_language(LANGUAGE_PASCAL);
	}
	else if (text.contains("c++"))
	{
	    program_language(LANGUAGE_C);
	}
	else if (text.contains("mod")
	    || text.contains("m2")
	    || text.contains("m3"))
	{
	    program_language(LANGUAGE_PASCAL);
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


//-----------------------------------------------------------------------------
// Handle error messages
//-----------------------------------------------------------------------------

void GDBAgent::PanicHP(Agent *source, void *, void *call_data)
{
    string msg = (char *)call_data;
    string path = source->path();
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    if (gdb != 0)
	path = downcase(gdb->title());
    cerr << path << ": " << msg << "\n";
}

void GDBAgent::StrangeHP(Agent *source, void *client_data, void *call_data)
{
    string msg = (char *)call_data;
    msg.prepend("warning: ");
    PanicHP(source, client_data, (char *)msg);
}
