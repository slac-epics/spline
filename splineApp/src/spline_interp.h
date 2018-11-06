#ifndef SPLINE_INTERP_H
#define SPLINE_INTERP_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <interpolation.h>
#include <string.h>
#include <assert.h>

//Object that holds spline
class spline{
   
  alglib::real_1d_array x_a;
  alglib::real_1d_array y_a;
  alglib::spline1dinterpolant interp;
  alglib::spline1dinterpolant interp_inv;
  /* indicates if object has been contructed or not*/
  bool initialized ;
  const char* filename;
  int N;

  public:
  
    /*default contructor*/
    spline(){initialized=false;};
    /*contructor that takes data file*/
    spline(std::string filename);
  
  
    bool is_initialized();
    double calc(double point);
    double calc_inv(double point);
    void dospline(std::string trans_name, std::string filepath);
    /* get the limits on x and y */
    double get_max_X();
    double get_min_X();
    double get_max_Y();
    double get_min_Y();

  private:

    void parse_file();
    void set_array_length();
    std::pair<double,double> parse(std::string line, char delim); 
    std::vector<std::string> split(std::string str, char delimiter);

};


#endif
