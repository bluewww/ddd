// $Id$
// Communicate with separate GDB process

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

//-----------------------------------------------------------------------------
// A GDBAgent creates a connection to an inferior GDB via a TTYAgent.
// There are three ways to send commands to GDB:
// 1. send_user_cmd: is used for user input at the GDB prompt.  GDB output
//    (partial answers) are processed immediately.
// 2. send_question: is used for internal communication.  All answers
//    are buffered until the whole answer is received.  send_question can
//    only be used if GDB is ready for input.
// 3. send_qu_array: issues a list of queries to GDB.  The passed function
//    is called as soon as the last query is processed.  The passed function
//    must free the memory claimed by the query array.
//-----------------------------------------------------------------------------

#ifndef _DDD_GDBAgent_h
#define _DDD_GDBAgent_h

#ifdef __GNUG__
#pragma interface
#endif

#include "TTYAgent.h"
#include "HandlerL.h"
#include "strclass.h"
#include "bool.h"
#include "assert.h"

//-----------------------------------------------------------------------------
// Debugger types
//-----------------------------------------------------------------------------

enum DebuggerType { GDB, DBX, XDB };


//-----------------------------------------------------------------------------
// Program language
//-----------------------------------------------------------------------------

#ifdef LANGUAGE_C
#undef LANGUAGE_C		// DEC defines this
#endif

enum ProgramLanguage { 
    LANGUAGE_C,			// C-like: C, C++
    LANGUAGE_PASCAL,		// Pascal-like: Pascal, Modula, Ada
    LANGUAGE_CHILL,		// Chill, as supported by GDB.
    LANGUAGE_FORTRAN,		// FORTRAN, as supported by GDB.
    LANGUAGE_OTHER		// Others
};

//-----------------------------------------------------------------------------
// Procedure types.
//-----------------------------------------------------------------------------

// Called as soon as an ANSWER on send_user_cmd (or a part of it)
// is received
typedef void (* OAProc) (const string& answer,
			 void* user_data);

// Called after the whole answer to send_user_cmd has been received
typedef void (* OACProc) (void* user_data);

// Called from send_question with the complete answer
typedef void (* OQCProc) (const string& complete_answer,
			  void*  qu_data);

// Called from send_qu_array with the complete answers
typedef void (* OQACProc) (string complete_answers[],
			   void*  qu_datas[],
			   int    count,
			   void*  data);

//-----------------------------------------------------------------------------
// Determine debugger type
DebuggerType debugger_type(const string& type);

// Create appropriate call for the given debugger
string build_gdb_call(DebuggerType debugger_type,
		      const string& debugger_command,
		      const string& init_file,
		      int argc, char *argv[],
		      string myArguments = "");

// Handlers
const unsigned ReadyForQuestion = TTYAgent_NTypes;
const unsigned ReadyForCmd      = ReadyForQuestion + 1;
const unsigned LanguageChanged  = ReadyForCmd + 1;
const unsigned ReplyRequired    = LanguageChanged + 1;
const unsigned EchoDetected     = ReplyRequired + 1;
const unsigned GDBAgent_NTypes  = EchoDetected + 1;

// Handler info
struct ReplyRequiredInfo {
    string question;		// Question asked by GDB
    string reply;		// Our reply

    ReplyRequiredInfo()
	: question(), reply()
    {}
};

//-----------------------------------------------------------------------------
// The GDBAgent class.
//-----------------------------------------------------------------------------

class GDBAgent: public TTYAgent {
public:
    DECLARE_TYPE_INFO

protected:
    enum State {
	ReadyWithPrompt,
	BusyOnCmd, 
	BusyOnQuestion, 
	BusyOnQuArray,
	BusyOnInitialCmds
    };
    State state;		// Current state

private:
    DebuggerType    _type;	// Debugger type
    void*           _user_data;	// used in callbacks etc.

    bool _has_frame_command;	// Debugger properties
    bool _has_func_command;	
    bool _has_run_io_command;
    bool _has_print_r_option;
    bool _has_output_command;
    bool _has_where_h_option;
    bool _has_display_command;
    bool _has_clear_command;
    bool _has_handler_command;
    bool _has_pwd_command;
    bool _has_setenv_command;
    bool _has_edit_command;
    bool _has_make_command;
    bool _has_jump_command;
    bool _has_regs_command;
    bool _has_named_values;
    bool _has_when_command;
    bool _has_when_semicolon;
    bool _has_delete_comma;
    bool _has_err_redirection;
    bool _has_givenfile_command;
    bool _has_cont_sig_command;

