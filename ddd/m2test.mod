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
  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  
  DDD is the data display debugger.
  For details, see the DDD World-Wide-Web page, 
  `http://www.cs.tu-bs.de/softech/ddd/',
  or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.
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
		    dayOfWeek	: DayOfWeek;
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

VAR mainI: INTEGER;

PROCEDURE setDate(VAR d: Date; dayOfWeek: DayOfWeek;
		      day: INTEGER; month: INTEGER; year: INTEGER);
BEGIN
   d.dayOfWeek := dayOfWeek;
   d.day       := day;
   d.month     := month;
   d.year      := year
END setDate;

PROCEDURE newDate(VAR d: DatePtr; dayOfWeek: DayOfWeek;
		      day: INTEGER; month: INTEGER; year: INTEGER);
BEGIN
   NEW(d);
   setDate(d^, dayOfWeek, day, month, year)
END newDate;

PROCEDURE setHoliday(VAR h: Holiday; dayOfWeek: DayOfWeek;
		         day: INTEGER; month: INTEGER; year: INTEGER;
			 name: ARRAY OF CHAR);
VAR success: BOOLEAN;
BEGIN
   setDate(h.date, dayOfWeek, day, month, year);
   Assign(name, h.name, success)
END setHoliday;

PROCEDURE newTree(VAR p: TreePtr; value: INTEGER; name: ARRAY OF CHAR);
VAR success: BOOLEAN;
BEGIN
   NEW(p);
   p^.value := value;
   Assign(name, p^.name, success);
   p^.left  := NIL;
   p^.right := NIL
END newTree;

PROCEDURE disposeTree(p: TreePtr);
BEGIN
   IF p^.left <> NIL THEN
      disposeTree(p^.left);
   END;	
   IF p^.right <> NIL THEN
      disposeTree(p^.right);
   END;

   DISPOSE(p)
END disposeTree;

PROCEDURE treeTest;
VAR tree : TreePtr;
BEGIN
   tree := NIL;
   newTree(tree,              7, 'Ada');      (*Byron Lovelace*)
   newTree(tree^.left,        1, 'Grace');    (*Murray Hopper*)
   newTree(tree^.left^.left,  5, 'Judy');     (*Clapp*)
   newTree(tree^.left^.right, 6, 'Kathleen'); (*McNulty*)
   newTree(tree^.right,       9, 'Mildred');  (*Koss*)

   setDate(tree^.date, Tue, 29, 11, 1994);
   setDate(tree^.date, Wed, 30, 11, 1994);

   disposeTree(tree)
END treeTest;

PROCEDURE arrayTest;
VAR i		: INTEGER;
   daysOfWeek	: ARRAY[1..7] OF DayOfWeek;
   twodim	: ARRAY[1..2] OF ARRAY [1..3] OF ARRAY[1..20] OF CHAR;
   dates	: ARRAY[1..4] OF Date;
   datePtrs	: ARRAY[1..4] OF DatePtr;
BEGIN
   daysOfWeek[1] := Sun;
   daysOfWeek[2] := Mon;
   daysOfWeek[3] := Tue;
   daysOfWeek[4] := Wed;
   daysOfWeek[5] := Thu;
   daysOfWeek[6] := Fri;
   daysOfWeek[7] := Sat;

   twodim[1,1] := "Pioneering";
   twodim[1,2] := "women";
   twodim[1,3] := "in";
   twodim[2,1] := "computer";
   twodim[2,2] := "science";
   twodim[2,3] := "!";

   newDate(datePtrs[1], Thu, 1, 9, 1994);
   newDate(datePtrs[2], Tue, 10, 5, 1994);
   newDate(datePtrs[3], Fri, 15, 7, 1994);
   newDate(datePtrs[4], Sat, 24, 12, 1994);

   FOR i := 1 TO 4 DO
      dates[i] := datePtrs[i]^;
      DISPOSE(datePtrs[i]);
   END
END arrayTest;

PROCEDURE typeTest;
VAR holiday : Holiday;
   r	    : REAL;
   c	    : CHAR;
BEGIN
   setHoliday(holiday, Sat, 31, 12, 1994, 'May all acquaintance be forgot');
   r := 3.1415;
   c := 'A'
END typeTest;

PROCEDURE inOutTest;
VAR name : ARRAY[1..80] OF CHAR;
BEGIN
   WriteString('What is your name? ');
   ReadString(name);
   WriteString('Hello, ');
   WriteString(name);
   WriteString('!');
   WriteLn
END inOutTest;

BEGIN
   mainI := 42;
   treeTest;
   mainI := mainI + 1;
   arrayTest;
   mainI := mainI + 1;
   typeTest;
   mainI := mainI - 1;
   inOutTest
END m2test.
