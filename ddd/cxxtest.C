// $Id$ 
// Test program

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

//--------------------------------------------------------------------------
// This program defines some data structures and values that may be
// examined using DDD.
//--------------------------------------------------------------------------

char cxxtest_rcsid[] =
    "$Id$";

#include <iostream.h>

enum DayOfWeek {Sun, Mon, Tue, Wed, Thu, Fri, Sat};

//--------------------------------------------------------------------------
class Date {
    DayOfWeek day_of_week;
    int day;
    int month;
    int year;
public:
    Date() 
	: day_of_week(Thu), day(1), month(1), year(1970)
    {}
    Date(DayOfWeek w, int d, int m, int y)
	: day_of_week(w), day(d), month(m), year(y)
    {}
    virtual ~Date()
    {}
    void set(DayOfWeek w, int d, int m, int y)
    {
	day_of_week = w;
	day         = d;
	month       = m;
	year        = y;
    }

    virtual void print()    {}
    virtual void print(int) {}
};

//--------------------------------------------------------------------------
class Holiday : public Date {
    const char *name;

public:
    Holiday(DayOfWeek w, int d, int m, int y, const char *n) :
	Date (w, d, m, y), name(n)
    {}
    virtual ~Holiday()
    {}

    virtual void print()    {}
    virtual void print(int) {}
};
//--------------------------------------------------------------------------
class Tree {
    int   value;
    const char *name;
public:
    Date date;
    Tree *left;
    Tree *right;

    Tree(int v, char *n):
	value(v), name(n), left(0), right(0)
    {}
    ~Tree() 
    {
	if (left)
	    delete left;
	if (right)
	    delete right;
    }
};
//--------------------------------------------------------------------------
class List {
    int value;
public:
    List *self;
    List *next;

    List(int v):
	value(v), self(this), next(this)
    {}
};

//--------------------------------------------------------------------------
// Simple binary tree
void tree_test()
{
    Tree *tree = 0;
    tree =              new Tree(7, "Ada");      // Byron Lovelace
    tree->left =        new Tree(1, "Grace");    // Murray Hopper
    tree->left->left =  new Tree(5, "Judy");     // Clapp
    tree->left->right = new Tree(6, "Kathleen"); // McNulty
    tree->right =       new Tree(9, "Mildred");  // Koss

    tree->date.set(Tue, 29, 11, 1994);
    tree->date.set(Wed, 30, 11, 1994);

    delete tree;
}

//--------------------------------------------------------------------------
// Simple circular list.  Examine `list' with alias detection enabled.
void list_test(int start)
{
    List *list = 0;

    list                         = new List(start++);
    list->next                   = new List(start++);
    list->next->next             = new List(start++);
    list->next->next->next       = list;

    delete list->next->next;
    delete list->next;
    delete list;
}

// Test disambiguation
void list_test(double d)
{
    list_test(int(d));
}

//--------------------------------------------------------------------------
void reference_test(Date& date, Date*& date_ptr)
{
    date = *date_ptr;
    delete date_ptr;
    date_ptr = 0;
}

//--------------------------------------------------------------------------
void array_test()
{
    // Play woth rotate and show/hide buttons
    DayOfWeek days_of_week[7] = {Sun, Mon, Tue, Wed, Thu, Fri, Sat};

    char *twodim [2][3] = {{ "Pioneering", "women", "in"},
			   { "computer", "science", "!"}};

    (void) twodim;		// Use it
    (void) days_of_week;	// Use it

    // Dereference this
    Date *date_ptrs[4];
    date_ptrs[0] = new Date(Thu, 1, 9, 1994);
    date_ptrs[1] = new Date(Tue, 10, 5, 1994);
    date_ptrs[2] = new Date(Fri, 15, 7, 1994);
    date_ptrs[3] = new Date(Sat, 24, 12, 1994);

    Date *date_ptr;
    for (int i = 0; i < 4; i++)
    {
	// Dereference DATE_PTR to see it traverse the individual *DATE_PTRs
	date_ptr = date_ptrs[i];
	reference_test(*date_ptr, date_ptrs[i]);
    }
}

//--------------------------------------------------------------------------
void type_test()
{
    Holiday new_years_eve(Sat, 31, 12, 1994, 
			  "May all acquaintance be forgot");

    Date *date = new Date(Sat, 24, 12, 1994);
    void *voidptr = date;

    (void) voidptr;		// Use it

    struct Uni {
	int ii;
	unsigned bit1:1;
	unsigned bit2:2;
	union {
	    int j;
	    char c;
	}u;
    } uni;

    struct Guni {
	int ii;
	union {
	    unsigned bit1:1;
	    int j;
	    char c;
	};
	union {
	    unsigned bit2:1;
	    unsigned bit3:1;
	};
    };

    uni.ii   = 7;
    uni.bit1 = 1;
    uni.bit2 = 3;
    uni.u.j  = 9;
    uni.u.c  = 'a';

    Guni guni; 
    guni.ii   = 1;
    guni.j    = 2;
    guni.c    = 'a';
    guni.bit1 = 1;
    guni.bit2 = 1;
    guni.bit3 = 0;

    float pi       = 3.14159265358979323846;
    double sqrt2   = 1.41421356237309504880;
    signed char sc = 'a';

    guni.ii = int(pi * sqrt2 * sc);
}

//--------------------------------------------------------------------------
void cin_cout_test()
{
    // Simple I/O
    char name[1024];
    cout << "What's your name? ";
    cin >> name;
    cout << "Hello, " << name << "!\n";
}

//--------------------------------------------------------------------------
int main(int /* argc */, char ** /* argv */)
{
    int i = 42;
    tree_test();
    i++;
    list_test(i);
    i++;
    array_test();
    i++;
    type_test();
    --i;
    cin_cout_test();
    return 0;
}
