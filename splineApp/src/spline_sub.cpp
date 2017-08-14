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
* spline getSplineFromContainer(aSubRecord* psub){
*     -retrieves spline objects from container.
*      record address indicates which spline to grab
*/
static spline getSplineFromContainer(std::string psub){
   spline s;
   SplineContainer::iterator it = scon.begin();
    for (; it !=scon.end() ; ++it ){
       if (psub.compare((*it).first) == 0){
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

  
  //Cast EPICS fields to correct types
  double* inpa; char* inpb; int* inpc;
  double out[1];
  inpa = (double*) psub->a;
  inpb = (char*) psub->b;
  inpc = (int* ) psub->c;

  /*Grab subroutine that is paired with the record
  based on tname from field b */
  s = getSplineFromContainer(std::string(inpb));


  
  /*If this is first call then initialize
  the spline*/
  if ( ! s.is_initialized() ) {
    try{
          printf("No such transformation %s\n",inpb);
          return -1;
    }catch (int e) {
      if( e < 0 ) {
          printf("Encoutered error please check data for syntax errors, and discontinuities\n");
        return e;
      }
    } catch (alglib::ap_error a) {
          printf("Encoutered error please check data for syntax errors, and discontinuities\n");
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




/*
This block sets up a interface for the ioc shell. It allows the user to intializes
their splines in the st.cmd. Each spline has a filepath and a tranformation name(tname)
When the subroutine is processed it will use the tname in field b to find which spline belongs to it
*/
static const iocshArg spline_init_arg0 = { "transform_name", iocshArgString};
static const iocshArg spline_init_arg1 = { "filename", iocshArgString};

static const iocshArg *const spline_init_args[] = {&spline_init_arg0, &spline_init_arg1};
static const iocshFuncDef spline_init_def = {"splineInit", 2, spline_init_args};
static void spline_init(const iocshArgBuf *args ){
   std::string tname   (  args[0].sval);
   std::string filepath ( args[1].sval);
   printf("Filepath is  %s  \nTransformation name is %s \n",filepath.c_str(),tname.c_str());

   scon.push_back( std::make_pair( tname, spline(filepath) ) );

}

static void drvSplineRegistrar(){
    iocshRegister(&spline_init_def, spline_init);
}


epicsRegisterFunction(splineIt);
epicsExportRegistrar(drvSplineRegistrar);