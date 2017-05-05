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
  bool isEmpty = True;

   spline(std::string filename);

  std::vector<std::string> spline::spilt(std::string str, char delimiter);
  void spline::parseFile(std::ifstream &f, std::vector<double> &x, std::vector<double> &y);
  int spline::extract_points(std::ifstream &f, alglib::real_1d_array* AX, alglib::real_1d_array* AY );
}


#endif