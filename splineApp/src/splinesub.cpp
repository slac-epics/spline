/*Standard libs*/
#include<stdlib.h>
#include<stdio.h>
#include<stdarg.h>

/*EPICS libs*/
#include <iocsh.h>
#include <epicsExport.h>
#include <registryFunction.h>
#include <dbDefs.h>
#include <aSubRecord.h>
#include <dbCommon.h>
#include <recSup.h>
#include <dbAccess.h>

/*Local libs*/
#include "spline_interp.h"




typedef std::vector< std::pair<aSubRecord*,spline::spline> > SplineContainer;

/*Main data structure*/
SplineContainer scon;


/*
*
* makePath(struct subRecord *psub)
* -creates absolute path to data file
*
*/
static void  makePath(char* abspath, char* filename){
    //TODO if IOC_DATA/IOC fails check local dir
    char* dir1 = getenv("IOC_DATA");
    char* dir2 = getenv("IOC");
    strcat(abspath,dir1);
    strcat(abspath,"/");
    strcat(abspath,dir2);
    strcat(abspath,"/measurements/");
    strcat(abspath,filename);
}


static void initSplines(aSubRecord* psub){
    char filename[40]  = "";
    char abspath[1024] = "";
    char* inpb;

    inpb = (char*) psub->b;
    strcpy(filename,inpb);
    makePath(& (abspath[0]) , filename);
    
    debugPrintf("Subroutine Initialized\n");
    debugPrintf("%s\n",abspath);

    scon.push_back( std::make_pair( psub, spline(abspath) ) );
}


static spline getSplineFromContainer(aSubRecord* psub){
  spline s;
   SplineContainer::iterator it = scon.begin();
    for (; it !=scon.end() ; ++it ){
       if ((*it).first == psub){
          return (*it).second; 
       }
    }
    return s;
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
  debugPrintf("Subroutine called\n");
  spline s;
  
  //Cast EPICS fields to correct types and grab transformation
  //that is paired with the record
  double* inpa; char* inpb; int* inpc;
  double out[1];
  inpa = (double*) psub->a;
  inpb = (char*) psub->b;
  inpc = (int* ) psub->c;
  s = getSplineFromContainer(psub);

  debugPrintf("VAL A = %f\nVAL B = %s\nVAL C = %d\n", inpa[0],inpb,inpc[0]);
  debugPrintf("LINKA = %s\nLINKB = %s\n", psub->inpa,psub->inpb);
  
  /*If this is first call then initialize
  the spline*/
  if ( ! s.isInitialized() ) {
    initSplines(psub);
  } else {
    debugPrintf("Subroutine executed\n");
    double in = inpa[0] ;
    double isInverse = inpc[0];

    /*Calculate output, then set value a to the output*/
    out[0] = (isInverse) ? s.calcInv(in) : s.calc(in);
    debugPrintf("%f = F(%f)\n",out[0],in);
    *(double *)(psub->vala) = out[0];
  
  }
  
  return 0;    
}

epicsRegisterFunction(splineIt);
