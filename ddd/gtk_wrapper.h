#ifndef GTK_WRAPPER_H
#define GTK_WRAPPER_H

#ifdef IF_MOTIF

#include <X11/Intrinsic.h>

typedef int DimType;

#define CHARSTR(s) (s)
class string;
typedef string WidgetNameType;

typedef Display *DISPLAY_P;
typedef Screen *SCREEN_P;

typedef Widget CONTAINER_P;
typedef Widget TOGGLEBUTTON_P;
typedef Widget RADIOBUTTON_P;
typedef Widget BUTTON_P;
typedef Widget DIALOG_P;
typedef Widget NOTEBOOK_P;
typedef Widget BOX_P;
typedef Widget TABLE_P;
typedef Widget LABEL_P;
typedef Widget ENTRY_P;
typedef Widget TOOLBAR_P;
typedef Widget STATUSBAR_P;
typedef Widget SCROLLEDWINDOW_P;
typedef Widget SCROLLEDTEXT_P;
typedef Widget TEXTVIEW_P;
typedef Widget WINDOW_P;
typedef Widget MENUSHELL_P;
typedef Widget MENU_P;
typedef Widget MENUBAR_P;
typedef Widget COMBOBOXENTRYTEXT_P;
typedef Widget SPINBUTTON_P;
typedef Widget FILECHOOSERDIALOG_P;
typedef Widget TREEVIEW_P;
typedef Widget RANGE_P;
typedef Widget SCALE_P;
typedef Widget FIXED_P;
typedef XImage *XIMAGE_P;

typedef const char *NAME_T;
#define NO_WORK 0
#define NO_SOURCE 0
#define NO_TIMER 0
#define NO_DISPLAY 0
#define NO_SCREEN 0
#define NO_GC 0
#define NO_WINDOW 0
typedef XFontStruct *FONT_P;
#define NO_FONT 0
#define XtNIL (XtPointer)0
#define DEFAULT_FONT_NAME "fixed"

#define MAYBE_FALSE
#define MAYBE_TRUE

#define NO_UPDATE

// XtWorkProc
#define WP_RETURN_TYPE Boolean
#define WP_ALIST_NULL XtPointer
#define WP_ALIST_1(d) d
#define WP_ARGS_NULL (XtPointer)0
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

#define TIMEOUT_RETURN_TYPE void
#define TIMEOUT_ARG_LIST(p,id) XtPointer p, XtIntervalId *id
#define TM_ALIST_NULL XtPointer, XtIntervalId *
#define TM_ALIST_1(p) p, XtIntervalId *

// A generic XtCallbackProc has 3 args: widget, client_data and
// call_data.  The CB_ARG_LIST macros document which of these are
// actually used by the callback.  In the GTK version, the unused
// arguments are not declared at all.

#define CB_ARG_LIST_NULL Widget, XtPointer, XtPointer
#define CB_ARG_LIST_1(w) Widget w, XtPointer, XtPointer
#define CB_ARG_LIST_2(client) Widget, XtPointer client, XtPointer
#define CB_ARG_LIST_3(call) Widget, XtPointer, XtPointer call
#define CB_ARG_LIST_12(w, client) Widget w, XtPointer client, XtPointer
#define CB_ARG_LIST_13(w, call) Widget w, XtPointer, XtPointer call
#define CB_ARG_LIST_23(client, call) Widget, XtPointer client, XtPointer call
#define CB_ARG_LIST_123(w, client, call) Widget w, XtPointer client, XtPointer call
#define CB_ALIST_NULL Widget, XtPointer, XtPointer
#define CB_ALIST_1(w) w, XtPointer, XtPointer
#define CB_ALIST_2(client) Widget, client, XtPointer
#define CB_ALIST_3(call) Widget, XtPointer, call
#define CB_ALIST_12(w, client) w, client, XtPointer
#define CB_ALIST_13(w, call) w, XtPointer, call
#define CB_ALIST_23(client, call) Widget, client, call
#define CB_ALIST_123(w, client, call) w, client, call
#define XtP(t) XtPointer
#define CB_ARGS_NULL NULL, NULL, NULL
#define CB_ARGS_1(w)  (Widget)w, XtNIL, XtNIL
#define CB_ARGS_2(client)  (Widget)0, (XtPointer)client, XtNIL
#define CB_ARGS_3(call)  (Widget)0, XtNIL, (XtPointer)call
#define CB_ARGS_12(w, client)  (Widget)w, (XtPointer)client, XtNIL
#define CB_ARGS_13(w, call)  (Widget)w, XtNIL, (XtPointer)call
#define CB_ARGS_23(client, call)  (Widget)0, (XtPointer)client, (XtPointer)call
#define CB_ARGS_123(w, client, call)  (Widget)w, (XtPointer)client, (XtPointer)call
#define CB_ARG_PASS_NULL(f) f
#define CB_ARG_PASS_1(f) f
#define CB_ARG_PASS_2(f) f
#define CB_ARG_PASS_3(f) f
#define CB_ARG_PASS_12(f) f
#define CB_ARG_PASS_13(f) f
#define CB_ARG_HIDE_0(f) f
#define CB_ARG_HIDE_01(f) f
#define CB_ARG_HIDE_1(f) f
#define CB_ARG_HIDE_2(f) f
#define BIND_1(f, c) { f, (void *)c }
#define BIND_0(f) { f, XtNIL }
#define HIDE_0(f) { f, XtNIL }
#define HIDE_1(f) { f, XtNIL }
#define HIDE_0_BIND_1(f, c) { f, (void *)c }

