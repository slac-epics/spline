#!../../bin/rhel6-x86_64/umc
## --------------------------------------------------------------------------------

< envPaths
cd ${TOP}

## --------------------------------------------------------------------------------

## Register all support components
dbLoadDatabase "dbd/umc.dbd"
umc_registerRecordDeviceDriver pdbbase

## --------------------------------------------------------------------------------
# Set environment variables
## -------------------------------------------------------------------------------

# Stream device protocol file path
epicsEnvSet( "STREAM_PROTOCOL_PATH", "$(TOP)/umcApp/src" )

# IOC and PV prefix 
epicsEnvSet("IOC_NAME", "SIOC:TEST:SPL")
epicsEnvSet("LOCATION", "B34")
epicsEnvSet("SEGMENT" , "TST1")
epicsEnvSet("PREFIX"  , "USEG:${LOCATION}:${SEGMENT}")

epicsEnvSet("ENGINEER" , "Alex Montironi")

## --------------------------------------------------------------------------------

## SPLINE INIT
#   (1) name of the spline transformation [use this in template macro TNAME]
#   (2) path to file with spline input data [absolute path]
splineInit("k2gap","iocBoot/datafiles/sxuXXX_k2gap.dat")

## --------------------------------------------------------------------------------

## Load record instances

dbLoadRecords(db/splineTesting.db, "U=${PREFIX}")

## --------------------------------------------------------------------------------

cd ${TOP}/iocBoot/${IOC}
iocInit


