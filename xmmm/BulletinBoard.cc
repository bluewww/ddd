// High-level GUI wrapper for Xm.

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

#include <Xmmm/BulletinBoard.h>

using namespace Xmmm;

BulletinBoard::BulletinBoard(Xmmm::Widget &parent, const Xmmm::String &name,
			     std::vector<Arg> &args)
{
    bb_ = XmCreateBulletinBoard(parent.xt_container(), (char *)name.c(),
				&args[0], args.size());
    postinit();
}

BulletinBoard::BulletinBoard(::Widget parent, const Xmmm::String &name,
			     std::vector<Arg> &args)
{
    bb_ = XmCreateBulletinBoard(parent, (char *)name.c(),
				&args[0], args.size());
    postinit();
}

BulletinBoard::~BulletinBoard(void)
{
    XtDestroyWidget(bb_);
}

::Widget BulletinBoard::internal(void)
{
    return bb_;
}

