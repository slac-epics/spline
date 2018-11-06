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

/*Local libs*/
#include "spline_interp.h"

typedef std::vector< std::pair<std::string, spline> > SplineContainer;

/*Main data structure*/
  SplineContainer scon;

static std::string  makePath(const char* filename_c_str);
static void initSpline(std::string trans_name, std::string filepath);
static spline getSplineFromContainer(std::string psub);
static long splineIt(aSubRecord *psub);
static long getLimits(aSubRecord *psub);

#endif
