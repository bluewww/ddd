// $Id$ -*- C++ -*-
// Graph layout functions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Christian Lindig (lindig@ips.cs.tu-bs.de).
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

char layout_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "layout.h"
#include "assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/StringDefs.h>


// This is an implementation of the Sugiyama/Misue graph layout
// algorithm.  For details, see
// 
// @Article{sugiyama/misue/visualization,
//   author =       "Kozo Sugiyama and Kazuo Misue",
//   title =        "Visualization of Structural Information: Automatic
//                  Drawing of Compound Digraphs",
//   journal =      "IEEE Transactions on Systems, Man, and Cybernetics",
//   year =         "1991",
//   volume =       "SMC-21",
//   number =       "4",
//   pages =        "876--892",
//   month =        "July/August",
// }



const int MINXDIST    = 20;
const int MINYDIST    = 20;
const int XITERATIONS = 6;
const int REVERSE     = FALSE;

/*
 * PULLUP
 * if TRUE each node will be placed on the highest level possible. 
 * Otherwise each node will be placed on the level determined 
 * by a topological sort.
 */
const int PULLUP     = FALSE;
const int NOPULLUP   = FALSE;

const int HINTPRIO   = 100;

const int NOLEVEL    = -1;
const int NOPOSITION = -1;

/*
 * defaults for handling of strings: each node can carry an assosiated
 * string
 */

const int STR_STORE = 10 * 1024;   /* 10k - hope this is enough */


/*
 * definitions for return values
 */

const int NO_ERROR      = 0;
const int MEMORY_ERROR  = 1;

const int EMPTY_LIST    = 2;
const int NOT_MEMBER    = 3;

const int NO_PREV_LEVEL = 4;
const int NO_NEXT_LEVEL = 5;
const int LOOPS         = 6;

const int CMD_LINE      = 7;

const int NODE_TYPE     = 8;
const int LEVEL_ERROR   = 9;
const int NO_EDGE       = 10;
const int INTERNAL      = 11;
const int NOT_REGULAR   = 12;

/*
 * definitions for state of Edge (NORMAL,REVERSE)
 */

const int NORMAL_EDGE = 0;
const int REVERSE_EDGE = 1;


/*****************************************************************************
    Interface layer
*****************************************************************************/

void (*Layout::node_callback)(char *, int, int) = 0;
void (*Layout::hint_callback)(char *, char *, int, int) = 0;
int  (*Layout::compare_callback)(char *, char *) = 0;

#define UP 0
#define DOWN 1

#define WARN_IF_ALREADY_PRESENT 0

// GRAPHTAB Layout::tab;
static GRAPHTAB tab;

/*
 * add_graph
 * define a new graph
 * TODO: this is a dummy - we don't distinct between different graphs yet.
 */

void Layout::add_graph (char *g)

{
	GRAPH *graph;
	graph = graphGet (&tab,g);
	if (graph) {
#if WARN_IF_ALREADY_PRESENT
		fprintf (stderr,"add-graph warning: ");
		fprintf (stderr,"graph %s exists - not added!\n", g);
#endif /* WARN_IF_ALREADY_PRESENT */
		return;
	} else {
		graphNew (&tab,g);
	}
}

/* 
 * add_node
 * N is added to G, with all node attributes set to default values.  G
 * must exist. If there is already a node called N, this action has no
 * effect.
 */

void Layout::add_node (char *g, char *node)

{ 

	NODE *nd;
	GRAPH *graph;
	ID id;

	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"add-node warning: graph %s unknown\n",g);
		return ;
	}
	id.label = node;
	/*
	 * check for dublicates
	 */
	nd = graphGetNode (graph,&id,Regular);
	if (nd) {
#if WARN_IF_ALREADY_PRESENT
		fprintf (stderr,"add_node: Warning - node already");
		fprintf (stderr,"member of the graph - not added\n");
#endif /* WARN_IF_ALREADY_PRESENT */
		return ;
	}
	/*
	 * enter node with default width and height
	 */
	nd  = graphEnterNode (graph, &id, Regular);
	nd->attr.node.w = 10 * strlen (node);
	nd->attr.node.h = 30 ;
}
	
/* 
 * add_edge
 * The edge leading from N1 to N2 is added to the graph G, with all edge
 * attributes set to default values.  G, N1 and N2 must exist.  If there
 * is already an edge (N1, N2), this action has no effect.
 * TODO: check, if edge allready exists.
 */ 

void Layout::add_edge (char *g, char *node1, char *node2)

{

	NODE *source;
	NODE *target;
	GRAPH *graph;
	ID id1;
	ID id2;

	id1.label = node1;
	id2.label = node2;

	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"add-edge warning: graph %s unknown\n",g);
		return ;
	}
	source = graphGetNode (graph, &id1, Regular);
	if (!source) {
		fprintf (stderr,"add_edge: unknown node %s\n",node1);
		exit (NOT_MEMBER);
	}
	target = graphGetNode (graph, &id2, Regular);
	if (!target) {
		fprintf (stderr,"add_edge: unknown node %s\n",node2);
		exit (NOT_MEMBER);
	}
	if (source == target) {
		/*
		 * LOOP ! Mark the node
		 */
		source->loop = 1;
	} else {
		graphInsertEdge (graph, source,target);
	}
}


/*
 * set_node_width
 * The width of N is set to W.  G and N must exist.  The old width is
 * overwritten.
 */

void Layout::set_node_width (char *g, char *node, int width)

{
	NODE *nd;
	GRAPH *graph;
	ID id;

	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"set-node-width warning: ");
		fprintf (stderr,"graph %s unknown\n",g);
		return ;
	}
	id.label = node;
	nd = graphGetNode (graph, &id, Regular);
	if (!nd) {
		fprintf (stderr,"set_node_width: node %s unknown to %s\n",
			 node, g);
		return ;
	}
	nd->attr.node.w = width;
}
	

/*
 * set_node_height
 * The height of N is set to H.  G and N must exist.  The old width is
 * overwritten.
 */

void Layout::set_node_height (char *g, char *node, int height)

{
	NODE *nd;
	GRAPH *graph;
	ID id;

	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"set-node warning: graph %s unknown\n",g);
		return ;
	}
	id.label = node;
	nd = graphGetNode (graph, &id, Regular);
	if (!nd) {
		fprintf (stderr,"set_node_width: node %s unknown to %s\n",
			 node, g);
		return ;
	}
	nd->attr.node.h = height;
}
	
/*
 * set_node_position
 * The position of N is set to (X, Y).  G and N must exist.  The old
 * position is overwritten.
 */

void Layout::set_node_position (char *g, char *node, int x, int y)

{
	NODE *nd;
	GRAPH *graph;
	ID id;

	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"set-node-position warning: ");
		fprintf (stderr,"graph %s unknown\n",g);
		return ;
	}
	id.label = node;
	nd = graphGetNode (graph, &id, Regular);
	if (!nd) {
		fprintf (stderr,"set_node_position: node %s unknown to %s\n",
			 node, g);
		return ;
	}
	nd->oldx = x;
	nd->oldy = y;
}

/*
 * add_edge_hint
 * The hint (X, Y) is added to the edge (N1, N2).  This means that the
 * edge is supposed to touch this position.  If there is already a hint
 * (X, Y), this has no effect.
 */

void Layout::add_edge_hint (char *, char *, char *, int, int)

{

}

/* 
 * remove_edge_hint The hint (X, Y) is remove from the edge (N1, N2).
 * If there is no such hint, this action has no effect.  
 */

void Layout::remove_edge_hint (char *, char *, char *, int, int)

{

}

/* 
 * remove_edge 
 * The edge (N1, N2) is removed from G.  If there is no
 * such edge, this action has no effect.  
 */

void Layout::remove_edge (char *g, char *node1, char *node2)

{
	GRAPH *graph;
	NODE *source;
	NODE *target;
	NODE *hint;
	NODE *tmp;
	EDGE *toTarget;
	EDGE *toSource;
	ID id1, id2, tmpid;
	int direction;		/* UP or DOWN */

	id1.label = node1;
	id2.label = node2;

	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"remove-edge warning: graph %s unknown\n",g);
		return ;
	}

	source = graphGetNode (graph, &id1, Regular);
	if (!source) {
		fprintf (stderr,"remove_edge: unknown node %s\n",node1);
		return;
	}
	target = graphGetNode (graph, &id2, Regular);
	if (!target) {
		fprintf (stderr,"remove_edge: unknown node %s\n",node2);
		return;
	}

	/*
	 * try to find the edge between the two nodes
	 */

	toTarget = graphFindEdgeAtSource (source,target);
	if (!toTarget) {
		fprintf (stderr,"remove_edge: can't find edge from");
		fprintf (stderr," %s to %s \n", node1, node2);
		return ;
	}
	toSource = graphFindEdgeAtTarget (source,target);
	if (!toSource) {
		fprintf (stderr,"remove_edge: can't find edge from");
		fprintf (stderr," %s to %s \n", node1, node2);
		return;
	}

	/*
	 * remove all hints
	 * start at the source node and move to the target node
	 * removing all hints on that way. We have to determine
	 * if we have to go UP ord DOWN at each hint we find, because
	 * the edge may be inverted!
	 */

	hint = toTarget->node;
	if (hint->type == Hint && hint->attr.hint.up == source) {
		direction = DOWN;
	} else {
		direction = UP;
	}
	while (hint != target) {
		if (hint->level != NOLEVEL) {
			/*
			 * remove hint form its level
			 */
			levelsRemoveNode (graph, hint, hint->level);
		}
		tmp = ( direction == DOWN ? 
		       hint->attr.hint.down : hint->attr.hint.up );
		tmpid.id = hint->attr.hint.id;
		graphRemoveNode (graph, &tmpid, Hint);
		hint = tmp;
	}

	/*
	 * remove edges
	 */
	
	listRemoveEdge (&source->attr.node.down, toTarget);
	listRemoveEdge (&target->attr.node.up, toSource);	

}

