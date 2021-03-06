#include <assert.h>
#include "Riostream.h"
#include "StG2TrackVertexMap.h"
#include "TVector3.h"
#include "StMessMgr.h"
StG2TrackVertexMap *StG2TrackVertexMap::fgInstance = 0;
using namespace std;
static Int_t _debug = 0;
//________________________________________________________________________________
StG2TrackVertexMap *StG2TrackVertexMap::instance(St_g2t_track *track, St_g2t_vertex *vertex) {
  if (! fgInstance) {fgInstance = new StG2TrackVertexMap(track,vertex); }
  else if (track && vertex) {
    fgInstance->Reset(track,vertex);
  }
  return fgInstance;
}
//________________________________________________________________________________
void StG2TrackVertexMap::Reset(St_g2t_track *track, St_g2t_vertex *vertex) {
  fTrack2Vertex.clear();
  fVertex2ParentTrack.clear();
  fTrack = track;
  fVertex = vertex;
  if (! track ||  ! vertex) return;
  Int_t Nt = track->GetNRows();
  Int_t NV = vertex->GetNRows();
  g2t_track_st *t = track->GetTable();
  g2t_vertex_st *v = vertex->GetTable();
  for (Int_t i = 0; i < Nt; i++) {
    Int_t IdT = t[i].id - 1;                 
    if (! (IdT >= 0 && IdT < Nt)) {
      LOG_ERROR << " StG2TrackVertexMap::Reset Illegal IdT = " << IdT + 1 << " and Nt = " << Nt << endm;
      //      assert(IdT >= 0 && IdT < Nt);
      continue;
    }
    Int_t IdV = t[i].start_vertex_p - 1;     
    if (! (IdV >= 0 && IdV < NV)) {
      LOG_ERROR << " StG2TrackVertexMap::Reset Illegal IdV = " << IdV + 1 << " and NV = " << NV << endm;
      //      assert(IdV >= 0 && IdV < NV);
      continue;
    }
    TVector3 Vx1(v[IdV].ge_x);
    Float_t tof1 = v[IdV].ge_tof;
    Int_t IdP = v[IdV].parent_p - 1;         
    if (IdP >= 0 && IdP < Nt) {
      Int_t IdVxP = t[IdP].start_vertex_p - 1; 
      if (! (IdVxP >= 0 && IdVxP < NV)) {
	LOG_ERROR << " StG2TrackVertexMap::Reset Illegal IdVxP = " << IdV + 1 << " and NV = " << NV << endm;
	//      assert(IdVxP >= 0 && IdVxP < NV);
	continue;
      }
      TVector3 Vx2(v[IdVxP].ge_x);
      Vx2 -= Vx1;
      while ((IdVxP > 0 && IdV != IdVxP && Vx2.Mag() < 10e-4 && TMath::Abs(tof1-v[IdVxP].ge_tof) < 1e-9)) {
	IdV = IdVxP;
	IdP = v[IdV].parent_p - 1; if (IdP < 0) break;
	IdVxP = t[IdP].start_vertex_p - 1;  if (IdVxP < 0) break;
	Vx2 = TVector3(v[IdVxP].ge_x);
	Vx2 -= Vx1;
      }
    }
    fTrack2Vertex[IdT+1] = IdV+1;
  }
  for (Int_t i = 0; i < NV; i++) {
    fVertex2ParentTrack[v[i].id] = v[i].parent_p;
  }
  if (_debug) {
    Print();
  }
}
//________________________________________________________________________________
void StG2TrackVertexMap::Print(Option_t *option) const {
  LOG_INFO << "StG2TrackVertexMap:";
  if (fVertex) LOG_INFO << " No.g2tVertices = " << fVertex->GetNRows();
  if (fTrack)  LOG_INFO << " No.g2tTracks = " << fTrack->GetNRows();
  LOG_INFO << endm;
  LOG_INFO << "Track to Vertex map" << endm;
  Int_t t = 0;
  for (auto x : fTrack2Vertex) {
    if (t > 0 && t%5 == 0) LOG_INFO << endm;
    LOG_INFO << "V[" << x.first << "] = " << x.second << "\t" << IdVertex(x.first) << "\t";
    t++;
  }
  LOG_INFO << endm;
  LOG_INFO << "Vertex to Parent track map" << endm;
  t = 0;
  for (auto x : fVertex2ParentTrack) {
    if (t > 0 && t%5 == 0) LOG_INFO << endm;
    LOG_INFO << "T[" << x.first << "] = " << x.second << "\t" << IdParentTrack(x.first) << "\t";
    t++;
  }
  LOG_INFO << endm;
}
