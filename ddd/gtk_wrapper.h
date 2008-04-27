#ifndef GTK_WRAPPER_H
#define GTK_WRAPPER_H

#if defined(IF_MOTIF)

#include <X11/Intrinsic.h>

typedef Widget FIXED_P;
typedef XImage *XIMAGE_P;

typedef const char *NAME_T;
#define NO_TIMER 0
#define NO_DISPLAY 0
#define NO_SCREEN 0

#define NO_WINDOW 0

#define XtNIL (XtPointer)0
#define DEFAULT_FONT_NAME "fixed"

#define MAYBE_FALSE
#define MAYBE_TRUE

#define NO_UPDATE

// XtWorkProc
#define IDLE_STOP True
#define IDLE_CONT False

typedef XtCallbackProc GTK_SLOT_WP;
typedef XtCallbackProc GTK_PROC_WP;
typedef XtCallbackProc GTK_SLOT_P;
typedef XtCallbackProc GTK_PROC_P;
typedef XtCallbackProc GTK_SLOT_W;
typedef XtCallbackProc GTK_PROC_W;
typedef XtCallbackProc GTK_SLOT_;
typedef XtCallbackProc GTK_PROC_;

#define USTRING(s) (s)
#define PTR_FUN(f) (f)

#define WAITREC(f) { f }



#define BIND_1(f, c) { f, (void *)c }
#define BIND_0(f) { f, XtNIL }
#define HIDE_0(f) { f, XtNIL }
#define HIDE_1(f) { f, XtNIL }
#define HIDE_0_BIND_1(f, c) { f, (void *)c }

#define M_ARGS_1(w) w
#define M_ARGS_2(w, a) w, a
#define M_ARGS_3(w, a, b) w, a, b
#define M_ARGS_4(w, a, b, c) w, a, b, c

#define BUTTON_CB_RETURN_TYPE void
#define DELETE_CB_RETURN_TYPE void

#define XtCastShell(w) (w)
#define XmCastDialogShell(w) (w)

typedef Pixel ImageColor;

typedef Widget Glyph_T;

// ****************************************************************************

#else // NOT IF_MOTIF

extern int complain;

#include <glibmm/listhandle.h>
#include <glibmm/main.h>
#include <gtkmm/widget.h>
#include <libxml/tree.h>
#include <ext/hash_map>

// Macros



#ifdef NAG_ME
#warning Usually used for shells, not menus, so:
#endif
#define XtPopdown(w) (w)->hide()

#define XmMenuPosition(w, e) do {} while(0)

// End menus

// Gdk

#include <gdkmm/display.h>
#include <gdkmm/screen.h>
#include <gdkmm/color.h>
#include <gdkmm/colormap.h>
#include <gdkmm/cursor.h>
#include <gdkmm/region.h>

#define XFlush(d) ((d)->flush())



typedef unsigned long Pixel;
typedef Gdk::Color XColor;

typedef Gdk::Color ImageColor;

struct error_struct {
    int foo;
};
typedef error_struct XWindowAttributes;

typedef Glib::RefPtr<Gdk::Colormap> Colormap;

typedef Glib::RefPtr<Gdk::Pixbuf> XIMAGE_P;

typedef Gdk::Region *Region;

// End gdk

// Gdk events

#define StructureNotifyMask Gdk::STRUCTURE_MASK
#define VisibilityChangeMask Gdk::VISIBILITY_NOTIFY_MASK

typedef GdkEvent XEvent;
typedef GdkEventButton XButtonEvent;

#define ShiftMask GDK_CONTROL_MASK
#define LockMask GDK_LOCK_MASK
#define ControlMask GDK_CONTROL_MASK
#define Mod1Mask GDK_MOD1_MASK
#define Mod2Mask GDK_MOD2_MASK
#define Mod3Mask GDK_MOD3_MASK
#define Mod4Mask GDK_MOD4_MASK
#define Mod5Mask GDK_MOD5_MASK


typedef guint KeySym;

// End gdk events

// Utilities

#define XtNumber(x) (sizeof(x)/sizeof(x[0]))

#define XtMalloc(n) malloc(n)
#define XtFree(p) free(p)
#define XtRealloc(p,n) realloc(p,n)

// End Utilities

// Displays and screens

#define XDisplayString(d) (d->get_default_screen()->make_display_name().c_str())

// End Displays and screens

typedef Gtk::Widget *Widget;
typedef long XmTextPosition;

#define DEFAULT_FONT_NAME "Monospace 12"

typedef bool Boolean;
typedef bool Bool;

#define False false
#define True true

// Strings

#include <string.h>

#define _XtString char*
typedef Glib::ustring XmString;
typedef char *String;
#define GLIB_NOSTRING (Glib::ustring::size_type)-1
#define USTRING(s) Glib::ustring(s.chars())
typedef Glib::ustring NAME_T;

#define XmStringFree(s)

#define XtNewString(s) strdup(s)

// End strings

typedef char *XmStringCharSet;
#define XmSTRING_DEFAULT_CHARSET "IGNORED"
typedef void *XmFontList;             // FIXME: IGNORED

typedef unsigned int Cardinal;
typedef void *XtPointer;
#define XtNIL (XtPointer)0