    ProgramLanguage _program_language; // Current program language

    bool _trace_dialog;		// True if dialog is to be traced
    bool _verbatim;		// True if in verbatim mode

    string last_prompt;		// Last prompt received
    string last_written;	// Last command sent
    int echoed_characters;      // # of echoed characters so far (-1: no echo)

    GDBAgent& operator = (const GDBAgent&) { assert(0); return *this; }

protected:
    // Return PREFIX + EXPR, parenthesizing EXPR if needed
    static string prepend_prefix(const string& prefix, const string& expr);

    // Return EXPR + SUFFIX, parenthesizing EXPR if needed
    static string append_suffix(const string& expr, const string& suffix);

public:
    // Constructor
    GDBAgent (XtAppContext app_context,
	      const string& gdb_call,
	      DebuggerType type,
	      unsigned nTypes = GDBAgent_NTypes);

    // Duplicator
    GDBAgent (const GDBAgent& gdb);
    virtual Agent *dup() const { return new GDBAgent(*this); }

    ~GDBAgent ();

    // Start new process
    void do_start (OAProc  on_answer,
		   OACProc on_answer_completion,
		   void*   user_data);

    // After receiving the first GDB prompt, send commands to GDB.
    // Processing is very much like send_user_cmd_plus.
    void start_plus (OAProc   on_answer,
		     OACProc  on_answer_completion,
		     void*    user_data,
		     string   cmds [],
		     void*    qu_datas [],
		     int      qu_count,
		     OQACProc on_qu_array_completion,
		     void*    qa_data);

    // true iff command was sent.
    // If user_data == 0, _user_data remains unchanged.
    bool send_user_cmd      (string cmd, void* user_data = 0);  
    bool send_user_ctrl_cmd (string cmd, void* user_data = 0);


    // Order of tasks:
    // 1. Send USER_CMD to GDB, as in send_user_cmd
    //    (state: ReadyWithPrompt --> BusyOnCmd)
    // 2. Call OAProc when answer comes in
    //    (state:BusyOnCmd --> BusyOnQuArray)
    // 3. Send CMDS, as in send_qu_array.
    // 4. When all replies have come in: call OACProc and OQACProc.
    //
    bool send_user_cmd_plus (string   cmds [],
			     void*    qu_datas [],
			     int      qu_count,
			     OQACProc on_qu_array_completion,
			     void*    qa_data,
			     string user_cmd, void* user_data = 0);

    bool send_question (string  cmd,
			OQCProc on_question_completion,
			void*   qu_data);

    bool send_qu_array (string   cmds [],
			void*    qu_datas [],
			int      qu_count,
			OQACProc on_qu_array_completion,
			void*    qa_data);


    // Resources
    DebuggerType type()       const { return _type; }
    string title()            const;
    bool isReadyWithPrompt()  const { return state == ReadyWithPrompt; }
    bool isBusyOnCmd()        const { return state == BusyOnCmd
					  || state == BusyOnInitialCmds; }
    bool isBusyOnQuestion()   const { return state == BusyOnQuestion
					  || state == BusyOnQuArray; }
    string prompt()           const { return last_prompt; }


    // Debugger properties
    // True if debugger has `frame' command
    bool has_frame_command() const    { return _has_frame_command; }	
    bool has_frame_command(bool val)  { return _has_frame_command = val; }

    // True if debugger has `func' command
    bool has_func_command() const    { return _has_func_command; }	
    bool has_func_command(bool val)  { return _has_func_command = val; }

    // True if debugger has `run_io' command
    bool has_run_io_command() const    { return _has_run_io_command; }
    bool has_run_io_command(bool val)  { return _has_run_io_command = val; }

    // True if debugger has `-r' option in `print' and `display'
    bool has_print_r_option() const   { return _has_print_r_option; }
    bool has_print_r_option(bool val) { return _has_print_r_option = val; }

