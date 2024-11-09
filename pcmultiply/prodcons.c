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

Matrix** bigMatrix;
int bufferSize = 0;
int headIndex = 0;
int tailIndex = 0;

// Probably init in main.
Matrix** initBoundedBuffer() {

  bigMatrix = (Matrix**)malloc(sizeof(Matrix*) * BOUNDED_BUFFER_SIZE);
  for (int n = 0; n < BOUNDED_BUFFER_SIZE; n++) {
    bigMatrix[n] = (Matrix*)malloc(sizeof(Matrix));
  }
  return bigMatrix;
}

// Define Locks, Condition variables, and so on here

// Bounded buffer put() get()
/**
 *
 * queue:
 * 0 1 2 3 4
 * top->0
 * put(A)
 * top->1
 * put(B)
 * top->2
 * get()
 * getting at top-1
 *
*/
int put(Matrix* value)
{
  bigMatrix[headIndex] = value;
  printf("PUT Matrix:");
  DisplayMatrix(bigMatrix[headIndex], stdout);

  headIndex = (headIndex + 1) % BOUNDED_BUFFER_SIZE;

  if (headIndex == tailIndex) // when head runs into tail
  {
    tailIndex = (tailIndex + 1) % BOUNDED_BUFFER_SIZE;
  }

  bufferSize += 1;
}

Matrix* get()
{
  bufferSize--;
  assert(bufferSize > 0); // there must be at least 1 matrix to retrieve

  Matrix* value = bigMatrix[tailIndex];

  printf("GET Matrix:");
  DisplayMatrix(value, stdout);

  if (headIndex != tailIndex)
  {
    tailIndex = (tailIndex + 1) % BOUNDED_BUFFER_SIZE;
  }
}



// Matrix PRODUCER worker thread
void* prod_worker(void* arg)
{
  return NULL;
}

// Matrix CONSUMER worker thread
void* cons_worker(void* arg)
{
  return NULL;
}