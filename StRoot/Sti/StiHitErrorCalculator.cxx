
/*!
 * $Id: StiHitErrorCalculator.cxx,v 2.13 2003/07/30 19:18:23 pruneau Exp $  
 *
 * Author: A. Rose, WSU, Jan 2002
 *
 * The Hit Error Calculator class is responsible for returning
 * errors for a hit from an arbitrary detector acording to a 
 * parameterization established by Mike Lisa et. al.,
 *
 * http://www.star.bnl.gov/~lisa/HitErrors (Mar 14, 2001)
 *
 *
 * $Log: StiHitErrorCalculator.cxx,v $
 * Revision 2.13  2003/07/30 19:18:23  pruneau
 * sigh
 *
 * Revision 2.11  2003/07/14 21:12:43  andrewar
 * Revert to old version to eliminate introduced bug
 *
 * Revision 2.9  2003/06/26 20:22:18  andrewar
 * Fixed error in drift calc.
 *
 * Revision 2.8  2003/05/09 22:07:54  pruneau
 * Added protection to avoid 90deg tracks and ill defined eloss
 *
 * Revision 2.7  2003/05/09 14:57:19  pruneau
 * Synching
 *
 * Revision 2.6  2003/05/01 20:46:41  pruneau
 * changed error parametrization
 *
 * Revision 2.5  2003/04/22 21:20:05  pruneau
 * Added hit filter
 * Tuning og finder pars
 * Tuning of KalmanTrackNode
 *
 * Revision 2.4  2003/04/10 12:02:12  pruneau
 * various changes
 *
 * Revision 2.3  2003/04/04 14:43:44  pruneau
 * Fix to the hit error calculator and the getCharge methods.
 *
 * Revision 2.2  2003/04/02 16:45:19  pruneau
 * Fixed error calculation and impose hard coded upper cut on the size of the error.
 *
 * Revision 2.1  2003/01/08 21:17:33  pruneau
 * Addind class StiSortedHitIterator to work in the seed finder
 * and StiDummyVertex finder to provide an StEvent based vertex
 * retrieval mechanism.
 *
 * Revision 2.0  2002/12/10 21:59:13  pruneau
 * Introducing version 2.0
 *
 *
 */


//Sti inlcudes
#include "StiKalmanTrackNode.h"
#include "StiHitErrorCalculator.h"

StiDefaultHitErrorCalculator::StiDefaultHitErrorCalculator()
{
  set(0.,0.,0.,0.,0.,0.); 
}

StiDefaultHitErrorCalculator::~StiDefaultHitErrorCalculator()
{}

void StiDefaultHitErrorCalculator::set(double intrinsicZ, double driftZ,
				    double crossZ, double intrinsicX,
				    double driftX, double crossX)
{
  coeff[0]= intrinsicZ;
  coeff[1]= driftZ;
  coeff[2]= crossZ;
  coeff[3]= intrinsicX;
  coeff[4]= driftX;
  coeff[5]= crossX;
}

void StiDefaultHitErrorCalculator::calculateError(StiKalmanTrackNode * node) const
{  
  double dz = (200.-fabs(node->getZ()))/100.;
  double cosCA = node->_cosCA;
  double sinCA = node->_sinCA;
  if (cosCA==0.)
    cosCA=1.e-10;
  double tanCA = sinCA/cosCA;
  double ecross=coeff[0]+coeff[1]*dz/(cosCA*cosCA) +coeff[2]*tanCA*tanCA;
  double tanDip=node->getTanL();
  double cosDipInv2=1+tanDip*tanDip;
  double edip=coeff[3]+coeff[4]*dz*cosDipInv2+coeff[5]*tanDip*tanDip;
  if (ecross>50) ecross = 50.; 
  if (edip>50) edip = 50.; 
  double scaling;
  if (node->_x>120)
    scaling = StiKalmanTrackNode::pars->getOuterScaling();
  else
    scaling = StiKalmanTrackNode::pars->getInnerScaling();
  node->eyy = ecross*scaling*scaling; // in cm^2
  node->ezz = edip*scaling*scaling;
}

