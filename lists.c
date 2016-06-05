/*
 * lists - Simple array types in C
 * Copyright (C) 2013 Jonathan Ruttan (JonRuttan@gmail.com)
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
 * $Id: lists.c,v 0.7 2013-05-25 00:03:59 jon Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lists.h"

/***************************
 *  Integer List functions
 */
int IntListInit(IntListPtr list, long reserve, long grow)
{
	list->start = list->ptr = NULL;
	list->reserve = list->grow = 0;

	if(reserve && (list->start = list->ptr = (long *)malloc(reserve * sizeof(long))) == NULL)
		return(FALSE);

	list->reserve = reserve;
	list->grow = grow;

	return(TRUE);
}

void IntListFree(IntListPtr list)
{
	if(list->start)
	{
		free(list->start);
		list->start = NULL;
	}

	list->ptr = NULL;
	list->reserve = 0;
	list->grow = 0;
}

void IntListDump(IntListPtr list, FILE *stream, int type)
{
	if(type) {
		fprintf(stream, "Integer List [0x%p]\n", list);
		fprintf(stream, " |--Start: 0x%p\n", list->start);
		fprintf(stream, " |--Pointer: 0x%p(+%zu)\n", list->ptr, list->ptr - list->start);
		fprintf(stream, " |--Reserve: %lu\n", list->reserve);
		fprintf(stream, " +--Grow: +%lu\n\n", list->grow);
	} else
		fprintf(stream, "Integer List [0x%p] Data: [0x%p:+%zu], Reserve [%lu:+%lu]\n",
				list, list->start, list->ptr - list->start,
				list->reserve, list->grow);
}

long intlist_print(IntListPtr list, long index, long value, void *custom)
{
	fprintf(custom, "%ld ", value);
	return(value);
}

void IntListPrint(IntListPtr list, FILE *stream, IntListMapFunc func)
{
	fprintf(stream, "[ ");
	IntListMap(list, func ? func : intlist_print, stream);
	fprintf(stream, "]\n");
}

int IntListReserve(IntListPtr list, unsigned long size, unsigned long grow)
{
	unsigned long index = list->ptr - list->start;
	long *ptr;

	if(!size)
		size = index;

	if(grow)
		list->grow = grow;

	if(size < index)
		size = index;

	size = (size / list->grow + 1) * list->grow;

	if(size < list->grow)
		size = list->grow;

	if((ptr = realloc(list->start, size * sizeof(long))) == NULL)
		return(FALSE);

	list->start = ptr;
	list->ptr = ptr + index;
	list->reserve = size;

	return(TRUE);
}

long IntListSize(IntListPtr list)
{
	return(list->ptr - list->start);
}

int IntListRemove(IntListPtr list, long index, unsigned long size)
{
	long *dest = list->start + index, *src = dest + size;

	if(!size || src > list->ptr)
		return(FALSE);

	if(src < list->ptr)
		memmove(dest, src, (list->ptr - src) * sizeof(long));

	list->ptr -= size;

	IntListReserve(list, 0, 0);

	return(TRUE);
}

int IntListResize(IntListPtr list, unsigned long size, long value)
{
	unsigned long index = list->ptr - list->start;
	long *ptr;

	if(size == index)
		return(TRUE);

	if(size < index)
		return(IntListRemove(list, size, index - size));

	IntListReserve(list, size, 0);
	ptr = list->start + size;
	while(list->ptr < ptr)
		*list->ptr++ = value;

	return(TRUE);
}

int IntListPush(IntListPtr list, long value)
{
	if(list->ptr >= list->start + list->reserve)
		if(!IntListReserve(list, 0, 0))
			return(FALSE);

	*list->ptr = value;
	list->ptr++;

	return(TRUE);
}

int IntListPop(IntListPtr list, long *value)
{
	unsigned long size;

	if(list->ptr <= list->start)
		return(FALSE);

	list->ptr--;

	if(value)
		*value = *list->ptr;

	if(list->reserve - (list->grow << 1) >= (size = list->ptr - list->start))
		IntListReserve(list, size, 0);

	return(TRUE);
}

long *IntListPointer(IntListPtr list, long index)
{
	long *ptr = (index < 0 ? list->ptr : list->start) + index;

	if(ptr < list->start || ptr >= list->ptr)
		return(NULL);

	return(ptr);
}

int IntListSet(IntListPtr list, long index, long value)
{
	long *ptr;

	if(!(ptr = IntListPointer(list, index)))
		return(FALSE);

	*ptr = value;

	return(TRUE);
}

