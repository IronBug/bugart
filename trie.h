typedef struct _trie_s
{
	char state;
	char * value;
	struct _trie_s * sibling;
	struct _trie_s * children;
}
trie_s;

trie_s *trie_new();
void trie_free(trie_s *);
trie_s *trie_node(char);
void trie_add(trie_s *, char *, int, char *);
char * trie_get(trie_s *, char *, int);
