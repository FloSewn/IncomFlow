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
  | Node coordinates 
  -------------------------------------------------------*/
  icfDouble xy[2];
  icfIndex  index; 

  /*-------------------------------------------------------
  | Position of this node in the mesh stack 
  -------------------------------------------------------*/
  icfListNode *stackPos;

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
