/* splineMain.cpp */
/* Author:  Marty Kraimer Date:    17MAR2000 */
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"

static const iocshArg testerArg0 = { "filename", iocshArgString};
static const iocshArg *const testerArgs[1] = {&testerArg0};
static const iocshFuncDef testerFuncDef = {"tester",1,testerArgs};
//static void tester(const iocshArgBuf *args ){
//   printf("test complete %s\n",args[0].dval);
//}

//static void subsRegister(){
//    iocshRegister(&testerFuncDef, tester);
//}

int main(int argc,char *argv[])
{
    //subsRegister();
    if(argc>=2) {    
        iocsh(argv[1]);
        epicsThreadSleep(.2);
    }
    iocsh(NULL);
    epicsExit(0);
    return(0);
}
