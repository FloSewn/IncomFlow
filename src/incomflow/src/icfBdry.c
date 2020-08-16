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
  | Bdry edges 
  -------------------------------------------------------*/
  bdry->nEdges = 0;
  bdry->edgeStack = icfList_create();

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
  icfList_destroy(bdry->edgeStack);

  free(bdry);

  return 0;
} /* icfBdry_destroy() */


/**********************************************************
* Function: icfBdry_addEdge
*----------------------------------------------------------
* Adds an edge to a bdry structure
* @param: bdry - pointer to bdry structure 
* @param: edge - pointer to edge structure
*----------------------------------------------------------
* @return: icfNode list node on the bdry's edge stack
**********************************************************/
icfListNode *icfBdry_addEdge(icfBdry *bdry, icfEdge *edge)
{
  edge->bdry = bdry;

  bdry->nEdges += 1;
  icfList_push(bdry->edgeStack, edge);
  icfListNode *edgePos = icfList_last_node(bdry->edgeStack);
  return edgePos;
} /* icfBdry_addEdge() */

/**********************************************************
* Function: icfBdry_remEdge()
*----------------------------------------------------------
* Function to remove an icfEdge from an icfBdry
*----------------------------------------------------------
* 
**********************************************************/
void icfBdry_remEdge(icfBdry *bdry, icfEdge *edge)
{
  icfList_remove(bdry->edgeStack, edge->bdryStackPos);
  bdry->nEdges -= 1;
} /* tmBdrBdry_remEdge() */

