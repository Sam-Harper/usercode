#ifndef SHISOLTRACK
#define SHISOLTRACK

//this class is a basic track used for storing all the tracks near the electron
//it is intended to be very lightweight to save diskspace

#include "TVector3.h"
#include <bitset>
namespace reco{
  class Track;
}

class SHIsolTrack : public TObject {

private:
  float pt_;
  float eta_;
  float phi_;
  float vx_;
  float vy_;
  float vz_;
  float chi2_; 
  float ptErr_;
  
  int algosAndQual_; //packed with algoID, prevAlgoID and quailty 
                     //(note quality is bits rather than number, breaking with CMSSW, 9th bit is undefined)
  //track data
  //bit 0 = +ve charge
  //bit 1-7 = vertex number (0-127)
  //bit 8-15 = ndof (0-255)
  int16_t trkData_;
  //bit 0-6 = nr hits (0-127)
  //bit 7-11 = nr layers  (0-31)
  //bit 12-15 = nr lost hits (0-15)
  //bit 16-19 = nr pixel hits (0-31)
  int hitData_;
 

  static constexpr int kAlgoIdMask=0x3F;
  static constexpr int kAlgoIdSize=6;
  static constexpr int kQualMask=0x1FF;
  static constexpr int kQualSize=9;
  
  //using gcc instrinsic to get the number of bits of the previous mask automatically
  static constexpr int kChargeMask=0x1;
  static constexpr int kChargeOffset=0;
  static constexpr int kVertexNrMask=0x7F;
  static constexpr int kVertexNrOffset=__builtin_popcount(kChargeMask)+kChargeOffset;
  static constexpr int kNDOFMask=0xFF;
  static constexpr int kNDOFOffset=__builtin_popcount(kVertexNrMask)+kVertexNrOffset;
  
  static constexpr int kNrHitsMask=0x7F;
  static constexpr int kNrHitsOffset=0;
  static constexpr int kNrLayersMask=0x1F;
  static constexpr int kNrLayersOffset=__builtin_popcount(kNrHitsMask)+kNrHitsOffset;
  static constexpr int kNrLostHitsMask=0x0F;
  static constexpr int kNrLostHitsOffset=__builtin_popcount(kNrLayersMask)+kNrLayersOffset;
  static constexpr int kNrPixelHitsMask=0x1F;
  static constexpr int kNrPixelHitsOffset=__builtin_popcount(kNrLostHitsMask)+kNrLostHitsOffset;
  
  
    
public:
  SHIsolTrack();
  SHIsolTrack(const reco::Track& trk,int vertexNr);
  //SHIsolTrack(const TVector3& p3,const TVector3& vtxPos,bool posCharge,int vertexNr,float chi2,int ndof,int algosAndQual);
   //debuging
  void setAlgosAndQual(int val){algosAndQual_=val;}
  void setTrkData(uint16_t val){trkData_=val;}
  void setHitData(int val){hitData_=val;}

  TVector3 p3()const{TVector3 tmp;tmp.SetPtEtaPhi(pt_,eta_,phi_);return tmp;}
  TVector3 vtxPos()const{return TVector3(vx_,vy_,vz_);}
  int charge()const{return ((trkData_>>kChargeOffset)&kChargeMask)!=0 ? 1 : -1;}
  float dz()const;
  float pt()const{return pt_;}
  float eta()const{return eta_;}
  float phi()const{return phi_;}
  float vx()const{return vx_;}
  float vy()const{return vy_;}
  float vz()const{return vz_;}
  int vertexNr()const{return (trkData_>>kVertexNrOffset)&kVertexNrMask;}
  int ndof()const{return (trkData_>>kNDOFOffset)&kNDOFMask;}
  float chi2()const{return chi2_;}
  int algo()const{return algosAndQual_&kAlgoIdMask;}
  int prevAlgo()const{return (algosAndQual_>>kAlgoIdSize)&kAlgoIdMask;}
  int quality()const{return (algosAndQual_>>kAlgoIdSize*2)&kQualMask;}
  int nrHits()const{return (hitData_>>kNrHitsOffset)&kNrHitsMask;}
  int nrLayers()const{return (hitData_>>kNrLayersOffset)&kNrLayersMask;}
  int nrPixelHits()const{return (hitData_>>kNrPixelHitsOffset)&kNrPixelHitsMask;}
  int nrLostHits()const{return (hitData_>>kNrLostHitsOffset)&kNrLostHitsMask;}
  float ptErr()const{return ptErr_;}
  static int packAlgoIDInfo(int algoId,int prevAlgoId,int quality);
  static void unpackAlgoIDInfo(const int packedId,int& algoId,int& prevAlgoId,int& quality); //mainly for debuging
  static int packTrackData(bool posCharge,int vertexNr,int ndof);
  static int packHitData(int nrHits,int nrLayers,int nrLostHits,int nrPixelHits);

  ClassDef(SHIsolTrack,6)
};

#endif