int IntListGet(IntListPtr list, long index, long *value)
{
	long *ptr;

	if(!(ptr = IntListPointer(list, index)))
		return(FALSE);

	if(value)
		*value = *ptr;

	return(TRUE);
}

long IntListFind(IntListPtr list, long value)
{
	long *ptr = list->ptr;

	while(--ptr >= list->start)
		if(*ptr == value)
			return(ptr - list->start);

	return(-1);
}

int IntListReverse(IntListPtr list, long index, unsigned long size)
{
	long tmp, *ptr[2];
	unsigned long list_size = list->ptr - list->start;

	if(!list_size || index + size > list_size)
		return(FALSE);

	if(!size)
		size = list_size - index;

	if(size < 2)
		return(TRUE);

	if(!(ptr[0] = IntListPointer(list, index)))
		return(FALSE);

	ptr[1] = ptr[0] + size - 1;

	while(ptr[0] < ptr[1]) {
		memcpy(&tmp, ptr[1], sizeof(long));
		memcpy(ptr[1], ptr[0], sizeof(long));
		memcpy(ptr[0], &tmp, sizeof(long));
		ptr[0]++, ptr[1]--;
	}

	return(TRUE);
}

int IntListRotate(IntListPtr list, long amount, long index, unsigned long size)
{
	long *tmp, *ptr[2];
	unsigned long list_size = list->ptr - list->start;

	if(!list_size || index + size > list_size)
		return(FALSE);

	if(!size)
		size = list_size - index;

	if(size < 2)
		return(TRUE);

	if(!(ptr[0] = IntListPointer(list, index)))
		return(FALSE);

	ptr[1] = ptr[0] + size;

	if((tmp = malloc(labs(amount) * sizeof(long))) == NULL)
		return(FALSE);

	if(amount > 0) {
		memcpy(tmp, ptr[1] - amount, sizeof(long) * amount);
		memmove(ptr[0] + amount, ptr[0], sizeof(long) * (size - amount));
		memcpy(ptr[0], tmp, sizeof(long) * amount);
	} else {
		amount = -amount;
		memcpy(tmp, ptr[0], sizeof(long) * amount);
		memmove(ptr[0], ptr[0] + amount, sizeof(long) * (size - amount));
		memcpy(ptr[1] - amount, tmp, sizeof(long) * amount);
	}

	free(tmp);

	return(TRUE);
}

int IntListCopy(IntListPtr dest, IntListPtr src, long index, unsigned long size)
{
	long *ptr;
	unsigned long dest_size = dest->ptr - dest->start,
			src_size = src->ptr - src->start;

	if(index + size > src_size)
		return(FALSE);

	if(!(ptr = IntListPointer(src, index)))
		return(FALSE);

	if(!size)
		size = src_size - index;

	if(!IntListReserve(dest, dest_size + size, src->grow))
		return(FALSE);

	memmove(dest->ptr, ptr, sizeof(long) * size);

	dest->ptr += size;

	return(TRUE);
}

int IntListMove(IntListPtr dest, IntListPtr src, long index, unsigned long size)
{
	if(!(IntListCopy(dest, src, index, size)))
		return(FALSE);

	if(!IntListResize(src, 0, 0))
		return(FALSE);

	return(TRUE);
}

/* Int List mapping functions */
int IntListMap(IntListPtr list, IntListMapFunc map_func, void *custom)
{
	long *ptr = list->start, index = 0;

	while(ptr < list->ptr) {
		*ptr = map_func(list, index++, *ptr, custom);
		ptr++;
	}

	return(TRUE);
}

int IntListFilter(IntListPtr list, IntListFilterFunc filter_func, void *custom)
{
	long *ptr = list->ptr, n;

	while(--ptr >= list->start)
		if(!filter_func(list, *ptr, custom))
			if((n = --list->ptr - ptr))
				memmove(ptr, ptr + 1, n * sizeof(long));

	return(TRUE);
}

int IntListReduce(IntListPtr list, IntListReduceFunc reduce_func, long *value, long initial, void *custom)
{
	long *ptr = list->ptr;

	while(--ptr >= list->start)
		initial = reduce_func(list, initial, *ptr, custom);

	if(value)
		*value = initial;

	return(TRUE);
}



/************************
 *  Void List functions
 */
