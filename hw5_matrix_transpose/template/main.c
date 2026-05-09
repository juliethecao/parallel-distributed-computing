#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

void
print_help(char *prog_name)
{
    fprintf(stderr, "Transpose a square matrix of size n.\n\n"
            "USAGE:\n"
            "  %s <n> s <verb>\n"
            "  %s <n> p <g> <t> <verb>\n\n", prog_name, prog_name);
    fprintf(stderr,"ARGUMENTS:\n"
            "  n    : Size of the square matrix. 2 <= n <= 20000\n"
            "  s/p  : Character that indicates whether to use the sequential\n"
            "         or the parallel transposer.\n"
            "  g    : Granularity used by the multi-thread transposer.\n"
            "  t    : Number of threads used by the multi-thread transposer.\n"
            "  verb : 0=prints only execution time.\n"
            "         1=print the resulting matrix.\n"
            "         2=print the original and resulting matrix.\n");
}

int main(int argc, char **argv){
    //get the command line arguments
    unsigned int n, alg, gran, thr, verb;
    if(argc == 4){
        n = atoi(argv[1]);
        alg = argv[2][0];
        gran = 1;
        thr = 1;
        verb = atoi(argv[3]);
    } else if(argc == 6){
        n = atoi(argv[1]);
        alg = argv[2][0];
        gran = atoi(argv[3]);
        thr = atoi(argv[4]);
        verb = atoi(argv[5]);
    } else {
        print_help(argv[0]);
        exit(1);
    }

    // validate arguments
    if(n < 2 || 20000 < n){
        fprintf(stderr, "The size of the matrix 'n' (%d) must be >= 2 and <= 20,000.\n", n);
        exit(1);
    }
    if(gran < 1 || (n*n-n)/2 < gran){
        fprintf(stderr, "The granularity 'g' (%d) must be >= 1 and <= %d.\n", \
               gran, (n*n-n)/2);
        exit(1);
    }
    if(thr < 1){
        fprintf(stderr, "The number of threads 't' (%d) must be >= 1.\n", thr);
        exit(1);
    }

    //create and fill one matrix
    Mat M;
    mat_alloc(&M, n, n);
    mat_fill_rand(&M);
    
    //print original matrix
    if(verb >= 2){
        printf("Original:\n");
        mat_print(&M);
        printf("\n");
    }

    //run and measure time taken
    double end, start;
    if(alg == 's'){
        start = stopwatch(); 
        mat_sqtrans_seq(&M);
        end = stopwatch();
    }else if (alg == 'p'){
        start = stopwatch();
        mat_sqtrans_par(&M, gran, thr);
        end = stopwatch();
    }else{
        fprintf(stderr, "Error: Unexpected algorithm '%c'.\n", (char)alg);
        exit(1);
    }

    //print resulting matrices
    if(verb >= 1){
        printf("Transposed:\n");
        mat_print(&M);
    }

    //print execution time
    printf("%.8f\n", end-start);
    
    return 0;
}
