#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"

#include "input_file.h"

#include "info_node.h"

static struct rb_root item_info_tree	= RB_ROOT;
static struct rb_root *item_info_root	= &item_info_tree;

typedef struct {
	int fd;
	char *p;
} item_info_space_t;

static item_info_space_t item_info_space;

int item_info_insert(int key, char *val) 
{
	struct rb_node **new, *parent;
	struct info_node *this, *data;
	
	new = &(item_info_root->rb_node);
	
	parent = NULL;
	
	while (*new) {
		
		this = container_of(*new, struct info_node, node);

		parent = *new;
		
		if (key < this->key) {
			new = &((*new)->rb_left);
		} else if (key > this->key) {
			new = &((*new)->rb_right);
		} else if (key == this->key) {
			return TRUE;
		} 
	}

	data = malloc(sizeof(struct info_node));
	if (!data) {
		print_error("alloc error");
		return FALSE;
	}
	
	data->key 	= key;
	data->val 	= val;
	
	item_info_root->count++;
	
	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, item_info_root);

	return TRUE;
}

char *item_info_search(int key) 
{
	struct rb_node **new;
	struct info_node *this;
	
	new = &(item_info_root->rb_node);
	
	while (*new) {
		
		this = container_of(*new, struct info_node, node);

		if (key < this->key) {
			new = &((*new)->rb_left);
		} else if (key > this->key) {
			new = &((*new)->rb_right);
		} else if (key == this->key) {
			return this->val;
		} 
	}

	return NULL;

}


#define NUM_LEN_MAX		255
void load_item_info()
{
	char num_buf[NUM_LEN_MAX];
	char *np, *p = item_info_space.p;
	long count;
	
	int key;
	char *val;

	np = (char *)num_buf;
	
	count = 0;
		
	while(*p) {
		
		if (count == 1) {
			if (*p != '\n') {
				p++;
				continue;
			} 
			
			count = 0;
			p++;
			continue;
		}
		

		if (*p != ',' && *p != '\n') {
			*np++ = *p++;
			continue;
		} 
		
		p++;
		
		// 분리
		*np = 0;
		np = num_buf;			

		key = atol(np);
		val = p;
		
		item_info_insert(key, val);
		count++;
		
	}
	
	print_info("item info load ok!");	
	
	return;
	
}

int get_info_str(int key, char *buf)
{
	char *val, *pbuf;
	
	pbuf = buf;
	
	val = item_info_search(key);
	
	if (!val) return FALSE;
	
	while(*val) {
		if (*val != '\n') {
			*pbuf++ = *val++;
		} else {
			*pbuf = 0;
			break;
		}
	}
	
	return TRUE;

}

void start_info(char *filename)
{
	item_info_space.fd = attach_data(filename, &(item_info_space.p));
	
	print_info("info file %s attach ok %p", filename, item_info_space.p);
	print_info("info file rows = %ld", get_data_rows(item_info_space.p));
	
	load_item_info();
}

void end_info(void)
{
	detach_data(item_info_space.fd, item_info_space.p);
	
}

void test_movie(char *filename)
{
	char movie[512];
	
	start_info(filename);
	
	if (!get_info_str(455, movie)) {
		print_error("find movie 455 failed");
	}
	
	print_info("find movie 455 = %s", movie);
	
	end_info();
	
}

