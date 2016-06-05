// gcc -g lists.c lists_test.c -o lists_test

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lists.h"

#define TMPLEN			65536
#define WORDLIST		"/usr/share/dict/words"

void int_getset(IntListPtr list, long index, long value, long assert)
{
	long i;

	printf("Get Item %ld/%lu: ", index, IntListSize(list));
	if(IntListGet(list, index, &i))
		printf("%ld (should be %ld)\n", i, assert);
	else
		printf("Error.\n");

	printf("Set Item %ld/%lu to %ld: ", index, IntListSize(list), value);
	if(!IntListSet(list, index, value))
		printf("Error.\n");
	if(IntListGet(list, index, &i))
		printf("Item %ld: %ld (should now be %ld)\n", index, i, value);
}

void void_getset(VoidListPtr list, long index, char *value, char *assert)
{
	char s[TMPLEN];

	printf("Get Item %ld/%lu: ", index, VoidListSize(list));
	if(VoidListGet(list, index, s, strlen(assert) + 1))
		printf("%s (should be %s)\n", s, assert);
	else
		printf("Error.\n");

	printf("Set Item %ld/%lu: ", index, VoidListSize(list));
	if(!VoidListSet(list, index, value, strlen(value) + 1))
		printf("Error.\n");
	if(VoidListGet(list, index, s, strlen(value) + 1))
		printf("Item %ld: %s (should now be %s)\n", index, s, value);

}

void str_getset(StrListPtr list, long index, char *value, char *assert)
{
	char s[TMPLEN];

	printf("Get Item %ld/%lu: ", index, StrListSize(list));
	if(StrListGet(list, index, s))
		printf("%s (should be %s)\n", s, assert);
	else
		printf("Error.\n");

	printf("Set Item %ld/%lu to %s: ", index, StrListSize(list), value);
	if(!StrListSet(list, index, value))
		printf("Error.\n");
	if(StrListGet(list, index, s))
		printf("Item %ld: %s (should now be %s)\n", index, s, value);
}

