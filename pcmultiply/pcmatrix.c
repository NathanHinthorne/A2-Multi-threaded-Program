
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

  // Here is an example to define one producer and one consumer
  pthread_t producerThread;
  pthread_t consumerThread;

  // Add your code here to create threads and so on

  pthread_create(&producerThread, NULL, prod_worker, counter->prod);
  pthread_create(&consumerThread, NULL, cons_worker, counter->cons);

  ProdConsStats *prodStats;
  ProdConsStats *consStats;
  pthread_join(producerThread, (void **)&prodStats);
  pthread_join(consumerThread, (void **)&consStats);
  // TODO with multiple threads, we'll have to sum the stats from each thread

  // These are used to aggregate total numbers for main thread output
  int prs = prodStats->matrixtotal;   // total # of matrices produced
  int cos = consStats->matrixtotal;   // total # of matrices consumed
  int prodtot = prodStats->sumtotal;  // total sum of elements for matrices produced
  int constot = consStats->sumtotal;  // total sum of elements for matrices consumed
  int consmul = consStats->multtotal; // total # multiplications

  // consume ProdConsStats from producer and consumer threads [HINT: return from join]
  // add up total matrix stats in prs, cos, prodtot, constot, consmul
  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n", prs, cos);
  printf("Matrices produced=%d consumed=%d multiplied=%d\n", prodtot, constot, consmul);

  // free buffer
  for (int n = 0; n < MAX_BOUNDED_BUFFER_SIZE; n++)
  {
    FreeMatrix(buffer[n]);
  }
  free(buffer);

  // free ProdConsStats
  free(prodStats);
  free(consStats);

  // free counters
  free(counter->prod);
  free(counter->cons);
  free(counter);

  return 0;
}
