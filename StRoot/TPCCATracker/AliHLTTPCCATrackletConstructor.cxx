// @(#) $Id: AliHLTTPCCATrackletConstructor.cxx,v 1.8 2012/08/14 16:30:42 fisyak Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************


#include "AliHLTTPCCATracker.h"
#include "AliHLTTPCCATrackParamVector.h"
#include "AliHLTTPCCAParameters.h"
#include "AliHLTTPCCAGrid.h"
#include "AliHLTTPCCAMath.h"
#include "AliHLTTPCCADef.h"
#include "AliHLTTPCCATracklet.h"
#include "AliHLTTPCCATrackletConstructor.h"
#include "AliHLTArray.h"
#include "debug.h"
#include <iomanip>

#ifdef MAIN_DRAW
#include "AliHLTTPCCADisplay.h"
#endif // MAIN_DRAW

#include <limits>
#include <Vc/limits>

using std::endl;

/*
 * Tracklets start as WaitingForFitLinkedHits. Once the start row is reached they go to
 * FitLinkedHits. When the seed is fitted they go to ExtrapolateUp. At some row upwards
 * extrapolation will stop and the tracklet goes into WaitingForExtrapolateDown. Once all tracklets
 * in the vector are done with ExtrapolateUp down extrapolation starts from the highest row where
 * fitting was done. Once a tracklet reaches its fEndRow it goes to the ExtrapolateDown state until
 * extrapolation is done, when it will go into DoneStage.
 *
 * Tracklets in the vector whose trackIndex is < nTracks are in the NullStage.
 */
enum Stage_t {
  WaitingForFitLinkedHits = 0,
  FitLinkedHits,
  ExtrapolateUp,
  WaitingForExtrapolateDown,
  ExtrapolateDown,
  DoneStage,
  NullStage
};

struct AliHLTTPCCATrackletConstructor::TrackMemory {
  int_v fStartRow;  // min row index with only the fitted part
  uint_v fEndRow;    // max row index with only the fitted part
  int_v fFirstRow;  // min row index with the extrapolated parts
  uint_v fLastRow;   // max row index with the extrapolated parts
  int_v fCurrentHitIndex; // indef of the current hit (for chain-fit stage it is next candidate to filtrate, for extra stage it is last filtered hit)
  int_v fStage; // reco stage
  uint_v fNHits; // n track hits
  int_v fRemainingGap; // n missed hits during search
  float_v fLastY; // Y of the last fitted cluster
  float_v fLastZ; // Z of the last fitted cluster
  TrackParamVector fParam;
  uint_m fIsFragile; // is extrapolation and finding of additional hits needed
  // uint_m fIsOnChain;
  
  inline TrackMemory()
    : fStartRow( Vc::Zero ),
    fCurrentHitIndex( -1 ),
    fStage( Vc::Zero ),
    fNHits( 2 ), // the first two hits are certain
    fRemainingGap( AliHLTTPCCAParameters::MaximumExtrapolationRowGap ),
    fLastY( Vc::Zero ),
    fLastZ( Vc::Zero ),
    fIsFragile( Vc::Zero )
    // fIsOnChain( Vc::Zero )
  {}

  int_m IsInvalid() {
    return fNHits < 3 ||
           static_cast<int_m>( CAMath::Abs( fParam.SinPhi() ) > .999f  ||
           fParam.Chi2() >= float_v(15.f)*static_cast<float_v>(fParam.NDF()) );
  }

  std::ostream &operator<<( std::ostream &out ) {
#define SEP endl
//#define SEP " "
    out << "SR " << fStartRow << SEP;
    out << "ER " << fEndRow << SEP;
    out << "FR " << fFirstRow << SEP;
    out << "LR " << fLastRow << SEP;
    out << "CI " << fCurrentHitIndex << SEP;
    out << "St " << fStage << SEP;
    out << "NH " << fNHits << SEP;
    out << "RG " << fRemainingGap << SEP;
    out << "LY " << fLastY << SEP;
    out << "LZ " << fLastZ << SEP;
    out << "IF " << fIsFragile << SEP;
    out << fParam;
    return out;
#undef SEP
  };
};

class InitTracklets
{
  public:
    InitTracklets( AliHLTTPCCATrackletConstructor::TrackMemory &_r, SliceData &data,
        const Tracker &tracker,
        TrackletVector &trackletVector, const uint_v &_trackIndex )
      : r( _r ), fData( data ), fTracker( tracker ),
      fTrackletVector( trackletVector ), trackIndex( _trackIndex ) {}

    void operator()( int rowIndex );

  private:
    AliHLTTPCCATrackletConstructor::TrackMemory &r;
    SliceData &fData;
    const Tracker &fTracker;
    TrackletVector &fTrackletVector;
    const uint_v &trackIndex;
};

/**
 * Fit tracklet to the string of hits connected by links
 */
