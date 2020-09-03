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
#include "incomflow/icfBdry.h"
#include "incomflow/icfMesh.h"
#include "incomflow/icfNode.h"
#include "incomflow/icfEdge.h"
#include "incomflow/icfTri.h"

/**********************************************************
* Function: icfMesh_create
*----------------------------------------------------------
* Create a new mesh structure and return a pointer 
* to it                                            
*----------------------------------------------------------
* @return: pointer to new mesh structure
**********************************************************/
icfMesh *icfMesh_create(void)
{
  icfMesh *mesh = (icfMesh*) calloc(1, sizeof(icfMesh) );
  check_mem(mesh);

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  mesh->nNodes = 0;
  mesh->nodeStack = icfList_create();
  mesh->nodes = (icfNode**) calloc(0, sizeof(icfNode*) );

  /*-------------------------------------------------------
  | Mesh edges 
  -------------------------------------------------------*/
  mesh->nEdges = 0;
  mesh->edgeStack = icfList_create();

  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  mesh->nTris = 0;
  mesh->triStack = icfList_create();

  /*-------------------------------------------------------
  | Mesh boundaries 
  -------------------------------------------------------*/
  mesh->nBdrys = 0;
  mesh->bdryStack = icfList_create();

  /*-------------------------------------------------------
  | Mesh edge leafs 
  -------------------------------------------------------*/
  mesh->nEdgeLeafs = 0;
  mesh->edgeLeafs = (icfEdge**) calloc(0, sizeof(icfEdge*));

  /*-------------------------------------------------------
  | Mesh triangle leafs 
  -------------------------------------------------------*/
  mesh->nTriLeafs = 0;
  mesh->triLeafs = (icfTri**) calloc(0, sizeof(icfTri*));


  return mesh;
error:
  return NULL;

} /* icfMesh_create() */

/**********************************************************
* Function: icfMesh_destroy
*----------------------------------------------------------
* Destroys a mesh structure
* @param: mesh - pointer to mesh structure 
*----------------------------------------------------------
* @return: returns 0 on success
**********************************************************/
int icfMesh_destroy(icfMesh *mesh)
{
  icfListNode *cur, *nxt;

  /*-------------------------------------------------------
  | Free all edges on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->edgeStack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    icfEdge_destroy(cur->value);
    cur = nxt;
  }
#if (ICF_DEBUG > 0)
  icfPrint("MESH EDGES FREE");
#endif

  /*-------------------------------------------------------
  | Free all tris on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->triStack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    icfTri_destroy(cur->value);
    cur = nxt;
  }
#if (ICF_DEBUG > 0)
  icfPrint("MESH TRIANGLES FREE");
#endif

  /*-------------------------------------------------------
  | Free all bdrys on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->bdryStack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    icfBdry_destroy(cur->value);
    cur = nxt;
  }
#if (ICF_DEBUG > 0)
  icfPrint("MESH BOUNDARIES FREE");
#endif

  /*-------------------------------------------------------
  | Free all nodes on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->nodeStack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    icfNode_destroy(cur->value);
    cur = nxt;
  }
#if (ICF_DEBUG > 0)
  icfPrint("MESH NODES FREE");
#endif

  /*-------------------------------------------------------
  | Free all mesh list structures
  -------------------------------------------------------*/
  icfList_destroy(mesh->nodeStack);
  icfList_destroy(mesh->edgeStack);
  icfList_destroy(mesh->triStack);
  icfList_destroy(mesh->bdryStack);

  /*-------------------------------------------------------
  | Free all mesh leaf arrays
  -------------------------------------------------------*/
  free(mesh->edgeLeafs);
  free(mesh->triLeafs);
  free(mesh->nodes);

  /*-------------------------------------------------------
  | Finally free mesh structure memory
  -------------------------------------------------------*/
  free(mesh);

  return 0;

error:
  return -1;

} /* icfMesh_create() */


/**********************************************************
* Function: icfMesh_addNode()
*----------------------------------------------------------
* Function to add an icfNode to an icfMesh
*----------------------------------------------------------
* @return: icfNode list node on the mesh's node stack
**********************************************************/
icfListNode *icfMesh_addNode(icfMesh *mesh, icfNode *node)
{
  mesh->nNodes += 1;
  icfList_push(mesh->nodeStack, node);
  icfListNode *nodePos = icfList_last_node(mesh->nodeStack);
  return nodePos;
} /* icfMesh_addNode() */

