#ifndef SPLINE_INTERP_H
#define SPLINE_INTERP_H

#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<interpolation.h>
#include<string.h>
#include<assert.h>

#define DEBUG

//Object that holds spline
class spline{
   
  alglib::real_1d_array k;
  alglib::real_1d_array gap;
  alglib::spline1dinterpolant interp;
  bool initialized ;

  public:
  
  /*default contructor*/
  spline(){initialized=false;};
  /*contructor that takes data file*/
  spline(char* filename);

  bool isInitialized();
  double calc(double point);
  int extract_points(std::ifstream &f, alglib::real_1d_array* AX, alglib::real_1d_array* AY );
  void parseFile(std::ifstream &f, std::vector<double> &x, std::vector<double> &y);
  std::vector<std::string> spilt(std::string str, char delimiter);
};


#endif
