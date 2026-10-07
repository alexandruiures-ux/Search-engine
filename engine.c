#include "engine.h"

tree init_tree()
{
	tree aux;
	aux = malloc(sizeof(node));
	if (!aux) {
		perror("Could not init tree");
		return NULL;
	}
	//putting the root as a cell with no info
	aux->letter = 0;
	aux->left = NULL;
	aux->right = NULL;
	aux->word_files = NULL;
	return aux;
}

char *read_string(FILE *in)
{
	//reading string with unkown length
	char *read_string;
	int size_string = 2;
	int length = 0;
	read_string = malloc(size_string);
	if (!read_string) {
		perror("Could not allocate command");
		return NULL;
	}
	char character;
	do {
		character = fgetc(in);
	} while (character == ' ' || character == '\n'); // jump over any space or endline remained
	read_string[length] = character;
	length++;
	do {
		character = fgetc(in);
		if (character != '\n' && character != EOF && character != ' ') {
			read_string[length] = character;
			length++;
		}
		if (length >= size_string) {
			size_string *= 2;
			char *aux_string;
			aux_string = realloc(read_string, size_string);
			if (!aux_string) {
				free(read_string);
				return NULL;
			}
			read_string = aux_string;
		}
	} while (character != '\n' && character != EOF && character != ' ');
	read_string[length] = '\0';
	int len = strlen(read_string);
	//makes the string have the size of its length plus 1
	read_string = realloc(read_string, len + 1);
	return read_string;
}

tree create_tree_cell(char c)
{
	tree aux = malloc(sizeof(node));
	if (!aux) {
		perror("could not make tree cell");
		return NULL;
	}
	aux->left = NULL;
	aux->word_files = NULL;
	aux->right = NULL;
	aux->letter = c;
	return aux;
}

void add_word_in_tree(char *word, tree t, file *f)
{
	int len = 0; //the character of the string that will be added
	while(len < strlen(word)) {
		tree aux;
		aux = create_tree_cell(word[len]);
		if(!t->left) {
			//add new layer for tree
			t->left = aux;
			t = t->left;
		} else {
			tree root;
			root = t;
			t = t->left;
			//find the place in the level where to put the new letter
			if (word[len] < t->letter) {
				aux->right = t;
				root->left = aux;
				t = aux;
			} else {
				if(word[len] == t->letter)
					free(aux); //the letter is already in the tree at that level
				else {
					while(word[len] != t->letter) {
				if (!t->right) {
					t->right = aux;
					t = t->right;
					break;
				}
				if (word[len] < t->right->letter) {
					tree aux_insert_alfabetical;
					aux_insert_alfabetical = t->right;
					t->right = aux;
					aux->right = aux_insert_alfabetical;
					t = t->right;
					break;
				}
				if (t->right && word[len] == t->right->letter) {
					//add at the end
					free(aux);
					t = t->right;
					break;
				}
				t = t->right;
				}
				}
			}

		}
		len++;
	}
	//adding the file ref at the last node in the tree
	files_terminal_node aux_file;
	aux_file = malloc(sizeof(file_tree_cell));
	if (!aux_file) {
		perror("Could not make files ref");
		return;
	}
	aux_file->info = f;
	aux_file->next = NULL;
	files_terminal_node aux_insert;
	aux_insert = t->word_files;
	if(!aux_insert) {
		t->word_files = aux_file;
		return;
	}
	if (strcmp(aux_insert->info->id, aux_file->info->id) > 0) {
		aux_file->next = t->word_files;
		t->word_files = aux_file;
		return;
	}
	if (!strcmp(aux_insert->info->id, aux_file->info->id)) {
		free(aux_file);
		return;
	}
	while(aux_insert->next) {
		if (!strcmp(aux_insert->next->info->id, aux_file->info->id)) {
			free(aux_file);
			return;
		}   
		if (strcmp(aux_insert->next->info->id, aux_file->info->id) > 0)
			if (strcmp(aux_insert->info->id, aux_file->info->id) < 0)
				break;
		aux_insert = aux_insert->next;
	}
	if(aux_insert->next) {
		aux_file->next = aux_insert->next;
		aux_insert->next = aux_file;
		return;
	}
	aux_insert->next = aux_file;
}

