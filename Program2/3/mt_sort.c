#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *runner(void *param);

int *global_array;
int *global_sorted_array;

// Structure used for passing sort parameters.	
struct sort_data{
	int start;
	int end;
};

int main(int argc, char *argv[]){
	//Creates the file object for the input
	FILE *input_file;
	char buffer[1000]; // Buffer used for reading in the data from the input file
	int number_of_ints = 0; // Used to dynamically allocate memory for the global arrays
	input_file = fopen(argv[1], "r");

	if(!input_file){
		fprintf(stderr, "File does not exist");
		return 1;
	}

	char *countToken;
	// Goes through the input file and counts the number of elements in it.
	while(fgets(buffer,1000,input_file)!=NULL){
		countToken = strtok(buffer," \n");
		while(countToken != NULL){
			//fprintf(stdout, "%d\n", atoi(token));
			number_of_ints += 1;
			countToken = strtok(NULL, " \n");
		}
	}

	// Closes the original stream to the input file
	fclose(input_file);

	// Reopens it so we can grab items from the beginning
	input_file = fopen(argv[1], "r");

	int i = 0;
	// Creates both arrays using malloc
	global_array = (int *)malloc(sizeof(int)*number_of_ints);
	global_sorted_array = (int *)malloc(sizeof(int)*number_of_ints);
	
	// Token used for getting the numbers
	char *token;

	while(fgets(buffer,1000,input_file) != NULL){
		token = strtok(buffer," \n");
		while(token != NULL){
			//fprintf(stdout, "%d\n", atoi(token));
			global_array[i] = atoi(token);
			i++;
			token = strtok(NULL, " \n");
		}	
	}

	// Finds the halfway mark in the array
	int halfway = i/2;

	// Creates the structures to be passed to the thread functions
	struct sort_data first;
	first.start = 0;
	first.end = halfway-1;

	struct sort_data second;
	second.start = halfway;
	second.end = i-1;

	// Creates the thread ids and other things
	pthread_t firstHalf;
	pthread_t secondHalf;

	pthread_attr_t firstAttr;
	pthread_attr_t secondAttr;

	pthread_attr_init(&firstAttr);
	pthread_attr_init(&secondAttr);

	// Creates the threads and then joins them.
	pthread_create(&firstHalf,&firstAttr,&runner,&first);
	pthread_create(&secondHalf,&secondAttr,&runner,&second);

	pthread_join(firstHalf,NULL);
	pthread_join(secondHalf,NULL);

	// Goes through the unsorted global array and merges them until one of the halves is done
	int m = 0;
	int n = halfway;
	int k = 0;

	while((m <= first.end) && (n <= second.end)){
		if((global_array[m] < global_array[n])){
			global_sorted_array[k] = global_array[m];
			m++;
			k++;
		} else {
			global_sorted_array[k] = global_array[n];
			n++;
			k++;
		}
	}

	// Once one of the halves is done it means the rest of the other half goes at the end of the sorted array.
	while(m <= first.end){
		global_sorted_array[k] = global_array[m];
		m++;
		k++;
	}
	while(n <= second.end){
		global_sorted_array[k] = global_array[n];
		n++;
		k++;
	}
	// Prints out the values of the array so they can be redirected to a file to check for differences.
	for(n=0; n < i; n++){
		fprintf(stdout, "%d\n", global_sorted_array[n]);
	}
}

// My compare function
int cmpfunc(const void *a, const void *b){
	int va = *(const int*)a;
	int vb = *(const int*)b;
	return( (va > vb) - (va < vb) );
}

// My thread function that uses qsort.
void *runner(void *param){
	struct sort_data* p = (struct sort_data*)param;
	qsort(global_array+p->start, (p->end - p->start)+1, sizeof(int), cmpfunc);
}
