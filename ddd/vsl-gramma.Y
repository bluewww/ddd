/* $Id$ -*- C++ -*- */
/* VSL grammar */

%{

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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


// Some declarations and utilities

char VSL_yacc_rcsid[] = 
    "$Id$";


// Funktionsaufrufe

// Geeigneten Knoten fuer Funktionsnamen zurueckgeben
VSLNode *VSLLib::_call(const string& func_name, VSLNode *arg)
{
    // Definitionsliste finden
    VSLDefList* def = deflist(func_name);
    if (def == 0)
    {
	VSLLib::parse_error(string("'") + func_name + "(...)' undefined");
	delete arg;
	arg = 0;
    }

    // Aufruf zurueckgeben
    if (def && arg)
	return new DefCallNode(def, arg);

    return 0;
}

VSLNode *VSLLib::call(const string& name)
{
    return _call(name, new EmptyListNode);
}

VSLNode *VSLLib::call(const string& name, VSLNode *arg)
{
    if (arg)
	return _call(name, new FixListNode(arg));

    return 0;
}

VSLNode *VSLLib::call(const string& name, VSLNode *arg1, VSLNode *arg2)
{
    if (arg1 && arg2)
	return _call(name, new FixListNode(arg1, arg2));

    return 0;
}

VSLNode *VSLLib::call(const string& name, 
		      VSLNode *arg1, VSLNode *arg2, VSLNode *arg3)
{
    if (arg1 && arg2 && arg3)
	return _call(name, new FixListNode(arg1, arg2, arg3));

    return 0;
}

// Some settings
#define YYERROR_VERBOSE

#ifdef YYERROR_VERBOSE
#define YYDEBUG 1
#endif
%}

/*** Tokens ***/
%token IDENTIFIER		/* [_a-zA-Z][_a-zA-Z0-9]* */
%token STRING			/* \"(\\.|[^\"\n])*\" */
%token INTEGER			/* [0-9]+ */
%token ARROW			/* -> */
%token IF			/* if */
%token THEN			/* then */
%token ELSE			/* else */
%token ELSIF			/* elsif */
%token FI			/* fi */
%token OR			/* or */
%token AND			/* and */
%token NOT			/* not */
%token LET			/* let */
%token IN			/* in */
%token WHERE			/* where */
%token OVERRIDE			/* ^#override */
%token REPLACE			/* ^#replace */
%token EQ			/* = */
%token NE			/* <> */
%token GT			/* > */
%token GE			/* >= */
%token LT			/* < */
%token LE			/* <= */
%token HALIGN			/* & */
%token VALIGN			/* | */
%token UALIGN			/* ^ */
%token TALIGN			/* ~ */
%token APPEND			/* : */
%token CONS			/* :: */
%token THREEDOTS		/* ... */

/*** Operator associativity and precedence (raising) ***/
%left OR			/* or */
%left AND			/* and */
%left EQ NE			/* = <> */
%left LE LT GE GT		/* <= < >= > */
%right CONS			/* : */
%left VALIGN			/* | */
%left UALIGN			/* ^ */
%left TALIGN			/* ~ */
%left HALIGN			/* & */
%left '+' '-'			/* + - */
%left '*' '/' '%'		/* * / % */
%right NOT			/* not */

%union {
    // Our special yacctoC program makes this a struct -- 
    // thus we use an anonymous union (does not harm in other cases)
    union {
	VSLNode *node;
	string *str;
	int num;
	double fnum;
	struct {
	    string *id;
	    VSLNode *pattern;
	    string *file;
	    int line;
	} header;
	struct {
	    VSLNode *pattern;
	    VSLNode *args;
	} vardef;
    };
}

%type <str> identifier function_identifier string_constant
%type <header> function_header local_header global_header
%type <node> function_body box_expression_with_defs box_expression_with_wheres
	box_expression const_expression
	unary_expression binary_expression function_call function_argument
	argument_or_function cond_expression else_expression list_expression 
	box_expression_list multiple_box_expression_list
	in_box_expression box_expression_with_where
