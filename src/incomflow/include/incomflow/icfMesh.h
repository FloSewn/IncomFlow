/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef INCOMFLOW_ICFMESH_H
#define INCOMFLOW_ICFMESH_H

/**********************************************************
* icfMesh:  
**********************************************************/
typedef struct icfMesh {

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  int      nNodes;
  icfList *nodeStack;

  /*-------------------------------------------------------
  | Mesh edges 
  -------------------------------------------------------*/
  int      nEdges;
  icfList *edgeStack;

  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  int      nTris;
  icfList *triStack;

} icfMesh;


/**********************************************************
* Function: icfMesh_create
*----------------------------------------------------------
* Create a new mesh structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new mesh structure
**********************************************************/
icfMesh *icfMesh_create(void);

/**********************************************************
* Function: icfMesh_destroy
*----------------------------------------------------------
* Destroys a mesh structure
* @param: mesh - pointer to mesh structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfMesh_destroy(icfMesh *mesh);

/**********************************************************
* Function: icfMesh_addNode()
*----------------------------------------------------------
* Function to add an icfNode to an icfMesh
*----------------------------------------------------------
* @return: icfNode list node on the mesh's node stack
**********************************************************/
icfListNode *icfMesh_addNode(icfMesh *mesh, icfNode *node);

/**********************************************************
* Function: icfMesh_addEdge()
*----------------------------------------------------------
* Function to add an icfEdge to an icfMesh
*----------------------------------------------------------
* @return: icfEdge list node on the mesh's edge stack
**********************************************************/
icfListNode *icfMesh_addEdge(icfMesh *mesh, icfEdge *edge);

/**********************************************************
* Function: icfMesh_addTri()
*----------------------------------------------------------
* Function to add an icfTri to an icfMesh
*----------------------------------------------------------
* @return: icfTri list node on the mesh's tri stack
**********************************************************/
icfListNode *icfMesh_addTri(icfMesh *mesh, icfTri *tri);

/**********************************************************
* Function: icfMesh_remNode()
*----------------------------------------------------------
* Function to remove an icfNode from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remNode(icfMesh *mesh, icfNode *node);

/**********************************************************
* Function: icfMesh_remEdge()
*----------------------------------------------------------
* Function to remove an icfEdge from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remEdge(icfMesh *mesh, icfEdge *edge);

/**********************************************************
* Function: icfMesh_remTri()
*----------------------------------------------------------
* Function to remove an icfTri from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remTri(icfMesh *mesh, icfTri *tri);

/**********************************************************
* Function: icfMesh_refine()
*----------------------------------------------------------
* Function to refine an icfMesh mesh structure
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_refine(icfFlowData *flowData, icfMesh *mesh);

/**********************************************************
* Function: icfMesh_printMesh()
*----------------------------------------------------------
* Fuction to print out the mesh data
*----------------------------------------------------------
* @param mesh: pointer to mesh structure
**********************************************************/
void icfMesh_printMesh(icfMesh *mesh);

#endif
