#include "incomflow/icfTypes.h"
#include "incomflow/minunit.h"
#include "incomflow/dbg.h"

#include "incomflow/icfList.h"
#include "incomflow/icfFlowData.h"
#include "incomflow/icfMesh.h"
#include "incomflow/icfNode.h"
#include "incomflow/icfEdge.h"
#include "incomflow/icfTri.h"
#include "incomflow/icfBdry.h"

/*************************************************************
* Dummy refinement function
*************************************************************/
static inline icfBool refineFun(icfFlowData *flowData, 
                                icfTri      *tri)
{
  icfDouble xc = 0.0;
  icfDouble yc = 0.0;

  int i;
  for (i = 0; i < 3; i++)
  {
    xc += tri->n[i]->xy[0];
    yc += tri->n[i]->xy[1];
  }

  xc /= 3.0;
  yc /= 3.0;


  if (fabs(xc - 0.75) < 0.2)
    return TRUE;

  return FALSE;
  
}


/*************************************************************
* Unit test function for geometric functions
*************************************************************/
char *test_basic_structures()
{
  /*----------------------------------------------------------
  | Create flow data container and mesh 
  ----------------------------------------------------------*/
  icfFlowData *flowData = icfFlowData_create();

  icfMesh *mesh       = icfMesh_create();
  flowData->mesh      = mesh;
  flowData->refineFun = refineFun;

  /*----------------------------------------------------------
  | Define boundaries
  ----------------------------------------------------------*/
  icfBdry *bdrySouth = icfBdry_create(mesh, 0, "SOUTH");
  icfBdry *bdryEast  = icfBdry_create(mesh, 0, "EAST");
  icfBdry *bdryNorth = icfBdry_create(mesh, 0, "NORTH");
  icfBdry *bdryWest  = icfBdry_create(mesh, 0, "WEST");

  /*----------------------------------------------------------
  | Create mesh nodes
  ----------------------------------------------------------*/
  icfDouble xy0[2] = {0.0,0.0};
  icfNode  *n0     = icfNode_create(mesh, xy0);

  icfDouble xy1[2] = {1.0,0.0};
  icfNode  *n1     = icfNode_create(mesh, xy1);

  icfDouble xy2[2] = {1.0,1.0};
  icfNode  *n2     = icfNode_create(mesh, xy2);

  icfDouble xy3[2] = {0.0,1.0};
  icfNode  *n3     = icfNode_create(mesh, xy3);


  /*----------------------------------------------------------
  | Create mesh edges
  ----------------------------------------------------------*/
  icfEdge *e0   = icfEdge_create(mesh);
  icfEdge_setNodes(e0, n0, n1);
  icfBdry_addEdge(bdrySouth, e0);
  icfBdry_addNode(bdrySouth, n0, 0);
  icfBdry_addNode(bdrySouth, n1, 1);

  icfEdge *e1   = icfEdge_create(mesh);
  icfEdge_setNodes(e1, n1, n2);
  icfBdry_addEdge(bdryEast, e1);
  icfBdry_addNode(bdryEast, n1, 0);
  icfBdry_addNode(bdryEast, n2, 1);

  icfEdge *e2   = icfEdge_create(mesh);
  icfEdge_setNodes(e2, n2, n3);
  icfBdry_addEdge(bdryNorth, e2);
  icfBdry_addNode(bdryNorth, n2, 0);
  icfBdry_addNode(bdryNorth, n3, 1);

  icfEdge *e3   = icfEdge_create(mesh);
  icfEdge_setNodes(e3, n3, n0);
  icfBdry_addEdge(bdryWest, e3);
  icfBdry_addNode(bdryWest, n3, 0);
  icfBdry_addNode(bdryWest, n0, 1);

  icfEdge *e4   = icfEdge_create(mesh);
  icfEdge_setNodes(e4, n0, n2);


  /*----------------------------------------------------------
  | Create mesh triangles
  ----------------------------------------------------------*/
  icfTri *t0 = icfTri_create(mesh);
  icfTri_setNodes(t0, n0, n1, n2);
  icfTri_setEdges(t0, e0, e1, e4);

  icfTri *t1 = icfTri_create(mesh);
  icfTri_setNodes(t1, n2, n3, n0);
  icfTri_setEdges(t1, e2, e3, e4);

  icfTri_setTris(t0, NULL, t1, NULL);
  icfTri_setTris(t1, NULL, t0, NULL);

  icfEdge_setTris(e0, t0, NULL);
  icfEdge_setTris(e1, t0, NULL);
  icfEdge_setTris(e2, t1, NULL);
  icfEdge_setTris(e3, t1, NULL);
  icfEdge_setTris(e4, t1, t0);

  /*----------------------------------------------------------
  | Refine the mesh
  ----------------------------------------------------------*/
  int i = 0;
  for (i = 0; i < 7; i++)
  {
    icfPrint("------- ITERATION %d --------", i);
    icfMesh_refine(flowData, mesh);
  }

  /*----------------------------------------------------------
  | Print the mesh
  ----------------------------------------------------------*/
  icfMesh_printMesh(mesh);

  /*----------------------------------------------------------
  | Clear structures
  ----------------------------------------------------------*/
  icfFlowData_destroy(flowData);

  return NULL;
} /* test_basic_tests() */
