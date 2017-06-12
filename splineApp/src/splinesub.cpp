/*Standard libs*/
#include <stdio.h>
#include <stdarg.h>
#include <string>

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
* $IOC_DATA/$IOC/measurements/filename [default]
* ./filename                           [backup]
*
*/
static std::string  makePath(const char* filename_c_str){
    const char* ioc_c_str = getenv("IOC");
    const char* ioc_data_c_str = getenv("IOC_DATA");
    if (ioc_c_str && ioc_data_c_str){
       std::string ioc_data(ioc_data_c_str);
       std::string ioc(ioc_c_str);
       std::string filename(filename_c_str);
       std::string path = ioc_data + "/" + ioc + "/measurements/" + filename;
       return path;
    } else {
       return std::string(filename_c_str);
    }
}


/*
*
* initSplines(aSubRecord* psub){
*   - constructs spline object from datafiles 
*     places it into container where spline can be
*     matched with a specific record
*/
static void initSplines(aSubRecord* psub){
    char* inpb;
    inpb = (char*) psub->b;
    std::string path = makePath(inpb);
    
    
    scon.push_back( std::make_pair( psub, spline(path) ) );
}

/*
* spline getSplineFromContainer(aSubRecord* psub){
*     -retrieves spline objects from container.
*      record address indicates which spline to grab
*/
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
  spline s;
  s = getSplineFromContainer(psub);
  
  //Cast EPICS fields to correct types and grab transformation
  //that is paired with the record
  double* inpa; char* inpb; int* inpc;
  double out[1];
  inpa = (double*) psub->a;
  inpb = (char*) psub->b;
  inpc = (int* ) psub->c;

  
  /*If this is first call then initialize
  the spline*/
  if ( ! s.is_initialized() ) {
    try{
          initSplines(psub);
    }catch (int e) {
      if( e < 0 ) {
        return e;
      }
    } catch (alglib::ap_error a) {
      return -1;
    }

  } else {
    /*Field C indicates whether process is inverse or forward*/
    double in = inpa[0] ;
    double isInverse = inpc[0];

    /*Calculate output, then set value a to the output*/
    out[0] = (isInverse) ? s.calc_inv(in) : s.calc(in);
    *(double *)(psub->vala) = out[0];
  
  }
  
  return 0;    
}

epicsRegisterFunction(splineIt);
