// -*- C++ -*-

#ifndef GTKX_EVENTS_H
#define GTKX_EVENTS_H

#include <gdk/gdkevents.h>
#include <gdkmm/window.h>

namespace GtkX {

#include <GtkX/EventType.h>

    int translate_event(GdkEvent *in, Event *out);
    int translate_event_mask(EventMask &in, Gdk::EventMask &out);
    int translate_mod_mask(GdkModifierType &in, ModifierType &out);

}

#endif /* GTKX_EVENTS_H */
