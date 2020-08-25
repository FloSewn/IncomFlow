/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef INCOMFLOW_ICFFLOWDATA_H
#define INCOMFLOW_ICFFLOWDATA_H

#include "incomflow/icfTypes.h"

/**********************************************************
* icfFlowData:  
**********************************************************/
typedef struct icfFlowData {

  /*-------------------------------------------------------
  | Mesh  
  -------------------------------------------------------*/
  icfMesh *mesh;

  /*-------------------------------------------------------
  | Refinement / Coarsening functions 
  -------------------------------------------------------*/
  icfRefineFun refineFun;
  icfRefineFun coarseFun;

} icfFlowData;

/**********************************************************
* Function: icfFlowData_create
*----------------------------------------------------------
* Create a new FlowData structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new FlowData structure
**********************************************************/
icfFlowData *icfFlowData_create(void);

/**********************************************************
* Function: icfFlowData_destroy
*----------------------------------------------------------
* Destroys a flowdata structure
* @param: flowData - pointer to data structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfFlowData_destroy(icfFlowData *flowData);


#endif
