# Multi-threaded Matrix Multiplication

## Tasks

- [x] Task 1 - Implement a bounded buffer. This will be a buffer of pointers to Matrix structs (records). The datatype should be “Matrix ** bigmatrix”, and the bounded buffer will be limited to BOUNDED_BUFFER_SIZE size. Note: the demo code has it in the pcmatrix.c and similar idea can be borrowed.

- [x] Task 2 – Implement get() and put() routines for the bounded buffer.

- [ ] Task 3 – Call put() from within prod_worker() and add all necessary uses of mutex locks, condition variables, and signals. Integrate the counters.
  - [ ] 1. Surround all calls to put() with a mutex lock.
  - [ ] 2. Create 2 condition variables (one for put and one for get) to synchronize the producer and consumer threads.
  - [ ] 3. Use `counters_t` struct as an argument to track production.
  - [ ] 4. One call to prod_worker() should add one matrix at a time in a loop until the buffer is full.
  - Some information found from this ChatGPT conversation: https://chatgpt.com/share/6732d9bf-db28-8013-a89e-192cf7a115d3

- [ ] Task 4 – Call get() from within cons_worker() and all necessary uses of mutex locks, condition variables, and signals. Integrate the counters. Implement the matrix multiplication by consuming matrices from the bounded buffer as described above.  

- [ ] Task 5 – Create one producer pthread and one consumer pthread in pcmatrix.c to launch the parallel matrix production and multiplication.  

- [ ] Tasks 6- Once a 1 producer and 1 consumer version of the program is working correctly, refactor pcmatrix.c to use an array of producer threads, and an array of consumer threads. The array size is numw. (Extra credit for correct implementation of 3 or more producer/consumer pthreads).

## Citations

Chatgpt gave us this command to complie code and link the object files.
gcc -pthread -I. -Wall -Wno-int-conversion -D_GNU_SOURCE -fcommon counter.c prodcons.c matrix.c pcmatrix.c -o pcmatrix

## Credits

Developed by Nathan Hinthorne and Caleb Krauter