int VoidListInit(VoidListPtr list, long reserve, long grow, size_t width,
		VoidListAllocFunc alloc_func, VoidListFreeFunc free_func,
		VoidListCpyFunc cpy_func, VoidListCmpFunc cmp_func)
{
	list->start = list->ptr = NULL;
	list->reserve = list->grow = 0;

	if(reserve && (list->start = list->ptr = (void **)malloc(reserve * sizeof(void *))) == NULL)
		return(FALSE);

	list->reserve = reserve;
	list->grow = grow;
	list->width = width;
	list->alloc_func = alloc_func ? alloc_func : voidlist_alloc;
	list->free_func = free_func ? free_func : voidlist_free;
	list->cpy_func = cpy_func ? cpy_func : voidlist_cpy;
	list->cmp_func = cmp_func ? cmp_func : voidlist_cmp;

	return(TRUE);
}

void VoidListFree(VoidListPtr list)
{
	if(list->free_func) {
		while(list->ptr > list->start)
			list->free_func(list, *--list->ptr);

		free(list->start);
		list->start = NULL;
	}

	list->reserve = 0;
	list->grow = 0;
	list->width = 0;
	list->alloc_func = NULL;
	list->free_func = NULL;
	list->cpy_func = NULL;
	list->cmp_func = NULL;
}

void _VoidListDump(VoidListPtr list, FILE *stream, int type, char *name)
{
	if(type) {
		fprintf(stream, "%s[0x%p]\n", name, list);
		fprintf(stream, " |--Start: 0x%p\n", list->start);
		fprintf(stream, " |--Pointer: 0x%p(+%zu)\n", list->ptr, list->ptr - list->start);
		fprintf(stream, " |--Reserve: %lu units\n", list->reserve);
		fprintf(stream, " |--Grow: +%lu units\n\n", list->grow);
		fprintf(stream, " +--Width: +%zu bytes\n\n", list->width);
	} else
		fprintf(stream, "%s[0x%p] Data: [0x%p:+%zu], Reserve [%lu:+%lu], Width [%zu b.]\n",
				name, list, list->start, list->ptr - list->start,
				list->reserve, list->grow, list->width);
}

void VoidListDump(VoidListPtr list, FILE *stream, int type)
{
	_VoidListDump(list, stream, type, "Void List");
}

void *voidlist_print(VoidListPtr list, long index, void *value, void *custom)
{
	fprintf(custom, "0x%p ", value);
	return(value);
}

void VoidListPrint(VoidListPtr list, FILE *stream, VoidListMapFunc func)
{
	fprintf(stream, "[ ");
	VoidListMap(list, func ? func : voidlist_print, stream);
	fprintf(stream, "]\n");
}

int VoidListReserve(VoidListPtr list, unsigned long size, unsigned long grow, size_t width)
{
	unsigned long index = list->ptr - list->start;
	void **ptr;

	if(!size)
		size = index;

	if(grow)
		list->grow = grow;

	if(size < index)
		size = index;

	size = (size / list->grow + 1) * list->grow;

	if(size < list->grow)
		size = list->grow;

	if((ptr = realloc(list->start, size * sizeof(void *))) == NULL)
		return(FALSE);

	list->start = ptr;
	list->ptr = ptr + index;
	list->reserve = size;
	if(width)
		list->width = width;

	return(TRUE);
}

long VoidListSize(VoidListPtr list)
{
	return(list->ptr - list->start);
}

int VoidListRemove(VoidListPtr list, long index, long size)
{
	void **dest = list->start + index, **src = dest + size, **ptr = dest;
	long n = size;

	if(!size || src > list->ptr)
		return(FALSE);

	while(n--)
		list->free_func(list, *ptr++);

	if(src < list->ptr)
		memmove(dest, src, (list->ptr - src) * sizeof(void *));

	list->ptr -= size;

	VoidListReserve(list, index + size, 0, 0);

	return(TRUE);
}

int VoidListResize(VoidListPtr list, unsigned long size, void *value, size_t width)
{
	unsigned long index = list->ptr - list->start;
	void **ptr;

	if(size == index)
		return(TRUE);

	if(size < index)
		return(VoidListRemove(list, size, index - size));

	VoidListReserve(list, size, 0, 0);
	ptr = list->start + size;
	while(list->ptr < ptr)
		list->alloc_func(list, list->ptr++, value, width ? width : list->width);

	return(TRUE);
}

void *VoidListPush(VoidListPtr list, void *value, size_t width)
{
	if(list->ptr >= list->start + list->reserve)
		if(!VoidListReserve(list, list->reserve, 0, 0))
			return NULL;

	list->alloc_func(list, list->ptr, value, width ? width : list->width);
	return *list->ptr++;
}