/* 
 * remove_node
 * The node N is removed from G. All edges coming
 * from or leading to N are also removed. If there is no such node,
 * this action has no effect.
 */

void Layout::remove_node (char *g, char *label)

{
	GRAPH *graph;
	NODE *node;
	EDGE *edge;
	ID id;

	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"remove-edge warning: graph %s unknown\n",g);
		return ;
	}
	id.label = label;
	node = graphGetNode (graph, &id, Regular);
	if (!node ) {
		fprintf (stderr,"remove_node: unknown node %s\n", label);
		exit (NOT_MEMBER);
	}
	if (node->level != NOLEVEL) {
		levelsRemoveNode (graph, node, node->level);
	}
	/* 
	 * remove all edges leading down ...
	 */

	edge = node->attr.node.down.head ;
	while (edge) {
		remove_edge (g,label, edge->target->attr.node.label);
		edge = edge->next;
	}
	
	/* 
	 * remove all edges leading up  ...
	 */

	edge = node->attr.node.up.head ;
	while (edge) {
		remove_edge (g,edge->target->attr.node.label, label);
		edge = edge->next;
	}
	/*
	 * remove node by itself
	 */
	graphRemoveNode (graph, &id, Regular);
}

/* 
 * remove_graph
 * The graph G is removed, including all its edges and
 * nodes.  If there is no such graph, this action has no effect.
 */

void Layout::remove_graph (char *g)

{
	graphRemove (&tab,g);
}

/* 
 * layout
 * A layout for all nodes in G is computed.  Changing node
 * positions are echoed on the standard output in the form
 * !^node-position(G, N, X, Y)  for each old (now invalid) node
 * position (X, Y) and !node-position(G, N, X, Y) for each new node
 * position (X, Y). Changing edge hints are echoed on the stan- dard
 * output in the form ^!edge-hint(G, N1, N2, X, Y) for each old (now
 * invalid) edge hint (X, Y) and !edge-hint(G, N1, N2, X, Y) for each
 * new edge hint (X, Y).
 */

void Layout::layout (char *g)

{
	GRAPH *graph;
	
	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"layout warning: graph %s unknown\n",g);
		return ;
	}

	if (graph->layouted) {
		inc_layout(graph);
	} else {
		new_layout(graph);
	}
	noraOutput (graph);

}

/*
 * debug
 */
void Layout::noraDebug (char *g)

{
	GRAPH *graph;
	
	graph = graphGet (&tab,g);
	if (!graph) {
		fprintf (stderr,"debug warning: graph %s unknown\n",g);
		return ;
	}

	if (graph->layouted) {
		inc_layout (graph);
	} else {
		new_layout (graph);
	}
	debugGraphXFig (graph);
}


/*
 * inc_layout
 * perform an incremental layout
 */

void Layout::inc_layout (GRAPH *graph)

{
	int i;

	levelsEnterNodes (graph,graph->pullup);
	sortInsertHints (graph);

	sortGraphUpperBary (graph);
	sortGraphLowerBary (graph);
	sortInitX (graph);

	/*
	 * there are two ways for finetunig the x-coordinates.
	 * graph.xiterations tells the number of iterations.
	 */
	if (graph->reverseflag) {
		for (i=0;i < graph->xiterations/2;i++) {
			sortGraphDownX (graph);
			sortGraphUpX (graph);
		}
		if (graph->xiterations % 2) {
			sortGraphUpX (graph);
		}
	} else {
	
		for (i=0;i<graph->xiterations/2;i++) {
			sortGraphUpX (graph);
			sortGraphDownX (graph);
		}
		if (graph->xiterations % 2) {
			sortGraphUpX (graph);
		}
	}

	sortGraphVertical (graph);
}


/*
 * new_layout
 * create a new layout
 */

void Layout::new_layout (GRAPH *graph)

{

	int i;

	graph->layouted = TRUE;

	levelsEnterNodes (graph,graph->pullup);
	sortInsertHints (graph);

	/*
	 * there are two ways for finetunig the x-coordinates.
	 * graph->xiterations tells the number of iterations.
	 */
	if (graph->reverseflag) {

		sortGraphLowerBary (graph);
		sortGraphUpperBary (graph);
		sortGraphLowerBary (graph);	
		sortGraphUpperBary (graph);
		if (graph->xiterations % 2) {
			sortGraphLowerBary (graph);
		}

		sortInitX (graph);


		for (i=0;i < graph->xiterations/2;i++) {
			sortGraphDownX (graph);
			sortGraphUpX (graph);
		}
		if (graph->xiterations % 2) {
			sortGraphDownX (graph);
		}
	} else {
		sortGraphUpperBary (graph);
		sortGraphLowerBary (graph);
		sortGraphUpperBary (graph);
		sortGraphLowerBary (graph);	
		if (graph->xiterations % 2) {
			sortGraphUpperBary (graph);
		}

		sortInitX (graph);

		for (i=0;i<graph->xiterations/2;i++) {
			sortGraphUpX (graph);
			sortGraphDownX (graph);
		}
		if (graph->xiterations % 2) {
			sortGraphUpX (graph);
		}
	}

	sortGraphVertical (graph);
}

/*
 * noraOutput
 * echo the changes to stdout. For each node its new position will
 * be written out. 
 */

void Layout::noraOutput (GRAPH *graph)
{
    int i;
    NODE *node;
    
    for (i = 0; i < PRIME; i++) {
	node = graph->hashtab[i];
	while (node) {
	    noraNodeOut (graph->label, node);
	    node = node->hashnext;
	}
    }
}

/*
 * noraNodeOut
 * write out the node position 
 */

void Layout::noraNodeOut (char *, NODE *node)

{
    if (node->x == node->oldx && node->y == node->oldy) {
	return;	/* no changes */
    }

    if (node->type == Regular) {
	node_callback(node->attr.node.label,
			     node->x,
			     node->y);
    } else {
	hint_callback(node->attr.hint.source->attr.node.label,
			     node->attr.hint.target->attr.node.label,
			     node->x,
			     node->y);
    }
    node->oldx = node->x;
    node->oldy = node->y;
    node->layouted = TRUE;
}


/*****************************************************************************
    Debugging functions
*****************************************************************************/

#define XFIGHEADER "#FIG 2.1\n80 2\n"
#define BOXHEADER  "2 2 0 1 -1 0 0 0 0.000 0 0 0\n\t"
#define TEXTHEADER "4 1 0 12 0 -1 0 0.000 0 0 0 "
#define FWDLINE "2 1 0 1 -1 0 0 0 0.000 0 1 0\n\t0 0 1.000 4.000 8.000\n\t"
#define BKWDLINE "2 1 0 1 -1 0 0 0 0.000 0 0 1\n\t0 0 1.000 4.000 8.000\n\t"
#define LINE "2 1 0 1 -1 0 0 0 0.000 0 0 0\n\t"

#define HERE 0
#define OTHER 1
#define NOTHING 3

/*
 * debugNode
 * print a node
 */

void Layout::debugNode (NODE *node)

{
	EDGE *tmp;

	printf ("level=%i center=%i x=%i ",node->level, node->center,
		node->x);
	if (node->type == Regular) {
		printf ("regular label=%s\n",node->attr.node.label);
		printf ("down: ");
		tmp = node->attr.node.down.head ;
		while (tmp) {
			if (tmp->node->type == Regular) {
				printf ("%s ",tmp->node->attr.node.label);
			} else {
				printf ("%i ",tmp->node->attr.hint.id);
			}
			tmp=tmp->next;
		}
		printf ("\n");
		printf ("up: ");
		tmp = node->attr.node.up.head ;
		while (tmp) {
			if (tmp->node->type == Regular) {
				printf ("%s ",tmp->node->attr.node.label);
			} else {
				printf ("%i ",tmp->node->attr.hint.id);
			}
			tmp=tmp->next;
		}
		printf ("\n");
	} else {
		printf ("hint %i\n",node->attr.hint.id);
		printf ("down: ");
		if (node->attr.hint.down) {
			if (node->attr.hint.down->type == Regular) {
				printf ("%s ",node->attr.hint.down
					->attr.node.label);
			} else {
				printf ("%i ",node->attr.hint.down
					->attr.hint.id);
			}
		}
		printf ("\n");
		printf ("up: ");
		if (node->attr.hint.up) {
			if (node->attr.hint.up->type == Regular) {
				printf ("%s ",node->attr.hint.up
					->attr.node.label);
			} else {
				printf ("%i ",node->attr.hint.up
					->attr.hint.id);
			}
		}
		printf ("\n");
	}
}