void add(FILE *in, tree t, files *f, FILE *out)
{
	char *id;
	int number_words, score;
	id = read_string(in);
	fscanf(in, "%d %d", &score, &number_words);
	files aux_alpha = *f, aux_vf = *f;
	//adding the file in the list of files
	if (aux_vf) {
		while(aux_vf->next) {
			if(aux_vf->info.id) {
				if(!strcmp(aux_vf->info.id, id)) {
					//the file already exists
					fprintf(out,"EXISTS\n");
					for (int i = 0; i < number_words; i++) {
						char *word;
						word = read_string(in);
						add_word_in_tree(word, t, &(aux_vf->info));
						free(word);
					}
					free(id);
					return;
				} 
			}
			aux_vf = aux_vf->next;
		}
		if(aux_vf->info.id) {
			if(!strcmp(aux_vf->info.id, id)) {
				fprintf(out,"EXISTS\n");
				for (int i = 0; i < number_words; i++) {
					char *word;
					word = read_string(in);
					free(word);
				}
				free(id);
				return;
			} 
		}
	}
	//create the file
	file aux;
	aux.id = malloc(strlen(id) + 1);
	if (!aux.id) {
		perror("can not make id");
		free(id);
		return;
	}
	aux.score = score;
	aux.number = number_words;
	strcpy(aux.id, id);
	//create the cell of a file in list
	files aux_create_file;
	aux_create_file = malloc(sizeof(file_cell));
	if (!aux_create_file) {
		perror("Could not make new file");
		free(id);
		return;
	}
	aux_create_file->info = aux;
	aux_create_file->next = NULL;
	if (!(*f)) {
		aux_create_file->pre = NULL;
		aux_create_file->next = NULL;
		*f = aux_create_file;
	} else {
		while(aux_alpha->next)
			aux_alpha = aux_alpha->next;
		aux_alpha->next = aux_create_file;
		aux_create_file->pre = aux_alpha;
	}
	fgetc(in);
	for (int i = 0; i < number_words; i++) {
		char *word;
		word = read_string(in);
		add_word_in_tree(word, t, &((aux_create_file)->info));
		free(word);
	}
	fprintf(out,"OK\n");
	free(id);
}

files find_file(files f, char *id)
{
	//finding the file needed in the linked list
	while (f) {
		if (!strcmp(f->info.id, id))
			return f;
		f = f->next;
	}
	return NULL;
}

void remove_files(tree *t, char *id)
{
	//removing a file reference from the terminal nodes in the tree
	if(!(*t))
		return;
	if ((*t)->word_files) {
		files_terminal_node aux = (*t)->word_files;
		files_terminal_node before = NULL;
		while(aux) {
			if(!strcmp(aux->info->id, id))
				break;
			before = aux;
			aux = aux->next;
		}
		if (aux) {
			if (aux == (*t)->word_files) {
				(*t)->word_files = (*t)->word_files->next;
				free(aux);
			}
			else {
				before->next = aux->next;
				free(aux);
			}
		}
	}
	if ((*t)->right)
		remove_files(&((*t)->right), id); 
	remove_files(&((*t)->left), id); 
}

int no_files(tree t)
{
	//verify if a node has no files referenced
	if(!t)
		return 0;
	if (t->word_files)
		return 1;
	return no_files(t->left);
}

void free_tree(tree *t) {
	//free the tree
	if(!(*t))
		return;
	free_tree(&((*t)->left));
	free_tree(&((*t)->right));
	files_terminal_node delete_lists;
	delete_lists = (*t)->word_files;
	while (delete_lists) {
		files_terminal_node aux = delete_lists->next;
		free(delete_lists);
		delete_lists = aux;
	}
	free(*t);
	*t = NULL;
}

