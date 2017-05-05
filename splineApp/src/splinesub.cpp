#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<interpolation.h>
#include<string.h>
#include<assert.h>

#define not no_t

#include <iocsh.h>
#include<epicsExport.h>
#include<registryFunction.h>
#include<dbDefs.h>
#include<aSubRecord.h>
#include<dbCommon.h>
#include<recSup.h>
#include<dbAccess.h>
#define DEBUG


alglib::spline1dinterpolant d;

/*
*
* spilt(std::string str, char delimiter){
*    -spilts string into a vector of string with a charater delimiter
*
*/
std::vector<std::string> spilt(std::string str, char delimiter){
    std::vector<std::string> internal;
    std::stringstream ss(str);
    std::string tok;
    while (getline(ss,tok,delimiter)){
        internal.push_back(tok);
    }

    return internal;
}


/*
*
* parsePoints(std::vector<double> &x, std::vector<double> &y, char* data)
*  -parses character array, taking numbers from comma seperated values
*   and placing them into x,y arrays
*
*/
void parseFile(std::ifstream &f, std::vector<double> &x, std::vector<double> &y){
    std::string line;
    char delim = ',';
    if(f.is_open()){
       while( getline(f,line) ){
           //spilt on comma first element goes to x vector
	   //second element goes to y vector
           std::vector<std::string> out = spilt(line,delim);
	   //file format error
	   assert(out.size() == 2);
           
	   x.push_back( atof(out[0].c_str() )  );
	   y.push_back( atof(out[1].c_str() )  );
       }
    }
}




/*
*
* extract_points(FILE* f, alglib::real_1d_array* AX, alglib::real_1d_array* AY )
* -reads files then parses data into the alglib standard arrays
* -files is assumed to be csv with pairs of values x,y
*
*/
int extract_points(std::ifstream &f, alglib::real_1d_array* AX, alglib::real_1d_array* AY ){
  std::vector<double> x;
  std::vector<double> y;
  parseFile(f,x,y);

  #ifdef DEBUG
  printf("%d\n",x.size());
  for (int i = 0; i < x.size() ; i++){
      printf("%f,",  x[i]);
      printf("%f\n", y[i]);
  }
  #endif
  
  AX->setcontent(x.size(), &(x[0]));
  AY->setcontent(y.size(), &(y[0]));

  #ifdef DEBUG
  double* xprime = AX->getcontent();
  double* yprime = AY->getcontent();

  printf("%d\n",x.size());
  for (int i = 0; i < x.size() ; i++){
      printf("%f,",  xprime[i]);
      printf("%f\n", yprime[i]);
  }
  #endif
  
  return x.size();
}

/*
struct ebuf{
   DBRstatus
   DBRtime
   string value[1];
}ebuf;

long initTest(aSubRecord *psub){
  long options,num_elem,status;
  options = DBR_STATUS | DBR_TIME;
  num_elem = 1;
  status = 0;
  
  DBADDR *pdbAddr = dbGetPdbAddrFromLink( &psub->inpb );
  if(pdbAddr != -1){
     status = dbGetField(pdbAddr,DBR_STRING, &ebuf,&options,&num_elem);
  }else{
     printf("No address for link\n");
  }
}
*/




/*
*
* initSpline(struct subRecord *psub)
* -called by EPICS record initialization
* -parses k,gap data file and create interpolation
*
*/
long initSpline(aSubRecord *psub){
  printf("Subroutine Initialized\n");
  char** valb;
  char** vald;
  double* vala;
  double* valc;
  vala = (double *) psub->a;
  valb = (char**) psub->b;
  vala = (double *) psub->c;
  vald = (char**) psub->d;
  
  printf("LINK = %s\n", psub->inpd);
  printf("VAL A = %f\n",vala[0]);
  printf("VAL B = %s\n",valb);
  printf("VAL C = %f\n",valc[0]);
  printf("VAL D = %s\n",vald[0]);
  /*  alglib::real_1d_array k;
    alglib::real_1d_array gap;
    char*  filename;
    #ifdef DEBUG
    filename = (char*) psub->b;
    printf("initSpline :: Test %s\n",filename);
    //printf("Opening this file %s\n",filename);
    #endif
    std::ifstream file ( filename  );
    int N = extract_points(file,&k,&gap);
    file.close();
    
    #ifdef DEBUG
    printf("Extracted %d points\n",N);
    #endif

    alglib::spline1dbuildcubic(k,gap,N,0,0,0,0,d);

    #ifdef DEBUG
    printf("Build interpolation @ %p\n",&d);
    #endif
    */
    return 0;
}

/*
*
* initSplineInv(struct subRecord *psub)
* -called by EPICS record initialization
* -parses k,gap data file and creates an
*  inverse interpolation
*
*/
long initSplineInv(aSubRecord *psub){
    alglib::real_1d_array k;
    alglib::real_1d_array gap;
    char* filename;
    filename = (char* ) psub->b;

    #ifdef DEBUG
    printf("initSplineInV :: Opening this file %s\n",filename);
    #endif
    
    std::ifstream file ( filename  );
    int N = extract_points(file,&k,&gap);
    file.close();
    
    #ifdef DEBUG
    printf("initSplineInv :: Extracted %d points\n",N);
    #endif

    //alglib::spline1dbuildcubic(gap,k,N,0,0,0,0,d);

    #ifdef DEBUG
    printf("initSplineInv :: Build interpolation @ %p\n",&d);
    #endif

    return 0;
}

/*
*
* splineIt(struct subRecord *psub)
* -calls spline function and sets record value
* to interpolation at point A
* - VAL = f(A)
*
*/
static long splineIt(aSubRecord *psub){
  printf("Subroutine called\n");
  char** valb;
  double* vala;
  double* valc;
  vala = (double *) psub->a;
  valb = (char**) psub->b;
  valc = (double *) psub->c;
  
  printf("VAL A = %f\n",vala[0]);
  printf("VAL B = %s\n",valb);
  printf("VAL C = %f\n",valc[0]);
  /*double eval_point;
  double* input_a =  (double *)  psub->a ;
  eval_point = input_a[0];
  char* filename;
  filename = (char* ) psub->b;
  #ifdef DEBUG
  FILE* f;
  f = fopen("ioc.log","w");
  fprintf(f,"splineIt :: Build interpolation @ %p\n",&d);
  fclose(f);
  #endif
  //TODO assign type to vala
  //psub->val = alglib::spline1dcalc(d,eval_point) ;
  */
  return 0;    
}

epicsRegisterFunction(initSpline);
epicsRegisterFunction(initSplineInv);
epicsRegisterFunction(splineIt);
