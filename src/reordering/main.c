#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "time.h"

#define MAX_LINE_LENGTH 1000

void report_error(char *s){
    fprintf(stderr,"%s\n",s);
    exit(-1);
}

/******** QUEUE functions - begin *********/

typedef struct queue{
    int size;
    int *elts;
    int begin;
    int end;
} queue;

queue *empty_queue(int size){
    queue *q;
    if( (q=(queue *)malloc(sizeof(queue))) == NULL )
        report_error("empty_queue: Pb with malloc");
    q->size = size+1;
    if( (q->elts = (int *)malloc((size+1)*sizeof(int))) == NULL )
        report_error("empty_queue: Pb with malloc");
    q->begin = q->end = 0;
    return(q);
}

void free_queue(queue *q){
    if ((q==NULL) || (q->elts==NULL))
        report_error("free_queue: NULL pointer");
    free(q->elts);
    free(q);
}

int is_empty_queue(queue *q){
    return(q->begin == q->end);
}

int is_full_queue(queue *q){
    return(q->begin == (q->end+1)%q->size);
}

void queue_add(queue *q, int e){
    if (is_full_queue(q))
        report_error("queue_add: queue is full");
    q->elts[q->end] = e;
    q->end++;
    q->end %= q->size;
}

int queue_get(queue *q){
    int r;
    if (is_empty_queue(q))
        report_error("queue_get: queue is empty");
    r = q->elts[q->begin];
    q->begin++;
    q->begin %= q->size;
    return(r);
}

/******** QUEUE functions - end *********/

/******** GRAPH functions - begin *********/

typedef struct graph{
    int n;
    int m;
    int **links;
    int *degrees;
    int *capacities;
} graph;

