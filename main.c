#include <stdio.h>

#define AVL_TREE_IMPLEMENTATION
#include "avl_tree.h"

#define BOOL_FMT(exp) exp ? "true" : "false"

typedef struct{
  int x, y;
}Point;

int cmp(const void *a, const void *b) {
  return (*(Point*)a).x - (*(Point*)b).x;
}

FILE *f;

void tree_to_graph_write(const void *_node) {
  Node *node = (Node*) _node;
  int k = 0;
  Point from = NODE_VAL(node, Point);
  if(node->left) {    
    k++;
    Point left = NODE_VAL(node->left, Point);
    fprintf(f, "\t%dB%d -> %dB%d[label=\"l\"];\n", from.x, from.y, left.x, left.y);
  }
  if(node->right) {
    Point right = NODE_VAL(node->right, Point);
    fprintf(f, "\t%dB%d -> %dB%d[label=\"r\"];\n", from.x, from.y, right.x, right.y);
    k++;
  }
  if(k==0) fprintf(f, "\t%dB%d\n", from.x, from.y);
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
  for(int i=0;i<30;i++) {
    p.x = i;
    p.y = 0;
    tree_insert(tree, &p);
  }
  
  tree_to_graph(tree, "graph.dot");
    
  tree_free(tree);
  return 0;
}
