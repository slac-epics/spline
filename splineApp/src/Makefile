TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

# ================================================
# Build the IOC application
# ================================================
# This will attempt to build an IOC application 
# for every target including the host computer.
LIBRARY += spline

# Use the following for RTEMS Only IOCs
#PROD_IOC_RTEMS = spline

#My external library
#========================================================

# PATH TO THE EXTERNAL LIBRARIES
#===================================
USR_INCLUDES +=  -I $(ALGLIB_INCLUDE)
alglib_DIR=$(ALGLIB_LIB)

# LINK EXTERNAL LIBRARIES STATICALLY
#====================================

# spline.dbd will be created and installed
DBD += spline.dbd
# spline.dbd will be made up from these files:
spline_DBD += subs.dbd

# ================================================================================
# The system command will be mainly used by linux hosted IOCs
# Very useful for changing kernel thread priorities for kernel drivers
# ================================================================================
spline_DBD += system.dbd

# ======================================================================
# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
spline_SRCS += spline_registerRecordDeviceDriver.cpp

spline_SRCS_DEFAULT += splineMain.cpp

spline_SRCS_vxWorks += -nil-

# ========================================================================
# ========================================================================
# Add locally compiled object code
# For example, subroutines
# ========================================================================
spline_SRCS += spline_sub.cpp
spline_SRCS += spline_interp.cpp

# Need to link algorighm library
spline_LIBS += alglib

spline_LIBS += $(EPICS_BASE_IOC_LIBS)
#===========================


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

