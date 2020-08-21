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

#include "incomflow/dbg.h"
#include "incomflow/bstrlib.h"
#include "incomflow/icfIO.h"


/*************************************************************
* Function to create a new parameter file reader
*
* Reference:
* https://stackoverflow.com/questions/14002954/c-programming
* -how-to-read-the-whole-file-contents-into-a-buffer
*************************************************************/
icfIOReader *icfIO_createReader(const char *file_path)
{
  /*---------------------------------------------------------
  | Allocate memory for txtio structure 
  ---------------------------------------------------------*/
  icfIOReader *txtfile = calloc(1, sizeof(icfIOReader));
  check_mem(txtfile);

  txtfile->path = file_path;

  /*---------------------------------------------------------
  | Open text file and copy its data 
  ---------------------------------------------------------*/
  FILE *fptr = NULL;
  fptr = fopen(txtfile->path, "rb");
  check(fptr, "Failed to open %s.", txtfile->path);

  /* Estimate length of chars in whole file                */
  fseek(fptr, 0, SEEK_END);
  long length = ftell(fptr);
  fseek(fptr, 0, SEEK_SET);

  /* Read total file into buffer                           */
  char *buffer = (char *) malloc(length + 1);
  buffer[length] = '\0';
  int nReturn = fread(buffer, 1, length, fptr);

  /* Copy relevant data to icfIOReader structure           */
  bstring bbuffer = bfromcstr( buffer );
  txtfile->txt    = bbuffer;
  txtfile->length = length + 1;

  /* Split buffer according to newlines                    */
  char splitter = '\n';
  txtfile->txtlist = bsplit(bbuffer, splitter);
  txtfile->nlines = txtfile->txtlist->qty;

  fclose(fptr);
  free(buffer);

  return txtfile;
error:
  return NULL;
}

/*************************************************************
* Function to destroy a file reader structure
*************************************************************/
int icfIO_destroyReader(icfIOReader *file)
{
  bstrListDestroy(file->txtlist);
  bdestroy(file->txt);
  free(file);
  return 0;
}

/*************************************************************
* Function returns a bstring list of lines, that 
* do not contain a specifier
*************************************************************/
struct bstrList *icfIO_popLinesWith(struct bstrList *txtlist, 
                                    const char *fltr)
{
  bstring *fl_ptr = txtlist->entry;
  bstring  bfltr  = bfromcstr(fltr); 

  bstring *bl_ptr     = NULL;
  struct bstrList *bl = NULL;
  int i;
  int hits = 0;
  int cnt  = 0;
  int *marker = (int*) calloc(txtlist->qty, sizeof(int));
  check_mem(marker);

  /*----------------------------------------------------------
  | Fill array of markers with line numbers, that do not 
  | contain the filter string
  ----------------------------------------------------------*/
  for (i = 0; i < txtlist->qty; i++) 
  {
    if ( binstr(fl_ptr[i], 0, bfltr) == BSTR_ERR )
    {
      hits += 1;
      marker[i] = i;
    }
    else
    {
      marker[i] = -1;
    }
  }

  /*----------------------------------------------------------
  | Create new bstrList that will contained all filtered lines
  ----------------------------------------------------------*/
  bl = bstrListCreate();
  bstrListAlloc(bl, hits);

  /*----------------------------------------------------------
  | Copy marked lines into new bstrList
  ----------------------------------------------------------*/
  bl_ptr = bl->entry;

  for (i = 0; i < txtlist->qty; i++) 
  {
    if (marker[i] >= 0)
    {
      const int curline = marker[i];
      bl_ptr[cnt] = bstrcpy(fl_ptr[curline]);
      bl->qty += 1;
      cnt += 1;
    }
  }

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy(bfltr);
  free(marker);

  return bl;
error:

  bdestroy(bfltr);
  free(marker);
  return NULL;
}

