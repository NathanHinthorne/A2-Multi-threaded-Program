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

Matrix** buffer;
int count = 0;
int topIndex = 0;

// Probably init in main.
Matrix** initBoundedBuffer() {

  buffer = (Matrix**)malloc(sizeof(Matrix*) * BOUNDED_BUFFER_SIZE);
  for (int n = 0; n < BOUNDED_BUFFER_SIZE; n++) {
    buffer[n] = (Matrix*)malloc(sizeof(Matrix));
  }
  printf("Initialized buffer!");
  printf("BOUNDED_BUFFER_SIZE=%d", BOUNDED_BUFFER_SIZE);
  return buffer;
}

// Define Locks, Condition variables, and so on here

// Bounded buffer put() get()
int put(Matrix* value)
{
  buffer[topIndex] = value;

  DisplayMatrix(value, stdout);

  topIndex = (topIndex + 1) % BOUNDED_BUFFER_SIZE;
  count += 1;
}

Matrix* get()
{
  assert(count > 0); // there must be at least 1 matrix to retrieve
  count -= 1;
  return buffer[count]; // probably wrong
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