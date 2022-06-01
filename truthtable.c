#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
struct circuit {
    char gate[17];
    int n;
    int s;
    int *inputs;
    int *outputs;
    int *selectors;
};

int power(int base, int exp){
    if(exp < 0){return -1;}
    int result;
    for(int i = 0; i<exp; i++){
        if(i==0){result = base; continue;}
        result = result*base;
    }
    return result;
}

int incrementInputs(int *arr, int icount) {
    int i;
    for (i = icount + 1; i >= 2; i--) {
        arr[i] = !arr[i];
        if (arr[i] == 1) {
            return 1;
        }
    }
    return 0;
}

void NOT(int *values, int iindex, int oindex) {
    values[oindex] = !values[iindex];
}

void AND(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] && values[iindex2];
}

void OR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] || values[iindex2];
}

void NAND(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = !(values[iindex1] && values[iindex2]);
}

void NOR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = !(values[iindex1] || values[iindex2]);
}

void XOR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] ^ values[iindex2];
}

void PASS(int *values, int iindex, int oindex) {
    values[oindex] = values[iindex];
}

void DECODER(int *values, int n, int *iindex, int *oindex) {
    int s = 0;
    int k = power(2,n);
    int i;
    int ll;
    for (i = 0; i < k; i++) {
        values[oindex[i]] = 0;
    }
    for (i = 0; i < n; i++) {
        ll = power(2, n - i - 1);
        s += values[iindex[i]] *ll;
    }
    if(values[iindex[n-1]]==1){
    s = s-1;
    }
    //printf("%d\n", s);
    values[oindex[s]] = 1;
}

void MUX(int *values, int n, int *iindex, int *sindex, int oindex){
    int s = 0;
    int i;
    int ll;
    for (i = 0; i < n; i++) {
        ll = power(2, n - i - 1);
        s += values[sindex[i]] *ll;	
    }

    if(values[sindex[n-1]]==1){
    s = s-1;
    }
    values[oindex] = values[iindex[s]];
}

int indexOf(int size, char **arr, char *var) {
    int i;
    for (i = 0; i < size; i++) {
        if (strcmp(arr[i], var) == 0) {
            return i;
        }
    }
    return -1;
}

void resetValues(int size, int *arr) {
    int i;
    for (i = 0; i < size; i++) {
        arr[i] = 0;
    }
    arr[1] = 1;
}