/*************************************************************
* Function returns a bstring list of lines, that 
* do contain a certain specifier
*************************************************************/
struct bstrList *icfIO_getLinesWith(struct bstrList *txtlist,
                                    const char *fltr)
{
  bstring *fl_ptr = txtlist->entry;
  bstring  bfltr  = bfromcstr( fltr ); 

  bstring *bl_ptr     = NULL;
  struct bstrList *bl = NULL;

  int i;
  int hits = 0;
  int cnt  = 0;
  int *marker = (int*) calloc(txtlist->qty, sizeof(int));
  check_mem(marker);

  /*----------------------------------------------------------
  | Fill array of markers with line numbers, that  
  | contain the filter string
  ----------------------------------------------------------*/
  for (i = 0; i < txtlist->qty; i++) 
  {
    if ( binstr(fl_ptr[i], 0, bfltr) != BSTR_ERR )
    {
      hits += 1;
      marker[i] = i;
    }
    else
    {
      marker[i] = -1;
    }
  }

  /*----------------------------------------------------------
  | Create new bstrList that will contained all filtered lines
  ----------------------------------------------------------*/
  bl = bstrListCreate();
  bstrListAlloc(bl, hits);

  /*----------------------------------------------------------
  | Copy marked lines into new bstrList
  ----------------------------------------------------------*/
  bl_ptr = bl->entry;

  for (i = 0; i < txtlist->qty; i++) 
  {
    if (marker[i] >= 0)
    {
      const int curline = marker[i];
      bl_ptr[cnt] = bstrcpy(fl_ptr[curline]);
      bl->qty += 1;
      cnt += 1;
    }
  }

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy(bfltr);
  free(marker);

  return bl;

error:
  bdestroy(bfltr);
  free(marker);
  return NULL;
}

/*************************************************************
* Function searches for a specifier <fltr> in a bstrList.
* The parameter behind the specifier is then extracted 
* from the file and stored into <value>.
* The value is casted to a prescribed type
* type = 0: integer
* type = 1: double
* type = 2: string
*
* Returns 0 if specifier was not found in the file.
* Otherwise, it returns the number of times, the 
* specifier was found.
* Returns -1 on errors.
*************************************************************/
int icfIO_extractParam(struct bstrList *txtlist,
                       const char *fltr, int type,
                       void *value)
{
  int i;
  int nfound = 0;
  bstring line, valstr;
  struct bstrList *fltTxt = NULL;
  struct bstrList *blstextr = NULL;
  bstring bfltr, bextr;

  /*----------------------------------------------------------
  | Get all lines, containing the specifier
  ----------------------------------------------------------*/
  fltTxt = icfIO_getLinesWith(txtlist, fltr);
  nfound = fltTxt->qty;

  /*----------------------------------------------------------
  | Return if specifier is not found
  ----------------------------------------------------------*/
  if (fltTxt->qty < 1)
  {
    bstrListDestroy(fltTxt);
    return 0;
  }

  /*----------------------------------------------------------
  | Take last string, in which specifier was found
  ----------------------------------------------------------*/
  line  = fltTxt->entry[fltTxt->qty - 1];
  bfltr = bfromcstr( fltr ); 
  
  int off = binstr(line, 0, bfltr); 
  int len = bfltr->slen;

  bextr = bmidstr( line, off+len, line->slen );

  /*----------------------------------------------------------
  | Remove leading whitespaces and copy first value
  ----------------------------------------------------------*/
  valstr = bextr;

  if (type == 0)
    *(int*)value = atoi(valstr->data);
  else if (type == 1)
    *(double*)value = atof(valstr->data);
  else if (type == 2)
    *(bstring*)value = bfromcstr( valstr->data );
  else
    log_err("Wrong type definition.");

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  bstrListDestroy( blstextr );

  return nfound;

error:
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  bstrListDestroy( blstextr );
  return -1;
}

