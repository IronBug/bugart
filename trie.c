#include <stdlib.h>
#include <stdio.h>

struct _trie {
	char state;
	char * value;
	struct _trie * sibling;
	struct _trie * children;
};
typedef struct _trie trie;

trie * trie_new()
{
	trie * t = (trie *) malloc(sizeof(trie));
	t->state = '\0';
	t->value = NULL;
	t->sibling = NULL;
	t->children = NULL;
	return t;
}

void trie_free(trie * t)
{
	if(t->sibling)
		trie_free(t->sibling);
	if(t->children)
		trie_free(t->children);
	free(t);
}

trie * trie_node(char state)
{
	trie * t = trie_new();
	t->state = state;
	return t;
}

void trie_add(trie * t, char * data, int length, char * value)
{
	// look for the child
	trie * child = t->children;
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

char * trie_get(trie * t, char * data, int length)
{
	trie * child = t;

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
	trie * trie = trie_new();
	trie_add(trie, "foo", 3, "foo");
	trie_add(trie, "bar", 3, "bar");
	trie_add(trie, "food", 4, "food");

	printf("foo: %s\n", trie_get(trie, "foo", 3));
	printf("food: %s\n", trie_get(trie, "food", 4));
	printf("bar: %s\n", trie_get(trie, "bar", 3));
	printf("argh: %s\n", trie_get(trie, "argh", 4));
}
*/