void delete_no_file_words(tree *t)
{
	//deleting the letter from a word that has no files anymore linked to it
	if (!(*t))
		return;
	delete_no_file_words(&((*t)->left));
	if ((*t)->right)
		delete_no_file_words(&((*t)->right));
	if(!no_files(*t)) {
		tree to_delete;
		to_delete = *t;
		*t = to_delete->right;
		to_delete->right = NULL;
		free_tree(&to_delete);
	}
}

void delete_list(tree *t, files *f, char *id, FILE *out)
{
	//deleting a file from the list files
	int ok = 1;
	if (!(*f)) {
		fprintf(out,"NOT FOUND\n");
		return;
	}
	if (!strcmp((*f)->info.id, id)) {
		//the file has been found
		remove_files(t, id);
		delete_no_file_words(t);
		//verify if there are no words in the tree if there are not we reinitialise
		if (!(*t)) 
			*t = init_tree();
		if ((*f)->next)
			(*f)->next->pre = NULL;
		files deleting = *f;
		*f = (*f)->next;
		free(deleting->info.id);
		free(deleting);
		fprintf(out,"OK\n");
		return;
	}
	files aux = find_file((*f)->next, id);
	if (!aux) {
		fprintf(out, "NOT FOUND\n");
		return;
	}
	fprintf(out, "OK\n");
	remove_files(t, id);
	delete_no_file_words(t);
	if (!(*t)) 
		*t = init_tree();
	files deleting;
	deleting = aux->pre;
	deleting->next = aux->next;
	if (aux->next)
		deleting->next->pre = deleting;
	free(aux->info.id);
	free(aux);
}

void del(FILE *in, tree *t, files *f, FILE *out)
{
	char *id_file;
	id_file = read_string(in);
	if (!id_file) {
		perror("can not ged id delete file");
		return;
	}
	delete_list(t, f, id_file, out);
	free(id_file);
}

void addkw(FILE *in, tree *t, files *f, FILE *out)
{
	char *id_file;
	id_file = read_string(in);
	if (!id_file) {
		perror("can not ged id delete file");
		return;
	}
	files which_file = find_file(*f, id_file);
	char *word_to_add;
	word_to_add = read_string(in);
	if (!which_file) {
		//there is no file like the one in command
		free(word_to_add);
		free(id_file);
		fprintf(out, "NOT FOUND\n");
		return;
	}
	fprintf(out, "OK\n");
	tree duplicate = find_word(t, word_to_add, strlen(word_to_add));
	if (duplicate) {
		files_terminal_node aux_dup = duplicate->word_files;
		while (aux_dup) {
			if (!strcmp(aux_dup->info->id, id_file))
				break;
			aux_dup = aux_dup->next;
		}
		if (!aux_dup)
			which_file->info.number++; // add the word to the file
	} else {
		add_word_in_tree(word_to_add, *t, &(which_file->info));
		which_file->info.number++;
	}
	free(word_to_add);
	free(id_file);
}

tree find_word(tree *t, char *word, int len)
{
	//returning the terminal node of a word in a tree
	if (!(*t))
		return NULL;
	tree aux = *t;
	int i = 0;
	while (i < len) {
		while(aux && aux->letter != word[i])
			aux = aux->right;
		if(!aux)
			return NULL;
		if (i == len - 1)
			return aux;
		aux = aux->left;
		i++;
	}
}