/*
 * debugLevel
 * print all nodes of a level
 */

void Layout::debugLevel (GRAPH *graph, int n)

{
	NODE **level = graph->level+n;
	NODE *node;

	node = *level ;
	while (node) {
		debugNode (node);
		node = node->right;
	}
}

/*
 * debugAllLevels
 * print the nodes of all levels
 */

void Layout::debugAllLevel (GRAPH *graph)

{
	int i;

	for ( i = 0 ; i < graph->levels; i++) {
		printf ("*** level %i ***\n",i);
		debugLevel (graph,i);
	}
}	
	
/*
 * debugAllNodes
 * write out all nodes
 */

void Layout::debugAllNodes (GRAPH *graph)

{
	int i;
	NODE *node;

	for (i=0;i<PRIME;i++) {
		if (graph->hashtab[i] ) {
			node = graph->hashtab[i] ;
			while (node) {
				debugNode (node);
				node = node->hashnext;
			}
		}
	}
}


/*
 * debugNodeXFig
 * write a xfig-representation for the given node to stdout. The function
 * will display a rectangle for the node and lines to all descants.
 */

void Layout::debugNodeXFig (NODE *nd)

{
	EDGE *edge;
	int arrow;
	int w,h;

	if (nd->type == Regular) {

		w = nd->attr.node.w/2;
		h = nd->attr.node.h/2;

		printf ( BOXHEADER );
		printf ("%i %i ",nd->x - w , nd->y - h);
		printf ("%i %i ",nd->x + w , nd->y - h);
		printf ("%i %i ",nd->x + w , nd->y + h);
		printf ("%i %i ",nd->x - w , nd->y + h);
		printf ("%i %i ",nd->x - w , nd->y - h);
		printf (" 9999 9999\n");
		printf ( TEXTHEADER );
		printf ("%i %i %s\x01\n", nd->x, nd->y, nd->attr.node.label);

		/*
		 * draw the lines to all descendants
		 */

		edge = nd->attr.node.down.head;
		while (edge) {
			arrow = ( edge->arrow == Here ? HERE : OTHER);
			if (arrow == HERE) {
				debugEdgeXFig (nd, edge->node, HERE) ;
			} else {
				if (edge->node->type == Regular) {
					debugEdgeXFig (nd, edge->node, OTHER);
				} else {
					debugEdgeXFig (nd, edge->node,NOTHING);
				}
			}
			edge = edge->next;
		}

	} else if (nd->attr.hint.down) {
		/*
		 * draw the line 
		 */
		if (nd->attr.hint.down->type == Regular) {
			if (nd->attr.hint.target == nd->attr.hint.down) {
				debugEdgeXFig (nd, nd->attr.hint.down, OTHER);
			} else {
				debugEdgeXFig (nd, nd->attr.hint.down,NOTHING);
			}
		} else {
		}

		if (nd->attr.hint.down->type == Hint) {
			debugEdgeXFig (nd, nd->attr.hint.down, NOTHING);
		} else {
			if (nd->attr.hint.target == nd->attr.hint.down) {
				debugEdgeXFig (nd, nd->attr.hint.down, OTHER);
			} else {
				debugEdgeXFig (nd, nd->attr.hint.down,NOTHING);
			}
		}
			
	}
}

/*
 * debugEdgeXFig
 * write a xfig-representation for a line between to nodes to stdout
 */

void Layout::debugEdgeXFig (NODE *source, NODE *target, int arrow)

{
	int x1,y1,x2,y2;

	x1 = source->x ;
	y1 = source->y ;
	if (source->type == Regular ) {
		y1 += source->attr.node.h/2;
	}
	x2 = target->x ;
	y2 = target->y ;
	if (target->type == Regular ) {
		y2 -= target->attr.node.h/2;
	}
	switch (arrow) {
	case OTHER:
		printf (FWDLINE);
		break;
	case HERE:
		printf (BKWDLINE);
		break;
	case NOTHING:
	default:
		printf (LINE);
		break;
	}
	printf ("%i %i %i %i 9999 9999\n",x1,y1,x2,y2);

}

/*
 * debugGraphXFig
 */

void Layout::debugGraphXFig (GRAPH *graph)

{
	NODE *node;
	int i;

	printf (XFIGHEADER);
	for ( i = 0 ; i < PRIME; i++) {
		node = graph->hashtab[i];
		while (node) {
			debugNodeXFig (node);
			node = node->hashnext;
		}
	}
	
}


/*****************************************************************************
    Edgelist functions
*****************************************************************************/

/*
 * listInit
 * initialize a list of edges
 */

void Layout::listInit (EDGELIST *list)

{
	list->head = (EDGE*) NULL;
	list->tail = (EDGE*) NULL;
	list->length = 0;
}

/*
 * listInsertEdge
 * insert a new edge to a list
 */

EDGE *Layout::listInsertEdge (EDGELIST *list, NODE *node)

{
	EDGE *edge;
	EDGE *tail;

	/*
	 * create a new edge
	 */
	edge = (EDGE*) malloc (sizeof (EDGE));
	if (!edge) {
		fprintf (stderr,"listInsertEdge: out of memory\n");
		exit (MEMORY_ERROR);
	}
	/*
	 * link the edge to the list
	 */
	edge->next = (EDGE*) NULL;
	edge->prev = (EDGE*) NULL;

	tail = list->head;
	list->head = edge;
	edge->next = tail;
	if (!tail) {
		list->tail = edge;
	} else {
		tail->prev = edge;
	}
	
	/*
	 * enter the node to the edge
	 */
	edge->node = node;

	list->length++;

	return edge;
}
	
/*
 * listRemoveEdge
 * remove one edge form a list of edges
 */

void Layout::listRemoveEdge (EDGELIST *list, EDGE *edge)

{
	if (edge->prev && edge->next) {
		/* edge neither head nor tail of list */
		edge->next->prev = edge->prev ;
		edge->prev->next = edge->next ;
	} else {
		if (!edge->next) {
			/* last entry of list */
			list->tail = edge->prev;
			if (edge->prev) {
				edge->prev->next = (EDGE*) NULL;
			}
		}
		if (!edge->prev) {
			/* first entry of list */
			list->head = edge->next ;
			if (edge->next) {
				edge->next->prev = (EDGE*) NULL;
			}
		}
	}
	free ((char*)edge);
	/*
	 * correct number of entries 
	 */
	list->length-- ;

}

/*
 * listFindNode
 * find an edge to a specified node
 */

EDGE *Layout::listFindNode (EDGELIST *list, NODE *node)

{
	EDGE *edge;
	
	edge = list->head;
	while (edge && edge->node != node) {
		edge = edge->next;
	}
	if (!edge) {
		fprintf (stderr,"listFindEntry: can't find entry\n");
		exit (NOT_MEMBER);
	}
	
	return edge;
}

/*
 * listFindTarget
 * find an edge to a specified target
 */

EDGE *Layout::listFindTarget (EDGELIST *list, NODE *target)

{
	EDGE *edge;
	
	edge = list->head;
	while (edge && edge->target != target) {
		edge = edge->next;
	}
	if (!edge) {
		fprintf (stderr,"listFindEntry: can't find entry\n");
		edge = (EDGE*) NULL;
	}
	
	return edge;
}

/*
 * listRemove
 * remove all edges from a list
 * this function is just for cleaning up
 */

void Layout::listRemove (EDGELIST *list)

{
	EDGE *edge;
	EDGE *tmp;
	
	edge = list->head;
	while (edge) {
		tmp = edge->next;
		free ((char*) edge);
		edge = tmp;
	}
	list->head = (EDGE*) NULL;
	list->tail = (EDGE*) NULL;
}


/*****************************************************************************
    Node functions
*****************************************************************************/

/*
 * nodeInit
 * initialize a node
 */

void Layout::nodeInit (NODE* node, ID *id , NODETYPE type)
 
