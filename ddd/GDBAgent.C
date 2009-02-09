// $Id$
// Communicate with separate GDB process

// Copyright (C) 1995-1999 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 1999-2001 Universitaet Passau, Germany.
// Copyright (C) 2001, 2003, 2004, 2005, 2006, 2008 Free Software Foundation, Inc.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@gnu.org>.
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

//-----------------------------------------------------------------------------
// GDBAgent implementation
//-----------------------------------------------------------------------------

#include "GDBAgent.h"
#include "cook.h"
#include "ddd.h"
#include "string-fun.h"
#include "regexps.h"
#include "index.h"
#include "isid.h"
#include "home.h"
#include "value-read.h"		// read_token
#include "casts.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <time.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif


DEFINE_TYPE_INFO_1(GDBAgent, TTYAgent);

#define DBG_PROMPT "dbg>"

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
      _has_frame_command(tp == BASH || tp == DBG || tp == GDB 
			 || tp == MAKE || tp == PYDB || tp == XDB ),
      _has_func_command(tp == DBX),
      _has_file_command(tp == DBX),
      _has_run_io_command(false),
      _has_print_r_option(false),
      _has_output_command(false),
      _has_where_h_option(false),
      _has_display_command(tp == BASH || tp == DBG || tp == DBX || tp == GDB 
			   || tp == PYDB),
      _has_clear_command(tp == BASH || tp == DBG || tp == DBX || tp == GDB 
			 || tp == JDB || tp == PERL),
      _has_handler_command(false),
      _has_pwd_command(tp == BASH || tp == DBG || tp == DBX || tp == GDB ||
		       tp == MAKE || tp == PYDB || tp == PERL),
      _has_setenv_command(tp == DBX),
      _has_edit_command(tp == DBX),
      _has_make_command(tp == BASH || tp == DBX || tp == GDB || 
			tp == MAKE || tp == PYDB || tp == PERL),
      _has_jump_command(tp == GDB || tp == DBX || tp == PYDB || tp == XDB),
      _has_regs_command(tp == GDB),
      _has_watch_command(0),	// see below
      _has_named_values(tp == DBG || tp == DBX || tp == GDB || tp == JDB),
      _has_when_command(tp == DBX),
      _has_when_semicolon(tp == DBX),
      _wants_delete_comma(false),
      _has_err_redirection(tp == DBG || tp == DBX || tp == GDB ||  tp == XDB),
      _has_givenfile_command(false),
      _has_cont_sig_command(false),
      _has_examine_command(tp == GDB || tp == DBX),
      _has_rerun_command(tp == DBX),
      _rerun_clears_args(false),
      _has_attach_command(tp == GDB || tp == DBX),
      _has_addproc_command(false),
      _has_debug_command(true),
      _is_windriver_gdb(false),
      _program_language((tp == BASH) ? LANGUAGE_BASH : 
			(tp == DBG)  ? LANGUAGE_PHP  :
			(tp == JDB)  ? LANGUAGE_JAVA :
			(tp == MAKE) ? LANGUAGE_MAKE :
			(tp == PERL) ? LANGUAGE_PERL : 
			(tp == PYDB) ? LANGUAGE_PYTHON : 
			LANGUAGE_C),
      _verbatim(false),
      _recording(false),
      _detect_echos(true),
      _buffer_gdb_output(false),
      _flush_next_output(false),
      last_prompt(""),
      last_written(""),
      echoed_characters(-1),
      exception_state(false),
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
    else if (type() == DBX || type() == BASH)
	_has_watch_command = WATCH_CHANGE;
    else
	_has_watch_command = 0;
    
    cpu = cpu_unknown;
}


// Copy constructor
GDBAgent::GDBAgent(const GDBAgent& gdb)
    : TTYAgent(gdb),
      state(gdb.state),
      _type(gdb.type()),
      _user_data(0),
      _has_frame_command(gdb.has_frame_command()),
      _has_func_command(gdb.has_func_command()),
      _has_file_command(gdb.has_file_command()),
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
      _has_examine_command(gdb.has_examine_command()),
      _has_rerun_command(gdb.has_rerun_command()),
      _rerun_clears_args(gdb.rerun_clears_args()),
      _has_attach_command(gdb.has_attach_command()),
      _has_addproc_command(gdb.has_addproc_command()),
      _has_debug_command(gdb.has_debug_command()),
      _is_windriver_gdb(gdb.is_windriver_gdb()),
      _program_language(gdb.program_language()),
      _verbatim(gdb.verbatim()),
      _recording(gdb.recording()),
      _detect_echos(gdb.detect_echos()),
      _buffer_gdb_output(gdb.buffer_gdb_output()),
      _flush_next_output(gdb.flush_next_output()),
      last_prompt(""),
      last_written(""),
      echoed_characters(-1),
      exception_state(false),
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
      _on_qu_array_completion(0),
      complete_answer("")
{}

// Return default title
const string& GDBAgent::title() const
{
#define TITLES \
X(tBASH,"Bash"),  \
X(tDBG,"DBG"),  \
X(tDBX,"DBX"),  \
X(tGDB,"GDB"),  \
X(tJDB,"JDB"),  \
X(tLADEBUG,"Ladebug"),  \
X(tMAKE,"GNU Make"),  \
X(tPERL,"Perl"),  \
X(tPYDB,"PYDB"),  \
X(tWBD,"WDB"),  \
X(tXDB,"XDB"),  \
X(tDEBUGGER,"debugger")
    enum{
#define X(a,b) a
        TITLES
#undef X
    };

    static
    string const titles[] =
      {
#define X(a,b) b
        TITLES
#undef X
      };
#undef TITLES

    switch (type())
    {
    case BASH:
        return titles[tBASH];

    case DBG:
        return titles[tDBG];

    case DBX:
	if (is_ladebug())
	    return titles[tLADEBUG];
	else
	    return titles[tDBX];

    case GDB:
	if (path().contains("wdb"))
	    return titles[tWBD];
	else
	    return titles[tGDB];

    case JDB:
	return titles[tJDB];

    case MAKE:
        return titles[tMAKE];

    case PERL:
	return titles[tPERL];

    case PYDB:
	return titles[tPYDB];

    case XDB:
	return titles[tXDB];
    }

    return titles[tDEBUGGER];
}

bool GDBAgent::is_ladebug() const
{
    return type() == DBX &&
	(path().contains("ladebug") || prompt().contains("ladebug"));
}

// Trace communication
void GDBAgent::trace(const char *prefix, void *call_data) const
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
    string nl = "\\n\"\n";
    nl += replicate(' ', strlen(prefix));
    nl += "\"";
    s.gsub("\\n", nl);

    if (s_ends_with_nl)
	s = s.before(int(s.length() - 1)) + "\\n" + 
	    s.from(int(s.length() - 1));


#if HAVE_STRFTIME
    char ltime[24];

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    size_t r = strftime(ltime, sizeof(ltime), "%Y.%m.%d %H:%M:%S", &tm);
    ltime[r] = '\0';
#elif HAVE_ASCTIME
    time_t t = time(NULL);
    const char* ltime = asctime(localtime(&t));
#else
    const char* ltime = "";
