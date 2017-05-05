#include<spline_interp.h>

spline(std::string filename){

    std::ifstream file ( filename  );
    int N = extract_points(file,&k,&gap);
    file.close();
    
    #ifdef DEBUG
    printf("Extracted %d points\n",N);
    #endif

    alglib::spline1dbuildcubic(k,gap,N,0,0,0,0,interp);

    #ifdef DEBUG
    printf("Build interpolation @ %p\n",&d);
    #endif
}

double spline::calc(double point){
     return alglib::spline1dcalc(interp,point) ;
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
int spline::extract_points(std::ifstream &f, alglib::real_1d_array* AX, alglib::real_1d_array* AY ){
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


