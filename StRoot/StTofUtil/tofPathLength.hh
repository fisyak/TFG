 /***************************************************************************
 *
 * $Id: tofPathLength.hh,v 1.1 2003/08/06 23:42:56 geurts Exp $ tofPathLength.cc,v 1.2 2003/07/11 05:08:49 geurts Exp $
 *
 * Author: Frank Geurts
 ***************************************************************************
 *
 * Description: Calculate helix path length between to points.
 *              requires begin and end point StThreeVectors and helix curvature
 *
 ***************************************************************************
 *
 * $Log: tofPathLength.hh,v $
 * Revision 1.1  2003/08/06 23:42:56  geurts
 * function definitions in seperate header file
 *
 *
 **************************************************/
#ifndef TOFPATHLENGTH_HH
#define TOFPATHLENGTH_HH
#include "StThreeVector.hh"
class StThreeVectorD;
class StThreeVectorF;

double tofPathLength(const StThreeVector<double>*, const StThreeVector<double>*, const double);
double tofPathLength(const StThreeVectorD*, const StThreeVectorD*, const double);
double tofPathLength(const StThreeVectorF*, const StThreeVectorD*, const double);
double tofPathLength(const StThreeVectorF*, const StThreeVectorF*, const double);
#endif