%type <num> numeric_constant
%type <vardef> var_definition 

%start file

%%

/*** files ***/

file			:	item_list

item_list		:	/* empty */
			|	item_list item

item			:	function_declaration ';'
			|	function_definition ';'
			|	override_declaration
			|	replace_declaration
			|	';'
			|	error ';'

/*** functions ***/

function_declaration	:	function_header
				{
				  if ($1.pattern)
				  {
				    vsllib->add(*$1.id,
				      $1.pattern, 0, False,
				      *$1.file, $1.line);
				  }
				  delete $1.id;
				  delete $1.file;
				}

function_header		:	function_identifier function_argument
				{
				  $$.id      = $1;
				  $$.pattern = $2;
				  $$.file    = new string(yyfilename);
				  $$.line    = yylinenumber;
				}
			|	function_identifier
				{
				  $$.id      = new string(string("#") + *$1);
				  $$.pattern = new EmptyListNode;
				  $$.file    = new string(yyfilename);
				  $$.line    = yylinenumber;

				  delete $1;
				}

function_identifier	:	identifier
				{ $$ = $1; }
			|	'(' EQ ')'
				{ $$ = new string("(=)"); }
			|	'(' NE ')'
				{ $$ = new string("(<>)"); }
			|	'(' GT ')'
				{ $$ = new string("(>)"); }
			|	'(' GE ')'
				{ $$ = new string("(>=)"); }
			|	'(' LT ')'
				{ $$ = new string("(<)"); }
			|	'(' LE ')'
				{ $$ = new string("(<=)"); }
			|	'(' HALIGN ')'
				{ $$ = new string("(&)"); }
			|	'(' VALIGN ')'
				{ $$ = new string("(|)"); }
			|	'(' UALIGN ')'
				{ $$ = new string("(^)"); }
			|	'(' TALIGN ')'
				{ $$ = new string("(~)"); }
			|	'(' '+' ')'
				{ $$ = new string("(+)"); }
			|	'(' '-' ')'
				{ $$ = new string("(-)"); }
			|	'(' '*' ')'
				{ $$ = new string("(*)"); }
			|	'(' '/' ')'
				{ $$ = new string("(/)"); }
			|	'(' '%' ')'
				{ $$ = new string("(%)"); }
			|	'(' CONS ')'
				{ $$ = new string("(::)"); }
			|	'(' NOT ')'
				{ $$ = new string("(not)"); }

identifier		:	IDENTIFIER
				{ $$ = new string((char *)yytext); }

function_definition	:	local_definition
			|	global_definition

local_definition	: 	local_header function_body
				{ 
				  if ($1.pattern)
				  {
				    // Funktion definieren
				    vsllib->add(*$1.id,
				      $1.pattern, $2, False,
				      *$1.file, $1.line);
				  }
				  delete $1.id;
				  delete $1.file;
				}

local_header		:	function_header EQ
				{
				  if ($1.pattern)
				  {
				    // Funktion bereits deklarieren
				    // (fuer rekursive Aufrufe)
				    vsllib->add(*$1.id,
				      $1.pattern->dup(), 0, False,
				      yyfilename, yylinenumber);
				  }
				  $$ = $1;
				}

global_definition	:	global_header function_body
				{ 
				  if ($1.pattern)
				  {
				    // Funktion definieren
				    vsllib->add(*$1.id, 
				      $1.pattern, $2, True,
				      *$1.file, $1.line);
				  }
				  delete $1.id;
				  delete $1.file;
				}

global_header		:	function_header ARROW
				{
				  if ($1.pattern)
				  {
				    // Funktion bereits deklarieren
				    // (fuer rekursive Aufrufe)
				    vsllib->add(*$1.id,
					$1.pattern->dup(), 0, True,
					yyfilename, yylinenumber);
				  }
				  $$ = $1;
				}

function_body		:	box_expression_with_defs
				{ $$ = $1; }



/*** expressions ***/

/*** LET, WHERE ***/