void AliHLTTPCCATrackletConstructor::FitTracklet( TrackMemory &r, const int rowIndex,
    const uint_v trackIndex, TrackletVector &trackletVector )
{
  const AliHLTTPCCARow &row = fData.Row( rowIndex );

  const int_m active = r.fStage == FitLinkedHits; // active chains (ones, which contains no fitted hits).
  
  assert( AliHLTTPCCAParameters::RowStep == 1 );
  // if (rowStep == 2){
  //   active = active && ( r.fStartRow & int_v( Vc::One ) ) == ( rowIndex & 1 );
  // }
  
  debugF() << "FitTracklet(" << rowIndex << ") stage: " << r.fStage << " r.fStartRow: " << r.fStartRow << " active: " << active << endl;

  assert( active == ( active && r.fStage == FitLinkedHits ) );
  assert( active == ( active && r.fCurrentHitIndex >= 0 ) );
  assert( active == ( active && rowIndex > r.fStartRow ) );
  //assert( r.fActive == ( ( rowIndex - r.fStartRow ) % 2 == 0 ) );
  ASSERT( active == ( active && r.fCurrentHitIndex < row.NHits() ),
    active << r.fCurrentHitIndex << row.NHits() );

  const uint_v oldHitIndex = static_cast<uint_v>( r.fCurrentHitIndex );
  // fData.SetHitAsUsedInTrack( fData.Row( rowIndex - 1 ), static_cast<uint_v>( r.fCurrentHitIndex ), active ); // TODO mark first hit
  
#ifndef NVALGRIND
  VALGRIND_CHECK_VALUE_IS_DEFINED( active );
  VALGRIND_CHECK_VALUE_IS_DEFINED( oldHitIndex );
  for ( int i = 0; i < oldHitIndex.Size; ++i ) {
    if ( active[i] ) {
      debugF() << rowIndex << " - " << i << " - " << oldHitIndex[i] << endl;
      VALGRIND_CHECK_VALUE_IS_DEFINED( fData.HitLinkUpData( row )[oldHitIndex[i]] );
    }
  }
#endif


  // const int_v aaaaaItIsNotUsedButDoHelpsALotToSpeedUpAaaaaa = int_v(fData.HitDataIsUsed( row ), Vc::IndexesFromZero, int_m(Vc::Zero)); // TODO understand
  const int_v isUsed(fData.HitDataIsUsed( row ), static_cast<uint_v>(oldHitIndex), active);
  const int_m fitMask = active && ( (isUsed != int_v(2)) && (isUsed != int_v(3)) ); // mask to add a new hit. // don't take hits from other tracks. In order to reduce calculations.

  const float_v x = fData.RowX( rowIndex ); // convert to float_v once now
  const float_v y = fData.HitPDataY( row , oldHitIndex, float_m(fitMask));
  const float_v z = fData.HitPDataZ( row , oldHitIndex, float_m(fitMask));
  VALGRIND_CHECK_VALUE_IS_DEFINED( x );
  VALGRIND_CHECK_VALUE_IS_DEFINED( y );
  VALGRIND_CHECK_VALUE_IS_DEFINED( z );

  debugF() << "x, y, z: " << x << y << z << endl;

  float_m activeF( static_cast<float_m>( fitMask ) ); // create float_v mask
    // correct SinPhi if it is necessary
    // calculate displacement to previous hit
  const float_v dx = x - r.fParam.X();
  VALGRIND_CHECK_VALUE_IS_DEFINED( dx );
  VALGRIND_CHECK_VALUE_IS_DEFINED( r.fLastY );
  const float_v dy = y - r.fLastY;
  VALGRIND_CHECK_VALUE_IS_DEFINED( dy );
  const float_v dz = z - r.fLastZ;
  debugF() << "dx, dy, dz: " << dx << dy << dz << endl;
  r.fLastY( activeF ) = y;
  r.fLastZ( activeF ) = z;

  float_v err2Y, err2Z;
  float_v sinPhi = r.fParam.GetSinPhi();
  VALGRIND_CHECK_VALUE_IS_DEFINED( sinPhi );


  const float_m fragile =
      static_cast<float_m>( r.fNHits < uint_v(AliHLTTPCCAParameters::MinimumHitsForFragileTracklet) ) || CAMath::Abs( r.fParam.SinPhi() ) >= .99f;
  VALGRIND_CHECK_VALUE_IS_DEFINED( fragile );
  sinPhi( fragile ) = dy * CAMath::RSqrt( dx * dx + dy * dy );
  VALGRIND_CHECK_VALUE_IS_DEFINED( sinPhi );


  assert( ( x == 0 && activeF ).isEmpty() );
  
  activeF = r.fParam.TransportToX( x, sinPhi, fTracker.Param().cBz(), -1.f, activeF );
  
  VALGRIND_CHECK_VALUE_IS_DEFINED( sinPhi );
  
  


  VALGRIND_CHECK_VALUE_IS_DEFINED( active );
  if ( !activeF.isEmpty() )
  {
    fTracker.GetErrors2( rowIndex, r.fParam, &err2Y, &err2Z );
    const int_m hitAdded = static_cast<int_m>( r.fParam.Filter( activeF, y, z, err2Y, err2Z, .99f ) );
    VALGRIND_CHECK_VALUE_IS_DEFINED( hitAdded );
    trackletVector.SetRowHits( rowIndex, trackIndex, static_cast<uint_v>(r.fCurrentHitIndex), hitAdded );
    ++r.fNHits( static_cast<uint_m>(hitAdded) );
    r.fEndRow( hitAdded ) = rowIndex;
      
    ASSERT( ( (row.NHits() > static_cast<uint_v>(oldHitIndex) ) && hitAdded ) == hitAdded,
      row.NHits() << static_cast<uint_v>(oldHitIndex) << hitAdded );
    fData.SetHitAsUsedInTrackFit( row, static_cast<uint_v>( oldHitIndex ), hitAdded );
  }
  
  r.fCurrentHitIndex.gather( fData.HitLinkUpData( row ), static_cast<uint_v>( oldHitIndex ), active ); // set to next linked hit

  VALGRIND_CHECK_VALUE_IS_DEFINED( r.fCurrentHitIndex );
  const int_m fittingDone = r.fCurrentHitIndex < 0 && active;
  debugF() << "fittingDone = " << fittingDone << endl;
//  if ( ISUNLIKELY( !fittingDone.isEmpty() ) ) {
  ++r.fStage( fittingDone ); // goes to ExtrapolateUp if fitting is done (no other hit linked)
  r.fLastRow( fittingDone ) = rowIndex;
  debugF() << "hits: " << r.fNHits << ", sinphi: " << r.fParam.SinPhi() << endl;
  

  const int_m invalidTracklet = r.IsInvalid() && fittingDone;
  
  r.fNHits.setZero( invalidTracklet );
  r.fStage( invalidTracklet || (fittingDone && !r.fIsFragile) ) = DoneStage;
  
  debugF() << "r.fStage: " << r.fStage << endl;
//  }
} // FitTracklet

void AliHLTTPCCATrackletConstructor::FindNextHit( TrackMemory &r, const AliHLTTPCCARow &row,
                                                  float_v &dy_best, float_v &dz_best, int_m &active)
{ 
  const float_v fY = r.fParam.Y();
  const float_v fZ = r.fParam.Z();
  const uint_v fIndYmin = row.Grid().GetBinBounded( fY - AliHLTTPCCAParameters::MinCellSize*.5f, fZ - AliHLTTPCCAParameters::MinCellSize*.5f );

  int_v fHitIndexes(-1);

  for(int trackletIndex =0; trackletIndex<int_v::Size; trackletIndex++)
  {
    if(!active[trackletIndex]) continue;
//   foreach_bit( int trackletIndex, active ) {
    float minRadius2 = std::numeric_limits<float>::max();

    const float y = fY[trackletIndex];
    const float z = fZ[trackletIndex];
    const int indYmin = fIndYmin[trackletIndex];

    { // look at iY = 0,1; iZ = 0
      const int end = fData.FirstHitInBin( row, indYmin + 2 );
      for ( int hitIndex = fData.FirstHitInBin( row, indYmin ); hitIndex < end; hitIndex += float_v::Size ) {
        float_v yz; 
	yz = fData.HitPDataY( row, hitIndex );
        const float_v dy = yz - y;
	yz = fData.HitPDataZ( row, hitIndex );
//        const float_v dz = float_v::loadUnaligned( fData.HitDataZ( row ) + hitIndex ) - z;
        const float_v dz = yz - z;
        float_v radius2 = std::numeric_limits<float_v>::max();
        radius2( float_m(uint_v( Vc::IndexesFromZero ) + uint_v(hitIndex) < uint_v(end)) ) = dy * dy + dz * dz; // XXX Manhattan distance
//         std::cout << y << " dy = "  << dy << std::endl  <<
//             z << " dz = "  << dz << std::endl  <<
//             "  Manhattan distance = " << radius2 << std::endl;
        const float min = radius2.min();
        if ( min < minRadius2 ) {
          minRadius2 = min;
          int i = ( radius2 == min ).firstOne();
          assert( minRadius2 == radius2[i] );
          dy_best[trackletIndex] = dy[i];
          dz_best[trackletIndex] = dz[i];
          fHitIndexes[trackletIndex] = hitIndex + i;
        }
      }
    }
    { // look at iY = 0,1; iZ = 1
      const int nY = row.Grid().Ny();
      const int end = fData.FirstHitInBin( row, indYmin + nY + 2 );
      for ( int hitIndex = fData.FirstHitInBin( row, indYmin + nY ); hitIndex < end; hitIndex += float_v::Size ) {
        float_v yz;
        yz = fData.HitPDataY( row, hitIndex );
        const float_v dy = yz - y;
	yz = fData.HitPDataZ( row, hitIndex );
        const float_v dz = yz - z;
//        const float_v dy = float_v::loadUnaligned( fData.HitDataY( row ) + hitIndex ) - y;
//        const float_v dz = float_v::loadUnaligned( fData.HitDataZ( row ) + hitIndex ) - z;
        float_v radius2 = std::numeric_limits<float_v>::max();
        radius2( float_m(uint_v( Vc::IndexesFromZero ) + uint_v(hitIndex) < uint_v(end)) ) = dy * dy + dz * dz; // XXX Manhattan distance
        const float min = radius2.min();
        if ( min < minRadius2 ) {
          minRadius2 = min;
          int i = ( radius2 == min ).firstOne();
          assert( minRadius2 == radius2[i] );
          dy_best[trackletIndex] = dy[i];
          dz_best[trackletIndex] = dz[i];
          fHitIndexes[trackletIndex] = hitIndex + i;
        }
      }
    }
  }

  r.fCurrentHitIndex( active ) = fHitIndexes;
//  r.fCurrentHitIndex( static_cast<int_m>( row.NHits() <= 0 ) ) = -1; // sometimes row.NHits() == 0 but firstHitInBin != 0 CHECKME. But same checked in Extend\ExtrapolateTracklet
  active &= (r.fCurrentHitIndex != int_v(-1));
} // FindNextHit

