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

//-----------------------------------------------------------------------------
// GDBAgent stellt eine Verbindung zu GDB ueber einen TTYAgent her.
// Es gibt drei Arten Befehle an GDB zu schicken:
// 1. send_user_cmd: ist dazu gedacht, die GDB-Befehle, die der Benutzer
//           an einem Prompt eingibt an den GDB weiterzuleiten. Die 
//           Antwort(teile) werden sofort weitergeleitet.
// 2. send_question: ist dazu gedacht, interne Anfragen an GDB zu schicken.
//           Unterschiede zu send_user_cmd:
//           - nur moeglich, wenn die letzte Ausgabe der prompt war. 
//           - die Antwort auf die Anfrage wird gepuffert und erst am Ende
//             weitergeleitet.
//           - bis der naechste GDB-Prompt kommt ist keine weitere Anfrage
//             moeglich.
// 3. send_qu_array: ermoeglicht es, mehrere interne Anfragen im Paket 
//           loszuschicken
//           - die Antworten werden in einem neu erzeugten Array gepuffert
//           - Aufruf der uebergebenen Funktion wenn alle Antworten komplett
//           - die aufgerufene Funktion soll den Speicher der Arrays freigeben!
//-----------------------------------------------------------------------------


#ifndef _GDBAgent_h
#define _GDBAgent_h

#ifdef __GNUG__
#pragma interface
#endif

#include "TTYAgent.h"
#include "HandlerL.h"
#include "strclass.h"
#include "bool.h"

//-----------------------------------------------------------------------------
// Debugger types
//-----------------------------------------------------------------------------

enum DebuggerType { GDB, DBX };


//-----------------------------------------------------------------------------
// Typen der aufzurufenden Prozeduren
//-----------------------------------------------------------------------------

// wird aufgerufen, sobald eine Antwort auf send_user_cmd 
// (oder ein Teil davon) ankommt
typedef void (* OAProc) (string answer,
			 void*  user_data);

// wird aufgerufen, wenn der prompt kommt nach einer Antwort auf send_user_cmd
typedef void (* OACProc) (void* user_data);

// wird aufgerufen mit der kompletten Antwort auf send_question
typedef void (* OQCProc) (string complete_answer,
			  void*  qu_data);

// wird aufgerufen mit den kompletten Antworten auf send_qu_array
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

// Handler zum Anzeigen einer Zustandsaenderung
static const unsigned ReadyForQuestion = 0;
static const unsigned ReadyForCmd      = ReadyForQuestion + 1;

static const unsigned BusyNTypes       = ReadyForCmd + 1;

//-----------------------------------------------------------------------------
// Die Klasse GDBAgent
//-----------------------------------------------------------------------------
class GDBAgent: public TTYAgent {
public:
    DECLARE_TYPE_INFO

protected:
    enum State {ReadyWithPrompt, BusyOnCmd, BusyOnQuestion, BusyOnQuArray,
                BusyOnInitialCmds};
    State state;

private:
    DebuggerType _type;
    void*        _user_data;
    HandlerList  busy_handlers;
    regex        _prompt;

public:
    GDBAgent (XtAppContext app_context,
	      const string& gdb_call,
	      DebuggerType type);

    ~GDBAgent ();

    void do_start (OAProc  on_answer,
		   OACProc on_answer_completion,
		   void*   user_data);

    // Nach Empfang des ersten gdb-Prompts werden cmds abgeschickt.
    // Bearbeitung aehnlich send_user_cmd_plus.
    //
    void start_plus (OAProc   on_answer,
		     OACProc  on_answer_completion,
		     void*    user_data,
		     string   cmds [],
		     void*    qu_datas [],
		     int      qu_count,
		     OQACProc on_qu_array_completion,
		     void*    qa_data);

    // true, wenn Befehl abgschickt wurde 
    // Ist user_data == 0, so bleibt _user_data unveraendert.
    bool send_user_cmd      (string cmd, void* user_data = 0);  
    bool send_user_ctrl_cmd (string cmd, void* user_data = 0);


    // Reihenfolge der Abarbeitung:
    // Abschicken von user_cmd, wie bei send_user_cmd
    // (state:ReadyWithPrompt --> BusyOnCmd)
    // Aufruf von OAProc bei Eintreffen der Antwort.
    // (state:BusyOnCmd --> BusyOnQuArray)
    // Abschicken der cmds, wie bei send_qu_array.
    // Nach Beendigung aller Antworten: Aufruf von OACProc und OQACProc.
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


    // Zustandsabfragen
    DebuggerType type()      const {return _type;}
    bool isReadyWithPrompt() const {return state == ReadyWithPrompt;}
    bool isBusyOnCmd()       const {return (state == BusyOnCmd)
					   || (state == BusyOnInitialCmds);}
    bool isBusyOnQuestion()  const {return (state == BusyOnQuestion)
					   || (state == BusyOnQuArray);}

    void addBusyHandler (unsigned    type,
			 HandlerProc proc,
			 void*       client_data = 0);

    void removeBusyHandler (unsigned    type,
			    HandlerProc proc,
			    void        *client_data = 0);

    void callBusyHandlers ();

    const regex& prompt() const { return _prompt; }
    string default_prompt() const;


    // Konfigurationen
    void set_trace_dialog (bool trace);

private:
    bool trace_dialog;

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

    inline void    init_qu_array (string   cmds [],
				  void*    qu_datas [],
				  int      qu_count,
				  OQACProc on_qu_array_completion,
				  void*    qa_data);
    inline bool ends_with_prompt (const string& answer);
    inline void cut_off_prompt (string& answer);
protected:
    string complete_answer;

    static void InputHP (Agent*, void*, void*);
    static void DiedHP  (Agent*, void*, void*);
    static void PanicHP (Agent*, void*, void*);

    static void traceInputHP (Agent*, void*, void*);
    static void traceOutputHP (Agent*, void*, void*);
    static void traceErrorHP (Agent*, void*, void*);


    virtual int setupChildCommunication();
};
#endif


