#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  /* define bool */

#include "byutr.h"


#define OOPS 5

/*
 * Example of a program that uses the Brigham Young University
 * trace reader.  You do not need this program, but you will
 * need to do what's in it to make your program work.

 * DO NOT INCLUDE THIS IN YOUR PROGRAM
 * USE IT FOR AN EXAMPLE OF HOW TO READ THE TRACE FILE
 * gcc -std=c11 -o sample_read sample_read.c byu_tracereader.c
 */

int main(int argc, char **argv) {
  /* hardcoded to a specific filename, you need to handle this
   * based on your input in argc/argv
   */
  FILE *fp = fopen("trace.sample.tr", "r");

  if (fp == NULL) {
    /* couldn't open, bail gracefully */
    printf("uh oh\n");
    exit(OOPS);
  }

  /* Start reading addresses */
  p2AddrTr trace_item;  /* Structure with trace information */
  bool done = false;
  while (! done) {
    // Grabe the next address
    int bytesread = NextAddress(fp, &trace_item);
    // Check if we actually got something
    done = bytesread == 0;
    if (! done) 
      printf("Address %x\n", trace_item.addr);
  }
}