void delkw(FILE *in, tree *t, files *f, FILE *out)
{
	char *id_file = read_string(in);
	char *word_del = read_string(in);
	files aux = find_file(*f, id_file);
	if (!aux) {
		//there is no file with this name
		free(id_file);
		free(word_del);
		fprintf(out,"NOT FOUND\n");
		return;
	}
	fprintf(out,"OK\n");
	tree terminal = find_word((&(*t)->left), word_del, strlen(word_del));
	if (!terminal) {
		//there is no word to delete
		free(id_file);
		free(word_del);
		return;
	}
	files_terminal_node to_del = terminal->word_files;
	files_terminal_node before_del = NULL;
	while(to_del) {
		//searching for the file with the word to delete
		if(!strcmp(to_del->info->id, id_file))
			break;
		before_del = to_del;
		to_del = to_del->next;
	}
	if (!to_del) {
		//there is nothing to delete
		free(id_file);
		free(word_del);
		return;
	}
	aux->info.number--; // a word has been removed
	if (!aux->info.number) {
		free(aux->info.id);
		if (!aux->pre) {
			*f = aux->next;
		} else {
			if(!aux->next)
				aux->pre->next = NULL;
			else {
				aux->pre->next = aux->next;
				aux->next->pre = aux->pre;
			}
		}
		free(aux);
	}
	if (to_del == terminal->word_files) {
		//the file is the first one in the list
		terminal->word_files = terminal->word_files->next;
		free(to_del);
	} else {
		//deleting the word
		before_del->next = to_del->next;
		free(to_del);
	}
	delete_no_file_words(t);
	//verify if there are any words remaining in the tree
	if (!(*t))
		*t = init_tree();
	free(id_file);
	free(word_del);
}

int heap_order(file *f1, file *f2)
{
	if(f1->score > f2->score)
		return 1;
	if(f1->score < f2->score)
		return -1;
	if(strcmp(f1->id, f2->id) < 0)
		return 1;
	return -1;
}

heap_files* init_heap(int max_elem, cmp rule)
{
	heap_files *h = (heap_files *) malloc(sizeof(struct heap));
	if (!h) {
		perror("can not make heap");
		return NULL;
	}
	h->files_word = (file **) malloc(max_elem * sizeof(file *));
	if (!h->files_word) {
		free(h);
		return NULL;
	}
	h->no_max = max_elem;
	h->no_element = 0;
	h->heap_rule = rule;
	return h;
}

file* extr_heap(heap_files *h)
{
	h->no_element--;
	h->no_max--;
	file *root = h->files_word[0];
	h->files_word[0] = h->files_word[h->no_element];
	int poz = 0;
	file* move_heap;
	while (2 * poz + 1 < h->no_element) {
		move_heap = h->files_word[2 * poz + 1];
		int new_pos;
		new_pos = poz * 2 + 1;
		if (2 * poz + 2 < h->no_element) {
			if (heap_order(move_heap, h->files_word[2 * poz + 2 ]) == -1) {
				move_heap = h->files_word[2 * poz + 2 ];
				new_pos = 2 * poz + 2;
			}
		}
		if (heap_order(h->files_word[poz], move_heap) == 1) 
			break;
		else {
			file* aux_switch;
			aux_switch = h->files_word[poz];
			h->files_word[poz] = h->files_word[new_pos];
			h->files_word[new_pos] = aux_switch;
			poz = new_pos;
		}
	}
	return root;
}

void insert_heap (heap_files *h, file *f)
{
	int poz = h->no_element;
	h->no_element++;
	if (h->no_element > h->no_max) {
		h->no_max = h->no_element;
		int space_realoc = h->no_max * sizeof(file *);
		h->files_word =(file **) realloc(h->files_word, space_realoc);
		if (!h->files_word) {
			perror("could not realloc heap");
			return;
		}
	}
	h->files_word[poz] = f;
	while (poz) {
		int pos_parent = (poz - 1) / 2;
		if (heap_order(h->files_word[pos_parent], h->files_word[poz]) == -1) {
			file *aux;
			aux = h->files_word[poz];
			h->files_word[poz] = h->files_word[pos_parent];
			h->files_word[pos_parent] = aux;
			poz = pos_parent;
		} else {
			break;
		}
	}

}

