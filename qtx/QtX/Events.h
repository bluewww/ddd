// -*- C++ -*-

#ifndef QTX_EVENTS_H
#define QTX_EVENTS_H

#if 0
#include <gdk/gdkevents.h>
#include <gdkmm/window.h>
#endif

namespace QtX {

#include <QtX/EventType.h>

#if 0
    int translate_event(GdkEvent *in, Event *out);
    int translate_event_mask(EventMask &in, Gdk::EventMask &out);
    int translate_mod_mask(GdkModifierType &in, ModifierType &out);
#endif

}

#endif /* QTX_EVENTS_H */
