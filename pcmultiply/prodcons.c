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
int currBufferSize = 0;

Matrix **initBoundedBuffer()
{
  buffer = (Matrix **)malloc(sizeof(Matrix *) * MAX_BOUNDED_BUFFER_SIZE);
  for (int n = 0; n < MAX_BOUNDED_BUFFER_SIZE; n++)
  {
    buffer[n] = (Matrix *)malloc(sizeof(Matrix));
  }

  return buffer;
}

// Define Locks, Condition variables, and so on here

// Bounded buffer put() get()
int put(Matrix *value)
{
  buffer[headIndex] = value;
  printf("PUT Matrix:\n");
  DisplayMatrix(buffer[headIndex], stdout);

  headIndex = (headIndex + 1) % MAX_BOUNDED_BUFFER_SIZE;

  if (headIndex == tailIndex) // when head runs into tail
  {
    tailIndex = (tailIndex + 1) % MAX_BOUNDED_BUFFER_SIZE;
  }

  currBufferSize++;
  return 0;
}

Matrix *get()
{
  assert(currBufferSize > 0); // there must be at least 1 matrix to retrieve

  Matrix *value = buffer[tailIndex];

  printf("GET Matrix:\n");
  DisplayMatrix(value, stdout);
  if (headIndex != tailIndex)
  {
    tailIndex = (tailIndex + 1) % MAX_BOUNDED_BUFFER_SIZE;
  }

  currBufferSize--;
  return value;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

/*
 pthread_create requires this EXACT signature for thread functions:
 void *thread_function(void *arg)

 This is because:
  pthread_create is designed to work with ANY type of thread function
  It can 't know in advance what specific type you' ll want to return void *
  is a "generic pointer" that can point to any type
  This makes pthread_create flexible and reusable
*/

int finishedProducing = 0;

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  // get counter from args
  counter_t *prodCounter = (counter_t *)arg;

  // Individual stats for this thread
  ProdConsStats *prodStats = (ProdConsStats *)(malloc(sizeof(ProdConsStats)));

  while (get_cnt(prodCounter) < NUMBER_OF_MATRICES)
  {
    // critical section
    pthread_mutex_lock(&mutex);

    // keep waiting when buffer is full
    while (currBufferSize >= MAX_BOUNDED_BUFFER_SIZE)
    {
      printf("DEBUG: Buffer is full\n");
      pthread_cond_wait(&not_full, &mutex);
    }

    Matrix *matrix = GenMatrixRandom();

    put(matrix);

    // Update this thread's statistics
    prodStats->matrixtotal++;
    prodStats->sumtotal += SumMatrix(matrix);

    // increment counter (indicating a new matrix was added)
    increment_cnt(prodCounter);

    // signal consumers
    pthread_cond_signal(&not_empty);

    pthread_mutex_unlock(&mutex);
  }

  finishedProducing = 1;

  return (void *)prodStats;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  // get counter from args
  counter_t *consCounter = (counter_t *)arg;

  // Individual stats for this thread
  ProdConsStats *consStats = (ProdConsStats *)(malloc(sizeof(ProdConsStats)));
  Matrix *m1, *m2, *m3;

  while (get_cnt(consCounter) < NUMBER_OF_MATRICES)
  {
    // critical section
    pthread_mutex_lock(&mutex);

    // keep waiting when buffer is empty
    while (currBufferSize <= 0)
    {
      pthread_cond_wait(&not_empty, &mutex);
      if (finishedProducing == 1)
      {
        return (void *)consStats;
      }
    }

    m1 = get();
    consStats->matrixtotal++; // Count consumption
    consStats->sumtotal += SumMatrix(m1);
    increment_cnt(consCounter);

    /* Originally, we were grabbing
    two matrices on every step of the while loop in prod_worker().
    Claude.ai gave us code to grab one matrix at a time. */

    int matrixIsValid = 0;
    while (!matrixIsValid)
    {
      // keep waiting when buffer is empty
      // TODO check that this is good, maybe a chance that get is still producing and consumer won't consume the last matrix becasue it returned.
      // TODO consider calling pthread_cond_wait
      // if (currBufferSize <= 0)
      // {
      //   return (void *)consStats;
      // }
      // keep waiting when buffer is empty
      while (currBufferSize <= 0)
      {
        printf("DEBUG: Buffer is empty\n");
        pthread_cond_wait(&not_full, &mutex);
        printf("DEBUG: Released\n");
        if (finishedProducing == 1)
        {
          return (void *)consStats;
        }
      }
      m2 = get();
      consStats->matrixtotal++; // Count consumption
      consStats->sumtotal += SumMatrix(m2);
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

    DisplayMatrix(m1, stdout);
    printf("X\n");
    DisplayMatrix(m2, stdout);
    printf("=\n");
    DisplayMatrix(m3, stdout);

    FreeMatrix(m1);
    FreeMatrix(m2);
    FreeMatrix(m3);

    // signal producers
    pthread_cond_signal(&not_full);

    pthread_mutex_unlock(&mutex);
  }

  return (void *)consStats;
}