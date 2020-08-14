/*
 * This source file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#include "incomflow/icfTypes.h"
#include "incomflow/icfList.h"
#include "incomflow/icfFlowData.h"
#include "incomflow/icfMesh.h"
#include "incomflow/icfNode.h"
#include "incomflow/icfEdge.h"
#include "incomflow/icfTri.h"

/**********************************************************
* Function: icfMesh_create
*----------------------------------------------------------
* Create a new mesh structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new mesh structure
**********************************************************/
icfMesh *icfMesh_create(void)
{
  icfMesh *mesh = (icfMesh*) calloc(1, sizeof(icfMesh) );
  check_mem(mesh);

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  mesh->nNodes = 0;
  mesh->nodeStack = icfList_create();

  /*-------------------------------------------------------
  | Mesh edges 
  -------------------------------------------------------*/
  mesh->nEdges = 0;
  mesh->edgeStack = icfList_create();

  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  mesh->nTris = 0;
  mesh->triStack = icfList_create();


  return mesh;
error:
  return NULL;

} /* icfMesh_create() */

/**********************************************************
* Function: icfMesh_destroy
*----------------------------------------------------------
* Destroys a mesh structure
* @param: mesh - pointer to mesh structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfMesh_destroy(icfMesh *mesh)
{
  icfListNode *cur, *nxt;

  /*-------------------------------------------------------
  | Free all edges on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->edgeStack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    icfEdge_destroy(cur->value);
    cur = nxt;
  }
#if (ICF_DEBUG > 0)
  icfPrint("MESH EDGES FREE");
#endif

  /*-------------------------------------------------------
  | Free all tris on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->triStack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    icfTri_destroy(cur->value);
    cur = nxt;
  }
#if (ICF_DEBUG > 0)
  icfPrint("MESH TRIANGLES FREE");
#endif

  /*-------------------------------------------------------
  | Free all nodes on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->nodeStack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    icfNode_destroy(cur->value);
    cur = nxt;
  }
#if (ICF_DEBUG > 0)
  icfPrint("MESH NODES FREE");
#endif

  /*-------------------------------------------------------
  | Free all mesh list structures
  -------------------------------------------------------*/
  icfList_destroy(mesh->nodeStack);
  icfList_destroy(mesh->edgeStack);
  icfList_destroy(mesh->triStack);

  /*-------------------------------------------------------
  | Finally free mesh structure memory
  -------------------------------------------------------*/
  free(mesh);

  return 0;

error:
  return -1;

} /* icfMesh_create() */


/**********************************************************
* Function: icfMesh_addNode()
*----------------------------------------------------------
* Function to add an icfNode to an icfMesh
*----------------------------------------------------------
* @return: icfNode list node on the mesh's node stack
**********************************************************/
icfListNode *icfMesh_addNode(icfMesh *mesh, icfNode *node)
{
  mesh->nNodes += 1;
  icfList_push(mesh->nodeStack, node);
  icfListNode *nodePos = icfList_last_node(mesh->nodeStack);
  return nodePos;
} /* icfMesh_addNode() */

/**********************************************************
* Function: icfMesh_addEdge()
*----------------------------------------------------------
* Function to add an icfEdge to an icfMesh
*----------------------------------------------------------
* @return: icfEdge list node on the mesh's edge stack
**********************************************************/
icfListNode *icfMesh_addEdge(icfMesh *mesh, icfEdge *edge)
{
  mesh->nEdges += 1;
  icfList_push(mesh->edgeStack, edge);
  icfListNode *edgePos = icfList_last_node(mesh->edgeStack);
  return edgePos;
} /* icfMesh_addEdge() */

/**********************************************************
* Function: icfMesh_addTri()
*----------------------------------------------------------
* Function to add an icfTri to an icfMesh
*----------------------------------------------------------
* @return: icfTri list node on the mesh's tri stack
**********************************************************/
icfListNode *icfMesh_addTri(icfMesh *mesh, icfTri *tri)
{
  mesh->nTris += 1;
  icfList_push(mesh->triStack, tri);
  icfListNode *triPos = icfList_last_node(mesh->triStack);
  return triPos;
} /* icfMesh_addTri() */

/**********************************************************
* Function: icfMesh_remNode()
*----------------------------------------------------------
* Function to remove an icfNode from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remNode(icfMesh *mesh, icfNode *node)
{
  icfList_remove(mesh->nodeStack, node->stackPos);
  mesh->nNodes -= 1;
} /* tmMesh_remNode() */

/**********************************************************
* Function: icfMesh_remEdge()
*----------------------------------------------------------
* Function to remove an icfEdge from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remEdge(icfMesh *mesh, icfEdge *edge)
{
  icfList_remove(mesh->edgeStack, edge->stackPos);
  mesh->nEdges -= 1;
} /* tmMesh_remEdge() */

/**********************************************************
* Function: icfMesh_remTri()
*----------------------------------------------------------
* Function to remove an icfTri from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remTri(icfMesh *mesh, icfTri *tri)
{
  icfList_remove(mesh->triStack, tri->stackPos);
  mesh->nTris -= 1;
} /* tmMesh_remTri() */

/**********************************************************
* Function: icfMesh_refine()
*----------------------------------------------------------
* Function to refine an icfMesh mesh structure
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_refine(icfFlowData *flowData, icfMesh *mesh)
{
  icfListNode *cur;

  icfRefineFun refineFun = flowData->refineFun;
  check(refineFun != NULL,
      "Refinement function has not been defined.");

  /*-------------------------------------------------------
  | Mark all triangles and respective edges to refine
  -------------------------------------------------------*/
  for (cur = mesh->triStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfTri *t = (icfTri*)cur->value;

    if (refineFun(flowData, t) == TRUE)
      icfTri_markToSplit(t);
  }


  return;
error:
  return;

} /* icfMesh_refine() */
