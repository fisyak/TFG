 /***************************************************************************
 *
 * $Id: tofPathLength.cc,v 1.3 2003/08/06 23:42:56 geurts Exp $
 *
 * Author: Frank Geurts
 ***************************************************************************
 *
 * Description: Calculate helix path length between to points.
 *              requires begin and end point StThreeVectors and helix curvature
 *
 ***************************************************************************
 *
 * $Log: tofPathLength.cc,v $
 * Revision 1.3  2003/08/06 23:42:56  geurts
 * function definitions in seperate header file
 *
 * Revision 1.2  2003/07/11 05:08:49  geurts
 * added extra overloaded function.
 *
 * Revision 1.1  2003/04/15 01:45:16  geurts
 * Introduction of standalone function for TOF path length calculation
 *
 **************************************************/
#include "StThreeVectorD.hh"
#include "StThreeVectorF.hh"
#include "tofPathLength.hh"

// ----------------------------------------------------------------------------------------------
double tofPathLength(const StThreeVectorD*  beginPoint, const StThreeVectorD* endPoint, const double curvature){
  // used in StTofGeometry::tofHelixToArray
  double x,y,z;
  x = (double)beginPoint->x();
  y = (double)beginPoint->y();
  z = (double)beginPoint->z();
  StThreeVector<double> bp (x,y,z);
  x = (double)endPoint->x();
  y = (double)endPoint->y();
  z = (double)endPoint->z();
  StThreeVector<double> ep(x,y,z);
  return tofPathLength(&bp,&ep,curvature);
}


double tofPathLength(const StThreeVectorF* beginPoint, const StThreeVectorD* endPoint, const double curvature){
  double x,y,z;
  x = (double)beginPoint->x();
  y = (double)beginPoint->y();
  z = (double)beginPoint->z();
  StThreeVector<double> bp (x,y,z);
  x = (double)endPoint->x();
  y = (double)endPoint->y();
  z = (double)endPoint->z();
  StThreeVector<double> ep(x,y,z);
  return tofPathLength(&bp,&ep,curvature);
}

double tofPathLength(const StThreeVectorF* beginPoint, const StThreeVectorF* endPoint, const double curvature){
  double x,y,z;
  x = (double)beginPoint->x();
  y = (double)beginPoint->y();
  z = (double)beginPoint->z();
  StThreeVector<double> bp (x,y,z);
  x = (double)endPoint->x();
  y = (double)endPoint->y();
  z = (double)endPoint->z();
  StThreeVector<double> ep(x,y,z);
  return tofPathLength(&bp,&ep,curvature);
}


// ----------------------------------------------------------------------------------------------
double tofPathLength(const StThreeVector<double>*  beginPoint, const StThreeVector<double>* endPoint, const double curvature){

  double xdif =  endPoint->x() - beginPoint->x();
  double ydif =  endPoint->y() - beginPoint->y();
  
  double C = sqrt(xdif*xdif + ydif*ydif);
  double s_perp = C;
  if (curvature){
    double R = 1/curvature;
    s_perp = 2*R * asin(C/(2*R));
  }

  double s_z = fabs(endPoint->z() - beginPoint->z());
  double value = sqrt(s_perp*s_perp + s_z*s_z);

  return(value);
}


