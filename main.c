#include <stdio.h>

#define AVL_TREE_IMPLEMENTATION
#include "avl_tree.h"

#define BOOL_FMT(exp) exp ? "true" : "false"

typedef struct{
  int x, y;
}Point;

int cmp(const void *a, const void *b) {
  Point *_a = (Point *)a;
  Point *_b = (Point *)b;

  int dx = _a->x - _b->x;
  int dy = _a->y - _b->y;
  
  if(dx) return dx;
  return dy;
}

FILE *f;

void tree_to_graph_write(const void *_node) {
  Node *node = (Node*) _node;
  int k = 0;
  Point from = NODE_VAL(node, Point);
  if(node->left) {    
    k++;
    Point left = NODE_VAL(node->left, Point);
    fprintf(f, "\t\"(%d, %d)\" -> \"(%d, %d)\"[label=\"l\"];\n", from.x, from.y, left.x, left.y);
  }
  if(node->right) {
    Point right = NODE_VAL(node->right, Point);
    fprintf(f, "\t\"(%d, %d)\" -> \"(%d, %d)\"[label=\"r\"];\n", from.x, from.y, right.x, right.y);
    k++;
  }
  if(k==0) fprintf(f, "\t\"(%d, %d)\"\n", from.x, from.y);
}

int tree_to_graph(Tree *tree, const char *file_name) {
  f = fopen(file_name, "wb");
  if(!f) {
    return -1;
  }

  fprintf(f, "digraph {\n");
  tree_traverse(tree, tree_to_graph_write, TREE_INORDER);
  fprintf(f, "}\n");

  fclose(f);
  return 0;
}

int main() {
  Tree *tree = tree_init(sizeof(Point), cmp);

  Point p;
  for(int i=0;i<5;i++) {
    for(int j=0;j<5;j++) {
      p.x = i;
      p.y = j;
      tree_insert(tree, &p);
    }
  }
  
  tree_to_graph(tree, "graph.dot");
    
  tree_free(tree);
  return 0;
}