/**
 * Now that we have a tracklet try to find more hits that would fit with this tracklet
 */
int_m AliHLTTPCCATrackletConstructor::ExtrapolateTracklet( TrackMemory &r, const int rowIndex, const uint_v trackIndex,
   TrackletVector &trackletVector, const bool dir, const int_m &mask )
{
  //assert( active == ( active && ( r.fStage == ExtrapolateUp || r.fStage == ExtrapolateDown ) ) );
  // reconstruction of tracklets, tracklets update step
  
  const AliHLTTPCCARow &row = fData.Row( rowIndex );

  int_m active = mask;
  --r.fRemainingGap( active );
#ifdef __ASSERT_YF__
  assert( r.fRemainingGap >= 0 || !active );
#endif
  debugF() << "ExtrapolateTracklet(" << rowIndex << ") fRemainingGap: " << r.fRemainingGap << endl;

  
  const float_v x = fData.RowX( rowIndex );

//     // correct SinPhi if it is necessary
//     // calculate displacement to previous hit
//   const float_v dx = x - r.fParam.X();
//   VALGRIND_CHECK_VALUE_IS_DEFINED( dx );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( r.fLastY );
//   const float_v dy = y - r.fLastY;
//   VALGRIND_CHECK_VALUE_IS_DEFINED( dy );
//   const float_v dz = z - r.fLastZ;
//   debugF() << "dx, dy, dz: " << dx << dy << dz << endl;
//   r.fLastY( activeF ) = y;
//   r.fLastZ( activeF ) = z;
// 
//   float_v err2Y, err2Z;
//  float_v sinPhi = r.fParam.GetSinPhi();
//   VALGRIND_CHECK_VALUE_IS_DEFINED( sinPhi );
// 
//   const float_v ri = float_v( Vc::One ) / CAMath::Sqrt( dx * dx + dy * dy ); // RSqrt
//  const float_m fragile =
//      static_cast<float_m>( r.fNHits < AliHLTTPCCAParameters::MinimumHitsForFragileTracklet ) || CAMath::Abs( r.fParam.SinPhi() ) >= .99f;
//   VALGRIND_CHECK_VALUE_IS_DEFINED( fragile );
//   sinPhi( fragile ) = dy * ri;
//   VALGRIND_CHECK_VALUE_IS_DEFINED( sinPhi );

  
  float_m activeF( active ); // get float mask
  assert( ( x == 0 && activeF ).isEmpty() );

//   float_m transport = r.fParam.TransportToXWithMaterial( x,  fTracker.Param().Bz(), .99f );
//   float_m transport = r.fParam.TransportToX( x, sinPhi, fTracker.Param().cBz(), .99f, activeF );
  float_m transport = r.fParam.TransportToX( x, r.fParam.SinPhi(), fTracker.Param().cBz(), .99f, activeF );
  activeF &= transport;

  active = static_cast<int_m>( activeF );

  if ( row.NHits() < 1 || active.isEmpty() ) {
    ++r.fStage( r.fRemainingGap == 0 && mask ); // go to WaitingForExtrapolateDown or DoneStage if the gap got too big
#ifndef NODEBUG
    debugF() << "r.fStage: " << r.fStage << " after ExtrapolateTracklet found empty row and fRemainingGap == " << r.fRemainingGap << endl;
#endif
    // no hits in this row, skip (don't do this before TransportToX)
#ifdef __ASSERT_YF__
    ASSERT( r.fRemainingGap >= 0,
      r.fRemainingGap );
#endif
    return int_m( Vc::Zero );
  }

  //std::cout << std::setw( 4 ) << rowIndex << ": " << active << endl;

    // find next hit


  int_m linkMask = active; linkMask &= int_m(Vc::Zero);
  // if ( dir == 1 ) {
  //   if ( rowIndex-1 >= 0 ) {
  //     linkMask &= r.fLastRow == rowIndex-1; // check only hits on previous row. Others can't have link on current row.
  //     const AliHLTTPCCARow &prevRow = fData.Row( rowIndex-1 );
  //     r.fCurrentHitIndex.gather( fData.HitLinkUpData( prevRow ), static_cast<uint_v>( r.fCurrentHitIndex ), linkMask ); // set to next linked hit
  //   }
  //   else linkMask &= int_m(Vc::Zero);
  // }
  // else {
  //   if( rowIndex+1 < fTracker.Param().NRows() ) {
  //     linkMask &= r.fFirstRow == rowIndex+1;
  //     const AliHLTTPCCARow &prevRow = fData.Row( rowIndex+1 );
  //     r.fCurrentHitIndex.gather( fData.HitLinkDownData( prevRow ), static_cast<uint_v>( r.fCurrentHitIndex ), linkMask );
  //   }
  //   else linkMask &= int_m(Vc::Zero);
  // }
  // linkMask &= (r.fCurrentHitIndex != -1);

  
  // float_v y( fData.HitDataY( row ), static_cast<uint_v>( r.fCurrentHitIndex ), linkMask );
  // float_v z( fData.HitDataZ( row ), static_cast<uint_v>( r.fCurrentHitIndex ), linkMask );

  // foreach_bit( int trackletIndex, linkMask ) { // TODO optimize
  //   const float yT = r.fParam.Y()[trackletIndex];
  //   const float zT = r.fParam.Z()[trackletIndex];
  //   dy_tmp[trackletIndex] = y[trackletIndex] - yT;
  //   dz_tmp[trackletIndex] = z[trackletIndex] - zT;
  // }
  
    // try to find hit by trying all closed hits
  float_v dy_tmp(Vc::Zero), dz_tmp(Vc::Zero);
  int_m findMask  = active && !linkMask;
  
  FindNextHit(r, row, dy_tmp, dz_tmp, findMask);

  const float_v dy = dy_tmp;
  const float_v dz = dz_tmp;
  active = linkMask || findMask;
    
  // XXX use this branch? with 8 or 16 tracklets extrapolated at the same time this might be an
  // unlikely return? Also see above.
//X   if ( active.isEmpty() ) {
//X     return;
//X   }
  
    // const int_v isUsed(fData.HitDataIsUsed( row ), static_cast<uint_v>(r.fCurrentHitIndex), active);
    // active &= (isUsed != int_v( 2 )) && (isUsed != int_v( 1 )); // don't take hits from other chains. In order to reduce calculations.
    // active &= (isUsed != int_v( 2 ));

    // check if found hit is acceptable
  float_v err2Y(Vc::Zero), err2Z(Vc::Zero);
  fTracker.GetErrors2( rowIndex, r.fParam, &err2Y, &err2Z );

  const float_v kFactor = AliHLTTPCCAParameters::HitPickUpFactor * AliHLTTPCCAParameters::HitPickUpFactor * 3.5f * 3.5f;
  const float_v two( 2.f );

  const float_v sy2 = CAMath::Min( two, kFactor * ( r.fParam.GetErr2Y() + err2Y ) );
  const float_v sz2 = CAMath::Min( two, kFactor * ( r.fParam.GetErr2Z() + err2Z ) );

  activeF = static_cast<float_m>( active );
  debugF() << "activeF: " << activeF;
  activeF &= dy * dy <= sy2 && dz * dz <= sz2;
  debugF() << " -> " << activeF;

  
  float_m filtred = r.fParam.FilterDelta( activeF, dy, dz, err2Y, err2Z, .99f );
  activeF &= filtred;
  // r.fCurrentHitIndex( static_cast<int_m>(!activeF) && active ) = -1; // delete indexes of noaccepted hits
  debugF() << " -> " << activeF;
  active = static_cast<int_m>( activeF );
  debugF() << " -> " << active << endl;

  debugF() << "SetRowHits( " << rowIndex << ", " << r.fCurrentHitIndex << ", " << active << ")" << endl;

  ASSERT( ( (row.NHits() > r.fCurrentHitIndex) && active) == active,
          row.NHits() << r.fCurrentHitIndex << active );
  fData.SetHitAsUsedInTrackExtend( row, static_cast<uint_v>(r.fCurrentHitIndex), active );
  
  trackletVector.SetRowHits( rowIndex, trackIndex,  static_cast<uint_v>(r.fCurrentHitIndex), active );
  ++r.fNHits( static_cast<uint_m>(active) );
  r.fRemainingGap( active ) = AliHLTTPCCAParameters::MaximumExtrapolationRowGap;
  ++r.fStage( r.fRemainingGap == 0 && mask ); // go to WaitingForExtrapolateDown or DoneStage if the gap got too big
//  r.fStage( r.fNHits > 10 && mask ) = DoneStage; // don't need long tracklets. merger will do work.
 
  r.fLastRow(  active && int_m( dir) ) = rowIndex;
  r.fFirstRow( active && int_m(!dir) ) = rowIndex;
  return active;
} // ExtrapolateTracklet

