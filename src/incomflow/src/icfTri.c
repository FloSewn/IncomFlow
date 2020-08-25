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
#include "incomflow/icfBdry.h"
#include "incomflow/icfEdge.h"
#include "incomflow/icfNode.h"
#include "incomflow/icfFlowData.h"
#include "incomflow/dbg.h"

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
  tri->index       = -1;
  tri->split       = FALSE;
  tri->merge       = FALSE;
  tri->isSplit     = FALSE;
  tri->isLeaf      = FALSE;
  tri->treeLevel   = 0;

  /*-------------------------------------------------------
  | Geometric triangle properties
  -------------------------------------------------------*/
  tri->xy[0]       = 0.0;
  tri->xy[1]       = 0.0;
  tri->aspectRatio = 0.0;
  tri->area        = 0.0;

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

  /*-------------------------------------------------------
  | Compute triangle centroid
  -------------------------------------------------------*/
  tri->xy[0] = (n0->xy[0] + n1->xy[0] + n2->xy[0]) / 3.0;
  tri->xy[1] = (n0->xy[1] + n1->xy[1] + n2->xy[1]) / 3.0;

  /*-------------------------------------------------------
  | Compute triangle area
  -------------------------------------------------------*/
  icfDouble a2 = (n1->xy[0]-n0->xy[0])*(n2->xy[1]-n0->xy[1])
               - (n2->xy[0]-n0->xy[0])*(n1->xy[1]-n0->xy[1]);
  tri->area = 0.5 * a2;

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


  /*-------------------------------------------------------
  | Compute triangle aspect ratio
  -------------------------------------------------------*/
  int i;
  icfDouble eMin = 1e10;
  icfDouble eMax =-1e10;

  for (i = 0; i < 3; i++)
  {
    eMin = tri->e[i]->len < eMin ? tri->e[i]->len : eMin;
    eMax = tri->e[i]->len > eMax ? tri->e[i]->len : eMax;
  }

  tri->aspectRatio = eMax / eMin;


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
  check(eL != NULL, "Triangle has wrong edge connectivity");

  /*-------------------------------------------------------
  | Mark elments for splitting
  -------------------------------------------------------*/
  tri->split         = TRUE;
  tri->e_split       = eL;
  eL->split          = TRUE;

  if (tri->t[iNb] != NULL)
  {
    tri->t[iNb]->split = TRUE;
    tri->t[iNb]->e_split = eL;
  }


#if (ICF_DEBUG > 2)
  icfPrint("MARKED EDGE (%d,%d) IN TRIANGLE (%d,%d,%d) FOR SPLITTING",
      eL->n[0]->index, eL->n[1]->index,
      tri->n[0]->index, tri->n[1]->index, tri->n[2]->index);
#endif

  return;
error:
  return;

} /* icfTri_markToSplit() */


/**********************************************************
* Function: icfTri_markToMerge
*----------------------------------------------------------
* Marks a triangle and its associated refinement tree 
* siblings for merge
* @param: tri - triangle structure 
*----------------------------------------------------------
* 
**********************************************************/
void icfTri_markToMerge(icfTri *tri)
{
  /*-------------------------------------------------------
  | Find siblings in refinement tree
  -------------------------------------------------------*/
  icfNode *n = tri->n_c;
  check(n != NULL,
      "Can not merge unrefined triangle");

  icfEdge *eH0 = n->e_c[0];
  icfEdge *eV0 = n->e_c[1];
  icfEdge *eH1 = n->e_c[2];
  icfEdge *eV1 = n->e_c[3];

  icfTri  *tR0 = n->t_c[0];
  icfTri  *tR1 = n->t_c[1];
  icfTri  *tL1 = n->t_c[2];
  icfTri  *tL0 = n->t_c[3];

  if (eH0 != NULL)
    eH0->merge = TRUE;
  if (eV0 != NULL)
    eV0->merge = TRUE;
  if (eH1 != NULL)
    eH1->merge = TRUE;
  if (eV1 != NULL)
    eV1->merge = TRUE;

  if (tR0 != NULL)
    tR0->merge = TRUE;
  if (tR1 != NULL)
    tR1->merge = TRUE;
  if (tL1 != NULL)
    tL1->merge = TRUE;
  if (tL0 != NULL)
    tL0->merge = TRUE;

  return;
error:
  return;

} /*  icfTri_markToMerge() */
