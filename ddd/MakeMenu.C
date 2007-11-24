// $Id$
// Create custom Motif Menus

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char MakeMenu_rcsid[] = 
    "$Id$";

#include "config.h"

#include "MakeMenu.h"

#include "assert.h"
#include "strclass.h"
#include "MString.h"
#include "TimeOut.h"
#include "misc.h"
#include "string-fun.h"
#include "charsets.h"
#include "wm.h"

#include <stdlib.h>
#if defined(IF_MOTIF)
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/ArrowB.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MenuShell.h>
#include <X11/Xutil.h>

#include "LessTifH.h"
#else
#include "gtk_wrapper.h"
#include <gtkmm/optionmenu.h>
#include <gtkmm/box.h>
#include <gtkmm/scale.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/comboboxentrytext.h>
#include <gtkmm/menubar.h>
#include <gtkmm/image.h>
#endif

#if !defined(IF_XM)
#include <GUI/Button.h>
#include <GUI/SpinButton.h>
#include <GUI/MenuItem.h>
#include <GUI/CheckMenuItem.h>
#include <GUI/CheckButton.h>
#include <GUI/RadioButton.h>
#include <GUI/Label.h>
#include <GUI/OptionMenu.h>
#include <GUI/Box.h>
#include <GUI/Scale.h>
#include <GUI/Entry.h>
#include <GUI/ComboBox.h>
#include <GUI/SeparatorMenuItem.h>
#include <GUI/MenuBar.h>
#endif

#include "bool.h"
#include "verify.h"
#include "findParent.h"
#include "frame.h"
#if defined(IF_MOTIF)
#include "ComboBox.h"
#include "SpinBox.h"
#endif
#include "AutoRaise.h"

#ifndef LOG_FLATTENING
#define LOG_FLATTENING 0
#endif

#ifndef LOG_PUSH_MENUS
#define LOG_PUSH_MENUS 0
#endif

#if defined(IF_MOTIF)
// Pushmenu callbacks
static void ArmPushMenuCB(Widget, XtPointer, XtPointer);
static void RedrawPushMenuCB(Widget, XtPointer, XtPointer);
static void PopupPushMenuAct(Widget w, XEvent* e, String *, Cardinal *);
static void DecoratePushMenuAct(Widget w, XEvent* e, String *, Cardinal *);

static XtActionsRec actions [] = {
    {XTARECSTR("popup-push-menu"),            PopupPushMenuAct },
    {XTARECSTR("decorate-push-menu"),         DecoratePushMenuAct },
};

static const char *pushMenuTranslations = 
    "<Expose>:          decorate-push-menu()\n"
;

static const char *lesstif_pushMenuTranslations = 
    "None<Btn3Down>:	popup-push-menu()\n"
;

#else
#ifdef NAG_ME
#warning No PushMenu handling
#endif
#endif

struct PushMenuInfo {
    Widget widget;		// The PushButton
    Widget subMenu;		// Submenu of this PushButton
    bool flat;			// Whether the PushButton is flattened
    XtIntervalId timer;		// Timer while waiting

    PushMenuInfo(Widget w, Widget s, bool f)
	: widget(w), subMenu(s), flat(f)
#if defined(IF_MOTIF)
	, timer(0)
#endif
    {}

private:
    PushMenuInfo(const PushMenuInfo&);
    PushMenuInfo& operator= (const PushMenuInfo&);
};

#if defined(IF_MOTIF)
//-----------------------------------------------------------------------
// Flat buttons
//-----------------------------------------------------------------------

// The currently unflattened button
static Widget active_button = 0;

static void flatten_button(Widget w, bool switch_colors = true)
{
    Pixel background;
    Pixmap highlight_pixmap, label_pixmap;
    Pixmap bottom_shadow_pixmap;
    XtVaGetValues(w,
		  XmNbackground,         &background,
		  XmNlabelPixmap,        &label_pixmap,
		  XmNhighlightPixmap,    &highlight_pixmap,
		  XmNbottomShadowPixmap, &bottom_shadow_pixmap,
		  XtPointer(0));

    if (bottom_shadow_pixmap == XmUNSPECIFIED_PIXMAP)
    {
#if LOG_FLATTENING
	std::clog << "Flattening " << XtName(w) << "\n";
#endif

	Arg args[10];
	Cardinal arg = 0;

	Pixmap empty = XmGetPixmap(XtScreen(w), XMST("background"), 
				   background, background);

	XtSetArg(args[arg], XmNbottomShadowPixmap, empty); arg++;
	XtSetArg(args[arg], XmNtopShadowPixmap,    empty); arg++;

	if (switch_colors)
	{
	    XtSetArg(args[arg], XmNlabelPixmap, highlight_pixmap); arg++;
	    XtSetArg(args[arg], XmNhighlightPixmap, label_pixmap); arg++;
	}

	XtSetValues(w, args, arg);
    }
}