void *VoidListPop(VoidListPtr list, void *value, size_t width)
{
	unsigned long size;

	if(list->ptr <= list->start)
		return NULL;

	list->ptr--;

	if(value)
		list->cpy_func(list, value, *list->ptr, width ? width : list->width);

	list->free_func(list, *list->ptr);

	if(list->reserve - (list->grow << 1) >= (size = list->ptr - list->start))
		VoidListReserve(list, size, 0, 0);

	return value;
}

void **VoidListPointer(VoidListPtr list, long index)
{
	void **ptr = (index < 0 ? list->ptr : list->start) + index;

	if(ptr < list->start || ptr >= list->ptr)
		return(NULL);

	return(ptr);
}

int VoidListSet(VoidListPtr list, long index, void *value, size_t width)
{
	void **ptr;

	if(!(ptr = VoidListPointer(list, index)))
		return(FALSE);

	list->free_func(list, *ptr);
	list->alloc_func(list, ptr, value, width ? width : list->width);

	return(TRUE);
}

int VoidListGet(VoidListPtr list, long index, void *value, size_t width)
{
	void **ptr;

	if(!(ptr = VoidListPointer(list, index)))
		return(FALSE);

	if(value)
		list->cpy_func(list, value, *ptr, width ? width : list->width);

	return(TRUE);
}

long VoidListFind(VoidListPtr list, void *value, size_t width)
{
	void **ptr = list->ptr;

	while(--ptr >= list->start)
		if(list->cmp_func(list, *ptr, value, width ? width : list->width))
			return(ptr - list->start);

	return(-1);
}

int VoidListReverse(VoidListPtr list, long index, unsigned long size)
{
	void *tmp, **ptr[2];
	unsigned long list_size = list->ptr - list->start;

	if(!list_size || index + size > list_size)
		return(FALSE);

	if(!size)
		size = list_size - index;

	if(size < 2)
		return(TRUE);

	if(!(ptr[0] = VoidListPointer(list, index)))
		return(FALSE);

	ptr[1] = ptr[0] + size - 1;

	while(ptr[0] < ptr[1]) {
		memcpy(&tmp, ptr[1], sizeof(void *));
		memcpy(ptr[1], ptr[0], sizeof(void *));
		memcpy(ptr[0], &tmp, sizeof(void *));
		ptr[0]++, ptr[1]--;
	}

	return(TRUE);
}

int VoidListRotate(VoidListPtr list, long amount, long index, unsigned long size)
{
	void **tmp, **ptr[2];
	unsigned long list_size = list->ptr - list->start;

	if(!list_size || index + size > list_size)
		return(FALSE);

	if(!size)
		size = list_size - index;

	if(size < 2)
		return(TRUE);

	if(!(ptr[0] = VoidListPointer(list, index)))
		return(FALSE);

	ptr[1] = ptr[0] + size;

	if((tmp = malloc(labs(amount) * sizeof(void *))) == NULL)
		return(FALSE);

	if(amount > 0) {
		memcpy(tmp, ptr[1] - amount, sizeof(void *) * amount);
		memmove(ptr[0] + amount, ptr[0], sizeof(void *) * (size - amount));
		memcpy(ptr[0], tmp, sizeof(void *) * amount);
	} else {
		amount = -amount;
		memcpy(tmp, ptr[0], sizeof(void *) * amount);
		memmove(ptr[0], ptr[0] + amount, sizeof(void *) * (size - amount));
		memcpy(ptr[1] - amount, tmp, sizeof(void *) * amount);
	}

	free(tmp);

	return(TRUE);
}

int VoidListCopy(VoidListPtr dest, VoidListPtr src, long index, unsigned long size)
{
	void **ptr;
	unsigned long dest_size = dest->ptr - dest->start,
			src_size = src->ptr - src->start;

	if(index + size > src_size)
		return(FALSE);

	if(!(ptr = VoidListPointer(src, index)))
		return(FALSE);

	if(!size)
		size = src_size - index;

	if(!VoidListReserve(dest, dest_size + size, src->grow, 0))
		return(FALSE);

	while(size--)
		dest->alloc_func(dest, dest->ptr++, *ptr++, src->width);

	return(TRUE);
}

int VoidListMove(VoidListPtr dest, VoidListPtr src, long index, unsigned long size)
{
	if(!(VoidListCopy(dest, src, index, size)))
		return(FALSE);

	if(!VoidListResize(src, 0, NULL, 0))
		return(FALSE);

	return(TRUE);
}

/* Void List mapping functions */
int VoidListMap(VoidListPtr list, VoidListMapFunc map_func, void *custom)
{
	void **ptr = list->start;
	long index = 0;

	while(ptr < list->ptr) {
		*ptr = map_func(list, index++, *ptr, custom);
		ptr++;
	}

	return(TRUE);
}

