/* $Id$ -*- C++ -*- */
/* VSL grammar */

%{

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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


// Some declarations and utilities

char VSL_yacc_rcsid[] = 
    "$Id$";


// Function calls

// Return appropriate node for function call
VSLNode *VSLLib::_call(const string& func_name, VSLNode *arg)
{
    // Find definition list
    VSLDefList* def = deflist(func_name);
    if (def == 0)
    {
	VSLLib::parse_error("'" + func_name + "(...)' undefined");
	delete arg;
	arg = 0;
    }

    // Return calling node
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
				    ASSERT($1.pattern == 0 || 
					   $1.pattern->OK());

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
				    ASSERT($1->OK());
				    ASSERT($2 == 0 || $2->OK());

				    $$.id      = $1;
				    $$.pattern = $2;
				    $$.file    = new string(vslfilename);
				    $$.line    = vsllinenumber;
				}
			|	function_identifier
				{
				    ASSERT($1->OK());

				    $$.id      = new string("#" + *$1);
				    $$.pattern = new EmptyListNode;
				    $$.file    = new string(vslfilename);
				    $$.line    = vsllinenumber;

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
				{ $$ = new string((char *)vsltext); }

function_definition	:	local_definition
			|	global_definition

local_definition	: 	local_header function_body
				{ 
				    ASSERT($1.pattern == 0 || 
					   $1.pattern->OK());
				    ASSERT($2 == 0 || $2->OK());

				    if ($1.pattern)
				    {
					// Define function
					vsllib->add(*$1.id,
						    $1.pattern, $2, False,
						    *$1.file, $1.line);
				    }

				    delete $1.id;
				    delete $1.file;
				}

local_header		:	function_header EQ
				{
				    ASSERT($1.pattern == 0 || 
					   $1.pattern->OK());

				    if ($1.pattern)
				    {
					// Declare function now
					// (for recursive calls)
					vsllib->add(*$1.id,
						    $1.pattern->dup(), 0, 
						    False,
						    vslfilename, 
						    vsllinenumber);
				    }

				    $$ = $1;
				}

global_definition	:	global_header function_body
				{ 
				    ASSERT($1.pattern == 0 || 
					   $1.pattern->OK());
				    ASSERT($2 == 0 || $2->OK());

				    if ($1.pattern)
				    {
					// Define function
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
				      ASSERT($1.pattern->OK());

				      // Declare function now
				      // (for recursive calls)
				      vsllib->add(*$1.id,
						  $1.pattern->dup(), 0, True,
						  vslfilename, vsllinenumber);
				  }
				  $$ = $1;
				}

function_body		:	box_expression_with_defs
				{ $$ = $1; }



/*** expressions ***/

/*** LET, WHERE ***/

box_expression_with_defs:	box_expression_with_wheres
				{
				    $$ = $1;
				}
			|	LET var_definition in_box_expression
				{ 
				    ASSERT($2.pattern == 0 || 
					   $2.pattern->OK());
				    ASSERT($2.args == 0 || 
					   $2.args->OK());
				    ASSERT($3 == 0 || $3->OK());

				    $$ = ($2.pattern && $2.args && $3) ?
					new LetNode($2.pattern, $2.args, $3) :
					0;
				    ASSERT($$ == 0 || $$->OK());
				}

in_box_expression	:	IN box_expression_with_defs
				{
				    $$ = $2;
				}
			|	',' var_definition in_box_expression
				{ 
				    ASSERT($2.pattern == 0 || 
					   $2.pattern->OK());
				    ASSERT($2.args == 0 || 
					   $2.args->OK());
				    ASSERT($3 == 0 || $3->OK());

				    $$ = ($2.pattern && $2.args && $3) ?
					new LetNode($2.pattern, $2.args, $3) :
					0;

				    ASSERT($$ == 0 || $$->OK());
				}

box_expression_with_wheres:	box_expression
				{ $$ = $1; }
			|	box_expression_with_where
				{ $$ = $1; }

box_expression_with_where:	box_expression_with_wheres 
				WHERE var_definition
				{
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3.pattern == 0 || 
					   $3.pattern->OK());
				    ASSERT($3.args == 0 || 
					   $3.args->OK());

				    $$ = ($3.pattern && $3.args && $1) ?
					new WhereNode($3.pattern, $3.args, $1):
					0;
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression_with_where 
				',' var_definition
				{
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3.pattern == 0 || 
					   $3.pattern->OK());
				    ASSERT($3.args == 0 || 
					   $3.args->OK());

				    $$ = ($3.pattern && $3.args && $1) ?
					new WhereNode($3.pattern, $3.args, $1):
					0;

				    ASSERT($$ == 0 || $$->OK());
				}

var_definition		:	box_expression EQ box_expression
				{
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());

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
				{
				    $$ = new EmptyListNode;
				    ASSERT($$->OK());
				}
			|	'[' box_expression_list ']'
				{
				    $$ = $2;
				    ASSERT($$ == 0 || $$->OK());
				}
			|	'(' ')'
				{
				    $$ = new EmptyListNode;
				    ASSERT($$->OK());
				}
			|	'(' multiple_box_expression_list ')'
				{
				    $$ = $2;
				    ASSERT($$ == 0 || $$->OK());
				}

box_expression_list	:	box_expression_with_defs
				{ 
				    ASSERT($1 == 0 || $1->OK());

				    $$ = ($1) ? new FixListNode($1) : 0;
				    ASSERT($$ == 0 || $$->OK());
				}
			|	multiple_box_expression_list
				{ 
				    $$ = $1; 
				}

multiple_box_expression_list:	box_expression APPEND box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());

				    $$ = ($1 && $3) ? new ListNode($1, $3) : 0;
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression ',' box_expression_list
				{
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());

				    $$ = ($1 && $3) ? new ListNode($1, $3) : 0;
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression THREEDOTS
				{ 
				    $$ = $1; 
				}
			|	THREEDOTS
				{
				    $$ = new NameNode("...");
				    ASSERT($$ == 0 || $$->OK());
				}

