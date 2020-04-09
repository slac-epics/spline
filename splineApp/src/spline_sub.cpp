/*Local libs*/
#include "spline_sub.h"

/* Define SUCCESS macro*/
#ifndef SUCCESS
#define SUCCESS 1
#endif

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

/* long splineInit(aSubRecord *psub) 
 * Generic spline initialization function 
 * To be used in the INAM field
*/
static long splineInit(aSubRecord *psub) {
  long status = SUCCESS;
  devicePvt_ts *dpvt_ps = NULL;
  /* Check that this pv has not already been initialized */
  if ( psub->dpvt ) return (-1);
  
  /* Allocate the private device information */
  dpvt_ps = (devicePvt_ts*)callocMustSucceed(1,sizeof(devicePvt_ts),"calloc failed for mgntIVBInit" );
  dpvt_ps->mlock = epicsMutexMustCreate();
  psub->dpvt = dpvt_ps;
  return status;
}

/*
*
* splineCalcOutput(struct subRecord *psub)
* -calls spline function and sets record value
* to interpolation at point A
* - VAL = f(A)
*
*/
static long splineCalcOutput(aSubRecord *psub){
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
*
* splineGetLimits(struct subRecord *psub)
* - retrieve max and min values of data points used to build spline 
*
*/
static long splineGetLimits(aSubRecord *psub){
  spline s;

  
  //Cast EPICS fields to correct types
  double outa[1]; double outb[1]; double outc[1]; double outd[1];
  char* inpa; 
  inpa = (char*) psub->a;
  
  s = getSplineFromContainer(std::string(inpa));
  /*If this is first call then initialize
  the spline*/
  if ( ! s.is_initialized() ) {
    try{
          printf("No such transformation %s\n",inpa);
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

    /*Calculate output, then set value a to the output*/
    /* max gap */
    outa[0] = s.get_max_Y(); 
    /* min gap */
    outb[0] = s.get_min_Y(); 
    /* max K */
    outc[0] = s.get_max_X(); 
    /* min K */
    outd[0] = s.get_min_X(); 
    /* Return everything */
    *(double *)(psub->vala) = outa[0];
    *(double *)(psub->valb) = outb[0];
    *(double *)(psub->valc) = outc[0];
    *(double *)(psub->vald) = outd[0];
    printf("Max gap: %f, min gap: %f, max K: %f, min K: %f\n", outa[0], outb[0], outc[0], outd[0]); 
  }
  
  return 0;    
}

/*
*
* splineGetNumPoints(struct subRecord *psub)
* - get number of data points used to buld spline 
*
*/
static long splineGetNumPoints(aSubRecord *psub){
  spline s;

  
  //Cast EPICS fields to correct types
  //epicsInt32 outa[1]; 
  epicsInt32 numDataPoints; 
  char* inpa; 
  inpa = (char*) psub->a;
   
  s = getSplineFromContainer(std::string(inpa));
  /*If this is first call then initialize
  the spline*/
  if ( ! s.is_initialized() ) {
    try{
          printf("No such transformation %s\n",inpa);
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
    /*Fetch number of data points*/
    //outa[0] = s.get_num_points(); 
    //*(epicsInt32 *)(psub->vala) = outa[0];
    numDataPoints = s.get_num_points(); 
    *(epicsInt32 *)(psub->vala) = numDataPoints;
    printf("Num points: %d\n", numDataPoints); 
  }
  
  return 0;    
}

/*
*
* splineGetPoints(struct subRecord *psub)
* - get data points from input file (points stored in memory used for interpolation)
*
*/
static long splineGetPoints(aSubRecord *psub){
  spline s;
  // Pointers to hold the returned values
  double* xpts = NULL; 
  double* ypts = NULL; 
  int npts = 0;
  int xnpts = 0;
  int ynpts = 0;
  devicePvt_ts  *dpvt_ps = NULL;
  size_t         nbytes = sizeof(double) * psub->nova;

  //Cast EPICS fields to correct types
  char* inpa; 
  inpa = (char*) psub->a;
  dpvt_ps = (devicePvt_ts *)psub->dpvt;
   s = getSplineFromContainer(std::string(inpa));
  /*If this is first call then initialize
  the spline*/
  if ( ! s.is_initialized() ) {
    try{
          printf("No such transformation %s\n",inpa);
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
     /* clear waveform */
     epicsMutexMustLock(dpvt_ps->mlock); 
     memset(psub->vala,0,nbytes);
     epicsMutexUnlock(dpvt_ps->mlock);
     psub->neva=0;
     
     epicsMutexMustLock(dpvt_ps->mlock); 
     memset(psub->valb,0,nbytes);
     epicsMutexUnlock(dpvt_ps->mlock);
     psub->nevb=0;
     
     /* Fetch the data points */
     s.get_X_array(xpts, &xnpts);
     s.get_Y_array(ypts, &ynpts);
     /* Make sure we have retrieved the same number of points from the two arrays*/
    if(xnpts == ynpts) {
        npts = xnpts;
     }
     else {
        printf("X and Y number of data points differ. Exiting. xnpts: %d, ynpts: %d\n", xnpts, ynpts);
        return 1;
     }
     /* Assign data points to the output*/
     for (int i = 0; i < npts; i++) {
         epicsMutexMustLock(dpvt_ps->mlock)
         ((double*)psub->vala)[i] = xpts[i];
         epicsMutexUnlock(dpvt_ps->mlock);
     }
     psub->neva = npts;
     
     /* Assign data points to the output*/
     for (int i = 0; i < npts; i++) {
         epicsMutexMustLock(dpvt_ps->mlock)
         ((double*)psub->valb)[i] = ypts[i];
         epicsMutexUnlock(dpvt_ps->mlock);
     }
     psub->nevb = npts;
     
   }
  
   return 0;    
}

/*
*
*splineGetDate(struct subRecord *psub)
* - get date from calibration file 
*
*/
static long splineGetDate(aSubRecord *psub){
  spline s;
  // Pointers to hold the returned values
  char* date = NULL; 

  //Cast EPICS fields to correct types
  char* inpa; 
  inpa = (char*) psub->a;
  
  s = getSplineFromContainer(std::string(inpa));
  /*If this is first call then initialize
  the spline*/
  if ( ! s.is_initialized() ) {
    try{
          printf("No such transformation %s\n",inpa);
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
     
     /* Fetch the date */
     s.get_date(date);
     /* Assign date to the output*/
    for(int i = 0; i < DATEFILE_CHARS; i++) {
      if(date[i]  == '\0' || date[i] == '\r') {
         break;
      }
      else {
        ((char*)psub->vala)[i]  = date[i];
      }
    }
   }
  
   return 0;    
}

/* 
 * splineGetInpPrms(struct subRecord *psub)
 * - Get all input parameters that make up the spline used for interpolation
 * - OUTA: Number of data points (LONG)
 * - OUTB: Data points in X array (DOUBLE - waveform)
 * - OUTC: Data points in Y array (DOUBLE - waveform)
 * - OUTD: Date when the calibration file was saved (STRING)
*/ 
static long splineGetInpPrms(aSubRecord *psub){
  spline s;
  // Pointers to hold the returned values
  char* date = NULL; 
  epicsInt32 numDataPoints; 
  double* xpts = NULL; 
  double* ypts = NULL; 
  int npts = 0;
  int xnpts = 0;
  int ynpts = 0;
  devicePvt_ts  *dpvt_ps = NULL;
  size_t nbytes = sizeof(double) * psub->nova;

  //Cast EPICS fields to correct types
  char* inpa; 
  inpa = (char*) psub->a;
  
  dpvt_ps = (devicePvt_ts *)psub->dpvt;
  s = getSplineFromContainer(std::string(inpa));
  /*If this is first call then initialize
  the spline*/
  if ( ! s.is_initialized() ) {
    try{
          printf("No such transformation %s\n",inpa);
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
  
  /* Fetch the number of data points */   
  numDataPoints = s.get_num_points(); 
  *(epicsInt32 *)(psub->vala) = numDataPoints;
  printf("Num points splineGetInpParams: %d\n", numDataPoints); 
     
  /* Fetch the data points */
  /* clear waveform */
  epicsMutexMustLock(dpvt_ps->mlock); 
  memset(psub->valb,0,nbytes);
  epicsMutexUnlock(dpvt_ps->mlock);
  psub->nevb=0;
     
  epicsMutexMustLock(dpvt_ps->mlock); 
  memset(psub->valc,0,nbytes);
  epicsMutexUnlock(dpvt_ps->mlock);
  psub->nevc=0;
     
  /* Fetch the data points */
  s.get_X_array(xpts, &xnpts);
  s.get_Y_array(ypts, &ynpts);
  /* Make sure we have retrieved the same number of points from the two arrays*/
  if(xnpts == ynpts) {
      npts = xnpts;
  }
  else {
     printf("X and Y number of data points differ. Exiting. xnpts: %d, ynpts: %d\n", xnpts, ynpts);
     return 1;
  }
  /* Assign data points to the output*/
  for (int i = 0; i < npts; i++) {
      epicsMutexMustLock(dpvt_ps->mlock)
      ((double*)psub->valb)[i] = xpts[i];
      epicsMutexUnlock(dpvt_ps->mlock);
  }
  psub->nevb = npts;
     
 /* Assign data points to the output*/
 for (int i = 0; i < npts; i++) {
     epicsMutexMustLock(dpvt_ps->mlock)
     ((double*)psub->valc)[i] = ypts[i];
     epicsMutexUnlock(dpvt_ps->mlock);
 }
 psub->nevc = npts;
     
 /* Fetch the date when the file was saved */
 s.get_date(date);
 /* Assign all the character to the output*/
 for(int i = 0; i < DATEFILE_CHARS; i++) {
     if(date[i]  == '\0' || date[i] == '\r') {
         break;
      }
      else {
        ((char*)psub->vald)[i]  = date[i];
      }
    }
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


epicsRegisterFunction(splineCalcOutput);
epicsRegisterFunction(splineInit);
epicsRegisterFunction(splineGetLimits);
epicsRegisterFunction(splineGetNumPoints);
epicsRegisterFunction(splineGetPoints);
epicsRegisterFunction(splineGetDate);
epicsRegisterFunction(splineGetInpPrms);
epicsExportRegistrar(drvSplineRegistrar);
