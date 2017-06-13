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
PROD_IOC = spline

# Use the following for RTEMS Only IOCs
#PROD_IOC_RTEMS = spline

#My external library
#========================================================

USR_INCLUDES += $(ALGLIB_INCLUDE)
# PATH TO THE EXTERNAL LIBRARIES
#===================================
alg_DIR=$(ALGLIB_LIB)

# LINK EXTERNAL LIBRARIES STATICALLY
#====================================
spline_LIBS+=alg

# spline.dbd will be created and installed
DBD += spline.dbd

# spline.dbd will be made up from these files:
spline_DBD += subs.dbd
spline_DBD += base.dbd
spline_DBD += devIocStats.dbd
spline_DBD += autosaveSupport.dbd
spline_DBD += caPutLog.dbd
spline_DBD += $(CEXP_DBDS)

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
spline_SRCS += splinesub.cpp
spline_SRCS += spline_interp.cpp

# =====================================================
# Link in the code from other libraries
# =====================================================
# Needed to link in iocAdmin support
spline_LIBS += devIocStats

# Needed to link in autosave support
spline_LIBS += autosave

# Needed to link caPutLog support
spline_LIBS += caPutLog

# Needed to link cexpsh() support
spline_LIBS += $(CEXP_LIBS)

spline_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

