
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "coeff.h" 
#include "ISR.h"

int buffer[1000];
void ProcessBuffer(void)
///////////////////////////////////////////////////////////////////////
// Purpose:   Processes the data in buffer[ready_index] and stores
//            the results back into the buffer 
//            Data is packed into the buffer, alternating right/left
//              Be careful of the order of packing
//
// Input:     None
//
// Returns:   Nothing
//
// Calls:     Nothing
//
// Notes:     None
///////////////////////////////////////////////////////////////////////
{   
		int BUFFER_COUNT = 9999;
	
	int ready_index = 0;
	int buffer_ready = 0;
	
    int *pBuf = &buffer[ready_index];
    // extra buffer room for convolution "edge effects"
    // N is filter order from coeff.h
    static float Left[1000]={0}, Right[1000]={0};
    float *pL = Left, *pR = Right;
    float yLeft, yRight;
    uint32_t i, j, k;
    
    // offset pointers to start filling after N elements
    pR += N;
    pL += N;

    for(i = 0;i < BUFFER_COUNT;i++) { // extract data to float buffers
    // order is important here: must go right first then left
       *pR++ = *pBuf++;
       *pL++ = *pBuf++;
    }

    // reinitialize pointer before FOR loop
    *pBuf = buffer[ready_index];
      
////////////////////////////////////////
// Implement FIR filter
// Ensure COEFF.C is part of project
////////////////////////////////////////  
   for(i=0;i < BUFFER_COUNT;i++){ 
      yLeft  = 0;                      // initialize the LEFT output value
      yRight = 0;                      // initialize the RIGHT output value
      
      for(j=0,k=i+N;j <= N;j++,k--){
        yLeft  += Left[k] * B[j];        // perform the LEFT dot-product
        yRight += Right[k] * B[j];        // perform the RIGHT dot-product
      }
      
      // pack into buffer after bounding (must be right then left)
      //*pBuf++ = _spint(yRight * 65536) >> 16;
      //pBuf++ = _spint(yLeft * 65536) >> 16;
   }  
  
   // save end values at end of buffer array for next pass
   //  by placing at beginning of buffer array
   for(i=BUFFER_COUNT,j=0;i < BUFFER_COUNT+N;i++,j++){ 
      Left[j]=Left[i];
      Right[j]=Right[i];
   }

//////// end of FIR routine ///////////  

    // reinitialize pointer
    *pBuf = buffer[ready_index];

    buffer_ready = 0; // signal we are done
}