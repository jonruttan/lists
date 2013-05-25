/*
 * lists - Simple array types in C
 * Copyright (C) 2010 Jonathan Ruttan (JonRuttan@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: lists.h,v 0.6 2012-05-05 10:39:59 jon Exp $
 */

#ifndef __LISTS_H__
#define __LISTS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif


/* Int List structures */
typedef struct {
	long *start;
	long *ptr;
	long reserve;
	long grow;
	size_t width;
} IntListType;
typedef IntListType *IntListPtr;

/* Int List functions */
int IntListInit(IntListPtr list, long reserve, long grow);
void IntListFree(IntListPtr list);
void IntListDump(IntListPtr list, FILE *stream, int type);
int IntListReserve(IntListPtr list, unsigned long size, unsigned long grow);
long IntListSize(IntListPtr list);
int IntListRemove(IntListPtr list, long index, unsigned long size);
int IntListResize(IntListPtr list, unsigned long size, long value);
int IntListPush(IntListPtr list, long value);
int IntListPop(IntListPtr list, long *value);
long *IntListPointer(IntListPtr list, long index);
int IntListSet(IntListPtr list, long index, long value);
int IntListGet(IntListPtr list, long index, long *value);
long IntListFind(IntListPtr list, long value);
int IntListReverse(IntListPtr list, long index, unsigned long size);
int IntListRotate(IntListPtr list, long amount, long index, unsigned long size);
int IntListCopy(IntListPtr dest, IntListPtr src, long index, unsigned long size);
int IntListMove(IntListPtr dest, IntListPtr src, long index, unsigned long size);

/* Int List mapping callback functions */
typedef long (IntListMapFunc)(IntListPtr list, long index, long value, void *custom);
typedef int (IntListFilterFunc)(IntListPtr list, long value, void *custom);
typedef long (IntListReduceFunc)(IntListPtr list, long x, long value, void *custom);

/* Int List mapping functions */
int IntListMap(IntListPtr list, IntListMapFunc map_func, void *custom);
int IntListFilter(IntListPtr list, IntListFilterFunc filter_func, void *custom);
int IntListReduce(IntListPtr list, IntListReduceFunc reduce_func, long *value, long initial, void *custom);

void IntListPrint(IntListPtr list, FILE *stream, IntListMapFunc func);


/* Void List structures */
struct VoidListStruct;
typedef struct VoidListStruct VoidListType;
typedef VoidListType *VoidListPtr;

/* Void List callback functions */
typedef int (VoidListAllocFunc)(VoidListPtr list, void **, void *, size_t);
typedef void (VoidListFreeFunc)(VoidListPtr list, void *);
typedef void (VoidListCpyFunc)(VoidListPtr list, void *, void *, size_t);
typedef int (VoidListCmpFunc)(VoidListPtr list, void *, void *, size_t);

/* Void List structures continued */
struct VoidListStruct {
	void **start;
	void **ptr;
	unsigned long reserve;
	unsigned long grow;
	size_t width;
	VoidListAllocFunc *alloc_func;
	VoidListFreeFunc *free_func;
	VoidListCpyFunc *cpy_func;
	VoidListCmpFunc *cmp_func;
};

/* Void List functions */
int VoidListInit(VoidListPtr list, long reserve, long grow, size_t width,
		VoidListAllocFunc alloc_func, VoidListFreeFunc free_func,
		VoidListCpyFunc cpy_func, VoidListCmpFunc cmp_func);
void VoidListFree(VoidListPtr list);
void VoidListDump(VoidListPtr list, FILE *stream, int type);
int VoidListReserve(VoidListPtr list, unsigned long size, unsigned long grow, size_t width);
long VoidListSize(VoidListPtr list);
int VoidListRemove(VoidListPtr list, long index, long size);
int VoidListResize(VoidListPtr list, unsigned long size, void *value, size_t width);
void *VoidListPush(VoidListPtr list, void *value, size_t width);
void *VoidListPop(VoidListPtr list, void *value, size_t width);
void **VoidListPointer(VoidListPtr list, long index);
int VoidListSet(VoidListPtr list, long index, void *value, size_t width);
int VoidListGet(VoidListPtr list, long index, void *value, size_t width);
long VoidListFind(VoidListPtr list, void *value, size_t width);
int VoidListReverse(VoidListPtr list, long index, unsigned long size);
int VoidListRotate(VoidListPtr list, long amount, long index, unsigned long size);
int VoidListCopy(VoidListPtr dest, VoidListPtr src, long index, unsigned long size);
int VoidListMove(VoidListPtr dest, VoidListPtr src, long index, unsigned long size);