#define CB_ARG_LIST_TOGGLE(w, call_data) CB_ARG_LIST_13(w, call_data)
#define CB_ARG_LIST_TOGGLE_X(w, client_data, call_data) CB_ARG_LIST_123(w, client_data, call_data)

#define M_ARGS_1(w) w
#define M_ARGS_2(w, a) w, a
#define M_ARGS_3(w, a, b) w, a, b
#define M_ARGS_4(w, a, b, c) w, a, b, c

#define BUTTON_CB_RETURN_TYPE void
#define DELETE_CB_RETURN_TYPE void

#define XtCastShell(w) (w)
#define XmCastDialogShell(w) (w)

#define NM(a,b) a
#define IM(a,b) a
#define NIM(a,b,c) a

typedef Pixel ImageColor;

typedef Widget Glyph_T;

#else // NOT IF_MOTIF

typedef double DimType;

#include <glibmm/listhandle.h>
#include <glibmm/main.h>
#include <gtkmm/widget.h>
#include <libxml/tree.h>
#include <ext/hash_map>

// Macros

// Force compilation of advanced stuff so we can try to
// match it.
#define XmVersion 5000

// End macros

// Dialogs

#include <gtkmm/dialog.h>

typedef Gtk::Dialog *DIALOG_P;

// End dialogs

// Menus

#include <gtkmm/menushell.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/comboboxentrytext.h>

typedef Gtk::MenuShell *MENUSHELL_P;
typedef Gtk::Menu *MENU_P;
typedef Gtk::MenuBar *MENUBAR_P;
typedef Gtk::ComboBoxEntryText *COMBOBOXENTRYTEXT_P;

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

typedef Glib::RefPtr<Gdk::Display> DISPLAY_P;
typedef Glib::RefPtr<Gdk::Screen> SCREEN_P;

#define NO_DISPLAY DISPLAY_P()
#define NO_SCREEN SCREEN_P()

#define XFlush(d) ((d)->flush())

typedef Glib::RefPtr<Gdk::GC> GC;

#define NO_GC GC()

typedef Gdk::Cursor *Cursor;

typedef unsigned long Pixel;
typedef Gdk::Color XColor;

typedef Gdk::Color ImageColor;

struct error_struct {
    int foo;
};
typedef error_struct XWindowAttributes;

typedef Glib::RefPtr<Gdk::Colormap> Colormap;

typedef Glib::RefPtr<Gdk::Pixmap> Pixmap;
typedef Glib::RefPtr<Gdk::Pixbuf> XIMAGE_P;

typedef Glib::RefPtr<Gdk::Window> Window;
typedef Glib::RefPtr<Gdk::Drawable> Drawable;

#define NO_WINDOW Window()

#define RootWindowOfScreen(screen) (screen)->get_root_window()

typedef Gdk::Region *Region;

// End gdk

// Gdk events

#define StructureNotifyMask Gdk::STRUCTURE_MASK
#define VisibilityChangeMask Gdk::VISIBILITY_NOTIFY_MASK

typedef GdkEvent XEvent;
typedef GdkEventButton XButtonEvent;