box_expression_with_defs:	box_expression_with_wheres
				{ $$ = $1; }
			|	LET var_definition in_box_expression
				{ 
				  $$ = ($2.pattern && $2.args && $3) ?
				    new LetNode($2.pattern, $2.args, $3) : 0;
				}

in_box_expression	:	IN box_expression_with_defs
				{ $$ = $2; }
			|	',' var_definition in_box_expression
				{ 
				  $$ = ($2.pattern && $2.args && $3) ?
				    new LetNode($2.pattern, $2.args, $3) : 0;
				}

box_expression_with_wheres:	box_expression
				{ $$ = $1; }
			|	box_expression_with_where
				{ $$ = $1; }

box_expression_with_where:	box_expression_with_wheres 
				WHERE var_definition
				{
				  $$ = ($3.pattern && $3.args && $1) ?
				    new WhereNode($3.pattern, $3.args, $1) : 0;
				}
			|	box_expression_with_where 
				',' var_definition
				{
				  $$ = ($3.pattern && $3.args && $1) ?
				    new WhereNode($3.pattern, $3.args, $1) : 0;
				}

var_definition		:	box_expression EQ box_expression
				{
				  $$.pattern = $1;
				  $$.args    = $3;
				}


/*** basic expresions ***/

box_expression		:	'(' box_expression_with_defs ')'
				{ $$ = $2; }
			|	list_expression
				{ $$ = $1; }
			|	const_expression
				{ $$ = $1; }
			|	binary_expression
				{ $$ = $1; }
			|	unary_expression
				{ $$ = $1; }
			|	cond_expression
				{ $$ = $1; }
			|	function_call
				{ $$ = $1; }
			|	argument_or_function
				{ $$ = $1; }

list_expression		:	'[' ']'
				{ $$ = new EmptyListNode; }
			|	'[' box_expression_list ']'
				{ $$ = $2; }
			|	'(' ')'
				{ $$ = new EmptyListNode; }
			|	'(' multiple_box_expression_list ')'
				{ $$ = $2; }

box_expression_list	:	box_expression_with_defs
				{ 
				  $$ = ($1) ? new FixListNode($1) : 0;
				}
			|	multiple_box_expression_list
				{ 
				  $$ = $1; 
				}

multiple_box_expression_list:	box_expression APPEND box_expression
				{ 
				  $$ = ($1 && $3) ? new ListNode($1, $3) : 0;
				}
			|	box_expression ',' box_expression_list
				{
				  $$ = ($1 && $3) ? new ListNode($1, $3) : 0;
				}
			|	box_expression THREEDOTS
				{ 
				  $$ = $1; 
				}
			|	THREEDOTS
				{
				  $$ = new NameNode("...");
				}

const_expression	:	string_constant
				{ 
				  // bug workaround
				  char *buf = (char *)*$1;
				  string name = buf;
				  $$ = new StringNode(name);
				  delete $1;
				}
			|	numeric_constant
				{ $$ = new NumNode($1); }

string_constant		:	STRING
				{ $$ = new string(unquote((char *)yytext)); }
			|	string_constant STRING
				{ 
				  $$ = $1;
				  *$$ += unquote((char *)yytext);
				}

numeric_constant	:	INTEGER
				{ $$ = atoi((char *)yytext); }

function_call		:	function_identifier function_argument
				{
				  $$ = ($2) ? 
				    vsllib->_call(*$1, $2) : 0;
				}

unary_expression	:	NOT box_expression
				{ 
				  $$ = vsllib->call("(not)", $2); 
				}
			|	'+' box_expression
				{ $$ = $2; }
			|	'-' box_expression
				{
				  // -x durch 0-x simulieren
				  $$ = ($2) ? vsllib->call("(-)", 
					new NullNode, $2) : 0;
				}

/*** operators ***/

