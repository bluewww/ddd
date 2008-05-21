#ifndef GTK_WRAPPER_H
#define GTK_WRAPPER_H

#if defined(IF_MOTIF)

#include <X11/Intrinsic.h>



// ****************************************************************************

#else // NOT IF_MOTIF

#include <GUI/Widget.h>
#include <gtkmm/widget.h>



typedef Gtk::Widget *Widget;

typedef bool Boolean;
typedef bool Bool;

#define False false
#define True true

#endif // IF_MOTIF

#endif // GTK_WRAPPER_H
