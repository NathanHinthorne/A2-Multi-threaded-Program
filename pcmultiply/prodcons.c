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

Matrix **buffer;
int headIndex = 0;
int tailIndex = 0;
counter_t *currBufferSize;
Matrix **initBoundedBuffer()
{
  buffer = (Matrix **)malloc(sizeof(Matrix *) * MAX_BOUNDED_BUFFER_SIZE);
  currBufferSize = (counter_t *)malloc(sizeof(counter_t));
  init_cnt(currBufferSize); // initialize counter to 0
  return buffer;
}

// Bounded buffer put() get()
int put(Matrix *value)
{
  // Don't allow NULL matrices to be put into buffer
  if (value == NULL)
  {
    printf("Error: Attempting to put NULL matrix\n");
    return -1;
  }

  // Only put if there's space
  if (get_cnt(currBufferSize) >= MAX_BOUNDED_BUFFER_SIZE)
  {
    printf("Error: Buffer full, cannot put matrix\n");
    return -1;
  }

  printf("PUT Matrix:\n");
  buffer[headIndex] = value;
  DisplayMatrix(buffer[headIndex], stdout);

  headIndex = (headIndex + 1) % MAX_BOUNDED_BUFFER_SIZE;

  increment_cnt(currBufferSize);
  return 0;
}

Matrix *get()
{
  assert(get_cnt(currBufferSize) > 0); // there must be at least 1 matrix to retrieve

  if (buffer[tailIndex] == NULL)
  {
    printf("Error: Attempting to get NULL matrix\n");
    return NULL;
  }

  printf("GET Matrix:\n");

  Matrix *value = buffer[tailIndex];
  DisplayMatrix(value, stdout);

  tailIndex = (tailIndex + 1) % MAX_BOUNDED_BUFFER_SIZE;

  decrement_cnt(currBufferSize);
  return value;
}

// Bounded buffer mutex and condition variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

// flag to indicate when production is complete
int finishedProducing = 0;

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  // get counter from args
  counter_t *prodCounter = (counter_t *)arg;

  // Individual stats for this thread
  ProdConsStats *prodStats = (ProdConsStats *)(malloc(sizeof(ProdConsStats)));

  // init stats
  prodStats->sumtotal = 0;
  prodStats->multtotal = 0;
  prodStats->matrixtotal = 0;

  while (get_cnt(prodCounter) < NUMBER_OF_MATRICES)
  {
    // critical section
    pthread_mutex_lock(&mutex);

    // keep waiting when buffer is full
    while (get_cnt(currBufferSize) >= MAX_BOUNDED_BUFFER_SIZE)
    {
      pthread_cond_wait(&not_full, &mutex);
    }

    // when thread is woken, check again since another prod worker may have added a matrix
    if (get_cnt(prodCounter) >= NUMBER_OF_MATRICES)
    {
      // finish early
      pthread_cond_signal(&not_empty);
      pthread_mutex_unlock(&mutex);
      return (void *)prodStats;
    }

    Matrix *matrix;
    if (MATRIX_MODE == 0)
    {
      // no size given, generate random
      matrix = GenMatrixRandom();
    }
    else
    {
      // size given
      matrix = GenMatrixBySize(MATRIX_MODE, MATRIX_MODE);
    }

    put(matrix);

    // Update this thread's statistics
    prodStats->matrixtotal++;
    prodStats->sumtotal += SumMatrix(matrix);

    // Update synchronized counter
    increment_cnt(prodCounter);

    // signal consumers
    pthread_cond_signal(&not_empty);

    pthread_mutex_unlock(&mutex);
  }

  // lock to avoid race condition
  pthread_mutex_lock(&mutex);
  finishedProducing = 1;
  pthread_cond_broadcast(&not_empty); // Wake up all consumers to avoid deadlock
  pthread_mutex_unlock(&mutex);

  return (void *)prodStats;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  // get counter from args
  counter_t *consCounter = (counter_t *)arg;

  // Individual stats for this thread
  ProdConsStats *consStats = (ProdConsStats *)(malloc(sizeof(ProdConsStats)));

  // init stats
  consStats->sumtotal = 0;
  consStats->multtotal = 0;
  consStats->matrixtotal = 0;

  Matrix *m1, *m2, *m3;

  while (get_cnt(consCounter) < NUMBER_OF_MATRICES)
  {
    // critical section
    pthread_mutex_lock(&mutex);

    // keep waiting when buffer is empty
    while (get_cnt(currBufferSize) <= 0)
    {
      if (finishedProducing && get_cnt(currBufferSize) == 0)
      {
        pthread_mutex_unlock(&mutex);
        return (void *)consStats;
      }

      pthread_cond_wait(&not_empty, &mutex);

      // when thread is woken, check again since another cons worker may have consumed a matrix
      if (get_cnt(consCounter) >= NUMBER_OF_MATRICES)
      {
        // finish early
        pthread_mutex_unlock(&mutex);
        return (void *)consStats;
      }
    }

    m1 = get();

    // Update this thread's statistics
    consStats->matrixtotal++; // Count consumption
    consStats->sumtotal += SumMatrix(m1);

    // Update synchronized counter
    increment_cnt(consCounter);

    /* Originally, we were grabbing
    two matrices on every step of the while loop in prod_worker().
    Claude.ai gave us code to grab one matrix at a time. */

    int matrixIsValid = 0;
    while (!matrixIsValid)
    {
      // keep waiting when buffer is empty
      while (get_cnt(currBufferSize) <= 0)
      {
        if (finishedProducing && get_cnt(currBufferSize) == 0)
        {
          FreeMatrix(m1);
          pthread_mutex_unlock(&mutex);
          return (void *)consStats;
        }

        pthread_cond_wait(&not_empty, &mutex);

        // when thread is woken, check again since another cons worker may have consumed a matrix
        if (get_cnt(consCounter) >= NUMBER_OF_MATRICES)
        {
          pthread_mutex_unlock(&mutex);
          return (void *)consStats;
        }
      }

      m2 = get();

      // Update this thread's statistics
      consStats->matrixtotal++;
      consStats->sumtotal += SumMatrix(m2);

      // Update synchronized counter
      increment_cnt(consCounter);

      m3 = MatrixMultiply(m1, m2);
      if (m3 != NULL)
      {
        consStats->multtotal++; // Count successful multiplication
        matrixIsValid = 1;
      }
      else
      {
        FreeMatrix(m2); // Invalid M2, try another
      }
    }

    // show results
    DisplayMatrix(m1, stdout);
    printf("   X\n");
    DisplayMatrix(m2, stdout);
    printf("   =\n");
    DisplayMatrix(m3, stdout);

    // clean up
    FreeMatrix(m1);
    FreeMatrix(m2);
    FreeMatrix(m3);

    // signal producers
    pthread_cond_signal(&not_full);

    pthread_mutex_unlock(&mutex);
  }

  return (void *)consStats;
}