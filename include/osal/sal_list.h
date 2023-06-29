/*
 * $Copyright Broadcom Corporation$
 *
 */
#ifndef __SAL_LIST_H__
#define __SAL_LIST_H__

#include "sal_types.h"

typedef struct _node_t
{
    struct _node_t *previous;
    struct _node_t *next;
} node_t;

typedef struct _list_t
{
    node_t node;
    uint32 count;
}list_t;

#define lst_tail node.previous
#define lst_head node.next

extern void        sal_lst_init(list_t * pLst);
extern void        sal_lst_insert(list_t * pLst, node_t * pPrevious, node_t * pNode);
extern void        sal_lst_add(list_t * pLst, node_t * pNode);
extern void        sal_lst_delete(list_t * pLst, node_t * pNode);
extern node_t      *sal_lst_first(list_t * pLst);
extern node_t      *sal_lst_last(list_t * pLst);
extern node_t      *sal_lst_get(list_t * pLst);
extern node_t      *sal_lst_prev(node_t * pNode);
extern node_t      *sal_lst_next(node_t * pNode);
extern uint32    sal_lst_count(list_t * pLst);

#define sal_lst_scan(pList,pNode,type) for(pNode=(type)sal_lst_first(pList);pNode;pNode=(type)sal_lst_next((node_t *)pNode))


#endif
