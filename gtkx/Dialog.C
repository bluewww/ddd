// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <GtkX/Dialog.h>

using namespace GtkX;

Dialog::Dialog(Gtk::Window *parent, const String &name):
    Gtk::Dialog(name.s(), *parent)
{
    set_name(name.s());
    set_title(name.s());
    // GtkX::VBox *box = new GtkX::VBox(*this, name+String("_vbox"));
    GtkX::VBox *box = new GtkX::VBox(get_vbox(), name+String("_vbox"));
    box->show();
    // buttons_ = new GtkX::HBox(*this, name+String("_buttons"));
    buttons_ = new GtkX::HBox(get_vbox(), name+String("_buttons"));
    buttons_->show();
    vbox_ = box;
    postinit();
}

Dialog::Dialog(GtkX::Widget &parent, const String &name):
    Gtk::Dialog(name.s(), *dynamic_cast<Gtk::Window *>(parent.internal()))
{
    set_name(name.s());
    set_title(name.s());
    // GtkX::VBox *box = new GtkX::VBox(*this, name+String("_vbox"));
    GtkX::VBox *box = new GtkX::VBox(get_vbox(), name+String("_vbox"));
    box->show();
    // buttons_ = new GtkX::HBox(*this, name+String("_buttons"));
    buttons_ = new GtkX::HBox(get_vbox(), name+String("_buttons"));
    buttons_->show();
    vbox_ = box;
    postinit();
}

Dialog::Dialog(const String &name):
    Gtk::Dialog(name.s())
{
    set_name(name.s());
    set_title(name.s());
    // GtkX::VBox *box = new GtkX::VBox(*this, name+String("_vbox"));
    GtkX::VBox *box = new GtkX::VBox(get_vbox(), name+String("_vbox"));
    box->show();
    // buttons_ = new GtkX::HBox(*this, name+String("_buttons"));
    buttons_ = new GtkX::HBox(get_vbox(), name+String("_buttons"));
    buttons_->show();
    vbox_ = box;
    postinit();
}

Dialog::~Dialog(void)
{
    delete vbox_;
    delete buttons_;
}

Gtk::Widget *
Dialog::internal(void)
{
    return this;
}

Gtk::Container *
Dialog::gtk_container(void)
{
    return vbox_?vbox_:get_vbox();
}

Button *
Dialog::add_button(const String &name, const String &label)
{
    return new Button(*buttons_, name, label);
}

MessageDialog::MessageDialog(GtkX::Widget &parent, const String &name,
			     const String &label, MessageType type):
    Dialog(parent, name)
{
    message_ = new GtkX::Label(*this, name+String("-label"));
}

MessageDialog::MessageDialog(const String &name, const String &label,
			     MessageType type):
    Dialog(name)
{
    message_ = new GtkX::Label(*this, name+String("-label"));
}

MessageDialog::~MessageDialog(void)
{
    delete message_;
}