int_m AliHLTTPCCATrackletConstructor::ExtendTracklet( TrackMemory &r, const int rowIndex, const uint_v trackIndex,
   TrackletVector &trackletVector, const bool dir, const int_m &mask )
{
  
  //assert( activeExtraMask == ( activeExtraMask && ( r.fStage == ExtrapolateUp || r.fStage == ExtrapolateDown ) ) );
  // reconstruction of tracklets, tracklets update step
  assert( AliHLTTPCCAParameters::RowStep == 1 );

  
  int_m activeExtraMask = mask;
  const int_m activeFitMask = (r.fStage == FitLinkedHits); // active chains (ones, which contains no fitted hits).
  
  const AliHLTTPCCARow &row = fData.Row( rowIndex );
    
  assert( activeFitMask == ( activeFitMask && r.fStage == FitLinkedHits ) );
  assert( activeFitMask == ( activeFitMask && r.fCurrentHitIndex >= 0 ) );
  assert( activeFitMask == ( activeFitMask && rowIndex > r.fStartRow ) );
  ASSERT( activeFitMask == ( activeFitMask && r.fCurrentHitIndex < row.NHits() ),
    activeFitMask << r.fCurrentHitIndex << row.NHits() );
  assert( (activeFitMask && activeExtraMask).isEmpty() );

  
  const uint_v oldHitIndex = static_cast<uint_v>( r.fCurrentHitIndex );


    
  --r.fRemainingGap( activeExtraMask );
  debugF() << "ExtrapolateTracklet(" << rowIndex << ") fRemainingGap: " << r.fRemainingGap << endl;
  
  
  // -- TRANSPORT --

  const float_v x = fData.RowX( rowIndex );

  // const int_v aaaaaItIsNotUsedButDoesHelpALotToSpeedUpAaaaaa = int_v(fData.HitDataIsUsed( row ), Vc::IndexesFromZero, int_m(Vc::Zero)); // todo understand. Here it takes a lot of time...
  const int_v isUsed(fData.HitDataIsUsed( row ), static_cast<uint_v>(oldHitIndex), activeFitMask); // Here it takes a lot of time...
  const int_m fitMask = activeFitMask && (isUsed != int_v(2)) && (isUsed != int_v(3)) && (r.fCurrentHitIndex >= 0); // mask to add a new hit. // don't take hits from other tracks. In order to reduce calculations.

  float_m activeFitMaskF( static_cast<float_m>( fitMask ) ); // create float_v mask
  float_m activeExtraMaskF( static_cast<float_m>(activeExtraMask) );
  assert( (row.NHits() > oldHitIndex || !fitMask).isFull() );
  float_v y = fData.HitPDataY( row, oldHitIndex, activeFitMaskF );
  float_v z = fData.HitPDataZ( row, oldHitIndex, activeFitMaskF );

  
  const float_v dx = x - r.fParam.X();

  float_v dy = y - r.fLastY;
  float_v dz = z - r.fLastZ;
  r.fLastY( activeFitMaskF ) = y;
  r.fLastZ( activeFitMaskF ) = z;
  
  float_v sinPhi = r.fParam.GetSinPhi();

  const float_m fragile = activeFitMaskF &&
      ( static_cast<float_m>( r.fNHits < uint_v(AliHLTTPCCAParameters::MinimumHitsForFragileTracklet) ) || CAMath::Abs( r.fParam.SinPhi() ) >= .99f );
  sinPhi( fragile ) = dy * CAMath::RSqrt( dx * dx + dy * dy );
  
  float_v maxSinPhi = .99f;
  maxSinPhi( activeFitMaskF ) = -1.f;

  
//   float_m transport = r.fParam.TransportToXWithMaterial( x,  fTracker.Param().Bz(), .99f );
//   float_m transport = r.fParam.TransportToX( x, sinPhi, fTracker.Param().cBz(), .99f, activeExtraMaskF );
  
  assert( ( (x == 0) && (activeExtraMaskF || activeFitMaskF) ).isEmpty() );

  float_m transport = r.fParam.TransportToX( x, sinPhi, fTracker.Param().cBz(), maxSinPhi, activeExtraMaskF || activeFitMaskF );
  activeExtraMaskF &= transport;
  activeFitMaskF &= transport;


  if ( row.NHits() < 1 || ( activeExtraMaskF || activeFitMaskF ).isEmpty() ) { 
      // no hits in this row, skip (don't do this before TransportToX)
      // end with extrapolation
    ++r.fStage( r.fRemainingGap == 0 && mask ); // go to WaitingForExtrapolateDown or DoneStage if the gap got too big
#ifndef NODEBUG
    debugF() << "r.fStage: " << r.fStage << " after ExtrapolateTracklet found empty row and fRemainingGap == " << r.fRemainingGap << endl;
#endif
    
      // end with chain fit
    r.fCurrentHitIndex.gather( fData.HitLinkUpData( row ), static_cast<uint_v>( oldHitIndex ), activeFitMask ); // prepare new hit for fit // TODO 2 dir??
    
    const int_m fittingDone = r.fCurrentHitIndex < 0 && activeFitMask;
    ++r.fStage( fittingDone ); // goes to ExtrapolateUp if fitting is done (no other hit linked)
    r.fLastRow( fittingDone ) = rowIndex;

    const int_m invalidTracklet = r.IsInvalid() && fittingDone;
    r.fNHits.setZero( invalidTracklet );
    r.fStage( invalidTracklet || (fittingDone && !r.fIsFragile) ) = DoneStage;
    
    return int_m( Vc::Zero );
  }

  // -- FIND A NEXT HIT --

  
  activeExtraMask = static_cast<int_m>( activeExtraMaskF );

    // try find hit by using links
  int_m linkMask = int_m(Vc::Zero);//activeExtraMask && r.fIsOnChain; - // use links during extrapolation is turned off
  // if ( !linkMask.isEmpty() ) {
  // if ( dir == 1 ) {
  //   if ( rowIndex-1 >= 0 ) {
  //     linkMask &= r.fLastRow == rowIndex-1; // check only hits on previous row. Others can't have link on current row.
  //     const AliHLTTPCCARow &prevRow = fData.Row( rowIndex-1 );
  //     r.fCurrentHitIndex.gather( fData.HitLinkUpData( prevRow ), static_cast<uint_v>( r.fCurrentHitIndex ), linkMask ); // set to next linked hit
  //   }
  //   else linkMask &= int_m(Vc::Zero);
  // }
  // else {
  //   if( rowIndex+1 < fTracker.Param().NRows() ) {
  //     linkMask &= r.fFirstRow == rowIndex+1;
  //     const AliHLTTPCCARow &prevRow = fData.Row( rowIndex+1 );
  //     r.fCurrentHitIndex.gather( fData.HitLinkDownData( prevRow ), static_cast<uint_v>( r.fCurrentHitIndex ), linkMask );
  //   }
  //   else linkMask &= int_m(Vc::Zero);
  // }
  // r.fIsOnChain &= (r.fCurrentHitIndex != -1); // chain is ended
  // linkMask &= (r.fCurrentHitIndex != -1);

  // assert( row.NHits() > static_cast<uint_v>( r.fCurrentHitIndex ) || !linkMask );
  // y.gather( fData.HitDataY( row ), static_cast<uint_v>( r.fCurrentHitIndex ), static_cast<float_m>(linkMask) );
  // z.gather( fData.HitDataZ( row ), static_cast<uint_v>( r.fCurrentHitIndex ), static_cast<float_m>(linkMask) );
  // }
  
  dy = y - r.fParam.Y();
  dz = z - r.fParam.Z();

    // try to find hit by trying all closed hits
  float_v  dy_tmp(Vc::Zero), dz_tmp(Vc::Zero);
  int_m findMask  = activeExtraMask && !linkMask; // try to find only if there is no chain
  if ( !findMask.isEmpty() ) {
    // const int_v prev(r.fCurrentHitIndex); // TODO oldHI

    FindNextHit(r, row, dy_tmp, dz_tmp, findMask);

    // if (dir == 1) {
    //   const int_v prev2( fData.HitLinkDownData( row ), static_cast<uint_v>( r.fCurrentHitIndex ), findMask );
    //   r.fIsOnChain = prev == prev2 && findMask;
    // }
    // else {
    //   const int_v prev2( fData.HitLinkUpData( row ), static_cast<uint_v>( r.fCurrentHitIndex ), findMask );
    //   r.fIsOnChain = prev == prev2 && findMask;
    // }
  }
  activeExtraMask = linkMask || findMask;
  activeExtraMaskF = static_cast<float_m>( activeExtraMask );
  
  dy( static_cast<float_m>( findMask ) ) = dy_tmp;
  dz( static_cast<float_m>( findMask ) ) = dz_tmp;

  // XXX use this branch? with 8 or 16 tracklets extrapolated at the same time this might be an
  // unlikely return? Also see above.
//X   if ( activeExtraMask.isEmpty() ) {
//X     return;
//X   }
  
//    const int_v isUsed2(fData.HitDataIsUsed( row ), static_cast<uint_v>(r.fCurrentHitIndex), activeExtraMask);
//    activeExtraMask &= (isUsed2 != int_v( 2 )) && (isUsed2 != int_v( 1 )); // don't take hits from other chains. In order to reduce calculations.
//    activeExtraMask &= (isUsed2 != int_v( 2 ));

    // check if found hit is acceptable
  float_v err2Y, err2Z;
  fTracker.GetErrors2( rowIndex, r.fParam, &err2Y, &err2Z );

  const float_v kFactor = AliHLTTPCCAParameters::HitPickUpFactor * AliHLTTPCCAParameters::HitPickUpFactor * 3.5f * 3.5f;
  const float_v two( 2.f );

  const float_v sy2 = CAMath::Min( two, kFactor * ( r.fParam.GetErr2Y() + err2Y ) );
  const float_v sz2 = CAMath::Min( two, kFactor * ( r.fParam.GetErr2Z() + err2Z ) );


  activeExtraMaskF &= dy * dy <= sy2 && dz * dz <= sz2;

  activeFitMaskF &= (dy * dy <= sy2 && dz * dz <= sz2) || float_m(r.fNHits < uint_v(3)); // very strong cut for ghosts.

 
    // -- FILTER THE NEXT HIT --
  
  const float_m filtred = r.fParam.FilterDelta( activeExtraMaskF || activeFitMaskF, dy, dz, err2Y, err2Z, .99f );
  activeExtraMaskF &= filtred;
  activeExtraMask = static_cast<int_m>( activeExtraMaskF );
  activeFitMaskF &= filtred;
  const int_m hitAdded = static_cast<int_m>( activeFitMaskF );

  
    // -- SAVE THE NEXT HIT --

  
  trackletVector.SetRowHits( rowIndex, trackIndex,  static_cast<uint_v>(r.fCurrentHitIndex), activeExtraMask || hitAdded );
  
  r.fCurrentHitIndex.gather( fData.HitLinkUpData( row ), static_cast<uint_v>( oldHitIndex ), uint_m(activeFitMask) ); // prepare new hit for fit
  

  ASSERT( ( (row.NHits() > r.fCurrentHitIndex) && activeExtraMask) == activeExtraMask,
          row.NHits() << r.fCurrentHitIndex << activeExtraMask );
  fData.SetHitAsUsedInTrackExtend( row, static_cast<uint_v>( r.fCurrentHitIndex ), activeExtraMask ); // TODO 1 function
  fData.SetHitAsUsedInTrackFit(    row, static_cast<uint_v>( oldHitIndex ),        hitAdded );
  
  ++r.fNHits( static_cast<uint_m>( activeExtraMask || hitAdded ) );
  r.fRemainingGap( activeExtraMask ) = AliHLTTPCCAParameters::MaximumExtrapolationRowGap;

    // -- MOVE TO NEXT STAGE --
  
  ++r.fStage( (r.fRemainingGap == 0) && mask ); // go to WaitingForExtrapolateDown or DoneStage if the gap got too big TODO not active
#ifdef __ASSERT_YF__
  ASSERT( r.fRemainingGap >= 0,
    r.fRemainingGap << mask << activeExtraMask );
#endif
//  r.fStage( r.fNHits > 10 && mask ) = DoneStage; // don't need long tracklets. merdger will do work.
  
  const int_m fittingDone = r.fCurrentHitIndex < 0 && activeFitMask;
  ++r.fStage( fittingDone ); // goes to ExtrapolateUp if fitting is done (no other hit linked)
  r.fLastRow( fittingDone ) = rowIndex;

  const int_m invalidTracklet = r.IsInvalid() && fittingDone;
  r.fNHits.setZero( invalidTracklet );
  r.fStage( invalidTracklet || (fittingDone && !r.fIsFragile) ) = DoneStage; // TODO unmark used hits
  
  r.fEndRow( hitAdded ) = rowIndex;

  r.fLastRow(  activeExtraMask && int_m( dir) ) = rowIndex;
  r.fFirstRow( activeExtraMask && int_m(!dir) ) = rowIndex;
  return activeExtraMask;
} // ExtendTracklet


