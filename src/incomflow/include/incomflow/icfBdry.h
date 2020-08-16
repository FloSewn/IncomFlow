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
* Function: icfBdry_addEdge
*----------------------------------------------------------
* Adds an edge to a bdry structure
* @param: bdry - pointer to bdry structure 
* @param: edge - pointer to edge structure
*----------------------------------------------------------
* @return: icfNode list node on the bdry's edge stack
**********************************************************/
icfListNode *icfBdry_addEdge(icfBdry *bdry, icfEdge *edge);

/**********************************************************
* Function: icfBdry_remEdge()
*----------------------------------------------------------
* Function to remove an icfEdge from an icfBdry
*----------------------------------------------------------
* 
**********************************************************/
void icfBdry_remEdge(icfBdry *bdry, icfEdge *edge);

#endif