/**********************************************************
* Function: icfMesh_addEdge()
*----------------------------------------------------------
* Function to add an icfEdge to an icfMesh
*----------------------------------------------------------
* @return: icfEdge list node on the mesh's edge stack
**********************************************************/
icfListNode *icfMesh_addEdge(icfMesh *mesh, icfEdge *edge)
{
  mesh->nEdges += 1;
  icfList_push(mesh->edgeStack, edge);
  icfListNode *edgePos = icfList_last_node(mesh->edgeStack);
  return edgePos;
} /* icfMesh_addEdge() */

/**********************************************************
* Function: icfMesh_addTri()
*----------------------------------------------------------
* Function to add an icfTri to an icfMesh
*----------------------------------------------------------
* @return: icfTri list node on the mesh's tri stack
**********************************************************/
icfListNode *icfMesh_addTri(icfMesh *mesh, icfTri *tri)
{
  mesh->nTris += 1;
  icfList_push(mesh->triStack, tri);
  icfListNode *triPos = icfList_last_node(mesh->triStack);
  return triPos;
} /* icfMesh_addTri() */

/**********************************************************
* Function: icfMesh_addBdry()
*----------------------------------------------------------
* Function to add an icfBdry to an icfMesh
*----------------------------------------------------------
* @return: icfBdry list node on the mesh's bdry stack
**********************************************************/
icfListNode *icfMesh_addBdry(icfMesh *mesh, icfBdry *bdry)
{
  mesh->nBdrys += 1;
  icfList_push(mesh->bdryStack, bdry);
  icfListNode *bdryPos = icfList_last_node(mesh->bdryStack);
  return bdryPos;
} /* icfMesh_addBdry() */

/**********************************************************
* Function: icfMesh_remNode()
*----------------------------------------------------------
* Function to remove an icfNode from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remNode(icfMesh *mesh, icfNode *node)
{
  icfList_remove(mesh->nodeStack, node->stackPos);
  mesh->nNodes -= 1;
} /* tmMesh_remNode() */

/**********************************************************
* Function: icfMesh_remEdge()
*----------------------------------------------------------
* Function to remove an icfEdge from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remEdge(icfMesh *mesh, icfEdge *edge)
{
  icfList_remove(mesh->edgeStack, edge->stackPos);
  mesh->nEdges -= 1;
} /* tmMesh_remEdge() */

/**********************************************************
* Function: icfMesh_remTri()
*----------------------------------------------------------
* Function to remove an icfTri from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remTri(icfMesh *mesh, icfTri *tri)
{
  icfList_remove(mesh->triStack, tri->stackPos);
  mesh->nTris -= 1;
} /* tmMesh_remTri() */

/**********************************************************
* Function: icfMesh_remBdry()
*----------------------------------------------------------
* Function to remove an icfBdry from an icfMesh
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_remBdry(icfMesh *mesh, icfBdry *bdry)
{
  icfList_remove(mesh->bdryStack, bdry->stackPos);
  mesh->nBdrys -= 1;
} /* tmMesh_remBdry() */

/**********************************************************
* Function: icfMesh_refine()
*----------------------------------------------------------
* Function to refine an icfMesh mesh structure
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_refine(icfFlowData *flowData, icfMesh *mesh)
{
  icfListNode *cur;

  icfRefineFun refineFun = flowData->refineFun;
  check(refineFun != NULL,
      "Refinement function has not been defined.");

  /*-------------------------------------------------------
  | Mark all triangles and respective edges to refine
  -------------------------------------------------------*/
  for (cur = mesh->triStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfTri *t = (icfTri*)cur->value;

    if (refineFun(flowData, t) == TRUE && t->isSplit == FALSE)
      icfTri_markToSplit(t);

  }
  /*-------------------------------------------------------
  | Split all marked edges 
  -------------------------------------------------------*/
  for (cur = mesh->edgeStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfEdge *e = (icfEdge*)cur->value;

    if (e->split == TRUE && e->isSplit == FALSE)
      icfEdge_split(e);
  }

  /*-------------------------------------------------------
  | Update the mesh leafs
  -------------------------------------------------------*/
  icfMesh_update(mesh);

  return;
