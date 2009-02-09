// $Id$ -*- C++ -*-
// Graph layout functions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Free Software Foundation, Inc.
// Written by Christian Lindig <lindig@ips.cs.tu-bs.de>.
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

#ifndef _DDD_layout_h
#define _DDD_layout_h

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
    const char *label;
};

struct _NODE {
    int x,y;                /* position */
    int oldx,oldy;          /* previous position */

    bool layouted;           /* flag: already layouted? */
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
    GRAPH *hashnext;
    GRAPH *hashprev;

    int minxdist;
    int minydist;
    bool reverseflag;
    int xiterations;
    bool pullup;

    bool layouted;           /* flag, if graph was layouted recently */
};

// Interface
class Layout {
public:
    static void add_graph(const char *g);
    static void add_node(const char *g, const char *node);
    static void add_edge(const char *g, const char *node1, const char *node2);
    static void set_node_width(const char *g, const char *node, int width);
    static void set_node_height(const char *g, const char *node, int height);
    static void set_node_position(const char *g, const char *node, int x, int y);
    static void add_edge_hint(const char *g, const char *node1, const char *node2, 
			      int x, int y);
    static void remove_edge_hint(const char *g, const char *node1, const char *node2, 
				 int x, int y);
    static void remove_edge(const char *g, const char *node1, const char *node2);
    static void remove_node(const char *g, const char *label);
    static void remove_graph(const char *g);
    static void layout(const char *g);
    
    static void (*node_callback)(const char *, int, int);
    static void (*hint_callback)(const char *, const char *, int, int);
    static int (*compare_callback)(const char *, const char *);


    // Data
    // static GRAPHTAB tab;

    // Helpers
    static void dddDebug(const char *g);
    static void inc_layout(GRAPH *graph);
    static void new_layout(GRAPH *graph);
    static void dddOutput(GRAPH *graph);
    static void dddNodeOut(const char *graph, NODE *node);
    static void debugNode(NODE *node);
    static void debugLevel(GRAPH *graph, int n);
    static void debugAllLevel(GRAPH *graph);
    static void debugAllNodes(GRAPH *graph);
    static void debugNodeXFig(NODE *nd);
    enum Arrow { HERE=0, OTHER=1, NOTHING=3 };
    static void debugEdgeXFig(NODE *source, NODE *target, Arrow arrow);
    static void debugGraphXFig(GRAPH *graph);
    static void listInit(EDGELIST *list);
    static EDGE *listInsertEdge(EDGELIST *list, NODE *node);
    static void listRemoveEdge(EDGELIST *list, EDGE *edge);
    static EDGE *listFindNode(EDGELIST *list, NODE *node);
    static EDGE *listFindTarget(EDGELIST *list, NODE *target);
    static void listRemove(EDGELIST *list);
    static void nodeInit(NODE* node, const ID *id , NODETYPE type);
    static void nodeRemove(NODE *node);
    static void graphInit(GRAPH *graph, const char *label);
    static NODE *graphEnterNode(GRAPH *graph, const ID *id, NODETYPE type);
    static NODE *graphGetNode(GRAPH *graph, const ID *id, NODETYPE type);
    static void graphRemoveNode(GRAPH *graph, const ID *id, NODETYPE type);
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
    static int graphHashStr(const char *str, int prime);
    static GRAPH *graphGet(GRAPHTAB *tab, const char *label);
    static GRAPH *graphNew(GRAPHTAB *tab, const char *label);
    static void graphRemove(GRAPHTAB *tab, const char *label);
    static void graphTabInit(GRAPHTAB *tab);
    static void levelsInsertNode(GRAPH *graph, NODE *node, int n);
    static void levelsRemoveNode(GRAPH *graph, NODE *node, int n);
    static void levelsEnterNodes(GRAPH *graph, bool pullup);
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

#endif // _DDD_layout_h
// DON'T ADD ANYTHING BEHIND THIS #endif
