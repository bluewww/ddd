// $Id$ 
// Test program

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

//--------------------------------------------------------------------------
// This program defines some data structures and values that may be
// examined using DDD.
//--------------------------------------------------------------------------

char cxxtest_rcsid[] =
    "$Id$";

#include "config.h"
#include "bool.h"

#include <iostream.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//--------------------------------------------------------------------------
extern "C" {
#if HAVE_RANDOM && !HAVE_RANDOM_DECL && !defined(random)
long int random();
#endif
#if HAVE_SRANDOM && !HAVE_SRANDOM_DECL && !defined(srandom)
void srandom(unsigned int seed);
#endif
#if HAVE_RAND && !HAVE_RAND_DECL && !defined(rand)
int rand();
#endif
#if HAVE_SRAND && !HAVE_SRAND_DECL && !defined(srand)
void srand(unsigned int seed);
#endif
};

static void init_random_seed()
{
    static bool seed_initialized = false;
    if (seed_initialized)
	return;

    time_t tm;
    time(&tm);

#if HAVE_SRAND
    srand((int)tm);
#elif HAVE_SRANDOM
    srandom((int)tm);
#endif

    seed_initialized = true;
}

static int rnd(int x)
{ 
    init_random_seed();

#if HAVE_RAND
    return rand() % x;
#else /* HAVE_RANDOM */
    return random() % x;
#endif
}

enum DayOfWeek {Sun, Mon, Tue, Wed, Thu, Fri, Sat};

static int a_global = 42;	// Place watchpoints on this one

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
    {
	a_global = 1;
    }
    virtual ~Holiday()
    {}

    virtual void print()    {}
    virtual void print(int) {}
};

//--------------------------------------------------------------------------
class Tree {
    int   value;
    const char *_name;

    Tree *_left;
    Tree *_right;
    bool left_thread;
    bool right_thread;

protected:
    void add_left_threads(Tree *);
    void add_right_threads(Tree *);

public:
    Date date;

    static int shared;

    Tree(int v, const char *n):
	value(v), _name(n), _left(0), _right(0), 
	left_thread(false), right_thread(false)
    {}

    Tree *left()  { return left_thread  ? 0 : _left; }
    Tree *right() { return right_thread ? 0 : _right; }

    const char *name() { return _name; }

    void set_left(Tree *t)  { _left = t; }
    void set_right(Tree *t) { _right = t; }

    // In-order traversal
    Tree *first();
    Tree *next();
    Tree *last();
    Tree *prev();

    ~Tree()
    {
	delete left();
 	delete right();
    }

    void add_threads();
};

int Tree::shared = 4711;

void Tree::add_threads()
{
    add_left_threads(0);
    add_right_threads(0);
}

void Tree::add_right_threads(Tree *parent)
{
    if (_right == 0)
    {
	_right = parent;
	right_thread = (_right != 0);
    }
    else
    {
	_right->add_right_threads(parent);
    }

    if (_left != 0 && !left_thread)
	_left->add_right_threads(this);
}

void Tree::add_left_threads(Tree *parent)
{
    if (_left == 0)
    {
	_left = parent;
	left_thread = (_left != 0);
    }
    else
    {
	_left->add_left_threads(parent);
    }

    if (_right != 0 && !right_thread)
	_right->add_left_threads(this);
}

Tree *Tree::next()
{
    if (right_thread)
	return _right;
    if (_right != 0)
	return _right->first();
    return 0;
}

Tree *Tree::prev()
{
    if (left_thread)
	return _left;
    if (_left != 0)
	return _left->last();
    return 0;
}

Tree *Tree::first()
{
    Tree *t = this;
    while (t->_left != 0 && !t->left_thread)
	t = t->_left;

    return t;
}

Tree *Tree::last()
{
    Tree *t = this;
    while (t->_right != 0 && !t->right_thread)
	t = t->_right;

    return t;
}

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
// Simple threaded tree

void tree_test()
{
    Tree *tree = 0;

    tree = new Tree(7, "Ada");                        // Byron Lovelace
    tree->set_left(new Tree(1, "Grace"));             // Murray Hopper
    tree->left()->set_left(new Tree(5, "Judy"));      // Clapp
    tree->left()->set_right(new Tree(6, "Kathleen")); // McNulty
    tree->set_right(new Tree(1, "Mildred"));          // Koss

    tree->add_threads();

    for (Tree *t = tree->first(); t != 0; t = t->next())
	cout << t->name() << " ";
    cout << "\n";

    tree->date.set(Tue, 29, 11, 1994);
    tree->date.set(Wed, 30, 11, 1994);

    delete tree;
}

//--------------------------------------------------------------------------
// Simple circular list.  Examine `list' with alias detection enabled.
void list_test(int start)
{
    List *list = 0;

    list                         = new List(a_global + start++);
    list->next                   = new List(a_global + start++);
    list->next->next             = new List(a_global + start++);
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
    // Play with rotate and show/hide buttons
    DayOfWeek days_of_week[7] = {Sun, Mon, Tue, Wed, Thu, Fri, Sat};

    char *twodim [2][3] = {{ "Pioneering", "women", "in"},
			   { "computer", "science", "!"}};

    int array[12][12];

    (void) twodim;		// Use it
    (void) days_of_week;	// Use it
    (void) array;		// Use it

    // Initialize array
    for (int i = 0; i < int(sizeof(array) / sizeof(array[0])); i++)
	for (int j = 0; j < int(sizeof(array[0]) / sizeof(array[0][0])); j++)
	    array[i][j] = 0;

    // Dereference this
    Date *date_ptrs[4];
    date_ptrs[0] = new Date(Thu, 1, 9, 1994);
    date_ptrs[1] = new Date(Tue, 10, 5, 1994);
    date_ptrs[2] = new Date(Fri, 15, 7, 1994);
    date_ptrs[3] = new Date(Sat, 24, 12, 1994);

    Date *date_ptr;
    for (int k = 0; k < 4; k++)
    {
	// Dereference DATE_PTR to see it traverse the individual *DATE_PTRs
	date_ptr = date_ptrs[k];
	reference_test(*date_ptr, date_ptrs[k]);
	a_global = k;
    }

    // A `repeat' test.
    static int *ar[100];
    static int 	ir[100];

    ar[1]  = &ir[1];	ir[1]  = 1;
    ar[50] = &ir[50];	ir[50] = 50;
    ar[51] = &ir[51];	ir[51] = 51;
    ar[99] = &ir[99];	ir[99] = 99;
}

//--------------------------------------------------------------------------
#define numbers(x) (sizeof((x)) / sizeof((x)[0]))

void shell_sort(int a[], int size)
{
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= size);
    do {
	h /= 3;
	for (int i = h; i < size; i++)
	{
	    int v = a[i];
	    int j;
	    for (j = i; j >= h && a[j - h] > v; j -= h)
		a[j] = a[j - h];
	    if (i != j)
		a[j] = v;
	}
    } while (h != 1);
}

void plot_test()
{
    static int ir[100];

    for (int i = 0; i < numbers(ir); i++)
	ir[i] = rnd(100);

    shell_sort(ir, numbers(ir));

    static double dr[10][100];
    double pi = 3.14159265358979323846;

    for (int i = 0; i < numbers(dr); i++)
	for (int j = 0; j < numbers(dr[0]); j++)
	    dr[i][j] = i * i * sin(j * 2 * pi / numbers(dr[0]) * 3);

    (void) dr;
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
    guni.bit1 = 0;
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
    cerr << "Hello, " << name << "!\n";
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
    plot_test();
    i++;
    type_test();
    --i;
    cin_cout_test();
    return 0;
}