/*************************************************************
* Function searches for a specifier <fltr> in a bstrList.
* The string behind the specifier is then extracted 
* from the file and processed as an array of values 
* and stored in <value>.
* The values are casted to a prescribed type
* type = 0: integer
* type = 1: double
* type = 2: bstrList (not working )
*
* Returns 0 if specifier was not found in the file.
* Otherwise, it returns the number of times, the 
* specifier was found.
* Returns -1 on errors.
*************************************************************/
int icfIO_extractArray(struct bstrList *txtlist,
                       const char *fltr, int type,
                       void *value)
{

  int i;
  int nfound = 0;
  bstring line;
  struct bstrList *fltTxt = NULL;
  bstring bfltr, bextr;

  /*----------------------------------------------------------
  | Get all lines, containing the specifier
  ----------------------------------------------------------*/
  fltTxt = icfIO_getLinesWith(txtlist, fltr);
  nfound = fltTxt->qty;

  /*----------------------------------------------------------
  | Return if specifier is not found
  ----------------------------------------------------------*/
  if (nfound < 1)
  {
    bstrListDestroy(fltTxt);
    return 0;
  }

  /*----------------------------------------------------------
  | Take last string, in which specifier was found
  ----------------------------------------------------------*/
  line  = fltTxt->entry[fltTxt->qty - 1];
  bfltr = bfromcstr( fltr ); 

  int off = binstr(line, 0, bfltr); 
  int len = bfltr->slen;

  bextr = bmidstr( line, off+len, line->slen );

  /*----------------------------------------------------------
  | Remove leading whitespaces and copy first value
  ----------------------------------------------------------*/
  bstring wsfnd = bfromcstr( " " );
  bstring wsrpl = bfromcstr( "" );
  bfindreplace(bextr, wsfnd, wsrpl, 0);

  /*----------------------------------------------------------
  | Split into list of string -> comma is separator
  ----------------------------------------------------------*/
  struct bstrList *arrStr = bsplit(bextr, ',');
  int nEntries            = arrStr->qty;
  bstring *arr_ptr        = arrStr->entry;

  if (type == 0)
  {
    int *array = calloc(nEntries, sizeof(int));

    for (i = 0; i < nEntries; i++) 
      array[i] = atoi(arr_ptr[i]->data);

    *(int**)value = array;

  }
  else if (type == 1)
  {
    double *array = calloc(nEntries, sizeof(double));

    for (i = 0; i < nEntries; i++) 
      array[i] = atof(arr_ptr[i]->data);

    *(double**)value = array;

  }
  else if (type == 2)
  {
    *(struct bstrList**)value = arrStr;
  }
  else
    log_err("Wrong type definition.");

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy( wsfnd );
  bdestroy( wsrpl );
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  bstrListDestroy( arrStr);

  return nfound;

error:
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  return -1;

} /* icfIO_extractArray() */

/**********************************************************
* Function: icfIO_readMeshNodes
*----------------------------------------------------------
* Function to read the mesh nodes from a mesh file
* and writes them into an array of doubles
*----------------------------------------------------------
* @param:  txtlist  - text file
* @param:  xyNodes_ - array to write node coordinates
* @param:  nNodes_  - integer to write number of nodes
**********************************************************/
void icfIO_readMeshNodes(struct bstrList *txtlist,
                         icfDouble      (**xyNodes_)[2], 
                         int             *nNodes_)
{
  icfDouble (*xyNodes)[2];
  int nNodes = 0;

  int i, iMax, nValues, nodeID;

  bstring line;
  bstring *valPtr;
  struct bstrList *values;

  /*----------------------------------------------------------
  | Get total number of nodes 
  ----------------------------------------------------------*/
  icfIO_extractParam(txtlist, "NODES", 0, &nNodes);
  check(nNodes > 0, "No nodes defined in mesh file.");

  /*----------------------------------------------------------
  | Get line index where we start to gather nodes
  ----------------------------------------------------------*/
  bstring *flPtr  = txtlist->entry;
  bstring markStr = bfromcstr( "NODES" );

  for (i = 0; i < txtlist->qty; i++)
    if ( binstr(flPtr[i], 0, markStr) != BSTR_ERR )
      break;

  iMax = i + nNodes + 1;

  xyNodes = calloc(nNodes, 2*sizeof(icfDouble));

  for (i = i+1; i < iMax; i++)
  {
    line  = flPtr[i];

    values  = bsplit(line, '\t');
    nValues = values->qty;
    valPtr  = values->entry;

    check (nValues == 3, 
        "Wrong definition for node coordinates.");

    nodeID = atoi(valPtr[0]->data);

    xyNodes[nodeID][0] = atof(valPtr[1]->data);
    xyNodes[nodeID][1] = atof(valPtr[2]->data);

    bstrListDestroy( values );

  }

  bdestroy( markStr );

  *xyNodes_ = xyNodes;
  *nNodes_  = nNodes;

  return;

error:
  return;

} /* icfIO_readMeshNodes() */


