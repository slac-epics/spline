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

#include<spline_interp.h>

spline::Spline s;

/*
*
* initSpline(struct subRecord *psub)
* -called by EPICS record initialization
* -parses k,gap data file and create interpolation
*
*/
long initSpline(char** filename){
  printf("Subroutine Initialized\n");
   
  std::string filename_str(filename);
  spline::Spline s(filename_str);
  return 0;
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
  double* vala;
  char** valb;
  vala = (double *) psub->a;
  valb = (char**) psub->b;
 
  #ifdef DEBUG 
    printf("VAL A = %f\n",vala[0]);
    printf("VAL B = %s\n",valb);
    printf("LINKA = %s\n", psub->inpa);
    printf("LINKB = %s\n", psub->inpb);
  #endif

  if (s.isEmpty){
    //TODOmight want to reduce function and put inline
    initSpline();

  }else{
   double eval_point;
   double* vala =  (double *)  psub->a ;
   eval_point = vala[0];
   
   //TODO find which field to assign to 
   psub-val = s.calc(eval_point);
  }
  

  return 0;    
}

//epicsRegisterFunction(initSpline);
//TODO write inverse mapp
//epicsRegisterFunction(initSplineInv);
epicsRegisterFunction(splineIt);