#define KeyPress GDK_KEY_PRESS
#define KeyRelease GDK_KEY_RELEASE
#define ButtonPress GDK_BUTTON_PRESS
#define ButtonRelease GDK_BUTTON_RELEASE
#define MotionNotify GDK_MOTION_NOTIFY
#define EnterNotify GDK_ENTER_NOTIFY
#define LeaveNotify GDK_LEAVE_NOTIFY
#define Expose GDK_EXPOSE
#ifdef NAG_ME
#warning FIXME Some X events have no GDK counterpart
#endif
#define GraphicsExpose GDK_NOTHING
#define CreateNotify GDK_NOTHING
#define ReparentNotify GDK_NOTHING
#define ConfigureNotify GDK_CONFIGURE
#define GravityNotify GDK_NOTHING
#define ConfigureRequest GDK_NOTHING
#define PropertyNotify GDK_PROPERTY_NOTIFY
#define SelectionClear GDK_SELECTION_CLEAR
#define SelectionRequest GDK_SELECTION_REQUEST
#define SelectionNotify GDK_SELECTION_NOTIFY

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

typedef Glib::RefPtr<Pango::Font> FONT_P;
#define NO_FONT FONT_P()

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

// Convert Glib::ustring to std::string

#define CHARSTR(s) string(s.c_str())

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

#if 0
typedef struct {
    String	name;
    XtArgVal	value;
} Arg, *ArgList;

#define XmNorientation String("orientation")

extern void XtSetArg(Arg &, const String &, long);
#endif

typedef int Position;

// Application context

typedef Gtk::Window *XtAppContext;    // The top-level window
#define XtWidgetToApplicationContext(w) 0

// End application context

// Main loop

typedef sigc::connection XtInputId;
typedef sigc::connection XtIntervalId;
typedef sigc::connection XtWorkProcId;
#define NO_CONNECTION sigc::connection()
#define NO_SOURCE sigc::connection()
#define NO_WORK sigc::connection()
#define NO_TIMER sigc::connection()

#define MAYBE_FALSE false
#define MAYBE_TRUE true

#define NO_UPDATE true

// XtWorkProc replaced by idle handler (signal_idle())
#define WP_RETURN_TYPE bool
#define WP_ALIST_NULL
#define WP_ALIST_1(d) d
#define WP_ARGS_NULL
#define IDLE_STOP false
#define IDLE_CONT true

#define XtInputWriteMask Glib::IO_OUT
#define XtInputReadMask Glib::IO_IN
#define XtInputExceptMask (Glib::IO_PRI|Glib::IO_ERR)

#define TIMEOUT_RETURN_TYPE bool
#define TIMEOUT_ARG_LIST(p,id)
#define TM_ALIST_NULL
#define TM_ALIST_1(p) p

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

// Standard argument lists corresponding to XtCallbacks.
#define CB_ARG_LIST_NULL
#define CB_ARG_LIST_1(w) Widget w
#define CB_ARG_LIST_2(client) XtPointer client
#define CB_ARG_LIST_3(call) XtPointer call
#define CB_ARG_LIST_12(w, client) Widget w, XtPointer client
#define CB_ARG_LIST_13(w, call) Widget w, XtPointer call
#define CB_ARG_LIST_23(client, call) XtPointer client, XtPointer call
#define CB_ARG_LIST_123(w, client, call) Widget w, XtPointer client, XtPointer call
// The CB_ALIST_* macros are used to specify callback argument
// lists. Motif CB arguments have a standard format: (Widget,
// XtPointer, XtPointer) but not all slots are used. These macros
// specify only those slots which are actually used. The Motif
// variants fill in dummy arguments in the other slots.
#define CB_ALIST_NULL
#define CB_ALIST_1(w) w
#define CB_ALIST_2(client) client
#define CB_ALIST_3(call) call
#define CB_ALIST_12(w, client) w, client
#define CB_ALIST_13(w, call) w, call
#define CB_ALIST_23(client, call) client, call
#define CB_ALIST_123(w, client, call) w, client, call
// This indicates a data type for use in a callback argument list.
// In Motif these are all replaced by XtPointer.
#define XtP(t) t
// Retype a functor
#define CB_ARGS_NULL
#define CB_ARGS_1(w) w
#define CB_ARGS_2(client) client
#define CB_ARGS_3(call) call
#define CB_ARGS_12(w, client) w, client
#define CB_ARGS_13(w, call) w, call
#define CB_ARGS_23(client, call) client, call
#define CB_ARGS_123(w, client, call) w, client, call
// These work on the assumption the f has 3 args (w,client_data,call_data).
#define CB_ARG_PASS_NULL(f) sigc::hide(sigc::hide(sigc::hide(f)))
#define CB_ARG_PASS_1(f) sigc::hide(sigc::hide(f))
#define CB_ARG_PASS_2(f) sigc::hide(sigc::hide<0>(f))
#define CB_ARG_PASS_3(f) sigc::hide<0>(sigc::hide<0>(f))
#define CB_ARG_PASS_12(f) sigc::hide(f)
#define CB_ARG_PASS_13(f) sigc::hide<1>(f)
#define CB_ARG_HIDE_0(f) sigc::hide<0>(f)
#define CB_ARG_HIDE_01(f) sigc::hide<0>(sigc::hide<0>(f))
#define CB_ARG_HIDE_1(f) sigc::hide<1>(f)
#define CB_ARG_HIDE_2(f) sigc::hide<2>(f)
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
#define CB_ARG_LIST_TOGGLE(w, call_data) TOGGLEBUTTON_P w
// A toggle with an additional client_data argument.
#define CB_ARG_LIST_TOGGLE_X(w, client_data, call_data) \
TOGGLEBUTTON_P w, XtPointer client_data

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

