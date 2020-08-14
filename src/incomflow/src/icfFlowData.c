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
* Function: icfFlowData_create
*----------------------------------------------------------
* Create a new FlowData structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new FlowData structure
**********************************************************/
icfFlowData *icfFlowData_create(void)
{
  icfFlowData *flowData = (icfFlowData*) calloc(1, sizeof(icfFlowData));
  check_mem(flowData);

  /*-------------------------------------------------------
  | Mesh  
  -------------------------------------------------------*/
  flowData->mesh = NULL;

  /*-------------------------------------------------------
  | Refinement functions 
  -------------------------------------------------------*/
  flowData->refineFun = NULL;

  return flowData;
error:
  return NULL;

} /* icfFlowData_create() */

/**********************************************************
* Function: icfFlowData_destroy
*----------------------------------------------------------
* Destroys a flowdata structure
* @param: flowData - pointer to data structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfFlowData_destroy(icfFlowData *flowData)
{
  /*-------------------------------------------------------
  | Free mesh memory
  -------------------------------------------------------*/
  icfMesh_destroy(flowData->mesh);

  free(flowData);
  return 0;
} /* icfFlowData_destroy() */

