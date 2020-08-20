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
* Function: icfIO_readMeshNodes
*----------------------------------------------------------
* Function to read the mesh nodes from a mesh file
* and writes them into an array of doubles
*----------------------------------------------------------
* @param:  txtlist  - text file
* @param:  xyNodes_ - array to write node coordinates
* @param:  nNodes_  - integer to write number of nodes
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
* Function: icfIO_readMesh
*----------------------------------------------------------
* Function to read a mesh file an create a mesh structure
* from it
*----------------------------------------------------------
* @return: pointer to new mesh structure
**********************************************************/
icfMesh *icfIO_readMesh(const char *meshFile)
{
  int nNodes;
  int nTris;

  icfDouble (*xyNodes)[2];
  icfIndex  (*idxTris)[3];


  /*----------------------------------------------------------
  | Set up file reader
  ----------------------------------------------------------*/
  icfIOReader *file = icfIO_createReader( meshFile );

  /*----------------------------------------------------------
  | read node coordinates
  ----------------------------------------------------------*/
  icfIO_readMeshNodes(file->txtlist, &xyNodes, &nNodes);

  icfIO_readMeshTriangles(file->txtlist, &idxTris, &nTris);

  return NULL;

} /* icfIO_readMesh() */