/**********************************************************
* Function: icfIO_readMeshTriangles
*----------------------------------------------------------
* Function to read the mesh nodes from a mesh file
* and writes them into an array of ints
*----------------------------------------------------------
* @param:  txtlist  - text file
* @param:  idxTris_ - array to write triangles node indices
* @param:  nTris_   - integer to write number of triangles
**********************************************************/
void icfIO_readMeshTriangles(struct bstrList *txtlist,
                             icfIndex      (**idxTris_)[3], 
                             int             *nTris_)
{
  icfIndex (*idxTris)[3];
  int nTris = 0;

  int i, iMax, nValues, triID;

  bstring line;
  bstring *valPtr;
  struct bstrList *values;

  /*----------------------------------------------------------
  | Get total number of triangles 
  ----------------------------------------------------------*/
  icfIO_extractParam(txtlist, "TRIANGLES", 0, &nTris);
  check(nTris > 0, "No triangles defined in mesh file.");

  /*----------------------------------------------------------
  | Get line index where we start to gather tris
  ----------------------------------------------------------*/
  bstring *flPtr  = txtlist->entry;
  bstring markStr = bfromcstr( "TRIANGLES" );

  for (i = 0; i < txtlist->qty; i++)
    if ( binstr(flPtr[i], 0, markStr) != BSTR_ERR )
      break;

  iMax = i + nTris + 1;

  idxTris = calloc(nTris, 3*sizeof(icfIndex));

  for (i = i+1; i < iMax; i++)
  {
    line    = flPtr[i];
    values  = bsplit(line, '\t');
    nValues = values->qty;
    valPtr  = values->entry;

    check (nValues == 4, 
        "Wrong definition for triangles.");

    triID = atoi(valPtr[0]->data);

    idxTris[triID][0] = atof(valPtr[1]->data);
    idxTris[triID][1] = atof(valPtr[2]->data);
    idxTris[triID][2] = atof(valPtr[3]->data);

    bstrListDestroy( values );

  }

  bdestroy( markStr );

  *idxTris_ = idxTris;
  *nTris_   = nTris;

  return;

error:
  return;

} /* icfIO_readMeshTriangles() */

/**********************************************************
* Function: icfIO_readMeshNeighbors
*----------------------------------------------------------
* Function to read the mesh triangle neighbor connectivity
* from a mesh file and writes them into an array of ints
*----------------------------------------------------------
* @param:  txtlist  - text file
* @param:  idxTriNbrs_ - array to write tri-neighbor indices 
* @param:  nNbrs_   - integer to write number of triangles
**********************************************************/
void icfIO_readMeshTriNbrs(struct bstrList *txtlist,
                           icfIndex      (**idxTriNbrs_)[3], 
                           int             *nTris_)
{
  icfIndex (*idxTriNbrs)[3];
  int nTris = 0;

  int i, iMax, nValues, triID;

  bstring line;
  bstring *valPtr;
  struct bstrList *values;

  /*----------------------------------------------------------
  | Get total number of triangles 
  ----------------------------------------------------------*/
  icfIO_extractParam(txtlist, "NEIGHBORS", 0, &nTris);
  check(nTris > 0, "No triangles defined in mesh file.");

  /*----------------------------------------------------------
  | Get line index where we start to gather tris
  ----------------------------------------------------------*/
  bstring *flPtr  = txtlist->entry;
  bstring markStr = bfromcstr( "NEIGHBORS" );

  for (i = 0; i < txtlist->qty; i++)
    if ( binstr(flPtr[i], 0, markStr) != BSTR_ERR )
      break;

  iMax = i + nTris + 1;

  idxTriNbrs = calloc(nTris, 3*sizeof(icfIndex));

  for (i = i+1; i < iMax; i++)
  {
    line    = flPtr[i];
    values  = bsplit(line, '\t');
    nValues = values->qty;
    valPtr  = values->entry;

    check (nValues == 4, 
        "Wrong definition for triangles.");

    triID = atoi(valPtr[0]->data);

    idxTriNbrs[triID][0] = atof(valPtr[1]->data);
    idxTriNbrs[triID][1] = atof(valPtr[2]->data);
    idxTriNbrs[triID][2] = atof(valPtr[3]->data);

    bstrListDestroy( values );

  }

  bdestroy( markStr );

  *idxTriNbrs_ = idxTriNbrs;
  *nTris_   = nTris;

  return;

error:
  return;

} /* icfIO_readMeshNeighbors() */

