#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define BOOL_FMT(exp) exp ? "true" : "false"

int max(int a, int b) {
  return a > b ? a : b;
}

struct Node{
  int val;
  int height;
  struct Node *left, *right;
};

typedef struct Node Node;

#define NODE_CAP 1024
Node node_pool[NODE_CAP] = {0};
int node_used[NODE_CAP] = {0};
size_t node_size = 0;

Node *node_alloc() {
  if(node_size>=NODE_CAP) {
    fprintf(stderr, "ERROR: Node-Overflow\n");
    exit(1);
  }
  for(int i=0;i<NODE_CAP;i++) {
    if(!node_used[i]) {
      node_used[i] = 1;
      Node *node = &node_pool[i];
      node_size++;
      memset(node, 0, sizeof(Node));
      node->height = 1;
      return node;
    }
  }
  return NULL;
}

int node_height(Node *node) {
  if(!node) return 0;
  return node->height;
}

void node_free(Node *node) {
  if(node==NULL) {
    fprintf(stderr, "ERROR: Nullpointer-Exception: node_free\n");
    exit(1);        
  }
  if(node_size<=0) {
    fprintf(stderr, "ERROR: Node-Underflow\n");
    exit(1);    
  }
  
  int index = (int) (node - &node_pool[0]);
  node_used[index] = 0;
  node_size--;
}

int tree_diff(Node *node) {
  if(!node) return 0;
  int diff = node_height(node->left) - node_height(node->right);
  return diff;
}

void tree_rotate_right(Node **node) {
  if(node==NULL || (*node)==NULL || (*node)->left==NULL) {
    fprintf(stderr, "ERROR: Nullpointer-Exception: tree_rotate_right\n");
    exit(1);
  }
  Node *left = (*node)->left;
  Node *left_right = left->right;

  left->right = *node;
  (*node)->left = left_right;

  (*node)->height = 1 + max(node_height((*node)->left),
			    node_height((*node)->right));
  left->height    = 1 + max(node_height(left->left), node_height(left->right));

  *node = left;
}

void tree_rotate_left(Node **node) {
  if(node==NULL || (*node)==NULL || (*node)->right==NULL) {
    fprintf(stderr, "ERROR: Nullpointer-Exception: tree_rotate_left\n");
    exit(1);
  }
  Node *right = (*node)->right;
  Node *right_left = right->left;

  right->left = *node;
  (*node)->right = right_left;

  (*node)->height = 1 + max(node_height((*node)->left),
			    node_height((*node)->right));
  right->height = 1 + max(node_height(right->left),
			  node_height(right->right));

  *node = right;
}


void tree_insert(Node **node, int n) {
  if(!(*node)) {
    Node *tmp = node_alloc();
    tmp->val = n;
    *node = tmp;
    return;
  }
    
  if((*node)->val < n) tree_insert(&(*node)->right, n);
  else if(n < (*node)->val) tree_insert(&(*node)->left, n);
  else return;

  int hr = node_height((*node)->right);
  int hl = node_height((*node)->left);

  (*node)->height = 1 + (hl > hr ? hl : hr);

  int diff = hl - hr;
  if(diff > 1 && n < (*node)->left->val) {
    tree_rotate_right(node);
  }
  else if (diff < -1 && n > (*node)->right->val) {
    tree_rotate_left(node);
  }
  else if (diff > 1 && n > (*node)->left->val) {
    tree_rotate_left(&(*node)->left);
    tree_rotate_right(node);
  }
  else if (diff < -1 && n < (*node)->right->val) {
    tree_rotate_right(&(*node)->right);
    tree_rotate_left(node);    
  }
}

typedef enum{
  TREE_INORDER,
  TREE_PREORDER,
  TREE_POSTORDER
}Tree_Order;

void tree_traverse(Node *node, void (*f)(Node *), Tree_Order order) {
  if(!node) return;
  switch(order) {
  case TREE_INORDER:
    if(node->left) tree_traverse(node->left, f, order);
    f(node);
    if(node->right) tree_traverse(node->right, f, order);
    break;
  case TREE_PREORDER:
    f(node);
    if(node->left) tree_traverse(node->left, f, order);
    if(node->right) tree_traverse(node->right, f, order);
    break;
  case TREE_POSTORDER:    
    if(node->left) tree_traverse(node->left, f, order);
    if(node->right) tree_traverse(node->right, f, order);
    f(node);
    break;
  default:
    break;
  }
}

Node **tree_contains(Node **node, int n) {
  Node **tmp = node;
  while(*tmp) {
    if((*tmp)->val==n) return tmp;
    else if((*tmp)->val < n) tmp=&(*tmp)->right;
    else if(n < (*tmp)->val) tmp=&(*tmp)->left;    
  }
  return NULL;
}

