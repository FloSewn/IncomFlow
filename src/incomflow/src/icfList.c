/*
 * This source file is part of the incomflow library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 *
 * Most code in this file has been used from Zed Shaws Book
 * "Learn C the hard way"
 */
#include <stdlib.h>
#include "incomflow/icfList.h"
#include "incomflow/dbg.h"


/**************************************************************
* This function creates a new list structure
**************************************************************/
icfList *icfList_create()
{
  return calloc(1, sizeof(icfList));
}

/**************************************************************
* This function frees the memory of all icfListNodes of a given 
* list structure
**************************************************************/
void icfList_destroy(icfList *list)
{
  ICFLIST_FOREACH(list, first, next, cur) {
    if (cur->prev) {
      free(cur->prev);
    }
  }
  free(list->last);
  free(list);
}

/**************************************************************
* This function frees all memory of the data that is stored 
* in a given list structure
**************************************************************/
void icfList_clear(icfList *list)
{
  ICFLIST_FOREACH(list, first, next, cur) {
    free(cur->value);
  }
}

/**************************************************************
* This function frees all memory of a list structure
**************************************************************/
void icfList_clear_destroy(icfList *list) 
{
  icfList_clear(list);
  icfList_destroy(list);
}

/**************************************************************
* Push a new list node to the end of a list structure
**************************************************************/
void icfList_push(icfList *list, void *value)
{
  icfListNode *node = calloc(1, sizeof(icfListNode));
  check_mem(node);

  node->value = value;

  if (list->last == NULL) {
    list->first = node;
    list->last = node;
  }
  else {
    list->last->next = node;
    node->prev = list->last;
    list->last = node;
  }
  
  list->count++;

error:
  return;
}

/**************************************************************
* Remove the last node from a list structure
**************************************************************/
void *icfList_pop(icfList *list)
{
  icfListNode *node = list->last;
  return node != NULL ? icfList_remove(list, node) : NULL;
}

/**************************************************************
* Push a new list node to the beginning of a list structure
**************************************************************/
void icfList_unshift(icfList *list, void *value)
{
  icfListNode *node = calloc(1, sizeof(icfListNode));
  check_mem(node);

  node->value = value;

  if (list->first == NULL) {
    list->first = node;
    list->last = node;
  }
  else {
    node->next = list->first;
    list->first->prev = node;
    list->first = node;
  }
  
  list->count++;

error:
  return;
}

/**************************************************************
* Remove the first node from a list structure
**************************************************************/
void *icfList_shift(icfList *list)
{
  icfListNode *node = list->first;
  return node != NULL ? icfList_remove(list, node) : NULL;
}

/**************************************************************
* Remove a specific node from a list structure
**************************************************************/
void *icfList_remove(icfList *list, icfListNode *node)
{
  void *result = NULL;

  check(list->first && list->last, "icfList is empty.");
  check(node, "node can't be NULL.");

  if (node == list->first && node == list->last) {
    list->first = NULL;
    list->last = NULL;
  }
  else if (node == list->first) {
    list->first = node->next;
    check(list->first != NULL, "Invalid list, somehow got a first that is NULL.");
    list->first->prev = NULL;
  } 
  else if (node == list->last) {
    list->last = node->prev;
    check(list->last != NULL, "Invalid list, somehow got a last that is NULL.");
    list->last->next = NULL;
  }
  else {
    icfListNode *after = node->next;
    icfListNode *before = node->prev;

    after->prev = before;
    before->next = after;
  }
  
  list->count--;
  result = node->value;
  free(node);

error:
  return result;
}

/**************************************************************
* Appends <list2> to the end of <list1>
**************************************************************/
void *icfList_join(icfList *list1, icfList *list2)
{
  check(list1->first || list2->first, "Both lists are empty.");

  if (list1->first == NULL) {
    list1->first = list2->first;
    list1->last  = list2->last;
    list1->count = list2->count;

  }
  else {
    list1->last->next = list2->first;
    list2->first->prev = list1->last;
    list1->last = list2->last;
    list1->count += list2->count;

  }

  list2->count = 0;
  list2->first = NULL;
  list2->last  = NULL;

  icfList_destroy(list2);

error:
  return NULL;
}


