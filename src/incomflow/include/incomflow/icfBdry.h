/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef INCOMFLOW_ICFBDRY_H
#define INCOMFLOW_ICFBDRY_H

#include "incomflow/icfTypes.h"

/**********************************************************
* icfBdry:  
**********************************************************/
typedef struct icfBdry {

  /*-------------------------------------------------------
  | Parents
  -------------------------------------------------------*/
  icfMesh *mesh;

  /*-------------------------------------------------------
  | Boundary nodes
  -------------------------------------------------------*/
  int      nNodes;
  icfList *nodeStack;

  /*-------------------------------------------------------
  | Boundary edges
  -------------------------------------------------------*/
  int      nEdges;
  icfList *edgeStack;

  /*-------------------------------------------------------
  | Boundary properties
  -------------------------------------------------------*/
  icfIndex type;
  char    *name;

  /*-------------------------------------------------------
  | Position in mesh's bdry stack
  -------------------------------------------------------*/
  icfListNode *stackPos;

} icfBdry;

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
                        char    *name);

/**********************************************************
* Function: icfBdry_destroy
*----------------------------------------------------------
* Destroys a bdry structure
* @param: bdry - pointer to bdry structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfBdry_destroy(icfBdry *bdry);

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
                     icfIndex index);

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
void icfBdry_addEdge(icfBdry *bdry, icfEdge *edge);

/**********************************************************
* Function: icfBdry_remNode()
*----------------------------------------------------------
* Function to remove an icfNode from an icfBdry
* Clears also the connectivity of the boundary in the node
*----------------------------------------------------------
* 
**********************************************************/
void icfBdry_remNode(icfBdry *bdry, icfNode *node);

/**********************************************************
* Function: icfBdry_remEdge()
*----------------------------------------------------------
* Function to remove an icfEdge from an icfBdry
*----------------------------------------------------------
* 
**********************************************************/
void icfBdry_remEdge(icfBdry *bdry, icfEdge *edge);

#endif