const_expression	:	string_constant
				{ 
				    ASSERT($1->OK());

				    // Bug workaround
				    char *buf = (char *)*$1;
				    string name = buf;
				    $$ = new StringNode(name);
				    delete $1;

				    ASSERT($$->OK());
				}
			|	numeric_constant
				{ 
				    $$ = new NumNode($1);
				    ASSERT($$->OK());
				}

string_constant		:	STRING
				{
				    $$ = new string(unquote((char *)vsltext));
				    ASSERT($$->OK());
				}
			|	string_constant STRING
				{ 
				    ASSERT($1->OK());

				    $$ = $1;
				    *$$ += unquote((char *)vsltext);

				    ASSERT($$->OK());
				}

numeric_constant	:	INTEGER
				{
				    $$ = atoi((char *)vsltext);
				}

function_call		:	function_identifier function_argument
				{
				    ASSERT($1->OK());
				    ASSERT($2 == 0 || $2->OK());

				    $$ = ($2) ? 
					vsllib->_call(*$1, $2) : 0;
				    ASSERT($$ == 0 || $$->OK());
				}

unary_expression	:	NOT box_expression
				{
				    ASSERT($2 == 0 || $2->OK());
				    $$ = vsllib->call("(not)", $2);
				    ASSERT($$ == 0 || $$->OK());
				}
			|	'+' box_expression
				{
				    $$ = $2;
				}
			|	'-' box_expression
				{
				    // Simulate `-X' by `0-X'
				    ASSERT($2 == 0 || $2->OK());
				    $$ = ($2) ? 
					vsllib->call("(-)", new NullNode, $2) :
					0;
				    ASSERT($$ == 0 || $$->OK());
				}

/*** operators ***/

