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
//#include "SHEvent/SHIsolSuperCluster.hh"
//#include "SHEvent/SHIsolCluster.hh"
#include "SHarper/SHNtupliser/interface/SHEleCMSSWStructs.hh"
#include "SHarper/SHNtupliser/interface/CaloTools.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"

namespace heep{
  class Ele;
}

namespace reco{
  class GsfElectron;
  class SuperCluster;
  class Photon;

}




class SHEvent;

class SHElectron : public TObject {
  
 private:
  //classification variables
  int type_;
  
  //kinematic quantities
  TLorentzVector p4_; //p4 stores energy
  float et_; //only exist so I can see what the et is in a tree->Show(0)
  float nrgy_; 
  float nrgyErr_;  
  float rawNrgy_;
  float preShowerNrgy_;
  float epCombNrgy_; //the e-p combined energy 
  float phoNrgy_; //not set by the electron
  float altNrgy_; //not set by the electron
  float altNrgyErr_; //not set by the electron
  float altEPCombNrgy_; 
  float altPhoNrgy_; //not set by the electron
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
  float d0_;
  float trkMomErr_;

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
  float sigmaIPhiIPhi_;
  
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
  int cutCodeVID_;
  float e1x5Over5x5_;
  float e2x5Over5x5_;

  //new 310 varibles (next 4 are really for validation purposes)
  bool isEcalDriven_;
  bool isTrackerDriven_;
  float isolEmDR04_;
  float isolHadDepth1DR04_;
  float isolHadDepth2DR04_;
  float isolPtTrksDR04_;
  int seedId_;
  bool isBarrel_;
  bool isEBEEGap_;   // true if particle is in the crack between EB and EE
  bool isEBEtaGap_;  // true if particle is in EB, and inside the eta gaps between modules
  bool isEBPhiGap_;  // true if particle is in EB, and inside the phi gaps between modules
  bool isEEDeeGap_;  // true if particle is in EE, and inside the gaps between dees
  bool isEERingGap_; // true if particle is in EE, and inside the gaps between rings
  bool posChargeTrk_;

  TVector3 posTrackInnToSeed_;
  TVector3 posTrackOutToSeed_;

  //conversion rejection
  int nrMissingHits_;
  float dCotTheta_; //lets be naughty and put conver result here
  float dist_;
  float radius_;

  //pflow isolation
  float isolChargedHadron_;
  float isolNeutralHadron_;
  float isolPhoton_;

  //new H/E defination
  float hademDepth1BC_;
  float hademDepth2BC_;
  float isolHadDepth1BC_;
  float isolHadDepth2BC_;

  //dxy error info
  float dxyErr_;
  float dzErr_;

  //mva info
  float isolMVA_;
  float nonIsolMVA_;
  
  bool passCutPreSel_;
  bool passMVAPreSel_;
  bool passPFlowPreSel_;

  //pm variables
  float pmDPhi1_;
  float pmDPhi2_;
  float pmDRz1_;
  float pmDRz2_;
  int pmSubDets_;

  int nrSatCrysIn5x5_; //new for v25
  std::vector<int> ids_; //associated IDs
  //userfloats make a long resisted appearence
  std::vector<std::pair<std::string,float> > userFloats_;
  std::vector<std::pair<std::string,int> > userInts_;
  std::vector<std::pair<std::string,float> > userIDs_;
  using UserFloatSorter = TempFuncs::PairComp<std::string,float,std::less<std::string> >;
  using UserIntSorter = TempFuncs::PairComp<std::string,int,std::less<std::string> >;
  using UserIDSorter = TempFuncs::PairComp<std::string,float,std::less<std::string> >;
  

  float rhoCorr_; //! set by the event each time we get it

  //backwards link to the mother event
  //needs to be set everytime the event is read
  const SHEvent* mEvent_;//! transient, not stored in root

private:
  SHElectron& operator=(const SHElectron& ){return *this;} 

 public:
  SHElectron();
  SHElectron(const SHElectron& rhs);
  SHElectron(const heep::Ele& ele,int superClusNr=-1);
 //fills off a GsfElectron, doesnt fill nr trks isol or cutcode
  SHElectron(const reco::GsfElectron& ele,int superClusNr=-1);
  SHElectron(const reco::Photon& pho,int superClusNr=-1);
  SHElectron(const TLorentzVector&p4,const reco::SuperCluster& superClus,const cmssw::FiducialFlags& fid,
  	     const cmssw::ShowerShape& shape,const cmssw::IsolationVariables& isol03,
  	     const cmssw::IsolationVariables& isol04,int superClusNr);
  ~SHElectron(){}

