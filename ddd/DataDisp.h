// $Id$
// Data Display

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

#ifndef _DataDisp_h
#define _DataDisp_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// Diese Klasse erzeugt das Data-Display-Fenster und stellt die noetigen
// Callback-Funktionen zur Verfuegung.
//
// Namenskonventionen:
// ...CB  : Callback-Funktion.
// ...DCB : Dialog-Callback-Funktion.
// ...CD  : 'create dialog' ; erzeugt einen Dialog.
// ...Act : Aktions-Funktion.
// ...SQ  : ruft gdb->send_question() auf.
// ...OQC : fuer on_question_completion, Typ: OQCProc, siehe GDBAgent.
// ...SQA : ruft gdb->send_qu_array() auf.
// ...OQAC: fuer on_question_array_completion, Typ: OQACProc, siehe GDBAgent.
// ...HP  : Handler-Prozedur. Typ: HandlerProc, siehe HandlerL.h
// 
//-----------------------------------------------------------------------------

// includes von Motif-Zeugs
#include <Xm/Xm.h>

// includes von Nora-Zeugs
#include "strclass.h"
#include "MakeMenu.h"
#include "HelpCB.h"
#include "ExitCB.h"
#include "events.h"
#include "VSEFlags.h"
#include "VSLLib.h"
#include "Delay.h"

// includes eigener Sachen
#include "ArgField.h"
#include "GDBAgent.h"
#include "DispGraph.h"
#include "DispBox.h"
#include "DispBox.h"


//-----------------------------------------------------------------------------
extern GDBAgent* gdb;
extern ArgField* source_arg;

//-----------------------------------------------------------------------------
class DataDisp {