extern Glib::RefPtr<Gdk::Window> XtWindow(Widget w);
extern Glib::RefPtr<Gdk::Display> XtDisplay(Widget w);
extern Glib::RefPtr<Gdk::Screen> XtScreen(Widget w);

extern bool XtIsWidget(Widget w);

#define XtIsManaged(w) (w)->is_visible()
extern Boolean XtIsRealized(Widget w);

#define XtParent(w) (w)->get_parent()

#define XtSetSensitive(w, s) (w)->set_sensitive(s)

typedef Glib::ustring WidgetNameType;

typedef Glib::ListHandle<Gtk::Widget*> WidgetList;
typedef Glib::ListHandle<Gtk::Widget*>::iterator WidgetListIterator;

typedef Gtk::Container *CONTAINER_P;

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

typedef Gtk::Window *WINDOW_P;

extern bool XtIsShell(Widget w);
extern bool XtIsTopLevelShell(Widget w);
extern bool XtIsWMShell(Widget w);
extern bool XmIsDialogShell(Widget w);

#define XtCastShell(w) dynamic_cast<Gtk::Window *>(w)
#define XmCastDialogShell(w) dynamic_cast<Gtk::Dialog *>(w)

// End windows

// Boxes

#include <gtkmm/box.h>

typedef Gtk::Box *BOX_P;

// End boxes

// Tables

#include <gtkmm/table.h>

typedef Gtk::Table *TABLE_P;

// End tables

// Notebookes

#include <gtkmm/notebook.h>

typedef Gtk::Notebook *NOTEBOOK_P;

// End boxes

// Buttons

#include <gtkmm/togglebutton.h>
#include <gtkmm/radiobutton.h>

typedef Gtk::ToggleButton *TOGGLEBUTTON_P;
typedef Gtk::RadioButton *RADIOBUTTON_P;
typedef Gtk::Button *BUTTON_P;

#define XmToggleButtonGetState(b) (b)->get_active()
#define XmToggleButtonSetState(b, s, n) (b)->set_active(s)
#define XmIsToggleButton(w) (dynamic_cast<Gtk::ToggleButton *>(w) != NULL)

#ifdef NAG_ME
#warning Convenience function to cast Widget to Button and set label.
#endif
void set_button_label(Gtk::Widget *, const Glib::ustring &);

// End Buttons

// "Fixed" widget

#include <gtkmm/fixed.h>

typedef Gtk::Fixed *FIXED_P;

// End "Fixed" widget

// Scale widgets
#include <gtkmm/range.h>
#include <gtkmm/scale.h>

typedef Gtk::Scale *SCALE_P;
typedef Gtk::Range *RANGE_P;

// Label widget

#include <gtkmm/label.h>

typedef Gtk::Label *LABEL_P;

#define XmIsLabel(w) (dynamic_cast<Gtk::Label *>(w) != NULL)

// End label widget

// Convenience functions

// Copy text from any widget to default clipboard.
// Glib::RefPtr<Gtk::Clipboard> clip =
//   Gtk::Clipboard::get_for_display(XtDisplay(w));
bool text_copy_from(Widget w);
bool text_cut_from(Widget w);

// End convenience functions

// Entry widget

#include <gtkmm/entry.h>

typedef Gtk::Entry *ENTRY_P;

#define XmTextFieldSetString(w, s) (w)->set_text(s)

// End entry widget

// Toolbar widget

#include <gtkmm/toolbar.h>

typedef Gtk::Toolbar *TOOLBAR_P;

// End Toolbar widget

// Statusbar widget

#include <gtkmm/statusbar.h>

typedef Gtk::Statusbar *STATUSBAR_P;

// End Statusbar widget

// SpinButton widget

#include <gtkmm/spinbutton.h>

typedef Gtk::SpinButton *SPINBUTTON_P;

// End SpinButton widget

// FileChooserDialog widget

#include <gtkmm/filechooserdialog.h>

