#ifndef SHVERTEX_H
#define SHVERTEX_H

#include "TLorentzVector.h"
#include "TVector3.h"
#include "TObject.h"

namespace reco{
  class Vertex;
}

class SHVertex : public TObject {
private:
  float vx_;
  float vy_;
  float vz_;

  float chi2_;
  float sumPt_;
  
  //bit packed
  //bit 0 : isValid
  //bit 1-10 : ndof (0-1023)
  //bit 11-20 : nrtracks (0-1023)
  int data_;
  
  static constexpr int kIsValidMask=0x1;
  static constexpr int kIsValidOffset=0;
  static constexpr int kNDOFMask=0x3FF;
  static constexpr int kNDOFOffset=__builtin_popcount(kIsValidMask)+kIsValidOffset;
  static constexpr int kNrTrksMask=0x3FF;
  static constexpr int kNrTrksOffset=__builtin_popcount(kNDOFMask)+kNDOFOffset;
  
public:
  SHVertex():vx_(0.),vy_(0.),vz_(0.),chi2_(-1),sumPt_(-1),data_(0){}
  SHVertex(const reco::Vertex& vertex);
  //debuging
  void setData(int val){data_=val;}

  TVector3 pos()const{return TVector3(vx_,vy_,vz_);}
  float vx()const{return vx_;}
  float vy()const{return vy_;}
  float vz()const{return vz_;}
  float chi2()const{return chi2_;}
  float sumPt()const{return sumPt_;}
  int ndof()const{return (data_>>kNDOFOffset) & kNDOFMask;}
  int nrTracks()const{return (data_>>kNrTrksOffset) & kNrTrksMask;}
  bool isValid()const{return (data_>>kIsValidOffset) & kIsValidMask;}
  
  static int packData(const bool isValid,const int ndof,const int nrTrks);
  
  ClassDef(SHVertex,1)

};


#endif
