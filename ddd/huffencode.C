// $Id$ -*- C++ -*-
// Huffman-encode standard input

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char huffencode_rcsid[] = 
    "$Id$";

#include <limits.h>
#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>

#include "assert.h"
#include "strclass.h"
#include "cook.h"
#include "DynArray.h"
#include "bool.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

struct HuffNode {
    bool isleaf;
    union {
	struct {		// Internal node:
	    HuffNode *left;	// Left child   (0)
	    HuffNode *right;	// Right child  (1)
	} i;
	struct {		// Leaf node:
	    char c;		// Character
	} l;
    };
    int sum;			// Sum of the weights in the leaves
    HuffNode *next;		// For queue usage
    int number;			// For print usage

    HuffNode(HuffNode *l, HuffNode *r)
	: isleaf(false), sum(l->sum + r->sum), next(0)
    {
	i.left  = l;
	i.right = r;
    }
    HuffNode(char c, int s)
	: isleaf(true), sum(s), next(0)
    {
	l.c = c;
    }
};

void insert(HuffNode*& queue, HuffNode *node)
{
    if (queue == 0)
    {
	queue = node;
    }
    else if (queue->sum > node->sum)
    {
	node->next = queue;
	queue = node;
    }
    else
    {
	insert(queue->next, node);
    }
}

HuffNode *extract_min(HuffNode*& queue)
{
    HuffNode *node = queue;
    queue = queue->next;
    return node;
}

HuffNode *initial_queue(const string& s)
{
    int occurrences[CHAR_MAX - CHAR_MIN];

    char c;
    for (c = CHAR_MIN; c < CHAR_MAX; c++)
	occurrences[c - CHAR_MIN] = 0;
    for (int i = 0; i < int(s.length()); i++)
	occurrences[s[i] - CHAR_MIN]++;

    HuffNode *queue = 0;
    for (c = CHAR_MIN; c < CHAR_MAX; c++)
	if (occurrences[c - CHAR_MIN] > 0)
	    insert(queue, new HuffNode(c, occurrences[c - CHAR_MIN]));

    return queue;
}

int length(HuffNode *queue)
{
    int len = 0;
    while (queue != 0)
    {
	len++;
	queue = queue->next;
    }

    return len;
}

HuffNode *huffman(const string& s)
{
    HuffNode *queue = initial_queue(s);
    int n = length(queue);

    for (int i = 0; i < n - 1; i++)
    {
	HuffNode *x = extract_min(queue);
	HuffNode *y = extract_min(queue);
	insert(queue, new HuffNode(x, y));
    }

    return extract_min(queue);
}

void write_huffman(HuffNode *tree)
{
    static int tics = 0;

    if (tree->isleaf)
    {
	tree->number = tics++;
	cout << "const HuffCode _huff" << tree->number 
	     << " = { '" << cook(tree->l.c) << "', 0, 0 };\n";
    }
    else
    {
	write_huffman(tree->i.left);
	write_huffman(tree->i.right);

	tree->number = tics++;
	cout << "const HuffCode _huff" << tree->number 
	     << " = { '\\0', &_huff" << tree->i.left->number 
	     << ", &_huff" << tree->i.right->number << " };\n";
    }
}

void init_codes(string codes[], HuffNode *tree, string prefix = "")
{
    if (tree->isleaf)
    {
	codes[tree->l.c] = prefix;
    }
    else
    {
	init_codes(codes, tree->i.left,  prefix + "0");
	init_codes(codes, tree->i.right, prefix + "1");
    }
}

char bits_to_byte(string bits)
{
    unsigned char c = 0;

    for (int i = 0; i < int(bits.length()); i++)
	c = (c << 1) | (bits[i] == '0' ? 0 : 1);

    // clog << "bits_to_byte(" << cook(bits) << ") = " << int(c) << "\n";

    return (char)c;
}

const int BITS_PER_CHAR = 8;

string encode(const string& text, HuffNode *tree)
{
    string codes[CHAR_MAX - CHAR_MIN];

    init_codes(codes, tree);

    int i;
    cout << "// Encoding:\n";
    for (i = 0; i < CHAR_MAX - CHAR_MIN; i++)
    {
	if (codes[i] != "")
	    cout << "// '" << cook(char(i))
		 << "'\t" << codes[i] << "\n";
    }

    string bit_encoding;
    for (i = 0; i < int(text.length()); i++)
	bit_encoding += codes[text[i]];

    // cout << "\n// " << bit_encoding << "\n";

    string byte_encoding;
    for (i = 0; i < int(bit_encoding.length()) - BITS_PER_CHAR; 
	 i += BITS_PER_CHAR)
    {
	byte_encoding +=
	    bits_to_byte(bit_encoding.at(i, BITS_PER_CHAR));
    }
    byte_encoding += bits_to_byte(bit_encoding.from(i));

    return byte_encoding;
}

void write_encoding(const string& byte_encoding)
{
    cout << "const char hufftext[" 
	 << byte_encoding.length() + 1 << "] = \n\"";

    int p = 0;

    while (p < int(byte_encoding.length()))
    {
	if (p > 0 && p % 16 == 0)
	    cout << "\"\n\"";

	cout << "\\" << oct << setw(3) 
	     << setfill('0') << int((unsigned char)byte_encoding[p]);

	p++;
    }

    cout << "\";\n";
}    

void read_input(string& s)
{
    char c;

    while (cin.get(c))
	s += c;
}

int main()
{
    string text;

    read_input(text);
    HuffNode *tree = huffman(text);
    string encoding = encode(text, tree);

    cout << "\n";
    write_huffman(tree);
    cout << "\n";
    cout << "const HuffCode *const huffcode = &_huff" 
	 << tree->number << ";\n";
    cout << "\n";
    cout << "const int hufflength = " << text.length() << ";\n";

    write_encoding(encoding);

    return EXIT_SUCCESS;
}