typedef Gtk::FileChooserDialog *FILECHOOSERDIALOG_P;

// End FileChooserDialog widget

// TreeView widget

#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>

typedef Gtk::TreeView *TREEVIEW_P;
typedef Gtk::TreeModel *TREEMODEL_P;

// Get the selected item positions
#ifdef NAG_ME
#warning FIXME: Quick&Dirty - For compatibility only.
#warning Defined in file.C.
#endif
extern int list_get_positions(TREEVIEW_P selectionList, int *&positions, int &n_positions);

// End TreeView widget

// ScrolledWindow widget

#include <gtkmm/scrolledwindow.h>

typedef Gtk::ScrolledWindow *SCROLLEDWINDOW_P;

// End ScrolledWindow widget

// Gtk Scrolled Text

#include "GtkScrolledText.h"

typedef GtkScrolledText *SCROLLEDTEXT_P;
typedef Gtk::TextView *TEXTVIEW_P;

#define XmTextSetEditable(w, b) (w)->set_editable(b)

// End Gtk Scrolled Text

#define XtSetLanguageProc(a, b, c)

// Databases

typedef Glib::ValueBase XrmValue;

#if 0
typedef  __gnu_cxx::hash_map<const char *, XrmValue *> *XrmDatabase;
#else
typedef xmlDoc *XrmDatabase;
#endif

typedef struct _XtResource {
    String	resource_name;	/* Resource name			    */
    String	resource_class;	/* Resource class			    */
    String	resource_type;	/* Representation type desired		    */
    Cardinal	resource_size;	/* Size in bytes of representation	    */
    Cardinal	resource_offset;/* Offset from base to put resource value   */
    String	default_type;	/* representation type of specified default */
    XtPointer	default_addr;	/* Address of default resource		    */
} XtResource, *XtResourceList;

extern XrmDatabase get_string_database(const char *s);
void merge_databases(XrmDatabase source_db, XrmDatabase target_db);
bool get_resource(XrmDatabase database, const char *str_name, const char *str_class,
		  XrmValue &value_return);
bool put_resource(XrmDatabase database, const char *str_name, const char *str_class,
		  XrmValue &value);
Gtk::Window *find_toplevel(Gtk::Widget *w);
#define XrmGetFileDatabase(f) get_file_database(f)
extern XrmDatabase get_file_database(const char *f);

#define XtRBitmap "Bitmap"
#define XtRBool "Bool"
#define XtRBoolean "Boolean"
#define XtRCallback "Callback"
#define XtRCallProc "CallProc"
#define XtRCardinal "Cardinal"
#define XtRColor "Color"
#define XtRColormap "Colormap"
#define XtRCursor "Cursor"
#define XtRDimension "Dimension"
#define XtRDisplay "Display"
#define XtREditMode "EditMode"
#define XtREnum "Enum"
#define XtRFile "File"
#define XtRFloat "Float"
#define XtRFont "Font"
#define XtRFontStruct "FontStruct"
#define XtRFunction "Function"
#define XtRGeometry "Geometry"
#define XtRImmediate "Immediate"
#define XtRInitialState "InitialState"
#define XtRInt "Int"
#define XtRJustify "Justify"
#define XtRLongBoolean "Bool"
#define XtRObject "Object"
#define XtROrientation "Orientation"
#define XtRPixel "Pixel"
#define XtRPixmap "Pixmap"
#define XtRPointer "Pointer"
#define XtRPosition "Position"
#define XtRScreen "Screen"
#define XtRShort "Short"
#define XtRString "String"
#define XtRStringArray "StringArray"
#define XtRStringTable "StringTable"
#define XtRUnsignedChar "UnsignedChar"
#define XtRTranslationTable "TranslationTable"
#define XtRVisual "Visual"
#define XtRWidget "Widget"
#define XtRWidgetClass "WidgetClass"
#define XtRWidgetList "WidgetList"
#define XtRWindow "Window"

// End Databases

#define NM(a,b) a, b, NULL
#define IM(a,b) a, Glib::ustring(), b
#define NIM(a,b,c) a, b, c

class GtkGlyphMark;
typedef GtkGlyphMark *Glyph_T;





// TREE MODEL FOR SIMPLE LIST OF STRINGS

#include <gtkmm/treemodelcolumn.h>

// Models for the ListStore.
template <class T>
class SimpleListColumns: public Gtk::TreeModel::ColumnRecord
{
public:
  Gtk::TreeModelColumn<T> value;
  SimpleListColumns()
  {
    add(value);
  }
};

#endif // IF_MOTIF

#endif // GTK_WRAPPER_H
