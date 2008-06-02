// $Id$ -*- C++ -*-
// Tic-Tac-Toe Game

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

// Based on a Java program by Scott L. Patterson <scott@uniqsys.com>.
// See http://www.uniqsys.com/~scpatt/java/TicTacToe/ for details.

char tictactoe_rcsid[] = 
    "$Id$";

#include "tictactoe.h"

#include "icons/tictactoe/circle.xbm"
#include "icons/tictactoe/cross.xbm"
#include "icons/tictactoe/empty.xbm"

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/SelectioB.h>	// XmCreatePromptDialog()

#include <iostream>
#include <unistd.h>

#include "Command.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "InitImage.h"
#include "LessTifH.h"
#include "TimeOut.h"
#include "assert.h"
#include "bool.h"
#include "cook.h"
#include "verify.h"
#include "wm.h"
#include "status.h"
#include "version.h"


// Board is: 1 2 3
//           4 5 6
//           7 8 9

static int board[10];
static bool winning[10];
static Widget buttons[10];

#define NO_ONE 0
#define PLAYER1 1
#define PLAYER2 4
#define CATSEYE -1

// Search the rows for a near win
static bool moveRow(int current_player)
{
    for (int x = 0; x < 3; x++)
    {
	int sum = 0;
	for (int y = 1; y < 4; y++)
	    sum += board[(x * 3) + y];

	// Almost a win -- block it
	if (sum == (current_player * 2))
	    for (int y = 1; y < 4; y++) 
		if (board[(x * 3) + y] == NO_ONE) 
		{
		    board[(x * 3) + y] = PLAYER2;
		    return true;
                }
    }
    return false;
}

// Search the columns for a near win
static bool moveColumn(int current_player)
{
    for (int x = 1; x < 4; x++)
    {
	int sum = 0;
	for (int y = 0; y < 3; y++)
	    sum += board[x+(y*3)];

	// Almost a win -- block it
	if (sum == (current_player * 2))
	    for (int y = 0; y < 3; y++)
		if (board[x + (y * 3)] == NO_ONE)
		{
		    board[x + (y * 3)] = PLAYER2;
		    return true;
		}
    }
    return false;
}

// Search the diagonals for a near win
static bool moveDiagonal(int current_player) 
{
    static const int p1[3] = {1, 5, 9};
    static const int p2[3] = {3, 5, 7};

    int sum = 0;

    int i;
    for (i = 0; i < 3; i++)
	sum += board[p1[i]];

    if (sum == (current_player*2))
	for (i = 0; i < 3; i++)
	    if (board[p1[i]] == NO_ONE)
	    {
		board[p1[i]] = PLAYER2;
		return true;
	    }

    sum = 0;
    for (i = 0; i < 3; i++)
	sum += board[p2[i]];

    if (sum == (current_player * 2))
	for (i = 0; i < 3; i++)
	    if (board[p2[i]] == NO_ONE)
	    {
		board[p2[i]] = PLAYER2;
		return true;
	    }
    return false;
}

