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
#include <ap.h>
#include "iocsh.h"
#include <epicsExport.h>

/*Local libs*/
#include "spline_interp.h"

typedef std::vector< std::pair<std::string,spline::spline> > SplineContainer;

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
    const char* top_c_str = getenv("TOP");
    if (top_c_str){
       std::string top(top_c_str);
       std::string filename(filename_c_str);
       std::string path = top + "/measurements/" + filename;
       return path;
    } else {
       return std::string(filename_c_str);
    }
}


/*
* 
* initSpline(std::string trans_name, std::string filepat){
*   - constructs spline object from datafiles 
*     places it into container where spline can be
*     matched with a specific transformation name

static void initSpline(){
    std::string tname = std::string(args[0].sval);
    std::string filepath = std::string(args[1].sval);
    printf("initSpline :: Params: %s %s \n",tname,filepath);
    scon.push_back( std::make_pair( tname, spline(filepath) ) );
}*/

/*
* spline getSplineFromContainer(aSubRecord* psub){
*     -retrieves spline objects from container.
*      record address indicates which spline to grab
*/
static spline getSplineFromContainer(std::string psub){
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
  //Grab subroutine that is paired with the record
  //Get spline from field name
  s = getSplineFromContainer("placeholder");
  
  //Cast EPICS fields to correct types
  double* inpa; char* inpb; int* inpc;
  double out[1];
  inpa = (double*) psub->a;
  inpb = (char*) psub->b;
  inpc = (int* ) psub->c;

  
  /*If this is first call then initialize
  the spline*/
  if ( ! s.is_initialized() ) {
    try{
          printf("Initializing spline\n");
          //Rework now that st.cmd handles init
          //initSplines(psub);
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




static const iocshArg spline_init_arg0 = { "transform_name", iocshArgString};
static const iocshArg spline_init_arg1 = { "filename", iocshArgString};

static const iocshArg *const spline_init_args[] = {&spline_init_arg0, &spline_init_arg1};
static const iocshFuncDef spline_init_def = {"splineInit", 2, spline_init_args};
static void spline_init(const iocshArgBuf *args ){
   printf("Hello there!\n");
   std::string tname   (  args[0].sval,1024);
   std::string filepath ( args[1].sval,1024);
   printf("Filepath is  %s  \nTransformation name is %s \n",filepath.c_str(),tname.c_str());

   scon.push_back( std::make_pair( tname, spline(filepath) ) );

}

static void drvSplineRegistrar(){
    iocshRegister(&spline_init_def, spline_init);
}

epicsExportRegistrar(drvSplineRegistrar);