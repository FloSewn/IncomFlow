/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef INCOMFLOW_ICFNODE_H
#define INCOMFLOW_ICFNODE_H

#include "incomflow/icfTypes.h"

/**********************************************************
* icfNode: Node 
**********************************************************/
typedef struct icfNode {

  /*-------------------------------------------------------
  | Parent mesh properties
  -------------------------------------------------------*/
  icfMesh *mesh;

  /*-------------------------------------------------------
  | If this node is a refinement node, it contains the
  | connectivity to all neighboring triangles / edges
  | in the respective refinement tree level
  |
  |             n3
  |            /^\
  |          /  |  \
  | t3  e3 /   eV1   \ e2  t2
  |      /  tL0 | tL1  \
  |    /        |        \
  |  /          |          \
  | n0---eH0--->n---eH1--->n2
  |  \          ^          /
  |    \        |        /
  |      \  tR0 | tR1  /
  | t0  e0 \    |    / e1  t1
  |          \ eV0 /
  |            \|/
  |             n1
  |
  | e_c = [eH0, eV0, eH1, eV1]
  | t_c = [tR0, tR1, tL1, tL0]
  |
  -------------------------------------------------------*/
  icfEdge *e_c[4];
  icfTri  *t_c[4];

  /*-------------------------------------------------------
  | Node coordinates 
  -------------------------------------------------------*/
  icfDouble xy[2];
  icfIndex  index; 

  /*-------------------------------------------------------
  | Position of this node in the mesh stack 
  -------------------------------------------------------*/
  icfListNode *stackPos;

  /*-------------------------------------------------------
  | Boundary connectivity
  -------------------------------------------------------*/
  icfListNode *bdryStackPos[2];
  icfBdry     *bdry[2];

  /*-------------------------------------------------------
  | Median-dual grid metrics
  -------------------------------------------------------*/
  /* Volumes of associated median-dual elements          */
  icfDouble vol;                    

} icfNode;


/**********************************************************
* Function: icfNode_create
*----------------------------------------------------------
* Create a new node structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new node structure
**********************************************************/
icfNode *icfNode_create(icfMesh *mesh, icfDouble *xy);

/**********************************************************
* Function: icfNode_destroy
*----------------------------------------------------------
* Destroys a node structure
* @param: node - pointer to node structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfNode_destroy(icfNode *node);

#endif
