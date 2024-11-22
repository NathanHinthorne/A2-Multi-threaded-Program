# Multi-threaded Matrix Multiplication

## Tasks

- [x] Task 1 - Implement a bounded buffer. This will be a buffer of pointers to Matrix structs (records). The datatype should be “Matrix ** bigmatrix”, and the bounded buffer will be limited to BOUNDED_BUFFER_SIZE size. Note: the demo code has it in the pcmatrix.c and similar idea can be borrowed.

- [x] Task 2 – Implement get() and put() routines for the bounded buffer.

- [ ] Task 3 – Call put() from within prod_worker() and add all necessary uses of mutex locks, condition variables, and signals. Integrate the counters.

- [ ] Task 4 – Call get() from within cons_worker() and all necessary uses of mutex locks, condition variables, and signals. Integrate the counters. Implement the matrix multiplication by consuming matrices from the bounded buffer as described above.  

- [ ] Task 5 – Create one producer pthread and one consumer pthread in pcmatrix.c to launch the parallel matrix production and multiplication.  

- [ ] Tasks 6- Once a 1 producer and 1 consumer version of the program is working correctly, refactor pcmatrix.c to use an array of producer threads, and an array of consumer threads. The array size is numw. (Extra credit for correct implementation of 3 or more producer/consumer pthreads).

## Citations

- Chatgpt gave us this command to complie code and link the object files: gcc -pthread -I. -Wall -Wno-int-conversion -D_GNU_SOURCE -fcommon counter.c prodcons.c matrix.c pcmatrix.c -o pcmatrix

- Originally, we were grabbing two matrices on every step of the while loop in prod_worker(). Claude.ai gave us code for changing prod_worker() to grab one matrix at a time.

- After writing nearly the entire `prod_worker` and `cons_worker` functions from scratch, we used Claude.ai to simplify parts of the code.

## Credits

Developed by Nathan Hinthorne and Caleb Krauter
