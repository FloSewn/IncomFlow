/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef INCOMFLOW_ICFTRI_H
#define INCOMFLOW_ICFTRI_H

#include "incomflow/icfTypes.h"

/**********************************************************
* icfTri:  
*                   n[2]
*                   /| \
*                  / |  \
*                 /  |   \
*    (t[1])      /   |    \     (t[0])
*               /    |     \
*         e[2] /     |      \ e[1]
*             /      |       \
*            /       |        \
*           / t_c[0] | t_c[1]  \
*          /         |          \
*         /          n_c         \
*       n[0]-------------------->n[1]
*                   e[0]
*
*                  (t[2])
*
*
**********************************************************/
typedef struct icfTri {

  /*-------------------------------------------------------
  | Parents
  -------------------------------------------------------*/
  icfMesh *mesh;
  icfTri  *parent;

  /*-------------------------------------------------------
  | Children
  -------------------------------------------------------*/
  icfTri  *t_c[2];

  /*-------------------------------------------------------
  | Node that connects this triangle to all its
  | trianlge/edge siblings in the refinement tree 
  -------------------------------------------------------*/
  icfNode *n_c;

  /*-------------------------------------------------------
  | Defining nodes: 
  -------------------------------------------------------*/
  icfNode *n[3];

  /*-------------------------------------------------------
  | Defining edges: 
  -------------------------------------------------------*/
  icfEdge *e[3];
  icfEdge *e_split;

  /*-------------------------------------------------------
  | Adjacent triangles: 
  -------------------------------------------------------*/
  icfTri *t[3];

  /*-------------------------------------------------------
  | Triangle properties
  -------------------------------------------------------*/
  icfIndex  index; 
  icfBool   split;
  icfBool   merge;
  icfBool   isSplit;
  icfIndex  treeLevel;
  icfBool   isLeaf;

  /*-------------------------------------------------------
  | Geometric triangle properties
  -------------------------------------------------------*/
  icfDouble xy[2];       /*            Triangle centroid */
  icfDouble aspectRatio; /*                 Aspect ratio */
  icfDouble area;        /*                         Area */

  /*-------------------------------------------------------
  | Position of this triangle in mesh stack 
  -------------------------------------------------------*/
  icfListNode *stackPos;

} icfTri;


/**********************************************************
* Function: icfTri_create
*----------------------------------------------------------
* Create a new tri structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new tri structure
**********************************************************/
icfTri *icfTri_create(icfMesh *mesh);

/**********************************************************
* Function: icfTri_destroy
*----------------------------------------------------------
* Destroys a tri structure
* @param: tri - pointer to tri structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfTri_destroy(icfTri *tri);

/**********************************************************
* Function: icfTri_setNodes
*----------------------------------------------------------
* Sets the nodes for an icfTri
* @param: tri - triangle structure to define nodes for
* @param: n0, n1, n2 - pointer to triangle nodes 
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_setNodes(icfTri  *tri, 
                     icfNode *n0,
                     icfNode *n1,
                     icfNode *n2);

/**********************************************************
* Function: icfTri_setEdges
*----------------------------------------------------------
* Sets the edges for an icfTri
* @param: tri - triangle structure to define edges for
* @param: e0, e1, e2 - pointer to triangle edges 
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_setEdges(icfTri  *tri, 
                     icfEdge *e0,
                     icfEdge *e1,
                     icfEdge *e2);

/**********************************************************
* Function: icfTri_setTris
*----------------------------------------------------------
* Sets the neighboring triangles for an icfTri
* @param: tri - triangle structure to define tris for
* @param: t0, t1, t2 - pointer to triangle neighbors 
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_setTris(icfTri *tri, 
                    icfTri *t0,
                    icfTri *t1,
                    icfTri *t2);

/**********************************************************
* Function: icfTri_markToRefine
*----------------------------------------------------------
* Marks a triangle and its associated longest edge for 
* refinement. Its neighbors are then marked too.
* @param: tri - triangle structure to define tris for
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_markToSplit(icfTri *tri);

/**********************************************************
* Function: icfTri_markToMerge
*----------------------------------------------------------
* Marks a triangle and its associated refinement tree 
* siblings for merge
* @param: tri - triangle structure 
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_markToMerge(icfTri *tri);

#endif