  // void setPosTrackInnToSeed(const TVector3& pos){posTrackInnToSeed_=pos;}
  // void setPosTrackOutToSeed(const TVector3& pos){posTrackOutToSeed_=pos;}
  void setD0(float d0){d0_=d0;}
  void setNrSatCrysIn5x5(int val){nrSatCrysIn5x5_=val;}
  void setCutCode(int val){cutCode_=val;}
  void setCutCodeVID(int val){cutCodeVID_=val;}
  // void setIsolMVA(float mva){isolMVA_=mva;}
  // void setCaloIsol(double isolEm,double isolHad,double isolHadDepth1,double isolHadDepth2);
  // void setPFIsol(float charged,float neutral,float photon);
  // void fixTrkIsol();
  // void setIsConversion(float isCon){dCotTheta_=isCon;}
  // void setConvInfo(float iDist,float iDcot){dCotTheta_=iDcot;dist_=iDist;}
  // void setShowerShape(float sigmaEtaEta,float sigmaIEtaIEta,float e1x5,float e2x5Max,float e5x5);
  // void setPassPFlowPreSel(bool pass){passPFlowPreSel_=pass;}
  // void setPassMVAPreSel(bool pass){passMVAPreSel_=pass;}
  void setTrkIsol(float isolPt03,float isolPt04,float isolNrTrks){isolPtTrks_=isolPt03;isolPtTrksDR04_=isolPt04;isolNrTrks_=isolNrTrks;}
  void setNrgyExtra(float iAltNrgy,float iAltNrgyErr,float iAltEPCombNrgy,float iPhoNrgy,float iAltPhoNrgy){
    altNrgy_=iAltNrgy;altNrgyErr_=iAltNrgyErr;altEPCombNrgy_=iAltEPCombNrgy;phoNrgy_=iPhoNrgy;altPhoNrgy_=iAltPhoNrgy;
  }
  void setIDs(std::vector<int> ids){ids_=std::move(ids);}
  void setUserInts(std::vector<std::pair<std::string,int> > vars){
    userInts_=std::move(vars);
    std::sort(userInts_.begin(),userInts_.end(),UserIntSorter());
  }
  void setUserFloats(std::vector<std::pair<std::string,float> > vars){
    userFloats_=std::move(vars);
    std::sort(userFloats_.begin(),userFloats_.end(),UserFloatSorter());
  }
  void setUserIDs(std::vector<std::pair<std::string,float> > vars){
    userIDs_=std::move(vars);
    std::sort(userIDs_.begin(),userIDs_.end(),UserIDSorter());
  }
  
  //get the seed + super clusters
  //tried to avoid pointers but it looks envitable as sometimes the ele wont
  //have any seed/super clusters and want those calls to degrade gracefully
  const SHSuperCluster* superClus()const;
  const SHBasicCluster* seedClus()const;
  int seedIdFromClus()const;

  //classification variables
  int type()const{return type_;}
  int region()const{return CaloTools::region(detEta());}
  bool isBarrel()const{return isEB();}//fabs(detEta_)<1.5;}
  //bool isEndcap()const{return !isBarrel();}

  bool isFid()const{return region()<=2;}

  bool isEB()const{return isBarrel_;}
  bool isEBEEGap()const{return isEBEEGap_;}   
  bool isEBEtaGap()const{return isEBEtaGap_;}   
  bool isEBPhiGap()const{return isEBPhiGap_;}   
  bool isEEDeeGap() const{return isEEDeeGap_;}    
  bool isEERingGap()const{return isEERingGap_;}    

  bool hasTrack()const{return dEtaIn_<900;}
  //kinematic quantities
  float nrgy()const{return nrgy_;}
  float nrgyErr()const{return nrgyErr_;}
  float rawNrgy()const{return rawNrgy_;}
  float epCombNrgy()const{return epCombNrgy_;}
  float phoNrgy()const{return phoNrgy_;}
  float altNrgy()const{return altNrgy_;}
  float altNrgyErr()const{return altNrgyErr_;}
  float altEPCombNrgy()const{return altEPCombNrgy_;}
  float altPhoNrgy()const{return altPhoNrgy_;}
  
  float et()const;
  float clusEt()const{return posCal().Pt()/posCal().Mag()*clusNrgy();}
  float clusNrgy()const{return superClus()!=NULL ? superClus()->nrgy() : -999.;}
  
  float rawEt()const{return posCal().Pt()/posCal().Mag()*rawNrgy();}

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
  float dxy(const TVector3& point) const;
  float dxy()const;
  float dxy00()const{TVector3 zeroPoint;return dxy(zeroPoint);}
  float dzTrkVtx()const;
  const TVector3& posTrackInnToSeed()const{return posTrackInnToSeed_;}
  const TVector3& posTrackOutToSeed()const{return posTrackOutToSeed_;}
  float trkMomErr()const{return trkMomErr_;}
  //id quantities
  float epIn()const{return epIn_;}
  float epOut()const{return epOut_;}
  float hadem()const{return hadem_;}
  float hademDepth1()const{return hademDepth1_;}
  float hademDepth2()const{return hademDepth2_;}
  float dEtaIn()const{return dEtaIn_;}
  float dEtaInSeed()const{return mEvent_ && dEtaIn_!=999 ? dEtaIn_-detEta_+superClus()->seedClus()->eta(): 999;}
  float dEtaInSC()const{return dEtaIn_;}
  float dEtaOut()const{return dEtaOut_;}
  float dPhiIn()const{return dPhiIn_;}
  float dPhiOut()const{return dPhiOut_;}

