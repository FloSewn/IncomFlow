#include <incomflow/minunit.h>
#include <incomflow/bstrlib.h>
#include <incomflow/icfIO.h>
#include "icfIO_tests.h"

const char *testfile = "/datadisk/Code/C-Code/incomFlow/share/files/mesh.dat";

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

  icfIO_extractParam(file->txtlist, "NODES", 0, &nNodes);
  mu_assert(nNodes == 109, "Error in icfIO_extractParam()");


  icfIO_readMeshNodes(file->txtlist, &xyNodes, &nNodes); 

  for (i = 0; i < nNodes; i++)
    icfPrint("%d: (%f,%f)", i, xyNodes[i][0], xyNodes[i][1]);

  icfIO_readMeshTriangles(file->txtlist, &idxTris, &nTris); 

  for (i = 0; i < nTris; i++)
    icfPrint("%d: (%d,%d,%d)", i, idxTris[i][0], idxTris[i][1], idxTris[i][2]);


  icfIO_destroyReader( file );
  free(xyNodes);

  return NULL;
}
