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
#include <alarm.h>
#include <devLib.h>            /* for S_dev_badRequest and S_dev_badInit */
#include <recGbl.h>

/*Local libs*/
#include "spline_interp.h"

/* Macros for return codes */
/* Define SUCCESS macro*/
#ifndef SPL_SUCCESS
#define SPL_SUCCESS 0 
#endif
/* Define BAD_PTR macro*/
#ifndef SPL_BAD_PTR
#define SPL_BAD_PTR -1
#endif
/* Define SPL_NO_TRANS macro*/
#ifndef SPL_NO_TRANS
#define SPL_NO_TRANS -2
#endif
/* Define SPL_ALGLIB_ERR macro*/
#ifndef SPL_ALGLIB_ERR
#define SPL_ALGLIB_ERR -3
#endif
/* Define SPL_BAD_DATA macro*/
#ifndef SPL_BAD_DATA
#define SPL_BAD_DATA -4
#endif

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
static epicsInt32 splineCalcOutput(aSubRecord *psub);
static epicsInt32 splineInit(aSubRecord *psub);
static epicsInt32 splineGetLimits(aSubRecord *psub);
static epicsInt32 splineGetNumPoints(aSubRecord *psub);
static epicsInt32 splineGetPoints(aSubRecord *psub);
static epicsInt32 splineGetDate(aSubRecord *psub);
static epicsInt32 splineGetInpPrms(aSubRecord *psub);

#endif
