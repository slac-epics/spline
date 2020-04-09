#ifndef SPLINE_SUB_H
#define SPLINE_SUB_H

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
#include <epicsMutex.h>
#include <cantProceed.h>        /* for callocMustSucceed()             */
#include <dbFldTypes.h>         /* for DBF_DOUBLE                      */

/*Local libs*/
#include "spline_interp.h"


typedef std::vector< std::pair<std::string, spline> > SplineContainer;

/*Main data structure*/
SplineContainer scon;

typedef struct devicePvt_s
{
   epicsUInt32   npts;
   epicsMutexId  mlock;
}devicePvt_ts;

//static std::string  makePath(const char* filename_c_str);
//static void initSpline(std::string trans_name, std::string filepath);
static spline getSplineFromContainer(std::string psub);
static long splineCalcOutput(aSubRecord *psub);
static long splineInit(aSubRecord *psub);
static long splineGetLimits(aSubRecord *psub);
static long splineGetNumPoints(aSubRecord *psub);
static long splineGetPoints(aSubRecord *psub);
static long splineGetDate(aSubRecord *psub);
static long splineGetInpPrms(aSubRecord *psub);

#endif
