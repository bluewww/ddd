// $Id$ 
// Java test program

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

//--------------------------------------------------------------------------
// This program defines some data structures and values that may be
// examined using DDD.
//--------------------------------------------------------------------------

public class jtest {
    static void tree_test()
    {
	Tree tree;
	tree =            new Tree(7, "Ada");      // Byron Lovelace
	tree.left =       new Tree(1, "Grace");    // Murray Hopper
	tree.left.left =  new Tree(5, "Judy");     // Clapp
	tree.left.right = new Tree(6, "Kathleen"); // McNulty
	tree.right =      new Tree(9, "Mildred");  // Koss

	tree.date.set(29, 11, 1994);
	tree.date.set(30, 11, 1994);
    }

    static void list_test(int start)
    {
	List list;

	list                = new List(start++);
	list.next           = new List(start++);
	list.next.next      = new List(start++);
	list.next.next.next = list;
    }

    static void array_test()
    {
	int days_of_week[] = {1, 2, 3, 4, 5, 6, 7};

	String twodim [][] = {{ "Pioneering", "women", "in"},
			      { "computer", "science", "!"}};

	int array[][] = new int[12][12];

	// Initialize array
	for (int i = 0; i < 12; i++)
	    for (int j = 0; j < 12; j++)
		array[i][j] = i;

	// Dereference this
	Date dates[] = new Date[4];
	dates[0] = new Date(1, 9, 1994);
	dates[1] = new Date(10, 5, 1994);
	dates[2] = new Date(15, 7, 1994);
	dates[3] = new Date(24, 12, 1994);

	Date date;
	for (int k = 0; k < 4; k++)
	{
	    // Dereference DATE to see it traverse the individual DATES
	    date = dates[k];
	}
    }

    static void type_test()
    {
	Holiday new_years_eve = 
	    new Holiday(31, 12, 1994, "May all acquaintance be forgot");
	Date date = new Date(24, 12, 1994);

	float pi     = (float)3.14159265358979323846;
	double sqrt2 = 1.41421356237309504880;
    }

    public static void main(String[] args) {
	int i = 42;
	tree_test();
	i++;
	list_test(i);
	i++;
	array_test();
	i++;
	type_test();
	--i;
	System.out.println("Hello, World!");

	for (int j = 0; j < 50000; j++)
	     tree_test();
    }
}
