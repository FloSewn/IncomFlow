/*
 * This source file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#include "incomflow/icfTypes.h"
#include "incomflow/icfList.h"
#include "incomflow/icfNode.h"
#include "incomflow/icfMesh.h"
#include "incomflow/icfFlowData.h"

/**********************************************************
* Function: icfNode_create
*----------------------------------------------------------
* Create a new node structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new node structure
**********************************************************/
icfNode *icfNode_create(icfMesh *mesh, icfDouble *xy)
{
  icfNode *node = (icfNode*) calloc(1, sizeof(icfNode));
  check_mem(node);

  /*-------------------------------------------------------
  | Parents
  -------------------------------------------------------*/
  node->mesh   = mesh;

  /*-------------------------------------------------------
  | If this node is a refinement node, it contains the
  | connectivity to all neighboring triangles / edges
  | in the respective refinement tree level
  -------------------------------------------------------*/
  node->e_c[0] = NULL;
  node->e_c[1] = NULL;
  node->e_c[2] = NULL;
  node->e_c[3] = NULL;

  node->t_c[0] = NULL;
  node->t_c[1] = NULL;
  node->t_c[2] = NULL;
  node->t_c[3] = NULL;

  /*-------------------------------------------------------
  | Node coordinates 
  -------------------------------------------------------*/
  node->xy[0]  = xy[0];
  node->xy[1]  = xy[1];

  /*-------------------------------------------------------
  | Node index 
  -------------------------------------------------------*/
  node->index  = mesh->nodeStack->count;

  /*-------------------------------------------------------
  | Position of this node in the mesh stack 
  -------------------------------------------------------*/
  node->stackPos = icfMesh_addNode(mesh, node);

  /*-------------------------------------------------------
  | Boundary connectivity
  -------------------------------------------------------*/
  node->bdryStackPos[0] = NULL;
  node->bdryStackPos[1] = NULL;
  node->bdry[0]         = NULL;
  node->bdry[1]         = NULL;

  /*-------------------------------------------------------
  | Median-dual grid metrics
  -------------------------------------------------------*/
  node->vol      = 0.0;

  return node;
error:
  return NULL;

} /* icfNode_create() */

/**********************************************************
* Function: icfNode_destroy
*----------------------------------------------------------
* Destroys a node structure
* @param: node - pointer to node structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfNode_destroy(icfNode *node)
{
  icfMesh_remNode(node->mesh, node);
  free(node);
  return 0;
} /* icfNode_destroy() */