// Computer AI
// 1. Search for possible wins for me
// 2. Search for possible wins for opponent
// 3. Perform most strategic move
static void autoMove()
{
    // Can I win
    if (moveRow(PLAYER2)) 
	return;
    if (moveColumn(PLAYER2)) 
	return;
    if (moveDiagonal(PLAYER2)) 
	return;

    // Can he/she win
    if (moveRow(PLAYER1)) 
	return;
    if (moveColumn(PLAYER1)) 
	return;
    if (moveDiagonal(PLAYER1)) 
	return;

    // Most strategic move
    if (board[5] == NO_ONE)
    {
	board[5] = PLAYER2;
	return;
    }

    // Very Special cases
    if ((board[5] == PLAYER2) && (board[9] == NO_ONE) &&
	(board[8] == PLAYER1) && (board[3] == PLAYER1)) 
    {
	board[9] = PLAYER2;
	return;
    }

    if ((board[5] == PLAYER2) && (board[9] == NO_ONE) &&
	(board[7] == PLAYER1) && (board[6] == PLAYER1)) 
    {
	board[9] = PLAYER2;
	return;
    }

    if ((board[5] == PLAYER2) && (board[7] == NO_ONE) &&
	(board[8] == PLAYER1) && (board[1] == PLAYER1))
    {
	board[7] = PLAYER2;
	return;     
    }

    // Special cases
    int *r = 0;
    if ((board[5] == PLAYER2) && 
	(((board[1] == PLAYER1) && (board[9] == PLAYER1)) ||
	 ((board[3] == PLAYER1) && (board[7] == PLAYER1))))
    {
	static int rtmp[9] = {5, 2, 4, 6, 8, 1, 3, 7, 9};
	r = rtmp;
    }
    else if ((board[5] == PLAYER2) &&
	     (board[2] == PLAYER1) && (board[4] == PLAYER1))
    {
	static int rtmp[9] = {5, 1, 4, 6, 8, 2, 3, 7, 9};
	r = rtmp;
    }
    else if ((board[5] == PLAYER2) &&
	     (board[2] == PLAYER1) && (board[6] == PLAYER1))
    {
	static int rtmp[9] = {5, 3, 4, 6, 8, 1, 2, 7, 9};
	r = rtmp;
    } 
    else if ((board[5] == PLAYER2) &&
	     (board[8] == PLAYER1) && (board[4] == PLAYER1))
    {
	static int rtmp[9] = {5, 7, 4, 6, 8, 1, 3, 2, 9};
	r = rtmp;
    }
    else if ((board[5] == PLAYER2) &&
	     (board[8] == PLAYER1) && (board[6] == PLAYER1))
    {
	static int rtmp[9] = {5, 9, 4, 6, 8, 1, 3, 7, 2};
	r = rtmp;
    }
    else 
    {
	static int tics = 0;
	if (++tics % 10 == 0)
	{
	    // In one out of 10 cases, give the user a chance.
	    static int rtmp[9] = {5, 2, 4, 6, 8, 1, 3, 7, 9};
	    r = rtmp;
	}
	else
	{
	    static int rtmp[9] = {5, 1, 3, 7, 9, 2, 4, 6, 8};
	    r = rtmp;
	}
    }

    for (int i = 0; i < 9; i++)
	if (board[r[i]] == NO_ONE) {
	    board[r[i]] = PLAYER2;
	    return;
	}
}

static int winner() 
{
    int i;
    for (i = 1; i < 10; i++)
	winning[i] = false;

    // Check for diagonal win
    int sum;

    sum = board[3] + board[5] + board[7];
    if (sum == 3 || sum == 12)
    {
	winning[3] = winning[5] = winning[7] = true;
	return sum == 3 ? PLAYER1 : PLAYER2;
    }

    sum = board[1] + board[5] + board[9];
    if (sum == 3 || sum == 12)
    {
	winning[1] = winning[5] = winning[9] = true;
	return sum == 3 ? PLAYER1 : PLAYER2;
    }

    // Search the columns for a win
    int x;
    for (x = 1; x < 4; x++) 
    {
	sum = 0;
	for (int y = 0; y < 3; y++)
	    sum += board[x + (y * 3)];

	if (sum == 3 || sum == 12)
	{
	    for (int y = 0; y < 3; y++)
		winning[x + (y * 3)] = true;
	    
	    return sum == 3 ? PLAYER1 : PLAYER2;
	}
    }

    // Search the rows for a near win
    for (x = 0; x < 3; x++)
    {
	sum = 0;
	for (int y = 1; y < 4; y++)
	    sum += board[(x * 3) + y];

	if (sum == 3 || sum == 12)
	{
	    for (int y = 1; y < 4; y++)
		winning[(x * 3) + y] = true;
	    
	    return sum == 3 ? PLAYER1 : PLAYER2;
	}
    }

    // Check for all squares filled
    for (i = 1; i < 10; i++)
	if (board[i] == NO_ONE)
	    return NO_ONE;

    return CATSEYE;
}

static void initBoard() 
{
    for (int i = 1; i < 10; i++)
	board[i] = NO_ONE;
}

// Convert NAME into a color, using PIX as default
static Pixel color(Widget w, const _XtString name, Pixel pixel)
{
    XrmValue from, to;
    from.size = strlen(name);
    from.addr = CONST_CAST(char *,name);
    to.size   = sizeof(pixel);
    to.addr   = (String)&pixel;

    XtConvertAndStore(w, XtRString, &from, XtRPixel, &to);
    return pixel;
}