graph *graph_from_file(FILE *f){
    char line[MAX_LINE_LENGTH];
    int i, u, v;
    graph *g;

    if( (g=(graph *)malloc(sizeof(graph))) == NULL )
        report_error("graph_from_file: malloc() error 1");

    /* read n */
    if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
        report_error("graph_from_file: read error (fgets) 1");
    if( sscanf(line, "%d\n", &(g->n)) != 1 )
        report_error("graph_from_file: read error (sscanf) 2");

    /* read the degree sequence */
    if( (g->capacities=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("graph_from_file: malloc() error 2");
    if( (g->degrees=(int *)calloc(g->n,sizeof(int))) == NULL )
        report_error("graph_from_file: calloc() error");
    for(i=0;i<g->n;i++){
        if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
            report_error("graph_from_file; read error (fgets) 2");
        if( sscanf(line, "%d %d\n", &v, &(g->capacities[i])) != 2 )
            report_error("graph_from_file; read error (sscanf) 2");
        if( v != i ){
            fprintf(stderr,"Line just read : %s\n i = %d; v = %d\n",line,i,v);
            report_error("graph_from_file: error while reading degrees");
        }
    }

    /* compute the number of links */
    g->m=0;
    for(i=0;i<g->n;i++)
        g->m += g->capacities[i];
    g->m /= 2;

    /* create contiguous space for links */
    if (g->n==0){
        g->links = NULL; g->degrees = NULL; g->capacities = NULL;
    }
    else {
        if( (g->links=(int **)malloc(g->n*sizeof(int*))) == NULL )
            report_error("graph_from_file: malloc() error 3");
        if( (g->links[0]=(int *)malloc(2*g->m*sizeof(int))) == NULL )
            report_error("graph_from_file: malloc() error 4");
        for(i=1;i<g->n;i++)
            g->links[i] = g->links[i-1] + g->capacities[i-1];
    }

    /* read the links */
    for(i=0;i<g->m;i++) {
        if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
            report_error("graph_from_file; read error (fgets) 3");
        if( sscanf(line, "%d %d\n", &u, &v) != 2 ){
            fprintf(stderr,"Attempt to scan link #%d failed. Line read:%s\n", i, line);
            report_error("graph_from_file; read error (sscanf) 3");
        }
        if ( (u>=g->n) || (v>=g->n) || (u<0) || (v<0) ) {
            fprintf(stderr,"Line just read: %s",line);
            report_error("graph_from_file: bad node number");
        }
        if ( (g->degrees[u]>=g->capacities[u]) ||
                (g->degrees[v]>=g->capacities[v]) ){
            fprintf(stderr, "reading link %s\n", line);
            report_error("graph_from_file: too many links for a node");
        }
        g->links[u][g->degrees[u]] = v;
        g->degrees[u]++;
        g->links[v][g->degrees[v]] = u;
        g->degrees[v]++;
    }
    for(i=0;i<g->n;i++)
        if (g->degrees[i]!=g->capacities[i])
            report_error("graph_from_file: capacities <> degrees");
    if( fgets(line,MAX_LINE_LENGTH,f) != NULL )
        report_error("graph_from_file; too many lines");

    return(g);
}



int *bfs_tree(graph *g, int v){
    int u, i;
    int *tree;
    queue *q;
    int* res_bfs = NULL;
    int j = 0;
    q = empty_queue(g->n);
    if( (tree=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("bfs_tree: malloc() error");
    if( (res_bfs=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("bfs_tree: malloc() error");
     for (i=0;i<g->n;i++)
        tree[i] = -1;
    queue_add(q,v);
    tree[v] = v;
    while (!is_empty_queue(q)) {
        v = queue_get(q);
        /*
         *  Return the result of the bfs instead of tree
         */
        res_bfs[j] = v;
        j++;
        for (i=0;i<g->degrees[v];i++) {
            u = g->links[v][i];
            if (tree[u]==-1){
                queue_add(q,u);
                tree[u] = v;
            }
        }
    }
    free_queue(q);
    return(res_bfs);
}

void renumbering(graph *g, int *perm){
    int *tmpp, **tmppp;
    int i, j;

    for (i=g->n-1;i>=0;i--)
        for (j=g->degrees[i]-1;j>=0;j--)
            g->links[i][j] = perm[g->links[i][j]];

    if( (tmpp=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("renumbering: malloc() error");
    if( (tmppp=(int **)malloc(g->n*sizeof(int *))) == NULL )
        report_error("renumbering: malloc() error");

    memcpy(tmppp,g->links,g->n*sizeof(int *));
    for (i=g->n-1;i>=0;i--)
        g->links[perm[i]] = tmppp[i];

    memcpy(tmpp,g->degrees,g->n*sizeof(int));
    for (i=g->n-1;i>=0;i--)
        g->degrees[perm[i]] = tmpp[i];

    memcpy(tmpp,g->capacities,g->n*sizeof(int));
    for (i=g->n-1;i>=0;i--)
        g->capacities[perm[i]] = tmpp[i];

    free(tmpp);
    free(tmppp);
}

void graph_to_file(graph* g, FILE *f){
    /* write number of nodes */
    fprintf(f, "%d\n", g->n);

    /* write degree of each node */
    for(int i = 0; i < g->n; i++) {
        fprintf(f, "%d %d\n", i, g->capacities[i]);
    }

    /* write every links of every nodes */
    for(int i = 0; i < g->n; i++) {
        for(int j =0; j < g->degrees[i]; j++) {
            fprintf(f, "%d %d\n", i, g->links[i][j]);
        }
    }
}

/******** GRAPH functions - end *********/

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("Please specified a graph\n");
        return 0;
    }

    FILE* file = fopen(argv[1], "r");
    graph* g = graph_from_file(file);
    int bfs_start = rand() % (g->n);
    int* bfs_result = bfs_tree(g, bfs_start);
    /*for (int i = 0; i < g->n; i++) {
        printf("%d\n", bfs_result[i]);
    }*/
    renumbering(g, bfs_result);
    //bfs_result = bfs_tree(g, 0);
    
    //printf("\n --- Renumbering --- \n");
    /*
    for (int i = 0; i < g->n; i++) {
        printf("%d\n", res[i]);
    }*/
    FILE* output = fopen(argv[2], "w");
    graph_to_file(g, output);
    return 1;
}
