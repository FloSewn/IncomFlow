/*
 * This source file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#include "incomflow/icfTypes.h"
#include "incomflow/icfList.h"
#include "incomflow/icfNode.h"
#include "incomflow/icfEdge.h"
#include "incomflow/icfBdry.h"
#include "incomflow/icfTri.h"
#include "incomflow/icfMesh.h"
#include "incomflow/icfFlowData.h"

/**********************************************************
* Function: icfEdge_create
*----------------------------------------------------------
* Create a new edge structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new edge structure
**********************************************************/
icfEdge *icfEdge_create(icfMesh *mesh) 
{
  icfEdge *edge = (icfEdge*) calloc(1, sizeof(icfEdge));
  check_mem(edge);

  /*-------------------------------------------------------
  | Parents
  -------------------------------------------------------*/
  edge->mesh   = mesh;
  edge->parent = NULL;

  /*-------------------------------------------------------
  | Children
  -------------------------------------------------------*/
  edge->e_c[0] = NULL;
  edge->e_c[1] = NULL;
  edge->e_c[2] = NULL;
  edge->e_c[3] = NULL;
  edge->n_c    = NULL;

  /*-------------------------------------------------------
  | Edge nodes: edge points from n[0] to n[1] 
  -------------------------------------------------------*/
  edge->n[0] = NULL;
  edge->n[1] = NULL;

  /*-------------------------------------------------------
  | Adjacent triangles: t0 is left, t1 is right
  -------------------------------------------------------*/
  edge->t[0] = NULL;
  edge->t[1] = NULL;

  /*-------------------------------------------------------
  | Boundary properties
  -------------------------------------------------------*/
  edge->bdry         = NULL;
  edge->bdryStackPos = NULL;

  /*-------------------------------------------------------
  | Edge properties
  -------------------------------------------------------*/
  edge->index     = -1;
  edge->split     = FALSE;
  edge->merge     = FALSE;
  edge->isSplit   = FALSE;
  edge->isLeaf    = FALSE;
  edge->treeLevel = 0;

  /*-------------------------------------------------------
  | Position in mesh's edge stack
  -------------------------------------------------------*/
  edge->stackPos = icfMesh_addEdge(mesh, edge);
  edge->leafPos  = -1;

  /*-------------------------------------------------------
  | Median-dual grid metrics
  -------------------------------------------------------*/
  edge->intrNorm[0]    = 0.0;
  edge->intrNorm[1]    = 0.0;
  edge->bdryNorm       = NULL;

  return edge;
error:
  return NULL;

} /* icfEdge_create() */

/**********************************************************
* Function: icfEdge_destroy
*----------------------------------------------------------
* Destroys a edge structure
* @param: edge - pointer to edge structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfEdge_destroy(icfEdge *edge)
{
  icfMesh_remEdge(edge->mesh, edge);
  if (edge->bdry != NULL)
    icfBdry_remEdge(edge->bdry, edge);
  free(edge->bdryNorm);
  free(edge);
  return 0;
} /* icfEdge_destroy() */

/**********************************************************
* Function: icfEdge_setNodes
*----------------------------------------------------------
* Sets the nodes for an icfEdge and calculates the 
* edge length.
* @param: edge - edge structure to define nodes for
* @param: n0, n1 - pointer to edge nodes 
*----------------------------------------------------------
* 
**********************************************************/
void icfEdge_setNodes(icfEdge *edge, 
                      icfNode *n0,
                      icfNode *n1)
{
  edge->n[0] = n0;
  edge->n[1] = n1;

  /*-------------------------------------------------------
  | Compute edge length
  -------------------------------------------------------*/
  icfDouble dx = n1->xy[0] - n0->xy[0];
  icfDouble dy = n1->xy[1] - n0->xy[1];
  edge->len    = sqrt(dx*dx + dy*dy);

  /*-------------------------------------------------------
  | Compute edge centroid
  -------------------------------------------------------*/
  edge->xy[0] = 0.5 * (n1->xy[0] + n0->xy[0]);
  edge->xy[1] = 0.5 * (n1->xy[1] + n0->xy[1]);

} /*icfEdge_setNodes() */

