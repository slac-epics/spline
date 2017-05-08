#include<stdlib.h>
#include<stdio.h>

#include <iocsh.h>
#include<epicsExport.h>
#include<registryFunction.h>
#include<dbDefs.h>
#include<aSubRecord.h>
#include<dbCommon.h>
#include<recSup.h>
#include<dbAccess.h>

#include "spline_interp.h"

//#define DEBUG

//TODO write inverse spline

spline::spline s ;



/*
*
* makePath(struct subRecord *psub)
* -creates absolute path to data file
*
*/
static char* makePath(char* filename){
    char* abspath = getenv("PWD");
    char* relpath = getenv("TOP");
    strcat(abspath,"/");
    strcat(abspath,relpath);
    strcat(abspath,"/splineApp/src/");
    strcat(abspath,filename);
    return abspath;
}


/*
*
* splineIt(struct subRecord *psub)
* -calls spline function and sets record value
* to interpolation at point A
* - VAL = f(A)
*
*/
static long splineIt(aSubRecord *psub){
  printf("Subroutine called\n");
  
  //Cast EPICS fields to correct types
  double* inpa;
  char* inpb;
  inpa = (double*) psub->a;
  inpb = (char*) psub->b;

  #ifdef DEBUG 
    printf("VAL A = %f\n", inpa[0]);
    printf("VAL B = %s\n", inpb);
    printf("LINKA = %s\n", psub->inpa);
    printf("LINKB = %s\n", psub->inpb);
  #endif

  /*If this is first call then initialize
  the spline*/
  if ( ! s.isInitialized() ){
    //B field contians name of file
    char filename[40];
    strcpy(filename,inpb);
    char* abspath = makePath(filename);
  
  #ifdef DEBUG 
    printf("Subroutine Initialized\n");
    printf("%s\n",abspath);
  #endif
    
    s =  spline ( abspath  );
  

  }else{
  
  #ifdef DEBUG 
    printf("Subroutine executed\n");
  #endif
  
   double in = inpa[0] ;
   double out[1];
   out[0]  = s.calc(in);
  #ifdef DEBUG 
    printf("%f = F(%f)\n",out[0],in);
  #endif
    *(double *)(psub->vala) = out[0];
  }
  

  return 0;    
}


epicsRegisterFunction(splineIt);