int main(int argc, char *argv[]){
if(argc -1 !=1){
    printf("Invalid number of arguments\n");
    return 0;
    }

//get file, return if invalid input
FILE *file = fopen(argv[1],"r");
if(!file){
    printf("Invalid Input");
    return 0;
    }

//make steps of circuit with struct circuit

int scount =0;
struct circuit *steps = NULL;
int size = 2;
int icount = 0;
int ocount = 0;
int tcount = 0;
char dir[17];
char **names;
int *values = malloc(size*sizeof(int));

fscanf(file, "%s", dir);
fscanf(file, "%d", &icount);

size += icount;
names = malloc(size * sizeof(char *));
names[0]= "0\0";
names[1]= "1\0";
int i;
for(i = 0; i<icount; i++){
    names[i+2] = malloc(17*sizeof(char));
    fscanf(file,"%*[: ]%16s", names[i+2]);
}

// Output info
fscanf(file, "%s", dir);
fscanf(file,"%d", &ocount);

size +=ocount;
names = realloc(names, size*sizeof(char *));
for(i = 0; i<ocount; i++){
    names[i + icount + 2]= malloc(17*sizeof(char));
    fscanf(file, "%*[: ]%16s",names[i + icount + 2]);
}

while(!feof(file)){
    struct circuit stepi;
    int numInputs =2;
    int numOutputs = 1;
    int fc = fscanf(file,"%s", dir);
    if(fc !=1){
        break;
    }
    scount++;
    stepi.n = 0;
    stepi.s = 0;
    strcpy(stepi.gate, dir);
    if (strcmp(dir, "NOT") == 0) {
        numInputs = 1;
    }
    if (strcmp(dir, "PASS") == 0) {
        numInputs = 1;
    }
    if (strcmp(dir, "DECODER") == 0) {
        fscanf(file, "%d", &numInputs);
        stepi.n = numInputs;
        numOutputs = power(2, numInputs);
	//printf("%d\n",numOutputs);
       // numOutputs = pow(2, numInputs);
    }
    if (strcmp(dir, "MULTIPLEXER") == 0) {
        fscanf(file, "%d", &numInputs);
        stepi.s = numInputs;
        numInputs = power(2,numInputs);
	//printf("%d\n",numInputs);
        //numInputs = pow(2, numInputs);
    }

    stepi.inputs = malloc(numInputs * sizeof(int));
    stepi.outputs = malloc(numOutputs * sizeof(int));
    stepi.selectors = malloc(stepi.s * sizeof(int));

    char v[17];

    for (i = 0; i < numInputs; i++) {
        fscanf(file, "%*[: ]%16s", v);
        stepi.inputs[i] = indexOf(size, names, v);
        }

    for (i = 0; i < stepi.s; i++) {
        fscanf(file, "%*[: ]%16s", v);
        stepi.selectors[i] = indexOf(size, names, v);
    }
    for (i = 0; i < numOutputs; i++) {
        fscanf(file, "%*[: ]%16s", v);
        int idx = indexOf(size, names, v);
        if (idx == -1) {
            size++;
            tcount++;
            names = realloc(names, size * sizeof(char *));
            names[size - 1] = malloc(17 * sizeof(char));
            strcpy(names[size - 1], v);
            stepi.outputs[i] = size - 1;
            }
        else {
            stepi.outputs[i] = idx;
            }
    }// and here jsut remember to switch these up
   
    if (!steps) {
        steps = malloc(sizeof(struct circuit));
        } else {
        steps = realloc(steps, scount * sizeof(struct circuit));
        }
        steps[scount - 1] = stepi;
}

//for (i = 0; i < 1; i++) {
//	struct circuit step = steps[i];
//	int numOutputs= power(2,step.n);
//	for(int j = 0; j<numOutputs;j++){
//		printf("%d\n",step.outputs[j]);
//	}
//}


//making big adjustement here
values = realloc(values, size*sizeof(int));
resetValues(size, values);
while(1 < 2) {
    //print inputs
    for (i = 0; i < icount; i++) {
        printf("%d ", values[i + 2]);
    }
    printf("|");

    //run through steps, calculate outputs
    for (i = 0; i < scount; i++) {
    struct circuit step = steps[i];
	//printf("%s\n",step.gate);
        if (strcmp(step.gate, "NOT") == 0) {
            NOT(values, step.inputs[0], step.outputs[0]);
        }
        if (strcmp(step.gate, "AND") == 0) {
            AND(values, step.inputs[0], step.inputs[1], step.outputs[0]);
        }
        if (strcmp(step.gate, "OR") == 0) {
            OR(values, step.inputs[0], step.inputs[1], step.outputs[0]);
        }
        if (strcmp(step.gate, "NAND") == 0) {
            NAND(values, step.inputs[0], step.inputs[1], step.outputs[0]);
        }
        if (strcmp(step.gate, "NOR") == 0) {
            NOR(values, step.inputs[0], step.inputs[1], step.outputs[0]);
        }
        if (strcmp(step.gate, "XOR") == 0) {
            XOR(values, step.inputs[0], step.inputs[1], step.outputs[0]);
        }
        if (strcmp(step.gate, "PASS") == 0) {
            PASS(values, step.inputs[0], step.outputs[0]);
        }
        if (strcmp(step.gate, "DECODER") == 0){
            DECODER(values, step.n, step.inputs, step.outputs);
        }
        if (strcmp(step.gate, "MULTIPLEXER") == 0) {
            MUX(values, step.s, step.inputs, step.selectors, step.outputs[0]);
        }
    }

    for (i = 0; i < ocount; i++) {
        printf(" %d", values[icount + i + 2]);
        }
    printf("\n");
    if (!incrementInputs(values, icount)) {
        break;
        }
}

for(int i =2; i<size; i++){
    free(names[i]);
}
for(int i = 0; i<scount;i++){
   free(steps[i].inputs);
   free(steps[i].outputs);
   free(steps[i].selectors);
}
free(names);
free(steps);
free(values); 
return 0;
}    