static void unflatten_button(Widget w, bool switch_colors = true)
{
    Pixel background;
    Pixmap highlight_pixmap, label_pixmap;
    Pixmap bottom_shadow_pixmap;
    XtVaGetValues(w,
		  XmNbackground,         &background,
		  XmNlabelPixmap,        &label_pixmap,
		  XmNhighlightPixmap,    &highlight_pixmap,
		  XmNbottomShadowPixmap, &bottom_shadow_pixmap,
		  XtPointer(0));

    if (bottom_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
    {
#if LOG_FLATTENING
	std::clog << "Unflattening " << XtName(w) << "\n";
#endif

	Arg args[10];
	Cardinal arg = 0;

	XtSetArg(args[arg], XmNbottomShadowPixmap, 
		 XmUNSPECIFIED_PIXMAP); arg++;
	XtSetArg(args[arg], XmNtopShadowPixmap,
		 XmUNSPECIFIED_PIXMAP); arg++;

	if (switch_colors)
	{
	    XtSetArg(args[arg], XmNlabelPixmap, highlight_pixmap); arg++;
	    XtSetArg(args[arg], XmNhighlightPixmap, label_pixmap); arg++;
	}

	XtSetValues(w, args, arg);
    }
}

static void FlattenEH(Widget w,
		      XtPointer /* client_data */,
		      XEvent *event, 
		      Boolean * /* continue_to_dispatch */)
{
    if (event->xcrossing.state & (Button1Mask | Button2Mask | Button3Mask | 
				  Button4Mask | Button5Mask))
	return;			// Button is still pressed

    switch (event->type)
    {
    case EnterNotify:
    {
#if LOG_FLATTENING
	std::clog << "Entering " << XtName(w) << "\n";
#endif

	unflatten_button(w);
	active_button = w;
	break;
    }

    case LeaveNotify:
    {
#if LOG_FLATTENING
	std::clog << "Leaving " << XtName(w) << "\n";
#endif

	flatten_button(w);
	active_button = 0;
	break;
    }
    }
}

// Handle Arm() and Disarm() actions
static void FlattenCB(Widget w, XtPointer client_data, XtPointer)
{
    if (w == active_button)
    {
	// We have already entered it -- don't interfere
	return;
    }

    bool set = bool(client_data);
    if (set)
    {
	// std::clog << "Arming " << XtName(w) << "\n";

	flatten_button(w, false);
    }
    else
    {
	// std::clog << "Disarming " << XtName(w) << "\n";

	unflatten_button(w, false);
    }
}

static void ReflattenButtonCB(Widget /* shell */, XtPointer client_data, 
			      XtPointer = 0)
{
    Widget w = (Widget)client_data;
    EventMask event_mask = EnterWindowMask | LeaveWindowMask;
    XtAddEventHandler(w, event_mask, False, FlattenEH, XtPointer(0));
    XtAddCallback(w, XmNarmCallback,    FlattenCB, XtPointer(False));
    XtAddCallback(w, XmNdisarmCallback, FlattenCB, XtPointer(True));
    flatten_button(w);
}
#else

#ifdef NAG_ME
#warning Button flattening?
#endif

#endif

#if !defined(IF_MOTIF)

#include <gtkmm/eventbox.h>

static void
pack_item(GUI::Container *container, GUI::Widget *widget)
{
    std::cerr << "PACK_ITEM NOT SUPPORTED FOR Gtk\n";
#if 0
    GUI::Box *cont_box = dynamic_cast<GUI::Box *>(container);

    GUI::Label *label = dynamic_cast<GUI::Label *>(widget);
#ifdef __GNUC__
#warning Why use the EventBox?
#endif
    if (label) {
	GUI::EventBox *box = new GUI::EventBox();
	box->modify_bg(GUI::STATE_NORMAL, Gdk::Color("antique white"));
	box->add(*widget);
	box->show();
	widget = box;
    }

    if (cont_box)
	cont_box->pack_start(*widget, GUI::PACK_SHRINK);
    else
	container->add(*widget);
#endif
}

#endif

#if defined(IF_XMMM)

static void
pack_item(GUI::Container *container, GUI::Widget *widget)
{
    std::cerr << "PACK_ITEM NOT SUPPORTED FOR XMMM\n";
}

#endif

//-----------------------------------------------------------------------
// Add items
//-----------------------------------------------------------------------

// Add items to shell.  If IGNORE_SEPS is set, all separators are ignored.
void MMaddItems(CONTAINER_P shell,
#if !defined(IF_XM)
		GUI::Container *xshell,
#endif
		MMDesc items[], bool ignore_seps)
{
#if defined(IF_MOTIF)
    static bool actions_added = false;
		
    if (!actions_added)
    {
	XtAppAddActions(XtWidgetToApplicationContext(shell), 
			actions, XtNumber(actions));
	actions_added = true;
    }
#else
#ifdef NAG_ME
#warning No analogue for "actions[]"?
#endif
#endif

#if defined(IF_MOTIF)
    Arg args[10];
    int arg;

    Widget container = shell;
#if !defined(IF_XM)
    bool menushell = false;
    bool image = false;
#endif
#else
    Gtk::MenuShell *menushell = dynamic_cast<Gtk::MenuShell *>(shell);
    Gtk::Container *container = dynamic_cast<Gtk::Container *>(shell);
    Gtk::RadioButtonGroup group;
#endif

    static const string textName  = "text";
    static const string labelName = "label";
    // Create lots of buttons...
    for (MMDesc *item = items; item && item->name; item++)
    {
#if defined(IF_XM)
	const char * const name = item->name;
#else
	cpString name = item->name;
#if !defined(IF_MOTIF)
	cpString label_string = item->label_string;
	XIMAGE_P *image = item->image;
#else
	cpString label_string = item->name;
	XIMAGE_P *image = item->image;
#endif
#endif
	MMType flags            = item->type;
	MMType type             = flags & MMTypeMask;
	Widget& widget          = item->widget;
	Widget *widgetptr       = item->widgetptr;
	MMDesc *subitems        = item->items;
	LABEL_P& label          = item->label;
#if !defined(IF_XM)
	GUI::Widget *&xwidget = item->xwidget;
	GUI::Widget **xwidgetptr = item->xwidgetptr;
	GUI::Label *&xlabel = item->xlabel;
#endif

	if (flags & MMIgnore)
	    continue;		// Don't create

	const string subMenuName = string(name) + "Menu";
	MENU_P subMenu = NULL;
#if defined(IF_XM)
	BOX_P box = NULL;
#else
	GUI::Container *box = NULL;
#endif
	BOX_P panel   = 0;
	bool flat = false;
	label = 0;
	widget = 0;

	switch(type) 
	{
	case MMFlatPush:
	{
	    flat = true;
	    // FALL THROUGH
	}

	case MMPush:
	{
#if defined(IF_XM)
	    // Create a PushButton
	    arg = 0;
	    if (flat)
	    {
		Pixel background;
		XtVaGetValues(shell, XmNbackground, &background, XtPointer(0));
		Pixmap empty = XmGetPixmap(XtScreen(shell), 
					   XMST("background"), 
					   background, background);

		XtSetArg(args[arg], XmNbottomShadowPixmap, empty); arg++;
		XtSetArg(args[arg], XmNtopShadowPixmap,    empty); arg++;
		XtSetArg(args[arg], XmNhighlightThickness, 0);     arg++;
		XtSetArg(args[arg], XmNshadowThickness,    2);     arg++;
	    }
	    else
	    {
		XtSetArg(args[arg], 
			 XmNhighlightPixmap, XmUNSPECIFIED_PIXMAP); arg++;
	    }

	    PushMenuInfo *info = 0;
	    if (lesstif_version <= 84)
	    {
		// LessTif 0.84 and earlier wants the PushButton as
		// parent of the menu
		widget = verify(XmCreatePushButton(shell, XMST(name), args, arg));

		if (subitems != 0)
		{
		    subMenu = MMcreatePushMenu(widget, subMenuName.chars(), subitems);
		    info = new PushMenuInfo(widget, subMenu, flat);
		    XtVaSetValues(widget,
				  XmNuserData, XtPointer(info),
				  XtPointer(0));
		}
	    }
	    else
	    {
		// Motif wants the shell as parent of the menu
		if (subitems != 0)
		{
		    subMenu = MMcreatePushMenu(shell, subMenuName.chars(), subitems);
		    info = new PushMenuInfo(0, subMenu, flat);
		    XtSetArg(args[arg], XmNuserData, XtPointer(info)); arg++;
		}

		widget = verify(XmCreatePushButton(shell, XMST(name), args, arg));

		if (info != 0)
		    info->widget = widget;
	    }
#else
#ifdef NAG_ME
#warning MMPush might be a MenuItem or a Button.  Check shell.
#endif
	    if (menushell) {
		xwidget = new GUI::MenuItem(container, label_string);
	    }
	    else {
		if (image) {
		    GUI::Button *button = new GUI::Button(container);
		    xwidget = button;
		    XIMAGE_P p1 = image[0];
		    XIMAGE_P p2 = image[1];
		    XIMAGE_P p3 = image[2];
		    XIMAGE_P p4 = image[3];
		    if (p1)
		    {
			Gtk::Image *im = new Gtk::Image(p1);
#ifdef NAG_ME
#warning This needs to be abstracted.
#endif
			Gtk::Button *b = (Gtk::Button *)button->internal();
			b->set_image(*im);
			b->set_label("");
			im->show();
		    }
		}
		else {
		    xwidget = new GUI::Button(container, label_string);
		}
	    }
	    // pack_item(container, widget);

	    if (subitems != 0)
	    {
		subMenu = MMcreatePushMenu(shell, subMenuName.chars(), subitems);
		PushMenuInfo *info = new PushMenuInfo(0, subMenu, flat);
		info->widget = widget;
		xwidget->internal()->property_user_data() = info;
	    }

#endif
	    break;
	}

	case MMToggle:
	{
	    // Create a ToggleButton
	    assert(subitems == 0);

#if defined(IF_MOTIF)
	    arg = 0;
	    widget = verify(XmCreateToggleButton(shell, XMST(name), args, arg));
#else
	    if (menushell) {
		std::cerr << "*** ERROR: CREATE MMToggle IN MENU ***\n";
		xwidget = new GUI::CheckMenuItem(container, label_string);
	    }
	    else
		xwidget = new GUI::CheckButton(container, label_string);
#endif
	    break;
	}

#if !defined(IF_MOTIF)
	case MMCheckItem:
	{
	    // Create a CheckItem
	    assert(subitems == 0);

	    xwidget = new GUI::CheckMenuItem(container, label_string);
	    break;
	}
#endif

#if !defined(IF_MOTIF)
	case MMRadio:
	{
	    // Create a ToggleButton in a radio group
	    assert(subitems == 0);
	    // FIXME: Each container has its own group?
	    xwidget = new GUI::RadioButton(container, /* group, */ label_string);
	    break;
	}
#endif

	case MMLabel:
	{
	    // Create a Label
	    assert(subitems == 0);

#if defined(IF_MOTIF)
	    arg = 0;
	    widget = verify(XmCreateLabel(shell, XMST(name), args, arg));
#else
	    widget = label = new GUI::Label(container, label_string);
	    widget->modify_fg(Gtk::STATE_NORMAL, Gdk::Color("red"));
	    label->set_alignment(Gtk::ALIGN_LEFT);
#endif
	    break;
	}

	case MMArrow:
	{
	    // Create an arrow
	    assert(subitems == 0);

#if defined(IF_MOTIF)
	    arg = 0;
	    widget = verify(XmCreateArrowButton(shell, XMST(name), args, arg));
#else
#ifdef NAG_ME
#warning What is an ArrowButton?
#endif
	    xwidget = new GUI::Button(container, label_string);
	    xwidget->set_name(name);
#endif
	    break;
	}

	case MMMenu:
	{
	    // Create a CascadeButton and a new PulldownMenu
	    assert(subitems != 0);

	    subMenu = MMcreatePulldownMenu(container, subMenuName.chars(), subitems);
#if defined(IF_MOTIF)
	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = verify(XmCreateCascadeButton(shell, XMST(name), args, arg));

            if (lesstif_version <= 79)
	    {
		// LessTif 0.79 and earlier has a very tight packing
		// of menu items; place a few spaces around the labels
		// to increase item distance.
		XmString old_label;
		XtVaGetValues(widget, 
			      XmNlabelString, &old_label,
			      XtPointer(0));
		MString new_label(old_label, true);
		XmStringFree(old_label);

		if (!new_label.isNull())
		{
		    new_label = MString("  ") + new_label + MString("  ");
		    XtVaSetValues(widget, 
				  XmNlabelString, new_label.xmstring(), 
				  XtPointer(0));
		}

		// Same applies to accelerator texts.
		XmString old_acc;
		XtVaGetValues(widget,
			      XmNacceleratorText, &old_acc,
			      XtPointer(0));
		MString new_acc(old_acc, true);
		XmStringFree(old_acc);

		if (!new_acc.isNull())
		{
		    new_acc = MString("  ") + new_acc;
		    XtVaSetValues(widget, 
				  XmNacceleratorText, new_acc.xmstring(), 
				  XtPointer(0));
		}
	    }
#else
	    if (menushell) {
		GUI::MenuItem *mi;
		xwidget = mi = new GUI::MenuItem(container, label_string);
		mi->set_submenu(*subMenu);
	    }
	    else {
		std::cerr << "Cannot attach menu to non-menushell\n";
		xwidget = new GUI::Button(container, label_string);
	    }
#endif
	    break;
	}

	case MMRadioMenu:
	{
	    // Create a CascadeButton and a new PulldownMenu
	    assert(subitems != 0);

	    subMenu = MMcreateRadioPulldownMenu(container, subMenuName.chars(), subitems);

#if defined(IF_MOTIF)
	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = verify(XmCreateCascadeButton(shell, XMST(name), args, arg));
#else
	    if (menushell) {
		GUI::MenuItem *mi;
		widget = mi = new GUI::MenuItem(container, label_string);
		mi->set_submenu(*subMenu);
	    }
	    else {
		std::cerr << "Cannot attach menu to non-menushell\n";
		xwidget = new GUI::Button(container, label_string);
	    }
#endif
	    break;
	}

	case MMOptionMenu:
	{
	    // Create an option menu
	    assert(subitems != 0);

	    subMenu = MMcreatePulldownMenu(container, subMenuName.chars(), subitems);

#if defined(IF_XM)
	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = verify(XmCreateOptionMenu(shell, XMST(name), args, arg));
#else
	    GUI::OptionMenu *om;
	    xwidget = om = new GUI::OptionMenu(container);
	    xwidget->set_name(name);
#ifdef NAG_ME
#warning OptionMenu is deprecated.
#endif
	    om->set_menu(*subMenu);
#endif
	    break;
	}

	case MMPanel:
	case MMRadioPanel:
	case MMButtonPanel:
	{
	    // Create a label with an associated panel
	    assert(subitems != 0);
	    const string panelName   = string(name) + "Panel";

	    bool have_label = 
		(name[0] != '\0' && (flags & MMUnmanagedLabel) == 0);

#if defined(IF_XM)
	    arg = 0;
	    XtSetArg(args[arg], XmNorientation, XmHORIZONTAL); arg++;
	    XtSetArg(args[arg], XmNborderWidth,     0); arg++;
	    XtSetArg(args[arg], XmNentryBorder,     0); arg++;
	    XtSetArg(args[arg], XmNspacing,         0); arg++;
	    XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
	    XtSetArg(args[arg], XmNmarginHeight,    0); arg++;
	    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;

	    widget = verify(XmCreateRowColumn(shell, XMST(panelName.chars()), args, arg));

	    arg = 0;
	    label = verify(XmCreateLabel(widget, XMST(name), args, arg));
	    if (have_label)
		XtManageChild(label);
#else
	    xwidget = box = new GUI::HBox(*xshell, panelName.chars());

	    xlabel = new GUI::Label(*box, label_string);
	    xlabel->set_alignment(Gtk::ALIGN_LEFT);
	    if (have_label)
		xlabel->show();
#endif

#if defined(IF_XM)
	    Widget (*create_panel)(Widget, const _XtString, MMDesc[], ArgList, Cardinal) = 0;
#else
	    GUI::Container *(*create_panel)(GUI::Container *, cpString, MMDesc[]) = 0;
#endif

	    switch (type)
	    {
	    case MMPanel:
		create_panel = MMcreatePanel;
		break;

	    case MMRadioPanel:
		create_panel = MMcreateRadioPanel;
		break;

	    case MMButtonPanel:
		create_panel = MMcreateButtonPanel;
		break;

	    default:
		assert(0);
		abort();
	    }

#if defined(IF_MOTIF)
	    arg = 0;
	    XtSetArg(args[arg], XmNorientation, 
		     (flags & MMVertical) ? XmVERTICAL : XmHORIZONTAL); arg++;

	    if (!have_label)
	    {
		XtSetArg(args[arg], XmNborderWidth,     0); arg++;
		XtSetArg(args[arg], XmNentryBorder,     0); arg++;
		XtSetArg(args[arg], XmNspacing,         0); arg++;
		XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
		XtSetArg(args[arg], XmNmarginHeight,    0); arg++;
		XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
	    }
#endif


#if defined(IF_MOTIF)
	    subMenu = create_panel(widget, subMenuName.chars(), subitems, args, arg);
	    XtManageChild(subMenu);
#else
	    box = create_panel(box, subMenuName.chars(), subitems);
#endif

	    break;
	}

	case MMScale:
	{
	    // Create a scale
	    assert(subitems == 0);

#if defined(IF_XM)
	    arg = 0;
	    widget = verify(XmCreateScale(shell, XMST(name), args, arg));
#else
	    xwidget = new GUI::HScale(*xshell, name);
#endif
	    break;
	}

	case MMSpinBox:
	case MMComboBox:
	case MMTextField:
	case MMEnterField:
	{
	    // Create a label with an associated text field
	    assert(subitems == 0);

#if defined(IF_XM)
	    arg = 0;
	    XtSetArg(args[arg], XmNorientation,     XmHORIZONTAL); arg++;
	    XtSetArg(args[arg], XmNborderWidth,     0); arg++;
	    XtSetArg(args[arg], XmNentryBorder,     0); arg++;
	    XtSetArg(args[arg], XmNspacing,         0); arg++;
	    XtSetArg(args[arg], XmNmarginWidth,     0); arg++;
	    XtSetArg(args[arg], XmNmarginHeight,    0); arg++;
	    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;

	    panel = verify(XmCreateRowColumn(shell, XMST(name), args, arg));
#else
	    panel = new GUI::HBox(*xshell, name);
#endif

#if defined(IF_MOTIF)
	    arg = 0;
	    label = verify(XmCreateLabel(panel, XMST(labelName.chars()), args, arg));
	    if (name[0] != '\0' && (flags & MMUnmanagedLabel) == 0)
		XtManageChild(label);
#else
	    xlabel = new GUI::Label(panel, label_string);
	    xlabel->set_alignment(Gtk::ALIGN_LEFT);
	    if (name[0] != '\0' && (flags & MMUnmanagedLabel) == 0)
		xlabel->show();
#endif

	    switch (type)
	    {
	    case MMSpinBox:
#if defined(IF_XM)
		arg = 0;
		widget = CreateSpinBox(panel, textName.chars(), args, arg);
#else
		xwidget = new GUI::SpinButton(panel, name);
#endif
		break;

	    case MMComboBox:
#if defined(IF_MOTIF)
		arg = 0;
		widget = CreateComboBox(panel, textName.chars(), args, arg);
#else
		xwidget = new GUI::ComboBoxEntryText(*xshell, GUI::String(textName.chars()));
#endif
		break;

	    case MMTextField:
	    case MMEnterField:
#if defined(IF_XM)
		arg = 0;
		widget = verify(XmCreateTextField(panel, XMST(textName.chars()), args, arg));
		XtManageChild(widget);
#else
		xwidget = new GUI::Entry(panel, GUI::String(textName.chars()));
		xwidget->show();
#endif
		break;
	    }
	    break;
	}

	case MMSeparator:
	{
	    // Create a separator
	    assert(subitems == 0);

	    if (ignore_seps)
		continue;
#if defined(IF_XM)
	    arg = 0;
	    widget = verify(XmCreateSeparator(shell, XMST(name), args, arg));
#else
	    xwidget = new GUI::SeparatorMenuItem(container, name);
#endif
	    break;
	}

	default:
	    // Invalid type
	    assert(0);
	    abort();
	}

	if (flags & MMHelp)
	{
#if defined(IF_MOTIF)
	    arg = 0;
	    XtSetArg(args[arg], XmNmenuHelpWidget, item->widget); arg++;
	    XtSetValues(shell, args, arg);
#else
#ifdef NAG_ME
#warning Set menu?
#endif
#endif
	}

	Widget panel_widget = panel?panel:widget;

	if (panel_widget) {
	    // Non-OO widget
	    if (flags & MMInsensitive)
		set_sensitive(panel_widget, false);

	    if (!(flags & MMUnmanaged))
		XtManageChild(panel_widget);

	    if (widgetptr != 0)
		*widgetptr = widget;
	}
#if !defined(IF_XM)
	if (xwidget) {
	    // OO widget
	    if (flags & MMInsensitive)
		xwidget->set_sensitive(false);

	    if (!(flags & MMUnmanaged))
		xwidget->show();

	    if (xwidgetptr != 0)
		*xwidgetptr = xwidget;
	    widget = xwidget->internal();
	    if (widgetptr != 0)
		*widgetptr = widget;
	}
#endif
    }
}

#if !defined(IF_XM)
void MMaddItems(GUI::Container *shell, MMDesc items[], bool ignore_seps)
{
#if defined(IF_XMMM)
    std::cerr << "Error: MMaddItems not implemented.\n";
    exit(1);
#else
    Gtk::Widget *w = shell->internal();
    Gtk::Container *cont = dynamic_cast<Gtk::Container *>(w);
    if (!cont) {
	std::cerr << "Error: MMaddItems to non-Container.\n";
	exit(1);
    }
    MMaddItems(cont, shell, items, ignore_seps);
#endif    
}
#endif


//-----------------------------------------------------------------------
// Custom menu creation
//-----------------------------------------------------------------------

// Create pulldown menu from items
MENU_P MMcreatePulldownMenu(CONTAINER_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
			    , ArgList args, Cardinal arg
#endif
			    )
{
#if defined(IF_MOTIF)
    Widget menu = verify(XmCreatePulldownMenu(parent, XMST(name), args, arg));
#else
    MENU_P menu = new Gtk::Menu();
    menu->set_name(name);
#endif
    MMaddItems(menu,
#if !defined(IF_XM)
	       NULL,
#endif
	       items);
#if defined(IF_MOTIF)
    auto_raise(XtParent(menu));
#endif

    return menu;
}

// Create radio pulldown menu from items
MENU_P MMcreateRadioPulldownMenu(CONTAINER_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
				 , ArgList _args, Cardinal _arg
#endif
				 )
{
#if defined(IF_MOTIF)
    ArgList args = new Arg[_arg + 10];
    Cardinal arg = 0;

    XtSetArg(args[arg], XmNisHomogeneous, True); arg++;
    XtSetArg(args[arg], XmNentryClass, xmToggleButtonWidgetClass); arg++;
    XtSetArg(args[arg], XmNradioBehavior, True); arg++;

    for (Cardinal i = 0; i < _arg; i++)
	args[arg++] = _args[i];

    Widget w = MMcreatePulldownMenu(parent, name, items, args, arg);

    delete[] args;
#else
    MENU_P w = MMcreatePulldownMenu(parent, name, items);
#endif
    return w;
}

// Create popup menu from items
MENU_P MMcreatePopupMenu(Widget parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
			 , ArgList args, Cardinal arg
#endif
			 )
{
#if defined(IF_MOTIF)
    Widget menu = verify(XmCreatePopupMenu(parent, XMST(name), args, arg));
#else
    MENU_P menu = new Gtk::Menu();
    menu->set_name(name);
#ifdef NAG_ME
#warning How do we specify the "parent" of a popup menu?
#endif
    // pack_item(parent, menu);
#endif
    MMaddItems(menu,
#if !defined(IF_XM)
	       NULL,
#endif
	       items);
#if defined(IF_MOTIF)
    auto_raise(XtParent(menu));
#endif

    return menu;
}

#if 0
template <class T>
T get_arg_value(ArgList args, Cardinal arg, String name)
{
  for (int i = 0; i < arg; i++) {
    if (args[i].name == name)
      return static_cast<Glib::Value<T> &>(args[i].value).get();
  }
  return T();
}
#endif

// Create menu bar from items
MENUBAR_P MMcreateMenuBar(CONTAINER_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
			  , ArgList args, Cardinal arg
#endif
			  )
{
#if defined(IF_XM)
    Widget bar = verify(XmCreateMenuBar(parent, XMST(name), args, arg));
#else
    GUI::MenuBar *bar = new GUI::MenuBar(parent, name);
#endif
    MMaddItems(bar,
#if !defined(IF_XM)
	       NULL,
#endif
	       items);
    XtManageChild(bar);

    return bar;
}

// Create work area from items
CONTAINER_P MMcreateWorkArea(DIALOG_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
			     , ArgList args, Cardinal arg
#endif
			     )
{
#if defined(IF_MOTIF)
    Widget bar = verify(XmCreateWorkArea(parent, XMST(name), args, arg));
#else
    BOX_P bar = new Gtk::HBox();
    bar->set_name(XMST(name));
    parent->get_vbox()->pack_start(*bar, Gtk::PACK_SHRINK);
#endif
    MMaddItems(bar,
#if !defined(IF_XM)
	       NULL,
#endif
	       items, true);
    XtManageChild(bar);

    return bar;
}

#if defined(IF_XM)
// Create panel from items
BOX_P MMcreatePanel(CONTAINER_P parent, NAME_T name, MMDesc items[]
		    , ArgList args, Cardinal arg
		    )
{
    BOX_P panel = verify(XmCreateWorkArea(parent, XMST(name), args, arg));
    MMaddItems(panel, items);
    XtManageChild(panel);

    return panel;
}
#else
GUI::Container *MMcreatePanel(GUI::Container *parent, cpString name, MMDesc items[])
{
    GUI::Container *panel = new GUI::VBox(*parent, name);
    MMaddItems(panel, items);
    panel->show();
    return panel;
}
#endif

void MMadjustPanel(const MMDesc items[], Dimension space)
{
    // Align panel labels
    Dimension max_label_width = 0;
    const MMDesc *item;
    for (item = items; item != 0 && item->name != 0; item++)
    {
	if (item->label == 0)
	    continue;

#if defined(IF_MOTIF)
	XtWidgetGeometry size;
	size.request_mode = CWWidth;
	XtQueryGeometry(item->label, (XtWidgetGeometry *)0, &size);
	Dimension size_width = size.width;
#else
#ifdef GTKMM_DISABLE_DEPRECATED
	Gtk::Requisition req = item->label->size_request();
#else
	Gtk::Requisition req;
	item->label->size_request(req);
#endif
	Dimension size_width = req.width;
#endif
	max_label_width = max(max_label_width, size_width);
    }

    // Leave some extra space
    max_label_width += space;

    for (item = items; item != 0 && item->name != 0; item++)
    {
	if (item->label == 0)
	    continue;

#if defined(IF_MOTIF)
	XtVaSetValues(item->label,
		      XmNrecomputeSize, False,
		      XmNwidth, max_label_width,
		      XtPointer(0));
#else
	item->label->set_size_request(max_label_width);
#endif
    }
}

#if defined(IF_XM)
// Create radio panel from items
BOX_P MMcreateRadioPanel(CONTAINER_P parent, NAME_T name, MMDesc items[],
			 ArgList _args, Cardinal _arg
			 )
{
    ArgList args = new Arg[_arg + 10];
    Cardinal arg = 0;

    XtSetArg(args[arg], XmNisHomogeneous, True);                      arg++;
    XtSetArg(args[arg], XmNentryClass,    xmToggleButtonWidgetClass); arg++;
    XtSetArg(args[arg], XmNradioBehavior, True);                      arg++;

    for (Cardinal i = 0; i < _arg; i++)
	args[arg++] = _args[i];

    Widget panel = verify(XmCreateRowColumn(parent, XMST(name), args, arg));

    MMaddItems(panel, items);
    XtManageChild(panel);

    delete[] args;
    return panel;
}
#else
// Create radio panel from items
GUI::Container *MMcreateRadioPanel(GUI::Container *parent, cpString name, MMDesc items[])
{
#ifdef NAG_ME
#warning Extra args?
#endif
    GUI::Container *panel = new GUI::HBox(*parent, name);

    MMaddItems(panel, items);
    panel->show();

    return panel;
}
#endif

#if defined(IF_XM)
// Create button panel from items
BOX_P MMcreateButtonPanel(CONTAINER_P parent, NAME_T name, MMDesc items[],
			  ArgList args, Cardinal arg)
{
    Widget panel = verify(XmCreateRowColumn(parent, XMST(name), args, arg));
    MMaddItems(panel, items);
    XtManageChild(panel);

    return panel;
}
#else
// Create button panel from items
GUI::Container *MMcreateButtonPanel(GUI::Container *parent, cpString name, MMDesc items[])
{
    GUI::Container *panel = new GUI::HBox(*parent, name);
    MMaddItems(panel, items);
    panel->show();

    return panel;
}
#endif

#if !defined(IF_XM)
// Create button panel from items
GUI::Container *MMcreateVButtonPanel(GUI::Container *parent, cpString name, MMDesc items[])
{
    GUI::Container *panel = new GUI::VBox(*parent, name);
    MMaddItems(panel, items);
    panel->show();

    return panel;
}
#endif

// Perform proc on items
void MMonItems(const MMDesc items[], MMItemProc proc, XtPointer closure, int depth)
{
    if (depth == 0)
	return;

    for (const MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	if (item->type & MMIgnore)
	    continue;

	proc(item, closure);

	if (item->items)
	    MMonItems(item->items, proc, closure, depth - 1);
    }
}


//-----------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------

// Add callbacks to items
static void addCallback(const MMDesc *item, XtPointer default_closure)
{
    MMType flags            = item->type;
    MMType type             = flags & MMTypeMask;
    Widget widget           = item->widget;
    XtCallbackRec callback  = item->callback;

#if defined(IF_MOTIF)    
    if (callback.closure == 0)
	callback.closure = default_closure;
#endif

    bool flat = false;

    switch(type) 
    {
    case MMFlatPush:
    {
	flat = true;
	// FALL THROUGH
    }

    case MMPush:
    {
#if defined(IF_MOTIF)
	void *userData = 0;
	XtVaGetValues(widget, XmNuserData, &userData, XtPointer(0));

	if (userData != 0)
	{
	    PushMenuInfo *info = (PushMenuInfo *)userData;

	    // A 'push menu' is a menu associated with a push button.
	    // It pops up after pressing the button a certain time.
	    XtAddCallback(widget, XmNarmCallback,    ArmPushMenuCB, info);
	    XtAddCallback(widget, XmNarmCallback,    RedrawPushMenuCB, 0);
	    XtAddCallback(widget, XmNdisarmCallback, RedrawPushMenuCB, 0);

	    static XtTranslations translations =
		    XtParseTranslationTable(pushMenuTranslations);
	    XtAugmentTranslations(widget, translations);

	    if (lesstif_version <= 81)
	    {
		// In LessTif 0.81 and earlier, one must use button 3
		// to pop up push menus
		static XtTranslations lesstif_translations =
		    XtParseTranslationTable(lesstif_pushMenuTranslations);
		XtAugmentTranslations(widget, lesstif_translations);
	    }
	}
#else
#ifdef NAG_ME
#warning We do not handle "Push Menus" yet
#endif
#endif

#if defined(IF_MOTIF)
	if (flat)
	{
	    ReflattenButtonCB(widget, XtPointer(widget));
	}
#else
#ifdef NAG_ME
#warning We do not flatten buttons
#endif
#endif

	// FALL THROUGH
    }

    case MMArrow:
    {
#if defined(IF_MOTIF)
	if (callback.callback)
	    XtAddCallback(widget, 
			  XmNactivateCallback,
			  callback.callback, 
			  callback.closure);
#else
	if (callback) {
	    Gtk::Button *button = dynamic_cast<Gtk::Button *>(widget);
	    if (button)
		button->signal_clicked().connect(sigc::bind(callback, widget));
	    Gtk::MenuItem *mi = dynamic_cast<Gtk::MenuItem *>(widget);
	    if (mi)
		mi->signal_activate().connect(sigc::bind(callback, widget));
	}
#endif
	else
	    set_sensitive(widget, false);
	break;
    }

#if defined(IF_MOTIF)
    case MMToggle:
      // case MMCheckItem:
    case MMScale:
    {
	if (callback.callback != 0)
	    XtAddCallback(widget,
			  XmNvalueChangedCallback,
			  callback.callback, 
			  callback.closure);
	else
	    set_sensitive(widget, false);
	break;
    }
#else
    case MMToggle:
    case MMCheckItem:
    case MMRadio:
    {
	if (callback) {
	    Gtk::ToggleButton *button = dynamic_cast<Gtk::ToggleButton *>(widget);
	    Gtk::CheckMenuItem *mi = dynamic_cast<Gtk::CheckMenuItem *>(widget);
	    if (button)
		button->signal_toggled().connect(sigc::bind(callback, widget));
	    else if (mi)
		mi->signal_toggled().connect(sigc::bind(callback, widget));
	    else
		std::cerr << "WARNING: Item " << item->name << " has unexpected type\n";
	}
	else
	    set_sensitive(widget, false);
	break;
    }
    case MMScale:
    {
	if (callback) {
	    Gtk::Scale *scale = dynamic_cast<Gtk::Scale *>(widget);
	    if (scale)
		scale->signal_value_changed().connect(sigc::bind(callback, widget));
	}
	else
	    set_sensitive(widget, false);
	break;
    }
#endif

    case MMComboBox:
    {
#if defined(IF_MOTIF)
	if (callback.callback != 0)
	{
	    Widget list = ComboBoxList(widget);

	    XtAddCallback(list, XmNbrowseSelectionCallback,
			  callback.callback, callback.closure);
	    XtAddCallback(list, XmNsingleSelectionCallback,
			  callback.callback, callback.closure);
	    XtAddCallback(list, XmNmultipleSelectionCallback,
			  callback.callback, callback.closure);
	    XtAddCallback(list, XmNextendedSelectionCallback,
			  callback.callback, callback.closure);
	}
#else
	if (callback)
	{
	    Gtk::ComboBoxEntryText *combo = dynamic_cast<Gtk::ComboBoxEntryText *>(widget);
	    if (combo) {
#ifdef NAG_ME
#warning Combo box callbacks?
#endif
		std::cerr << "Combo box callbacks?\n";
	    }
	}
#endif

	// FALL THROUGH
    }

#if defined(IF_MOTIF)
    case MMSpinBox:
    case MMTextField:
    {
	if (callback.callback != 0)
	    XtAddCallback(widget,
			  XmNvalueChangedCallback,
			  callback.callback, 
			  callback.closure);

	if (type == MMTextField)
	    break;
	// FALL THROUGH
    }
#else
    case MMTextField:
    {
	if (callback) {
	    Gtk::Entry *entry = dynamic_cast<Gtk::Entry *>(widget);
	    if (entry)
		entry->signal_changed().connect(sigc::bind(callback, widget));
	}
	break;
    }
    case MMSpinBox:
    {
	if (callback) {
	    Gtk::SpinButton *spin = dynamic_cast<Gtk::SpinButton *>(widget);
	    if (spin)
		spin->signal_value_changed().connect(sigc::bind(callback, widget));
	}
	// FALL THROUGH
	// A SpinButton is also an Entry, and invokes a callback on activation.
    }
#endif

    case MMEnterField:
    {
#if defined(IF_MOTIF)
	if (callback.callback != 0) {
	    XtAddCallback(widget,
			  XmNactivateCallback,
			  callback.callback, 
			  callback.closure);
	}
#else
	if (callback) {
	    Gtk::Entry *entry = dynamic_cast<Gtk::Entry *>(widget);
	    if (entry)
		entry->signal_activate().connect(sigc::bind(callback, widget));
	}
#endif
	break;
    }

    case MMMenu:
    case MMRadioMenu:
    case MMOptionMenu:
    {
#if defined(IF_MOTIF)
	Widget subMenu = 0;
	XtVaGetValues(widget, XmNsubMenuId, &subMenu, XtPointer(0));

	if (subMenu != 0 && callback.callback != 0)
	{
	    XtAddCallback(subMenu,
			  XmNmapCallback,
			  callback.callback, 
			  callback.closure);
	    XtAddCallback(subMenu,
			  XmNunmapCallback,
			  callback.callback, 
			  callback.closure);
#if XmVersion >= 1002
	    XtAddCallback(subMenu,
			  XmNtearOffMenuActivateCallback,
			  callback.callback, 
			  callback.closure);
	    XtAddCallback(subMenu,
			  XmNtearOffMenuDeactivateCallback,
			  callback.callback, 
			  callback.closure);
#endif
	}
#else
#ifdef NAG_ME
#warning FIXME: Add callbacks to update menus.
#endif
	{
	    Gtk::MenuItem *mi = dynamic_cast<Gtk::MenuItem *>(widget);
	    Gtk::Menu *subMenu = NULL;
	    if (mi)
		subMenu = mi->get_submenu();
	    else
		std::cerr << "MenuItem is not a MenuItem!\n";

	    if (subMenu != 0 && callback)
	    {
		subMenu->signal_map().connect(sigc::bind(callback, widget));
		subMenu->signal_unmap().connect(sigc::bind(callback, widget));
	    }
	}
#endif
	break;
    }

    case MMLabel:
    case MMSeparator:
    case MMPanel:
    case MMRadioPanel:
    case MMButtonPanel:
#if defined(IF_MOTIF)
	assert(callback.callback == 0);
#else
#ifdef NAG_ME
#warning Check for empty callback?
#endif
	// We may have "bound" the NULL function pointer.
	// In this case !callback does not work.
	// assert(!callback);
#endif
	break;

    default:
	// invalid type
	assert(0);
	abort();
    }
}

void MMaddCallbacks(const MMDesc items[], XtPointer default_closure, int depth)
{
    MMonItems(items, addCallback, default_closure, depth);
}

#if defined(IF_MOTIF)
// Add help callback
struct addHelpCallback_t {
  XtCallbackProc proc;
};

static void addHelpCallback(const MMDesc *item, XtPointer closure)
{
    Widget widget       = item->widget;
    XtCallbackProc proc = STATIC_CAST(addHelpCallback_t*,closure)->proc;

    XtAddCallback(widget, XmNhelpCallback, proc, XtPointer(0));
}
#endif

void MMaddHelpCallback(const MMDesc items[], XtCallbackProc proc, int depth)
{
#if defined(IF_MOTIF)
    addHelpCallback_t proc_ = { proc };
    MMonItems(items, addHelpCallback, XtPointer(&proc_), depth);
#else
#ifdef NAG_ME
#warning No help callbacks yet
#endif
#endif
}



//-----------------------------------------------------------------------
// PushMenus
//-----------------------------------------------------------------------

// Create pushmenu from items
MENU_P MMcreatePushMenu(CONTAINER_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
			, ArgList _args, Cardinal _arg
#endif
			)
{
#if defined(IF_MOTIF)
    ArgList args = new Arg[_arg + 10];
    Cardinal arg = 0;

    // By default, PushButton menus are activated using Button 1.
    if (XmVersion < 1002 || lesstif_version <= 84)
    {
	// Setting the menuPost resource is required by Motif 1.1 and
	// LessTif 0.84 and earlier.  However, OSF/Motif 2.0 (and
	// OSF/Motif 1.2, according to Roy Dragseth
	// <royd@math.uit.no>) choke on this line - buttons become
	// entirely insensitive.
	XtSetArg(args[arg], XmNmenuPost, "<Btn1Down>"); arg++;
    }

#if XmVersion >= 1002
    // Tear-off push menus don't work well - in LessTif, they cause
    // frequent X errors, and in Motif, they disable the old menus
    // once torn off.  So, we explicitly disable them.
    XtSetArg(args[arg], XmNtearOffModel, XmTEAR_OFF_DISABLED); arg++;
#endif
#endif

#if defined(IF_MOTIF)    
    for (Cardinal i = 0; i < _arg; i++)
	args[arg++] = _args[i];

    Widget menu = verify(XmCreatePopupMenu(parent, XMST(name), args, arg));
#else
    MENU_P menu = new Gtk::Menu();
    menu->set_name(XMST(name));
#endif

    MMaddItems(menu,
#if !defined(IF_XM)
	       NULL,
#endif
	       items);
    auto_raise(XtParent(menu));

    // LessTif places a passive grab on the parent, such that the
    // pointer is grabbed as soon as the menuPost event occurs.  This
    // grab breaks PushMenus, so we cancel it.  Motif places a passive
    // grab on button 3, such that the pointer is grabbed as soon as
    // button 3 is pressed.  In Motif 1.1, it even remains grabbed!
    // This breaks any X session, so we cancel it.
#if defined(IF_MOTIF)
    XtUngrabButton(parent, AnyButton, AnyModifier);

    delete[] args;
#else
#ifdef NAG_ME
#warning Do we need the ungrab?
#endif
#endif
    return menu;
}




static XEvent last_push_menu_event; // Just save it

#if defined(IF_MOTIF)
// Remove time out again
static void CancelPopupPushMenuCB(Widget w, XtPointer client_data, 
				  XtPointer call_data)
{
    (void) call_data;		// Use it

#if LOG_PUSH_MENUS
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
#endif

    PushMenuInfo *info = (PushMenuInfo *)client_data;

    if (info->timer != 0)
    {
#if LOG_PUSH_MENUS
	std::clog << "canceling (reason " << cbs->reason << ")\n";
#endif

#if defined(IF_MOTIF)
	XtRemoveTimeOut(info->timer);
#else
	info->timer.disconnect();
#endif
	info->timer = NO_TIMER;
    }

    XtRemoveCallback(w, XmNdisarmCallback,
		     CancelPopupPushMenuCB, XtPointer(info));
    XtRemoveCallback(w, XmNactivateCallback,
		     CancelPopupPushMenuCB, XtPointer(info));

    XtUnmanageChild(info->subMenu);
    Widget shell = XtParent(info->subMenu);
    XtPopdown(shell);
}

// Popup menu right now
static void PopupPushMenuCB(XtPointer client_data, XtIntervalId *id)
{
    (void) id;			// Use it

    PushMenuInfo *info = (PushMenuInfo *)client_data;
    Widget w = info->widget;

    assert(info->timer == *id);
    info->timer = 0;

#if LOG_PUSH_MENUS
    std::clog << "popping up\n";
#endif

    XtRemoveCallback(w, XmNdisarmCallback,
		     CancelPopupPushMenuCB, XtPointer(info));
    XtRemoveCallback(w, XmNactivateCallback,
		     CancelPopupPushMenuCB, XtPointer(info));

    // Popup the menu
    XtCallActionProc(w, "popup-push-menu", &last_push_menu_event, 0, 0);

    // Unactivate pushbutton
    XtCallActionProc(w, "Disarm",          &last_push_menu_event, 0, 0);

#if XmVersion < 1002
    if (XtIsRealized(w))
    {
	// In Motif 1.1, the PushButton does not redisplay after being disarmed
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
    }
#endif
}
#else
#ifdef NAG_ME
#warning PUSH MENUS NOT IMPLEMENTED FOR NOW
#endif
#endif

#if defined(IF_MOTIF)
static void ReflattenButtonEH(Widget shell, XtPointer client_data, 
			      XEvent *event, Boolean *)
{
    switch (event->type)
    {
    case UnmapNotify:
	ReflattenButtonCB(shell, client_data);
	break;

    default:
	break;
    }
}
#else
#ifdef NAG_ME
#warning FLATTEN NOT IMPLEMENTED
#endif
#endif

#if defined(IF_MOTIF)
static void PopupPushMenuAct(Widget w, XEvent *event, String *, Cardinal *)
{
    if (!XmIsPushButton(w))
	return;

    void *userData = 0;
    XtVaGetValues(w, XmNuserData, &userData, XtPointer(0));
    if (userData == 0)
	return;

    PushMenuInfo *info = (PushMenuInfo *)userData;

    Widget shell = XtParent(info->subMenu);

    // Attempt to place menu below button
    Position button_x, button_y;
    XtTranslateCoords(w, 0, 0, &button_x, &button_y);

    Dimension height = 0;
    XtVaGetValues(w, XmNheight, &height, XtPointer(0));

    Position x = button_x;
    Position y = button_y + height;

    event->xbutton.x_root = x;
    event->xbutton.y_root = y;
    XmMenuPosition(info->subMenu, &event->xbutton);

    if (info->flat)
    {
	// Don't have the PushMenu interfere with flattening.  Disable
	// flattening until the menu is popped down again.
	EventMask event_mask = EnterWindowMask | LeaveWindowMask;
	XtRemoveEventHandler(w, event_mask, False, FlattenEH, XtPointer(0));
	XtRemoveCallback(w, XmNarmCallback,    FlattenCB, XtPointer(False));
	XtRemoveCallback(w, XmNdisarmCallback, FlattenCB, XtPointer(True));
#if 0
	XtAddCallback(shell, XtNpopdownCallback, ReflattenButtonCB, 
		      XtPointer(w));
#else
	// XtNpopDownCallback isn't called by LessTif and unknown in
	// Motif 1.1.  *Sigh*.
	XtAddEventHandler(shell, StructureNotifyMask, False, 
			  ReflattenButtonEH, XtPointer(w));
#endif
    }

    XtManageChild(info->subMenu);
    XtPopup(shell, XtGrabNone);
}

static void DecoratePushMenuAct(Widget w, XEvent */* event */, 
				String *, Cardinal *)
{
    if (!XmIsPushButton(w) || !XtIsRealized(w))
	return;

    // std::clog << "Redraw " << XtName(w) << "\n";

    // Draw a little triangle in upper right corner
    XPoint points[4] = {
	{-6, 1}, {5, 0}, {-3, 3}, {-2, -3}
    };

    Dimension width;
    Dimension highlightThickness;
    Dimension shadowThickness;
    Pixel foreground;
    Pixel background;
    Colormap colormap;
    XtVaGetValues(w,
		  XmNwidth, &width,
		  XmNhighlightThickness, &highlightThickness, 
		  XmNshadowThickness, &shadowThickness,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  XmNcolormap, &colormap,
		  XtPointer(0));

    points[0].x += width - highlightThickness - shadowThickness;
    points[0].y += highlightThickness + shadowThickness;

    Pixel color;
    XmGetColors(XtScreen(w), colormap, background, 0, 0, &color, 0);

    XGCValues xgc;
    xgc.foreground = color;
    xgc.background = background;
    GC gc = XtGetGC(w, GCForeground | GCBackground, &xgc);

    XFillPolygon(XtDisplay(w), XtWindow(w), gc, points, XtNumber(points), 
		 Convex, CoordModePrevious);
}

// Popup menu after some delay
struct subresource_values {
    int push_menu_popup_time;	// Delay before popping up menu
};

static XtResource subresources[] = {
    {
	XTRESSTR(XtNpushMenuPopupTime),
	XTRESSTR(XtCPushMenuPopupTime),
	XmRInt,
	sizeof(int),
	XtOffsetOf(subresource_values, push_menu_popup_time), 
	XmRImmediate,
	XtPointer(500)
    }
};

static void ArmPushMenuCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    PushMenuInfo *info = (PushMenuInfo *)client_data;

    subresource_values values;
    XtGetApplicationResources(w, &values, 
			      subresources, XtNumber(subresources), 
			      ArgList(0), 0);

    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    if (cbs && cbs->event)
	last_push_menu_event = *cbs->event;

    if (info->timer != 0)
	XtRemoveTimeOut(info->timer);

    info->timer = XtAppAddTimeOut(XtWidgetToApplicationContext(w), 
				  values.push_menu_popup_time, 
				  PopupPushMenuCB, XtPointer(info));

#if LOG_PUSH_MENUS
    std::clog << "Waiting for " << XtName(w) << " menu " 
	      << "(timer " << info->timer << ")...";
#endif

    // If we're disarmed or activated within the delay, don't popup.
    XtAddCallback(w, XmNdisarmCallback,   
		  CancelPopupPushMenuCB, XtPointer(info));
    XtAddCallback(w, XmNactivateCallback, 
		  CancelPopupPushMenuCB, XtPointer(info));
}

static void RedrawPushMenuCB(Widget w, XtPointer, XtPointer)
{
    XtCallActionProc(w, "decorate-push-menu", 0, 0, 0);
}
#else
#ifdef NAG_ME
#warning PUSH MENU NOT IMPLEMENTED
#endif
#endif


void set_sensitive(Widget w, bool state)
{
    if (w != 0)
    {
#if defined(IF_MOTIF)
	XtSetSensitive(w, state?True:False);
#else
	w->set_sensitive(state);
#endif

#if defined(IF_MOTIF)
	if (!state && w == active_button)
	{
	    // We won't get the LeaveWindow event, since W is now
	    // insensitive.  Flatten button explicitly.
	    flatten_button(w);
	}
#else
#ifdef NAG_ME
#warning Flattening not implemented
#endif
#endif
    }
}

#if !defined(IF_MOTIF)
#ifdef NAG_ME
#warning Can we not just test for a NULL callback and not connect anything?
#endif
void
dummy_callback(Widget)
{
}
#endif

