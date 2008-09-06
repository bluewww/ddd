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

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <GtkX/ImageW.h>

using namespace GtkX;

void
Image::init(Container &parent, PackOptions po,
	    const String &name)
{
    hbox_ = new Gtk::HBox();
    vbox_ = new Gtk::VBox();
    hbox_->pack_start(*vbox_, Gtk::PACK_EXPAND_PADDING, 4);
    frame_ = new Gtk::Frame();
    vbox_->pack_start(*frame_, Gtk::PACK_EXPAND_PADDING, 4);
    evbox_ = new Gtk::EventBox();
    frame_->add(*evbox_);
    image_ = new Gtk::Image();
    evbox_->add(*image_);
    image_->show();
    frame_->show();
    evbox_->show();
    vbox_->show();
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

Image::Image(Container &parent, PackOptions po,
	     const String &name)
{
    init(parent, po, name);
}

Image::Image(Container &parent, PackOptions po,
	     const String &name,
	     const ImageHandle &image)
{
    init(parent, po, name);
    set(image);
}

Image::~Image()
{
    delete image_;
    delete frame_;
    delete vbox_;
    delete hbox_;
}

Gtk::Widget *
Image::internal()
{
    return hbox_;
}

const Gtk::Widget *
Image::internal() const
{
    return hbox_;
}

void
Image::set(const ImageHandle &im)
{
    image_->set(im);
}

Gtk::Widget *
Image::signals_from()
{
    return evbox_;
}
