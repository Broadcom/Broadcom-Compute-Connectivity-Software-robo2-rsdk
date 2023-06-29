/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_list.c
 * * Purpose:
 * *     Robo2 linux osal module. This provides abstraction for linux
 * *     list related service.
 * *
 * */
#include "sal_list.h"

void sal_lst_init(list_t * pLst)
{
    pLst->lst_head = (void *)0;
    pLst->lst_tail = (void *)0;
    pLst->count = 0;
}

void sal_lst_insert(list_t * pLst, node_t * pPrevious, node_t * pNode)
{
    node_t * pNext = NULL;

    if (NULL == pPrevious) {
        pNext = pLst->lst_head;
        pLst->lst_head = pNode;
    } else {
        pNext = pPrevious->next;
        pPrevious->next = pNode;
    }

    if (NULL == pNext) {
        pLst->lst_tail = pNode;
    } else {
        pNext->previous = pNode;
    }

    pNode->previous = pPrevious;
    pNode->next = pNext;

    pLst->count++;
}

void sal_lst_add(list_t * pLst, node_t * pNode)
{
    sal_lst_insert(pLst, pLst->lst_tail, pNode);
}

void sal_lst_delete(list_t * pLst, node_t * pNode)
{
    if (pNode->previous == NULL) {
        pLst->lst_head = pNode->next;
    } else {
        pNode->previous->next = pNode->next;
    }

    if (pNode->next == NULL) {
        pLst->lst_tail = pNode->previous;
    } else {
        pNode->next->previous = pNode->previous;
    }

    pLst->count--;
}

node_t * sal_lst_first(list_t * pLst)
{
    return pLst->lst_head;
}

node_t * sal_lst_last(list_t * pLst)
{
    return pLst->lst_tail;
}

node_t * sal_lst_get(list_t * pLst)
{
    node_t * pNode = NULL;

    pNode = pLst->lst_head;

    if (pNode != NULL) {
        pLst->lst_head = pNode->next;

        if (pNode->next == NULL) {
            pLst->lst_tail = NULL;
        } else {
            pNode->next->previous = NULL;
        }

        pLst->count--;
    }

    return pNode;
}

node_t * sal_lst_prev(node_t * pNode)
{
    return pNode->previous;
}

node_t * sal_lst_next(node_t * pNode)
{
    return pNode->next;
}

uint32 sal_lst_count(list_t * pLst)
{
    return pLst->count;
}