void AliHLTTPCCATrackletConstructor::run()
{
  
#ifndef NVALGRIND
  for ( int i = 0; i < fTracker.Param().NRows(); ++i ) {
    const AliHLTTPCCARow &row = fData.Row( i );
    for ( int hit = 0; hit < row.NHits(); ++hit ) {
      debugF() << i << ", " << hit << endl;
      const int tmp = fData.HitLinkUpDataS( row, hit );
      VALGRIND_CHECK_VALUE_IS_DEFINED( tmp );
    }
  }
#endif
  assert( *fTracker.NTracklets() < 32768 );
  const int nTracks = *fTracker.NTracklets();

  for ( int trackIteration = 0; trackIteration * uint_v::Size < nTracks; ++trackIteration ) {
    const uint_v trackIndex( uint_v( Vc::IndexesFromZero ) + uint_v(trackIteration * uint_v::Size) );
    const uint_m active = trackIndex < nTracks;
    TrackMemory r;

      //std::cout << "----------------------------------------------------" << endl;
    r.fStage( !active ) = NullStage;

      // if rowStep = 2 TrackletStartHits need to be sorted such that all even start rows come first. The odd start rows - last.
//   r.fStartRow.gather( fTracker.TrackletStartHits(), reinterpret_cast<int AliHLTTPCCAStartHitId::*>( &AliHLTTPCCAStartHitId::fRow ), trackIndex, active );
    r.fStartRow(active) = fTracker.TrackletStartHits()[trackIndex][reinterpret_cast<int AliHLTTPCCAStartHitId::*>( &AliHLTTPCCAStartHitId::fRow )];
//    r.fCurrentHitIndex.gather( fTracker.TrackletStartHits(), reinterpret_cast<int AliHLTTPCCAStartHitId::*>( &AliHLTTPCCAStartHitId::fHit ), trackIndex, active );
    r.fCurrentHitIndex(active) = fTracker.TrackletStartHits()[trackIndex][reinterpret_cast<int AliHLTTPCCAStartHitId::*>( &AliHLTTPCCAStartHitId::fHit )];
    r.fEndRow = static_cast<uint_v>( r.fStartRow );
    r.fLastRow = static_cast<uint_v>( r.fStartRow );
    r.fStartRow( !active ) = std::numeric_limits<int_v>::max();
    r.fFirstRow = r.fStartRow;
//     uint_v length( fTracker.TrackletStartHits(), reinterpret_cast<unsigned int AliHLTTPCCAStartHitId::*>( &AliHLTTPCCAStartHitId::fLength ), trackIndex, active );
    uint_v length(Vc::Zero);
    length(uint_m(active)) = fTracker.TrackletStartHits()[trackIndex][reinterpret_cast<int AliHLTTPCCAStartHitId::*>( &AliHLTTPCCAStartHitId::fLength )];
    const uint_v MaxNHitsForFragileTracklet(6);
    r.fIsFragile = (length < MaxNHitsForFragileTracklet); // TODO parameter // TODO 5 is optimum for globalEff+timeSliceTracker. But one should take into account merger

    const float_v zero( Vc::Zero );
    const float_v one( Vc::One );
    r.fParam.SetSinPhi(  zero );
    r.fParam.SetDzDs(    zero );
    r.fParam.SetQPt(     zero );
    r.fParam.SetSignCosPhi( one );
    r.fParam.SetChi2(    zero );
    r.fParam.SetNDF(       -3 );
    r.fParam.SetCov(  0,  one );
    r.fParam.SetCov(  1, zero );
    r.fParam.SetCov(  2,  one );
    r.fParam.SetCov(  3, zero );
    r.fParam.SetCov(  4, zero );
    r.fParam.SetCov(  5,  one );
    r.fParam.SetCov(  6, zero );
    r.fParam.SetCov(  7, zero );
    r.fParam.SetCov(  8, zero );
    r.fParam.SetCov(  9,  one );
    r.fParam.SetCov( 10, zero );
    r.fParam.SetCov( 11, zero );
    r.fParam.SetCov( 12, zero );
    r.fParam.SetCov( 13, zero );
    r.fParam.SetCov( 14, 10.f );

      //     const int_m evenRows = ( r.fStartRow & int_v( Vc::One ) ) > int_v( Vc::Zero );
      // 
      //     // fStartRow should be sorted, with the one exception of odd/even boundary
      //     if ( r.fStartRow != r.fStartRow.sorted() ) {
      //       debugF() << r.fStartRow << r.fStartRow.sorted() << evenRows << evenRows.isMix() << endl;
      //       assert( r.fStartRow == r.fStartRow.sorted() || evenRows.isMix() );
      //     }
    {
      InitTracklets init( r, fData, fTracker, fTrackletVectors[trackIteration], trackIndex );
      r.fStartRow.callWithValuesSorted( init );
    }
    
//#define USE_COUNTERS
#ifdef USE_COUNTERS
    static unsigned int counters[6] = { 0, 0, 0, 0, 0, 0 };
    counters[0]++;
#endif // USE_COUNTERS
    
    const int rowStep = AliHLTTPCCAParameters::RowStep;
    { // fit and extrapolate upwards
      int rowIndex = r.fStartRow.min() + rowStep*2;
      const int_v activationRow = r.fStartRow + rowStep*2;
      debugF() << "============================================= Start Fitting Upwards =============================================" << endl;
      // TODO think about getting parameters of all tracks (fragile and !fragile) in the same point (begin or end of track). Needed by Merger
// #define DISABLE_HIT_SEARCH
#ifdef DISABLE_HIT_SEARCH
     while ( rowIndex < fTracker.Param().NRows() && !( r.fStage <= FitLinkedHits ).isEmpty() ) {
#ifdef USE_COUNTERS
       counters[1]++;
#endif // USE_COUNTERS
       
        ++r.fStage( rowIndex == activationRow ); // goes to FitLinkedHits on activation row
        FitTracklet( r, rowIndex, trackIndex, fTrackletVectors[trackIteration] );
        ++rowIndex;
      }


      const int_m ready = r.fStage <= DoneStage;
      const float_v x( fData.RowX(), float_v::IndexType( r.fEndRow ) );
//      const float_v x( fData.RowX(), float_v::IndexType( r.fStartRow ) );
      debugF() << x << float_v::IndexType( r.fEndRow ) << float_v( fData.RowX(), float_v::IndexType( Vc::IndexesFromZero ) ) << endl;
      assert( ( x == 0 && static_cast<float_m>( ready ) ).isEmpty() );
      //assert ( ( (r.fParam.X() == x) && static_cast<float_m>( ready )) == static_cast<float_m>( ready ));

      const int_m transported = static_cast<int_m>( r.fParam.TransportToX(
                                                           x, fTracker.Param().cBz(), .999f, static_cast<float_m>( ready ) ) );
      
      debugF() << "============================================= Stop Fitting Upwards ==============================================" << endl;
#ifdef MAIN_DRAW
      if ( AliHLTTPCCADisplay::Instance().DrawType() == 1 ) {
        foreach_bit( int ii, r.fStage < DoneStage ) {
          TrackParam t( r.fParam, ii );
          AliHLTTPCCADisplay::Instance().ClearView();
          AliHLTTPCCADisplay::Instance().DrawSlice( &fTracker, 0 );
          AliHLTTPCCADisplay::Instance().DrawSliceHits();
          AliHLTTPCCADisplay::Instance().DrawTrackParam( t );
          AliHLTTPCCADisplay::Instance().Ask();
        }
      }
#endif
#else // DISABLE_HIT_SEARCH
        // fit all chains
      while ( rowIndex < fTracker.Param().NRows() && ( r.fStage == ExtrapolateUp ).isEmpty() ) {
#ifdef USE_COUNTERS
        counters[1]++;
#endif // USE_COUNTERS
        
        r.fStage( rowIndex == activationRow ) = FitLinkedHits; // goes to FitLinkedHits on activation row
                
        ExtendTracklet( r, rowIndex, trackIndex, fTrackletVectors[trackIteration], 1, int_m(Vc::Zero) );
        // FitTracklet( r, rowIndex, trackIndex, fTrackletVectors[trackIteration] );
        ++rowIndex;
      }

      debugF() << "========================================== Start Extrapolating Upwards ==========================================" << endl;

        // some chains are fitted, some - are extrapolated
#ifdef __ASSERT_YF__	   
      assert( r.fRemainingGap >= 0 );
#endif
      //while ( rowIndex < fTracker.Param().NRows() && !( r.fStage <= FitLinkedHits ).isEmpty() ) {
      while ( rowIndex < fTracker.Param().NRows() && !( r.fStage <= ExtrapolateUp ).isEmpty() ) {
#ifdef USE_COUNTERS
       counters[2]++;
#endif // USE_COUNTERS

        r.fStage( rowIndex == activationRow ) = FitLinkedHits; // goes to FitLinkedHits on activation row
        const int_m toExtrapolate = (r.fStage == ExtrapolateUp);
        ExtendTracklet( r, rowIndex, trackIndex, fTrackletVectors[trackIteration], 1, toExtrapolate );
        ++rowIndex;
      }

      debugF() << "============================================= Stop Fitting Upwards ==============================================" << endl;

        // end extrapolate chains
      int_m mask;
#ifdef __ASSERT_YF__	   
      assert( r.fRemainingGap >= 0 );
#endif
      while ( rowIndex < fTracker.Param().NRows() && !( mask = r.fStage == ExtrapolateUp ).isEmpty() ) {
#ifdef USE_COUNTERS
        counters[3]++;
#endif // USE_COUNTERS

#ifdef __ASSERT_YF__	   
        assert ( r.fStage != FitLinkedHits );
#endif
        ExtrapolateTracklet( r, rowIndex, trackIndex, fTrackletVectors[trackIteration], 1, mask );
        ++rowIndex;
      }
      debugF() << "=========================================== Stop Extrapolating Upwards ==========================================" << endl;
    }

      // r.fStage (r.fNHits < MaxNHitsForFragileTracklet) = DoneStage; // TODO investigate
    { // extrapolate downwards
      r.fRemainingGap = AliHLTTPCCAParameters::MaximumExtrapolationRowGap; // allow full gaps again
      ++r.fStage( r.fStage == ExtrapolateUp ); // FitLinkedHits/ExtrapolateUp went so high that no gap put the tracklet into WaitingForExtrapolateDown
      const int_m ready = r.fStage == WaitingForExtrapolateDown;
      debugF() << "ready to extrapolate downwards: " << ready << endl;
      ++r.fStage( ready ); // the wait is over

      if(1){ // set track parameters to x of end row of the fitting stage
        //const float_v x( fData.RowX(), float_v::IndexType( r.fEndRow ) );
        const float_v x( fData.RowX(), float_v::IndexType( r.fStartRow ), static_cast<float_m>(ready));
        debugF() << x << float_v::IndexType( r.fEndRow ) << float_v( fData.RowX(), float_v::IndexType( Vc::IndexesFromZero ) ) << endl;
        assert( ( x == 0 && static_cast<float_m>( ready ) ).isEmpty() );
        const int_m transported = static_cast<int_m>( r.fParam.TransportToX(
                                                            x, fTracker.Param().cBz(), .999f, static_cast<float_m>( ready ) ) );
        ++r.fStage( !transported ); // all those where transportation failed go to DoneStage
      }
#ifdef MAIN_DRAW
      if ( AliHLTTPCCADisplay::Instance().DrawType() == 1 ) {
        for(int ii=0; ii<int_v::Size; ii++)
        {
          if(!(r.fStage[ii] < DoneStage)) continue;
//         foreach_bit( int ii, r.fStage < DoneStage ) {
          TrackParam t( r.fParam, ii );
          AliHLTTPCCADisplay::Instance().ClearView();
          AliHLTTPCCADisplay::Instance().DrawSlice( &fTracker, 0 );
          AliHLTTPCCADisplay::Instance().DrawSliceHits();
          AliHLTTPCCADisplay::Instance().DrawTrackParam( t, 2 );
          AliHLTTPCCADisplay::Instance().Ask();
        }
      }
#endif
      debugF() << "========================================= Start Extrapolating Downwards =========================================" << endl;
      // int rowIndex = r.fEndRow.max() - 1;
      int_v tmpRow(r.fStartRow); // take only one hit from fitted chain
      tmpRow(!active) = fTracker.Param().NRows()-1;
      int rowIndex = tmpRow.max();
        // int rowIndex = r.fLastRow.max() - 1;
        // extrapolate along fitted chains
      int_m mask; 
      const int minMaskedExtrRow = r.fStartRow.min();
      while ( rowIndex >= minMaskedExtrRow && !( mask = r.fStage == ExtrapolateDown ).isEmpty() ) {
#ifdef USE_COUNTERS
       counters[4]++;
#endif // USE_COUNTERS
        // mask &= rowIndex < r.fEndRow;
       mask &= rowIndex < r.fStartRow + 1;
       mask &= ( ( rowIndex - r.fStartRow ) & int_v( std::numeric_limits<int_v::EntryType>::min() + 1 ) ) != int_v( Vc::Zero ); // CHECKME why do we need this?
       ExtrapolateTracklet( r, rowIndex, trackIndex, fTrackletVectors[trackIteration], 0, mask );
       --rowIndex;
      }
        // extrapolote downwards and find addition hits
      while ( rowIndex >= 0 && !( mask = r.fStage == ExtrapolateDown ).isEmpty() ) {
#ifdef USE_COUNTERS
       counters[5]++;
#endif // USE_COUNTERS
       ExtrapolateTracklet( r, rowIndex, trackIndex, fTrackletVectors[trackIteration], 0, mask );
        --rowIndex;
      }
      r.fFirstRow = CAMath::Min( r.fFirstRow, r.fStartRow );
#ifdef MAIN_DRAW
      if ( AliHLTTPCCADisplay::Instance().DrawType() == 1 ) {
        for(int ii=0; ii<int_v::Size; ii++)
        {
          if(!(r.fStage[ii] < NullStage)) continue;
//         foreach_bit( int ii, r.fStage < NullStage ) {
          TrackParam t( r.fParam, ii );
          AliHLTTPCCADisplay::Instance().ClearView();
          AliHLTTPCCADisplay::Instance().DrawSlice( &fTracker, 0 );
          AliHLTTPCCADisplay::Instance().DrawSliceHits();
          AliHLTTPCCADisplay::Instance().DrawTrackParam( t, 4 );
          AliHLTTPCCADisplay::Instance().Ask();
        }
      }
#endif
    
        ///mvz start
        //std::cout << "fitted   " << r.fParam.QPt() <<std::endl;
        //r.fParam.SetQPt(     qpt_approx);
        ///mvz end
    
#endif // DISABLE_HIT_SEARCH
    }
    
#ifdef USE_COUNTERS
    std::cout << "Counters= " << counters[0] << " " << counters[1] << " " << counters[2] << " " << counters[3] << " " << counters[4] << " " << counters[5] << std::endl;
#endif // USE_COUNTERS
       
    float_m trackletOkF( r.fNHits >= uint_v(AliHLTTPCCAParameters::MinimumHitsForTracklet) );
    for ( int i = 0; i < 15; ++i ) {
      trackletOkF &= CAMath::Finite( r.fParam.Cov()[i] );
    }
    for ( int i = 0; i < 5; ++i ) {
      trackletOkF &= CAMath::Finite( r.fParam.Par()[i] );
    }

      // 80 is row 0; if X is that small this track is garbage.
      // XXX does this happen at all? If yes, under what conditions?
    assert( ( r.fParam.X() > 50.f && trackletOkF ) == trackletOkF );
    trackletOkF &= r.fParam.X() > 50.f // TODO: read from file!!! 


        // there must be errors and they must be positive or this track is garbage
      && r.fParam.Err2QPt()    > float_v(Vc::Zero);
    trackletOkF &= r.fParam.Err2Y()      > float_v(Vc::Zero) && r.fParam.Err2Z()      > float_v(Vc::Zero);
    trackletOkF &= r.fParam.Err2SinPhi() > float_v(Vc::Zero) && r.fParam.Err2DzDs()   > float_v(Vc::Zero);
//    trackletOkF &= ( r.fParam.Chi2()/static_cast<float_v>(r.fParam.NDF()) < 25.f ); // TODO 
    debugF() << r.fParam << "-> trackletOk: " << trackletOkF << endl;

    const int_m trackletOk( trackletOkF );
    r.fNHits.setZero( !trackletOk );

      //////////////////////////////////////////////////////////////////////
      //
      //////////////////////////////////////////////////////////////////////

    TrackletVector &tracklet = fTrackletVectors[trackIteration];
    tracklet.SetNHits( r.fNHits );

    if ( !( r.fNHits > 0 ).isEmpty() ) {
#ifdef MAIN_DRAW
      if ( AliHLTTPCCADisplay::Instance().DrawType() == 1 ) {
        for(int ii=0; ii<int_v::Size; ii++)
        {
          if(!(r.fStage[ii] < DoneStage)) continue;
//         foreach_bit( int ii, r.fStage < DoneStage ) {
          TrackParam t( r.fParam, ii );
          AliHLTTPCCADisplay::Instance().ClearView();
          AliHLTTPCCADisplay::Instance().DrawSlice( &fTracker, 0 );
          AliHLTTPCCADisplay::Instance().DrawSliceHits();
          AliHLTTPCCADisplay::Instance().DrawTrackParam( t, 2 );
          AliHLTTPCCADisplay::Instance().Ask();
        }
      }
#endif
        // start and end rows of the tracklet
      tracklet.SetFirstRow( static_cast<uint_v>( r.fFirstRow ) );
      tracklet.SetLastRow( r.fLastRow );

        ///mvz start 25.01.2010
      const float_m MinQPt = CAMath::Abs(r.fParam.QPt()) < AliHLTTPCCAParameters::MinimumQPt;
      float_v NewQPt = r.fParam.QPt();
      NewQPt(MinQPt) = AliHLTTPCCAParameters::MinimumQPt;
      r.fParam.SetQPt(NewQPt);
        //      r.fParam.SetQPt( CAMath::Max( AliHLTTPCCAParameters::MinimumQPt, r.fParam.QPt() ) );
        ///mvz end 25.01.2010
      tracklet.SetParam( r.fParam );

      debugTS() << r.fFirstRow << r.fLastRow << endl;
      debugTS() << "set hit weigths from row " << r.fFirstRow.min() << " until row " << r.fLastRow.max() << endl;
      const uint_v &weight = SliceData::CalculateHitWeight( r.fNHits, trackIndex );
        // for all rows where we have a hit let the fTracker know what weight our hits have
      for ( unsigned int rowIndex = r.fFirstRow.min(); rowIndex <= r.fLastRow.max(); ++rowIndex ) {
        const uint_v &hitIndex = tracklet.HitIndexAtRow( rowIndex );
        debugTS() << "MaximizeHitWeight at " << rowIndex << hitIndex << " with " << weight << endl;
        fData.MaximizeHitWeight( fData.Row( rowIndex ), hitIndex, weight );
      }
    }
  }
}

