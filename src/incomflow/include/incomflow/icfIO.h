/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef INCOMFLOW_ICFIO_H
#define INCOMFLOW_ICFIO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "incomflow/icfTypes.h"
#include "incomflow/bstrlib.h"

#define FILIO_ERR -1


/*************************************************************
* file reader structure
*************************************************************/
struct icfIOReader;
typedef struct icfIOReader {
  const char      *path;    /* Path of file                 */
  bstring          txt;     /* bstring with file data       */
  struct bstrList *txtlist; /* file, splitted for newlines  */

  long             length;  /* Number of chars in total file*/
                            /* -> including '\0' at end     */
  int              nlines;  /* Number of lines in total file*/

} icfIOReader;

/*************************************************************
* Function to create a new I/O file reader structure
*************************************************************/
icfIOReader *icfIO_createReader(const char *file_path);

/*************************************************************
* Function to destroy a file reader structure
*************************************************************/
int icfIO_destroyReader(icfIOReader *file);

/*************************************************************
* Function returns a bstring list of lines, that 
* do not contain a certain specifier
*************************************************************/
struct bstrList *icfIO_popLinesWith(struct bstrList *txtlist,
                                    const char *fltr);

/*************************************************************
* Function returns a bstring list of lines, that 
* do contain a certain specifier
*************************************************************/
struct bstrList *icfIO_getLinesWith(struct bstrList *txtlist,
                                    const char *fltr);

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
                       void *value);

/*************************************************************
* Function searches for a specifier <fltr> in a bstrList.
* The string behind the specifier is then extracted 
* from the file and processed as an array of values 
* and stored in <value>.
* The values are casted to a prescribed type
* type = 0: integer
* type = 1: double
* type = 2: string
*
* Returns 0 if specifier was not found in the file.
* Otherwise, it returns the number of times, the 
* specifier was found.
* Returns -1 on errors.
*************************************************************/
int icfIO_extractArray(struct bstrList *txtlist,
                       const char *fltr, int type,
                       void *value);



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
                         int             *nNodes_);

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
                             int             *nTris_);

/**********************************************************
* Function: icfIO_readMesh
*----------------------------------------------------------
* Function to read a mesh file an create a mesh structure
* from it
*----------------------------------------------------------
* @return: pointer to new mesh structure
**********************************************************/
icfMesh *icfIO_readMesh(const char *meshFile);

#endif
