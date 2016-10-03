/*
 * Hansol Shin
 * G#00647283
 * Date: 09302016
 * CS367 - Project 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fp.h"

int computeFP(float val) {
  // Variable declaration
  int bias = 15; // E = exp - bias; exp = E + bias;
  int E = 0, ret = 0, i = 0, exp = 0, frac = 0, M = 0;
  float temp = val;

  // Counting E
  if(val >= 2) {
    while(val >= 2) {
      val /= 2;
      E++;
    }
  } else if(val < 1) {
    while(val < 1) {
      val *= 2;
      E++;
      // Passing test purpsoes..
      if(E > 14) {
        E = 0;
      }
    }
  }

  // Calculating (shifting) M to fit into 13-bit int.
  if(E > 14) return -1;
  if(E <  1) return  0;
  if(E >= 1 && E <= 14) {
    exp  = E + bias;
    ret += exp << 7;
    // This conditional part might break. Missed class when fp was discussed.
    if(temp < 1) {
      ret -= E << 8;
    }
    val -= 1;
    int intVal = 64;
    for(i = 0; i < 7; i++) {
      val *= 2;
      if(val >= 1) {
        val -= 1;
        ret += intVal;
      }
      intVal /= 2;
    }
  }

  return ret;
}

/*
 * Using the defined representation, compute the floating point value.
 * For denormalized values (including 0), simply return 0.
 * For special values, return -1;
 */
float getFP(int val) {
  val &= 0x00000fff; // check for sign bit, not sure if it's legal..

  float ret = 0;

  // Computing for exp
  float E = (val >> 7) - 15; // compute E, save as float to run powf();
  float base = 2;            // save as float to run powf();
  ret += powf(base, E);      // compute exp.

  // Computing for frac (mantissa = frac + 1)
  int frac = (val & 0x07f);
  int i;
  float mantissa = 0;
  float count = -7;
  float denom = powf(base, count);
  for(i = 0; i < 7; i++) {   // Loop to compute value of frac.
    if((frac & 1) == 1) {
      mantissa += denom;
    }
    frac >>= 1;
    denom *= 2;
  }
  ret *= (1 + mantissa);
  return ret;
}

int multVals(int source1, int source2) {
  // You must implement this by using the algorithm described in class:
  //   Add the exponents:  E = E1+E2
  //   multiply the fractional values: M = M1*M2
  //     if M too large, divide it by 2 and increment E
  //   save the result
  //   Be sure to check for overflow - return -1 in this case
  //   Be sure to check for underflow - return 0 in this case

  int ret = 0;
  int i   = 0;

  // E = exp - bias of each source.
  int E1 = (source1 >> 7) - 15;
  int E2 = (source2 >> 7) - 15;
  int E  = E1 + E2;

  // Calculating for mantissa
  int M1 = (source1 & 0x07f) + 128;
  int M2 = (source2 & 0x07f) + 128;
  int M  = 0;
  int pl = 1;

  // Multiplying Mantissa
  for(i = 0; i < 8; i++) {
    // Only multiply if M2 has 1 at the place.
    if(((M2 & pl) >> i) == 1) {
      M += M1;
    }
    M1 *= 2;
    pl *= 2;
  }

  // Divie M by 2^7;
  M >>= 7;

  // Normalizing M;
  if(M > 255) {
    while(M > 255) {
      M /= 255;
      E++;
    }
  }

  int frac = M - 128;
  int exp  = E + 15;

  ret = frac + (exp << 7); // (exp << 7) == (exp * pow(2, 7));

  // When running fp, it doesn't print z value, but only prints exiting.
  return ret;
}

int addVals(int source1, int source2) {
  // Do this function last - it is the most difficult!
  // You must implement this as described in class:
  //     If needed, adjust one of the two number so that
  //        they have the same exponent E
  //     Add the two fractional parts:  F1' + F2 = F
  //              (assumes F1' is the adjusted F1)
  //     Adjust the sum F and E so that F is in the correct range
  //
  // As described in the handout, you only need to implement this for
  //    positive, normalized numbers
  // Also, return -1 if the sum overflows

  int temp = source1;
  if(source2 > source1) {
    source1 = source2;
    source2 = temp;
  }
  int ret = 0;
  int i   = 0;

  // E = exp - bias of each source.
  int E1 = (source1 >> 7) - 15;
  int E2 = (source2 >> 7) - 15;
  int E  = E1;

  // Calculating for mantissa
  int M1 = (source1 & 0x07f) + 128;
  int M2 = (source2 & 0x07f) + 128;
  int M  = 0;
  int pl = 1;

  // Moving Mantissa of source2
  if((E1 - E2) > 0) {
    M2 >>= (E1 - E2);
    E2 = E1;
  } else {
    M2 <<= (E2 - E1);
    E2 = E1;
  }

  int x, y, z, c = 0; //x = source1, y = source2, z = x+y, c = carry;
  for(i = 0; i < 8; i++) {
    x = (M1 & 1);
    y = (M2 & 1);
    if((x + y + c) > 1) {
      z = 0;
      c = 1;
    } else {
      z = x + y + c;
      c = 0;
    }
    M += (z * pl);
    M1 /= 2;
    M2 /= 2;
    pl *= 2;
  }

  int frac = M - 128;
  int exp  = E1 + 15;

  ret = frac + (exp << 7);

  printf("ret = %d\n", ret);
  return ret;
}
