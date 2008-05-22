// High-level GUI wrapper for Motif.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <Xmmm/ListView.h>
#include <Xm/DialogS.h>
#include <Xm/RowColumn.h>
#include <string>

using namespace Xmmm;

#define LIST_CHARSET XmStringCharSet("tt")

void
ListView::init_signals(void)
{
    XtAddCallback(list_, XmNsingleSelectionCallback, 
		  (XtCallbackProc)ListView::selection_changed_callback,
		  XtPointer(this));
    XtAddCallback(list_, XmNmultipleSelectionCallback, 
		  (XtCallbackProc)ListView::selection_changed_callback,
		  XtPointer(this));
    XtAddCallback(list_, XmNextendedSelectionCallback, 
		  (XtCallbackProc)ListView::selection_changed_callback,
		  XtPointer(this));
    XtAddCallback(list_, XmNbrowseSelectionCallback, 
		  (XtCallbackProc)ListView::selection_changed_callback,
		  XtPointer(this));
}

void
ListView::init(::Widget parent, const String &name,
	       const std::vector<String> &headers)
{
    Arg args[10];
    int nargs;
    nargs = 0;
    XtSetArg(args[nargs], XmNvisibleItemCount, 12); nargs++;
    XtSetArg(args[nargs], XmNselectionPolicy, XmBROWSE_SELECT); nargs++;
    XtSetArg(args[nargs], XmNlistSizePolicy, XmCONSTANT); nargs++;
    XtSetArg(args[nargs], XmNscrollBarDisplayPolicy, XmAS_NEEDED); nargs++;
    list_ = XmCreateList(parent, (char *)name.c(), args, nargs);
    init_signals();
}

ListView::ListView(Container &parent, const String &name,
		   const std::vector<String> &headers)
{
    init(parent.xt_container(), name, headers);
    postinit();
}

ListView::ListView(::Widget parent, const String &name,
		   const std::vector<String> &headers)
{
    init(parent, name, headers);
    postinit();
}

ListView::~ListView(void)
{
    if (list_) XtDestroyWidget(list_);
}

::Widget ListView::internal(void)
{
    return list_;
}

std::string
ListView::get_selected(void)
{
    XmStringTable mitems;
    int n;
    XtVaGetValues(list_,
		  XmNselectedItems, &mitems,
		  XmNselectedItemCount, &n,
		  XtPointer(0));
    if (n <= 0) return std::string("");
    ::String s;
    XmStringGetLtoR(mitems[0], LIST_CHARSET, &s);
    std::string result(s);
    XtFree(s);
    return result;
}

void
ListView::clear(void)
{
    XtVaSetValues(list_,
		  XmNitems, NULL,
		  XmNitemCount, 0,
		  XtPointer(0));
}

void
ListView::append(const String &item)
{
    XmStringTable mitems;
    int n;
    XtVaGetValues(list_,
		  XmNitems, &mitems,
		  XmNitemCount, &n,
		  XtPointer(0));
    XmStringTable mitems2 = XmStringTable(XtMalloc((n+1)*sizeof(XmString)));
    for (int i = 0; i < n; i++) {
	mitems2[i] = XmStringCopy(mitems[i]);
    }
    mitems2[n] = XmStringCreateLtoR((char *)item.c(), LIST_CHARSET);
    XtVaSetValues(list_,
		  XmNitems, mitems2,
		  XmNitemCount, n+1,
		  XtPointer(0));
}

void
ListView::selection_changed_callback(::Widget widget, XtPointer data)
{
    ((ListView *)data)->signal_selection_changed_();
}

sigc::signal<void> &
ListView::signal_selection_changed(void)
{
    return signal_selection_changed_;
}

int
ListView::get_selected_pos(void)
{
    int *pos;
    int n;
    XtVaGetValues(list_,
		  XmNselectedPositionCount, &n,
		  XmNselectedPositions, &pos,
		  XtPointer(0));
    if (n <= 0) return -1;
    return pos[0];
}    int *pos;


int
ListView::count(void) const
{
    int n;
    XtVaGetValues(list_,
		  XmNitemCount, &n,
		  XtPointer(0));
    return n;
}