    // True if debugger has `output' command
    bool has_output_command() const   { return _has_output_command; }
    bool has_output_command(bool val) { return _has_output_command = val; }

    // True if debugger has `-h' option in `where'
    bool has_where_h_option() const   { return _has_where_h_option; }
    bool has_where_h_option(bool val) { return _has_where_h_option = val; }

    // True if debugger has `display' command
    bool has_display_command() const   { return _has_display_command; }
    bool has_display_command(bool val) { return _has_display_command = val; }

    // True if debugger has `clear' command
    bool has_clear_command() const     { return _has_clear_command; }
    bool has_clear_command(bool val)   { return _has_clear_command = val; }

    // True if debugger has `handler' command
    bool has_handler_command() const   { return _has_handler_command; }
    bool has_handler_command(bool val) { return _has_handler_command = val; }

    // True if debugger has `pwd' command
    bool has_pwd_command() const       { return _has_pwd_command; }
    bool has_pwd_command(bool val)     { return _has_pwd_command = val; }

    // True if debugger has `setenv' command
    bool has_setenv_command() const    { return _has_setenv_command; }
    bool has_setenv_command(bool val)  { return _has_setenv_command = val; }

    // True if debugger has `edit' command
    bool has_edit_command() const      { return _has_edit_command; }
    bool has_edit_command(bool val)    { return _has_edit_command = val; }

    // True if debugger has `make' command
    bool has_make_command() const      { return _has_make_command; }
    bool has_make_command(bool val)    { return _has_make_command = val; }

    // True if debugger has `jump' command
    bool has_jump_command() const      { return _has_jump_command; }
    bool has_jump_command(bool val)    { return _has_jump_command = val; }

    // True if debugger has `regs' command
    bool has_regs_command() const      { return _has_regs_command; }
    bool has_regs_command(bool val)    { return _has_regs_command = val; }

    // True if debugger issues `NAME = VALUE' upon `print' commands
    bool has_named_values() const      { return _has_named_values; }
    bool has_named_values(bool val)    { return _has_named_values = val; }

    // True if debugger has `when' command
    bool has_when_command() const      { return _has_when_command; }
    bool has_when_command(bool val)    { return _has_when_command = val; }

    // True if debugger wants `;' at the end of `when' command specs
    bool has_when_semicolon() const    { return _has_when_semicolon; }
    bool has_when_semicolon(bool val)  { return _has_when_semicolon = val; }

    // True if debugger wants events separated by `,'
    bool has_delete_comma() const      { return _has_delete_comma; }
    bool has_delete_comma(bool val)    { return _has_delete_comma = val; }

    // True if debugger has stderr redirection using `>&'
    bool has_err_redirection() const   { return _has_err_redirection; }
    bool has_err_redirection(bool val) { return _has_err_redirection = val; }

    // True if debugger has `givenfile' command
    bool has_givenfile_command() const   { return _has_givenfile_command; }
    bool has_givenfile_command(bool val) 
    {
	return _has_givenfile_command = val;
    }

    // True if debugger has `cont sig' command
    bool has_cont_sig_command() const   { return _has_cont_sig_command; }
    bool has_cont_sig_command(bool val) { return _has_cont_sig_command = val; }

    // Current program language
    ProgramLanguage program_language() const   { return _program_language; }
    ProgramLanguage program_language(ProgramLanguage val) 
    {
	if (_program_language != val)
	{
	    _program_language = val;
	    callHandlers(LanguageChanged, (void *)val);
	}
	return program_language();
    }
    ProgramLanguage program_language(string text);

    // True if debugger can enable breakpoints
    bool has_enable_command() const
    { 
	return type() == GDB || type() == XDB || has_handler_command();
    }
    bool has_disable_command() const
    {
	return has_enable_command();
    }

    // True if debugger can set ignore counts on breakpoints
    bool has_ignore_command() const
    {
	return type() == GDB || type() == XDB || has_handler_command();
    }

    // True if debugger can set conditions on breakpoints
    bool has_condition_command() const
    {
	return type() == GDB;
    }

    // True if debugger dialog is traced on clog
    bool trace_dialog() const    { return _trace_dialog; }
    bool trace_dialog(bool val);

