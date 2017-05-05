#!../../bin/linux-x86/spline

## You may have to change spline to something else
## everywhere it appears in this file

< envPaths


cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/spline.dbd")
spline_registerRecordDeviceDriver(pdbbase)


# ====================================================================
# Setup some additional environment variables
# ====================================================================
# Setup environment variables
epicsEnvSet("ENGINEER","Jacob DeFilippis")
epicsEnvSet("LOCATION","??")
tester("foo");
# =====================================================================
## Load record instances
dbLoadRecords("db/k_to_gap.db","user=jpdefHost")

iocInit()

