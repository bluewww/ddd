// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

#include <iostream>

#include <GtkX/EventType.h>
#include <GtkX/Widget.h>

using namespace GtkX;

Event::Event(void)
{
    nrefs_ = 0;
    type = NOTHING;
}

Event::~Event(void)
{
}

Event *EventAny::clone() const
{
    return new EventAny(*this);
}

Event *EventExpose::clone() const
{
    return new EventExpose(*this);
}

Event *EventNoExpose::clone() const
{
    return new EventNoExpose(*this);
}

Event *EventVisibility::clone() const
{
    return new EventVisibility(*this);
}

Event *EventMotion::clone() const
{
    return new EventMotion(*this);
}

Event *EventButton::clone() const
{
    return new EventButton(*this);
}

Event *EventScroll::clone() const
{
    return new EventScroll(*this);
}

Event *EventKey::clone() const
{
    return new EventKey(*this);
}

Event *EventCrossing::clone() const
{
    return new EventCrossing(*this);
}

Event *EventFocus::clone() const
{
    return new EventFocus(*this);
}

Event *EventConfigure::clone() const
{
    return new EventConfigure(*this);
}

Event *EventProperty::clone() const
{
    return new EventProperty(*this);
}

Event *EventSelection::clone() const
{
    return new EventSelection(*this);
}

Event *EventOwnerChange::clone() const
{
    return new EventOwnerChange(*this);
}

Event *EventProximity::clone() const
{
    return new EventProximity(*this);
}

Event *EventClient::clone() const
{
    return new EventClient(*this);
}

Event *EventSetting::clone() const
{
    return new EventSetting(*this);
}

Event *EventWindowState::clone() const
{
    return new EventWindowState(*this);
}

Event *EventGrabBroken::clone() const
{
    return new EventGrabBroken(*this);
}

