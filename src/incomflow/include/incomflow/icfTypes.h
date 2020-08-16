/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef INCOMFLOW_ICFTYPES_H
#define INCOMFLOW_ICFTYPES_H

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "incomflow/dbg.h"
#include "incomflow/icfList.h"


/***********************************************************
* Own type definitions in order to allow to switch from 
* switch to short and from double to long in case of 
* special memory requirements
***********************************************************/
#define icfDouble double
#define icfIndex  int
#define icfBool   int
#define TRUE      1
#define FALSE     0

/***********************************************************
* Basic structure typedefs
***********************************************************/
typedef struct icfNode      icfNode;
typedef struct icfEdge      icfEdge;
typedef struct icfTri       icfTri;
typedef struct icfMesh      icfMesh;
typedef struct icfBdry      icfBdry;
typedef struct icfFlowData  icfFlowData;

/***********************************************************
* Function pointers
***********************************************************/
typedef icfBool (*icfRefineFun) (icfFlowData *flowData, icfTri *tri);


/***********************************************************
* Debugging Layers
* 0 -> No output
***********************************************************/
#define ICF_DEBUG 3

#ifdef ICF_DEBUG 
#define icfPrint(M, ...) fprintf(stdout, "> " M "\n",\
    ##__VA_ARGS__)
#else
#define icfPrint(M, ...)
#endif

/***********************************************************
* Constants used in code
***********************************************************/
#define PI_D 3.1415926535897932384626433832795


#endif
