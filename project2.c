#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_BUFFER_SIZE 127


typedef struct Process {
	int pid; //process id
	int FirstInsertion;  //ilk 
	int executionTime; 
	int executionTimeRem; //remaining
	double arrivalTime;
	double WT; //waiting time accumulated
	double currentPriority;
	int finishTime; // WT = finish -arrival -execution
} Process;


typedef struct BHNode {

double priority;
Process *p;
int degree;
struct BHNode *child;
struct BHNode *parent;
struct BHNode *sibling;
} BHNode;

typedef struct BinomialHeap {
BHNode *head;
} BinomialHeap;



/* -------- Node and Heap Initialization -------- */
BHNode *nodeInit(Process *p, double priority){
BHNode *node = malloc(sizeof(BHNode));
if (node == NULL)
return NULL;

node->priority = priority;
node->degree = 0;
node->p = p;
node->child = NULL;
node->parent = NULL;
node->sibling = NULL;
return node;
}

BinomialHeap *heapInit(){
BinomialHeap *heap=malloc(sizeof(BinomialHeap));
if (heap == NULL)
return NULL;
heap->head = NULL;
return heap;
}

int isHigherPriority(BHNode *a, BHNode *b) {
	if(a->priority < b->priority) return 1; //a daha oncelikli ise
	
	if(a->priority> b->priority) return 0; // b daha oncelikli ise
	
	//esitse
	return(a->p->arrivalTime < b->p->arrivalTime); 
}

/* -------- Heap Merge (by degree) -------- */
BHNode *heapMerge(BinomialHeap *heap1, BinomialHeap *heap2){
BHNode *head;
BHNode *tail;
BHNode *h1It;
BHNode *h2It;
if (heap1->head == NULL)
return heap2->head;
if (heap2->head == NULL)
return heap1->head;
h1It = heap1->head;
h2It = heap2->head;
if (h1It->degree <= h2It->degree) {
head = h1It;
h1It = h1It->sibling;
} else {
head = h2It;
h2It = h2It->sibling;
}
tail = head;
while (h1It != NULL && h2It != NULL) {
if (h1It->degree <= h2It->degree) {
tail->sibling = h1It;
h1It = h1It->sibling;
} else {
tail->sibling = h2It;
h2It = h2It->sibling;
}
tail = tail->sibling;
}
tail->sibling = (h1It != NULL) ? h1It : h2It;
return head;
}

/* -------- Heap Union -------- */
BHNode *heapUnion(BinomialHeap *original, BinomialHeap *uni){
BHNode *new_head;
BHNode *prev;
BHNode *aux;
BHNode *next;

new_head = heapMerge(original, uni);
original->head = NULL;
uni->head = NULL;

if (new_head == NULL) return NULL;

prev = NULL;
aux = new_head;
next = aux->sibling;

while (next != NULL) {
if (aux->degree != next->degree ||
(next->sibling != NULL &&
next->sibling->degree == aux->degree)) {
prev = aux;
aux = next;
} else {
if (isHigherPriority(aux, next)) {
aux->sibling = next->sibling;
next->parent = aux;
next->sibling = aux->child;
aux->child = next;
aux->degree++;
} else {
if (prev == NULL)
new_head = next;
else
prev->sibling = next;
aux->parent = next;
aux->sibling = next->child;
next->child = aux;
next->degree++;
aux = next;
}
}
next = aux->sibling;
}

return new_head;
}

/* -------- Heap Operations -------- */
void heapInsert(BinomialHeap *heap, BHNode *node){
BinomialHeap *temp;

temp = heapInit();
if (temp == NULL) return;

temp->head = node;
heap->head = heapUnion(heap, temp);
free(temp);
}


void heapRemove(BinomialHeap *heap, BHNode *node, BHNode *before){
BinomialHeap *temp;
BHNode *child;
BHNode *new_head;
BHNode *next;

if (node == heap->head)
heap->head = node->sibling;
else if (before != NULL)
before->sibling = node->sibling;

new_head = NULL;
child = node->child;

while (child != NULL) {
next = child->sibling;
child->sibling = new_head;
child->parent = NULL;
new_head = child;
child = next;
}

temp = heapInit();
if (temp == NULL) return;

temp->head = new_head;
heap->head = heapUnion(heap, temp);
free(temp);
}

BHNode *heapMin(BinomialHeap *heap){
BHNode *min;
BHNode *min_prev;
BHNode *curr;
BHNode *prev;

if (heap->head == NULL) return NULL;

min = heap->head;
min_prev = NULL;
prev = min;
curr = min->sibling;

while (curr != NULL) {
if (isHigherPriority(curr, min)) {
min = curr;
min_prev = prev;
}

prev = curr;
curr = curr->sibling;
}

heapRemove(heap, min, min_prev);
return min;
}

void heapFree(BinomialHeap *heap){
while (heapMin(heap) != NULL)
;
free(heap);
}

double calcPriority(Process *p, int emax) {
	if(p->FirstInsertion) return p->executionTimeRem;
	
	
	//tekrardan giris hesaplama c(e)*eN
	double c = 1.0 / exp(-pow((2.0 * p->executionTimeRem) / (3.0 * emax), 3));
	double result = c * p->executionTimeRem;
	
	return result;
}