    //-----------------------------------------------------------------------
    // Callbacks
    //-----------------------------------------------------------------------
    static void dispCB                  (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Button Callbacks
    //-----------------------------------------------------------------------
    static void dereferenceCB           (Widget, XtPointer, XtPointer);
    static void toggleDetailCB          (Widget, XtPointer, XtPointer);
    static void toggleRotateCB          (Widget, XtPointer, XtPointer);
    static void toggleDisableCB         (Widget, XtPointer, XtPointer);
    static void newCB                   (Widget, XtPointer, XtPointer);
    static void enableCB                (Widget, XtPointer, XtPointer);
    static void disableCB               (Widget, XtPointer, XtPointer);
    static void showDetailCB            (Widget, XtPointer, XtPointer);
    static void hideDetailCB            (Widget, XtPointer, XtPointer);
    static void dependentCB             (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Callbacks der Popup-Menues
    //-----------------------------------------------------------------------
    static void popup_new_argCB         (Widget, XtPointer, XtPointer);
    static void popup_newCB             (Widget, XtPointer, XtPointer);
    static void dependent_displayDCB    (Widget, XtPointer, XtPointer);
    static void new_displayDCB          (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Callbacks of the display editor
    //-----------------------------------------------------------------------
    static void UpdateGraphEditorSelectionCB    (Widget, XtPointer, XtPointer);
    static void UpdateDisplayEditorSelectionCB  (Widget, XtPointer, XtPointer);

    static bool ignore_update_graph_editor_selection;
    static bool ignore_update_display_editor_selection;

    //-----------------------------------------------------------------------
    // Sorting nodes for layout
    //-----------------------------------------------------------------------
    static void CompareNodesCB (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Dialog-Erzeugung
    //-----------------------------------------------------------------------
    static void new_displayCD (BoxPoint box_point = BoxPoint(-1,-1));

    //-----------------------------------------------------------------------
    // Sensitivitaet setzen
    //-----------------------------------------------------------------------
    static void no_displaysHP (void*, void*, void*);

    //-----------------------------------------------------------------------
    // Hilfs-Prozeduren
    //-----------------------------------------------------------------------

    static void source_argHP (void*, void*, void*);
    static void graph_argHP (void*, void*, void*);
    static void graph_unselectHP (void*, void*, void*);
    static void gdb_ready_for_questionHP (void*, void*, void*);

    static void set_args(BoxPoint p = BoxPoint(-1, -1));
    static void refresh_args();
    static void fill_labels();

    static DispValue *selected_value();
    static DispNode  *selected_node();

    static DispNode  *new_node(string& answer);

public:
    static void set_delay ();
    static void unset_delay ();

    static void set_handlers();
private:

    static inline int getDispNrAtPoint (BoxPoint point);

    //-----------------------------------------------------------------------
    // Aktions-Prozeduren
    //-----------------------------------------------------------------------
    static void graph_selectAct         (Widget, XEvent*, String*, Cardinal*);
    static void graph_select_or_moveAct (Widget, XEvent*, String*, Cardinal*);
    static void graph_popupAct          (Widget, XEvent*, String*, Cardinal*);
    static void graph_dereferenceAct    (Widget, XEvent*, String*, Cardinal*);
    static void graph_detailAct         (Widget, XEvent*, String*, Cardinal*);
    static void graph_rotateAct         (Widget, XEvent*, String*, Cardinal*);
    static void graph_dependentAct      (Widget, XEvent*, String*, Cardinal*);

    //-----------------------------------------------------------------------
    // Xt-Zeugs
    //-----------------------------------------------------------------------
    static XtActionsRec actions [];

    //-----------------------------------------------------------------------
    // Menues
    //-----------------------------------------------------------------------
private:
    static MMDesc graph_popup[];
    static MMDesc node_popup[];
    static MMDesc graph_cmd_area[];
    static MMDesc display_area[];

    //-----------------------------------------------------------------------
    // Anfragen an gdb stellen und Antworten bearbeiten
    //-----------------------------------------------------------------------
public:
    // sendet den Display-Befehl an den gdb
    // wird ein Boxpoint uebergeben, so wird der neue Knoten dorthin gesetzt
    // sonst an eine Default-Position.
    //
    static void new_displaySQ       (string display_expression,
				     BoxPoint* p = 0,
				     Widget origin = 0);

    // sendet den Befehl "display" an den gdb,
    // um Displays zu aktualisieren. (kein Loeschen von Displays)
    //
    static void refresh_displaySQ   ();

    // sendet die Befehle "info display" und "display" an den gdb,
    // um Displays zu aktualisieren.
    //
    static void refresh_displaySQA  (Widget origin = 0);

    // sendet den 'disable display'-Befehl mit den Nummern an den gdb
    // und aktualisiert den disp_graph.
    //
    static void disable_displaySQ   (int display_nrs[], int count);

    // sendet den 'enable display'-Befehl mit den Nummern an den gdb
    // und aktualisiert den disp_graph.
    //
    static void enable_displaySQ    (int display_nrs[], int count);

    // sendet den 'delete display'-Befehl mit den Nummern an den gdb
    // und aktualisiert den disp_graph.
    //
    static void delete_displaySQ    (int display_nrs[], int count);

    // sendet den Display-Befehl an den gdb (mit disp_nr als data).
    // spaeter wird disp_graph->insert_dependent (answer, data) aufgerufen.
    //
    static void dependent_displaySQ (string display_expression, int disp_nr);

    // wertet Antwort auf 'info display' aus und loescht bei Bedarf Displays.
    //
    static void process_info_display (string& info_display_answer);

    // liest die in display_answer enthaltenen Displays und aktualisiert die
    // eigenen Displays entsprechend. Fremde Displays werden zurueckgegeben.
    // Enthielt display_answer eine Fehlermeldung so ist disabling_occurred
    // false und es sollte ein display-Befehl geschickt und diese Funktion
    // erneut aufgerufen werden.
    //
    static string process_displays  (string& display_answer,
				     bool& disabling_occurred);
private:
    static void new_displayOQC       (string answer, void* data);
    static void new_display_extraOQC (string answer, void* data);
    static void new_displaysSQA      (string display_expression, BoxPoint* p);
    static void new_displaysOQAC     (string answers[],
				      void*  qu_datas[],
				      int    count,
				      void*  data);
    static void refresh_displayOQC   (string answer, void*  data);
    static void refresh_displayOQAC  (string answers[],
				      void*  qu_datas[],
				      int    count,
				      void*  data);
    static void disable_displayOQC   (string answer, void* data);
    static void enable_displayOQC    (string answer, void* data);
    static void delete_displayOQC    (string answer, void* data);
    static void dependent_displayOQC (string answer, void* data);
    static void dependent_display_extraOQC (string answer, void* data);
    static void dependent_displaysSQA (string display_expression, int disp_nr);
    static void dependent_displaysOQAC(string answers[],
				       void*  qu_datas[],
				       int    count,
				       void*  data);



    //-----------------------------------------------------------------------
    // Daten
    //-----------------------------------------------------------------------
    static DispGraph *disp_graph;
    static Delay *delay;
    static Widget last_origin;
    static Time last_select_time;

    static Widget graph_popup_w;
    static Widget node_popup_w;

    static Widget edit_displays_dialog_w;
    static Widget display_list_w;

public:
    static Widget graph_edit;
    static Widget graph_cmd_w;
    static ArgField *graph_arg;

    // Constructor
    DataDisp (XtAppContext app_context,
	      Widget       parent,
	      String       vslpath,          // aus app_data
	      String       vslLibrary,       // aus app_data
	      int          max_name_length,  // aus app_data
	      bool         panned);          // aus app_data

    inline int count_all() { return disp_graph->count_all(); }
    static void refresh_graph_edit ();

    // Callbacks for menu bar
    static void EditDisplaysCB(Widget, XtPointer, XtPointer);
    static void refreshCB(Widget, XtPointer, XtPointer);
    static void selectAllCB(Widget, XtPointer, XtPointer);
    static void deleteCB(Widget, XtPointer, XtPointer);
};

#endif
