#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "structs.h"
#include "pgFunctions.h"
#include "output_mode_helpers.h"
#include "byutr.h"

bool nFlag = false, oFlag = false;
FILE *printFile;
int limit;
int mode;

int levelCount;

bool bitmaskFlag = false;
bool logical2PhysicalFlag = false;
bool page2FrameFlag = false;
bool offsetFlag = false;
bool summaryFlag = false;


void parseArguments(int argc, char **argv) {
    levelCount = argc-2;
    int c;
    while((c = getopt(argc, argv, "n:o")) != -1) { 
        switch(c) {
            case 'n':           //-n flag sets a limit to number of addresses to process
                nFlag = true;
                limit = atoi(optarg);
                levelCount -= 2;
                break;
            case 'o': 

                if((strcmp(optarg , "bitmasks") == 0)){
                    bitmaskFlag = true;
                }

                if((strcmp(optarg  , "logical2physical") == 0)){
                    logical2PhysicalFlag = true;
                }

                if((strcmp(optarg , "page2frame") == 0)){
                    page2FrameFlag = true;
                }

                if((strcmp(optarg  , "offset") == 0)){
                    offsetFlag = true;
                }

                if((strcmp(optarg  , "summary") == 0)){
                    summaryFlag = true;
                }
                levelCount -= 2;
                break;
            default:
                summaryFlag = true;
                break;
        }
    }
}

void runStart(int argc, char **argv) {
    FILE *traceFile;
    
    /*create the page table and initialize it*/
    PAGETABLE *pageTable = calloc(1, sizeof(PAGETABLE));
    pageTable->levelCount = levelCount;
    int offsetBits = ADDRESS_LENGTH - initializePageTable(pageTable, argv, (argc - levelCount));
    int pageSize = pageByteSize(offsetBits);

    p2AddrTr trace;     //this struct contains address when fetched from trace file
    if ((traceFile = fopen(argv[argc-levelCount-1],"rb")) == NULL) {
        fprintf(stderr,"cannot open %s for reading\n",argv[argc-levelCount-1]);
        exit(1);
    }
    

    unsigned long addressCount = 0;
    unsigned int address = (unsigned int)trace.addr;
    
    int offsetMask = 0;

    for(int i =0; i < levelCount; i++){
        offsetMask = offsetMask | pageTable->bitmaskArray[i];
    }
    offsetMask = ~offsetMask;

    while (!feof(traceFile)) {
        if (nFlag)
            if (addressCount >= limit)      //break out of loop if limit is reached
                break;
        if (NextAddress(traceFile, &trace)) {
            unsigned int address = (unsigned int)trace.addr;
            uint32_t pages[levelCount];

            pageInsert(pageTable, address, pageTable->frameCount);

            for(int j = 0; j < levelCount; j++){
                pages[j] = logicalToPage(address, pageTable->bitmaskArray[j], pageTable->shiftArray[j]);
            }

            if(offsetFlag){
                report_logical2offset(address, address & offsetMask);
            }

            
            if (page2FrameFlag) {
                MAP *map = pageLookup(pageTable, address);
                report_pagemap(address,levelCount, pages, map->frame);
            } 

            
            if (logical2PhysicalFlag) {
                MAP *map = pageLookup(pageTable, address);
                unsigned int translated = address;
                translated &= ((1 << offsetBits) - 1);
                translated += (map->frame << (offsetBits));
                report_logical2physical(address, translated);
            }

            
           

        }
        addressCount++;
    }
    
    int byteAmount = addressCount;

    if (bitmaskFlag){
        report_bitmasks(levelCount, pageTable->bitmaskArray);
    }

    if(summaryFlag){
        report_summary(pageSize, pageTable->hits, addressCount, pageTable->frameCount, pageTable->bytesUsed);
    }

}

int main(int argc, char **argv) {


    parseArguments(argc, argv);

    runStart(argc, argv);

    return 0;
}