void printPriorityCalc(Process *p, int emax, double result) {
	if(p->FirstInsertion) 
	printf("P%d: %.3f ", p->pid + 1, result);
	else 
	printf("P%d: (1/exp-(2*%.0f/(3*%d))^3)*%.0f = %.3f ", p->pid + 1, (double)p->executionTimeRem, emax, (double)p->executionTimeRem, result);
	
}

void printNodes(BHNode *node){
	if(node == NULL) return;
	
	printf("P%d: %.3f ", node->p->pid +1, node->priority);
	
	if(node->child != NULL) printNodes(node->child);
	if(node->sibling != NULL) printNodes(node->sibling);
	
	
}
 
void printHeap(BinomialHeap *heap, int time) {
	printf("%-5d | Processes: ", time);
	if(heap->head == NULL) {
		printf("EMPTY");
	} else printNodes(heap->head);
	

}


double scheduleSystem(Process input[], int n, int q, int emax){
	//printf("calisiyo q=%d\n", q);
	
	Process process[100];
	int i;
	for(i = 0; i< n; i++) {
		process[i] =input[i];
		process[i].executionTimeRem = process[i].executionTime;
		process[i].WT = 0;
		process[i].FirstInsertion = 1;
	}
	
	BinomialHeap *heap = heapInit();
	
	int time = 0;
	int finished = 0;
	int next = 0;
	
	printf("\nTime  | Processes in Bh & Their Priorities\n");
	printf("-----------------------------------------------------------------\n");
	while(finished < n) {
		while(next < n && process[next].arrivalTime <= time) {	
			double pr = calcPriority(&process[next], emax);
			BHNode *node = nodeInit(&process[next], pr);
			//printf("deneme %lf\n", pr);
			heapInsert(heap, node); 
			process[next].FirstInsertion = 0;
			next++;
		}
		printHeap(heap, time);
		
		BHNode *node = heapMin(heap);
		if(node == NULL) {
			printf("\n");
			time++;
			continue;
		}
	
		Process *p = node->p;
		
		
		
		int run = (p->executionTimeRem > q) ? q: p->executionTimeRem;
		time += run;
		p->executionTimeRem -= run;
		
		while(next < n&& process[next].arrivalTime <= time) {
			double pr = calcPriority(&process[next], emax);
			//printf("tam: %lf\n", pr);
			BHNode *node = nodeInit(&process[next], pr);
			heapInsert(heap, node);
			next++;
		}
		
		if(p->executionTimeRem >0) {
			p->FirstInsertion = 0;
			double pr = calcPriority(p, emax);
			//printf("now: %lf\n", pr);
			printf(" | ");
			printPriorityCalc(p, emax, pr);
			BHNode *node = nodeInit(p, pr);
			heapInsert(heap, node);
		} else {
			p->finishTime = time;
			finished++;
		}
		free(node);
		printf("\n");
	}
	
	
	double sum = 0;
	
	for(i = 0; i< n; i++) {
		process[i].WT = process[i].finishTime - process[i].arrivalTime - process[i].executionTime;
		sum += process[i].WT;
	}
	
	double awt = sum / n;
	heapFree(heap);
	
	//printf("complete\n");
	return awt;
}


int main(void){
	FILE *file = fopen("input.txt", "r");
	if(!file) {printf("error\n"); return 1;
	}
	
	Process processes[100];
	int n = 0;
	int emax = 0;
	int bestq = -1;
	double minAWT = 9999.0;
	
	while(fscanf(file, "%d,%lf", &processes[n].executionTime, &processes[n].arrivalTime) == 2) {
		processes[n].pid = n;
		processes[n].executionTimeRem= processes[n].executionTime;
		processes[n].WT = 0;
		processes[n].FirstInsertion = 1;
		if(processes[n].executionTime > emax)
		emax = processes[n].executionTime;
		
		n++;
	} fclose(file);
	
	if(n == 0) {
		return 1;
	}
	
	int q;
	for(q = 1; q<= emax; q++) {
		double AWT = scheduleSystem(processes, n, q, emax);
		
		printf("For q = %-5d || AWT: %.3f\n", q, AWT);
		
		if(AWT < minAWT) {
			minAWT = AWT;
			bestq = q;
		}
	}
		//printf("Quantum q = %d -> AWT: %.2f\n", i, awt);
		
	//printf("max e = %d\n", emax);
	printf("\n\nBest q = %d, Minimum AWT = %.3f\n", bestq, minAWT);
/*BinomialHeap *heap;
char buf[MAX_BUFFER_SIZE];
char op;
int value;
BHNode *min;
heap = heapInit();
if (heap == NULL)
return 1;
while (fgets(buf, MAX_BUFFER_SIZE, stdin) != NULL) {
if (sscanf(buf, "%c %d", &op, &value) >= 1) {
if (op == 'i') {
heapInsert(heap, value);
} else if (op == 'd') {
min = heapMin(heap);
if (min != NULL)
printf("%d\n", min->value);
}
}
}
heapFree(heap);*/
return 0;
}
