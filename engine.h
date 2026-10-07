/*IURES Alexandru-Marian - 311CB*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


typedef struct file_content {
	int score;
	char *id;
	int number;
}file;

typedef struct file_list {
	file info;
	struct file_list *next;
	struct file_list *pre;
}file_cell, *files;

typedef struct file_for_tree {
	file *info;
	struct file_for_tree* next;
}file_tree_cell, *files_terminal_node; 

typedef struct node_tree
{
	char letter;
	struct node_tree* left;
	struct node_tree* right;
	files_terminal_node word_files;
	
}node, *tree;

typedef int (*cmp) (file*, file*);

typedef struct heap
{
	int no_element;
	int no_max;
	file **files_word;
	cmp heap_rule;

} heap_files;

tree init_tree();
void read_commands();
tree find_word(tree *t, char *word, int len);
void delete_list(tree *t, files *f, char *id, FILE *out);