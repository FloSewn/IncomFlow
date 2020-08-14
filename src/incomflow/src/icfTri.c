/*
 * This source file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#include "incomflow/icfTypes.h"
#include "incomflow/icfList.h"
#include "incomflow/icfTri.h"
#include "incomflow/icfMesh.h"
#include "incomflow/icfEdge.h"
#include "incomflow/icfNode.h"
#include "incomflow/icfFlowData.h"

/**********************************************************
* Function: icfTri_create
*----------------------------------------------------------
* Create a new tri structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new tri structure
**********************************************************/
icfTri *icfTri_create(icfMesh *mesh)
{
  icfTri *tri = (icfTri*) calloc(1, sizeof(icfTri));
  check_mem(tri);

  /*-------------------------------------------------------
  | Parents
  -------------------------------------------------------*/
  tri->mesh   = mesh;
  tri->parent = NULL;

  /*-------------------------------------------------------
  | Children
  -------------------------------------------------------*/
  tri->t_c[0] = NULL;
  tri->t_c[1] = NULL;
  tri->n_c    = NULL;

  /*-------------------------------------------------------
  | Defining nodes: 
  -------------------------------------------------------*/
  tri->n[0] = NULL;
  tri->n[1] = NULL;
  tri->n[2] = NULL;

  /*-------------------------------------------------------
  | Defining edges: 
  -------------------------------------------------------*/
  tri->e[0] = NULL;
  tri->e[1] = NULL;
  tri->e[2] = NULL;

  /*-------------------------------------------------------
  | Neighboring triangles: 
  -------------------------------------------------------*/
  tri->t[0] = NULL;
  tri->t[1] = NULL;
  tri->t[2] = NULL;

  /*-------------------------------------------------------
  | Triangle properties
  -------------------------------------------------------*/
  tri->index = -1;
  tri->split = FALSE;

  /*-------------------------------------------------------
  | Position of this triangle in mesh stack 
  -------------------------------------------------------*/
  tri->stackPos = icfMesh_addTri(mesh, tri);

  return tri;
error:
  return NULL;

} /* icfTri_create() */

/**********************************************************
* Function: icfTri_destroy
*----------------------------------------------------------
* Destroys a tri structure
* @param: tri - pointer to tri structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfTri_destroy(icfTri *tri)
{
  free(tri);
  return 0;
} /* icfTri_destroy() */

/**********************************************************
* Function: icfTri_setNodes
*----------------------------------------------------------
* Sets the nodes for an icfTri
* @param: tri - triangle structure to define nodes for
* @param: n0, n1, n2 - pointer to triangle nodes 
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_setNodes(icfTri  *tri, 
                     icfNode *n0,
                     icfNode *n1,
                     icfNode *n2)
{
  tri->n[0] = n0;
  tri->n[1] = n1;
  tri->n[2] = n2;
} /*icfTri_setNodes() */

/**********************************************************
* Function: icfTri_setEdges
*----------------------------------------------------------
* Sets the edges for an icfTri
* @param: tri - triangle structure to define edges for
* @param: e0, e1, e2 - pointer to triangle edges 
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_setEdges(icfTri  *tri, 
                     icfEdge *e0,
                     icfEdge *e1,
                     icfEdge *e2)
{
  tri->e[0] = e0;
  tri->e[1] = e1;
  tri->e[2] = e2;
} /*icfTri_setEdges() */

/**********************************************************
* Function: icfTri_setTris
*----------------------------------------------------------
* Sets the neighboring triangles for an icfTri
* @param: tri - triangle structure to define tris for
* @param: t0, t1, t2 - pointer to triangle neighbors 
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_setTris(icfTri *tri, 
                    icfTri *t0,
                    icfTri *t1,
                    icfTri *t2)
{
  tri->t[0] = t0;
  tri->t[1] = t1;
  tri->t[2] = t2;
} /*icfTri_setTris() */

/**********************************************************
* Function: icfTri_markToRefine
*----------------------------------------------------------
* Marks a triangle and its associated longest edge for 
* refinement. Its neighbors are then marked too.
* @param: tri - triangle structure to define tris for
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_markToSplit(icfTri *tri)
{
  /*-------------------------------------------------------
  | Return if triangle has already been marked
  -------------------------------------------------------*/
  if (tri->split == TRUE)
    return;

  int i;
  int iNb = 0;
  icfEdge *eL = tri->e[0];

  /*-------------------------------------------------------
  | Find larges edge eL
  -------------------------------------------------------*/
  for (i=1; i<3; i++)
  {
    if (eL == NULL)
      eL = tri->e[i];
    
    if (tri->e[i] != NULL)
      if (eL->len < tri->e[i]->len)
      {
        eL  = tri->e[i];
        iNb = i;
      }
  }

  /*-------------------------------------------------------
  | Mark elments for splitting
  -------------------------------------------------------*/
  tri->split         = TRUE;
  eL->split          = TRUE;

  if (tri->t[iNb] != NULL)
    tri->t[iNb]->split = TRUE;

  icfPrint("TRI SPLITTED");

} /* icfTri_markToRefine() */
