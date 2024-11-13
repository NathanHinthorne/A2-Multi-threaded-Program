/*
 *prodcons module
 * Producer Consumer module
 *
 *Implements routines for the producer consumer module based on
 * chapter 30, section 2 of Operating Systems : Three Easy Pieces
 *
 *University of Washington, Tacoma
 * TCSS 422 - Operating Systems
 */

// Include only libraries for this module
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"

Matrix **bigMatrix;
int headIndex = 0;
int tailIndex = 0;
int bufferSize = 0;

counters_t *counters;

// Probably init in main.
Matrix **initBoundedBuffer()
{
  bigMatrix = (Matrix **)malloc(sizeof(Matrix *) * BOUNDED_BUFFER_SIZE);
  for (int n = 0; n < BOUNDED_BUFFER_SIZE; n++)
  {
    bigMatrix[n] = (Matrix *)malloc(sizeof(Matrix));
  }

  // TODO fix segmentation fault.
  // init_cnt(&counters->prod);
  // init_cnt(&counters->cons);

  return bigMatrix;
}

counters_t *initCounters()
{
  // TODO set counters to some initial value (using malloc)
  // TODO call init_cnt() for each counter
}

// Define Locks, Condition variables, and so on here

// Bounded buffer put() get()
int put(Matrix *value)
{
  bigMatrix[headIndex] = value;
  printf("PUT Matrix:");
  DisplayMatrix(bigMatrix[headIndex], stdout);

  headIndex = (headIndex + 1) % BOUNDED_BUFFER_SIZE;

  if (headIndex == tailIndex) // when head runs into tail
  {
    tailIndex = (tailIndex + 1) % BOUNDED_BUFFER_SIZE;
  }

  bufferSize++;
}

Matrix *get()
{
  assert(bufferSize > 0); // there must be at least 1 matrix to retrieve

  Matrix *value = bigMatrix[tailIndex];

  printf("GET Matrix:");
  DisplayMatrix(value, stdout);

  if (headIndex != tailIndex)
  {
    tailIndex = (tailIndex + 1) % BOUNDED_BUFFER_SIZE;
  }

  bufferSize--;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  // TODO loop this WHOLE section of code using a while loop. repeat until the given global constant for num of matrices has been reached by counters->prod
  Matrix *randomMatrix = GenMatrixRandom();

  // critical section
  pthread_mutex_lock(&mutex);

  // keep waiting when buffer is full
  while (bufferSize >= BOUNDED_BUFFER_SIZE)
  {
    pthread_cond_wait(&not_full, &mutex);
  }

  put(randomMatrix);

  // increment counter (indicating a new matrix was added)
  increment_cnt(&counters->cons);

  // signal consumers
  pthread_cond_signal(&not_empty);

  pthread_mutex_unlock(&mutex);

  return NULL;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  // TODO remember to call FreeMatrix() after you're done using it

  // TODO take ONE matrix at a time. once you have two, check to make sure matrix multiplication is possible
  return NULL;
}