int VoidListFilter(VoidListPtr list, VoidListFilterFunc filter_func, void *custom)
{
	void **ptr = list->ptr;
	long n;

	while(--ptr >= list->start)
		if(!filter_func(list, *ptr, custom)) {
			list->free_func(list, *ptr);

			if((n = --list->ptr - ptr))
				memmove(ptr, ptr + 1, n * sizeof(long));
		}

	return(TRUE);
}

int VoidListReduce(VoidListPtr list, VoidListReduceFunc reduce_func, void **value, void *initial, void *custom)
{
	void **ptr = list->ptr;
	long size = ptr - list->start;

	while(size--)
		initial = reduce_func(list, initial, *--ptr, custom);

	if(value)
		*value = initial;

	return(TRUE);
}



/* Void List callback functions */
int voidlist_alloc(VoidListPtr list, void **ptr, void *value, size_t width)
{
	//*ptr = malloc(width);
	*ptr = calloc(1, width);

	if(value)
		list->cpy_func(list, *ptr, value, width);

	return(TRUE);
}

void voidlist_free(VoidListPtr list, void *ptr)
{
	free(ptr);
}

void voidlist_cpy(VoidListPtr list, void *dest, void *src, size_t width)
{
	memcpy(dest, src, width);
}

int voidlist_cmp(VoidListPtr list, void *ptr1, void *ptr2, size_t width)
{
	return(memcmp(ptr1, ptr2, width));
}


/**************************
 *  String List Functions
 */
int StrListInit(StrListPtr list, long reserve, long grow)
{
	return(VoidListInit(list, reserve, grow, 0, strlist_alloc, voidlist_free, strlist_cpy, strlist_cmp));
}

void StrListDump(StrListPtr list, FILE *stream, int type)
{
	_VoidListDump(list, stream, type, "String List");
}

int StrListReserve(VoidListPtr list, unsigned long size, unsigned long grow)
{
	return(VoidListReserve(list, size, grow, 0));
}

int StrListResize(StrListPtr list, unsigned long size, char *value)
{
	return(VoidListResize(list, size, value, 0));
}
char *StrListPush(StrListPtr list, char *value)
{
	return (char *)VoidListPush(list, value, 0);
}

char *StrListPop(StrListPtr list, char *value)
{
	return (char *)VoidListPop(list, value, 0);
}

int StrListSet(StrListPtr list, long index, char *value)
{
	return(VoidListSet(list, index, value, 0));
}

int StrListGet(StrListPtr list, long index, char *value)
{
	return(VoidListGet(list, index, value, 0));
}

long StrListFind(StrListPtr list, char *value)
{
	/* Note: this will bash current compare function */
	list->cmp_func = strlist_cmp;
	return(VoidListFind(list, value, 0));
}

long StrListFindCase(StrListPtr list, char *value)
{
	/* Note: this will bash current compare function */
	list->cmp_func = strlist_icmp;
	return(VoidListFind(list, value, 0));
}

/* String mapping functions */
int StrListMap(StrListPtr list, StrListMapFunc map_func, void *custom)
{
	return(VoidListMap(list, *((VoidListMapFunc *)&map_func), custom));
}

int StrListFilter(StrListPtr list, StrListFilterFunc filter_func, void *custom)
{
	return(VoidListFilter(list, *((VoidListFilterFunc *)&filter_func), custom));
}

int StrListReduce(StrListPtr list, StrListReduceFunc reduce_func, void **value, void *initial, void *custom)
{
	return(VoidListReduce(list, *((VoidListReduceFunc *)&reduce_func), value, initial, custom));
}

void StrListPrint(VoidListPtr list, FILE *stream, VoidListMapFunc func)
{
	VoidListPrint(list, stream, func ? func : strlist_print);
}

/* String List callback functions */
int strlist_alloc(VoidListPtr list, void **ptr, void *value, size_t width)
{
	return(voidlist_alloc(list, ptr, value, strlen(value) + 1));
}

void strlist_cpy(VoidListPtr list, void *dest, void *src, size_t width)
{
	strcpy(dest, src);
}

int strlist_cmp(VoidListPtr list, void *ptr1, void *ptr2, size_t width)
{
	return(strcmp(ptr1, ptr2));
}

int strlist_icmp(VoidListPtr list, void *ptr1, void *ptr2, size_t width)
{
	return(strcasecmp(ptr1, ptr2));
}

void *strlist_print(VoidListPtr list, long index, void *value, void *custom)
{
	fprintf(custom, "%s ", (char *)value);
	return(value);
}

