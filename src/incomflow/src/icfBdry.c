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
#include "incomflow/icfEdge.h"
#include "incomflow/icfTri.h"
#include "incomflow/icfMesh.h"
#include "incomflow/icfBdry.h"
#include "incomflow/icfFlowData.h"

/**********************************************************
* Function: icfBdry_create
*----------------------------------------------------------
* Create a new boundary structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new bdry structure
**********************************************************/
icfBdry *icfBdry_create(icfMesh *mesh, 
                        icfIndex type, 
                        char    *name)
{

  icfBdry *bdry = (icfBdry*) calloc(1, sizeof(icfBdry));
  check_mem(bdry);

  /*-------------------------------------------------------
  | Bdry nodes 
  -------------------------------------------------------*/
  bdry->nNodes = 0;
  bdry->nodeStack = icfList_create();
  bdry->bdryNodes = (icfNode**) calloc(0, sizeof(icfNode*));

  /*-------------------------------------------------------
  | Bdry edges 
  -------------------------------------------------------*/
  bdry->nEdges = 0;
  bdry->edgeStack = icfList_create();

  /*-------------------------------------------------------
  | Bdry edge leafs
  -------------------------------------------------------*/
  bdry->nEdgeLeafs = 0;
  bdry->edgeLeafs = (icfEdge**) calloc(0, sizeof(icfEdge*));

  /*-------------------------------------------------------
  | Boundary properties
  -------------------------------------------------------*/
  bdry->type = type;
  bdry->name = name;

  /*-------------------------------------------------------
  | Position in mesh's bdry stack
  -------------------------------------------------------*/
  bdry->stackPos = icfMesh_addBdry(mesh, bdry);

  return bdry;
error:
  return NULL;

} /* icfBdry_create() */

/**********************************************************
* Function: icfBdry_destroy
*----------------------------------------------------------
* Destroys a bdry structure
* @param: bdry - pointer to bdry structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfBdry_destroy(icfBdry *bdry)
{
  /*-------------------------------------------------------
  | Free all list structures 
  -------------------------------------------------------*/
  icfList_destroy(bdry->nodeStack);
  icfList_destroy(bdry->edgeStack);

  /*-------------------------------------------------------
  | Free all arrays 
  -------------------------------------------------------*/
  free(bdry->edgeLeafs);
  free(bdry->bdryNodes);

  free(bdry);

  return 0;
} /* icfBdry_destroy() */


/**********************************************************
* Function: icfBdry_addNode
*----------------------------------------------------------
* Adds a node to a bdry structure
* @param: bdry - pointer to bdry structure 
* @param: node - pointer to node structure
* @param: index - 0/1 index of boundary in node's bdry
*----------------------------------------------------------
* @return: 
**********************************************************/
void icfBdry_addNode(icfBdry *bdry, 
                     icfNode *node, 
                     icfIndex index)
{
  check( index == 0 || index == 1, 
      "Wrong index value for icfBdry_addNode()");

  bdry->nNodes += 1;
  icfList_push(bdry->nodeStack, node);
  icfListNode *nodePos = icfList_last_node(bdry->nodeStack);

  node->bdry[index]         = bdry;
  node->bdryStackPos[index] = nodePos;

  return;
error:
  return;

} /*icfBdry_addNode() */

/**********************************************************
* Function: icfBdry_addEdge
*----------------------------------------------------------
* Adds an edge to a bdry structure and connects the
* boundary to the edge
* @param: bdry - pointer to bdry structure 
* @param: edge - pointer to edge structure
*----------------------------------------------------------
* @return: 
**********************************************************/
void icfBdry_addEdge(icfBdry *bdry, icfEdge *edge)
{
  bdry->nEdges += 1;
  icfList_push(bdry->edgeStack, edge);
  icfListNode *edgePos = icfList_last_node(bdry->edgeStack);

  edge->bdry         = bdry;
  edge->bdryStackPos = edgePos;

} /* icfBdry_addEdge() */

/**********************************************************
* Function: icfBdry_remNode()
*----------------------------------------------------------
* Function to remove an icfNode from an icfBdry
* Clears also the connectivity of the boundary in the node
*----------------------------------------------------------
* 
**********************************************************/
void icfBdry_remNode(icfBdry *bdry, icfNode *node)
{
  icfIndex index;
  if (bdry == node->bdry[0])
    index = 0;
  else if (bdry == node->bdry[1])
    index = 1;
  else  
    log_err("icfBdry_remNode(): bdry not part of node.");

  icfList_remove(bdry->nodeStack, node->bdryStackPos[index]);
  bdry->nNodes -= 1;

  node->bdry[index]         = NULL;
  node->bdryStackPos[index] = NULL;

  return;
error:
  return;
} /* tmBdrBdry_remEdge() */

/**********************************************************
* Function: icfBdry_remEdge()
*----------------------------------------------------------
* Function to remove an icfEdge from an icfBdry
*----------------------------------------------------------
* 
**********************************************************/
void icfBdry_remEdge(icfBdry *bdry, icfEdge *edge)
{
  check(bdry == edge->bdry, 
      "icfBdry_remEdge(): bdry not part of edge.");

  icfList_remove(bdry->edgeStack, edge->bdryStackPos);
  bdry->nEdges -= 1;

  edge->bdry         = NULL;
  edge->bdryStackPos = NULL;

  return;
error:
  return;
} /* tmBdrBdry_remEdge() */