{
	node->x = 0;
	node->y = 0;
	node->oldx = NOPOSITION;
	node->oldy = NOPOSITION;
	node->layouted = FALSE;
	node->level = NOLEVEL;
	node->center = 0;
	node->index = 0 ;
	node->loop = 0;
	node->mark = (NODE*) NULL;
	
	node->left = (NODE*) NULL;
	node->right = (NODE*) NULL;

	node->hashnext = (NODE*) NULL;
	node->hashprev = (NODE*) NULL;
	
	node->type = type;

	if ( type == Regular ) {
		node->attr.node.label = (char*) malloc (strlen(id->label)+5);
		if (!node->attr.node.label) {
			fprintf (stderr,"nodeInit: out of memory!\n");
			exit (MEMORY_ERROR);
		}
		strcpy (node->attr.node.label, id->label);

		node->attr.node.w = 0;
		node->attr.node.h = 0;
		listInit (&node->attr.node.up);
		listInit (&node->attr.node.down);
	} else {
		node->attr.hint.id = id->id ;
		node->attr.hint.up = (NODE*) NULL;
		node->attr.hint.down = (NODE*) NULL;
		node->attr.hint.source = (NODE*) NULL;
		node->attr.hint.target = (NODE*) NULL;
	}
		
}		

/*
 * nodeRemove
 * remove a node.
 * remember to free the label and and the lists of adjacent nodes.
 */

void Layout::nodeRemove (NODE *node) 

{
	if (node->type == Regular) {
		free (node->attr.node.label);
		listRemove (&node->attr.node.up);
		listRemove (&node->attr.node.down);
	}
	
	free ((char*)node);
}

/*****************************************************************************
    Graph functions
*****************************************************************************/

/*
 * graphInit
 * initialize a graph
 */

void Layout::graphInit (GRAPH *graph, char *label)

{
	int i;
	
	graph->label = (char *)malloc (strlen(label)+1);
	if (!graph->label) {
		fprintf (stderr,"graphInit: out of memory!\n");
	}
	strcpy (graph->label, label);
	graph->hashnext = (GRAPH*) NULL;
	graph->hashprev = (GRAPH*) NULL;

	graph->minxdist    = MINXDIST ;
	graph->minydist    = MINYDIST;
	graph->xiterations = XITERATIONS;
	graph->reverseflag = REVERSE ;
	graph->pullup      = PULLUP;

	graph->levels = 0;
	graph->level = (NODE**) NULL;

	graph->layouted = FALSE ; 	/* the graph was never layouted */

	for ( i = 0; i < PRIME; i++) {
		graph->hashtab[i] = (NODE*) NULL;
	}
}

/*
 * graphEnterNode
 * enter a new node to the graph and return a pointer to the
 * new node
 */

NODE *Layout::graphEnterNode (GRAPH *graph, ID *id, NODETYPE type)

{
	NODE *node;
	NODE *tail;
	int pos;

	node = (NODE*) malloc (sizeof(NODE)) ;
	if (!node) {
		fprintf (stderr, "graphEnterNode: out of memory\n");
		exit (MEMORY_ERROR);
	}
	nodeInit (node,id,type);

	/*
	 * insert the new node to the hashing table
	 * TODO: check for dublicates of the given nodeID
	 */
	
	if (type == Regular) {
		pos = graphHashStr (node->attr.node.label, PRIME);
	} else {
		pos =  node->attr.hint.id % PRIME;
	}
	tail = graph->hashtab[pos] ;
	graph->hashtab[pos] = node;
	node->hashnext = tail ;
	node->hashprev = (NODE*) NULL;
	if (node->hashnext) {
		node->hashnext->hashprev = node;
	}
	
	return node;
}

/*
 * graphGetNode
 * get a pointer to a node described by its ID
 */

NODE *Layout::graphGetNode (GRAPH *graph, ID *id, NODETYPE type)

{
	int pos;
	int found = FALSE ;
	NODE *node;

	/*
	 * calculate the hash-entry
	 */
	if (type == Regular) {
		pos = graphHashStr (id->label, PRIME);
		node = graph->hashtab[pos];

		/*
		 * search for entry 
		 */
		while (node && !found) {
			if (node->type != Regular 
			    ||  strcmp(node->attr.node.label,id->label)) {
				node = node->hashnext;
			} else {
				found = TRUE;
			}
		} 

	} else {
		pos =  id->id % PRIME;
		node = graph->hashtab[pos];

		/*
		 * search for entry 
		 */

		while (node && !found) {
			if (node->type != Hint 
			    ||  node->attr.hint.id != id->id) {
				node = node->hashnext;
			} else {
				found = TRUE;
			}
		} 
	}

	/*
	 * node == NULL if not found
	 */
	return node;
}

/*
 * graphRemoveNode
 * delete a node from the hashtab of a graph. 
 * ATTENTION: You have to delete all edges connected to the node
 * and remove the node from its level by yourself!
 */

void Layout::graphRemoveNode (GRAPH *graph, ID *id, NODETYPE type)

{
	int pos;
	NODE *node;

	/*
	 * calculate the hash-entry
	 */
	if (type == Regular) {
		pos = graphHashStr (id->label, PRIME);
		node = graph->hashtab[pos];

		/*
		 * search for entry 
		 */
		while (node && strcmp(node->attr.node.label,id->label)) {
			node = node->hashnext;
		}

	} else {
		pos =  id->id % PRIME;
		node = graph->hashtab[pos];

		/*
		 * search for entry 
		 */
		while (node && node->attr.hint.id != id->id) {
			node = node->hashnext;
		}
	}

	/*
	 * node == NULL if not found
	 */

	if (!node) {
		fprintf (stderr,"graphRemoveNode: can't find entry!\n");
		exit (NOT_MEMBER);
	}

	/*
	 * remove the node from the double linked list
	 */

	if (node->hashprev && node->hashprev) {
		node->hashprev->hashnext = node->hashnext;
		node->hashnext->hashprev = node->hashprev;
	} else {
		if (!node->hashprev) {
			graph->hashtab[pos] = node->hashnext;
			if (node->hashnext) {
				node->hashnext->hashprev = (NODE*) NULL;
			}
		}
		if (!node->hashnext && node->hashprev) {
			node->hashprev->hashnext = (NODE*) NULL;
		}
	}
	nodeRemove (node);
}
	
/* 
 * graphCreateLevels
 * create a number of Levels for a graph and initialize it
 */

void Layout::graphCreateLevels (GRAPH *graph, int n)

{
	NODE **nodeptr;
	int i;

	graph->levels = n;
	graph->level = (NODE **) malloc (sizeof (NODE*) * n);
	if (!graph->level) {
		fprintf (stderr,"graphCreateLevels: out of memory!\n");
		exit (MEMORY_ERROR);
	}
	
	/*
	 * initialize it
	 */

	nodeptr = graph->level ;
	for ( i = 0 ; i < n ; i++ ) {
		*(nodeptr++) = (NODE*) NULL;
	}
}

/*
 * graphRemoveLevels
 * remove the level references.
 * ATTENTION: only the references will be deleted, not the nodes
 * contained by the levels!
 */

void Layout::graphRemoveLevels (GRAPH *graph)

{
	free ( (char*) graph->level);
	graph->level = (NODE**) NULL;
	graph->levels = 0;
}

/*
 * graphAddLevels
 * enlarge the table of levels by 'n'. The new levels will all be 
 * empty.
 */

void Layout::graphAddLevels (GRAPH *graph, int n)

{
	NODE **newtab;
	int i;
	

	/*
	 * create a larger table
	 */
	newtab = (NODE**) malloc (sizeof(NODE*) * (graph->levels + n));
	if (!newtab) {
		fprintf (stderr,"graphAddLevels: out of memory!\n");
		exit (MEMORY_ERROR);
	}
	/*
	 * fill the table ..
	 */
	for (i=0 ; i < graph->levels; i++) {
		*(newtab+i) = *(graph->level);
	}
	/*
	 * clear the new levels
	 */
	for (i=graph->levels; i < graph->levels+n; i++) {
		*(newtab+i) = (NODE*) NULL;
	}
	/*
	 * make the new table to the actual table
	 */
	graph->levels += n;
	free ((char*) graph->level);
	graph->level = newtab;
}

/*
 * graphInsertEdge
 * insert an edge of a specified type between two regular nodes of the graph
 * TODO: check for dublicates and loops
 */

void Layout::graphInsertEdge (GRAPH *, NODE *source, NODE *target)

{
	EDGE *from;
	EDGE *to;

	if (source->type != Regular) {
		fprintf (stderr,"graphInsertEdge: wrong node type\n");
		exit (NODE_TYPE);
	}
	if (target->type != Regular) {
		fprintf (stderr,"graphInsertEdge: wrong node type\n");
		exit (NODE_TYPE);
	}
	
	to = graphFindEdgeAtSource (source,target) ;
	if (to) {
		fprintf (stderr,"graphInsertEdge: warning - edge exists\n");
		return;
	}

	from = listInsertEdge (&source->attr.node.down, target);
	from->arrow = Other;
	from->target = target;
	from->node = target;

	to = listInsertEdge (&target->attr.node.up, source);
	to->arrow = Here;
	to->target = source;
	to->node = source;

}

