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
#include "incomflow/icfMesh.h"
#include "incomflow/icfFlowData.h"

/**********************************************************
* Function: icfEdge_create
*----------------------------------------------------------
* Create a new edge structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new edge structure
**********************************************************/
icfEdge *icfEdge_create(icfMesh *mesh) 
{
  icfEdge *edge = (icfEdge*) calloc(1, sizeof(icfEdge));
  check_mem(edge);

  /*-------------------------------------------------------
  | Parents
  -------------------------------------------------------*/
  edge->mesh   = mesh;
  edge->parent = NULL;

  /*-------------------------------------------------------
  | Children
  -------------------------------------------------------*/
  edge->e_c[0] = NULL;
  edge->e_c[1] = NULL;
  edge->n_c    = NULL;

  /*-------------------------------------------------------
  | Edge nodes: edge points from n[0] to n[1] 
  -------------------------------------------------------*/
  edge->n[0] = NULL;
  edge->n[1] = NULL;

  /*-------------------------------------------------------
  | Adjacent triangles: t0 is left, t1 is right
  -------------------------------------------------------*/
  edge->t[0] = NULL;
  edge->t[1] = NULL;

  /*-------------------------------------------------------
  | Edge properties
  -------------------------------------------------------*/
  edge->index = -1;
  edge->split = FALSE;

  /*-------------------------------------------------------
  | Position in mesh's edge stack
  -------------------------------------------------------*/
  edge->stackPos = icfMesh_addEdge(mesh, edge);

  return edge;
error:
  return NULL;

} /* icfEdge_create() */

/**********************************************************
* Function: icfEdge_destroy
*----------------------------------------------------------
* Destroys a edge structure
* @param: edge - pointer to edge structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfEdge_destroy(icfEdge *edge)
{
  free(edge);
  return 0;
} /* icfEdge_destroy() */

/**********************************************************
* Function: icfEdge_setNodes
*----------------------------------------------------------
* Sets the nodes for an icfEdge and calculates the 
* edge length.
* @param: edge - edge structure to define nodes for
* @param: n0, n1 - pointer to edge nodes 
*----------------------------------------------------------
* 
**********************************************************/
void icfEdge_setNodes(icfEdge *edge, 
                      icfNode *n0,
                      icfNode *n1)
{
  edge->n[0] = n0;
  edge->n[1] = n1;

  /*-------------------------------------------------------
  | Compute edge length
  -------------------------------------------------------*/
  icfDouble dx = n1->xy[0] - n0->xy[0];
  icfDouble dy = n1->xy[1] - n0->xy[1];
  edge->len    = sqrt(dx*dx + dy*dy);

} /*icfEdge_setNodes() */

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
                     icfTri  *t1)
{
  edge->t[0] = t0;
  edge->t[1] = t1;
} /*icfEdge_setTris() */
