struct _trie {
	char state;
	char * value;
	struct _trie * sibling;
	struct _trie * children;
};
typedef struct _trie trie;

trie * trie_new();
void trie_free(trie *);
trie * trie_node(char);
void trie_add(trie *, char *, int, char *);
char * trie_get(trie *, char *, int);