/*
 * graphInvertEdge
 * invert the internal represation of an edge.
 */

void Layout::graphInvertEdge (NODE *source, NODE *target)

{
	EDGE *to, *from;
	EDGEARROW srcarrow;


	if (source->type != Regular || target->type != Regular) {
		fprintf (stderr,"graphInvertEdge: node not regular!\n");
		exit (INTERNAL);
	}
	fprintf (stderr,"inverting Edge %s -> %s\n", source->attr.node.label,
		 target->attr.node.label);
	to = listFindTarget (&source->attr.node.down,target);
	from = listFindTarget (&target->attr.node.up,source);
	if (!to || !from) {
		fprintf (stderr,"graphInvertEdge: can't find edge!\n");
		exit (NO_EDGE);
	}
	srcarrow = to->arrow;

	/*
	 * remove the edge
	 */
	listRemoveEdge (&source->attr.node.down, to);
	listRemoveEdge (&target->attr.node.up,from);
	/*
	 * create inverted edge
	 */
	to = listInsertEdge (&source->attr.node.up, target);
	to->target = target;
	to->arrow = srcarrow;
	from = listInsertEdge (&target->attr.node.down, source);
	from->target = source;
	from->arrow = ( srcarrow == Here ? Other : Here );
}

/*
 * graphNewNodeID
 * return a new nodeID
 */

int Layout::graphNewNodeID()
{
	int counter = 1000 ;

	return counter++ ;
}

/*
 * graphInsertHint
 * insert a hint node by splitting an edge between two nodes.
 * A pointer to the new created hint node will be returned
 */

NODE *Layout::graphInsertHint (GRAPH *graph, NODE *source, NODE* target)

{
	ID id;
	NODE *hint = 0;
	EDGE *toTarget = 0;
	EDGE *toSource = 0;

	/*
	 * there must be an edge between source and target 
	 */

	id.id = graphNewNodeID();
	hint = graphEnterNode (graph,&id, Hint);
	if (source->type == Regular) {
		/*
		 * find edge to target
		 */
		toTarget = listFindNode (&source->attr.node.down, target);
		toTarget->node = hint;
	} else {
		source->attr.hint.down = hint;
	}

	if (target->type == Regular) {
		/*
		 * find edge to source
		 */
		toSource = listFindNode (&target->attr.node.up, source);
		toSource->node = hint;
	} else {
		target->attr.hint.up = hint;
	}

	hint->attr.hint.up = source;
	hint->attr.hint.down = target;

	/*
	 * enter the information, to which edge this hint belongs
	 */
	
	if (source->type == Hint) {
		hint->attr.hint.source = source->attr.hint.source;
		hint->attr.hint.target = source->attr.hint.target;
	} else if (target->type == Hint) {
		hint->attr.hint.source = target->attr.hint.source;
		hint->attr.hint.target = target->attr.hint.target;
	} else if (toTarget->arrow == Other){
		hint->attr.hint.source = source;
		hint->attr.hint.target = target;
	} else {
		hint->attr.hint.source = target;
		hint->attr.hint.target = source;
	}

	return hint;
}

/*
 * graphFindEdgeAtSource 
 * return the edge leading from source to target. Source and target
 * must be regular!
 */

EDGE *Layout::graphFindEdgeAtSource (NODE *source, NODE *target)

{
	EDGE *edge;
	
	edge = source->attr.node.down.head ;
	while (edge && !(edge->target == target && edge->arrow == Other)){
		edge = edge->next;
	}
	if (!edge) {
		edge = source->attr.node.up.head ;
		while (edge && !(edge->target == target && 
				 edge->arrow == Other )) {
			edge = edge->next;
		}
	}
	return edge;
}


/*
 * graphFindEdgeAtTarget
 * return the edge (at target) leading from source to target. Source and target
 * must be regular!
 */

EDGE *Layout::graphFindEdgeAtTarget (NODE *source, NODE *target)

{
	EDGE *edge;
	
	edge = target->attr.node.up.head ;
	while (edge && !(edge->target == source && edge->arrow == Here)){
		edge = edge->next;
	}
	if (!edge) {
		edge = target->attr.node.down.head ;
		while (edge && !(edge->target == source && 
				 edge->arrow == Here )) {
			edge = edge->next;
		}
	}
	return edge;
}


/*
 * graphResetLevels
 * set the level of all nodes to NOLEVEL. The entries of 'left' and 'right'
 * will be set to NULL.
 * ATTENTION: You first have to clear the Levels by calling
 * 'graphRemoveLevels' before you call this function!
 */

void Layout::graphResetLevels (GRAPH *graph) 

{
	int i;
	NODE *node;

	for (i = 0 ; i < PRIME; i++) {
		node = graph->hashtab[i];
		while (node) {
			node->level = NOLEVEL;
			node->left = (NODE*) NULL;
			node->right = (NODE*) NULL;

			node = node->hashnext;
		}
	}
}
	
	
/*
 * graphHashStr
 * calculate a hash-value for a given string and return it. The 
 * hash-value will belong to [0..PRIME]
 * original by P.J. Weinberger
 */

int Layout::graphHashStr (char *str, int prime) 