/**********************************************************
* Function: icfIO_readMesh
*----------------------------------------------------------
* Function to read a mesh file an create a mesh structure
* from it
*----------------------------------------------------------
* @param : meshFile - string with path to a mesh file
* @param : mesh - pointer to mesh structure
* @return: pointer to new mesh structure
**********************************************************/
void icfIO_readMesh(const char *meshFile, icfMesh *mesh)
{
  int nNodes;
  int nTris;
  int nEdges;
  int i,j;

  icfDouble (*xyNodes)[2];
  icfIndex  (*idxTris)[3];
  icfIndex  (*idxTriNbrs)[3];

  /*----------------------------------------------------------
  | Set up file reader
  ----------------------------------------------------------*/
  icfIOReader *file = icfIO_createReader( meshFile );

  /*----------------------------------------------------------
  | read node coordinates
  ----------------------------------------------------------*/
  icfIO_readMeshNodes(file->txtlist, &xyNodes, &nNodes);

  /*----------------------------------------------------------
  | read triangle connectivity
  ----------------------------------------------------------*/
  icfIO_readMeshTriangles(file->txtlist, &idxTris, &nTris);

  /*----------------------------------------------------------
  | read triangle neighborhood connectivity
  ----------------------------------------------------------*/
  icfIO_readMeshTriNbrs(file->txtlist, &idxTriNbrs, &nTris);

  /*----------------------------------------------------------
  | Create mesh nodes
  ----------------------------------------------------------*/
  icfNode **n = calloc(nNodes, sizeof(icfNode*));

  for (i = 0; i < nNodes; i++)
    n[i] = icfNode_create(mesh, xyNodes[i]);

  /*----------------------------------------------------------
  | Create mesh triangles
  ----------------------------------------------------------*/
  icfTri **t = calloc(nTris, sizeof(icfTri*));

  for (i = 0; i < nTris; i++)
  {
    t[i] = icfTri_create(mesh);
    icfTri_setNodes(t[i], n[idxTris[i][0]], 
                    n[idxTris[i][1]], n[idxTris[i][2]]);
  }

  /*----------------------------------------------------------
  | Create mesh edges
  | Determine number of edges from Euler's formula:
  | >   e = n + t - 1
  | Source:
  | https://en.wikipedia.org/wiki/Planar_graph#Euler.27s_formula)
  |
  |          n2 _____
  |          / \     /
  |      t1 / t \ t0/
  |        /_____\ /
  |      n0  t2   n1
  ----------------------------------------------------------*/
  nEdges = nNodes + nTris - 1;
  icfEdge **e = calloc(nEdges, sizeof(icfEdge*));

  icfListNode *cur;
  int iEdge = 0;

  for (i = 0; i < nTris; i++)
  {

    for (j = 0; j < 3; j++)
    {
      icfBdry *bdry = NULL;
      int triNbr    = idxTriNbrs[i][j];

      /*------------------------------------------------------
      | Create boundary edge (negative neighbor indices)
      ------------------------------------------------------*/
      if (triNbr < 0)
      {
        int marker = -triNbr;

        int iBdry = 0;
        for (cur = mesh->bdryStack->first;
             cur != NULL; cur = cur->next)
        {
          if (((icfBdry*)cur->value)->marker == marker)
          {
            bdry = (icfBdry*)cur->value;
            break;
          }
          iBdry++;
        }
        check(bdry != NULL, "Found undefined boundary marker %d in mesh.", marker);

        int n0     = idxTris[i][(j+1)%3];
        int n1     = idxTris[i][(j+2)%3];

        e[iEdge] = icfEdge_create(mesh);
        icfEdge_setNodes(e[iEdge], n[n0], n[n1]);
        icfEdge_setTris(e[iEdge], t[i], NULL);

        icfBdry_addEdge(bdry, e[iEdge]);
        icfBdry_addNode(bdry, n[n0], 0);
        icfBdry_addNode(bdry, n[n1], 1);

        iEdge++;

        t[i]->t[j] = NULL;
      }
      /*------------------------------------------------------
      | Create interior edge (only once)
      ------------------------------------------------------*/
      else if (triNbr > i)
      {
        int n0 = idxTris[i][(j+1)%3];
        int n1 = idxTris[i][(j+2)%3];

        e[iEdge] = icfEdge_create(mesh);

        icfEdge_setNodes(e[iEdge], n[n0], n[n1]);
        icfEdge_setTris(e[iEdge], t[i], t[triNbr]);

        t[i]->e[(j+1)%3] = e[iEdge];

        if (n[n0] == t[triNbr]->n[0])
          t[triNbr]->e[2] = e[iEdge];
        else if (n[n0] == t[triNbr]->n[1])
          t[triNbr]->e[0] = e[iEdge];
        else if (n[n0] == t[triNbr]->n[2])
          t[triNbr]->e[1] = e[iEdge];
        else
          log_err("Wrong triangle connectivity in mesh.");

        iEdge++;

        t[i]->t[j] = t[triNbr];
      }
      else if (triNbr < i)
      {
        t[i]->t[j] = t[triNbr];
      }
    }
  }


  /*----------------------------------------------------------
  | Free arrays
  ----------------------------------------------------------*/
  free(xyNodes);
  free(idxTris);
  free(idxTriNbrs);
  free(n);
  free(t);
  free(e);

  return;
error:
  free(xyNodes);
  free(idxTris);
  free(idxTriNbrs);
  free(n);
  free(t);
  free(e);

  return;

} /* icfIO_readMesh() */
