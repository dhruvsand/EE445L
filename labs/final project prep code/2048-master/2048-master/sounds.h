// Sounds for Lab 10
// sounds.h
// all sampled at 11.025 kHz

// 1) Wave files downloaded from http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
//    these .wav files are in the Sounds folder
// 2) Matlab used to open file and convert to numbers versus time
// 3) Excel used to convert numbers to integers for the DAC
//    these .xlsx files are in the Sounds folder
// to create sounds output these numbers to DAC at 11.025 kHz

// these constants are for a 4-bit DAC
// if you have 5,6,7,8 bits, open the .xlsx files change the number of bits
// and recopy the constants

/* This example accompanies the book
   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2012
 Copyright 2012 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#ifndef SOUNDS_H
#define SOUNDS_H
const unsigned char fastinvader1[982] = {
  8, 7, 6, 4, 3, 1, 1, 1, 1, 2, 3, 4, 6, 7, 8, 9, 10, 12, 12, 14, 
  14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 12, 12, 11, 
  10, 10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 4, 4, 3, 4, 4, 4, 5, 5, 6, 
  6, 6, 6, 5, 4, 4, 3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 
  7, 8, 8, 9, 9, 10, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 
  12, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5, 
  5, 4, 4, 4, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 
  3, 4, 5, 5, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 7, 
  6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
  1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3, 4, 4, 6, 7, 
  7, 8, 9, 9, 10, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 14, 13, 13, 12, 12, 12, 11, 10, 10, 10, 9, 9, 8, 8, 
  8, 8, 7, 7, 7, 7, 6, 6, 4, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 1, 2, 2, 3, 4, 5, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 7, 7, 
  7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  1, 2, 3, 3, 4, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 13, 14, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 11, 11, 10, 10, 10, 9, 
  9, 8, 8, 8, 8, 7, 7, 7, 6, 6, 6, 6, 4, 3, 1, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 8, 9, 9, 9, 9, 9, 9, 
  9, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 5, 4, 3, 3, 3, 2, 2, 1, 1, 
  1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3, 3, 2, 2, 2, 1, 1, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 1, 2, 3, 3, 4, 5, 6, 7, 8, 8, 9, 10, 10, 12, 12, 13, 14, 14, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 11, 11, 11, 10, 
  9, 9, 9, 8, 8, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 3, 
  3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 2, 2, 2, 2, 
  2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 3, 4, 4, 4, 4, 4, 4, 5, 
  4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 6, 7, 7, 7, 7, 7, 7, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 8, 9, 9, 9, 9, 9, 9, 10, 9, 
  10, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 10, 11, 11, 11, 10, 
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 
  11, 11, 11, 11, 10, 10, 11, 10, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 9, 10, 10, 10, 10, 9, 10, 10, 10, 9, 9, 9, 9, 9, 9, 
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 9, 9, 9, 8, 9, 8, 8, 8, 
  9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 8, 7, 8, 7, 7, 
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
  7, 7};

#endif
  