{
	char *p;
	unsigned h = 0, g;
	
	for (p=str; *p != '\0'; p++ ) {
		h = ( h << 4) + (*p);
		if ((g = h & 0xf0000000)) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h % prime;
}

/*
 * functions for maintaining multiple graphs
 */

/*
 * graphGet
 * return a pointer to a specified graph. Return NULL, if no such
 * graph exists.
 * 'hashtab' contains SMALLPRIME pointers to GRAPHs.
 */

GRAPH *Layout::graphGet (GRAPHTAB *tab, char *label)

{
	int pos;
	GRAPH *graph;

	pos = graphHashStr (label, SMALLPRIME);
	/*
	 * try to find graph
	 */
	graph = (*tab)[pos];
	while (graph && strcmp(graph->label, label)) {
		graph = graph->hashnext;
	}
	return graph;
}
	
	
/*
 * graphNew
 * create a new graph, initialize it and return a pointer to it
 * if a graph with the specified label allready exists a warning
 * is printed and no new graph is created (NULL returned).
 */

GRAPH *Layout::graphNew (GRAPHTAB *tab,char *label)

{
	GRAPH *graph;
	GRAPH *tail;
	int pos;

	/*
	 * check for dublicate
	 */
	graph = graphGet (tab, label);
	if (graph) {
		fprintf (stderr,"graphNew: %s allready there!\n",label);
		return (GRAPH*) NULL;
	}
		
	graph = (GRAPH*) malloc (sizeof(GRAPH));
	if (!graph) {
		fprintf (stderr,"graphNew: out of memory\n");
		exit (MEMORY_ERROR);
	}
	graphInit (graph,label);
	
	/*
	 * enter graph to tab
	 */
	pos = graphHashStr (label, SMALLPRIME);

	if ((*tab)[pos]) {
		tail = ((*tab)[pos])->hashnext;
	} else {
		tail = (GRAPH*) NULL;
	}
	(*tab)[pos] = graph;
	graph->hashnext = tail;
	graph->hashprev = (GRAPH*) NULL;
	if (graph->hashnext) {
		graph->hashnext->hashprev = graph;
	}
	
	return graph;
}
			
/*
 * graphRemove
 * remove a graph from a GRAPHTAB
 */

void Layout::graphRemove (GRAPHTAB *tab, char *label)

{
	int pos;
 	int i;
	GRAPH *graph;
	NODE *nextnode;
	NODE *node;

 	pos = graphHashStr (label, SMALLPRIME);
	/*
	 * try to find graph
	 */
	graph = (*tab)[pos];
	while (graph && strcmp(graph->label, label)) {
		graph = graph->hashnext;
	}
	if (!graph) {
		/*
		 * not found
		 */
		fprintf (stderr,"graphRemove: %s not found!\n",label);
		return;
	}
	/*
	 * remove the graph
	 */
	if (graph->hashprev && graph->hashprev) {
		graph->hashprev->hashnext = graph->hashnext;
		graph->hashnext->hashprev = graph->hashprev;
	} else {
		if (!graph->hashprev) {
			(*tab)[pos] = graph->hashnext;
			if (graph->hashnext) {
				graph->hashnext->hashprev = (GRAPH*) NULL;
			}
		}
		if (!graph->hashnext && graph->hashprev) {
			graph->hashprev->hashnext = (GRAPH*) NULL;
		}
	}

	graphRemoveLevels (graph); /* remove Levels */
	for (i=0; i < PRIME; i++) {
		node = graph->hashtab[i];
		while (node) {
			nextnode = node->hashnext;
			nodeRemove (node);
			node = nextnode;
		}
	}
	free (graph->label);
	free ((char*) graph);
}

/*
 * graphTabInit
 * initalize a GRAPHTAB
 */

void Layout::graphTabInit (GRAPHTAB *tab)

{
	int i;
	
	for (i=0 ; i < SMALLPRIME; i++) {
		(*tab)[i] = (GRAPH*) NULL;
	}
}


/*****************************************************************************
    Level functions
*****************************************************************************/

/*
 * levelsInsertNode
 * insert a node to a specified level. The node wil only be
 * inserted if its components 'left' and 'right' are NULL. Otherwise
 * the function assumes, that the node is allready member of
 * a level. These differences are made to allow incremental
 * layout of the graph.
 */

void Layout::levelsInsertNode (GRAPH *graph, NODE *node, int n)

{
	NODE **level;

	if (n > graph->levels || !graph->level) {
		fprintf (stderr,"levelsInsertNode: wrong Level!\n");
		exit (LEVEL_ERROR);
	}

	if (!node->right && !node->left) {

		/*
		 * insert Node at head of level
		 */
	
		level = graph->level+n ;
		node->right = *level;
		node->left = (NODE*) NULL;
		if (*level) {
			(*level)->left = node;
		}
		*level = node;
		node->level = n;
	}

	/*
	 * else do nothing, assuming the node is member of a level
	 */
}

/*
 * levelsRemoveNode
 * remove a node from a level
 * The node won't be deleted - it's just not referenced by the
 * specified level any more.
 */

void Layout::levelsRemoveNode (GRAPH *graph, NODE *node, int n)

{
	NODE **level = graph->level+n;

	if (!node->left) {
		*level = node->right;
	} else {
		node->left->right = node->right;
	}

	if (node->right) {
		node->right->left = node->left;
	}
	node->level = NOLEVEL ;
}


	
/*
 * levelsEnterNodes
 * enter all nodes to their level
 */

void Layout::levelsEnterNodes (GRAPH *graph, int pullup)

{
	int levels;
	int i;
	NODE *node;

	levels = sortApplyLevel (graph); 	/* apply levels */

	/*
	 * check for levels allready there
	 */
	if (!levels) {
		fprintf (stderr," levelsEnterNodes: internal Error\n");
		exit (INTERNAL);
	}
	if (!graph->level) {
		/*
		 * create levels
		 */
		graphCreateLevels (graph, levels);
	} else {
		/*
		 * maybee we have to add some levels
		 */
		if (levels > graph->levels) {
			graphAddLevels (graph, levels - graph->levels);
		}
	}
	for ( i = 0 ; i < PRIME ; i++ ) {
		node = graph->hashtab[i] ;
		while (node) {
			/*
			 * add only new nodes ..
			 */
			if (!node->layouted) {
				/* 
				 * node was previously not
				 * layouted -- add it
				 */
				levelsInsertNode (graph,node, node->level);
			}
			node = node->hashnext;
			
		}
	}
	if (pullup) {
		sortPullupNodes (graph); 		/* optimize */
	}

}
	
/*
 * levelsIndex
 * apply an index to a level: the first node of a level will get
 * index=1, the second index=2, ....
 */

void Layout::levelsIndex (NODE **level)

{
	int i = 1;
	NODE *node;

	node = *level ;
	while (node) {
		node->index = i;
		i++;
		node = node->right;
	}
}

/*
 * levelsLength
 * return the number of elements of a given level
 */

int Layout::levelsLength (NODE **level) 

{
	NODE *node;
	int len = 0;

	node = *level;
	while (node) {
		len++;
		node = node->right;
	}
	return len;
}


/*****************************************************************************
    Sorting functions
*****************************************************************************/


typedef int (*QuicksortCompareProc)(const void *, const void *);

/*
 * sortApplyLevel
 * apply a level to each node of the graph and return the number of
 * levels inside the graph
 */

int Layout::sortApplyLevel (GRAPH *graph)

{
	int level ;
	int maxlevel = 0;
	NODE *node;
	int i;

	for (i=0; i < PRIME; i++) {
		node = graph->hashtab[i];
		while (node) {
			if (node->level == NOLEVEL) {
				level = distance (node,node);
			} else {
				level = node->level;
			}
			maxlevel = ( level > maxlevel ? level : maxlevel );
			node = node->hashnext;
		}
	}
	return ++maxlevel;
}

/*
 * sortPullupNodes
 * each node should reside one level below the minimum of the levels
 * of its anchestors.
 * This function improves the assigned levels in the way mentioned
 * above. This works only for graph with no hints inside !
 */

void Layout::sortPullupNodes (GRAPH *graph)

{
	NODE **level;
	NODE *node;
	NODE *rightnode;
	int newlevel ;
	
	if (graph->levels < 2) {
		return ;
	}
	level = graph->level+(graph->levels-1);
	do {
		level--;
		node = *level;
		
		while (node) {
			newlevel = minimumLevel (node) - 1;
			rightnode = node->right;
			if (newlevel != node->level) {
				/*
				 * pull it up
				 */
				levelsRemoveNode (graph, node, node->level);
				node->left = (NODE*) NULL;
				node->right = (NODE*) NULL;
				levelsInsertNode (graph, node, newlevel);
			}
			node = rightnode;
		}
		
	} while ( level != graph->level);
}


/*
 * minimumLevel
 * return the minimum level of the anchestors of a given regular node.
 */

int Layout::minimumLevel (NODE *node)

{
	EDGE *edge;
	int min = 1000 ;
	int tmp;

	if (node->type != Regular) {
		fprintf (stderr,"minimumLevel: not a regular Node!\n");
		exit (NOT_REGULAR);
	}
	
	edge = node->attr.node.up.head ;
	if (edge) {
		while (edge) {
			tmp = edge->target->level;
			min = ( tmp < min ? tmp : min);
			edge = edge->next;
		}
		return min;
	} else {
		return node->level + 1;
	}
}
		

/*
 * distance
 * determine the max. number of descendants of a given node. Enter this
 * value to the 'level' component and return it. 
 */

int Layout::distance (NODE *node, NODE *origin)

{
	int dist = 0;
	int maxdist = 0;
	EDGE *edge;
	EDGE *tmpedge;

	node->mark = origin;
	/*
	 * have a look at the type of the node ...
	 */
	if (node->type == Regular) {
		edge = node->attr.node.down.head ;
		while (edge) {
			if (edge->node->level != NOLEVEL) {
				dist = 1 + edge->node->level;
				edge = edge->next;
			} else if ( edge->node->mark == origin ) {
				/*
				 * cycle detected ...
				 * there is a cycle (following the
				 * down-references) which is 
				 * closed by the path from node to
				 * edge->node
				 * The cycle can be brocken up by 
				 * inverting the edge between
				 * node and edge->node.
				 */
				dist = 0;
				tmpedge = edge->next;
				graphInvertEdge (node,edge->node);
				edge = tmpedge;
			} else {
				tmpedge = edge->next;
				dist = 1 + distance (edge->node, origin);
				edge = tmpedge;
			}
			maxdist = (dist > maxdist ? dist : maxdist);
		}
#if 1
	} else {
		fprintf (stderr,"distance: unleveled Hint!\n");
		exit (INTERNAL);
	}

#else
	} else if ( node->attr.hint.down ) {
		/*
		 * it's a hint
		 */
		if (node->attr.hint.down->level != NOLEVEL){
			maxdist = 1 + node->attr.hint.down->level ;
		} else if (node->attr.hint.down->mark == origin ) {
			/*
			 * cycle detected ...
			 */
			fprintf (stderr, "Cycle detected ..");
			exit (LOOPS);
		} else {
			maxdist = 1 + distance (node->attr.hint.down, origin);
		}
	} else {
		/*
		 * no descendants 
		 */
		maxdist = 0;
	}
#endif
	/*
	 * enter the distance to the node and return maxdist
	 */
	node->level = maxdist;
	return maxdist;
}

/*
 * sortInsertHints
 * check the descendants of each node. If a descendant is more than one
 * level apart insert hints on the level inbetween.
 */

void Layout::sortInsertHints (GRAPH *graph) 

{
	NODE *node;
	NODE **level;
	int i;

	level = graph->level+(graph->levels-1);
	for ( i = 0 ; i < graph->levels ; i++ ) {
		node = *level;
		while (node) {
			sortCheckNode (graph,node);
			node = node->right;
		}
		level--;
	}
}

/*
 * sortCheckNode
 * check the descaendants of a node: if a descandant is more than
 * one level away insert a hint node.
 */

void Layout::sortCheckNode (GRAPH *graph, NODE *node)

{
	EDGE *edge;
	NODE *des;
	NODE *hint;

	if (node->type == Regular) {
		edge = node->attr.node.down.head ;
		while (edge) {
			des = edge->node;
			if (des->level < node->level-1) {
				/* 
				 * insert hint
				 */
				hint = graphInsertHint (graph,node, des);
				levelsInsertNode (graph, hint, node->level-1);
			}
			edge = edge->next;
		}
	} else {
		if ( node->attr.hint.down
		    && node->attr.hint.down->level < node->level-1 ) {
			/*
			 * insert hint
			 */
			hint = graphInsertHint (graph,node, 
						node->attr.hint.down);
			levelsInsertNode (graph, hint, node->level-1);
		}
	}
}


