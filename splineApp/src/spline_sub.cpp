/* spline_sub.cpp: functions to be called by EPICS aSub records to
 * interface spline implementation with EPICS. 
 *
 * NOTE: If at initialization
 * time the spline is not built correctly, the s.is_initialized() 
 * function returns FALSE. This is used to prevent processing of the
 * functions at this level. If such fault occurs, the BRSV field
 * of the aSub PV using the function is set to 3 = INVALID.
 * User should check this field */

/*Local libs*/
#include "spline_sub.h"

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
static epicsInt32 splineInit(aSubRecord *psub) {
  epicsInt32 status = SPL_SUCCESS;
  devicePvt_ts *dpvt_ps = NULL;
  /* Check that this pv has not already been initialized */
  if ( psub->dpvt ) return (SPL_BAD_PTR);
  
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
static epicsInt32 splineCalcOutput(aSubRecord *psub){
  spline s;

  
  //Cast EPICS fields to correct types
  double* in; char* tnam; 
  epicsInt32* isInverse;
  epicsInt32 debug = 0;
  double out;
  // Input value to transformation
  in = (double*) psub->a;
  // Transformation name
  tnam = (char*) psub->b;
  // Transformation direction
  isInverse = (epicsInt32* ) psub->c;

  // See if debugging is enabled
  debug = *(epicsInt32* )psub->d;

  try{
	  /*Grab subroutine that is paired with the record
	  based on tname from field b */
	  s = getSplineFromContainer(std::string(tnam));

	  /*If this is first call then initialize
	  the spline*/
	  if ( ! s.is_initialized() ) {
		  recGblRecordError(S_dev_badInit, (void*)psub, "splineCalcOutput: no transformation");
		  psub->brsv = INVALID_ALARM;
                  return SPL_NO_TRANS;
	  }
	  else {
	          /* Make sure that the input value is in range */
		  if(s.check_input_inRange(*in, *isInverse)) {
        	      /*Calculate output, then set value a to the output*/
		      out = (*isInverse) ? s.calc_inv(*in) : s.calc(*in);
		      if(debug) {
                         printf("%s splineCalcOutput: out = %f\n", psub->name, out);
                      }
		      *(double *)(psub->vala) = out;
		  }
                  /* The input value is not in range */
		 else {
		     recGblRecordError(S_dev_badArgument, (void*)psub, "splineCalcOutput: input out of range");
		     psub->brsv = INVALID_ALARM;
                     return SPL_BAD_DATA;
		 } 
	  }
    } catch (alglib::ap_error a) {
        recGblRecordError(S_dev_badRequest, (void*)psub, "splineCalcOutput: alglib error");
        psub->brsv = MAJOR_ALARM;
        return SPL_ALGLIB_ERR;
    }catch (std::exception& e) {
        errlogSevPrintf(errlogMajor, "%s exception: %s", psub->name, e.what());
        return SPL_BAD_DATA;
    }
  return SPL_SUCCESS;    
}


/*
*
* splineGetLimits(struct subRecord *psub)
* - retrieve max and min values of data points used to build spline 
*
*/
static epicsInt32 splineGetLimits(aSubRecord *psub){
  spline s;

  
  //Cast EPICS fields to correct types
  double maxY; double minY; double maxX; double minX;
  char* tnam;
  epicsInt32 debug = 0; 
  tnam = (char*) psub->a;
  debug = *(epicsInt32*) psub->b;  

  try{
	s = getSplineFromContainer(std::string(tnam));
	/*If this is first call then initialize
	the spline*/
	if ( ! s.is_initialized() ) {
            recGblRecordError(S_dev_badInit, (void*)psub, "splineGetLimits: no transformation");
            psub->brsv = INVALID_ALARM;
            return SPL_NO_TRANS;
        }
	else {
	    /*Calculate output, then set value a to the output*/
	    /* max gap */
	    maxY = s.get_max_Y(); 
	    /* min gap */
	    minY = s.get_min_Y(); 
	    /* max K */
	    maxX = s.get_max_X(); 
	    /* min K */
	    minX = s.get_min_X(); 
	    /* Return everything */
	    *(double *)(psub->vala) = maxY;
	    *(double *)(psub->valb) = minY;
	    *(double *)(psub->valc) = maxX;
	    *(double *)(psub->vald) = minX;
            if(debug) {
	        printf("%s splineGetLimits: Max Y: %f, min Y: %f, max X: %f, min X: %f\n", psub->name, maxY, minY, maxX, minX); 
            }
	 }
  } catch (alglib::ap_error a) {
      recGblRecordError(S_dev_badRequest, (void*)psub, "splineGetLimits: alglib error");
      psub->brsv = MAJOR_ALARM;
      return SPL_ALGLIB_ERR;
  }catch (std::exception& e) {
      errlogSevPrintf(errlogMajor, "%s exception: %s", psub->name, e.what());
      return SPL_BAD_DATA;
  }
  return SPL_SUCCESS;    
}

/*
*
* splineGetNumPoints(struct subRecord *psub)
* - get number of data points used to buld spline 
*
*/
static epicsInt32 splineGetNumPoints(aSubRecord *psub){
  spline s;

  
  //Cast EPICS fields to correct types
  epicsInt32 numDataPoints; 
  char* tnam;
  epicsInt32 debug = 0; 
  tnam = (char*) psub->a;
  debug = *(epicsInt32*) psub->b;
   
  try{
	  s = getSplineFromContainer(std::string(tnam));
	  /*If this is first call then initialize
	  the spline*/
	  if ( ! s.is_initialized() ) {
		recGblRecordError(S_dev_badInit, (void*)psub, "splineGetNumPoints: no transformation");
		psub->brsv = INVALID_ALARM;
		return SPL_NO_TRANS;
	  }
	  else {
	       /*Fetch number of data points*/
	       numDataPoints = s.get_num_points(); 
	       *(epicsInt32 *)(psub->vala) = numDataPoints;
               if(debug) {
	           printf("%s splineGetNumPoints num points: %d\n",psub->name, numDataPoints); 
	       }
	  }
  } catch (alglib::ap_error a) {
      recGblRecordError(S_dev_badRequest, (void*)psub, "splineGetNumPoints: alglib error");
      psub->brsv = MAJOR_ALARM;
      return SPL_ALGLIB_ERR;
  }catch (std::exception& e) {
      errlogSevPrintf(errlogMajor, "%s exception: %s", psub->name, e.what());
      return SPL_BAD_DATA;
  }

  return SPL_SUCCESS;    
}

/*
*
* splineGetPoints(struct subRecord *psub)
* - get data points from input file (points stored in memory used for interpolation)
*
*/
static epicsInt32 splineGetPoints(aSubRecord *psub){
  spline s;
  // Pointers to hold the returned values
  double* xpts = NULL; 
  double* ypts = NULL; 
  epicsInt32 npts = 0;
  epicsInt32 xnpts = 0;
  epicsInt32 ynpts = 0;
  devicePvt_ts  *dpvt_ps = NULL;
  size_t         nbytes = sizeof(double) * psub->nova;
  epicsInt32 debug = 0;

  //Cast EPICS fields to correct types
  char* tnam; 
  tnam = (char*) psub->a;
  debug = *(epicsInt32*) psub->b;
  dpvt_ps = (devicePvt_ts *)psub->dpvt;
  try{
	s = getSplineFromContainer(std::string(tnam));
	/*If this is first call then initialize
	the spline*/
	if ( ! s.is_initialized() ) {
	     recGblRecordError(S_dev_badInit, (void*)psub, "splineGetPoints: no transformation");
             psub->brsv = INVALID_ALARM;
	     return SPL_NO_TRANS;
	}
	else {
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
		printf("%s splineGetPoints: X and Y number of data points differ. Exiting. xnpts: %d, ynpts: %d\n", psub->name, xnpts, ynpts);
		psub->brsv = MAJOR_ALARM;
		return SPL_BAD_DATA;
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
             /* Print out data points */
             if(debug) {
                 printf("%s splineGetPoints X points: ...\n", psub->name);
                 for(int i=0; i<npts; i++) {
		    printf("%f ", xpts[i]);
                 }
                 printf("\n");
                 printf("%s splineGetPoints Y points: ...\n", psub->name);
                 for(int i=0; i<npts; i++) {
		    printf("%f ", ypts[i]);
                 }
                 printf("\n");
             }
	     
	   }
  } catch (alglib::ap_error a) {
      recGblRecordError(S_dev_badRequest, (void*)psub, "splineGetPoints: alglib error");
      psub->brsv = MAJOR_ALARM;
      return SPL_ALGLIB_ERR;
  }catch (std::exception& e) {
      errlogSevPrintf(errlogMajor, "%s exception: %s", psub->name, e.what());
      return SPL_BAD_DATA;
  }


   return SPL_SUCCESS;    
}

/*
*
*splineGetDate(struct subRecord *psub)
* - get date from calibration file 
*
*/
static epicsInt32 splineGetDate(aSubRecord *psub){
  spline s;
  // Pointers to hold the returned values
  char* date = NULL; 
  epicsInt32 debug;
  //Cast EPICS fields to correct types
  char* tnam; 
  tnam = (char*) psub->a;
  debug = *(epicsInt32*) psub->b;
  
    try{
	s = getSplineFromContainer(std::string(tnam));
	/*If this is first call then initialize
	the spline*/
	if ( ! s.is_initialized() ) {
	      recGblRecordError(S_dev_badInit, (void*)psub, "splineGetDate: no transformation");
	      psub->brsv = INVALID_ALARM;
	      return SPL_NO_TRANS;
	}
	else {
	     
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
            if(debug) {
         	printf("%s splineGetDate: calibration file date %s\n", psub->name, date);
	    }
	}
  } catch (alglib::ap_error a) {
      recGblRecordError(S_dev_badRequest, (void*)psub, "splineGetDate: alglib error");
      psub->brsv = MAJOR_ALARM;
      return SPL_ALGLIB_ERR;
  }catch (std::exception& e) {
      errlogSevPrintf(errlogMajor, "%s exception: %s", psub->name, e.what());
      return SPL_BAD_DATA;
  }

   return SPL_SUCCESS;    
}

/* 
 * splineGetInpPrms(struct subRecord *psub)
 * - Get all input parameters that make up the spline used for interpolation
 * - OUTA: Number of data points (LONG)
 * - OUTB: Data points in X array (DOUBLE - waveform)
 * - OUTC: Data points in Y array (DOUBLE - waveform)
 * - OUTD: Date when the calibration file was saved (STRING)
*/ 
static epicsInt32 splineGetInpPrms(aSubRecord *psub){
  spline s;
  // Pointers to hold the returned values
  char* date = NULL; 
  epicsInt32 numDataPoints; 
  double* xpts = NULL; 
  double* ypts = NULL; 
  epicsInt32 npts = 0;
  epicsInt32 xnpts = 0;
  epicsInt32 ynpts = 0;
  devicePvt_ts  *dpvt_ps = NULL;
  size_t nbytes = sizeof(double) * psub->nova;
  epicsInt32 debug;

  //Cast EPICS fields to correct types
  char* tnam; 
  tnam = (char*) psub->a;
  debug = *(epicsInt32*) psub->b;
  dpvt_ps = (devicePvt_ts *)psub->dpvt;
    try{
	s = getSplineFromContainer(std::string(tnam));
	/*If this is first call then initialize
	the spline*/
	if ( ! s.is_initialized() ) {
		recGblRecordError(S_dev_badInit, (void*)psub, "splineGetInpPrms: no transformation");
		psub->brsv = INVALID_ALARM;
		return SPL_NO_TRANS;
	}
	else {
	  
	  /* Fetch the number of data points */   
	  numDataPoints = s.get_num_points(); 
	  *(epicsInt32 *)(psub->vala) = numDataPoints;
          if(debug) {
	      printf("%s splineGetInpParams Num points: %d\n",psub->name, numDataPoints); 
          }	     

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
             recGblRecordError(S_dev_badRequest, (void*)psub, "splineGetInpPrms: X and Y number of data points differ");
	     psub->brsv = MAJOR_ALARM;
	     return SPL_BAD_DATA;
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
         /* Print out data points */
         if(debug) {
             printf("%s splineGetInpPrms: X points: ...\n", psub->name);
             for(int i=0; i<npts; i++) {
	        printf("%f ", xpts[i]);
             }
             printf("\n");
             printf("%s splineGetInpPrms: Y points: ...\n", psub->name);
             for(int i=0; i<npts; i++) {
	         printf("%f ", ypts[i]);
             }
             printf("\n");
         }
	     
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
         if(debug) {
         	printf("%s splineGetInpPrms: calibration file date %s\n",psub->name, date);
	 }
   }
  } catch (alglib::ap_error a) {
      recGblRecordError(S_dev_badRequest, (void*)psub, "splineGetInpPrms: alglib error");
      psub->brsv = MAJOR_ALARM;
      return SPL_ALGLIB_ERR;
  }catch (std::exception& e) {
      errlogSevPrintf(errlogMajor, "%s exception: %s", psub->name, e.what());
      return SPL_BAD_DATA;
  }

   return SPL_SUCCESS;    
}


/*
This block sets up a interface for the ioc shell. It allows the user to intialize
their splines in the st.cmd. Each spline has a filepath and a tranformation name(tname)
When the subroutine is processed it will use the tname in field b to find which spline belongs to it
*/
static const iocshArg spline_bld_arg0 = { "transform_name", iocshArgString};
static const iocshArg spline_bld_arg1 = { "filename", iocshArgString};

static const iocshArg *const spline_bld_args[] = {&spline_bld_arg0, &spline_bld_arg1};
static const iocshFuncDef spline_bld_def = {"splineBuild", 2, spline_bld_args};
static void spline_bld(const iocshArgBuf *args ){
   std::string tname   (  args[0].sval);
   std::string filepath ( args[1].sval);
   printf("Filepath is  %s  \nTransformation name is %s \n",filepath.c_str(),tname.c_str());

   scon.push_back( std::make_pair( tname, spline(filepath) ) );

}

static void drvSplineRegistrar(){
    iocshRegister(&spline_bld_def, spline_bld);
}


epicsRegisterFunction(splineCalcOutput);
epicsRegisterFunction(splineInit);
epicsRegisterFunction(splineGetLimits);
epicsRegisterFunction(splineGetNumPoints);
epicsRegisterFunction(splineGetPoints);
epicsRegisterFunction(splineGetDate);
epicsRegisterFunction(splineGetInpPrms);
epicsExportRegistrar(drvSplineRegistrar);