void topk(FILE *in, tree *t, FILE *out)
{
	heap_files *h = NULL;
	int no_files;
	h = init_heap(100, heap_order); //make the heap
	char *word = read_string(in);
	int len = strlen(word);
	int k;
	fscanf(in, " %d", &k);
	tree finish_node = find_word(&((*t)->left), word, len); //finding the word in heap
	if (!finish_node || !finish_node->word_files) {
		//the word is not in the tree
		fprintf(out, "EMPTY\n");
		free(word);
		free(h->files_word);
		free(h);
		return;
	}
	files_terminal_node aux = finish_node->word_files;
	int no_print = 0; // the number of files that will be printed
	while(aux) {
		no_print++;
		aux = aux->next;
	}
	if (no_print > k)
		no_print = k; // there are more than k files in the heap
	fprintf(out,"%d ", no_print);
	aux = finish_node->word_files;
	while(aux) {
		insert_heap(h, aux->info);
		aux = aux->next;
	}
	while(k && h->no_element) {
		k--;
		file* print_heap = extr_heap(h);
		fprintf(out,"%s ", print_heap->id);
	}
	fprintf(out,"\n");
	free(h->files_word);
	free(h);
	free(word);
}

void find(FILE *in, tree *t, FILE *out)
{
	char *word_to_find = read_string(in);
	int len = strlen(word_to_find);
	//finding the terminal node in the tree
	tree finding = find_word(&((*t)->left), word_to_find, len);
	if (!finding) {
		fprintf(out, "EMPTY\n");
		free(word_to_find);
		return;
	}
	int ct = 0; //how many files are linked to the word
	files_terminal_node aux_print;
	aux_print = finding->word_files;
	while(aux_print) {
		ct++; // calculating the number
		aux_print = aux_print->next;
	}
	fprintf(out, "%d ", ct);
	aux_print = finding->word_files;
	//printing the files
	while(aux_print) {
		fprintf(out, "%s ", aux_print->info->id);
		aux_print = aux_print->next;
	}
	fprintf(out, "\n");
	free(word_to_find);
}

void print(FILE *out, tree t, files f, int len, int *len_mal, char **print_word)
{
	//see if the length of the word is larger then expexted
	if (len >= (*len_mal) - 1) {
		(*len_mal) *= 2;
		char *aux = realloc(*print_word, *len_mal);
		if (!aux) {
			perror("can not make word");
			return;
		}
		*print_word = aux;
	}
	(*print_word)[len] = t->letter; //making the word from the tree
	if (t->word_files) {
		int no_files = 0;
		//printing the files for each word
		files_terminal_node aux1;
		aux1 = t->word_files;
		while(aux1) {
			no_files++;
			aux1 = aux1->next;
		}
		(*print_word)[len + 1] = '\0';
		fprintf(out, "%s %d ", *print_word, no_files);
		aux1 = t->word_files;
		while(aux1) {
			fprintf(out, "%s ", aux1->info->id);
			aux1 = aux1->next;
		}
		fprintf(out, "\n");
	}
	//going recursive through the tree
	if (t->left)
		print(out, t->left, f, len + 1, len_mal, print_word);
	(*print_word)[len] = t->letter;
	if (t->right)
		print(out, t->right, f, len, len_mal, print_word);
}

void insert_in_list(files_terminal_node l, files_terminal_node to_insert)
{
	//inserting the file reference in the list from the terminal word
	//inserting it in lexicographic order
	while(l->next) {
		if (!strcmp(l->info->id, to_insert->info->id))
			return;
		if (strcmp(l->next->info->id, to_insert->info->id) > 0)
			break;
		l = l->next;
	}
	if (!strcmp(l->info->id, to_insert->info->id))
		return;
	files_terminal_node aux_to_insert;
	aux_to_insert = malloc(sizeof(file_tree_cell));
	if (!aux_to_insert) {
		perror("error prefix");
		return;
	}
	aux_to_insert->info = to_insert->info;
	aux_to_insert->next = l->next;
	l->next = aux_to_insert;
}