/*
 * sortNodeUpperBary
 * calculate the bary center og a node according to its anchestors.
 * ATTENTION: the 'index'-components of the anchestors must be valid!
 */
	

int Layout::sortNodeUpperBary (NODE *node)

{
	int sum = 0;
	int count = 0;
	EDGE *upnode;

	if (node->type == Hint) {
		if (node->attr.hint.up) {
			return (node->attr.hint.up->index * 10);
		} else {
			return 0;
		}
	} else {
		if (node->attr.node.up.length == 0) {
			return 0;
		} else {
			upnode = node->attr.node.up.head;
			while (upnode) {
				sum += upnode->node->index;
				count++;
				upnode=upnode->next;
			}
			return ( (sum * 10) / count );
		}
	}
}
			

/*
 * sortNodeLowerBary
 * calculate the bary center og a node according to its descendants.
 * ATTENTION: the 'index'-components of the descendabts must be valid!
 */
	
int Layout::sortNodeLowerBary (NODE *node)

{
	int sum = 0;
	int count = 0;
	EDGE *downnode;

	if (node->type == Hint) {
		if (node->attr.hint.down) {
			return (node->attr.hint.down->index * 10);
		} else {
			return 0;
		}
	} else {
		if (node->attr.node.down.length == 0) {
			return 0;
		} else {
			downnode = node->attr.node.down.head;
			while (downnode) {
				sum += downnode->node->index;
				count++;
				downnode=downnode->next;
			}
			return ( (sum * 10) / count );
		}
	}
}
			
/*
 * sortGraphUpperBary
 * assign to every node of every level its bary center according to its
 * anchestors and sort each level by the centers. The algorithm will
 * start with the top levels and will move down. If the flag is set,
 * the determined center will not be written to 'center' but 'upcenter' 
 * and the level won't be sorted. This will be done later by the 
 * sortAvrgCenter-function.
 */

void Layout::sortGraphUpperBary (GRAPH *graph)

{
	NODE **uplevel;
	NODE **level;
	NODE *node;

	if (graph->levels < 2) {
		/* only one level - nothing to do */
		return;
	}

	uplevel = graph->level+(graph->levels-1) ;
	level = uplevel;

	do {
		level--;
		levelsIndex (uplevel);
		node = *level;
		while (node) {
			node->center = sortNodeUpperBary (node);
			node = node->right;
		}
		sortByCenter (level);
		uplevel--;
		
	} while (level != graph->level);

}

/*
 * sortGraphLowerBary
 * assign to every node of every level its bary center according to its
 * descendants and sort each level by the centers. The algorithm will
 * start with the lowest levels and will move up.
 * If the flag is set,
 * the determined center will not be written to 'center' but 'downcenter' 
 * and the level won't be sorted. This will be done later by the 
 * sortAvrgCenter-function.
 */

void Layout::sortGraphLowerBary (GRAPH *graph)

{
	NODE **downlevel;
	NODE **toplevel;
	NODE **level;
	NODE *node;

	if (graph->levels < 2) {
		/* only one level - nothing to do */
		return;
	}

	toplevel = graph->level+(graph->levels-1);
	downlevel = graph->level ;
	level = downlevel;

	do {
		level++;
		levelsIndex (downlevel);
		node = *level;
		while (node) {
			node->center = sortNodeLowerBary (node);
			node = node->right;
		}
		sortByCenter (level);
		downlevel++;
		
	} while (level != toplevel);

}

/*
 * sortByCenter
 * sort a level by the bary centers of its nodes. The function will 
 * first calculate the order and then rebuild the level (i.e. its
 * list. 
 */

void Layout::sortByCenter (NODE **level) 

{
	NODE **index;
	NODE **tmp;
	NODE *node;
	int len = levelsLength (level);
	int i;
	
	if (len < 2) {
		return;
	}
	
	index = (NODE**) malloc (sizeof (NODE*) * len);
	if (!index) {
		fprintf (stderr,"sortByCenter: out of memory!\n");
		exit (MEMORY_ERROR);
	}

	/* fill the index */
	
	tmp = index;
	node = *level;
	while (node) {
		*(tmp++) = node;
		node = node->right;
	}

	/* sort the index */

	qsort ( (char*) index , len, sizeof (NODE*), 
	       (QuicksortCompareProc)sortCmpCenters );

	/*
	 * build up a new list according to the sorted index
	 */

	tmp = index;
	*level = *tmp;
	(*level)->left = (NODE*) NULL;

	for (i=1 ; i < len ; i++) {
		(*tmp)->right = *(tmp+1);
		(*(tmp+1))->left = *tmp;
		tmp++;
	}
	(*tmp)->right = (NODE*) NULL;

	free ( (char*) index);
}

#if 0
/*
 * sortAvrgCenter
 * calculate the average bary center for each node by its upper and 
 * lower bary center. This value will be stored in 'center' of each
 * node. Each level will be sorted by the average bary center.
 */

void Layout::sortAvrgCenter (GRAPH *graph)

{
	NODE **level;
	NODE *node;
	int i;
	
	level = graph->level;
	for (i=0; i < graph->levels; i++) {
		node = *level;
		while (node) {
			node->center = (node->upcenter + node->downcenter) / 2;
			node = node->right;
		}
		sortByCenter (level);
		level++;
	}
}
		
#endif

/*
 * sortCmpCenters
 * compare the centers of two nodes
 */

int Layout::sortCmpCenters (NODE **_n1, NODE **_n2) 

{	
    NODE *n1 = *_n1;
    NODE *n2 = *_n2;

    // Compare by center
    int ret = n1->center - n2->center;
    if (ret != 0 || compare_callback == 0)
	return ret;

    // Compare by target name
    while (n1 && n1->type == Hint)
	n1 = n1->attr.hint.target;
    while (n2 && n2->type == Hint)
	n2 = n2->attr.hint.target;

    assert (n1 != 0);
    assert (n1->type == Regular);
    assert (n2 != 0);
    assert (n2->type == Regular);

    char *label1 = n1->attr.node.label;
    char *label2 = n2->attr.node.label;

    return compare_callback(label1, label2);
}

				
				 
/*
 * sortInitX
 * assign initial x-ccordinates to all nodes. Nodes of the same level 
 * will have at least the minimum distance.
 * The x- and y-ccordinates of a node represent its center!
 */

void Layout::sortInitX (GRAPH *graph) 

{
	NODE **level = graph->level;
	NODE *node;
	int x;
	int nodex;
	int i;
	
	for (i=0; i < graph->levels; i++) {
		node = *level;
		x = 0;
		while (node) {
			if (node->type == Regular) {
				nodex = x + node->attr.node.w / 2;
			} else {
				nodex = x ;
			}
			node->x = nodex;
			x += graph->minxdist ;
			if (node->type == Regular) {
				x +=  node->attr.node.w ;
			} 
			node = node->right;
		}
		level++;
	}
}

/*
 * sortGraphUpX
 * assign a x-coordinate to each node of the graph. Each x-coordinate
 * is determined by the anchestors of each node. The function starts
 * with the highest level and moves down.
 */

void Layout::sortGraphUpX (GRAPH *graph)

{
	NODE **level;

	if (graph->levels < 2) {
		/* only one level - nothing to do */
		return;
	}

	level = graph->level+(graph->levels-1) ;

	do {
		level--;
		sortLevelUpX (level, graph->minxdist);
		
	} while (level != graph->level);
	
}

/*
 * sortGraphDownX
 * assign a x-coordinate to each node of the graph. Each x-coordinate
 * is determined by the descendants of each node. The function starts
 * with the lowest level and moves up.
 */

void Layout::sortGraphDownX (GRAPH *graph)

{
	NODE **level;
	NODE **toplevel = graph->level+(graph->levels-1);

	if (graph->levels < 2) {
		/* only one level - nothing to do */
		return;
	}

	level = graph->level;

	do {
		level++;
		sortLevelDownX (level, graph->minxdist);
	} while (level != toplevel);
	
}

/*
 * sortLevelUpX
 * assign x-ccordinates to each node of a level. The preferred x-position 
 * of a nodes is the average x-position of its anchestors. For resolving
 * conflicts each node gets a priority depending on its number of
 * anchestors. Nodes with high priority will be placed on its preferred
 * position with a higher probability.
 */

void Layout::sortLevelUpX (NODE **level, int dist)

