#ifndef SHELECTRON
#define SHELECTRON

/*********************************************************************************************
/                                                                                            /
/ SHElectron : a class which stores electron data and also has useful methods to manipulate  /
/              that data                                                                     /
/              it is intended to be a root storable object, be accessible within root and be /
/              able to be stored in a TClonesArray in a TTree                                /
/                                                                                            /
/ Author : Sam Harper (Rutherford Appleton Laboratory) Summer 2007                           /
/                                                                                            /
********************************************************************************************/

//notes for next update, idea to store eta and phi seperately as well as p4 to save computational time


#include "SHarper/SHNtupliser/interface/SHSuperCluster.hh"
#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
#include "SHarper/SHNtupliser/interface/SHIsolSuperCluster.hh"
#include "SHarper/SHNtupliser/interface/SHIsolCluster.hh"


#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"

namespace heep{
  class Ele;
}

class SHEvent;

class SHElectron : public TObject {
  
 private:
  //classification variables
  int type_;
  
  //kinematic quantities
  TLorentzVector p4_; //p4 stores energy
  float et_; //only exist so I can see what the et is in a tree->Show(0)
  float nrgy_; //only exist so I can see what the nrgy is in a tree->Show(0)
  float rawNrgy_;
  float preShowerNrgy_;
  float nrgyErr_;  
  TVector3 posCal_;
  float e5x5_; 
  float eta_;
  float detEta_;

  //track quantities (momentum + positions)
  TVector3 p3TrackVtx_;
  TVector3 p3TrackCal_;
  TVector3 p3TrackInn_;
  TVector3 p3TrackOut_;
  TVector3 posTrackVtx_;
  TVector3 posTrackCal_;
  TVector3 posTrackInn_;
  TVector3 posTrackOut_;
  float trkChi2_; 
  int nrDof_;
  bool posCharge_;

  //id quantities
  float epIn_;
  float epOut_;
  float hadem_; 
  float hademDepth1_; //currently not being filled 
  float hademDepth2_; //currently not being filled
  float dEtaIn_;
  float dEtaOut_; 
  float dPhiIn_;  
  float dPhiOut_; 
  float sigmaEtaEta_;
  float sigmaIEtaIEta_;

  //links to tracks, superclusters
  int superClusIndx_; //this is not copied by copy constructor

  //new isolation variables
  float isolEm_;
  float isolHad_; 
  float isolHadDepth1_;
  float isolHadDepth2_;
  float isolPtTrks_; 
  float isolNrTrks_; 

  int cutCode_;
  float e1x5Over5x5_;
  float e2x5Over5x5_;
 
  //backwards link to the mother event
  //needs to be set everytime the event is read
  const SHEvent* mEvent_;//! transient, not stored in root

private:
  SHElectron& operator=(const SHElectron& rhs){return *this;}

 public:
  SHElectron();
  SHElectron(const SHElectron& rhs);
  SHElectron(const heep::Ele& ele,int superClusNr=-1);
 
  ~SHElectron(){}

  //modifiers (as these arent members of PixelMatchGsfElectrons so are hacked in for now)
  //these will disappear in the future
  // void setIsolPtTrks(double isol){isolPtTrks_=isol;}
  //void setIsolNrTrks(int nrTracks){isolNrTrks_=nrTracks;}
  //void setIsolEm(double isol){isolEm_=isol;}
  //void setIsolHad(double isol){isolHad_=isol;}
  //void setPassStdSel(bool pass){passStdSel_=pass;}

  void setCaloIsol(double isolEm,double isolHad,double isolHadDepth1,double isolHadDepth2);
  void fixTrkIsol();
  //accessors
  
  //get the seed + super clusters
  //tried to avoid pointers but it looks envitable as sometimes the ele wont
  //have any seed/super clusters and want those calls to degrade gracefully
  const SHSuperCluster* superClus()const;
  const SHBasicCluster* seedClus()const;