int main()
{
	long i, reserve = 1024, grow = 1024, push = grow * 16;
	long neg_max = INT32_MIN;
	long size, n;
	long r;
	char *text = "Hello, World!", *text2 = "!dlroW ,olloH";
	char ptr[TMPLEN];
	FILE *file;

	IntListType int_list;

	printf("\n\n====== Testing Integer List ======\n\n");

	IntListInit(&int_list, reserve, grow);
	IntListDump(&int_list, stdout, 0);

	printf("Pushing %lu elements.\n", push);
	for(i=0; i < push; i++) {
		IntListPush(&int_list, i);
		if(i % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	IntListDump(&int_list, stdout, 0);

	r = push >> 1;
	printf("Searching for '%ld': ", r);
	if((i = IntListFind(&int_list, r)) >= 0)
		printf("Found item %ld.\n", i);
	else
		printf("not found.\n");

	printf("Pushing another %lu elements.\n", push);
	for(i=0; i < push; i++) {
		IntListPush(&int_list, i);
		if(i % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	IntListDump(&int_list, stdout, 0);

	printf("Popping %lu elements.\n", push);
	for(i=0; i < push; i++) {
		IntListPop(&int_list, NULL);
		if(i % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	IntListDump(&int_list, stdout, 0);

	size = IntListSize(&int_list);
	int_getset(&int_list, -size - 1, neg_max, 0);
	int_getset(&int_list, -size, neg_max, 0);
	int_getset(&int_list, -1, neg_max, size - 1);
	int_getset(&int_list, 0, neg_max, 0);
	int_getset(&int_list, size - 1, neg_max, size - 1);
	int_getset(&int_list, size, neg_max, size);

	n = IntListSize(&int_list) >> 2;
	printf("Removing first %lu/%lu element(s): ", n, IntListSize(&int_list));
	if(IntListRemove(&int_list, 0, n))
		printf("Okay\n");
	else
		printf("Error.\n");
	IntListDump(&int_list, stdout, 0);

	printf("Removing last %lu/%lu element(s): ", n, IntListSize(&int_list));
	if(IntListRemove(&int_list, IntListSize(&int_list) - n - 1, n))
		printf("Okay\n");
	else
		printf("Error.\n");
	IntListDump(&int_list, stdout, 0);

	printf("Freeing list.\n");
	IntListFree(&int_list);


	printf("\n\n====== Testing Void List ======\n\n");

	VoidListType void_list;

	VoidListInit(&void_list, reserve, grow, 0, voidlist_alloc, voidlist_free, voidlist_cpy, voidlist_cmp);
	VoidListDump(&void_list, stdout, 0);

	printf("Pushing %lu elements.\n", push);
	for(i=0; i < push; i++) {
		VoidListPush(&void_list, text, strlen(text) + 1);
		if(i % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	VoidListDump(&void_list, stdout, 0);

	printf("Pushing another %lu elements.\n", push);
	for(i=0; i < push; i++) {
		VoidListPush(&void_list, text, strlen(text) + 1);
		if(i % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	VoidListDump(&void_list, stdout, 0);

	printf("Popping %lu elements.\n", push);
	for(i=0; i < push; i++) {
		VoidListPop(&void_list, NULL, 0);
		if(i % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	VoidListDump(&void_list, stdout, 0);

	void_getset(&void_list, -size - 1, text2, text);
	void_getset(&void_list, -size, text2, text);
	void_getset(&void_list, -1, text2, text);
	void_getset(&void_list, 0, text2, text);
	void_getset(&void_list, size - 1, text2, text);
	void_getset(&void_list, size, text2, text);

	n = VoidListSize(&void_list) >> 2;
	printf("Removing first %lu/%lu element(s): ", n, VoidListSize(&void_list));
	if(VoidListRemove(&void_list, 0, n))
		printf("Okay\n");
	else
		printf("Error.\n");
	VoidListDump(&void_list, stdout, 0);

	printf("Removing last %lu/%lu element(s).\n", n, VoidListSize(&void_list));
	VoidListRemove(&void_list, VoidListSize(&void_list) - n - 1, n);
	VoidListDump(&void_list, stdout, 0);

	printf("Freeing list.\n");
	VoidListFree(&void_list);


	printf("\n\n====== Testing String List ======\n\n");

	StrListType str_list;

	StrListInit(&str_list, reserve, grow);
	StrListDump(&str_list, stdout, 0);

	if(!(file = fopen(WORDLIST, "r"))) {
		printf("Error: Wordlist %s doesn't exist\n", WORDLIST);
		return(-1);
	}

	i = -1;
	printf("Pushing wordlist (max. %lu)\n", i);
	while(fgets(ptr, 65536, file) && i--) {
		ptr[strlen(ptr) - 1] = 0;
		StrListPush(&str_list, ptr);
		if(StrListSize(&str_list) % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	StrListDump(&str_list, stdout, 0);

	fclose(file);

	printf("Searching for \"Hello\": ");
	if((i = StrListFind(&str_list, "Hello")) >= 0)
		printf("Found item %ld.\n", i);
	else
		printf("not found.\n");

	printf("Caseless search for \"Hello\": ");
	if((i = StrListFindCase(&str_list, "Hello")) >= 0)
		printf("Found item %ld.\n", i);
	else
		printf("not found.\n");


	printf("Pushing another %lu elements.\n", push);
	for(i=0; i < push; i++) {
		StrListPush(&str_list, text);
		if(i % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	StrListDump(&str_list, stdout, 0);

	printf("Popping %lu elements.\n", push);
	for(i=0; i < push; i++) {
		StrListPop(&str_list, NULL);
		if(i % grow == 0)
			putchar('.');fflush(stdout);
	} putchar('\n');
	StrListDump(&str_list, stdout, 0);

	size = StrListSize(&str_list);

	str_getset(&str_list, -size - 1, text, NULL);
	str_getset(&str_list, -size, text, NULL);
	str_getset(&str_list, -1, text2, "études");
	str_getset(&str_list, 0, text, "");
	str_getset(&str_list, size - 1, text, text2);
	str_getset(&str_list, size, text, NULL);

	n = size >> 2;
	printf("Removing first %lu/%lu element(s): ", n, StrListSize(&str_list));
	if(StrListRemove(&str_list, 0, n))
		printf("Okay\n");
	else
		printf("Error.\n");
	StrListDump(&str_list, stdout, 0);

	printf("Removing last %lu/%lu element(s).\n", n, StrListSize(&str_list));
	StrListRemove(&str_list, size - n - 1, n);
	StrListDump(&str_list, stdout, 0);

	printf("Freeing list.\n");
	StrListFree(&str_list);

	return(0);
}
