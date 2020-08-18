/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef INCOMFLOW_ICFEDGE_H
#define INCOMFLOW_ICFEDGE_H

#include "incomflow/icfTypes.h"

/**********************************************************
* icfEdge:  
*
*                  t[0]
*
*
*     n[0]-----------(n_c)--------->n[1]
*            e_c[0]         e_c[1]
*
*                  t[1]
*
**********************************************************/
typedef struct icfEdge {

  /*-------------------------------------------------------
  | Parents
  -------------------------------------------------------*/
  icfMesh *mesh;
  icfEdge *parent;

  /*-------------------------------------------------------
  | Children
  -------------------------------------------------------*/
  icfEdge *e_c[4];
  icfNode *n_c;

  /*-------------------------------------------------------
  | Edge nodes: edge points from n[0] to n[1] 
  -------------------------------------------------------*/
  icfNode *n[2];

  /*-------------------------------------------------------
  | Adjacent triangles: t0 is left, t1 is right
  -------------------------------------------------------*/
  icfTri *t[2];

  /*-------------------------------------------------------
  | Boundary properties
  -------------------------------------------------------*/
  icfBdry     *bdry;
  icfListNode *bdryStackPos;

  /*-------------------------------------------------------
  | Edge properties
  -------------------------------------------------------*/
  icfIndex  index; 
  icfBool   split;
  icfBool   isSplit;
  icfIndex  treeLevel;

  /*-------------------------------------------------------
  | Geometric edge properties
  -------------------------------------------------------*/
  icfDouble xy[2]; /*                      Edge centroid */
  icfDouble len;   /*                      Edge length   */

  /*-------------------------------------------------------
  | Position in mesh's edge stack
  -------------------------------------------------------*/
  icfListNode *stackPos;

} icfEdge;


/**********************************************************
* Function: icfEdge_create
*----------------------------------------------------------
* Create a new edge structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new edge structure
**********************************************************/
icfEdge *icfEdge_create(icfMesh *mesh);

/**********************************************************
* Function: icfEdge_destroy
*----------------------------------------------------------
* Destroys a edge structure
* @param: edge - pointer to edge structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfEdge_destroy(icfEdge *edge);

/**********************************************************
* Function: icfEdge_setNodes
*----------------------------------------------------------
* Sets the nodes for an icfEdge
* @param: edge - edge structure to define nodes for
* @param: n0, n1 - pointer to edge nodes 
*----------------------------------------------------------
* 
**********************************************************/
void icfEdge_setNodes(icfEdge *edge, 
                      icfNode *n0,
                      icfNode *n1);

/**********************************************************
* Function: icfEdge_setTris
*----------------------------------------------------------
* Sets the triangles for an icfEdge
* @param: edge - edge structure to define nodes for
* @param: t0, t1 - pointer to edge triangles 
*----------------------------------------------------------
* 
**********************************************************/
void icfEdge_setTris(icfEdge *edge, 
                     icfTri  *t0,
                     icfTri  *t1);

/**********************************************************
* Function: icfEdge_split
*----------------------------------------------------------
* Split a marked edge 
* @param: e - edge structure to split
*----------------------------------------------------------
* 
**********************************************************/
void icfEdge_split(icfEdge *e);

#endif