#endif

    dddlog << ltime << (strlen(ltime)!=0 ?"\n":"") << prefix << s << '\n';
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
			   void*    qa_data,
			   bool&    qa_data_registered)
{
    qa_data_registered = false;
    state = BusyOnInitialCmds;

    if (qu_count > 0) {
	questions_waiting = true;
	init_qu_array(cmds, qu_datas, qu_count, 
		      on_qu_array_completion, qa_data);
	qa_data_registered = true;
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
bool GDBAgent::send_user_cmd(string cmd, void *user_data)  // without '\n'
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
	write_cmd(cmd);
	flush();

	return true;

    case BusyOnQuArray:
    case BusyOnCmd:
	break;
    }

    return false;
}

// Send CMD to GDB (unconditionally), associated with USER_DATA.
bool GDBAgent::send_user_ctrl_cmd(const string& cmd, void *user_data)
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
				   bool& qa_data_registered,
				   string   user_cmd,
				   void* user_data)
{
    qa_data_registered = false;
    if (state != ReadyWithPrompt) 
	return false;

    if (user_data)
	_user_data = user_data;
    if (qu_count > 0)
    {
	questions_waiting = true;
	init_qu_array(cmds, qu_datas, qu_count,
		      on_qu_array_completion, qa_data);
	qa_data_registered = true;
    }

    // Process command
    state = BusyOnCmd;
    complete_answer = "";
    callHandlers(ReadyForQuestion, (void *)false);
    user_cmd += '\n';
    write_cmd(user_cmd);
    flush();

    return true;
}

// Send CMDS to GDB; upon completion, call ON_QU_ARRAY_COMPLETION with QU_DATAS
bool GDBAgent::send_qu_array (const StringArray& cmds,
			      const VoidArray& qu_datas,
			      int      qu_count,
			      OQACProc on_qu_array_completion,
			      void*    qa_data,
			      bool& qa_data_registered)
{
    qa_data_registered = false;
    if (qu_count == 0)
	return true;
    if (state != ReadyWithPrompt)
	return false;

    state = BusyOnQuArray;
    callHandlers(ReadyForQuestion, (void *)false);
    callHandlers(ReadyForCmd, (void *)false);

    init_qu_array(cmds, qu_datas, qu_count, on_qu_array_completion, qa_data);
    qa_data_registered = true;

    // Send first question
    write_cmd(cmd_array[0]);
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

    static const StringArray empty_s;
    static const VoidArray   empty_v;

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

    switch (type()) {
    case BASH:
    {
	// Any line ending in `bashdb<...> ' is a prompt.
	// Since N does not make sense in DDD, we use `DB<> ' instead.

#if RUNTIME_REGEX
	static regex rxbashprompt("bashdb<+[(]*[0-9][)]*>+");
#endif

	int i = answer.length() - 1;
	if (i < 1 || answer[i] != ' ' || answer[i - 1] != '>')
	    return false;

	while (i >= 0 && answer[i] != '\n' ) {
	  if (answer.contains("bashdb<", i)) {
	    string possible_prompt = answer.from(i);
	    if (possible_prompt.matches(rxbashprompt)) {
	      last_prompt = possible_prompt;
	      return true;
	    }
	  }
	  i--;
	}
	return false;
    }
    case GDB:
	// GDB reads in breakpoint commands using a `>' prompt
	if (recording() && answer.contains('>', -1))
	{
	    last_prompt = ">";
	    return true;
	}

	// In annotation level 1, GDB `annotates' its prompt.
	if (answer.contains("\032\032prompt\n", -1))
	    return true;

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
	// Marc Lepage <mlepage@kingston.hummingbird.com> says that
	// DBX on Solaris 2.5 has a prompt like `(dbx N) '.  We're
	// liberal here and allow anything in the form `(NAME) ',
	// where the first word in NAME must contain a `db' or `deb'.
	// (`deb' is there to support DEC's `ladebug')
	static regex rxprompt("[(][^ )]*de?b[^)]*[)] ");
#endif
	if (possible_prompt.matches(rxprompt))
	{
	    last_prompt = possible_prompt;
	    recording(false);
	    return true;
	}
	return false;
    }

    case DBG:
    {
	unsigned beginning_of_line = answer.index('\n', -1) + 1;
	if ( beginning_of_line < answer.length()
	    && answer.length() > 0
	    && answer.matches(DBG_PROMPT,beginning_of_line)) 
	{
	    recording(false);
	    last_prompt = DBG_PROMPT;
	    return true;
	}
	return false;
    }
    case JDB:
    {
	// JDB prompts using "> " or "THREAD[DEPTH] ".  All these
	// prompts may also occur asynchronously.

#if RUNTIME_REGEX
	// Standard prompt: "THREAD[DEPTH] " or "> "
	static regex rxjdbprompt        
	    ("([a-zA-Z][a-zA-Z0-9 ]*[a-zA-Z0-9][[][1-9][0-9]*[]]|>) ");
	// Same, but in reverse
	static regex rxjdbprompt_reverse
	    (" (>|[]][0-9]*[1-9][[][a-zA-Z0-9][a-zA-Z0-9 ]*[a-zA-Z])");
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
	const int beginning_of_line = answer.index('\n', -1) + 1;
	const string possible_prompt = answer.from(beginning_of_line);
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
	    const int beginning_of_line = last_nl + 1;

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
    case MAKE:
    {
	// Any line ending in `mdb<...> ' is a prompt.
	// Since N does not make sense in DDD, we use `DB<> ' instead.

#if RUNTIME_REGEX
	static regex rxmakeprompt("mdb<+[(]*[0-9][)]*>+");
#endif

	int i = answer.length() - 1;
	if (i < 1 || answer[i] != ' ' || answer[i - 1] != '>')
	    return false;

	while (i >= 0 && answer[i] != '\n' ) {
	  if (answer.contains("mdb<", i)) {
	    string possible_prompt = answer.from(i);
	    if (possible_prompt.matches(rxmakeprompt)) {
	      last_prompt = possible_prompt;
	      return true;
	    }
	  }
	  i--;
	}
	return false;
    }
    case PERL:
    {
	// Any line ending in `DB<N> ' is a prompt.
	// Since N does not make sense in DDD, we use `DB<> ' instead.
	//
	// "T. Pospisek's MailLists" <tpo2@sourcepole.ch> reports that
	// under Debian, Perl issues a prompt with control characters:
	// <- "\001\002  DB<1> \001\002"

#if RUNTIME_REGEX
	static regex rxperlprompt("[ \t\001\002]*DB<+[0-9]*>+[ \t\001\002]*");
#endif

	int i = answer.length() - 1;
	if (i < 1 || answer[i] != ' ' || answer[i - 1] != '>')
	    return false;

	while (i > 0 && answer[i - 1] != '\n' && !answer.contains("DB", i))
	    i--;

	string possible_prompt = answer.from(i);
	if (possible_prompt.matches(rxperlprompt))
	{
	    last_prompt = "DB<> ";
	    return true;
	}
	return false;
    }
    case PYDB:
    {
	// Any line ending in `(Pydb) ' is a prompt.

#if RUNTIME_REGEX
	static regex rxpyprompt("[(]+Pydb[)]+");
#endif

	int i = answer.length() - 1;
	if (i < 1 || answer[i] != ' ' || answer[i - 1] != ')')
	    return false;

	while (i >= 0 && answer[i] != '\n' ) {
	  if (answer.contains("(Pydb)", i)) {
	    string possible_prompt = answer.from(i);
	    if (possible_prompt.matches(rxpyprompt)) {
	      last_prompt = possible_prompt;
	      return true;
	    }
	  }
	  i--;
	}
	return false;
    }
    case XDB:
    {
	// Any line equal to `>' is a prompt.
	const unsigned beginning_of_line = answer.index('\n', -1) + 1;
	if (beginning_of_line < answer.length()
	    && answer.length() > 0
	    && answer[beginning_of_line] == '>')
	{
	    last_prompt = ">";
	    return true;
	}
	return false;
    }


    }

    return false;		// Never reached
}

