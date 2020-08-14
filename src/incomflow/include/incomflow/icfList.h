/*
 * This header file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 *
 * Most code in this file has been used from Zed Shaws Book
 * "Learn C the hard way"
 */
#ifndef INCOMFLOW_ICFLIST_H
#define INCOMFLOW_ICFLIST_H


/**************************************************************
* Defines for the handling with list structures
**************************************************************/
/* Returns the count of a list */
#define icfList_count(A) ((A)->count)
/* Return the value of the first list entry */
#define icfList_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
/* Return the value of the last list entry */
#define icfList_last(A) ((A)->last != NULL ? (A)->last->value : NULL)
/* Return the first list entry */
#define icfList_first_node(A) ((A)->first != NULL ? (A)->first : NULL)
/* Return the last list entry */
#define icfList_last_node(A) ((A)->last != NULL ? (A)->last : NULL)
/* Perform a loop over a list structure */
#define ICFLIST_FOREACH(L, S, M, V) icfListNode *_node = NULL;\
                                 icfListNode *V = NULL;\
for(V = _node = L->S; _node != NULL; V = _node = _node->M)



/**************************************************************
* ListNode Structure
**************************************************************/
struct icfListNode;
typedef struct icfListNode {
  struct icfListNode *prev;
  struct icfListNode *next;
  void *value;
} icfListNode;

/**************************************************************
* List Structure
**************************************************************/
typedef struct icfList {
  int count;
  icfListNode *first;
  icfListNode *last;
} icfList;

/*************************************************************
* Function pointer to compare function
*************************************************************/
typedef int (*icfList_compare) (const void *a, const void *b);



/**************************************************************
* This function creates a new list structure
**************************************************************/
icfList *icfList_create();

/**************************************************************
* This function frees the memory of all ListNodes of a given 
* list structure
**************************************************************/
void icfList_destroy(icfList *list);

/**************************************************************
* This function frees all memory of the data that is stored 
* in a given list structure
**************************************************************/
void icfList_clear(icfList *list);

/**************************************************************
* This function frees all memory of a list structure
**************************************************************/
void icfList_clear_destroy(icfList *list);

/**************************************************************
* Push a new list node to the end of a list structure
**************************************************************/
void icfList_push(icfList *list, void *value);

/**************************************************************
* Remove the last node from a list structure
**************************************************************/
void *icfList_pop(icfList *list);

/**************************************************************
* Push a new list node to the beginning of a list structure
**************************************************************/
void icfList_unshift(icfList *list, void *value);

/**************************************************************
* Remove the first node from a list structure
**************************************************************/
void *icfList_shift(icfList *list);

/**************************************************************
* Remove a specific node from a list structure
**************************************************************/
void *icfList_remove(icfList *list, icfListNode *node);

/**************************************************************
* Appends <list2> to the end of <list1> and frees 
* the <list2> structure.
**************************************************************/
void *icfList_join(icfList *list1, icfList *list2);

/*************************************************************
* Splits a list <list1> at a specified Listnode <node>
*************************************************************/
icfList *icfList_split(icfList *list1, int index);


/*************************************************************
* Bubble sort algorithm
*************************************************************/
int icfList_bubble_sort(icfList *list, icfList_compare cmp);

/*************************************************************
* Merge sort algorithm
*************************************************************/
icfList *icfList_merge_sort(icfList *list, icfList_compare cmp);

#endif /* INCOMFLOW_ICFLIST_H */