void print_files_prefix(files_terminal_node *l, tree *t, int root)
{
	if (!(*t))
		return;
	if ((*t)->word_files) {
		files_terminal_node aux;
		aux = (*t)->word_files;
		if (!(*l)) {
			files_terminal_node insert_first; 
			//the first file for the list of printed files with words with prefix
			insert_first = malloc(sizeof(file_tree_cell));
			if (!insert_first) {
				perror("error insert first element prefix");
				return;
			}
			insert_first->info = aux->info;
			insert_first->next = NULL;
			*l = insert_first;
			aux = aux->next;
		} else {
			//insert thw file reference in the corect place for lexicographic order
			if (strcmp(aux->info->id, (*l)->info->id) < 0) {
				files_terminal_node aux_to_insert;
				aux_to_insert = malloc(sizeof(file_tree_cell));
				if (!aux_to_insert) {
					perror("error prefix");
					return;
				}
				aux_to_insert->info = aux->info;
				aux_to_insert->next = *l;
				*l = aux_to_insert;
			}
		}
		while (aux) {
			insert_in_list(*l, aux);
			aux = aux->next;
		}
	}
	print_files_prefix(l, &((*t)->left), 1);
	if(root)
		print_files_prefix(l, &((*t)->right), 1);
}

void prefix(FILE *in, tree *t, FILE *out)
{
	char *prefix = read_string(in);
	tree finding_prefix = find_word(&((*t)->left), prefix, strlen(prefix));
	if (!finding_prefix) {
		fprintf(out,"EMPTY\n");
		free(finding_prefix);
		free(prefix);
		return;
	}
	files_terminal_node list_print = NULL; //making a list of the files to be listed
	print_files_prefix(&list_print, &finding_prefix, 0);
	int ct = 0;
	files_terminal_node aux_ct;
	aux_ct = list_print;
	while(aux_ct) {
		ct++;
		aux_ct = aux_ct->next;
	}
	fprintf(out,"%d ", ct);
	while(list_print) {
		fprintf(out,"%s ", list_print->info->id);
		aux_ct = list_print;
		list_print = list_print->next;
		free(aux_ct);
	}
	fprintf(out,"\n");
	free(prefix);
}

void command_to_do(char *command, tree *t, files *f, FILE *in, FILE *out)
{
	if(!strcmp(command,"ADD")) {
		add(in, *t, f, out);
		return;
	}
	if(!strcmp(command,"DEL")) {
		del(in, t, f, out);
		return;
	}
	if(!strcmp(command,"ADDKW")) {
		addkw(in, t, f, out);
		return;
	}
	if(!strcmp(command,"DELKW")) {
		delkw(in, t, f, out);
		return;
	}
	if(!strcmp(command,"FIND")) {
		find(in, t, out);
		return;
	}
	if(!strcmp(command,"TOPK")) {
		topk(in, t, out);
		return;
	}
	if(!strcmp(command,"PRINT")) {
		
		if(!(*t)->left) {
			fprintf(out,"EMPTY\n");
			return;
		}
		int len = 0;
		int len_malloc = 2;
		char *printword;
		printword = malloc(len_malloc);
		if (!printword) {
			perror("Could not print words");
			return;
		}
		if((*t)->left)
		print(out, (*t)->left, *f, len, &len_malloc, &printword);
		free(printword);
		return;
	}
	if(!strcmp(command,"PREFIX")) {
		prefix(in, t, out);
		return;
	}

}


void read_commands()
{
	FILE *in, *out;
	in = fopen("indexare.in", "rt");
	out = fopen("indexare.out", "wt");
	tree words;
	files list_files;
	list_files = NULL;
	words = init_tree();
	char *command;
	command = malloc(7); //7 is the max length of a command
	int no_commands;
	fscanf(in, "%d", &no_commands);
	for (int i = 0; i < no_commands; i++) {
		fscanf(in,"%s",command);
		command_to_do(command, &words, &list_files, in, out);
	}
	while (list_files) {
		files aux_del;
		aux_del = list_files;
		list_files = list_files->next;
		free(aux_del->info.id);
		free(aux_del);
	}
	free(command);
	free_tree(&words);
	fclose(in);
	fclose(out);
}