static bool ends_in(const string& answer, const char *prompt)
{
    return answer.contains(prompt, answer.length() - strlen(prompt));
}

// JDB should be applied on itself.
bool GDBAgent::is_exception_answer(const string& answer) const
{
    // Any JDB backtrace contains these lines.
    return type() == JDB && 
	(answer.contains("com.sun.tools.example.debug") ||
	 answer.contains("sun.tools.debug") ||
	 answer.contains("Internal exception:"));
}

void GDBAgent::set_exception_state(bool new_state)
{
    if (new_state != exception_state)
    {
	exception_state = new_state;
	callHandlers(ExceptionState, (void *)exception_state);

	if (exception_state && state != ReadyWithPrompt)
	{
	    // Report the exception message like an unexpected output
	    callHandlers(AsyncAnswer, (void *)&complete_answer);
	}
    }
}


// Return true iff ANSWER ends with secondary prompt.
bool GDBAgent::ends_with_secondary_prompt (const string& ans) const
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
	if (ends_in(answer, "): "))
	{
	    // DEC DBX wants a file name when being invoked without one:
	    // `enter object file name (default is `a.out'): '
	    // Reported by Matthew Johnson <matthew.johnson@speechworks.com>
#if RUNTIME_REGEX
	    static regex rxenter_file_name("enter .*file name");
#endif

	    int idx = index(answer, rxenter_file_name, "enter ", -1);
	    if (idx >= 0 && answer.index('\n', idx) < 0)
		return true;
	}

	// Prompt is `> ' at beginning of line
	return answer == "> " || ends_in(answer, "\n> ");

    case GDB:
	// Prompt is `> ' at beginning of line
	return answer == "> " || ends_in(answer, "\n> ");
	
    case BASH:
    case MAKE:
        return false; // No secondary prompt

    case DBG:
    case JDB:
    case PERL:
    case PYDB:
    case XDB:
	// Is there any secondary prompt in these debuggers? (FIXME)
	return false;
    }

    return false;		// Never reached
}

// Return true iff ANSWER ends with (yes or no)
bool GDBAgent::ends_with_yn (const string& answer) const
{
    if (ends_in(answer, "(y or n) "))
	return true;		// GDB

    if (ends_in(answer, "(y/n): "))
	return true;		// BASH

    if (ends_in(answer, "(yes or no) "))
	return true;

    if ((type() == XDB || type() == JDB) && ends_in(answer, "? "))
	return true;

    return false;
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
	|| last_line.contains("cont:")
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
			      "|  cont: "
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

    case DBG:
    {
	int i = answer.index(DBG_PROMPT, -1);
	while (i > 0 && answer[i - 1] == ' ')
	    i--;
	answer = answer.before(i);
	break;
    }
    
    case BASH:
    case JDB:
    case MAKE:
    case PYDB:
    {
	// Check for prompt at the end of the last line
	if (answer.contains(last_prompt, -1))
	{
	    answer = answer.before(int(answer.length()) - 
				   int(last_prompt.length()));
	}
	break;
    }

    case PERL:
    {
	int i = answer.index("DB<", -1);
	while (i > 0 && answer[i - 1] == ' ')
	    i--;
	answer.from(i) = "";
	break;
    }

    case XDB:
	answer = answer.before('>', -1);
	break;
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
	    s.at(warning, eol - warning) = "";
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
	char c = answer[source_index];
	switch (c)
	{
	case '\b':
	    // Delete last character
	    if (target_index > 0 && answer[target_index - 1] != '\n')
		target_index--;
	    else
	    {
		// Nothing to erase -- keep the '\b'.
		goto copy;
	    }
	    break;

	case '\r':
	    if (source_index + 1 < int(answer.length()))
	    {
		if (answer[source_index + 1] == '\n' ||
		    answer[source_index + 1] == '\r')
		{
		    // '\r' followed by '\n' or '\r' -- don't copy
		    break;
		}
	        else
		{
		    // '\r' followed by something else: 
		    // Return to beginning of line
		    while (target_index > 0 && 
			   answer[target_index - 1] != '\n')
			target_index--;
		}
	    }
	    else
	    {
		// Trailing '\r' -- keep it
		goto copy;
	    }
	    break;

	case '\032':
	    // In annotation level 1, GDB sends out `annotation'
	    // sequences like `\n\032\032prompt\n' or `\032\032prompt\n'.
	    // We use some of these to find the current source position.
	    if (
		  (
		    ( // if we've just seen a newline
		 	target_index > 0 && answer[target_index - 1] == '\n'
		    )
		    ||   // or if this is the start of the line
		    target_index == 0
		  )
		  &&    // and the next character's also a Ctrl-Z
		  source_index + 1 < int(answer.length()) &&
		  answer[source_index + 1] == '\032'
	        )
	    {
		// Ignore everything up to and including the next '\n'.
		int i = source_index;
		while (i < int(answer.length()) &&
		       answer[i] != '\n' && answer[i] != ':')
		    i++;
		if (i >= int(answer.length()))
		{
		    // The annotation is not finished yet -- copy it
		    goto copy;
		}
		else if (answer[i] == ':')
		{
		    // This is a normal `fullname' annotation, handled by DDD
		    goto copy;
		}
		else
		{
		    // Annotation found -- ignore it
		    if (target_index > 0)
		    {
			assert(answer[target_index - 1] == '\n');
			target_index--;
			assert(answer[i] == '\n');
		    }
		    source_index = i;
		}
	    }
	    else
	    {
		// Single or trailing `\032' -- keep it
		goto copy;
	    }
	    break;

	case '\033':		// aka `\e'
	    // XDB `more' and Ladebug send VT100 escape sequences like `\e[m',
	    // `\e[22;1H', `\e[7m', `\e[K', regardless of TERM
	    // settings.  We simply weed out everything up to and
	    // including to the next letter.
	    while (source_index < int(answer.length()) && 
		   !isalpha(answer[source_index]))
		source_index++;

	    if (source_index >= int(answer.length()))
	    {
		// The escape sequence is not finished yet - keep the '\e'.
		answer[target_index++] = c;
	    }
	    break;

	copy:
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
		// Ignore '\r' in comparisons
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

	if (e >= int(last_written.length()))
	{
	    // All characters written have been echoed.
	    // Remove echoed characters and keep on processing.
	    callHandlers(EchoDetected, (void *)true);
	    answer = answer.from(i);
	    echoed_characters = -1;
	}
	else if (i >= int(answer.length()))
	{
	    // A prefix of the last characters written has been echoed.
	    // Wait for further echos.
	    answer = "";
	    echoed_characters = e;
	}
	else
	{
	    // No echo.
	    // => Restore any echoed characters and keep on processing
	    answer.prepend(last_written.before(echoed_characters));
	    echoed_characters = -1;

	    // If a command of length ECHO_THRESHOLD is not echoed,
	    // disable echo detection.  The idea behind this is that
	    // echo disabling seems to have succeeded and we thus no
	    // longer need to check for echos.  This reduces the risk
	    // of echo detection altering output data.

	    // ECHO_THRESHOLD is 4 because the inferior debugger might
	    // not echo short interludes like `yes\n', `no\n' or `^C'.
	    const unsigned int ECHO_THRESHOLD = 4;

	    if (last_written.length() > ECHO_THRESHOLD)
	    {
		// No more echos - disable echo detection until re-activated
		callHandlers(EchoDetected, (void *)false);
		detect_echos(false);
	    }
	}
    }
}

