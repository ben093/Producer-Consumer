/*=============================================================================
 PROGRAMMER:         Yang Wang, Ben, Anthony
 COURSE:             CSC460: Operating System
 MODIFIED BY:        Ben (bb) - 50% (Counting semaphore, Debugging)
					 Anthony (av) - 50% (Binary semaphore, Debugging)
 LAST MODIFIED DATE: 4/14/2016
 MODIFIED REASON:	 There was a concurrency issue in the program which led
					 to threads consuming prior to the threads producing.
					 The buffer may have been empty, but it was still being
					 consumed.
 DESCRIPTION:        A producer thread receives the count of the products n
                     through a command-line argument and creates n products
                     (simulated with n non-negative integers generated
                     randomly) for the consumer thread to consume. The two
                     threads share a bounded buffer of size bufSize.
 NOTE:               The program is incomplete currently, and synchronization
                     between the two threads' operations are needed.
 FILES:              ProducerConsumer.cpp, showBuf.cpp, pc.h, makefile
 COMPILER:           GNU g++
 INSTRUCTION FOR COMPILATION AND EXECUTION:
     1. type: make                                      to COMPILE 
     2. type: ./pc  [productCount]                      to EXECUTE
==============================================================================*/
#include "pc.h"
#include "semaphore.h"

// global data shared by both producer and consumer threads
int const bufSize = 5;
int       buffer[bufSize];         // space to store the products


//*****************************************************************************
void* consumerThread(void* arg);

sem_t cnt_sem, bin_sem; // *bb* define semaphores

//*****************************************************************************
int main(int argCount, char* argList[]) 
{pthread_t  threadId;
 int        productCount,     // the number of the products to be produced
            aProduct,         // a product is an integer: 0~999
            in = 0;           // index for the next generated product

//--------------------------------------
// 1. Validate the command-line arguments
//--------------------------------------
 if (argCount != 2)
 {cout <<"\n************************************************\n"
       <<"The correct useage should be: " <<argList[0] <<" productCount"
       <<"\n************************************************\n\n";
  exit(0);
 }
 productCount = atoi(argList[1]);
 if (productCount < 1)
 {cout <<"\nSince the productCount " <<argList[1]
                                     <<" < 1, no product will be produced.\n\n";
  exit(0);
 }

//--------------------------------------
// 2. Other preparations
//--------------------------------------
 for (int j=0; j<bufSize; j++)   // A "-1" placed in the buffer is used as only
   buffer[j] = -1;               // a visual clue for an "empty" slot of the
                                 // buffer in the output, and it should not be
                                 // used for any condition test in the program
 showBuffer(Init, buffer, bufSize, 0, ' ');

 srand(time(NULL));              // for simulating time laps
 
 // *bb* added below if statement to initialize the counting semaphore.
 if (sem_init(&cnt_sem, 0, bufSize) != 0) {
 cout << "Semaphore initialization failed.\n";
 exit(0);
 }
 // *bb* added below if statement to initialize the binary semaphore.
 if (sem_init(&bin_sem, 0, 0) != 0) {
 cout << "Semaphore initialization failed.\n";
 exit(0);
 }

//--------------------------------------
// 3. Create the consumer thread
//--------------------------------------
 if (pthread_create(& threadId, NULL, consumerThread,  & productCount) != 0)
 {cout <<"\nError: cannot create the consumer thread.\n";
  sem_destroy(& cnt_sem); // *bb* release resource for semaphore
  sem_destroy(& bin_sem); // *bb* release resource for semaphore
  exit(0);
 }

//--------------------------------------
// 4. Producer produces productCount items
//--------------------------------------
 for (int i=0; i<productCount; i++)
 {sleep(rand()%4);               // simulate the time for producing a product
  aProduct = rand()%1000;        // products are integers: 0~999	
    sem_wait(&cnt_sem); // *bb* decrease value of counting semaphore	
	/* START CRITICAL SECTION */
  buffer[in] = aProduct;         // place the product into the buffer  
  in = (in+1) % bufSize;                    
  showBuffer(Producer, buffer, bufSize, in, '#');   // '#' marks the last 
													// in-product position
	/* END CRITICAL SECTION */					  										
	sem_post(&bin_sem); // *bb* increase value of binary semaphore
 }

//--------------------------------------
// 5. Wait for the consumer thread to finish
//--------------------------------------
 if (pthread_join(threadId, NULL) != 0)
  cout <<"Failed to wait for the consumer thread.\n";

//--------------------------------------
// 6. Wrap-up
//--------------------------------------
 cout <<"\nDone.\n";
 return 0;
}

//*****************************************************************************
void* consumerThread(void* arg)
{int productCount = * (int*) arg, // retrieve the count of the products
     out = 0;                     // index for the next to-be-consumed product   
 for (int i=0; i<productCount; i++)
 {
    sem_wait(&bin_sem); // *bb* decrease value of binary semaphore
	/* START CRITICAL SECTION */
  buffer[out] = -1;              // remove a product from the buffer
  out = (out + 1) % bufSize;
  showBuffer(Consumer, buffer, bufSize, out, '*');   // '*' marks the last
                                                     // out-product position
  sleep(rand()%4);               // simulate the time for consuming the product
    /* END CRITICAL SECTION */
    sem_post(&cnt_sem); // *bb* increase value of counting semaphore
 }
  
 sem_destroy(& cnt_sem); // *bb* release resource for counting semaphore
 sem_destroy(& bin_sem); // *bb* release resource for binary semaphore

 pthread_exit(NULL);
}

