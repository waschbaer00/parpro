#include "familytree.h"
#include <omp.h>

void traverse(tree *node, int numThreads){
	
			if(node != NULL){
				node->IQ = compute_IQ(node->data);
				genius[node->id] = node->IQ;
			}

			#pragma omp task firstprivate(node,numThreads)
			{		
				printf("hello1");
				traverse(node->right, numThreads);
			}

			#pragma omp task firstprivate(node,numThreads)
			{
				printf("hello2");
				traverse(node->left, numThreads);
			}
		

}