error:
  return;

} /* isfMesh_refine() */

/**********************************************************
* Function: icfMesh_coarsen()
*----------------------------------------------------------
* Function to coarsen an icfMesh mesh structure
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_coarsen(icfFlowData *flowData, icfMesh *mesh)
{

  icfTri  **triLeafs  = mesh->triLeafs;
  icfEdge **edgeLeafs = mesh->edgeLeafs;

  int i;
  int nTris  = mesh->nTriLeafs;
  int nEdges = mesh->nEdgeLeafs;

  icfRefineFun coarseFun = flowData->coarseFun;
  check(coarseFun != NULL,
      "Coarsening function has not been defined.");

  /*-------------------------------------------------------
  | Mark all triangles and respective edges to coarsen
  -------------------------------------------------------*/
  for (i = 0; i < nTris; i++)
  {
    icfTri *t = triLeafs[i];

    if (t->merge == FALSE && coarseFun(flowData, t) == TRUE)
      icfTri_markToMerge(t);

  }

  /*-------------------------------------------------------
  | Merge all marked leafs
  -------------------------------------------------------*/
  for (i = 0; i < nEdges; i++)
  {
    icfEdge *e = edgeLeafs[i];

    if (e != NULL)
      if (e->merge == TRUE)
        icfEdge_merge(e);
  }

  /*-------------------------------------------------------
  | Update the mesh leafs
  -------------------------------------------------------*/
  icfMesh_update(mesh);

  return;
error:
  return;

} /* icfMesh_coarsen() */