    // True if DBX output is to be processed verbatim
    bool verbatim() const        { return _verbatim; }
    bool verbatim(bool val)      { return _verbatim = val; }

    // GDB: "print EXPR".  If INTERNAL is set, avoid side effects
    // (such as setting the value history in GDB).
    string print_command(string expr = "", bool internal = true) const; 
    // GDB: "set variable VAR = EXPR"
    string assign_command(string var, string expr) const;

    string display_command(string expr = "") const; // GDB: "display EXPR"
    string where_command(int count = 0) const;	    // GDB: "where COUNT"
    string pwd_command() const;	                    // GDB: "pwd "
    string frame_command(int number) const;         // GDB: "frame NUMBER"
    string frame_command() const;                   // GDB: "frame"
    string func_command() const;                    // DBX: "func"
    string echo_command(string text) const;         // GDB: "echo TEXT"
    string whatis_command(string expr) const;       // GDB: "whatis EXPR"
    string dereferenced_expr(string expr) const;    // GDB: "*EXPR"
    string address_expr(string expr) const;         // GDB: "&EXPR"
    string index_expr(string expr, int index) const; // GDB: "EXPR[INDEX]"
    int default_index_base() const;                 // GDB: 0 in C, else 1
    string info_locals_command() const;	            // GDB: "info locals"
    string info_args_command() const;	            // GDB: "info args"
    string disassemble_command(string start, string end = "") const;
                                                // GDB: "disassemble START END"
    string make_command(string target) const;       // GDB: "make TARGET"
    string jump_command(string pc) const;           // GDB: "jump PC"
    string regs_command(bool all = true) const;	    // GDB: "info registers"
    string kill_command() const;                    // GDB: "kill"
    string enable_command(string bp = "") const;    // GDB: "enable BP"
    string disable_command(string bp = "") const;   // GDB: "disable BP"
    string delete_command(string bp = "") const;    // GDB: "delete BP"
    string ignore_command(string bp, int count) const; 
                                                    // GDB: "ignore BP COUNT"
    string condition_command(string bp, string expr) const; 
				                    // GDB: "cond BP EXPR"
    string shell_command(string cmd) const;	    // GDB: "shell CMD"
    string debug_command(string program = "") const; // GDB: "file PROGRAM"
    string signal_command(int sig) const;           // GDB: "signal SIG"

    // Default history file
    string history_file() const;                    // GDB: "~/.gdb_history"

    // Send DATA to process
    virtual int write(const char *data, int length)
    {
	last_written = string(data, length);
	echoed_characters = 0;
	return TTYAgent::write(data, length);
    }

    // Custom function
    int write(const string& data)
    {
	return write(data.chars(), data.length());
    }

    bool ends_with_prompt(const string& answer);
    bool ends_with_secondary_prompt(const string& answer);

private:
    bool questions_waiting;

    void* _qu_data;

    int     qu_index, _qu_count;
    string* cmd_array;
    string* complete_answers;
    void**  _qu_datas;
    void*   _qa_data;

    OAProc   _on_answer;
    OACProc  _on_answer_completion;
    OQCProc  _on_question_completion;
    OQACProc _on_qu_array_completion;

    void    init_qu_array (string   cmds [],
			   void*    qu_datas [],
			   int      qu_count,
			   OQACProc on_qu_array_completion,
			   void*    qa_data);

    string requires_reply(const string& answer);

    void cut_off_prompt(string& answer) const;
    void strip_dbx_comments(string& answer) const;
    void strip_control(string& answer) const;
    void strip_xdb_control(string& answer) const;
    void bad_xdb_control(const string &s, int p, char c, 
			 const char *expecting) const;

    void normalize_answer(string& answer) const;
    void normalize_address(string& addr) const;

protected:
    string complete_answer;

    static void InputHP  (Agent *, void *, void *);
    static void PanicHP  (Agent *, void *, void *);
    static void StrangeHP(Agent *, void *, void *);
    static void DiedHP   (Agent *, void *, void *);

    static void traceInputHP (Agent *, void *, void *);
    static void traceOutputHP(Agent *, void *, void *);
    static void traceErrorHP (Agent *, void *, void *);
};

#endif // _DDD_GDBAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