/**********************************************************
* Function: icfEdge_setTris
*----------------------------------------------------------
* Sets the triangles for an icfEdge
* @param: edge - edge structure to define nodes for
* @param: t0, t1 - pointer to edge triangles 
*----------------------------------------------------------
* 
**********************************************************/
void icfEdge_setTris(icfEdge *edge, 
                     icfTri  *t0,
                     icfTri  *t1)
{
  edge->t[0] = t0;
  edge->t[1] = t1;
} /*icfEdge_setTris() */

/**********************************************************
* Function: icfEdge_split
*----------------------------------------------------------
* Split a marked edge 
* @param: e - edge structure to split
*----------------------------------------------------------
* 
**********************************************************/
void icfEdge_split(icfEdge *e)
{
  if (e->split == FALSE)
    return;

  icfMesh *mesh = e->mesh;
  
  icfTri  *t_L  = e->t[0];
  icfTri  *t_R  = e->t[1];

  check(t_L != NULL || t_R != NULL,
    "Can not split edge with undefined triangle neighbors.");

#if (ICF_DEBUG > 2)
  icfPrint("SPLIT EDGE (%d,%d)",
      e->n[0]->index, e->n[1]->index);
#endif

  /*-------------------------------------------------------
  | Create new objects 
  | n        -> new node on e
  | eH1, eH2 -> horizontal edges (on e)
  | eV1, eV2 -> vertical edges (perpendicular to e)
  | tL0, tL1 -> triangles of left triangle
  | tR0, tR1 -> triangles of right triangle
  |
  |             n3
  |            /^\
  |          /  |  \
  | t3  e3 /   eV1   \ e2  t2
  |      /  tL0 | tL1  \
  |    /        |        \
  |  /          |          \
  | n0---eH0--->n---eH1--->n2
  |  \          ^          /
  |    \        |        /
  |      \  tR0 | tR1  /
  | t0  e0 \    |    / e1  t1
  |          \ eV0 /
  |            \|/
  |             n1
  |
  -------------------------------------------------------*/

  /*-------------------------------------------------------
  | Create new node at edge centroid
  -------------------------------------------------------*/
  icfNode *n = icfNode_create(mesh, e->xy);

  /*-------------------------------------------------------
  | Create new horizontal edges 
  -------------------------------------------------------*/
  icfEdge *eH0 = icfEdge_create(mesh);
  icfEdge_setNodes(eH0, e->n[0], n);
  eH0->n_c = n;

  icfEdge *eH1 = icfEdge_create(mesh);
  icfEdge_setNodes(eH1, n, e->n[1]);
  eH1->n_c = n;

  icfTri  *tL0, *tL1, *tR0, *tR1;
  icfNode *n0,  *n1,  *n2,  *n3;
  icfEdge *eV0, *eV1;
  icfEdge *e0,  *e1,  *e2,  *e3;
  icfTri  *t0,  *t1,  *t2,  *t3;

  /*-------------------------------------------------------
  | Determine left triangles
  -------------------------------------------------------*/
  tL0 = NULL;
  tL1 = NULL;
  t2  = NULL;
  t3  = NULL;
  eV0 = NULL;
  eV1 = NULL;

  if ( t_L != NULL)
  {
    if (e == t_L->e[0])
    {
      n0 = t_L->n[0];
      n2 = t_L->n[1];
      n3 = t_L->n[2];

      e2 = t_L->e[1]; 
      e3 = t_L->e[2]; 
    }
    else if (e == t_L->e[1])
    {
      n0 = t_L->n[1];
      n2 = t_L->n[2];
      n3 = t_L->n[0];

      e2 = t_L->e[2];
      e3 = t_L->e[0];

    }
    else if (e == t_L->e[2])
    {
      n0 = t_L->n[2];
      n2 = t_L->n[0];
      n3 = t_L->n[1];

      e2 = t_L->e[0];
      e3 = t_L->e[1];
    }
    else
      log_err("Triangle connectivity seems to be incorrect.");

    /*-----------------------------------------------------
    | Create new sub-triangles
    -----------------------------------------------------*/
    tL0 = icfTri_create(mesh);
    tL1 = icfTri_create(mesh);

    /*-----------------------------------------------------
    | Determine triangle neighbors
    | and connect new sub-triangles to edges
    -----------------------------------------------------*/
    icfPrint("t_L: (%d,%d,%d)", t_L->n[0]->index, t_L->n[1]->index, t_L->n[2]->index);
    if ( e3->t[0] == t_L )
    { 
      t3 = e3->t[1];
      e3->t[0] = tL0;
    }
    else 
    {
      t3 = e3->t[0];
      e3->t[1] = tL0;
    }

    if ( e2->t[0] == t_L )
    {
      t2 = e2->t[1];
      e2->t[0] = tL1;
    }
    else
    {
      t2 = e2->t[0];
      e2->t[1] = tL1;
    }

    /*-----------------------------------------------------
    | Create new vertical edge eV1 and connect 
    | new sub-triangles
    -----------------------------------------------------*/
    eV1 = icfEdge_create(mesh);
    icfEdge_setNodes(eV1, n, n3);
    icfEdge_setTris(eV1, tL0, tL1);

    icfTri_setNodes(tL0, n0,  n,   n3);
    icfTri_setEdges(tL0, eH0, eV1, e3);

    icfTri_setNodes(tL1, n, n2, n3);
    icfTri_setEdges(tL1, eH1, e2, eV1);

  }

  /*-------------------------------------------------------
  | Determine right triangles
  -------------------------------------------------------*/
  tR0 = NULL;
  tR1 = NULL;
  t0  = NULL;
  t1  = NULL;

  if ( t_R != NULL)
  {
    if (e == t_R->e[0])
    {
      n2 = t_R->n[0];
      n0 = t_R->n[1];
      n1 = t_R->n[2];

      e0 = t_R->e[1];
      e1 = t_R->e[2];
    }
    else if (e == t_R->e[1])
    {
      n2 = t_R->n[1];
      n0 = t_R->n[2];
      n1 = t_R->n[0];

      e0 = t_R->e[2];
      e1 = t_R->e[0];
    }
    else if (e == t_R->e[2])
    {
      n2 = t_R->n[2];
      n0 = t_R->n[0];
      n1 = t_R->n[1];

      e0 = t_R->e[0];
      e1 = t_R->e[1];
    }
    else
      log_err("Triangle connectivity seems to be incorrect.");

    /*-----------------------------------------------------
    | Create new sub-triangles
    -----------------------------------------------------*/
    tR0 = icfTri_create(mesh);
    tR1 = icfTri_create(mesh);

    /*-----------------------------------------------------
    | Determine triangle neighbors
    | and connect new sub-triangles to edges
    -----------------------------------------------------*/
    if ( e0->t[0] == t_R)
    {
      t0 = e0->t[1];
      e0->t[0] = tR0;
    }
    else
    {
      t0 = e0->t[0];
      e0->t[1] = tR0;
    }

    if ( e1->t[0] == t_R)
    {
      t1 = e1->t[1];
      e1->t[0] = tR1;
    }
    else 
    {
      t1 = e1->t[0];
      e1->t[1] = tR1;
    }

    /*-----------------------------------------------------
    | Create new vertical edge eV0 and both triangles to 
    | the right side of e 
    -----------------------------------------------------*/
    eV0 = icfEdge_create(mesh);
    icfEdge_setNodes(eV0, n1, n);

    icfTri_setNodes(tR0, n0,  n1,  n);
    icfTri_setEdges(tR0, e0, eV0, eH0);

    icfTri_setNodes(tR1, n, n1, n2);
    icfTri_setEdges(tR1, eV0, e1, eH1);

    icfEdge_setTris(eV0, tR0, tR1);

  }

  /*-------------------------------------------------------
  | Set final connectivites
  -------------------------------------------------------*/
  icfEdge_setTris(eH0, tL0, tR0);
  icfEdge_setTris(eH1, tL1, tR1);

  if (t_L != NULL)
  {
    icfTri_setTris(tL0, tL1, t3, tR0);
    icfTri_setTris(tL1, t2, tL0, tR1);
    t_L->split   = FALSE;
    t_L->isSplit = TRUE;

    tL0->n_c     = n;
    tL1->n_c     = n;
    eV1->n_c     = n;

    if (t3 != NULL)
      if (e3 == t3->e[0])
        t3->t[2] = tL0;
      else if (e3 == t3->e[1])
        t3->t[0] = tL0;
      else if (e3 == t3->e[2])
        t3->t[1] = tL0;
      else
        log_err("Triangle connectivity seems to be incorrect.");

    if (t2 != NULL)
      if (e2 == t2->e[0])
        t2->t[2] = tL1;
      else if (e2 == t2->e[1])
        t2->t[0] = tL1;
      else if (e2 == t2->e[2])
        t2->t[1] = tL1;
      else
        log_err("Triangle connectivity seems to be incorrect.");

    t_L->t_c[0]    = tL0;
    tL0->parent    = t_L;
    tL0->treeLevel = t_L->treeLevel + 1;

    t_L->t_c[1]    = tL1;
    tL1->parent    = t_L;
    tL1->treeLevel = t_L->treeLevel + 1;

  }


  if (t_R != NULL)
  {
    icfTri_setTris(tR0, tR1, tL0, t0);
    icfTri_setTris(tR1, t1, tL1, tR0);
    t_R->split   = FALSE;
    t_R->isSplit = TRUE;

    tR0->n_c     = n;
    tR1->n_c     = n;
    eV0->n_c     = n;

    if (t0 != NULL)
      if (e0 == t0->e[0])
        t0->t[2] = tR0;
      else if (e0 == t0->e[1])
        t0->t[0] = tR0;
      else if (e0 == t0->e[2])
        t0->t[1] = tR0;
      else
        log_err("Triangle connectivity seems to be incorrect.");

    if (t1 != NULL)
      if (e1 == t1->e[0])
        t1->t[2] = tR1;
      else if (e1 == t1->e[1])
        t1->t[0] = tR1;
      else if (e1 == t1->e[2])
        t1->t[1] = tR1;
      else
        log_err("Triangle connectivity seems to be incorrect.");


    t_R->t_c[0]    = tR0;
    tR0->parent    = t_R;
    tR0->treeLevel = t_R->treeLevel + 1;

    t_R->t_c[1]    = tR1;
    tR1->parent    = t_R;
    tR1->treeLevel = t_R->treeLevel + 1;

  }

  /*-------------------------------------------------------
  | Mark edge as splitted and set its children
  -------------------------------------------------------*/
  e->isSplit = TRUE;

  e->e_c[0]      = eH0;
  eH0->parent    = e;
  eH0->treeLevel = e->treeLevel + 1;

  e->e_c[1]      = eH1;
  eH1->parent    = e;
  eH1->treeLevel = e->treeLevel + 1;


  e->e_c[2] = eV0;
  if (eV0 != NULL)
  {
    eV0->parent    = e;
    eV0->treeLevel = e->treeLevel + 1;
  }

  e->e_c[3] = eV1;
  if (eV1 != NULL)
  {
    eV1->parent    = e;
    eV1->treeLevel = e->treeLevel + 1;
  }

  /*-------------------------------------------------------
  | Set connectivity to new node n at edge centroid
  -------------------------------------------------------*/
  n->e_c[0] = eH0;
  n->e_c[1] = eV0;
  n->e_c[2] = eH1;
  n->e_c[3] = eV1;

  n->t_c[0] = tR0;
  n->t_c[1] = tR1;
  n->t_c[2] = tL1;
  n->t_c[3] = tL0;


  /*-------------------------------------------------------
  | Set boundary properties for children
  -------------------------------------------------------*/
  if (e->bdry != NULL)
  {
    icfBdry_addEdge(e->bdry, eH0);
    icfBdry_addEdge(e->bdry, eH1);
    icfBdry_addNode(e->bdry, n, 0);
    icfBdry_addNode(e->bdry, n, 1);
  }

  return;
error:
  return;

} /* icfEdge_split() */


