#include <stdio.h>
#include <stdlib.h>

struct binaryTree {
	int data;
	struct binaryTree *right, *left;
};

typedef struct binaryTree node;

void insertTree(node **tree, int val) {
	node *temp = NULL;
	if(!(*tree)) {
		// temp = (node *)malloc(sizeof(node));
		temp = malloc(sizeof(node));
		temp->left = temp->right = NULL;
		temp->data = val;
		*tree = temp;
		return;
	}
	if(val < (*tree)->data) {
		insertTree(&(*tree)->left, val);
	}
	else if(val > (*tree)->data) {
		insertTree(&(*tree)->right, val);
	}
}

void printTree(node *tree) {
	if(tree) {
		printTree(tree->left);
		printf("%d ", tree->data);
		printTree(tree->right);
	}
}

void freeTree(node *tree) {
	if(tree) {
		freeTree(tree->left);	
		freeTree(tree->right);
		free(tree);
	}
}

int main() {
	node *root;
	root = NULL;

	insertTree(&root, 5);
	insertTree(&root, 2);
	insertTree(&root, 7);
	

	printTree(root);
	printf("\n");
	
	freeTree(root);
	return 0;
}