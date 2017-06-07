#include "spline_interp.h"
#include "debug.h"
/*
*
* spline::spline(char* filename)
* -constructor generates spline object from data file.
*  k, gap values are read in from data file. Then interpolation
*  is built
*
*/
spline::spline(char* filename){
    //Indicate class has been constructed
    initialized = true;

    
       debugPrintf("%s\n",filename);
    
    
    //Extract measured data from data file
    std::ifstream file( filename );
    int N = extract_points(file,&k,&gap);
    file.close();
  
    
    debugPrintf("Extracted %d points\n",N);
    

    //Build the spline for forward and inverse transformations
    alglib::spline1dbuildcubic(k,gap,N,0,0,0,0,interp);
    alglib::spline1dbuildcubic(gap,k,N,0,0,0,0,interp_inv);
    
    
    debugPrintf("Built interpolation @ %p and inverse @ %p\n",&interp,&interp_inv);
    
}


/*
*
* spline::calc(double point)
* -calculates the value of the interpolated function
*  at passed in point
*
*/
double spline::calc(double point){
     return alglib::spline1dcalc(interp,point) ;
}


/*
*
* spline::calc(double point)
* -calculates the inverse of the interpolated function
*  at passed in point
*
*/
double spline::calcInv(double point){
     return alglib::spline1dcalc(interp_inv,point) ;
}


/*
*
* spline::isInitialized()
* -returns copy of private flag indicating
*  whether a constructor to intialize k, gap
*  values was called
*
*/
bool spline::isInitialized(){
    return initialized;
}


/*
*
* spilt(std::string str, char delimiter){
*    -spilts string into a vector of string with a charater delimiter
*
*/
std::vector<std::string> spline::spilt(std::string str, char delimiter){
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
void spline::parseFile(std::ifstream &f, std::vector<double> &x, std::vector<double> &y){
    std::string line;
    char delim = ',';
    if(f.is_open()){
       while( getline(f,line) ){
         //spilt on comma first element goes to x vector
	       //second element goes to y vector
	       std::vector<std::string> out = spilt(line,delim);
	       //file format error
	       assert(out.size() == 2);
	       char* off = 0;
	       debugPrintf(" Readin : %f , %f\n", strtod(out[0].c_str(),&off) , strtod(out[1].c_str(),&off));
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
* -returns number of points
*
*/
int spline::extract_points(std::ifstream &f, alglib::real_1d_array* AX, 
                           alglib::real_1d_array* AY ){
  std::vector<double> x;
  std::vector<double> y;
  parseFile(f,x,y);

  
  debugPrintf("%d\n",x.size());
  for (int i = 0; i < x.size() ; i++){
      debugPrintf("%f,",  x[i]);
      debugPrintf("%f\n", y[i]);
  }
  
  
  AX->setcontent(x.size(), &(x[0]));
  AY->setcontent(y.size(), &(y[0]));

  
  double* xprime = AX->getcontent();
  double* yprime = AY->getcontent();

  debugPrintf("%d\n",x.size());
  for (int i = 0; i < x.size() ; i++){
      debugPrintf("%f,",  xprime[i]);
      debugPrintf("%f\n", yprime[i]);
  }
  
  
  return x.size();
}