  float sigmaEtaEta()const{return sigmaEtaEta_;}
  float sigmaIEtaIEta()const{return sigmaIEtaIEta_;}


  float sigmaIPhiIPhi()const{return sigmaIPhiIPhi_;}

  float invEinvP()const{return 1/nrgy() - 1/trkPt();}
  float bremFrac()const{return (p3TrackVtx().Mag() - p3TrackOut().Mag())/p3TrackVtx().Mag();}
  float e1x5Over5x5()const{return e1x5Over5x5_;}
  float e2x5Over5x5()const{return e2x5Over5x5_;}

  int nrSatCrysIn5x5()const{return nrSatCrysIn5x5_;}

  float trkChi2()const{return trkChi2_;}
  int nrDof()const{return nrDof_;}
  int charge()const{return posCharge_ ? 1 : -1;}

  //should gradually fad out one set of accessors
  float isolEm()const{return isolEm_;}
  float isolHad()const{return isolHad_;}
  float isolHadDepth1()const{return isolHadDepth1_;}
  float isolHadDepth2()const{return isolHadDepth2_;}
  float isolPtTrks()const{return isolNrTrks_!=-1 ? isolNrTrks_ : isolPtTrks_;} //really nasty hack to do with how I was storing CMSSW computed track isolation, should go away
  float isolPtTrksCMSSW()const{return isolPtTrks_;}
  float isolNrTrks()const{return isolNrTrks_;}
 
  float isolEmDR04()const{return isolEmDR04_;}
  float isolHadDR04()const{return isolHadDepth1DR04()+isolHadDepth2DR04();}
  float isolHadDepth1DR04()const{return isolHadDepth1DR04_;}
  float isolHadDepth2DR04()const{return isolHadDepth2DR04_;}
  float isolPtTrksDR04()const{return isolPtTrksDR04_;}
  bool isEcalDriven()const{return isEcalDriven_;}
  bool isTrackerDriven()const{return isTrackerDriven_;}
  bool passCutPreSel()const{return passCutPreSel_;}
  bool passMVAPreSel()const{return passMVAPreSel_;}
  bool passPFlowPreSel()const{return passPFlowPreSel_;}

  //  int seedId()const{return seedId_;}

  int seedId()const{return seedIdFromClus();}


  std::pair<int,float> isolTrk(double minDeltaR,double maxDeltaR,double lipCut,double ptCut)const;

  int cutCode()const{return cutCode_;}
  int cutCodeVID()const{return cutCodeVID_;}
 
  int calPhiRoad()const;

  int nrMissingHits()const{return nrMissingHits_;}
  float dCotTheta()const{return dCotTheta_;}
  float dist()const{return dist_;}
  float radius()const{return radius_;}
 float isConversion()const{return dCotTheta_;}

  float isolChargedHadron()const{return isolChargedHadron_;}
  float isolNeutralHadron()const{return isolNeutralHadron_;}
  float isolPhoton()const{return isolPhoton_;}

  float isolMVA()const{return isolMVA_;}
  float nonIsolMVA()const{return nonIsolMVA_;}

  float rhoCorr()const{return rhoCorr_;}
  
  // const SHIsolSuperCluster& getIsolSuperClus()const;
  // float isolEmClus(double coneRadius)const;
  // float isolEmEtClus(double coneRadius)const;
  const std::vector<int>& ids()const{return ids_;}
  const std::vector<std::pair<std::string,float> >& userFloats()const{return userFloats_;}
  const std::vector<std::pair<std::string,int> >& userInts()const{return userInts_;}
  const std::vector<std::pair<std::string,float> >& userIDs()const{return userIDs_;}
  
  float userFloat(const std::string& name)const{
    return getUserVar<float,UserFloatSorter>(name,userFloats_);
  }
  float userID(const std::string& name)const{
    return getUserVar<float,UserIDSorter>(name,userIDs_);
  }
  float userInt(const std::string& name)const{
    return getUserVar<int,UserIntSorter>(name,userInts_);
  }
   
  //links
  int superClusIndex()const{return superClusIndx_;}

  //modifiers
  void setRhoCorr(float rho){rhoCorr_=rho;}
  void setSuperClusIndex(int index){superClusIndx_=index;}
  void setMotherEvent(const SHEvent* event){mEvent_=event;}
  const SHEvent* motherEvent()const{return mEvent_;}

  void setNewNrgy(float nrgy);

private:
  template<typename T,typename Sorter> 
  static T getUserVar(const std::string& varName,const std::vector<std::pair<std::string,T> >& vars){
    auto res = std::equal_range(vars.begin(),vars.end(),varName,Sorter());
    const size_t nrFound = std::distance(res.first,res.second);
    if(nrFound==1) return res.first->second;
    else if(nrFound>1){
      LogErr << " Error,  "<<nrFound<<" keys match "<<varName<<std::endl;
    }
    return std::numeric_limits<T>::max();
  }
  
  ClassDef(SHElectron,30) 

};


#endif
