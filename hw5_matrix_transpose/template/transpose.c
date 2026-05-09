#include "util.h"
#include <pthread.h>
#include <stdatomic.h>
#include <math.h>

// structure to hold parameters for each worker thread in the parallel transposition
typedef struct {
    Mat *mat;                    // pointer to matrix to transpose
    unsigned int grain;          // coarseness level: number of exchanges per thread before requesting more work
    atomic_uint *work_index;     // atomic counter to track current exchange index (shared across threads)
} thread_params_t;

// single-threaded in-place matrix transposition
// performs in-place transposition of a square matrix by swapping elements
// symmetric with respect to the main diagonal
void
mat_sqtrans_seq(Mat *mat)
{
    unsigned int n = mat->n;
    double *ptr = mat->ptr;
    double temp;
    
    // iterate through the upper triangular part of the matrix
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = i + 1; j < n; j++) {
            // calculate linear indices for row-major storage
            // element A[i][j] is at index i*n + j
            // element A[j][i] is at index j*n + i
            unsigned int idx1 = i * n + j;
            unsigned int idx2 = j * n + i;
            
            // swap A[i][j] with A[j][i]
            temp = ptr[idx1];
            ptr[idx1] = ptr[idx2];
            ptr[idx2] = temp;
        }
    }
    
    return;
}

// worker function for each thread in parallel transposition
// each thread repeatedly requests work (grain number of exchanges) and performs
// transposition swaps
void *
thread_worker(void *arg)
{
    thread_params_t *params = (thread_params_t *)arg;
    Mat *mat = params->mat;
    unsigned int grain = params->grain;
    atomic_uint *work_index = params->work_index;
    
    unsigned int n = mat->n;
    double *ptr = mat->ptr;
    double temp;
    
    // total number of exchanges needed (upper triangular part)
    // number of exchanges = n*(n-1)/2
    unsigned int total_exchanges = (n * (n - 1)) / 2;
    
    // each thread requests work in chunks of grain exchanges
    while (1) {
        // get current work index and increment by grain
        unsigned int start_idx = atomic_fetch_add(work_index, grain);
        
        // if already processed all exchanges, exit
        if (start_idx >= total_exchanges) {
            break;
        }
        
        // determine how many exchanges this thread will process
        unsigned int end_idx = start_idx + grain;
        if (end_idx > total_exchanges) {
            end_idx = total_exchanges;
        }
        
        // process assigned exchanges
        for (unsigned int idx = start_idx; idx < end_idx; idx++) {
            // convert linear exchange index to (i, j) coordinates 
            // in row-major order of the upper triangle:
            // row i has (n-i-1) exchanges for columns j=i+1..n-1
            // number of exchanges before row i: i*n - i*(i+1)/2
            // i*n - i*(i+1)/2 <= idx to find row i
            // i = floor([(2n-1) - sqrt((2n-1)^2 - 8*idx)] / 2)
            
            double discriminant = (2.0 * n - 1) * (2.0 * n - 1) - 8.0 * idx;
            unsigned int i = (unsigned int)((2.0 * n - 1 - sqrt(discriminant)) / 2.0);
            
            // position within row
            unsigned int exchange_in_row = idx - (i * n - i * (i + 1) / 2);
            unsigned int j = i + 1 + exchange_in_row;
            
            // calculate linear indices for row-major storage
            unsigned int idx1 = i * n + j;
            unsigned int idx2 = j * n + i;
            
            // swap A[i][j] with A[j][i]
            temp = ptr[idx1];
            ptr[idx1] = ptr[idx2];
            ptr[idx2] = temp;
        }
    }
    
    return NULL;
}

// multi-threaded in-place matrix transposition
// performs in-place transposition using multiple threads with coarseness
// each thread performs grain exchanges before requesting more work 
void
mat_sqtrans_par(Mat *mat, unsigned int grain, unsigned int threads)
{
    if (threads == 1) {
        // fall back to sequential implementation for single thread
        mat_sqtrans_seq(mat);
        return;
    }
    
    // initialize atomic work index counter
    atomic_uint work_index = 0;
    
    // create thread parameters
    thread_params_t params;
    params.mat = mat;
    params.grain = grain;
    params.work_index = &work_index;
    
    // array to store thread handles
    pthread_t thread_ids[threads];
    
    // create all worker threads
    for (unsigned int i = 0; i < threads; i++) {
        pthread_create(&thread_ids[i], NULL, thread_worker, &params);
    }
    
    // wait for all threads to complete
    for (unsigned int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }
    
    return;
}