void GDBAgent::handle_more(string& answer)
{
    // Check for `More' prompt
    string reply = requires_reply(answer);
    if (!reply.empty())
    {
	// Oops - this should not happen.  Just hit the reply key.
	write(reply);
	flush();

	// Ignore the last line (containing the `More' prompt)
	const int last_beginning_of_line = answer.index('\n', -1) + 1;
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
	info.reply = "1\n";

	// Allow handlers to override the default reply
	callHandlers(ReplyRequired, (void *)&info);

	// Send reply immediately
	write(info.reply);
	flush();

	// Ignore the selection
	answer = info.question;
    }

    // Check for `yes or no'
    if (state != BusyOnCmd && ends_with_yn(answer))
    {
	ReplyRequiredInfo info;
	info.question = answer;
	info.reply = "no\n";

	// Allow handlers to override the default reply
	callHandlers(YesNoRequired, (void *)&info);

	// Send reply immediately
	write(info.reply);
	flush();

	// Ignore the selection
	answer = info.question;
    }
}

bool GDBAgent::recording(bool val)
{
    if (_recording != val)
    {
	_recording = val;
	callHandlers(Recording, (void *)recording());
    }
    return recording();
}

void GDBAgent::handle_input(string& answer)
{
    bool had_a_prompt;

    OAProc  on_answer = _on_answer;
    OACProc on_answer_completion = _on_answer_completion;
    void *user_data = _user_data;

    handle_echo(answer);
    handle_more(answer);
    handle_reply(answer);

    if (exception_state && state != ReadyWithPrompt)
    {
	// Be sure to report the exception like an unexpected output
	callHandlers(AsyncAnswer, (void *)&answer);
    }

    // Handle all other GDB output, depending on current state.
    switch (state)
    {
    case ReadyWithPrompt:
	// We have a prompt, and still get input?  Maybe this is an
	// answer to a command sent before the prompt was received -
	// or a reply to a control character (`Quit').
	strip_control(answer);
	callHandlers(AsyncAnswer, (void *)&answer);

	// Save answer in case of exceptions.
	complete_answer += answer;
	if (ends_with_prompt(complete_answer))
	{
	    set_exception_state(false);
	    complete_answer = "";
	}
	break;

    case BusyOnInitialCmds:
    case BusyOnCmd:
	complete_answer += answer;

	had_a_prompt = ends_with_prompt(complete_answer);

	if (had_a_prompt)
	    set_exception_state(false);

	if (_on_answer != 0)
	{
	    bool ready_to_process = true;

	    if (buffer_gdb_output())
	    {
		// Buffer answer
		ready_to_process = 
		    had_a_prompt || ends_with_yn(complete_answer);

		if (flush_next_output() && !ready_to_process)
		{
		    // Flush this answer
		    flush_next_output(false);
		    ready_to_process = true;

		    // Don't include it in the complete answer
		    complete_answer = 
			complete_answer.before(int(complete_answer.length() - 
						   answer.length()));
		}
		else
		{
		    // Simply buffer it all until we're ready.
		    if (ready_to_process)
			answer = complete_answer;
		}
	    }

	    if (ready_to_process)
	    {
		// Handle now
		if (had_a_prompt)
		{
		    normalize_answer(answer);
		}
		else
		{
		    strip_control(answer);
		    strip_dbx_comments(answer);
		}
		
		on_answer(answer, user_data);
	    }
	}

	if (had_a_prompt)
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

		    if (on_answer_completion != 0)
			on_answer_completion(user_data);
		}
		else
		{
		    state = BusyOnQuArray;

		    // Send first question
		    write_cmd(cmd_array[0]);
		    flush();
		}
	    }
	    else if (!questions_waiting)
	    {
		state = ReadyWithPrompt;
		callHandlers(ReadyForQuestion, (void *)true);

		if (on_answer_completion != 0)
		    on_answer_completion(user_data);
	    }
	    else
	    {
		state = BusyOnQuArray;
		callHandlers(ReadyForCmd, (void *)false);

		// Send first question
		write_cmd(cmd_array[0]);
		flush();
	    }
	}
	break;

    case BusyOnQuArray:
	complete_answers[qu_index] += answer;

	if (ends_with_prompt(complete_answers[qu_index]))
	{
	    set_exception_state(false);

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
		write_cmd(cmd_array[++qu_index]);
		flush();
	    }
	}
	break;

    default:
	assert(0);
	::abort();
	break;
    }

    if (is_exception_answer(complete_answer))
	set_exception_state(true);
}

// Write arbitrary data
int GDBAgent::write(const char *data, int length)
{
    last_written = string(data, length);

    echoed_characters = 0;
    return TTYAgent::write(data, length);
}

