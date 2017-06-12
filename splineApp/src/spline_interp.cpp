#include "spline_interp.h"
#include "debug.h"


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
* spline::calcInv(double point)
* -calculates the inverse of the interpolated function
*  at passed in point
*
*/
double spline::calc_inv(double point){
     return alglib::spline1dcalc(interp_inv,point) ;
}


/*
*
* spline::is_initialized()
* -returns copy of private flag indicating
*  whether a constructor to intialize x,y 
*  values was called
*
*/
bool spline::is_initialized(){
    return initialized;
}


/*
*spline::get_num_lines(){
*   -counts total number of lines in file
*/
void spline::set_array_length(){
  int i = 0;

  std::string line;
  std::ifstream f( filename );
  if (f.is_open()){
      while (getline(f,line)){
        i++;
      }
  }
  f.close();
  x_a.setlength(i);
  y_a.setlength(i);
  N = i;
}


/*
*
* split(std::string str, char delimiter){
*    -splits string into a vector of string with a charater delimiter
*
*/
std::vector<std::string> spline::split(std::string str, char delimiter){
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string tok;
    while (getline(ss,tok,delimiter)){
        result.push_back(tok);
    }

    return result;
}


/*
*
* parse(std::string line, char delim)
*  -parses a line for two tokens (x,y) and returns them as a pair
*
*/
std::pair<double,double> spline::parse(std::string line, char delim){ 
       /* split on comma first element goes to x vector second y vector*/
     std::pair<double, double> result;
     std::vector<std::string> out = split(line,delim);
     //file format error
     if (out.size() != 2) throw -1 ;
     result = std::make_pair( atof(out[0].c_str() ),  atof(out[1].c_str() ) );
     debugPrintf(" Readin : %f , %f\n", result.first, result.second);
     if (result.first == 0 || result.second == 0 ) throw -1 ;
     return result;
}



/*
*
* parse_file()
*  -parses data file, taking numbers from comma seperated values
*   and placing them into x,y arrays
*
*/
void spline::parse_file(){
    char delim = ','; int i = 0;
    std::string line;
    std::pair<double, double> p;

    std::ifstream f( filename );

    if( f.is_open() ){
      while( std::getline(f,line) ){
         debugPrintf("%s\n",line.c_str());
         p = parse(line,delim);
         debugPrintf("parsed\n");
         x_a[i] = p.first;
         y_a[i] = p.second;
         i++;
      }
    }
    f.close();
}


/*
*
* spline::spline(std::string filename)
* -constructor generates spline object from data file.
*  x, y values are read in from data file. Then interpolation
*  is built
*
*/
spline::spline(std::string filename){
    //Indicate class has been constructed
    initialized = true;
    this->filename = filename.c_str();
    debugPrintf("%s\n",this->filename);
    
    //Extract measured data from data file
    set_array_length();
    parse_file();   
    debugPrintf("Extracted %d points\n",N);
    //Build the spline for forward and inverse transformations
    alglib::spline1dbuildcubic(x_a,y_a,N,0,0,0,0,interp);
    alglib::spline1dbuildcubic(y_a,x_a,N,0,0,0,0,interp_inv);
    debugPrintf("Built interpolation @ %p and inverse @ %p\n",&interp,&interp_inv);
}