void InitTracklets::operator()( int rowIndex )
{
  if ( ISUNLIKELY( rowIndex >= fTracker.Param().NRows() ) ) {
    return;
  }
  debugF() << "InitTracklets(" << rowIndex << ")" << endl;
    //std::cout<< "InitTracklets(" << rowIndex << ")" << std::endl;
  const int rowStep = AliHLTTPCCAParameters::RowStep;
  assert( rowIndex < fTracker.Param().NRows() - rowStep );
    // the first hit is a special case (easy)
  int_m mask = rowIndex == r.fStartRow;
  float_m maskF( mask );
  {
    const AliHLTTPCCARow &row = fData.Row( rowIndex );
    VALGRIND_CHECK_MEM_IS_ADDRESSABLE( &row, sizeof( AliHLTTPCCARow ) );
    const float_v x = fData.RowX( rowIndex );
    const uint_v &hitIndex = static_cast<uint_v>( r.fCurrentHitIndex );
    const float_v y = fData.HitPDataY( row, hitIndex, maskF );
    const float_v z = fData.HitPDataZ( row, hitIndex, maskF );
    r.fParam.SetX( x, maskF );
    r.fParam.SetY( y, maskF );
    r.fParam.SetZ( z, maskF );
    fTrackletVector.SetRowHits( rowIndex, trackIndex, hitIndex, mask );

      // mark first hit as used
    const int_v isUsed(fData.HitDataIsUsed( row ), static_cast<uint_v>( r.fCurrentHitIndex ), mask);
    fData.SetHitAsUsedInTrackFit( row, static_cast<uint_v>( r.fCurrentHitIndex ), mask && ( isUsed == int_v(1) ) );

    r.fCurrentHitIndex.gather( fData.HitLinkUpData( row ), hitIndex, mask ); // set to next linked hit
    // the first hit in the Tracklet is guaranteed to have a link up, since StartHitsFinder
    // ensures it
    assert( ( r.fCurrentHitIndex >= 0 && mask ) == mask );
  }

  mask &= (r.fCurrentHitIndex >= 0);
  maskF &= static_cast<float_m>(r.fCurrentHitIndex >= 0);
  rowIndex += rowStep;
  {
    const AliHLTTPCCARow &row = fData.Row( rowIndex );
    const float_v x = fData.RowX( rowIndex );
    const uint_v &hitIndex = static_cast<uint_v>( r.fCurrentHitIndex );
    const float_v y = fData.HitPDataY( row, hitIndex, maskF );
    const float_v z = fData.HitPDataZ( row, hitIndex, maskF );
    const float_v &dx = x - r.fParam.X();
    const float_v &dy = y - r.fParam.Y();
    const float_v &dz = z - r.fParam.Z();
    const float_v &ri = float_v( Vc::One ) * CAMath::RSqrt( dx * dx + dy * dy );
    const float_v &sinPhi = dy * ri;
    r.fParam.SetSinPhi( sinPhi,  maskF );
    r.fParam.SetDzDs  ( dz * ri, maskF );
    float_v err2Y, err2Z;
///mvz start 20.01.2010
//    fTracker.GetErrors2( rowIndex, r.fParam, &err2Y, &err2Z );
    fTracker.GetErrors2( rowIndex, r.fParam, &err2Y, &err2Z );
///mvz end 20.01.2010
    r.fParam.SetCov( 0, err2Y, maskF );
    r.fParam.SetCov( 2, err2Z, maskF );

    const float_m transported = r.fParam.TransportToX( x, sinPhi, fTracker.Param().cBz(), -1.f, maskF );
    // assert( transported == maskF );
///mvz start 20.01.2010
//    fTracker.GetErrors2( rowIndex, r.fParam.GetZ(), sinPhi, r.fParam.GetDzDs(), &err2Y, &err2Z );
    fTracker.GetErrors2( rowIndex, r.fParam, &err2Y, &err2Z );
///mvz end 20.01.2010
    const int_m hitAdded( r.fParam.Filter( maskF, y, z, err2Y, err2Z, .99f ) );
    // assert( hitAdded == mask );
    UNUSED_PARAM2( transported, hitAdded );
    fTrackletVector.SetRowHits( rowIndex, trackIndex, hitIndex, mask );

    r.fLastY( maskF ) = y;
    r.fLastZ( maskF ) = z;
    
      // mark 2-nd hit as used
    const int_v isUsed(fData.HitDataIsUsed( row ), static_cast<uint_v>( r.fCurrentHitIndex ), mask);
    fData.SetHitAsUsedInTrackFit( row, static_cast<uint_v>( r.fCurrentHitIndex ), static_cast<int_m>(mask) && ( isUsed == int_v(1) ) );
    
    r.fCurrentHitIndex.gather( fData.HitLinkUpData( row ), hitIndex, mask ); // set to next linked hit

    // the second hit in the Tracklet is also guaranteed to have a link up, since StartHitsFinder
    // ensures it
    assert( ( r.fCurrentHitIndex >= 0 && mask ) == mask );
  }
}

