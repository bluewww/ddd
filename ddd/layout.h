// $Id$ -*- C++ -*-
// Graph layout functions

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Christian Lindig (lindig@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef _Nora_layout_h
#define _Nora_layout_h

#ifdef __GNUG__
#pragma interface
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// All these should be defined in the LayouterExpert class,
// but I can't get gcc 2.3.3 swallow it...
enum EDGEARROW { Here, Other };
enum NODETYPE { Regular, Hint };

typedef struct _EGDE EDGE;
typedef struct _EDGELIST EDGELIST;
typedef struct _REGULAR REGULAR;
typedef struct _HINT HINT;
typedef union _ATTRIBUTES ATTRIBUTES;
typedef union _ID ID;
typedef struct _NODE NODE;
typedef struct _GRAPH GRAPH;

#define PRIME 809
#define SMALLPRIME 41
typedef GRAPH *GRAPHTAB[SMALLPRIME];

// More definitions
struct _EGDE {
    NODE *node;
    NODE *target;
    EDGEARROW arrow;        /* kind of edge */
    EDGE *next;             /* next entry of list */
    EDGE *prev;             /* previous entry */
};

struct _EDGELIST {
    EDGE *head;             /* first entry */
    EDGE *tail;             /* last entry */
    int length;             /* number of elements */
};

struct _REGULAR {
    char *label;            /* the name of the node */
    int w,h;                /* regular nodes have width & height */
    EDGELIST up;            /* ancestors */
    EDGELIST down;          /* descendants */
};
struct _HINT {
    int id;                 /* hints are identified by ids */
    NODE *up;
    NODE *down;
    NODE *source;           /* the two nodes connected by */
    NODE *target;           /* the edge this hint belongs to */
};

union _ATTRIBUTES {
    REGULAR node;
    HINT hint;
};

union _ID {
    int id;
    char *label;
};

struct _NODE {
    int x,y;                /* position */
    int oldx,oldy;          /* previous position */

    int layouted;           /* flag: already layouted? */
    int level;              /* level inside graph */
    int center;             /* avrg. bary-center of node */
    int loop;               /* flag for loop */
    int index;              /* auxilliary */
    NODE *mark;             /* auxilliary */

    NODE *left;             /* same level - to the left */
    NODE *right;            /* same level - to the right */
    
    NODE *hashnext;         /* double linked list for hashtab */
    NODE *hashprev;

    NODETYPE type ;         /* type of node and tag for union */
    ATTRIBUTES attr;
};

struct _GRAPH {
    NODE *hashtab[PRIME];

    int levels;
    NODE **level;    
    char *label;
    struct GRAPH *hashnext;
    struct GRAPH *hashprev;

    int minxdist;
    int minydist;
    int reverseflag;
    int xiterations;
    int pullup;

    int layouted;           /* flag, if graph was layouted recently */
};

// Interface
class Layout {
public:
    static void add_graph(char *g);
    static void add_node(char *g, char *node);
    static void add_edge(char *g, char *node1, char *node2);
    static void set_node_width(char *g, char *node, int width);
    static void set_node_height(char *g, char *node, int height);
    static void set_node_position(char *g, char *node, int x, int y);
    static void add_edge_hint(char *g, char *node1, char *node2, 
			      int x, int y);
    static void remove_edge_hint(char *g, char *node1, char *node2, 
				 int x, int y);
    static void remove_edge(char *g, char *node1, char *node2);
    static void remove_node(char *g, char *label);
    static void remove_graph(char *g);
    static void layout(char *g);
    
    static void (*node_callback)(char *, int, int);
    static void (*hint_callback)(char *, char *, int, int);
    static int (*compare_callback)(char *, char *);


private:
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

    // Data
    // static GRAPHTAB tab;

    // Helpers
    static void noraDebug(char *g);
    static void inc_layout(GRAPH *graph);
    static void new_layout(GRAPH *graph);
    static void noraOutput(GRAPH *graph);
    static void noraNodeOut(char *graph,NODE *node);
    static void debugNode(NODE *node);
    static void debugLevel(GRAPH *graph, int n);
    static void debugAllLevel(GRAPH *graph);
    static void debugAllNodes(GRAPH *graph);
    static void debugNodeXFig(NODE *nd);
    static void debugEdgeXFig(NODE *source, NODE *target, int arrow);
    static void debugGraphXFig(GRAPH *graph);
    static void listInit(EDGELIST *list);
    static EDGE *listInsertEdge(EDGELIST *list, NODE *node);
    static void listRemoveEdge(EDGELIST *list, EDGE *edge);
    static EDGE *listFindNode(EDGELIST *list, NODE *node);
    static EDGE *listFindTarget(EDGELIST *list, NODE *target);
    static void listRemove(EDGELIST *list);
    static void nodeInit(NODE* node, ID *id , NODETYPE type);
    static void nodeRemove(NODE *node);
    static void graphInit(GRAPH *graph, char *label);
    static NODE *graphEnterNode(GRAPH *graph, ID *id, NODETYPE type);
    static NODE *graphGetNode(GRAPH *graph, ID *id, NODETYPE type);
    static void graphRemoveNode(GRAPH *graph, ID *id, NODETYPE type);
    static void graphCreateLevels(GRAPH *graph, int n);
    static void graphRemoveLevels(GRAPH *graph);
    static void graphAddLevels(GRAPH *graph, int n);
    static void graphInsertEdge(GRAPH *graph,NODE *source,NODE *target);
    static void graphInvertEdge(NODE *source, NODE *target);
    static int graphNewNodeID();
    static NODE *graphInsertHint(GRAPH *graph, NODE *source, NODE* target);
    static EDGE *graphFindEdgeAtSource(NODE *source, NODE *target);
    static EDGE *graphFindEdgeAtTarget(NODE *source, NODE *target);
    static void graphResetLevels(GRAPH *graph);
    static int graphHashStr(char *str, int prime);
    static GRAPH *graphGet(GRAPHTAB *tab, char *label);
    static GRAPH *graphNew(GRAPHTAB *tab,char *label);
    static void graphRemove(GRAPHTAB *tab, char *label);
    static void graphTabInit(GRAPHTAB *tab);
    static void levelsInsertNode(GRAPH *graph, NODE *node, int n);
    static void levelsRemoveNode(GRAPH *graph, NODE *node, int n);
    static void levelsEnterNodes(GRAPH *graph, int pullup);
    static void levelsIndex(NODE **level);
    static int levelsLength(NODE **level);
    static int sortApplyLevel(GRAPH *graph);
    static void sortPullupNodes(GRAPH *graph);
    static int minimumLevel(NODE *node);
    static int distance(NODE *node, NODE *origin);
    static void sortInsertHints(GRAPH *graph);
    static void sortCheckNode(GRAPH *graph, NODE *node);
    static int sortNodeUpperBary(NODE *node);
    static int sortNodeLowerBary(NODE *node);
    static void sortGraphUpperBary(GRAPH *graph);
    static void sortGraphLowerBary(GRAPH *graph);
    static void sortByCenter(NODE **level);
    static void sortAvrgCenter(GRAPH *graph);
    static int sortCmpCenters(NODE **first, NODE **second);
    static void sortInitX(GRAPH *graph);
    static void sortGraphUpX(GRAPH *graph);
    static void sortGraphDownX(GRAPH *graph);
    static void sortLevelUpX(NODE **level, int dist);
    static void sortLevelDownX(NODE **level, int dist);
    static int sortLeftSpace(NODE *node, int dist);
    static void sortMoveLeft(NODE *node, int newx, int dist);
    static void sortMoveRight(NODE *node, int newx, int mindist);
    static void sortMove(NODE *node, int newx, int mindist);
    static int sortAvrgUpperX(NODE *node);
    static int sortAvrgLowerX(NODE *node);
    static int sortCmpUpperPrio(NODE **fst, NODE **snd);
    static int sortCmpLowerPrio(NODE **fst, NODE **snd);
    static int sortLevelVertical(NODE **level, int miny, int minydist);
    static void sortGraphVertical(GRAPH *graph);
};

#endif // _Nora_layout_h
// DON'T ADD ANYTHING BEHIND THIS #endif