int tree_delete(Node **node, int n) {
  if(!node || !(*node)) {
    return 0;
  }

  if((*node)->val < n) {
    tree_delete(&(*node)->right, n);
  }
  else if(n < (*node)->val) {
    tree_delete(&(*node)->left, n);
  }
  else {
    Node *old = *node;
    Node *tmp = NULL;
    if(old->left) {
      Node *before = old->left;
      tmp = old->left;
      int f = 1;
      while(tmp->right) {
	tmp = tmp->right;
	if(f) f = 0;
	else before = before->right;
      }
      if(tmp!=old->left) tmp->left = old->left;
      tmp->right = old->right;
      if(tmp!=before) before->right = NULL;      
    }
    else if(old->right) {
      tmp = old->right;
    }
    *node = tmp;
    node_free(old);
  }

  if(!node || !(*node)) return 1;

  int hr = node_height((*node)->right);
  int hl = node_height((*node)->left);

  (*node)->height = 1 + (hl > hr ? hl : hr);

  int diff = hl - hr;

  if (diff > 1 && tree_diff((*node)->left) >= 0) {
    tree_rotate_right(node);
  }
  else if (diff > 1 && tree_diff((*node)->left) < 0) {
    tree_rotate_left(&(*node)->left);
    tree_rotate_right(node);
  }
  else if (diff < -1 && tree_diff((*node)->right) <= 0) {
    tree_rotate_left(node);
  }
  else if (diff < -1 && tree_diff((*node)->right) > 0) {
    tree_rotate_right(&(*node)->right);
    tree_rotate_left(node);
  }
  
  return 1;
}

int tree_count(Node *node) {
  if(!node) return 0;
  else return tree_count(node->left) + 1 + tree_count(node->right);
}

int tree_in_order(Node *node) {
  if(!node) return 1;
  if(node->left) {
    if(!tree_in_order(node->left)) return 0;
    if(!(node->left->val<node->val)) return 0;
  }
  if(node->right) {
    if(!tree_in_order(node->right)) return 0;
    if(!(node->right->val>node->val)) return 0;
  }
  return 1;
}

void tree_free(Node **node) {
  if(!node || !(*node)) return;
  tree_free(&(*node)->left);
  (*node)->left = NULL;
  node_free((*node));
  tree_free(&(*node)->right);
  (*node)->right = NULL;
  *node = NULL;
}

#define LOG_DIF 1

void tree_to_graph_write(Node *node, FILE *f) {
  if(!node) return;
  int k = 0;
  if(node->left) {
    tree_to_graph_write(node->left, f);
    fprintf(f, "\t%d -> %d[label=\"l\"];\n", node->val, node->left->val);
    k++;
  }
  if(node->right) {
    fprintf(f, "\t%d -> %d[label=\"r\"];\n", node->val, node->right->val);
    tree_to_graph_write(node->right, f);
    k++;
  }
#if LOG_DIF
  fprintf(f, "\t%d[xlabel=\"df %d\"]\n", node->val, tree_diff(node));
#else
  if(k==0) fprintf(f, "\t%d\n", node->val);
#endif  
}

int tree_to_graph(Node *node, const char *file_name) {
  FILE *f = fopen(file_name, "wb");
  if(!f) {
    return -1;
  }

  fprintf(f, "digraph {\n");
  tree_to_graph_write(node, f);
  fprintf(f, "}\n");

  fclose(f);
  return 0;
}

void print(Node *node) {
  printf("%d\n", node->val);
}

#define TREE_SAFE_INSERT(tree, x) do { \
    tree_insert(&tree, x); \
    assert(tree_in_order(tree)); \
    assert(tree_count(tree) == (int) node_size); \
  }while(0)

#define TREE_SAFE_DELETE(tree, x) do { \
    assert(tree_delete(&tree, x)); \
    assert(tree_in_order(tree)); \
    assert(tree_count(tree) == (int) node_size); \
  }while(0)

void tree_test_arr(int *xs, int xs_size) {
  Node *root = NULL; 
  //FILL
  for(int i=0;i<xs_size;i++)
    TREE_SAFE_INSERT(root, xs[i]);

  assert(tree_contains(&root, xs[0]));

  //DELETE REVERSE-ORDER
  for(int i=xs_size-1;i>=0;i--) 
    TREE_SAFE_DELETE(root, xs[i]);

  //FILL
  for(int i=0;i<xs_size;i++)
    TREE_SAFE_INSERT(root, xs[i]);

  assert(tree_contains(&root, xs[xs_size-1]));

  //DELETE IN-ORDER
  for(int i=0;i<xs_size;i++)
    TREE_SAFE_DELETE(root, xs[i]);

  assert(root==NULL);
  assert(node_size==0);
}

void tree_test_range(int bot, int top) {
  Node *root = NULL; 
  //FILL
  for(int i=bot;i<top;i++)
    TREE_SAFE_INSERT(root, i);

  assert(tree_contains(&root, bot));

  //DELETE REVERSE-ORDER
  for(int i=top-1;i>=bot;i--) 
    TREE_SAFE_DELETE(root, i);

  //FILL
  for(int i=bot;i<top;i++)
    TREE_SAFE_INSERT(root, i);

  assert(tree_contains(&root, top-1));

  //DELETE IN-ORDER
  for(int i=bot;i<top;i++)
    TREE_SAFE_DELETE(root, i);

  assert(root==NULL);
  assert(node_size==0);
}

#define ARR_SIZE(xs) (int) (sizeof(xs)/sizeof(xs[0]))

int xs[]  = {25, 36, 30, 40, 26, 32, 38, 48, 5, 12, 50};

int main() {
  return 0;
}
