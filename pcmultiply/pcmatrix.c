
/*
 *  pcmatrix module
 *  Primary module providing control flow for the pcMatrix program
 *
 *  Producer consumer bounded buffer program to produce random matrices in parallel
 *  and consume them while searching for valid pairs for matrix multiplication.
 *  Matrix multiplication requires the first matrix column count equal the
 *  second matrix row count.
 *
 *  A matrix is consumed from the bounded buffer.  Then matrices are consumed
 *  from the bounded buffer, ONE AT A TIME, until an eligible matrix for multiplication
 *  is found.
 *
 *  Totals are tracked using the ProdConsStats Struct for each thread separately:
 *  - the total number of matrices multiplied (multtotal from each consumer thread)
 *  - the total number of matrices produced (matrixtotal from each producer thread)
 *  - the total number of matrices consumed (matrixtotal from each consumer thread)
 *  - the sum of all elements of all matrices produced and consumed (sumtotal from each producer and consumer thread)
 *
 *  Then, these values from each thread are aggregated in main thread for output
 *
 *  Correct programs will produce and consume the same number of matrices, and
 *  report the same sum for all matrix elements produced and consumed.
 *
 *  Each thread produces a total sum of the value of
 *  randomly generated elements.  Producer sum and consumer sum must match.
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include "matrix.h"
#include "counter.h"
#include "prodcons.h"
#include "pcmatrix.h"

int main(int argc, char *argv[])
{
  // Process command line arguments
  int numw = NUMWORK;
  if (argc == 1)
  {

    MAX_BOUNDED_BUFFER_SIZE = MAX;
    NUMBER_OF_MATRICES = LOOPS;
    MATRIX_MODE = DEFAULT_MATRIX_MODE;
    printf("USING DEFAULTS: worker_threads=%d bounded_buffer_size=%d matricies=%d matrix_mode=%d\n", numw, MAX_BOUNDED_BUFFER_SIZE, NUMBER_OF_MATRICES, MATRIX_MODE);
  }
  else
  {
    if (argc == 2)
    {
      numw = atoi(argv[1]);
      MAX_BOUNDED_BUFFER_SIZE = MAX;
      NUMBER_OF_MATRICES = LOOPS;
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 3)
    {
      numw = atoi(argv[1]);
      MAX_BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = LOOPS;
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 4)
    {
      numw = atoi(argv[1]);
      MAX_BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = atoi(argv[3]);
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 5)
    {
      numw = atoi(argv[1]);
      MAX_BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = atoi(argv[3]);
      MATRIX_MODE = atoi(argv[4]);
    }
    printf("USING: worker_threads=%d bounded_buffer_size=%d matricies=%d matrix_mode=%d\n", numw, MAX_BOUNDED_BUFFER_SIZE, NUMBER_OF_MATRICES, MATRIX_MODE);
  }

  time_t t;
  // Seed the random number generator with the system time
  srand((unsigned)time(&t));

  Matrix **buffer = initBoundedBuffer();

  // Got help in debugging and chatgpt said to use this.
  counters_t *counter = (counters_t *)malloc(sizeof(counters_t));
  counter->prod = (counter_t *)malloc(sizeof(counter_t));
  counter->cons = (counter_t *)malloc(sizeof(counter_t));

  init_cnt(counter->prod);
  init_cnt(counter->cons);

  printf("Producing %d matrices in mode %d.\n", NUMBER_OF_MATRICES, MATRIX_MODE);
  printf("Using a shared buffer of size=%d\n", MAX_BOUNDED_BUFFER_SIZE);
  printf("With %d producer and consumer thread(s).\n", numw);
  printf("\n");

  pthread_t prodWorkerThreads[numw];
  pthread_t consWorkerThreads[numw];

  for (int i = 0; i < numw; i++)
  {
    // Create specified number of producer and consumer threads
    pthread_create(&prodWorkerThreads[i], NULL, prod_worker, counter->prod);
    pthread_create(&consWorkerThreads[i], NULL, cons_worker, counter->cons);
  }

  ProdConsStats *totalProdStats = (ProdConsStats *)malloc(sizeof(ProdConsStats));
  ProdConsStats *totalConsStats = (ProdConsStats *)malloc(sizeof(ProdConsStats));

  ProdConsStats *prodStats;
  ProdConsStats *consStats;
  for (int i = 0; i < numw; i++)
  {
    // wait for each thread to finish
    pthread_join(prodWorkerThreads[i], (void **)&prodStats);
    pthread_join(consWorkerThreads[i], (void **)&consStats);

    // Aggregate stats from each thread
    totalProdStats->matrixtotal += prodStats->matrixtotal;
    totalProdStats->sumtotal += prodStats->sumtotal;
    totalProdStats->multtotal += prodStats->multtotal;

    totalConsStats->matrixtotal += consStats->matrixtotal;
    totalConsStats->sumtotal += consStats->sumtotal;
    totalConsStats->multtotal += consStats->multtotal;
  }
  free(consStats);
  free(prodStats);

  int prs = totalProdStats->matrixtotal;   // total # of matrices produced
  int cos = totalConsStats->matrixtotal;   // total # of matrices consumed
  int prodtot = totalProdStats->sumtotal;  // total sum of elements for matrices produced
  int constot = totalConsStats->sumtotal;  // total sum of elements for matrices consumed
  int consmul = totalConsStats->multtotal; // total # multiplications

  // consume ProdConsStats from producer and consumer threads [HINT: return from join]
  // add up total matrix stats in prs, cos, prodtot, constot, consmul
  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n", prodtot, constot);
  printf("Matrices produced=%d consumed=%d multiplied=%d\n", prs, cos, consmul);

  free(buffer);

  // free ProdConsStats
  free(totalProdStats);
  free(totalConsStats);

  // free counters
  free(counter->prod);
  free(counter->cons);
  free(counter);

  return 0;
}