/**********************************************************
* Function: icfMesh_update()
*----------------------------------------------------------
* Function to update all mesh leafs structures and 
* the mesh arrays.
* The entities get their global indices here.
* Furthermore, the mesh normals and volumes for the 
* flow solver are calculated.
* This is mandatory after refining the mesh or setting
* up the mesh.
*----------------------------------------------------------
* 
**********************************************************/
void icfMesh_update(icfMesh *mesh)
{
  icfListNode *cur, *curBdry;

  /*-------------------------------------------------------
  | Count leafs in both triangle- and edge-trees
  | This is the point, where triangles and edges
  | get their global indices and their "isLeaf" property
  | is set to FALSE as default
  | 
  | Also: set default values for some attributes
  -------------------------------------------------------*/
  int iTri = 0;
  int iEdge = 0;
  int nTriLeafs = 0;
  int nEdgeLeafs = 0;

  for (cur = mesh->triStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfTri *t = (icfTri*)cur->value;
    t->index  = iTri;
    t->isLeaf = FALSE;
    t->merge  = FALSE;
    t->split  = FALSE;
    iTri++;

    if (t->isSplit == FALSE)
      nTriLeafs += 1;
  }

#if (ICF_DEBUG > 2)
  icfPrint("FOUND %d TRI LEAFS", nTriLeafs);
#endif

  for (cur = mesh->edgeStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfEdge *e = (icfEdge*)cur->value;
    e->index  = iEdge;
    e->isLeaf = FALSE;
    e->merge  = FALSE;
    e->split  = FALSE;
    iEdge++;

    if (e->isSplit == FALSE)
      nEdgeLeafs += 1;
  }

#if (ICF_DEBUG > 2)
  icfPrint("FOUND %d EDGE LEAFS", nEdgeLeafs);
#endif

  /*-------------------------------------------------------
  | reallocate memory for leafs
  -------------------------------------------------------*/
  icfEdge **newEdgeLeafs;
  mesh->nEdgeLeafs = nEdgeLeafs;
  newEdgeLeafs = (icfEdge**) realloc(mesh->edgeLeafs, 
      nEdgeLeafs*sizeof(icfEdge*));
  if (newEdgeLeafs != NULL)
    mesh->edgeLeafs = newEdgeLeafs;

  icfTri **newTriLeafs;
  mesh->nTriLeafs = nTriLeafs;
  newTriLeafs = (icfTri**) realloc(mesh->triLeafs, 
      nTriLeafs*sizeof(icfTri*));
  mesh->triLeafs = newTriLeafs;

  /*-------------------------------------------------------
  | Set pointer-array to triangles and mark leafs
  -------------------------------------------------------*/
  iTri  = 0;
  
  for (cur = mesh->triStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfTri *t = (icfTri*)cur->value;

    if (t->isSplit == FALSE)
    {
      t->isLeaf            = TRUE;
      mesh->triLeafs[iTri] = t;
      t->leafPos           = iTri;
      iTri++;
    }
  }

  /*-------------------------------------------------------
  | Set pointer-array to edges and mark leafs
  -------------------------------------------------------*/
  iEdge = 0;
  for (cur = mesh->edgeStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfEdge *e = (icfEdge*)cur->value;

    if (e->isSplit == FALSE)
    {
      e->isLeaf              = TRUE;
      mesh->edgeLeafs[iEdge] = e;
      e->leafPos             = iEdge;
      iEdge++;
    }
  }

  /*-------------------------------------------------------
  | Update arrays for all mesh nodes 
  | This is also the point, where the nodes get 
  | their global indices
  -------------------------------------------------------*/
  icfNode **newNodes;
  newNodes = (icfNode**) realloc(mesh->nodes, 
      mesh->nNodes*sizeof(icfNode*));
  if (newNodes != NULL)
    mesh->nodes = newNodes;

  int iNode = 0;
  for (cur = mesh->nodeStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfNode *n = (icfNode*)cur->value;
    mesh->nodes[iNode] = n;
    n->index = iNode;
    iNode++;
  }

  /*-------------------------------------------------------
  | Update arrays for all boundary nodes and boundary 
  | edge leafs
  -------------------------------------------------------*/
  for (curBdry = mesh->bdryStack->first; 
       curBdry != NULL; curBdry = curBdry->next)
  {
    icfBdry *bdry = (icfBdry*)curBdry->value;

    /*-----------------------------------------------------
    | Boundary nodes
    -----------------------------------------------------*/
    icfNode **newBdryNodes;
    newBdryNodes = (icfNode**) realloc(bdry->bdryNodes, 
        bdry->nNodes*sizeof(icfNode*));
    if (newBdryNodes != NULL)
      bdry->bdryNodes = newBdryNodes;

    iNode = 0;
    for (cur = bdry->nodeStack->first; 
         cur != NULL; cur = cur->next)
    {
      icfNode *n = (icfNode*)cur->value;
      bdry->bdryNodes[iNode] = n;
      iNode++;
    }

    /*-----------------------------------------------------
    | Boundary edge leafs
    -----------------------------------------------------*/
    iEdge = 0;
    nEdgeLeafs = 0;
    for (cur = bdry->edgeStack->first; 
         cur != NULL; cur = cur->next)
    {
      icfEdge *e = (icfEdge*)cur->value;
      iEdge++;

      if (e->isSplit == FALSE)
        nEdgeLeafs += 1;
    }

    bdry->nEdgeLeafs = nEdgeLeafs;
    newEdgeLeafs = (icfEdge**) realloc(bdry->edgeLeafs, 
        nEdgeLeafs*sizeof(icfEdge*));
    if (newEdgeLeafs != NULL)
      bdry->edgeLeafs = newEdgeLeafs;


    iEdge = 0;
    for (cur = bdry->edgeStack->first; 
         cur != NULL; cur = cur->next)
    {
      icfEdge *e = (icfEdge*)cur->value;

      if (e->isSplit == FALSE)
      {
        bdry->edgeLeafs[iEdge] = e;
        iEdge++;
      }
    }

  }

  /*-------------------------------------------------------
  | Compute interior dual face normals
  | and element volumes
  -------------------------------------------------------*/
  icfMesh_calcDualMetrics(mesh);

  
  return;
error:
  return;

} /* icfMesh_update() */