// Write command
int GDBAgent::write_cmd(const string& cmd)
{
    if (gdb->type() == PERL && cmd.contains("exec ", 0))
    {
	// Rename debugger
	string p = cmd.before('\n');
	p = p.after("exec ");
	if (p.contains('\'', 0) || p.contains('\"', 0))
	    p = unquote(p);
	if (!p.empty())
	    _path = p;
    }

    return write(cmd);
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
string GDBAgent::print_command(const char *expr, bool internal) const
{
    string cmd;

    switch (type())
    {
    case GDB:
    case DBX:
    case DBG:
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

    case BASH:
    case MAKE:
    case PERL:
	cmd = "x";
	break;

    case PYDB:
	cmd = "print";
	break;

    case JDB:
	if (internal)
	    cmd = "dump";
	else
	    cmd = "print";
	break;
    }

    if (strlen(expr) != 0) {
	cmd += ' ';
	cmd += expr;
    }

    return cmd;
}

// DBX 3.0 wants `display -r' instead of `display' for C++
string GDBAgent::display_command(const char *expr) const
{
    string cmd;
    if (!has_display_command())
	return cmd;

    if (has_print_r_option() && strlen(expr) != 0)
	cmd = "display -r";
    else
	cmd = "display";

    if (strlen(expr) != 0) {
        cmd += ' ';
        cmd += expr;
    }

    return cmd;
}

// DBX 3.0 wants `where -h' instead of `where'
string GDBAgent::where_command(int count) const
{
    string cmd;
    switch (type())
    {
    case BASH:
    case DBG:
    case DBX:
    case GDB:
    case JDB:
    case MAKE:
    case PYDB:
	if (has_where_h_option())
	    cmd = "where -h";
	else
	    cmd = "where";
	break;

    case PERL:
	cmd = "T";
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
    case MAKE:
    case PYDB:
	return "info locals";

    case DBX:
	return "dump";

    case XDB:
	return "l";

    case JDB:
	return "locals";

    case BASH:
    case PERL:
	return "V";

    case DBG:
      return "";

    }

    return "";			// Never reached
}

string GDBAgent::info_args_command() const
{
    switch (type())
    {
    case BASH:
    case GDB:
    case PYDB:
	return "info args";

    default:
	break;
    }

    return info_locals_command();
}

string GDBAgent::info_display_command() const
{
    return
      (type() == GDB || type() == BASH) ?
      "info display":
      display_command();
}


// Some DBXes want `sh pwd' instead of `pwd'
string GDBAgent::pwd_command() const
{
    switch (type())
    {
    case GDB:
    case DBX:
    case PYDB:
    case DBG:
	if (has_pwd_command())
	    return "pwd";
	else
	    return "sh pwd";

    case XDB:
	return "!pwd";

    case BASH:
    case MAKE:
	return "shell pwd";

    case PERL:
	return "p $ENV{'PWD'} || `pwd`";

    case JDB:
	return "";

    }

    return "";			// Never reached
}

// Some DBXes want `sh make' instead of `make'
string GDBAgent::make_command(const string& args) const
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

    case PERL:
	if (args.empty())
	    return "system 'make'";
	else
	    return "system 'make " + args + "'";

    case BASH:
    case MAKE:
    case PYDB:
	cmd = "shell make " + args ;
	break;

    case JDB:
    case DBG:
	return "";		// Not available
    }

    if (args.empty())
	return cmd;
    else
	return cmd + " " + args;
}

// Move PC to POS
string GDBAgent::jump_command(const string& pos) const
{
    if (!has_jump_command())
	return "";

    switch (type())
    {
    case GDB:
	return "jump " + pos;
   
    case XDB:
    {
        string pos_ = pos;
	if (pos_.contains('*', 0))
	    pos_ = pos_.after('*');
	return "g " + pos_;
    }

    case DBX:
	return "cont at " + pos;

    case BASH:
    case DBG:  
    case JDB:
    case MAKE:
    case PERL:
    case PYDB:
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

    case BASH:
    case DBG:
    case JDB:
    case MAKE:
    case PYDB:
    case PERL:
    case XDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Watch expressions
string GDBAgent::watch_command(const string& expr, WatchMode w) const
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
	{
	    if (has_handler_command())
	    {
		// Solaris 2.6 DBX wants `stop change VARIABLE'
		return "stop change " + expr;
	    }
	    else
	    {
		// `Traditional' DBX notation
		return "stop " + expr;
	    }
	}
	return "";

    case BASH:
	if ((w & WATCH_CHANGE) == WATCH_CHANGE)
	    return "watch " + expr;
	return "";
   
    case XDB:
	// Not available.  (There is the `assertion' concept which is
	// similar but won't fit into the DDD GUI.)
	return "";

    case JDB:
	if ((w & WATCH_CHANGE) == WATCH_CHANGE)
	    return "watch all " + expr;
	if ((w & WATCH_READ) == WATCH_READ)
	    return "watch access " + expr;
	if ((w & WATCH_ACCESS) == WATCH_ACCESS)
	    return "watch access " + expr;
	return "";

    case DBG:  // Is this right? 
    case MAKE:
    case PERL:
    case PYDB:
	return "";		// Not available
    }

    return "";			// Never reached
}


