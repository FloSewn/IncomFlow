#include "incomflow/minunit.h"
#include "incomflow/bstrlib.h"
#include "incomflow/icfIO.h"
#include "incomflow/icfBdry.h"
#include "incomflow/icfMesh.h"
#include "incomflow/icfFlowData.h"
#include "incomflow/icfTri.h"
#include "incomflow/icfNode.h"
#include "incomflow/icfEdge.h"
#include "icfIO_tests.h"

const char *testfile = "/datadisk/Code/C-Code/incomFlow/share/files/example_mesh.dat";

/*************************************************************
* Unit test function to handle creation and 
* destruction of bstrings
*************************************************************/
char *test_icfIO_readerFunctions()
{
  int i;
  bstring *ptr  = NULL;
  icfIOReader *file = icfIO_createReader( testfile );

  int nNodes;
  int nTris;

  icfDouble (*xyNodes)[2];
  icfIndex  (*idxTris)[3];
  icfIndex  (*idxTriNbrs)[3];

  icfIO_extractParam(file->txtlist, "NODES", 0, &nNodes);

  icfIO_readMeshNodes(file->txtlist, &xyNodes, &nNodes); 

  icfIO_readMeshTriangles(file->txtlist, &idxTris, &nTris); 

  icfIO_readMeshTriNbrs(file->txtlist, &idxTriNbrs, &nTris); 

  icfIO_destroyReader( file );
  free(xyNodes);
  free(idxTris);
  free(idxTriNbrs);

  return NULL;
}

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


  //if (fabs(xc - 0.75) < 0.2)
  if (fabs(xc - 4.75) < 1.5 && fabs(yc - 4.25) < 1.5)
    return TRUE;

  return FALSE;
}


/*************************************************************
* Unit test function to handle mesh initialization from file 
*************************************************************/
char *test_icfIO_readMesh()
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
  icfBdry *bdrySouth = icfBdry_create(mesh, 0, 1, "SOUTH");
  icfBdry *bdryEast  = icfBdry_create(mesh, 0, 2, "EAST");
  icfBdry *bdryNorth = icfBdry_create(mesh, 0, 3, "NORTH");
  //icfBdry *bdryWest  = icfBdry_create(mesh, 0, 4, "WEST");

  /*----------------------------------------------------------
  | Read the mesh from a file
  ----------------------------------------------------------*/
  icfIO_readMesh(testfile, mesh);
  icfMesh_refine(flowData, mesh);
  icfMesh_refine(flowData, mesh);

  /*----------------------------------------------------------
  | Print the mesh
  ----------------------------------------------------------*/
  icfMesh_printMesh(mesh);

  /*----------------------------------------------------------
  | Clear structures
  ----------------------------------------------------------*/
  icfFlowData_destroy(flowData);

  
  return NULL;
}