/*************************************************************
* Splits a list <list1> at a specified index of the list
*************************************************************/
icfList *icfList_split(icfList *list1, int index)
{
  if (index == 0)
    return list1;

  if (list1->count <= 1)
    return list1;

  if (index >= list1->count || index < 0)
    return list1;

  icfListNode *node = list1->first;

  int i = 0;
  while (i < index)
  {
    i++;
    node = node->next;
  }

  icfList *list2  = icfList_create();
  list2->first = node;
  list2->last  = list1->last;

  list1->last  = node->prev;

  node->prev->next = NULL;
  node->prev       = NULL;

  list2->count = list1->count - i;
  list1->count = i;
  
  return list2;

}




/*************************************************************
* Function for swapping the values of two nodes in a 
* list structure
*************************************************************/
static inline void icfListNode_swap(icfListNode *a, icfListNode *b);

/*************************************************************
* Function for merging two lists 
*************************************************************/
static inline icfList *icfList_merge(icfList *left, icfList *right, 
                               icfList_compare cmp);



/*************************************************************
* Function for swapping the values of two nodes in a 
* list structure
*************************************************************/
static inline void icfListNode_swap(icfListNode *a, icfListNode *b)
{
  void *temp = a->value;
  a->value = b->value;
  b->value = temp;
}

/*************************************************************
* Function for merging two lists 
*************************************************************/
static inline icfList *icfList_merge(icfList *left, icfList *right, 
                               icfList_compare cmp)
{
  icfList *result = icfList_create();
  void *val = NULL;

  while (icfList_count(left) > 0 || icfList_count(right) > 0) {
    if (icfList_count(left) > 0 && icfList_count(right) > 0) {
      if (cmp(icfList_first(left), icfList_first(right)) <= 0) {
        val = icfList_shift(left);
      } 
      else {
        val = icfList_shift(right);
      }
      icfList_push(result, val);
    }
    else if (icfList_count(left) > 0) {
      val = icfList_shift(left);
      icfList_push(result, val);
    }
    else if (icfList_count(right) > 0) {
      val = icfList_shift(right);
      icfList_push(result, val);
    }
  }

  return result;

}

/*************************************************************
* Bubble sort algorithm
*************************************************************/
int icfList_bubble_sort(icfList *list, icfList_compare cmp)
{
  int sorted = 1;

  if (icfList_count(list) <= 1) {
    return 0; // Already sorted 
  }

  do {
    sorted = 1;
    ICFLIST_FOREACH(list, first, next, cur) {
      if (cur->next) {
        if (cmp(cur->value, cur->next->value) > 0) {
          icfListNode_swap(cur, cur->next);
          sorted = 0;
        }
      }
    }
  } while (!sorted);

  return 0;
}


/*************************************************************
* Merge sort algorithm
*************************************************************/
icfList *icfList_merge_sort(icfList *list, icfList_compare cmp)
{
  icfList *result = NULL;

  if (icfList_count(list) <= 1) {
    return list;
  }

  icfList *left  = icfList_create();
  icfList *right = icfList_create();
  int middle = icfList_count(list) / 2;

  ICFLIST_FOREACH(list, first, next, cur) {
    if (middle > 0) {
      icfList_push(left, cur->value);
    }
    else {
      icfList_push(right, cur->value);
    }

    middle--;
  }

  icfList *sort_left  = icfList_merge_sort(left, cmp);
  icfList *sort_right = icfList_merge_sort(right, cmp);

  if (sort_left != left)
    icfList_destroy(left);
  if (sort_right != right)
    icfList_destroy(right);

  result = icfList_merge(sort_left, sort_right, cmp);

  icfList_destroy(sort_left);
  icfList_destroy(sort_right);

  return result;

}