string GDBAgent::kill_command() const
{
    switch (type())
    {
    case DBG:
    case DBX:
    case GDB:
	return "kill";
   
    case XDB:
	return "k";

    case BASH:
    case MAKE:
    case PERL:
    case PYDB:
	return "quit";

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

string GDBAgent::frame_command() const
{
    switch (type())
    {
    case BASH:
    case DBG:
    case DBX:
    case GDB:
    case MAKE:
    case PYDB:
	if (has_frame_command())
	    return "frame";
	else
	    return where_command(1);

    case XDB:
	return print_command("$depth");

    case JDB:
    case PERL:
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
    case BASH:
    case DBG:
    case DBX:
    case GDB:
    case MAKE:
    case PYDB:
	return frame_command() + " " + itostring(num);

    case XDB:
	return "V " + itostring(num);

    case JDB:
    case PERL:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Add OFFSET to current frame (i.e. OFFSET > 0: up, OFFSET < 0: down)
string GDBAgent::relative_frame_command(int offset) const
{
    if (gdb->type() == PERL)
	return "";		// No such command in Perl

    if (offset == -1)
	return "down";
    else if (offset < 0)
	return "down " + itostring(-offset);
    else if (offset == 1)
	return "up";
    else if (offset > 0)
	return "up " + itostring(offset);

    return "";			// Offset == 0
}

string GDBAgent::func_command() const
{
    switch (type())
    {
    case BASH:
    case DBG:
    case GDB:
    case JDB:
    case MAKE:
    case PERL:
    case PYDB:
    case XDB:
	return frame_command();

    case DBX:
	return has_func_command() ? "func" : "";
    }

    return "";			// Never reached
}

// Each debugger has its own way of echoing (sigh)
string GDBAgent::echo_command(const string& text) const
{
    switch (type())
    {
    case BASH:
    case PYDB:
	return "print " + quote(text);

    case DBG:
	return " ";
	
    case DBX:
	return print_command(quote(text), false);

    case GDB:
	return "echo " + cook(text);

    case MAKE:
	return "examine " + quote(text);

    case PERL:
	// We use `print DB::OUT' instead of `p' since this also works
	// in actions.
	return "print DB::OUT " + quote(text, '\"');

    case XDB:
	return quote(text);

    case JDB:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Prefer `ptype' on `whatis' in GDB
string GDBAgent::whatis_command(const string& text) const
{
    switch (type())
    {
    case GDB:
	return "ptype " + text;

    case DBX:
    case PYDB:
	if (has_print_r_option())
	    return "whatis -r " + text;
	else
	    return "whatis " + text;

    case XDB:
	return "p " + text + "\\T";

    case JDB:
	return "class " + text;  // JDB: as close as we can get

    case DBG:
    case PERL:
	return "";		// Who knows?

    case BASH:
    case MAKE:
	return "";		// Can't do yet.
    }

    return "";			// Never reached
}

// Enable breakpoint BP
string GDBAgent::enable_command(string bp) const
{
    if (!bp.empty())
	bp.prepend(' ');

    switch (type())
    {
    case BASH:
    case DBG:
    case GDB:
    case PYDB:
	return "enable" + bp;

    case DBX:
	if (is_ladebug())
	    return "enable" + bp;
	else if (has_handler_command())
	    return "handler -enable" + bp;
	else
	    return "";

    case XDB:
	return "ab" + bp;

    case JDB:
    case MAKE:
    case PERL:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Disable breakpoint BP
string GDBAgent::disable_command(string bp) const
{
    if (!bp.empty())
	bp.prepend(' ');

    switch (type())
    {
    case BASH:
    case DBG:
    case GDB:
    case PYDB:
	return "disable" + bp;

    case DBX:
	if (is_ladebug())
	    return "disable" + bp;
	else if (has_handler_command())
	    return "handler -disable" + bp;
	else
	    return "";

    case XDB:
	return "sb" + bp;

    case JDB:
    case MAKE:
    case PERL:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Delete breakpoint BP
string GDBAgent::delete_command(string bp) const
{
    if (!bp.empty())
	bp.prepend(' ');

    switch (type())
    {
    case BASH:
    case DBG:
    case DBX:
    case GDB:
    case MAKE:
    case PYDB:
	return "delete" + bp;

    case XDB:
	return "db" + bp;

    case JDB:
    case PERL:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Set ignore count of breakpoint BP to COUNT
string GDBAgent::ignore_command(const string& bp, int count) const
{
    switch (type())
    {
    case GDB:
    case PYDB:
	return "ignore " + bp + " " + itostring(count);

    case DBX:
	if (has_handler_command())
	    return "handler -count " + bp + " " + itostring(count);
	else
	    return "";

    case XDB:
	return "bc " + bp + " " + itostring(count);

    case BASH:
    case DBG:
    case JDB:
    case MAKE:
    case PERL:
	return "";		// Not available
    }

    return "";			// Never reached
}

// Set condition of breakpoint BP to EXPR
string GDBAgent::condition_command(const string& bp, const char *expr) const
{
    switch (type())
    {
    case GDB:
    case DBG:
    case PYDB:
    case BASH:
	return "condition " + bp + " " + expr;

    case MAKE:
	return "";		// Not available

    case DBX:
    case XDB:
    case JDB:
    case PERL:
	return "";		// FIXME
    }

    return "";			// Never reached
}

// Return shell escape command
string GDBAgent::shell_command(const string& cmd) const
{
    switch (type())
    {
    case BASH:
    case GDB:
    case MAKE:
    case PYDB:
	return "shell " + cmd;

    case DBX:
	return "sh " + cmd;

    case XDB:
	return "!" + cmd;

    case PERL:
	return "system " + quote(cmd, '\'');

    case DBG:
    case JDB:
	return "";		// Not available
    }
    return "";			// Never reached
}

// Return actual debugger command, with args
string GDBAgent::cmd() const
{
    string cmd = path();

    for (;;)
    {
	// We might get called as `/bin/sh -c 'exec CMD ARGS''.  Handle this.
	strip_leading_space(cmd);
	if (cmd.contains("/bin/sh -c ", 0))
	{
	    cmd = cmd.after("-c ");
	    strip_leading_space(cmd);
	    if (cmd.contains('\'', 0) || cmd.contains('\"', 0))
	    {
		cmd.gsub("\'\\\'\'", '\'');
		cmd = unquote(cmd);
	    }
	}
	else if (cmd.contains("exec ", 0))
	{
	    cmd = cmd.after("exec ");
	}
	else
	{
	    break;
	}
    }

    strip_space(cmd);
    return cmd;
}

// Return name of debugger
string GDBAgent::debugger() const
{
    string debugger = cmd();
    if (debugger.contains(' '))
	debugger = debugger.before(' ');

    if (debugger.contains('\'', 0) || debugger.contains('\"', 0))
	debugger = unquote(debugger);

    strip_space(debugger);
    return debugger;
}

// Return debugger arguments (including program name)
string GDBAgent::args() const
{
    string args = cmd();

    args = args.after(' ');
    strip_leading_space(args);

    if (args.contains('\'', 0) || args.contains('\"', 0))
	args = unquote(args);

    strip_space(args);
    return args;
}

// Return debugged program
string GDBAgent::program() const
{
    string program = args();
    while (program.contains("-", 0))
    {
	// Skip options
	program = program.after(' ');
	strip_leading_space(program);
    }

    if (program.contains(' '))
	program = program.before(' ');
    if (program.contains('\'', 0) || program.contains('\"', 0))
	program = unquote(program);

    return program;
}

// Place quotes around filename FILE if needed
string GDBAgent::quote_file(const string& file) const
{
    if (type() == GDB && file.contains(rxwhite))
    {
	// GDB has no special processing for characters within quotes
	// (i.e. backslashes, etc.)
	return '\'' + file + '\'';
    }
    else
    {
	return file;
    }
}

// Return command to debug PROGRAM
string GDBAgent::debug_command(const char *program, string args) const
{
    if (!args.empty() && !args.contains(' ', 0))
	args.prepend(' ');

    switch (type())
    {
    case GDB:
	if (is_windriver_gdb())
	    return "load " + quote_file(program);
	else
	    return "file " + quote_file(program);

    case DBG:
	return string("file ") + program;

    case DBX:
	if (is_ladebug())
	    return string("load ") + program;       // Compaq Ladebug
	else if (has_givenfile_command())
	    return string("givenfile ") + program; // SGI DBX
	else
	    return string("debug ") + program;     // SUN DBX

    case XDB:
	return string("#file ") + program; // just a dummy

    case JDB:
	return string("load ") + program;

    case PERL:
//	return "exec " + quote(debugger() + " -d " + program + args);
	return "R";

    case BASH:
    case PYDB:
	return string("debug ") + program + args;

	// restart/run is not the same a debug. But this is the closes
	// mdb has for now.
    case MAKE:
	return string("run ") + program + args;
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

    case BASH:
    case DBG:
    case JDB:
    case MAKE:
    case PERL:
    case PYDB:
	return "";		// Not available
    }

    return "";			// Never reached
}


// Return a command that does nothing.
string GDBAgent::nop_command(const char *comment) const
{
    if (type() == JDB)
	return " ";

    return string("# ") + comment;	// Works for all other inferior debuggers
}

// Run program with given ARGS
string GDBAgent::run_command(string args) const
{
    if (!args.empty() && !args.contains(' ', 0))
	args = " " + args;

    switch (type())
    {
    case GDB:
    {
	string c;
	if (args.empty())
	    c = "set args\n";
	return c + "run" + args;
    }

    case DBX:
	if (args.empty() && has_rerun_command() && rerun_clears_args())
	    return "rerun";
	else
	    return "run" + args;

    case BASH:
    case DBG:
    case JDB:
    case MAKE:
    case PYDB:
	return "run" + args;

    case XDB:
	if (args.empty())
	    return "R";
	else
	    return "r" + args;

    case PERL:
	return "exec " + quote(debugger() + " -d " + program() + args);
    }

    return "";			// Never reached
}

// Re-run program with previous arguments
string GDBAgent::rerun_command() const
{
    switch (type())
    {
    case BASH:
    case DBG:
    case GDB:
    case JDB:
    case MAKE:
    case PYDB:
	return "run";

    case DBX:
	if (has_rerun_command() && !rerun_clears_args())
	    return "rerun";
	else
	    return "run";

    case XDB:
	return "r";

    case PERL:
	return "R";
    }

    return "";			// Never reached
}

// Attach to process ID
string GDBAgent::attach_command(int pid, const string& file) const
{
    switch (type())
    {
    case GDB:
	return "attach " + itostring(pid);

    case DBX:
	if (has_handler_command())
	    return "debug - " + itostring(pid);	           // Sun DBX
	if (has_addproc_command())
	    return "addproc " + itostring(pid);	           // SGI DBX
	else if (is_ladebug() && has_attach_command())
	    return string("set $stoponattach=1\n") +
		"attach " + itostring(pid) + " " + file;   // DEC ladebug
	else if (has_attach_command())
	    return "attach " + itostring(pid);             // Others
	else
	    return "debug " + file + " " + itostring(pid); // Others

    case BASH:
    case DBG:
    case JDB:
    case MAKE:
    case PERL:
    case PYDB:
    case XDB:
	break;
    }

    return "";			// Unsupported
}

string GDBAgent::detach_command(int pid) const
{
    switch (type())
    {
    case GDB:
	return "detach";

    case DBX:
	if (has_addproc_command())
	    return "delproc " + itostring(pid);	// SGI DBX
	else
	    return "detach";	// Others

    case BASH:
    case DBG:
    case JDB:
    case MAKE:
    case PERL:
    case PYDB:
    case XDB:
	break;
    }

    return "";			// Unsupported
}


// Return PREFIX + EXPR, parenthesizing EXPR if needed
string GDBAgent::prepend_prefix(const string& prefix, const string& expr)
{
  return prepend_prefix( prefix.chars(), expr);
}

string GDBAgent::prepend_prefix(const char *prefix, const string& expr)
{
    if (expr.matches(rxidentifier)
	|| expr.contains("(", 0) && expr.contains(")", -1))
	return prefix + expr;
    else if (expr.empty())
	return prefix;
    else
	return string(prefix) + "(" + expr + ")";
}

// Return EXPR + SUFFIX, parenthesizing EXPR if needed
string GDBAgent::append_suffix(const string& expr, const string &suffix)
{
  return append_suffix( expr, suffix.chars() );
}

string GDBAgent::append_suffix(const string& expr, const char *suffix)
{
    if (expr.matches(rxidentifier)
	|| expr.contains("(", 0) && expr.contains(")", -1))
	return expr + suffix;
    else if (expr.empty())
	return string(suffix);
    else
	return "(" + expr + ")" + suffix;
}

// Dereference an expression.
string GDBAgent::dereferenced_expr(const string& expr) const
{
    switch (program_language())
    {
    case LANGUAGE_C:
	return prepend_prefix("*", expr);

    case LANGUAGE_BASH:
    case LANGUAGE_MAKE:
    case LANGUAGE_PHP:
    case LANGUAGE_PERL:
	// Perl has three `dereferencing' operators, depending on the
	// type of reference.  The `deref()' function provides a
	// better solution.
	return prepend_prefix("$", expr);

    case LANGUAGE_FORTRAN:
	// GDB prints dereferenced pointers as `**X', but accepts them as `*X'.
	return prepend_prefix("*", expr);

    case LANGUAGE_JAVA:
	if (type() == GDB)
	{
	    // GDB dereferences JAVA references by prepending `*'
	    return prepend_prefix("*", expr);
	}

	// JDB (and others?) dereference automatically
	return expr;

    case LANGUAGE_CHILL:
	return append_suffix(expr, "->");

    case LANGUAGE_PASCAL:
	return append_suffix(expr, "^");

    case LANGUAGE_ADA:
	// GDB 4.16.gnat.1.13 prepends `*' as in C
	return prepend_prefix("*", expr);

    case LANGUAGE_PYTHON:
	return "";		// No such thing in Python/PYDB

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

    case LANGUAGE_PYTHON:
	return "";		// Not supported in Python

    case LANGUAGE_BASH:
    case LANGUAGE_MAKE:
    case LANGUAGE_PHP:          // Is this right? 
    case LANGUAGE_PERL:
	return "";		// No such thing in bash/make/Perl

    case LANGUAGE_ADA:
	return "";		// Not supported in GNAT/Ada

    case LANGUAGE_OTHER:
	return "";		// All other languages
    }

    return "";			// All other languages
}

// Give the index of an expression.
string GDBAgent::index_expr(const string& expr, const string& index) const
{
    switch (program_language())
    {
    case LANGUAGE_FORTRAN:
    case LANGUAGE_ADA:
	return expr + "(" + index + ")";

    case LANGUAGE_PERL:
	if (!expr.empty() && expr[0] == '@')
	    return '$' + expr.after(0) + "[" + index + "]";
	else
	    return expr + "[" + index + "]";

    case LANGUAGE_BASH:
      return "${" + expr + "[" + index + "]}";

    // Not sure if this is really right.
    case LANGUAGE_MAKE:
      return "$(word $(" + expr + ")," + index + ")";

    default:
	break;
    }

    // All other languages
    return expr + "[" + index + "]";
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

    case LANGUAGE_ADA:
    case LANGUAGE_BASH:
    case LANGUAGE_C:
    case LANGUAGE_JAVA:
    case LANGUAGE_MAKE:
    case LANGUAGE_PERL:
    case LANGUAGE_PHP:
    case LANGUAGE_PYTHON:
    case LANGUAGE_OTHER:
	return 0;
    }

    return 0;			// Never reached
}

// Return member separator
string GDBAgent::member_separator() const
{
    switch (program_language())
    {
    case LANGUAGE_FORTRAN:
    case LANGUAGE_PASCAL:
    case LANGUAGE_CHILL:
    case LANGUAGE_C:
    case LANGUAGE_PYTHON:
    case LANGUAGE_OTHER:
    case LANGUAGE_PHP:
	return " = ";

    case LANGUAGE_JAVA:
	return ": ";

    case LANGUAGE_BASH:
    case LANGUAGE_MAKE:   // Might consider members of archives. Or target stem
        return "";

    case LANGUAGE_ADA:
    case LANGUAGE_PERL:
	return " => ";
    }

    return "";			// Never reached
}

// Return assignment command
string GDBAgent::assign_command(const string& var, const string& expr) const
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

    case DBG:
    case PYDB:
	cmd = "";		// No command needed
	break;

    case PERL:
	if (!var.empty() && !is_perl_prefix(var[0]))
	    return "";		// Cannot set this

	cmd = "";		// No command needed
	break;

    case BASH:
       if (!var.empty() && ! ('$' == var[0]))
	    return "";		// Cannot set this

	cmd = "eval ";
	break;

    case MAKE:
        // Need to test var.empty()? 
	cmd = "set variable";
	break;

    case JDB:
	if (has_debug_command())
	    return "";		// JDB 1.1: not available

	cmd = "set";		// JDB 1.2
	break;
    }

    cmd += " " + var + " ";

    switch (program_language())
    {
    case LANGUAGE_BASH:
    case LANGUAGE_C:
    case LANGUAGE_FORTRAN:
    case LANGUAGE_JAVA:
    case LANGUAGE_MAKE:
    case LANGUAGE_PERL:
    case LANGUAGE_PHP:
    case LANGUAGE_PYTHON:	// FIXME: vrbl names can conflict with commands
    case LANGUAGE_OTHER:
	cmd += "=";
	break;

    case LANGUAGE_ADA:
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

    if (!addr.empty())
    {
	switch (program_language())
	{
	case LANGUAGE_ADA:
	case LANGUAGE_BASH:
	case LANGUAGE_C:
	case LANGUAGE_FORTRAN:
	case LANGUAGE_JAVA:
	case LANGUAGE_MAKE:
	case LANGUAGE_PERL:
	case LANGUAGE_PHP:
	case LANGUAGE_PYTHON:
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
string GDBAgent::disassemble_command(string start, const char *end) const
{
    string cmd;
    if (type() != GDB)
	return cmd;

    normalize_address(start);
    cmd = "disassemble " + start;

    if (strlen(end) != 0)
    {
        string end_( end );
	normalize_address(end_);
	cmd += ' ';
	cmd += end_;
    }
    return cmd;
}


string GDBAgent::history_file() const
{
    switch (type())
    {
    case GDB:
    {
	const char *g = getenv("GDBHISTFILE");
	if (g != 0)
	    return g;
	else
	    return "./.gdb_history";
    }

    case BASH:
    case DBG:
    case DBX:
    case JDB:
    case MAKE:
    case PERL:
    case PYDB:
	return "";		// Unknown

    case XDB:
    {
	const char *g = getenv("XDBHIST");
	if (g != 0)
	    return g;
	else
	    return string(gethome()) + "/.xdbhist";
    }
    }

    return "";			// Unknown
}

// Return true iff A contains the word S
static bool contains_word(const string& a, const string& s)
{
    int index = -1;
    for (;;)
    {
	index = a.index(s, index + 1);
	if (index < 0)
	    break;

	if (index > 0 && isalpha(a[index - 1]))
	    continue;		// Letter before S

	int next = index + s.length();
	if (next < int(a.length()) && isalpha(a[next]))
	    continue;		// Letter after S

	return true;		// Okay
    }

    return false;		// Not found
}

// Determine language from TEXT
ProgramLanguage GDBAgent::program_language(string text)
{
    text.downcase();

    if (type() == GDB && text.contains("language"))
	text = text.after("language");

    if (text.contains("\n"))
	text = text.before("\n");

    static struct {
	const char *name;
	ProgramLanguage language;
    } const language_table[] = {
	{ "ada",     LANGUAGE_ADA },
	{ "bash",    LANGUAGE_BASH },
	{ "c",       LANGUAGE_C },
	{ "c++",     LANGUAGE_C },
	{ "chill",   LANGUAGE_CHILL },
	{ "fortran", LANGUAGE_FORTRAN },
	{ "f",       LANGUAGE_FORTRAN }, // F77, F90, F
	{ "java",    LANGUAGE_JAVA },
	{ "make",    LANGUAGE_MAKE },
	{ "modula",  LANGUAGE_PASCAL },
	{ "m",       LANGUAGE_PASCAL }, // M2, M3 or likewise
	{ "pascal",  LANGUAGE_PASCAL },
	{ "perl",    LANGUAGE_PERL },
	{ "python",  LANGUAGE_PYTHON },
	{ "php",     LANGUAGE_PHP },
	{ "auto",    LANGUAGE_OTHER }  // Keep current language
    };

    for (int i = 0; 
	 i < int(sizeof(language_table) / sizeof(language_table[0])); i++)
    {
	if (contains_word(text, language_table[i].name))
	{
	    ProgramLanguage new_language = language_table[i].language;
	    if (new_language != LANGUAGE_OTHER)
		program_language(new_language);

	    return program_language();
	}
    }

    // Unknown language - switch to default setting
    program_language(LANGUAGE_C);
    return program_language();
}


//-----------------------------------------------------------------------------
// Perl specials
//-----------------------------------------------------------------------------

void GDBAgent::split_perl_var(const string& var,
			      string& prefix,
			      string& package,
			      string& name)
{
    name = var.from(rxalpha);
    if (name.empty())
	name = var.from("_");
    if (name.empty())
	return;
    prefix = var.before(name);

    package = "";
    if (name.contains("::"))
    {
	package = name.before("::", -1);
	name = name.after(package + "::");
    }
}

// Bring VALUE into a form that might be recognized by DDD
void GDBAgent::munch_perl_scalar(string& value)
{
    strip_leading_space(value);
    if (value.contains("0  ", 0))
	value = value.after("0");

    strip_leading_space(value);
    if (value.contains(rxperlref, 0))
	value = value.before('\n');
}

// Bring VALUE into a form that might be recognized by DDD
void GDBAgent::munch_perl_array(string& value, bool hash)
{
    int n = value.freq('\n');
    string *lines = new string[n + 1];
    split(value, lines, n + 1, '\n');

    bool compact = false;
    bool first_elem = true;
    string new_value;
    bool arrow = true;

    for (int i = 0; i < n; i++)
    {
	string line = lines[i];
	if (!compact && line.contains(' ', 0))
	    continue;		// Sub-element; ignore line

	strip_space(line);

	if (!compact && line.contains(rxint, 0))
	{
	    // Strip index
	    line = line.after(rxint);

	    if (line.contains("..", 0))
	    {
		// In compact representation, Perl omits individual
		// indexes, and puts a START..END index instead.
		compact = true;
		line = line.after(rxint);
	    }
	    strip_space(line);
	}

	if (!first_elem)
	{
	    if (hash && arrow)
		new_value += " => ";
	    else
		new_value += ", ";
	    arrow = !arrow;
	}

	first_elem = false;

	new_value += line;
    }

    delete[] lines;

    if (!new_value.contains('(', 0))
	new_value = '(' + new_value + ')';

    value = new_value;
}

// Bring VALUE of VAR into a form understood by DDD
void GDBAgent::munch_value(string& value, const string& var) const
{
    while (value.contains(var + " = ", 0))
	value = value.after(" = ");
    strip_leading_space(value);

    if (gdb->type() != PERL || !value.contains(rxint, 0))
	return;

    // Special treatment
    if (!var.empty() && var[0] == '@')
	munch_perl_array(value, false);
    else if (!var.empty() && var[0] == '%')
	munch_perl_array(value, true);
    else
	munch_perl_scalar(value);
}



//-----------------------------------------------------------------------------
// Handle error messages
//-----------------------------------------------------------------------------

void GDBAgent::PanicHP(Agent *source, void *, void *call_data)
{
    string msg = STATIC_CAST(char *,call_data);
    string path = source->path();
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    if (gdb != 0)
	path = downcase(gdb->title());
    std::cerr << path << ": " << msg << "\n";
}

void GDBAgent::StrangeHP(Agent *source, void *client_data, void *call_data)
{
    string msg = STATIC_CAST(char *,call_data);
    msg.prepend("warning: ");
    PanicHP(source, client_data, CONST_CAST(char*,msg.chars()));
}

// Terminator
void GDBAgent::abort()
{
    // Clean up now
    TTYAgent::abort();

    // Reset state (in case we're restarted)
    state             = BusyOnInitialCmds;
    _verbatim         = false;
    _recording        = false;
    _detect_echos     = true;
    last_prompt       = "";
    last_written      = "";
    echoed_characters = -1;
    questions_waiting = false;
    complete_answer   = "";

    set_exception_state(false);
}