static void repaint()
{
    int win = winner();

    for (int i = 1; i <= 9; i++)
    {
	const char *name = 0;
	switch (board[i])
	{
	case NO_ONE:
	    name = "empty";
	    break;

	case PLAYER1:
	    name = "circle";
	    break;

	case PLAYER2:
	    name = "cross";
	    break;
	}

	assert(name != 0);

	Pixel foreground;
	Pixel background;
	XtVaGetValues(buttons[i], 
		      XmNforeground, &foreground,
		      XmNbackground, &background, 
		      XtPointer(0));

	if (win == NO_ONE || winning[i])
	{
	    if (board[i] == PLAYER1)
		foreground = color(buttons[i], "red4",
				   BlackPixelOfScreen(XtScreen(buttons[i])));
	    else
		foreground = BlackPixelOfScreen(XtScreen(buttons[i]));
	}

	Pixmap p = XmGetPixmap(XtScreen(buttons[i]), XMST(name), 
			       foreground, background);
	XtVaSetValues(buttons[i],
		      XmNlabelType, XmPIXMAP,
		      XmNlabelPixmap, p,
		      XmNlabelInsensitivePixmap, p,
		      XtPointer(0));

	XtSetSensitive(buttons[i], win == NO_ONE && board[i] == NO_ONE);
    }

    switch (win)
    {
    case PLAYER1:
	set_status("You win.");
	break;

    case PLAYER2:
	set_status(DDD_NAME " wins.");
	break;

    case CATSEYE:
	set_status("Cat's eye.");
	break;
    }
}

static void MoveCB(XtPointer client_data, XtIntervalId *id)
{
    (void) id;			// Use it

    XtIntervalId *timer = (XtIntervalId *)client_data;
    assert(*timer == *id);
    *timer = 0;

    autoMove();
    repaint();
}

static const int THINKING_TIME = 0; // `Thinking' time in ms

static void make_move(int move)
{
    static XtIntervalId timer = 0;

    // Is it a valid move?
    if ((move < 1) || (move > 9) || (board[move] != NO_ONE))
	return;

    // Are we still thinking?
    if (timer != 0)
	return;

    board[move] = PLAYER1;
    repaint();

    if (winner() == NO_ONE)
    {
	for (int i = 1; i < 10; i++)
	    XtSetSensitive(buttons[i], False);

	// Make a move in THINKING_TIME ms
	timer = XtAppAddTimeOut(XtWidgetToApplicationContext(buttons[move]),
				THINKING_TIME, MoveCB, XtPointer(&timer));
    }
}

// Install the given X bitmap as NAME
static void InstallBitmapAsImage(unsigned char *bits, int width, int height, 
				 const string& name)
{
    Boolean ok = InstallBitmap(bits, width, height, name.chars());
    if (!ok)
	std::cerr << "Could not install " << quote(name) << " bitmap\n";
}

static void install_images()
{
    static bool installed = false;
    if (installed)
	return;

    InstallBitmapAsImage(cross_bits,  cross_width,  cross_height,  "cross");
    InstallBitmapAsImage(circle_bits, circle_width, circle_height, "circle");
    InstallBitmapAsImage(empty_bits,  empty_width,  empty_height,  "empty");

    installed = true;
}

static void ResetTicTacToeCB(Widget, XtPointer, XtPointer)
{
    static int tics = 0;

    initBoard();

    if (tics++ % 2 == 0)
	autoMove();

    repaint();

    set_status("Welcome to Tic Tac Toe!");
}

static void MakeMoveCB(Widget, XtPointer client_data, XtPointer)
{
    if (winner() != NO_ONE)
	initBoard();

    make_move((int)(long)client_data);
}

static Widget create_tictactoe(Widget parent)
{
    static Widget board = 0;
    if (board != 0)
	return board;

    install_images();

    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNorientation, XmHORIZONTAL);  arg++;
    XtSetArg(args[arg], XmNpacking,     XmPACK_COLUMN); arg++;
    XtSetArg(args[arg], XmNnumColumns,  3);             arg++;
    board = XmCreateRowColumn(parent, XMST("board"), args, arg);

    for (int i = 1; i <= 9; i++)
    {
	arg = 0;
	buttons[i] = XmCreatePushButton(board, XMST("field"), args, arg);
	XtManageChild(buttons[i]);
	XtAddCallback(buttons[i], XmNactivateCallback, 
		      MakeMoveCB, XtPointer(i));
    }
    XtManageChild(board);

    return board;
}

void TicTacToeCB(Widget, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
	dialog = verify(XmCreatePromptDialog(find_shell(),
					     XMST("tictactoe"), args, arg));
	Delay::register_shell(dialog);

	if (lesstif_version <= 79)
	    XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
						   XmDIALOG_APPLY_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_HELP_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_TEXT));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_SELECTION_LABEL));

	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
	XtAddCallback(dialog, XmNokCallback, ResetTicTacToeCB, XtPointer(0));
	XtAddCallback(dialog, XmNcancelCallback, UnmanageThisCB, 
		      XtPointer(dialog));

	create_tictactoe(dialog);
    }

    ResetTicTacToeCB(0, 0, 0);
    manage_and_raise(dialog);
}