/**********************************************************
* Function: icfEdge_merge
*----------------------------------------------------------
* Merge a marked edge and its associated triangles
* @param: e - edge structure to split
*----------------------------------------------------------
* 
**********************************************************/
void icfEdge_merge(icfEdge *e)
{
  icfMesh *mesh = e->mesh;

  /*-------------------------------------------------------
  | Find siblings in refinement tree
  -------------------------------------------------------*/
  icfNode *n = e->n_c;
  check(n != NULL,
      "Can not merge unrefined edge");

  icfEdge *eH0 = n->e_c[0];
  icfEdge *eV0 = n->e_c[1];
  icfEdge *eH1 = n->e_c[2];
  icfEdge *eV1 = n->e_c[3];

  icfTri  *tR0 = n->t_c[0];
  icfTri  *tR1 = n->t_c[1];
  icfTri  *tL1 = n->t_c[2];
  icfTri  *tL0 = n->t_c[3];

  /*-------------------------------------------------------
  | Check that all siblings are leafs
  -------------------------------------------------------*/
  if (tR0 != NULL && tR0->isLeaf == FALSE)
    return;
  if (tR1 != NULL && tR1->isLeaf == FALSE)
    return;
  if (tL0 != NULL && tL0->isLeaf == FALSE)
    return;
  if (tL1 != NULL && tL1->isLeaf == FALSE)
    return;

#if (ICF_DEBUG > 2)
  icfPrint("MERGE EDGE (%d,%d)",
      e->n[0]->index, e->n[1]->index);
#endif


  /*-------------------------------------------------------
  | Change connectivity of neighbors
  |
  |             n3
  |            /^\
  |          /  |  \
  | t3  e3 /   eV1   \ e2  t2
  |      /  tL0 | tL1  \
  |    /        |        \
  |  /          |          \
  | n0---eH0--> n ---eH1-->n2
  |  \          ^          /
  |    \        |        /
  |      \  tR0 | tR1  /
  | t0  e0 \    |    / e1  t1
  |          \ eV0 /
  |            \|/
  |             n1
  -------------------------------------------------------*/

  /*-------------------------------------------------------
  | Entities of tR0
  -------------------------------------------------------*/
  icfTri  *t0 = NULL;
  icfEdge *e0 = NULL;

  if (tR0 != NULL)
  {
    if (tR0->e[0] == eV0) /* n == tR0->n[1] */
    {
      e0 = tR0->e[2];
      t0 = tR0->t[1];
    }
    else if (tR0->e[1] == eV0) /* n == tR0->n[2] */
    {
      e0 = tR0->e[0];
      t0 = tR0->t[2];
    }
    else if (tR0->e[2] == eV0) /* n == tR0->n[0] */
    {
      e0 = tR0->e[1];
      t0 = tR0->t[0];
    }
    else
      log_err("Error in mesh connectivity.");
  }

  /*-------------------------------------------------------
  | Entities of tR1
  -------------------------------------------------------*/
  icfTri  *t1 = NULL;
  icfEdge *e1 = NULL;

  if (tR1 != NULL)
  {
    if (tR1->e[0] == eV0) /* n == tR1->n[0] */
    {
      e1 = tR1->e[1];
      t1 = tR1->t[0];
    }
    else if (tR1->e[1] == eV0) /* n == tR1->n[1] */
    {
      e1 = tR1->e[2];
      t1 = tR1->t[1];
    }
    else if (tR1->e[2] == eV0) /* n == tR1->n[2] */
    {
      e1 = tR1->e[0];
      t1 = tR1->t[2];
    }
    else
      log_err("Error in mesh connectivity.");
  }


  /*-------------------------------------------------------
  | Entities of tL1
  -------------------------------------------------------*/
  icfTri  *t2 = NULL;
  icfEdge *e2 = NULL;
  
  if (tL1 != NULL)
  {
    if (tL1->e[0] == eV1) /* n == tL1->n[1] */
    {
      e2 = tL1->e[2];
      t2 = tL1->t[1];
    }
    else if (tL1->e[1] == eV1) /* n == tL1->n[2] */
    {
      e2 = tL1->e[0];
      t2 = tL1->t[2];
    }
    else if (tL1->e[2] == eV1) /* n == tL1->n[0] */
    {
      e2 = tL1->e[1];
      t2 = tL1->t[0];
    }
    else
      log_err("Error in mesh connectivity.");
  }


  /*-------------------------------------------------------
  | Entities of tL0
  -------------------------------------------------------*/
  icfTri  *t3 = NULL;
  icfEdge *e3 = NULL;

  if (tL0 != NULL)
  {
    if (tL0->e[0] == eV1) /* n == tL0->n[0] */
    {
      e3 = tL0->e[1];
      t3 = tL0->t[0];
    }
    else if (tL0->e[1] == eV1) /* n == tL0->n[1] */
    {
      e3 = tL0->e[2];
      t3 = tL0->t[1];
    }
    else if (tL0->e[2] == eV1) /* n == tL0->n[2] */
    {
      e3 = tL0->e[0];
      t3 = tL0->t[2];
    }
    else
      log_err("Error in mesh connectivity.");
  }


  /*-------------------------------------------------------
  | Get parent entities
  -------------------------------------------------------*/
  icfEdge *e_p = NULL;
  icfTri *tL_p = NULL;
  icfTri *tR_p = NULL;

  if (tL0 != NULL && tL1 != NULL)
  {
    tL_p = tL0->parent;
    e_p  = eV1->parent;
  }

  if (tR0 != NULL && tR1 != NULL)
  {
    tR_p = tR0->parent;
    e_p  = eV0->parent;
  }

  /*-------------------------------------------------------
  | Set neighbors for tL
  -------------------------------------------------------*/
  if (tL_p != NULL)
  {
    /*-----------------------------------------------------
    | neighbors of tL
    -----------------------------------------------------*/
    if (e3 == tL_p->e[0]) 
    {
      icfTri_setTris(tL_p,  tR_p, t2, t3);
      icfTri_setEdges(tL_p,  e3, e_p, e2);
    }
    else if (e3 == tL_p->e[1])
    {
      icfTri_setTris(tL_p,  t3, tR_p, t2);
      icfTri_setEdges(tL_p,  e2, e3, e_p);
    }
    else if (e3 == tL_p->e[2])
    {
      icfTri_setTris(tL_p,  t2, t3, tR_p);
      icfTri_setEdges(tL_p,  e_p, e2, e3);
    }
    else
      log_err("Error in mesh connectivity.");

    /*-----------------------------------------------------
    | neighbors of edges e2 & e3
    -----------------------------------------------------*/
    if (e3->t[0] == tL0)
      e3->t[0] = tL_p;
    else if (e3->t[1] == tL0)
      e3->t[1] = tL_p;
    else
      log_err("Error in mesh connectivity");

    if (e2->t[0] == tL1)
      e2->t[0] = tL_p;
    else if (e2->t[1] == tL1)
      e2->t[1] = tL_p;
    else
      log_err("Error in mesh connectivity");

    /*-----------------------------------------------------
    | neighbors of triangles t2 & t3
    -----------------------------------------------------*/
    if (t3 != NULL)
    {
      if (t3->t[0] == tL0)
        t3->t[0] = tL_p;
      else if (t3->t[1] == tL0)
        t3->t[1] = tL_p;
      else if (t3->t[2] == tL0)
        t3->t[2] = tL_p;
      else
        log_err("Error in mesh connectivity");
    }

    if (t2 != NULL)
    {
      if (t2->t[0] == tL1)
        t2->t[0] = tL_p;
      else if (t2->t[1] == tL1)
        t2->t[1] = tL_p;
      else if (t2->t[2] == tL1)
        t2->t[2] = tL_p;
      else
        log_err("Error in mesh connectivity");
    }
  }

  /*-------------------------------------------------------
  | Set neighbors for tR
  -------------------------------------------------------*/
  if (tR_p != NULL)
  {

    /*-----------------------------------------------------
    | neighbors of tR
    -----------------------------------------------------*/
    if (e0 == tR_p->e[0]) 
    {
      icfTri_setTris(tR_p,  t1, tL_p, t0);
      icfTri_setEdges(tR_p,  e0, e1, e_p);
    }
    else if (e0 == tR_p->e[1])
    {
      icfTri_setTris(tR_p,  t0, t1, tL_p);
      icfTri_setEdges(tR_p,  e_p, e0, e1);
    }
    else if (e0 == tR_p->e[2])
    {
      icfTri_setTris(tR_p,  tL_p, t0, t1);
      icfTri_setEdges(tR_p,  e1, e_p, e0);
    }
    else
      log_err("Error in mesh connectivity.");

    /*-----------------------------------------------------
    | neighbors of edges e0 & e1
    -----------------------------------------------------*/
    if (e0->t[0] == tR0)
      e0->t[0] = tR_p;
    else if (e0->t[1] == tR0)
      e0->t[1] = tR_p;
    else
      log_err("Error in mesh connectivity");

    if (e1->t[0] == tR1)
      e1->t[0] = tR_p;
    else if (e1->t[1] == tR1)
      e1->t[1] = tR_p;
    else
      log_err("Error in mesh connectivity");

    /*-----------------------------------------------------
    | neighbors of triangles t0 & t1
    -----------------------------------------------------*/
    if (t0 != NULL)
    {
      if (t0->t[0] == tR0)
        t0->t[0] = tR_p;
      else if (t0->t[1] == tR0)
        t0->t[1] = tR_p;
      else if (t0->t[2] == tR0)
        t0->t[2] = tR_p;
      else
        log_err("Error in mesh connectivity");
    }

    if (t1 != NULL)
    {
      if (t1->t[0] == tR1)
        t1->t[0] = tR_p;
      else if (t1->t[1] == tR1)
        t1->t[1] = tR_p;
      else if (t1->t[2] == tR1)
        t1->t[2] = tR_p;
      else
        log_err("Error in mesh connectivity");
    }
  }
       

  /*-------------------------------------------------------
  | Remove old leafs
  -------------------------------------------------------*/
  if (tL_p != NULL)
  {
#if (ICF_DEBUG > 2)
    icfPrint("DESTROY TRI (%d,%d,%d)",
        tL0->n[0]->index,tL0->n[1]->index,tL0->n[2]->index);
    icfPrint("DESTROY TRI (%d,%d,%d)",
        tL1->n[0]->index,tL1->n[1]->index,tL1->n[2]->index);
    icfPrint("DESTROY EDGE (%d,%d)",
        eV1->n[0]->index,eV1->n[1]->index);
#endif

    tL0->merge = FALSE;
    mesh->triLeafs[tL0->leafPos] = NULL;

    tL1->merge = FALSE;
    mesh->triLeafs[tL1->leafPos] = NULL;

    eV1->merge = FALSE;
    mesh->edgeLeafs[eV1->leafPos] = NULL;

    icfTri_destroy(tL0);
    icfTri_destroy(tL1);
    icfEdge_destroy(eV1);

    tL_p->t_c[0]  = NULL;
    tL_p->t_c[1]  = NULL;
    tL_p->isSplit = FALSE;

  }

  if (tR_p != NULL)
  {
#if (ICF_DEBUG > 2)
    icfPrint("DESTROY TRI (%d,%d,%d)",
        tR0->n[0]->index,tR0->n[1]->index,tR0->n[2]->index);
    icfPrint("DESTROY TRI (%d,%d,%d)",
        tR1->n[0]->index,tR1->n[1]->index,tR1->n[2]->index);
    icfPrint("DESTROY EDGE (%d,%d)",
        eV0->n[0]->index,eV0->n[1]->index);
#endif

    tR0->merge = FALSE;
    mesh->triLeafs[tR0->leafPos] = NULL;
    
    tR1->merge = FALSE;
    mesh->triLeafs[tR1->leafPos] = NULL;

    eV0->merge = FALSE;
    mesh->edgeLeafs[eV0->leafPos] = NULL;

    icfTri_destroy(tR0);
    icfTri_destroy(tR1);
    icfEdge_destroy(eV0);

    tR_p->t_c[0]  = NULL;
    tR_p->t_c[1]  = NULL;
    tR_p->isSplit = FALSE;

  }

#if (ICF_DEBUG > 2)
    icfPrint("DESTROY EDGE (%d,%d)",
        eH0->n[0]->index,eH0->n[1]->index);
    icfPrint("DESTROY EDGE (%d,%d)",
        eH1->n[0]->index,eH1->n[1]->index);
    icfPrint("DESTROY NODE %d",
        n->index);
#endif

  eH0->merge = FALSE;
  mesh->edgeLeafs[eH0->leafPos] = NULL;

  eH1->merge = FALSE;
  mesh->edgeLeafs[eH1->leafPos] = NULL;

  icfEdge_destroy(eH0);
  icfEdge_destroy(eH1);
  icfNode_destroy(n);

  e_p->isSplit = FALSE;


  return; 
error:
  return;
  
} /* icfEdge_merge() */
