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
#define DEBUG

#include "spline_interp.h"

//TODO write inverse spline

spline::spline s ;

char path[1024] = "/u/cd/jpdef/workspace/Spline/Spline/splineApp/src/";

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
  double* vala;
  char* valb;
  vala = (double*) psub->a;
  valb = (char*) psub->b;

  #ifdef DEBUG 
    printf("VAL A = %f\n",vala[0]);
    printf("VAL B = %s\n",valb);
    printf("LINKA = %s\n", psub->inpa);
    printf("LINKB = %s\n", psub->inpb);
  #endif
  if ( ! s.isInitialized() ){
    char filename[40];
    strcpy(filename,valb);
    strcat(path,filename);
  #ifdef DEBUG 
    printf("Subroutine Initialized\n");
    printf("%s\n",path);
  #endif
    //B field contians name of file
    s =  spline ( path  );
  

  }else{
  #ifdef DEBUG 
    printf("Subroutine executed\n");
  #endif
  
   double ina = vala[0] ;
    //TODO find which field to assign to 
   double val[1];
   val[0]  = s.calc(ina);
  #ifdef DEBUG 
    printf("%f = F(%f)\n",val[0],ina);
  #endif
    *(double *)(psub->vala) = val[0];
  }
  

  return 0;    
}

epicsRegisterFunction(splineIt);
