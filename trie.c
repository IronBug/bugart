#include <stdlib.h>
#include <stdio.h>

struct _trie_s {
	char state;
	char * value;
	struct _trie_s * sibling;
	struct _trie_s * children;
};
typedef struct _trie_s trie_s;

trie_s * trie_new()
{
	trie_s * t = (trie_s *) malloc(sizeof(trie_s));
	t->state = '\0';
	t->value = NULL;
	t->sibling = NULL;
	t->children = NULL;
	return t;
}

void trie_free(trie_s * t)
{
	if(t->sibling)
		trie_free(t->sibling);
	if(t->children)
		trie_free(t->children);
	free(t);
}

trie_s * trie_node(char state)
{
	trie_s * t = trie_new();
	t->state = state;
	return t;
}

void trie_add(trie_s * t, char * data, int length, char * value)
{
	// look for the child
	trie_s * child = t->children;
	while(child) {
		if(child->state == *data)
			break;
		child = child->sibling;
	}

	// if the child doesn't exist add it
	if(!child) {
		child = trie_node(*data);
		child->sibling = t->children;
		t->children = child;
	}

	if(length == 1) {
		child->value = value;
	} else {
		// oh, tail recursion... how I wish you existed in C.
		trie_add(child, data + 1, length - 1, value);
	}
}

char * trie_get(trie_s * t, char * data, int length)
{
	trie_s * child = t;

	// look for the child
	while(length > 0) {
		child = child->children;
		while(child) {
			if(child->state == *data)
				break;
			child = child->sibling;
		}
		if(!child)
			return NULL;

		length--;
		data++;
	}

	return child->value;
}

/*
int main() {
	trie_s * trie = trie_new();
	trie_add(trie, "foo", 3, "foo");
	trie_add(trie, "bar", 3, "bar");
	trie_add(trie, "food", 4, "food");

	printf("foo: %s\n", trie_get(trie, "foo", 3));
	printf("food: %s\n", trie_get(trie, "food", 4));
	printf("bar: %s\n", trie_get(trie, "bar", 3));
	printf("argh: %s\n", trie_get(trie, "argh", 4));
}
*/