/**********************************************************
* Function: icfMesh_calcDualMetrics()
*----------------------------------------------------------
* Fuction to compute the median dual normals for the mesh
* The dual normals are associated to the mesh edges.
* Edge normals point from n[0] to n[1].
*             /\
*           /    \
*         /   t0   \
*       /  ___/o     \
*     / __/    |       \
*   /  /       |xc       \
*  n0----------o--------->n1
*   \  \__     |         /
*     \   \__  |       /
*       \     \o     /
*         \   t1   /
*           \    /
*             \/
*
*----------------------------------------------------------
* @param mesh: pointer to mesh structure
**********************************************************/
void icfMesh_calcDualMetrics(icfMesh *mesh) 
{
  int      nEdges = mesh->nEdgeLeafs;
  icfEdge **edges = mesh->edgeLeafs;

  icfIndex iEdge;

  icfNode *n0, *n1;
  icfTri  *t0, *t1;

  /*-------------------------------------------------------
  | Compute interior face normals and associated 
  | median-dual element areas
  -------------------------------------------------------*/
  for (iEdge = 0; iEdge < nEdges; iEdge++)
  {
    icfEdge *edge = edges[iEdge];

    const icfDouble xc = edge->xy[0];
    const icfDouble yc = edge->xy[1];

    icfDouble dx0 = 0.0;
    icfDouble dy0 = 0.0;
    icfDouble dx1 = 0.0;
    icfDouble dy1 = 0.0;

    n0 = edge->n[0];
    n1 = edge->n[1];

    t0 = edge->t[0];
    if (t0 != NULL)
    {
      dx0 = t0->xy[0] - xc;
      dy0 = t0->xy[1] - yc;

      icfDouble a0 = (t0->xy[0]-n0->xy[0])*(yc-n0->xy[1])
                   - (t0->xy[1]-n0->xy[1])*(xc-n0->xy[0]);
      icfDouble a1 = (t0->xy[1]-n1->xy[1])*(xc-n1->xy[0])
                   - (t0->xy[0]-n1->xy[0])*(yc-n1->xy[1]);
      n0->vol -= 0.5 * a0;
      n1->vol -= 0.5 * a1;
    }

    t1 = edge->t[1];
    if (t1 != NULL)
    {
      dx1 = xc - t1->xy[0];
      dy1 = yc - t1->xy[1];

      icfDouble a0 = (t1->xy[1]-n0->xy[1])*(xc-n0->xy[0])
                   - (t1->xy[0]-n0->xy[0])*(yc-n0->xy[1]);
      icfDouble a1 = (t1->xy[0]-n1->xy[0])*(yc-n1->xy[1])
                   - (t1->xy[1]-n1->xy[1])*(xc-n1->xy[0]);
      n0->vol -= 0.5 * a0;
      n1->vol -= 0.5 * a1;
    }

    /* normals point from n0 to n1 */
    const icfDouble nx =  dy0 + dy1;
    const icfDouble ny = -dx0 - dx1;

    edge->intrNorm[0] += nx;
    edge->intrNorm[1] += ny;
  }

  /*-------------------------------------------------------
  | Compute boundary face normals
  | Boundary normals point outwards 
  | Boundary nodes contian normals of both adjacent 
  | boundary edges
  |        
  |   ----o----> n0 ----o----> n1 ----o----> 
  |          |       |     |        |
  |          |       |     |        |
  |          V       V     V        V
  | 
  -------------------------------------------------------*/
  icfListNode *curBdry;

  for (curBdry = mesh->bdryStack->first; 
       curBdry != NULL; curBdry = curBdry->next)
  {
    icfBdry *bdry = (icfBdry*)curBdry->value;

    for (iEdge = 0; iEdge < bdry->nEdgeLeafs; iEdge++)
    {
      icfEdge *e = bdry->edgeLeafs[iEdge];

      if (e->bdryNorm == NULL)
        e->bdryNorm = calloc(2, 2*sizeof(icfDouble));

      icfNode *n0 = e->n[0];
      icfNode *n1 = e->n[1];

      const icfDouble xc = e->xy[0];
      const icfDouble yc = e->xy[1];

      const icfDouble x0 = n0->xy[0];
      const icfDouble y0 = n0->xy[1];

      const icfDouble x1 = n1->xy[0];
      const icfDouble y1 = n1->xy[1];

      const icfDouble n0x =   yc - y0;
      const icfDouble n0y = -(xc - x0);
      const icfDouble n1x =   y1 - yc;
      const icfDouble n1y = -(x1 - xc);

      e->bdryNorm[0][0] = n0x;
      e->bdryNorm[0][1] = n0y;

      e->bdryNorm[1][0] = n1x;
      e->bdryNorm[1][1] = n1y;

    }
  }

} /* icfMesh_calcDualMetrics() */