{
	NODE **index;
	NODE **tmp;
	NODE *node;

	int len = levelsLength (level);
	int newx;

	index = (NODE**) malloc (sizeof (NODE*) * (len+1));
	if (!index) {
		fprintf (stderr,"sortByCenter: out of memory!\n");
		exit (MEMORY_ERROR);
	}

	/* fill the index */
	
	tmp = index;
	node = *level;
	while (node) {
		*(tmp++) = node;
		node = node->right;
	}
	*tmp = (NODE*) NULL;

	/* 
	 * sort the index by node priority (from low to high)
	 */

	qsort ( (char*)index, len, sizeof (NODE*), 
	       (QuicksortCompareProc)sortCmpUpperPrio);

	tmp = index;
	while (*tmp) {
		newx = sortAvrgUpperX (*tmp) ;
		sortMove (*tmp, newx, dist);
		tmp++;
	}

	free ( (char*) index);
}

/*
 * sortLevelDownX
 * assign x-ccordinates to each node of a level. The preferred x-position 
 * of a nodes is the average x-position of its descendants. For resolving
 * conflicts each node gets a priority depending on its number of
 * descendants. Nodes with high priority will be placed on its preferred
 * position with a higher probability.
 */

void Layout::sortLevelDownX (NODE **level, int dist)

{
	NODE **index;
	NODE **tmp;
	NODE *node;

	int len = levelsLength (level);
	int newx;

	index = (NODE**) malloc (sizeof (NODE*) * (len+1));
	if (!index) {
		fprintf (stderr,"sortLevelDownX: out of memory!\n");
		exit (MEMORY_ERROR);
	}

	/* fill the index */
	
	tmp = index;
	node = *level;
	while (node) {
		*(tmp++) = node;
		node = node->right;
	}
	*tmp = (NODE*) NULL;

	/* 
	 * sort the index by node priority (from low to high)
	 */

	qsort ( (char*)index, len, sizeof (NODE*), 
	       (QuicksortCompareProc)sortCmpLowerPrio);

	tmp = index;
	while (*tmp) {
		newx = sortAvrgLowerX (*tmp) ;
		sortMove (*tmp, newx, dist);
		tmp++;
	}

	free ( (char*) index);
}

/*
 * sortLeftSpace
 * return the free space to the left of a node. The free space is the 
 * ammount of space you can push all nodes to the left without falling
 * below the minimum distance between two nodes.
 */

int Layout::sortLeftSpace (NODE *node, int dist) 

{
	int space = 0;
	NODE *left;
	
	left = node->left;
	while (left) {
		space += node->x - left->x - dist;
		if (node->type == Regular) {
			space -= node->attr.node.w / 2;
		}
		if (left->type == Regular) {
			space -= left->attr.node.w / 2;
		}
		node = left;
		left = left->left;
	}
	space += node->x ;
	if (node->type == Regular) {
		space -= node->attr.node.w / 2;
	}
	return space;
}

/*
 * sortMoveLeft
 * move a given node to the left. All nodes to the left of the node
 * will be moved to the left if required but the minimum distance between 
 * two nodes won't be influenced. The function assumes, that there is at least
 * enough space to the left of the node!
 */

void Layout::sortMoveLeft (NODE *node, int newx, int dist)

{
	int maxleftx = 0;
	int ready    = 0;

	do {
		node->x = newx;
		if (node->left) {
			maxleftx = newx - dist;
			if (node->type == Regular) {
				maxleftx -= node->attr.node.w / 2;
			}
			if (node->left->type == Regular) {
				maxleftx -= node->left->attr.node.w / 2;
			}
			ready = ( node->left->x < maxleftx );
			
		}
		node = node->left;			
		newx = maxleftx;

	} while (node && !ready) ;
	
}

/*
 * sortMoveRight
 * move a given node to the right. All nodes to the right of the node
 * will be moved to the right if required but the minimum distance between 
 * two nodes won't be influenced. 
 */

void Layout::sortMoveRight (NODE *node, int newx, int mindist)

{
	int minrightx = 0;
	int ready     = 0;

	do {
		node->x = newx;

		if (node->right) {
			minrightx = newx + mindist ;
			if (node->type == Regular) {
				minrightx += node->attr.node.w /2;
			}
			if (node->right->type == Regular) {
				minrightx += node->right->attr.node.w /2;
			}
		}
		ready =  !(node->right) ||  (node->right->x > minrightx );

		node = node->right;
		newx = minrightx;

	} while (!ready) ;
}

/*
 * sortMove
 * move a node to a new x-ccordinate. All other nodes of the same level
 * will be moved, too if required. The minimum distance between two 
 * node won't be influenced.
 */ 

void Layout::sortMove (NODE *node, int newx, int mindist)

{
	int leftspace;
	int oldx;
	int move;

	oldx = node->x;
	if (newx < oldx) {
		move = oldx - newx;
		leftspace = sortLeftSpace (node, mindist);
		if ( move > leftspace ) {
			newx = oldx - leftspace;
		}
		sortMoveLeft (node,newx,mindist);
	} else if (newx > oldx) {
		sortMoveRight (node,newx,mindist);
	}
}
	
/*
 * sortAvrgUpperX
 * determine the average x-position of a nodes anchestors.
 */

int Layout::sortAvrgUpperX (NODE *node)

{
	EDGE *edge;
	int sumx = 0;
	int count = 0;

	if (node->type == Regular) {
		edge = node->attr.node.up.head ;
		while (edge) {
			sumx += edge->node->x;
			count++;
			edge = edge->next;
		}
	} else if (node->attr.hint.up) {
		sumx = node->attr.hint.up->x;
		count = 1;
	}
	
	if (count) {
		return (sumx / count);
	} else {
		return ((node->x * 3) / 4);
	}
}
/*
 * sortAvrgLowerX
 * determine the average x-position of a nodes descendants.
 */

int Layout::sortAvrgLowerX (NODE *node)

{
	EDGE *edge;
	int sumx = 0;
	int count = 0;

	if (node->type == Regular) {
		edge = node->attr.node.down.head ;
		while (edge) {
			sumx += edge->node->x;
			count++;
			edge = edge->next;
		}
	} else if (node->attr.hint.down) {
		sumx = node->attr.hint.down->x;
		count = 1;
	}
	
	if (count) {
		return (sumx / count);
	} else {
		return ((node->x * 3) / 4);
	}
}

/*
 * sortCmpUpperPrio
 * compare two nodes by its x-layout-priority: hint nodes will have
 * the highest priority, other nodes will have a priority equal to
 * their number of anchestors.
 */

int Layout::sortCmpUpperPrio (NODE **fst, NODE **snd)

{
	int fstprio, sndprio;

	if ( (*fst)->type == Hint ) {
		fstprio = HINTPRIO;
	} else {
		fstprio = (*fst)->attr.node.up.length ;
	}
	if ( (*snd)->type == Hint ) {
		sndprio = HINTPRIO;
	} else {
		sndprio = (*snd)->attr.node.up.length ;
	}

	return (fstprio - sndprio);
}

/*
 * sortCmpLowerPrio
 * compare two nodes by its x-layout-priority: hint nodes will have
 * the highest priority, other nodes will have a priority equal to
 * their number of descendants.
 */

int Layout::sortCmpLowerPrio (NODE **fst, NODE **snd)

{
	int fstprio, sndprio;

	if ( (*fst)->type == Hint ) {
		fstprio = HINTPRIO;
	} else {
		fstprio = (*fst)->attr.node.down.length ;
	}
	if ( (*snd)->type == Hint ) {
		sndprio = HINTPRIO;
	} else {
		sndprio = (*snd)->attr.node.down.length ;
	}

	return (fstprio - sndprio);
}

/*
 * sortLevelVertical
 * apply y-coordinates to all nodes of a level. Attention: the coordinates 
 * of a node represent its center! The function will return the highest
 * y-coordinate covered by the level. 
 */

int Layout::sortLevelVertical (NODE **level, int miny, int minydist)

{
	int length = 0 ;
	int maxheight = 0;
	int newy;
	NODE *node;

	/* 
	 * calculate the max. height of the nodes at this level
	 */

	node = *level;
	while (node) {
		if (node->type == Regular && 
		    node->attr.node.h > maxheight) {
			maxheight = node->attr.node.h;
		}
		length++;
		node = node->right;
	}
	/*
	 * the distance to the previous level will be 'minydist'
	 * and some additional dynamic space.
	 */
	
	newy = miny + minydist + maxheight / 2 + (length * maxheight) / 5;

	node = *level;
	while (node) {
		node->y = newy;
		node = node->right;
	}

	return newy + maxheight / 2;
}
	
/*
 * sortGraphVertical
 * apply a y-coordinate to each node.
 */

void Layout::sortGraphVertical (GRAPH *graph)

{
	int y = (- graph->minydist) + 1; /* for top level  */
	NODE **level;
	int i;
	
	if (!graph->levels) {
		/* nothing to do */
		return;
	}

	level = graph->level+(graph->levels-1);
	for (i = 0; i < graph->levels; i++) {
		y = sortLevelVertical (level,y, graph->minydist);
		level-- ;
	} 
}
