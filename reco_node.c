#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"

#include "reco_node.h"


float *make_reco_space(int count)
{
	float *reco_space;
	float *p;
	
	int i;
	
	reco_space = malloc(count * count * sizeof(float));
	if (!reco_space) {
		print_error("reco space alloc error!");
	}
	
	p = reco_space;
	
	for (i = 0; i < count * count; i++) {
		*p++ = 0.0;
	}
	
	return reco_space;
	
}


struct rb_root *make_reco_tree(struct rb_root *root)
{
	struct rb_root *reco_arr;
	
	// 마지막 널 처리	
	reco_arr = malloc(sizeof(struct rb_root) * (root->count + 1));
	if (!reco_arr) {
		print_info("alloc error!");
		return NULL;
	}
	
	memset(reco_arr, 0, sizeof(struct rb_root) * (root->count + 1));
	
	return reco_arr;
}


int reco_insert(struct rb_root *root, long simi_and_key, int limit) 
{
		
	struct rb_node **new, *parent;
	struct reco_node *this, *data;
	
	new = &(root->rb_node);
	parent = NULL;
	
//	tree_lock(&(root->locked));
	
	while (*new) {
		
		this = container_of(*new, struct reco_node, node);

		parent = *new;
		
		if (simi_and_key < this->key) {
			new = &((*new)->rb_left);
		} else if (simi_and_key > this->key) {
			new = &((*new)->rb_right);
		} else if (simi_and_key == this->key) {
//			tree_unlock(&(root->locked));
			return TRUE;
		} 
	}
	
	data = malloc(sizeof(struct reco_node));
	if (!data) {
		print_error("alloc error");
//		tree_unlock(&(root->locked));
		return FALSE;
	}

	data->key 	= simi_and_key;
	root->count++;
	
//	if (root->count > 100) 
//		print_info("count = %ld", root->count);

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);
	
	if (root->count > limit) {


		struct rb_node *remove;

		remove = rb_last(root);
		
		root->count--;
		rb_erase(remove, root);
		free(remove);
//		print_info("reco limit!");
	}	
	
//	tree_unlock(&(root->locked));
	return TRUE;
}

void dump_reco(struct rb_root *reco_arr, long count, int *user_conv, int *item_conv) 
{
	int i;
	struct rb_node *p;
	struct reco_node *this;
	double simi;
	long key;
	
	for (i = 0; i < count; i++) {
		p = rb_first(&reco_arr[i]);

		while(p) {
			this = container_of(p, struct reco_node, node);
			
			simi 	= 1.0 - ((double)((this->key & 0xffffffff00000000) >> 32) / 1000000.0);
			key 	= this->key & 0x00000000ffffffff;
			
//			printf("%d,%d,%lf\n", i, (int)key, simi);
			printf("%d,%d,%lf\n", *(user_conv + i), *(item_conv + (int)key), simi);
			p = rb_next(p);
		}
	}	
}