/**********************************************************
* Function: icfMesh_printMesh()
*----------------------------------------------------------
* Fuction to print out the mesh data
*----------------------------------------------------------
* @param mesh: pointer to mesh structure
**********************************************************/
void icfMesh_printMesh(icfMesh *mesh) 
{
  icfListNode *cur;
  int i;

  /*-------------------------------------------------------
  | Set node indices and print node coordinates
  -------------------------------------------------------*/
  fprintf(stdout,"NODES %d\n", mesh->nNodes);
  for (cur = mesh->nodeStack->first; 
       cur != NULL; cur = cur->next)
  {
    icfDouble *xy = ((icfNode*)cur->value)->xy;
    icfIndex   i  = ((icfNode*)cur->value)->index;

    char *bdry_0, *bdry_1;
    icfNode *curNode = (icfNode*)cur->value;
    if (curNode->bdry[0] != NULL)
      bdry_0 = curNode->bdry[0]->name;
    else 
      bdry_0 = "None";
    if (curNode->bdry[1] != NULL)
      bdry_1 = curNode->bdry[1]->name;
    else 
      bdry_1 = "None";

    fprintf(stdout,"%d\t%9.5f\t%9.5f\t%s\t%s\n", 
        i, xy[0], xy[1], bdry_0, bdry_1);
  }

  /*-------------------------------------------------------
  | print triangles
  -------------------------------------------------------*/
  fprintf(stdout,"TRIANGLES %d\n", mesh->nTriLeafs);
  for (i = 0; i < mesh->nTriLeafs; i++)
  {
    icfTri *curTri = mesh->triLeafs[i];

    fprintf(stdout,"%d\t%d\t%d\t%d\n", 
        curTri->index, 
        curTri->n[0]->index,
        curTri->n[1]->index,
        curTri->n[2]->index);
  }

  /*-------------------------------------------------------
  | print mesh edges
  -------------------------------------------------------*/
  fprintf(stdout,"EDGES %d\n", mesh->nEdgeLeafs);
  for (i = 0; i < mesh->nEdgeLeafs; i++)
  {
    icfEdge *curEdge = mesh->edgeLeafs[i];

    icfIndex n0 = curEdge->n[0]->index;
    icfIndex n1 = curEdge->n[1]->index;

    icfIndex i0 = -1;
    icfIndex i1 = -1;

    icfTri *t0 = curEdge->t[0];
    icfTri *t1 = curEdge->t[1];

    char *bdry;
    if (curEdge->bdry != NULL)
      bdry = curEdge->bdry->name;
    else
      bdry = "None";

    if (t0 != NULL)
      i0 = t0->index;
    if (t1 != NULL)
      i1 = t1->index;

    curEdge->index = i;
    fprintf(stdout,"%d\t%9d\t%9d\t%9d\t%9d\t%s\n", i, 
        n0, n1, i0, i1, bdry);
  }

  /*-------------------------------------------------------
  | print triangles neighbors
  -------------------------------------------------------*/
  fprintf(stdout,"TRI NEIGHBORS %d\n", mesh->nTriLeafs);
  
  for (i = 0; i < mesh->nTriLeafs; i++)
  {
    icfTri *curTri = mesh->triLeafs[i];

    icfTri *t0 = curTri->t[0];
    icfTri *t1 = curTri->t[1];
    icfTri *t2 = curTri->t[2];

    icfEdge *e0 = curTri->e[0];
    icfEdge *e1 = curTri->e[1];
    icfEdge *e2 = curTri->e[2];

    icfIndex it0 = -1;
    icfIndex it1 = -1;
    icfIndex it2 = -1;

    icfIndex ie0 = -1;
    icfIndex ie1 = -1;
    icfIndex ie2 = -1;

    if (t0 != NULL)
      it0 = t0->index;
    if (t1 != NULL)
      it1 = t1->index;
    if (t2 != NULL)
      it2 = t2->index;

    if (e0 != NULL)
      ie0 = e0->index;
    if (e1 != NULL)
      ie1 = e1->index;
    if (e2 != NULL)
      ie2 = e2->index;

    fprintf(stdout,"%d\t%9d\t%9d\t%9d\t%9d\t%9d\t%9d\n", 
        curTri->index, it0, it1, it2, ie0, ie1, ie2);
  } 

} /* tmMesh_printMesh() */
