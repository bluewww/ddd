{$Id$}
{Pascal Test Program}

{
  Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
  Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
  
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
  or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.
}

{--------------------------------------------------------------------------}
{ This program defines some data structures and values that may be         }
{ examined using DDD.                                                      }
{--------------------------------------------------------------------------}

program ptest(input, output);

const rcsid =
    '$Id$';

type DayOfWeek = (Sun, Mon, Tue, Wed, Thu, Fri, Sat);
   Date	       = record
		    day_of_week	: DayOfWeek;
		    day		: integer;
		    month	: integer;
		    year	: integer;
		 end;		
   DatePtr     = ^Date;
   Holiday     = record
		    date : Date;
		    name : string;
		 end;	 
   TreePtr     = ^Tree;
   Tree	       = record
		    value : integer;
		    name  : string;
		    date  : Date;
		    left  : TreePtr;
		    right : TreePtr;
		 end;

var main_i: integer;

procedure set_date(var d: Date; day_of_week: DayOfWeek;
		       day: integer; month: integer; year: integer);
begin
   d.day_of_week := day_of_week;
   d.day         := day;
   d.month       := month;
   d.year        := year
end; { set_date }

procedure new_date(var d: DatePtr; day_of_week: DayOfWeek;
		       day: integer; month: integer; year: integer);
begin
   new(d);
   set_date(d^, day_of_week, day, month, year)
end; { new_date }

procedure set_holiday(var h: Holiday; day_of_week: DayOfWeek;
			  day: integer; month: integer; year: integer;
			  name: string);
begin
   set_date(h.date, day_of_week, day, month, year);
   h.name := name
end; { set_holiday }

procedure new_tree(var p: TreePtr; value: integer; name: string);
begin
   new(p);
   p^.value := value;
   p^.name  := name;
   p^.left  := nil;
   p^.right := nil
end; { new_tree }

procedure dispose_tree(p: TreePtr);
begin
   if p^.left <> nil then
      dispose_tree(p^.left);
   if p^.right <> nil then
      dispose_tree(p^.right);

   dispose(p)
end; { dispose_tree }

procedure tree_test;
var tree : TreePtr;
begin
   tree := nil;
   new_tree(tree,              7, 'Ada');      {Byron Lovelace}
   new_tree(tree^.left,        1, 'Grace');    {Murray Hopper}
   new_tree(tree^.left^.left,  5, 'Judy');     {Clapp}
   new_tree(tree^.left^.right, 6, 'Kathleen'); {McNulty}
   new_tree(tree^.right,       9, 'Mildred');  {Koss}

   set_date(tree^.date, Tue, 29, 11, 1994);
   set_date(tree^.date, Wed, 30, 11, 1994);

   dispose_tree(tree)
end; { tree_test }

procedure array_test;
var i		: integer;
   days_of_week	: array[1..7] of DayOfWeek;
   twodim	: array[1..2,1..3] of string;
   dates	: array[1..4] of Date;
   date_ptrs	: array[1..4] of DatePtr;
begin
   days_of_week[1] := Sun;
   days_of_week[2] := Mon;
   days_of_week[3] := Tue;
   days_of_week[4] := Wed;
   days_of_week[5] := Thu;
   days_of_week[6] := Fri;
   days_of_week[7] := Sat;

   twodim[1,1] := 'Pioneering';
   twodim[1,2] := 'women';
   twodim[1,3] := 'in';
   twodim[2,1] := 'computer';
   twodim[2,2] := 'science';
   twodim[2,3] := '!';

   new_date(date_ptrs[1], Thu, 1, 9, 1994);
   new_date(date_ptrs[2], Tue, 10, 5, 1994);
   new_date(date_ptrs[3], Fri, 15, 7, 1994);
   new_date(date_ptrs[4], Sat, 24, 12, 1994);

   for i := 1 to 4 do
   begin
      dates[i] := date_ptrs[i]^;
      dispose(date_ptrs[i]);
   end
end; { array_test }

procedure type_test;
var holiday : Holiday;
   r	    : real;
   c	    : char;
begin
   set_holiday(holiday, Sat, 31, 12, 1994, 'May all acquaintance be forgot');
   r := 3.1415;
   c := 'A'
end; { type_test }

procedure in_out_test;
var name : string;
begin
   write('What is your name? ');
   read(name);
   readln;
   write('Hello, ', name:20, '!')
end;

begin
   main_i := 42;
   tree_test;
   main_i := main_i + 1;
   array_test;
   main_i := main_i + 1;
   type_test;
   main_i := main_i - 1;
   in_out_test
end.
