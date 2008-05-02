#ifndef GTK_WRAPPER_H
#define GTK_WRAPPER_H

#if defined(IF_MOTIF)

#include <X11/Intrinsic.h>



// ****************************************************************************

#else // NOT IF_MOTIF

#include <GUI/Widget.h>
#include <gtkmm/widget.h>

// Macros



// End menus

// Gdk

#include <gdkmm/display.h>
#include <gdkmm/screen.h>
#include <gdkmm/color.h>
#include <gdkmm/colormap.h>
#include <gdkmm/cursor.h>
#include <gdkmm/region.h>


typedef Gtk::Widget *Widget;

// typedef long XmTextPosition;

typedef bool Boolean;
typedef bool Bool;

#define False false
#define True true

#if 1
typedef unsigned int Cardinal;
typedef void *XtPointer;

#ifdef NAG_ME
#warning Perhaps we should use a gconfmm Value?
#endif
typedef Glib::ValueBase XtArgVal;
#endif

typedef int Position;

// Simple typedefs
typedef int Dimension;

typedef guint32 Time;
#define CurrentTime 0 /* special time */

// End simple typedefs

// FIXME globals
extern Glib::RefPtr<Pango::Context> pango_context;

// Callbacks, actions, signals, slots

typedef sigc::slot<void, void*> XtTimerCallbackProc;

// client_data is bound.
// call_data is never used by gtkmm.
typedef sigc::slot<void, Widget> XtCallbackRec;

#if defined(IF_XM)
typedef sigc::slot<void, Widget, XEvent*, String*, Cardinal*> XtActionProc;
#else
typedef sigc::slot<void, GUI::Widget *, GUI::Event *, GUI::String *, unsigned int *> XtActionProc;
#endif

// enums

#if 1
enum {
  XmPIXMAP = 1,
  XmSTRING,
  XmPIXMAP_AND_STRING
};

enum XmOrientation {
  XmNO_ORIENTATION,
  XmVERTICAL,
  XmHORIZONTAL
};

enum XmButtonState {
  XmUNSET,
  XmSET,
  XmINDETERMINATE
};

enum {
  XmEXPLICIT,
  XmPOINTER
};
#endif

// End enums

// Widgets

#include <gtkmm/container.h>

#define XtManageChild(w) (w)->show()
#define XtUnmanageChild(w) (w)->hide()
extern void XtRealizeWidget(Widget w);

#define XtName(w) (w)->get_name().c_str()
#define XtIsSensitive(w) (w)->is_sensitive()

extern bool XtIsWidget(Widget w);

#define XtIsManaged(w) (w)->is_visible()

#define XtParent(w) (w)->get_parent()

#define XtSetSensitive(w, s) (w)->set_sensitive(s)

#ifdef NAG_ME
#warning XmUpdateDisplay?
#endif

#define XmUpdateDisplay(w)

// End widgets

// Resources

#define XmNcolumns "columns"
#define XmNrows "rows"
#define XmNwidth "width"
#define XmNheight "height"

// Buttons

#include <gtkmm/togglebutton.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/checkmenuitem.h>

#ifdef NAG_ME
#warning Convenience function to cast Widget to Button and set label.
#endif
void set_button_label(Gtk::Widget *, const Glib::ustring &);

// End Buttons

// Convenience functions

// Copy text from any widget to default clipboard.
// Glib::RefPtr<Gtk::Clipboard> clip =
//   Gtk::Clipboard::get_for_display(XtDisplay(w));
bool text_copy_from(Widget w);
bool text_cut_from(Widget w);

// End convenience functions

#define XtSetLanguageProc(a, b, c)

Gtk::Window *find_toplevel(Gtk::Widget *w);




#endif // IF_MOTIF

// pString: "portable" string type.
// cpString: constant pString.

#if defined(IF_XM)

#define ListView_P Widget

typedef char *pString;
typedef const char *cpString;

#else

#include <GUI/WidgetPtr.h>
#include <GUI/ListView.h>

#define ListView_P GUI::WidgetPtr<GUI::ListView>

typedef GUI::String pString;
typedef GUI::String cpString;

#endif

#endif // GTK_WRAPPER_H