#ifdef NAG_ME
#warning Perhaps we should use a gconfmm Value?
#endif
typedef Glib::ValueBase XtArgVal;

typedef int Position;

// Application context

// End application context

// Main loop

typedef sigc::connection XtInputId;
typedef sigc::connection XtIntervalId;
typedef sigc::connection XtWorkProcId;
#define NO_CONNECTION sigc::connection()
#define NO_TIMER sigc::connection()

#define MAYBE_FALSE false
#define MAYBE_TRUE true

#define NO_UPDATE true

// XtWorkProc replaced by idle handler (signal_idle())
#define IDLE_STOP false
#define IDLE_CONT true

#define XtInputWriteMask Glib::IO_OUT
#define XtInputReadMask Glib::IO_IN
#define XtInputExceptMask (Glib::IO_PRI|Glib::IO_ERR)


#define XtRemoveInput(c) (c).disconnect()
#define XtRemoveWorkProc(c) (c).disconnect()

// End main loop

// Simple typedefs
typedef int Dimension;

typedef guint32 Time;
#define CurrentTime 0 /* special time */

// End simple typedefs

// FIXME globals
extern Glib::RefPtr<Pango::Context> pango_context;

// Callbacks, actions, signals, slots

#ifdef NAG_ME
#warning Should XtCallbackProc be a function type or a slot (functor)?
#endif
typedef sigc::slot<void, Widget, XtPointer, XtPointer> XtCallbackProc;
typedef void (*XtCallbackProc1)(Widget, XtPointer, XtPointer);

typedef sigc::slot<void, Widget, XtPointer> GTK_SLOT_WP;
typedef void (*GTK_PROC_WP)(Widget, XtPointer);
typedef sigc::slot<void, XtPointer> GTK_SLOT_P;
typedef void (*GTK_PROC_P)(XtPointer);
typedef sigc::slot<void, Widget> GTK_SLOT_W;
typedef void (*GTK_PROC_W)(Widget);
typedef sigc::slot<void> GTK_SLOT_;
typedef void (*GTK_PROC_)(void);

typedef sigc::slot<void, void*> XtTimerCallbackProc;

#define PTR_FUN(f) sigc::retype(sigc::ptr_fun(f))
#define MEM_FUN(o,f) sigc::mem_fun(o,f)

// client_data is bound.
// call_data is never used by gtkmm.
typedef sigc::slot<void, Widget> XtCallbackRec;

#ifdef NAG_ME
#warning Change this signature to something more idiomatic?
#endif
typedef sigc::slot<void, Widget, XEvent*, String*, Cardinal*> XtActionProc;

struct XtActionsRec {
  String string;
  XtActionProc proc;
};

// This is used to create a callback functor from a functor with
// one Widget argument and one client data argument.
#define BIND_1(f, c) sigc::bind(f, c)
// This is used to create a callback functor from a functor with
// exactly one Widget argument.
#define BIND_0(f) f
// This is used to create a callback functor from a functor with no
// arguments.  The Widget argument is ignored.
#define HIDE_0(f) sigc::hide<0>(f)
#define HIDE_1(f) sigc::hide<1>(f)
#define HIDE_0_BIND_1(f, c) sigc::hide<0>(sigc::bind(f, c))

// A simple toggle.  In Motif, the set/unset state is
// passed in the callback structure.
// In gtkmm, we read it fron the widget w.
// Note that the widget may either be a ToggleButton or a CheckMenuItem.
#define CB_ARG_LIST_TOGGLE(w, call_data) Widget w
// A toggle with an additional client_data argument.
#define CB_ARG_LIST_TOGGLE_X(w, client_data, call_data) Widget w, XtPointer client_data

#define M_ARGS_1(w) 
#define M_ARGS_2(w, a) a
#define M_ARGS_3(w, a, b) a, b
#define M_ARGS_4(w, a, b, c) a, b, c

#define BUTTON_CB_RETURN_TYPE bool
#define DELETE_CB_RETURN_TYPE bool

// End callbacks, actions, signals, slots

// enums

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
extern Boolean XtIsRealized(Widget w);

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

// Windows

#include <gtkmm/window.h>

#define XtCastShell(w) dynamic_cast<Gtk::Window *>(w)
#define XmCastDialogShell(w) dynamic_cast<Gtk::Dialog *>(w)

// End windows

// Buttons

#include <gtkmm/togglebutton.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/checkmenuitem.h>

#ifdef NAG_ME
#warning Convenience function to cast Widget to Button and set label.
#endif
void set_button_label(Gtk::Widget *, const Glib::ustring &);

// End Buttons

// "Fixed" widget

#include <gtkmm/fixed.h>

typedef Gtk::Fixed *FIXED_P;

// End "Fixed" widget

// Convenience functions

// Copy text from any widget to default clipboard.
// Glib::RefPtr<Gtk::Clipboard> clip =
//   Gtk::Clipboard::get_for_display(XtDisplay(w));
bool text_copy_from(Widget w);
bool text_cut_from(Widget w);

// End convenience functions

#define XtSetLanguageProc(a, b, c)

Gtk::Window *find_toplevel(Gtk::Widget *w);


class GtkGlyphMark;
typedef GtkGlyphMark *Glyph_T;






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
