#ifndef SHISOLTRACK
#define SHISOLTRACK

//this class is a basic track used for storing all the tracks near the electron
//it is intended to be very lightweight to save diskspace

#include "TVector3.h"

namespace reco{
  class Track;
}

class SHIsolTrack : public TObject {

private:
  TVector3 p3_; //the momentum
  TVector3 vtxPos_; //the vtx position
  bool posCharge_; //if the charge is positive, new for v2
  int vertexNr_;
  float chi2_;
  int ndof_;
  int algosAndQual_; //packed with algoID, prevAlgoID and quailty 
                     //(note quality is bits rather than number, breaking with CMSSW, 9th bit is undefined)
  int nrHits_; 
  int nrLostHits_;
  float ptErr_;

  static constexpr int kAlgoIdMask=0x3F;
  static constexpr int kAlgoIdSize=6;
  static constexpr int kQualMask=0x1FF;
  static constexpr int kQualSize=9;

public:
  SHIsolTrack();
  SHIsolTrack(const reco::Track& trk,int vertexNr);
  //SHIsolTrack(const TVector3& p3,const TVector3& vtxPos,bool posCharge,int vertexNr,float chi2,int ndof,int algosAndQual);
  
  const TVector3& p3()const{return p3_;}
  const TVector3& vtxPos()const{return vtxPos_;}
  int charge()const{return posCharge_ ? 1 : -1;}
  float dz()const;
  float pt()const{return p3().Pt();}
  int vertexNr()const{return vertexNr_;}
  int ndof()const{return ndof_;}
  float chi2()const{return chi2_;}
  int algo()const{return algosAndQual_&kAlgoIdMask;}
  int prevAlgo()const{return (algosAndQual_>>kAlgoIdSize)&kAlgoIdMask;}
  int quality()const{return (algosAndQual_>>kAlgoIdSize*2)&kQualMask;}
  int nrHits()const{return nrHits_;}
  int nrLostHits()const{return nrLostHits_;}
  float ptErr()const{return ptErr_;}
  static int packAlgoIDInfo(int algoId,int prevAlgoId,int quality);
  static void unpackAlgoIDInfo(const int packedId,int& algoId,int& prevAlgoId,int& quality); //mainly for debuging
  ClassDef(SHIsolTrack,6)
};

#endif
