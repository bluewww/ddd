(*$Id$*)
(*Modula-2 Test Program*)

(*
  Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
  Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
  
  This file is part of DDD.
  
  DDD is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.
  
  DDD is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public
  License along with DDD -- see the file COPYING.
  If not, write to the Free Software Foundation, Inc.,
  675 Mass Ave, Cambridge, MA 02139, USA.
  
  DDD is the GDB-based data display debugger.
  Contact ddd@ips.cs.tu-bs.de for details.
*)

(*--------------------------------------------------------------------------*)
(* This program defines some data structures and values that may be         *)
(* examined using DDD.                                                      *)
(*--------------------------------------------------------------------------*)

MODULE m2test;

FROM Storage IMPORT ALLOCATE, DEALLOCATE;
FROM String IMPORT Assign;
FROM InOut IMPORT WriteString, ReadString, WriteLn;

CONST rcsid = 
    '$Id$';

TYPE DayOfWeek = (Sun, Mon, Tue, Wed, Thu, Fri, Sat);
   Date	       = RECORD
		    day_of_week	: DayOfWeek;
		    day		: INTEGER;
		    month	: INTEGER;
		    year	: INTEGER;
		 END;		
   DatePtr     = POINTER TO Date;
   Holiday     = RECORD
		    date : Date;
		    name : ARRAY[1..20] OF CHAR;
		 END;	 
   TreePtr     = POINTER TO Tree;
   Tree	       = RECORD
		    value : INTEGER;
		    name  : ARRAY[1..20] OF CHAR;
		    date  : Date;
		    left  : TreePtr;
		    right : TreePtr;
		 END;

VAR main_i: INTEGER;

PROCEDURE set_date(VAR d: Date; day_of_week: DayOfWeek;
		       day: INTEGER; month: INTEGER; year: INTEGER);
BEGIN
   d.day_of_week := day_of_week;
   d.day := day;
   d.month := month;
   d.year := year
END set_date;

PROCEDURE new_date(VAR d: DatePtr; day_of_week: DayOfWeek;
		       day: INTEGER; month: INTEGER; year: INTEGER);
BEGIN
   NEW(d);
   set_date(d^, day_of_week, day, month, year)
END new_date;

PROCEDURE set_holiday(VAR h: Holiday; day_of_week: DayOfWeek;
			  day: INTEGER; month: INTEGER; year: INTEGER;
			  name: ARRAY OF CHAR);
VAR success: BOOLEAN;
BEGIN
   set_date(h.date, day_of_week, day, month, year);
   Assign(name, h.name, success)
END set_holiday;

PROCEDURE new_tree(VAR p: TreePtr; value: INTEGER; name: ARRAY OF CHAR);
VAR success: BOOLEAN;
BEGIN
   NEW(p);
   p^.value := value;
   Assign(name, p^.name, success);
   p^.left  := NIL;
   p^.right := NIL
END new_tree;

PROCEDURE dispose_tree(p: TreePtr);
BEGIN
   IF p^.left <> NIL THEN
      dispose_tree(p^.left);
   END;	
   IF p^.right <> NIL THEN
      dispose_tree(p^.right);
   END;

   DISPOSE(p)
END dispose_tree;

PROCEDURE tree_test;
VAR tree : TreePtr;
BEGIN
   tree := NIL;
   new_tree(tree,              7, 'Ada');      (*Byron Lovelace*)
   new_tree(tree^.left,        1, 'Grace');    (*Murray Hopper*)
   new_tree(tree^.left^.left,  5, 'Judy');     (*Clapp*)
   new_tree(tree^.left^.right, 6, 'Kathleen'); (*McNulty*)
   new_tree(tree^.right,       9, 'Mildred');  (*Koss*)

   set_date(tree^.date, Tue, 29, 11, 1994);
   set_date(tree^.date, Wed, 30, 11, 1994);

   dispose_tree(tree)
END tree_test;

PROCEDURE array_test;
VAR i		: INTEGER;
   days_of_week	: ARRAY[1..7] OF DayOfWeek;
   twodim	: ARRAY[1..2] OF ARRAY [1..3] OF ARRAY[1..20] OF CHAR;
   dates	: ARRAY[1..4] OF Date;
   date_ptrs	: ARRAY[1..4] OF DatePtr;
BEGIN
   days_of_week[1] := Sun;
   days_of_week[2] := Mon;
   days_of_week[3] := Tue;
   days_of_week[4] := Wed;
   days_of_week[5] := Thu;
   days_of_week[6] := Fri;
   days_of_week[7] := Sat;

   twodim[1,1] := "Pioneering";
   twodim[1,2] := "women";
   twodim[1,3] := "in";
   twodim[2,1] := "computer";
   twodim[2,2] := "science";
   twodim[2,3] := "!";

   new_date(date_ptrs[1], Thu, 1, 9, 1994);
   new_date(date_ptrs[2], Tue, 10, 5, 1994);
   new_date(date_ptrs[3], Fri, 15, 7, 1994);
   new_date(date_ptrs[4], Sat, 24, 12, 1994);

   FOR i := 1 TO 4 DO
      dates[i] := date_ptrs[i]^;
      DISPOSE(date_ptrs[i]);
   END
END array_test;

PROCEDURE type_test;
VAR holiday : Holiday;
   r	    : REAL;
   c	    : CHAR;
BEGIN
   set_holiday(holiday, Sat, 31, 12, 1994, 'May all acquaintance be forgot');
   r := 3.1415;
   c := 'A'
END type_test;

PROCEDURE in_out_test;
VAR name : ARRAY[1..80] OF CHAR;
BEGIN
   WriteString('What is your name? ');
   ReadString(name);
   WriteString('Hello, ');
   WriteString(name);
   WriteString('!');
   WriteLn
END in_out_test;

BEGIN
   main_i := 42;
   tree_test;
   main_i := main_i + 1;
   array_test;
   main_i := main_i + 1;
   type_test;
   main_i := main_i - 1;
   in_out_test
END m2test.
