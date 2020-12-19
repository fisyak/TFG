//-*- Mode: C++ -*-
// $Id: AliHLTTPCCAGBTrack.h,v 1.1 2013/01/28 21:19:03 yiguo Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef ALIHLTTPCCAGBTRACK_H
#define ALIHLTTPCCAGBTRACK_H


#include "AliHLTTPCCADef.h"
#include "AliHLTTPCCATrackParam.h"

namespace std
{
  template<typename T> struct char_traits;
  template<typename _CharT, typename _Traits> class basic_istream;
  typedef basic_istream<char, char_traits<char> > istream;
  template<typename _CharT, typename _Traits> class basic_ostream;
  typedef basic_ostream<char, char_traits<char> > ostream;
} // namespace std

/**
 * @class AliHLTTPCCAGBTrack
 *
 *
 */
class AliHLTTPCCAGBTrack
{
    friend std::istream &operator>>( std::istream &, AliHLTTPCCAGBTrack & );
    friend std::ostream &operator<<( std::ostream &, const AliHLTTPCCAGBTrack & );
  public:

    AliHLTTPCCAGBTrack()
      : fInnerParam()
      , fOuterParam()
      , fAlpha( 0 )
      , fDeDx( 0 )
      , fOuterAlpha( 0 )
      , fNDeDx(0)
      , fFirstHitRef( 0 )
      , fNHits( 0 )
      , fIsLooper(0)
      , fReco(false)
      , fIsClone(false)
      , fIsMerged(false)
      , fReverse(false)
    {}

    int NHits()               const { return fNHits; }
    int FirstHitRef()         const { return fFirstHitRef; }
    const AliHLTTPCCATrackParam &Param() const { return InnerParam(); }
    const AliHLTTPCCATrackParam &InnerParam() const { return fInnerParam; }
    const AliHLTTPCCATrackParam &OuterParam() const { return fOuterParam; }
    float Alpha()            const { return fAlpha; }
    int   NDeDx()            const { return fNDeDx; }
    float DeDx()             const { return fDeDx; }


    void SetNHits( int v )                 {  fNHits = v; }
    void SetFirstHitRef( int v )           {  fFirstHitRef = v; }
    void SetInnerParam( const AliHLTTPCCATrackParam &v ) {  fInnerParam = v; }
    void SetOuterParam( const AliHLTTPCCATrackParam &v ) {  fOuterParam = v; }
    void SetAlpha( float v )               {  fAlpha = v; }
    void SetNDeDx( int n)                  {  fNDeDx = n; }
    void SetDeDx( float v )                {  fDeDx = v; }


    static bool ComparePNClusters( const AliHLTTPCCAGBTrack *a, const AliHLTTPCCAGBTrack *b ) {
      return ( a->fNHits > b->fNHits );
    }

    void SetMerged() { fIsMerged = true; }
    bool IsMerged() const { return fIsMerged; }
    void SetClone( bool c = true ) { fIsClone = c; }
    bool IsClone() const { return fIsClone; }
#ifdef LOOPER_TEST
    void SetFirstMC( int mc ) { fFirstMC = mc; }
    int GetFirstMC() const { return fFirstMC; }
#endif
    float OuterAlpha()            const { return fOuterAlpha; }
    void SetOuterAlpha( float v )               {  fOuterAlpha = v; }
    void SetReco( bool r ) { fReco = r; }
    bool IsReco() const { return fReco; }
    void SetLooper() { fIsLooper++; }
    void SetLooperClone() { fIsLooper = -1; }
    int IsLooper() { return fIsLooper; }
    int IsLooper() const { return fIsLooper; }
    void SetReverse( bool r = true ) { fReverse = r; }
    bool IsReverse() const { return fReverse; }

    void ReverseInnerPar() { fInnerParam.ReversePar(); }
    void ReverseOuterPar() { fOuterParam.ReversePar(); }

  protected:

    AliHLTTPCCATrackParam fInnerParam; // fitted track parameters
    AliHLTTPCCATrackParam fOuterParam;
    float fAlpha;             //* Alpha angle of the parametrerisation
    float fDeDx;              //* DE/DX
    float fOuterAlpha;
    int   fNDeDx;
    int fFirstHitRef;        // index of the first hit reference in track->hit reference array
    int fNHits;              // number of track hits
    int fIsLooper;
#ifdef LOOPER_TEST
    int fFirstMC;
#endif

    bool fReco;
    bool fIsClone;
    bool fIsMerged;
    bool fReverse;
};

std::istream &operator>>( std::istream &, AliHLTTPCCAGBTrack & );
std::ostream &operator<<( std::ostream &, const AliHLTTPCCAGBTrack & );

#endif