binary_expression	:	box_expression EQ box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(=)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression NE box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(<>)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression GT box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(>)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression GE box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(>=)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression LT box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(<)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression LE box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(<=)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression HALIGN box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(&)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression VALIGN box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(|)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression UALIGN box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(^)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression TALIGN box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(~)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression '+' box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(+)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression '-' box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(-)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression '*' box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(*)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression '/' box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(/)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression '%' box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(%)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression CONS box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    $$ = vsllib->call("(::)", $1, $3); 
				    ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression OR box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    // If EXPR-1 then true else EXPR-2
				    $$ = ($1 && $3) ? 
					new TestNode($1, new TrueNode, $3) : 
					0;
				  ASSERT($$ == 0 || $$->OK());
				}
			|	box_expression AND box_expression
				{ 
				    ASSERT($1 == 0 || $1->OK());
				    ASSERT($3 == 0 || $3->OK());
				    // If EXPR-1 then EXPR-2 else false
				    $$ = ($1 && $3) ? 
					new TestNode($1, $3, new FalseNode) : 
					0;
				    ASSERT($$ == 0 || $$->OK());
				}

cond_expression		:	IF box_expression
				THEN box_expression_with_defs
				else_expression
				FI
				{ 
				    ASSERT($2 == 0 || $2->OK());
				    ASSERT($4 == 0 || $4->OK());
				    ASSERT($5 == 0 || $5->OK());

				    $$ = ($2 && $4 && $5) ?
					new TestNode($2, $4, $5) : 0;
				    ASSERT($$ == 0 || $$->OK());
				}

else_expression		:	ELSIF box_expression
				THEN box_expression_with_defs
				else_expression
				{ 
				    ASSERT($2 == 0 || $2->OK());
				    ASSERT($4 == 0 || $4->OK());
				    ASSERT($5 == 0 || $5->OK());

				    $$ = ($2 && $4 && $5) ?
					new TestNode($2, $4, $5) : 0;
				    ASSERT($$ == 0 || $$->OK());
				}
			|	ELSE box_expression_with_defs
				{
				    $$ = $2;
				}

function_argument	:	list_expression
				{
				    $$ = $1;
				}
			|	'(' box_expression_with_defs ')'
				{ 
				    ASSERT($2 == 0 || $2->OK());

				    $$ = ($2) ? new FixListNode($2) : 0; 
				    ASSERT($$ == 0 || $$->OK());
				}

argument_or_function	:	identifier
				{
				    ASSERT($1->OK());

				    if (*$1 == "_")
				    {
					$$ = new DummyNode;
				    }
				    else
				    {
					// If function is declared, use it;
					// otherwise create placeholder var
					
					if (vsllib->deflist("#" + *$1))
					    $$ = vsllib->call("#" + *$1);
					else
					    $$ = new NameNode(*$1);
				    }

				    ASSERT($$->OK());
				    delete $1;
				}


/*** directives ***/

override_declaration	:	OVERRIDE override_list

override_list		:	override_identifier
			|	override_list ',' override_identifier

override_identifier	:	function_identifier
				{
				    ASSERT($1->OK());

				    string func_name = *$1;
				    if (vsllib->override(func_name)
					&& vsllib->override("#" + func_name))
				    {
					VSLLib::parse_error("'" + func_name + 
							    "(...)'"
							    " undefined");
				    }
				}

replace_declaration	:	REPLACE replace_list

replace_list		:	replace_identifier
			|	replace_list ',' replace_identifier

replace_identifier	:	function_identifier
				{
				    ASSERT($1->OK());

				    string func_name = *$1;
				    if (vsllib->replace(func_name)
					&& vsllib->replace("#" + func_name))
				    {
					VSLLib::parse_error("'" + func_name + 
							    "(...)'"
							    " undefined");
				    }
				}

%% /* DO NOT REMOVE THIS COMMENT -- MUNCH-YACC DEPENDS ON IT */
