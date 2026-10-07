Mini Search Index in C

A keyword indexing engine built from scratch in C, using a multiway trie, a doubly linked list and a custom max-heap. Standard library only.

The problem

The goal is a simplified data organization system: a mechanism that indexes files by the keywords associated with them.

Each file is characterized by:

a unique identifier (the file name)
a relevance score
a list of keywords

The relevance score is a number that says how well a file matches a search based on its keywords. A higher score means a better match.

The system must support inserting, deleting, updating and searching files by keyword. It must also answer TOPK queries (the k most relevant files for a keyword) and PREFIX queries (all files having a keyword that starts with a given prefix).

To see how such a collection is stored, take this example:

File1, score 95: algebra, analiza
File2, score 88: patrat, patrulater
File3, score 91: triunghi, trigonometrie
File4, score 84: tetraedru, paralelipiped
File5, score 99: algebra, trigonometrie, triunghi
File6, score 77: analiza, patrat, paralel
The list of files

The files themselves are kept in a doubly linked list, in the order in which they appear in the system:

NULL <- [File1;95] <-> [File2;88] <-> [File3;91] <-> [File4;84] <-> [File5;99] <-> [File6;77] -> NULL
         head                                                                        tail
The keyword tree

The keywords are stored in a multiway trie. Each node corresponds to one character, and a path from the root to a terminal node spells a complete keyword. The files are not stored in the tree: each terminal node only keeps a list of references to the files that contain that word. For the collection above, the tree stores algebra, analiza, patrat, patrulater, triunghi, trigonometrie, tetraedru, paralelipiped and paralel. Words that share a beginning (patrat and patrulater, or algebra and analiza) share the nodes of their common prefix.

Consistency

The index must stay consistent while files and keywords are added and removed. Deleting a file removes its references from every keyword it was attached to. If a terminal node is left with no files, the word is removed from the tree, and the nodes that became useless (not terminal and without children) are deleted recursively, so the tree never keeps dead branches.

Commands
ADD id score t w1 ... wt: index a new document with t keywords
DEL id: remove a document and every reference to it
ADDKW id word: add a keyword to an existing document
DELKW id word: remove a keyword from a document
FIND word: documents containing the word, in lexicographic order
TOPK word k: the k most relevant documents for the word (score descending, id ascending on ties)
PRINT: the whole index, one word per line, in alphabetical order
PREFIX p: documents with at least one keyword starting with p, each listed once
Design
Documents: a doubly linked list in insertion order. Each cell holds the id, the score and the number of keywords. A document whose last keyword is removed is deleted automatically.
Keywords: a multiway trie in left-child / right-sibling form. left is the first child and right is the next sibling, with siblings kept in alphabetical order. No 26-pointer arrays are needed, and an in-order traversal gives sorted words for free.
Terminal nodes: each holds a linked list of pointers to documents, sorted by id.
Ranking: an array-based max-heap with a function-pointer comparator. TOPK collects the references of a word, builds the heap and extracts k elements.

Documents are stored once and the trie only points to them, so there is a single source of truth and nothing to keep in sync.

Notable details
On delete, nodes that are no longer terminal and have no children are freed, so the trie has no dead branches.
PREFIX finds the node of the last prefix character, traverses only that subtree and merges the references into one deduplicated, sorted list.
Words are read with a custom tokenizer that grows its buffer as needed, so input length is not fixed.
Every allocation has an owner and a matching free. The project is checked with Valgrind.
Build and run
make build
./search_index

The program reads indexare.in and writes indexare.out.

Example

indexare.in:

12
ADD File1 95 2 algebra analiza
ADD File2 88 2 patrat patrulater
ADD File3 91 2 triunghi trigonometrie
ADD File4 84 2 tetraedru paralelipiped
ADD File5 99 3 algebra trigonometrie triunghi
ADD File6 77 3 analiza patrat paralelipiped
FIND algebra
TOPK triunghi 3
ADDKW File4 triunghi
TOPK triunghi 4
DELKW File3 triunghi
PRINT

indexare.out:

OK
OK
OK
OK
OK
OK
2 File1 File5
2 File5 File3
OK
3 File5 File3 File4
OK
algebra 2 File1 File5
analiza 2 File1 File6
paralelipiped 2 File4 File6
patrat 2 File2 File6
patrulater 1 File2
tetraedru 1 File4
trigonometrie 2 File3 File5
triunghi 2 File4 File5

Reading the output: TOPK triunghi 3 returns File5 (score 99) before File3 (91). After ADDKW File4 triunghi, File4 (84) joins the ranking. After DELKW File3 triunghi, File3 stays in the system through trigonometrie but no longer appears under triunghi in PRINT.
