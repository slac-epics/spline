#include "spline_interp.h"


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
         // count only the lines with actual data
         if((line.at(0) != '#') && !(isalpha(line.at(0)))) {
           i++;
	 }
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
     if (out.size() != 2) {printf("spline::parse Not a pair of values\n"); throw -1 ;}
     result = std::make_pair( atof(out[0].c_str() ),  atof(out[1].c_str() ) );
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
    char b;
    int  c=0;
    if( f.is_open() ){
      /*This skips BOM utf headers and non-acsii
      junction before data*/
      f.read(&b,1);
      while ( (b & ~0x7F) != 0 ){
            //ignore they arent ascii
            f.read(&b,1);
            c++;//hah
      }
      f.clear();
      f.seekg(c);

      while( std::getline(f,line) ){
         if((line.at(0) != '#') && !(isalpha(line.at(0)))) {
           p = parse(line,delim);
           x_a[i] = p.second;
           y_a[i] = p.first;
           i++;
         }
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
    printf("spline::spline Filname is %s\n",filename.c_str());
    initialized = true;
    this->filename = filename.c_str();
    
    //Extract measured data from data file
    set_array_length();
    parse_file();   
    printf("Points for spline...\n");
    printf("x_a = ..\n");
    for (int i = 0 ; i < N ; ++i){
      printf("%f, ",x_a[i] );
    }
    printf("\n");
    printf("y_a = ..\n");
    for (int i = 0 ; i < N ; ++i){
      printf("%f, ",y_a[i] );
    }
    printf("\n");
    //Build the spline for forward and inverse transformations
    alglib::spline1dbuildcubic(x_a,y_a,N,0,0,0,0,interp);
    alglib::spline1dbuildcubic(y_a,x_a,N,0,0,0,0,interp_inv);
}

/* get the limits on x and y from the data file */
double spline::get_max_X() {
    double max = x_a[0];
    for (int i = 1; i < N; i++) {
        if(x_a[i] > max) {
            max = x_a[i];
        }
    }    
    return max;
}

double spline::get_min_X() {
    double min = x_a[0];
    for (int i = 1; i < N; i++) {
        if(x_a[i] < min) {
            min = x_a[i];
        }
    }    
    return min;
}

double spline::get_max_Y() {
    double max = y_a[0];
    for (int i = 1; i < N; i++) {
        if(y_a[i] > max) {
            max = y_a[i];
        }
    }    
    return max;
}

double spline::get_min_Y() {
    double min = y_a[0];
    for (int i = 1; i < N; i++) {
        if(y_a[i] < min) {
            min = y_a[i];
        }
    }    
    return min;
}

int spline::get_num_points() {
    return N;
}

/* return the X array*/
int spline::get_X_array(double *& xpts, int* npts){
    xpts = x_a.getcontent();
    *npts = N;
    return 0;
}

/* return the Y array*/
int spline::get_Y_array(double *& ypts, int* npts){
    ypts = y_a.getcontent();
    *npts = N;
    return 0;
}