  //classification variables
  int type()const{return type_;}

  //kinematic quantities
  float nrgy()const{return p4_.E();}
  float et()const;
  float clusEt()const{return posCal().Pt()/posCal().Mag()*clusNrgy();}
  float clusNrgy()const{return superClus()!=NULL ? superClus()->nrgy() : -999.;}
  float rawNrgy()const{return rawNrgy_;}
  float rawEt()const{return posCal().Pt()/posCal().Mag()*rawNrgy();}
  float nrgyErr()const{return nrgyErr_;}
  float preShowerNrgy()const{return preShowerNrgy_;}
  const TLorentzVector& p4()const{return p4_;}
  const TVector3& posCal()const{return posCal_;}
  float e5x5()const{return e5x5_;}
  float eta()const{return eta_;}
  float phi()const{return p4().Phi();}
  float detEta()const{return detEta_;} 
  float detPhi()const{return posCal().Phi();}
  
  //track quantities
  float trkPt()const{return p3TrackVtx().Pt();}
  const TVector3& p3TrackVtx()const{return p3TrackVtx_;}
  const TVector3& p3TrackCal()const{return p3TrackCal_;}
  const TVector3& p3TrackInn()const{return p3TrackInn_;}
  const TVector3& p3TrackOut()const{return p3TrackOut_;}
  const TVector3& posTrackVtx()const{return posTrackVtx_;}
  const TVector3& posTrackCal()const{return posTrackCal_;}
  const TVector3& posTrackInn()const{return posTrackInn_;}
  const TVector3& posTrackOut()const{return posTrackOut_;}
  float dzTrkVtx()const;


  //id quantities
  float epIn()const{return epIn_;}
  float epOut()const{return epOut_;}
  float hadem()const{return hadem_;}
  float hademDepth1()const{return hademDepth1_;}
  float hademDepth2()const{return hademDepth2_;}
  float dEtaIn()const{return dEtaIn_;}
  float dEtaOut()const{return dEtaOut_;}
  float dPhiIn()const{return dPhiIn_;}
  float dPhiOut()const{return dPhiOut_;}

  float sigmaEtaEta()const{return sigmaEtaEta_;}
  
  float sigmaIEtaIEta()const{return sigmaIEtaIEta_;}

  float invEinvP()const{return 1/nrgy() - 1/trkPt();}
  float bremFrac()const{return (p3TrackVtx().Mag() - p3TrackOut().Mag())/p3TrackVtx().Mag();}
  float e1x5Over5x5()const{return e1x5Over5x5_;}
  float e2x5Over5x5()const{return e2x5Over5x5_;}


  int nrDof()const{return nrDof_;}
  int charge()const{return posCharge_ ? 1 : -1;}

  //should gradually fad out one set of accessors
  float isolEm()const{return isolEm_;}
  float isolHad()const{return isolHad_;}
  float isolHadDepth1()const{return isolHadDepth1_;}
  float isolHadDepth2()const{return isolHadDepth2_;}
  float isolPtTrks()const{return isolPtTrks_;}
  float isolNrTrks()const{return isolNrTrks_;}
 
  std::pair<int,float> isolTrk(double minDeltaR,double maxDeltaR,double lipCut,double ptCut)const;
 
  int calPhiRoad()const;

  const SHIsolSuperCluster& getIsolSuperClus()const;
  float isolEmClus(double coneRadius)const;
  float isolEmEtClus(double coneRadius)const;

  void getHitsByDetId(std::vector<int>& hitDetIds)const;

  //links
  int superClusIndex()const{return superClusIndx_;}

  //modifiers
  void setSuperClusIndex(int index){superClusIndx_=index;}
  void setMotherEvent(const SHEvent* event){mEvent_=event;}
  const SHEvent* motherEvent()const{return mEvent_;}


  ClassDef(SHElectron,11) 

};


#endif