binary_expression	:	box_expression EQ box_expression
				{ 
				  $$ = vsllib->call("(=)", $1, $3); 
				}
			|	box_expression NE box_expression
				{ 
				  $$ = vsllib->call("(<>)", $1, $3); 
				}
			|	box_expression GT box_expression
				{ 
				  $$ = vsllib->call("(>)", $1, $3); 
				}
			|	box_expression GE box_expression
				{ 
				  $$ = vsllib->call("(>=)", $1, $3); 
				}
			|	box_expression LT box_expression
				{ 
				  $$ = vsllib->call("(<)", $1, $3); 
				}
			|	box_expression LE box_expression
				{ 
				  $$ = vsllib->call("(<=)", $1, $3); 
				}
			|	box_expression HALIGN box_expression
				{ 
				  $$ = vsllib->call("(&)", $1, $3); 
				}
			|	box_expression VALIGN box_expression
				{ 
				  $$ = vsllib->call("(|)", $1, $3); 
				}
			|	box_expression UALIGN box_expression
				{ 
				  $$ = vsllib->call("(^)", $1, $3); 
				}
			|	box_expression TALIGN box_expression
				{ 
				  $$ = vsllib->call("(~)", $1, $3); 
				}
			|	box_expression '+' box_expression
				{ 
				  $$ = vsllib->call("(+)", $1, $3); 
				}
			|	box_expression '-' box_expression
				{ 
				  $$ = vsllib->call("(-)", $1, $3); 
				}
			|	box_expression '*' box_expression
				{ 
				  $$ = vsllib->call("(*)", $1, $3); 
				}
			|	box_expression '/' box_expression
				{ 
				  $$ = vsllib->call("(/)", $1, $3); 
				}
			|	box_expression '%' box_expression
				{ 
				  $$ = vsllib->call("(%)", $1, $3); 
				}
			|	box_expression CONS box_expression
				{ 
				  $$ = vsllib->call("(::)", $1, $3); 
				}
			|	box_expression OR box_expression
				{ 
				  // if expr-1 then true else expr-2
				  $$ = ($1 && $3) ? 
			 	    new TestNode($1, new TrueNode, $3) : 0;
				}
			|	box_expression AND box_expression
				{ 
				  // if expr-1 then expr-2 else false
				  $$ = ($1 && $3) ? 
				    new TestNode($1, $3, new FalseNode) : 0;
				}

cond_expression		:	IF box_expression
				THEN box_expression_with_defs
				else_expression
				FI
				{ 
				  $$ = ($2 && $4 && $5) ?
					new TestNode($2, $4, $5) : 0;
				}

else_expression		:	ELSIF box_expression
				THEN box_expression_with_defs
				else_expression
				{ 
				  $$ = ($2 && $4 && $5) ?
					new TestNode($2, $4, $5) : 0;
				}
			|	ELSE box_expression_with_defs
				{ $$ = $2; }

function_argument	:	list_expression
				{ $$ = $1; }
			|	'(' box_expression_with_defs ')'
				{ $$ = ($2) ? new FixListNode($2) : 0; }

argument_or_function	:	identifier
				{
				  if (*$1 == "_")
				    $$ = new DummyNode;
				  else
				  {
				    // Wenn Funktion definiert, diese nehmen;
				    // sonst Platzhalter Variable bilden.

				    if (vsllib->deflist(string("#") + *$1))
				      $$ = vsllib->call(string("#") + *$1);
				    else
				      $$ = new NameNode(*$1);
				  }

				  delete $1;
				}


/*** directives ***/

override_declaration	:	OVERRIDE override_list

override_list		:	override_identifier
			|	override_list ',' override_identifier

override_identifier	:	function_identifier
				{
				  string func_name = *$1;
				  if (vsllib->override(func_name)
				    && vsllib->override(
				      string("#") + func_name))
				    VSLLib::parse_error(string("'") + func_name + 
				      "(...)' undefined");
				}

replace_declaration	:	REPLACE replace_list

replace_list		:	replace_identifier
			|	replace_list ',' replace_identifier

replace_identifier	:	function_identifier
				{
				  string func_name = *$1;
				  if (vsllib->replace(func_name)
				    && vsllib->replace(
				      string("#") + func_name))
				    VSLLib::parse_error(string("'") + func_name + 
				      "(...)' undefined");
				}

%% /* DO NOT REMOVE THIS COMMENT -- MUNCH-YACC DEPENDS ON IT */
