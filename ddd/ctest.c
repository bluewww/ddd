/* $Id$ */
/* C Test program */

/*
 * Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
 * Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
 * 
 * This file is part of DDD.
 * 
 * DDD is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * DDD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with DDD -- see the file COPYING.
 * If not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * DDD is the data display debugger.
 * For details, see the DDD World-Wide-Web page, 
 * `http://www.cs.tu-bs.de/softech/ddd/',
 * or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.
 */

/* --------------------------------------------------------------------------
 *  This program defines some data structures and values that may be
 *  examined using DDD.
 * --------------------------------------------------------------------------
 */

char ctest_rcsid[] =
    "$Id$";

#include <stdio.h>
#include <stdlib.h>

typedef enum _DayOfWeek {Sun, Mon, Tue, Wed, Thu, Fri, Sat} DayOfWeek;

typedef struct Date {
    DayOfWeek day_of_week;
    int day;
    int month;
    int year;
} Date;

void set_date(d, day_of_week, day, month, year)
    Date *d;
    DayOfWeek day_of_week;
    int day, month, year;
{
    d->day_of_week = day_of_week;
    d->day         = day;
    d->month       = month;
    d->year        = year;
}

Date *new_date(day_of_week, day, month, year)
    DayOfWeek day_of_week;
    int day, month, year;
{
    Date *date = (Date *)malloc(sizeof(Date));
    set_date(date, day_of_week, day, month, year);

    return date;
}

void free_date(date)
    Date *date;
{
    free(date);
}

/*--------------------------------------------------------------------------*/
typedef struct _Holiday {
    Date date;
    char *name;
} Holiday;

void set_holiday(d, day_of_week, day, month, year, name)
    Holiday *d;
    DayOfWeek day_of_week;
    int day, month, year;
    char *name;
{
    set_date(&d->date, day_of_week, day, month, year);
    d->name = name;
}

/*--------------------------------------------------------------------------*/
typedef struct _Tree {
    int   value;
    char *name;

    Date date;
    struct _Tree *left;
    struct _Tree *right;
} Tree;

Tree *new_tree(value, name)
    int value;
    char *name;
{
    Tree *tree = (Tree *)malloc(sizeof(Tree));
    tree->value = value;
    tree->name  = name;
    tree->left  = NULL;
    tree->right = NULL;

    return tree;
}

void free_tree(tree)
    Tree *tree;
{
    if (tree->left)
	free_tree(tree->left);
    if (tree->right)
	free_tree(tree->right);

    free(tree);
}

/*--------------------------------------------------------------------------*/
void tree_test ()
{
    Tree *tree = NULL;
    tree =              new_tree (7, "Ada");      /* Byron Lovelace */
    tree->left =        new_tree (1, "Grace");    /* Murray Hopper  */
    tree->left->left =  new_tree (5, "Judy");     /* Clapp          */
    tree->left->right = new_tree (6, "Kathleen"); /* McNulty        */
    tree->right =       new_tree (9, "Mildred");  /* Koss           */

    set_date(&tree->date, Tue, 29, 11, 1994);
    set_date(&tree->date, Wed, 30, 11, 1994);

    free_tree(tree);
}

/*--------------------------------------------------------------------------*/
void array_test ()
{
    int i;
    static DayOfWeek days_of_week[7] = {Sun, Mon, Tue, Wed, Thu, Fri, Sat};

    static char* twodim [2][3] = {{ "Pioneering", "women", "in"},
				  { "computer", "science", "!"}};

    Date dates[4];
    Date* date_ptrs[4];
    date_ptrs[0] = new_date (Thu, 1, 9, 1994);
    date_ptrs[1] = new_date (Tue, 10, 5, 1994);
    date_ptrs[2] = new_date (Fri, 15, 7, 1994);
    date_ptrs[3] = new_date (Sat, 24, 12, 1994);

    for (i = 0; i < 4; i++)
    {
	dates[i] = *date_ptrs[i];
	free_date(date_ptrs[i]);
    }
}

/*--------------------------------------------------------------------------*/

void type_test ()
{
    Holiday holiday;
    Date* date = new_date(Sat, 24, 12, 1994);
    void* voidptr = date;

    struct Uni {
	int ii;
	union jj {
	    int j;
	    char c;
	} u;
    } uni;

    float f  = 0.0;
    double d = 0.0;
    char sc  = 'a';

    set_holiday(&holiday, Sat, 31, 12, 1994, "May all acquaintance be forgot");

    uni.ii  = 7;
    uni.u.j = 9;
}

/*--------------------------------------------------------------------------*/
void in_out_test ()
{
    char name[80];
    printf("What's your name? ");
    gets(name);
    printf("Hello, %s!\n", name);
}

/*--------------------------------------------------------------------------*/
int main (argc, argv)
    int argc;
    char *argv[];
{
    int i = 42;
    tree_test();
    i++;
    array_test();
    i++;
    type_test();
    --i;
    in_out_test();
    return 0;
}
