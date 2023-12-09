#ifndef SHarper_TrigNtup_EGScoutingRegTreeStruct_hh
#define SHarper_TrigNtup_EGScoutingRegTreeStruct_hh

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/isFinite.h"

#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "SHarper/TrigNtup/interface/EGRegTreeStruct.hh"

#include "TTree.h"

class Run3ScoutingElectron;
namespace reco{
  class GenParticle;
}

namespace edm{
  class Event;
}


struct ScoutEleStruct {
  static constexpr int kMaxTracks = 4;
  float pt;
  float energy;
  float eta;
  float phi;
  float m;
  float trkd0[kMaxTracks];
  float trkdz[kMaxTracks];
  float trkpt[kMaxTracks];
  float trketa[kMaxTracks];
  float trkphi[kMaxTracks];
  float trkchi2overndf[kMaxTracks];
  float trkcharge[kMaxTracks];
  float dEtaIn;
  float dPhiIn;
  float sigmaIetaIeta;
  float hOverE;
  float ooEMOop;
  float missingHits;
  float ecalIso;
  float hcalIso;
  float trackIso;
  float r9;
  float sMin;
  float sMaj;
  unsigned int seedId; 
  unsigned int rechitZeroSuppression;
  int nrTrks;
  int isEB;
  int iEtaOrIX;
  int iPhiOrIY;
  int bestTrkIndx;
  void fill(const Run3ScoutingElectron& ele);
  static std::string contents(){return
      "pt/F:energy:eta:phi:m:trkd0[4]:trkdz[4]:trkpt[4]:trketa[4]:trkphi[4]:trkchi2overndf[4]:trkcharge[4]:dEtaIn:dPhiIn:sigmaIetaIeta:hOverE:ooEMOop:missingHits:ecalIso:hcalIso:trackIso:r9:sMin:sMaj:seedId/i:rechitZeroSuppression:nrTrks/I:isEB:iEtaOrIX:iPhiOrIY:bestTrkIndx";
  }
  void clear();

      
};

struct ScoutShowerShapeStruct {
  float eMax;
  float e2nd;
  float eLeft;
  float eRight;
  float eTop;
  float eBottom;
  float e1x5;
  float e5x5;
  float e2x5Max;
  float e2x5Left;
  float e2x5Right;
  float e2x5Top;
  float e2x5Bottom;
  void fill(const int seedId,const std::vector<unsigned int>& detIds,const std::vector<float>& nrgys);
  void clear(){
    eMax=e2nd=eLeft=eRight=eTop=eBottom=e1x5=e5x5=e2x5Max=e2x5Left=e2x5Right=e2x5Top=e2x5Bottom=0.f;
  }
  static std::string contents(){return "eMax/F:e2nd:eLeft:eRight:eTop:eBottom:e1x5:e5x5:e2x5Max:e2x5Left:e2x5Right:e2x5Top:e2x5Bottom";
  }
};

struct EGScoutingRegTreeStruct {
  int nrVert;
  float rho;
  float nrPUInt;
  float nrPUIntTrue;
  EvtStruct evt;
  ScoutEleStruct ele;
  ScoutShowerShapeStruct eleSS;
  GenInfoStruct mc;
  
  void createBranches(TTree* tree);
  void setBranchAddresses(TTree* tree);
  void fill(const edm::Event& event,int iNrVert,float iRho,float nrPUInt,float nrTruePUInt,const reco::GenParticle* iMC,const Run3ScoutingElectron* ele);
  void clear(){
    nrVert=0;
    rho=0.;
    nrPUInt=0.;
    nrPUIntTrue=0.;
    evt.clear();
    ele.clear();
    eleSS.clear();
    mc.clear();
  }
};



#endif