/* Void List mapping callback functions */
typedef void *(VoidListMapFunc)(VoidListPtr list, long index, void *value, void *custom);
typedef int (VoidListFilterFunc)(VoidListPtr list, void *value, void *custom);
typedef void *(VoidListReduceFunc)(VoidListPtr list, void *x, void *value, void *custom);

/* Void List mapping functions */
int VoidListMap(VoidListPtr list, VoidListMapFunc map_func, void *custom);
int VoidListFilter(VoidListPtr list, VoidListFilterFunc filter_func, void *custom);
int VoidListReduce(VoidListPtr list, VoidListReduceFunc reduce_func, void **value, void *initial, void *custom);

void VoidListPrint(VoidListPtr list, FILE *stream, VoidListMapFunc func);

/* Void List callback functions */
int voidlist_alloc(VoidListPtr list, void **ptr, void *value, size_t size);
void voidlist_free(VoidListPtr list, void *ptr);
void voidlist_cpy(VoidListPtr list, void *dest, void *src, size_t size);
int voidlist_cmp(VoidListPtr list, void *ptr1, void *ptr2, size_t size);
void *voidlist_print(VoidListPtr list, long index, void *value, void *custom);

/* String List types */
typedef VoidListType StrListType;
typedef StrListType *StrListPtr;

/* String List functions */
int StrListInit(StrListPtr list, long reserve, long grow);
#define StrListFree		VoidListFree
void StrListDump(StrListPtr list, FILE *stream, int type);
int StrListReserve(VoidListPtr list, unsigned long size, unsigned long grow);
#define StrListSize		VoidListSize
#define StrListRemove	VoidListRemove
int StrListResize(StrListPtr list, unsigned long size, char *value);
char *StrListPush(StrListPtr list, char *value);
char *StrListPop(StrListPtr list, char *value);
#define StrListPointer	VoidListPointer
int StrListSet(StrListPtr list, long index, char *value);
int StrListGet(StrListPtr list, long index, char *value);
long StrListFind(StrListPtr list, char *value);
long StrListFindCase(StrListPtr list, char *value);
#define StrListReverse	VoidListReverse
#define StrListRotate	VoidListRotate
#define StrListCopy		VoidListCopy
#define StrListMove		VoidListMove

/* String List mapping callback functions */
typedef char *(StrListMapFunc)(StrListPtr list, long index, char *value, void *custom);
typedef int (StrListFilterFunc)(StrListPtr list, char *value, void *custom);
typedef void *(StrListReduceFunc)(StrListPtr list, void *x, char *value, void *custom);

/* String mapping functions */
int StrListMap(StrListPtr list, StrListMapFunc map_func, void *custom);
int StrListFilter(StrListPtr list, StrListFilterFunc filter_func, void *custom);
int StrListReduce(StrListPtr list, StrListReduceFunc reduce_func, void **value, void *initial, void *custom);

void StrListPrint(StrListPtr list, FILE *stream, VoidListMapFunc func);

/* String List callback functions */
int strlist_alloc(StrListPtr list, void **ptr, void *value, size_t size);
void strlist_cpy(StrListPtr list, void *dest, void *src, size_t size);
int strlist_cmp(StrListPtr list, void *ptr1, void *ptr2, size_t size);
int strlist_icmp(StrListPtr list, void *ptr1, void *ptr2, size_t size);
void *strlist_print(StrListPtr list, long index, void *value, void *custom);

#endif /* __LISTS_H__ */
