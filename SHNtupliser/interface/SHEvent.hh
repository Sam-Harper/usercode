#ifndef SHEVENT
#define SHEVENT

//Event class which stores all the objects in the event
//currently limited to superclusters + electrons

#include "SHarper/SHNtupliser/interface/SHSuperCluster.hh"
#include "SHarper/SHNtupliser/interface/SHElectron.hh"
#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"
#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"
#include "SHarper/SHNtupliser/interface/SHJet.hh"
//#include "SHEvent/SHIsolCluster.hh"
//#include "SHEvent/SHIsolSuperCluster.hh"
#include "SHarper/SHNtupliser/interface/SHIsolTrack.hh"
#include "SHarper/SHNtupliser/interface/SHTrigInfo.hh"
#include "SHarper/SHNtupliser/interface/SHMet.hh"
#include "SHarper/SHNtupliser/interface/SHL1Cand.hh"
#include "SHarper/SHNtupliser/interface/SHMuon.hh"
#include "SHarper/SHNtupliser/interface/SHCaloTowerContainer.hh"
//#include "SHEvent/SHPileUpSummary.hh"
#include "SHarper/SHNtupliser/interface/SHPFCandContainer.hh"
#include "SHarper/SHNtupliser/interface/SHPFClusterContainer.hh"
#include "SHarper/SHNtupliser/interface/SHEleCMSSWStructs.hh"
#include "SHarper/SHNtupliser/interface/SHVertex.hh"
#include "SHarper/SHNtupliser/interface/SHGenInfo.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh"
#include "SHarper/SHNtupliser/interface/SHPileUpSummary.hh"
#include "SHarper/SHNtupliser/interface/SHRecHitsToClustersMap.hh"
#include "SHarper/SHNtupliser/interface/SHGainSwitchInfo.hh"
#include "SHarper/SHNtupliser/interface/SHJetMETExtra.hh"
#include "TObject.h"
#include "TClonesArray.h"
#include "TBits.h"

#include <map>
#include <vector>
#include <bitset>


namespace reco{
  class SuperCluster;
  class Photon;
  class CaloCluster;
  class GsfElectron;
  class Muon;
  class Vertex;
  class Track;
}


namespace pat{
  class Jet;
}

namespace heep{
  class Ele;
}

class SHEvent : public TObject {

 private:
  TClonesArray superClusArray_;
  TClonesArray electronArray_;
  TClonesArray mcPartArray_;
  TClonesArray jetArray_; 
  SHCaloHitContainer caloHits_; //! now transisent, stored in seperate branch for 5X
  TClonesArray isolTrkArray_; //! now transisent, stored in seperate branch for 5X
  TClonesArray trigArray_;
  TClonesArray muArray_;
  TClonesArray preShowerClusArray_; //! new in version 23
  
 
  int runnr_;

  int eventnr_;
  bool isMC_;
  int datasetCode_;
  double weight_;
  SHMet metData_; //new for v9
  double genEventPtHat_; 

  TBits l1Bits_; //not being filled but a use can be seen
  // TClonesArray l1CandArray_; //now lives in trigger container, redundant for 5X
  
  int lumiSec_;
  int bx_;
  int orbNr_;
  unsigned long long time_;

  int nrVertices_;
  TVector3 vertex_;
  TVector3 beamSpot_;

  SHMet pfMet_;

  int preScaleCol_; //new for V17
 
  float eleRhoCorr_; //new for V20 (2011 rho corr defination for electrons)
   
  //SHPileUpSummary puSummary_; //new for V18

  TClonesArray vertexArray_; //new for V22

  float rhoCorr_; //new for V22 
  int flags_; //new for V22, usefull for flagging things like events with bad laser corrections

  std::string configMD5SumStr_; //V27
  
  SHCaloTowerContainer caloTowers_; //! so this is a new experimental design, I store this on another branch and give it this memory location
  
  SHPFCandContainer pfCands_; //! like calo towers, this is stored on a seperate branch

  SHPFClusterContainer pfClusters_; //! like pf cands stored on a seperate branch

  SHGenInfo genInfo_;//! stored on a seperate branch
  SHTrigSummary trigSum_;//! stored on a seperate branch
  SHPileUpSummary puSum_;//! stored on a seperate branch 
  SHRecHitsToClustersMap recHitClusMap_;//! stored on a seperate branch 
  SHGainSwitchInfo gainSwitchInfo_;//! stored on a seperate branch 
  SHJetMETExtra jetMETExtra_;//! stored on a seperate branch


  SHEvent(const SHEvent &rhs):TObject(rhs){}//disabling copying for now
  SHEvent& operator=(const SHEvent&){return *this;}//disabling assignment

 public:
  SHEvent();
  virtual ~SHEvent();//need to check if need to delete TClonesArray objects


  void addElectron(const heep::Ele& ele);
  void addElectron(const reco::GsfElectron& ele);
  void addElectron(const reco::Photon& ele);
  void addElectron(const SHElectron& ele,const SHSuperCluster& superClus);
  void addElectron(const SHElectron& ele);
  void addElectron(const TLorentzVector&p4,const reco::SuperCluster& superClus, //for trackless electrons
		   const cmssw::FiducialFlags& fid,
		   const cmssw::ShowerShape& shape,
		   const cmssw::IsolationVariables& isol03,
		   const cmssw::IsolationVariables& isol04);
  void addJet(const pat::Jet& jet);
  void addJet(const SHJet& jet);
  void addSuperCluster(const reco::SuperCluster& superClus);
  /// void addIsolCluster(const reco::CaloCluster& clus);
  // void addIsolSuperCluster(const reco::SuperCluster& superClus);
  void addIsolTrk(const SHIsolTrack& trk);
  void addIsolTrk(const reco::Track& trk,int vertexNr);
  //  void addIsolTrk(const TVector3& p3,const TVector3& vtxPos,bool posCharge,int vertexNr,float chi2,int ndof,int algosAndQual);
 
  void addMCParticle(int partIndx,int partStdhep,int partIdhep,
		     int partJmo1,int partJmo2,int partNrMo,
		     int partJda1,int partJda2,int partNrDa,
		     const TLorentzVector& p4,const TVector3& pos);
  void addEcalHits(const std::vector<SHCaloHit> & hitVec);
  void addHcalHits(const std::vector<SHCaloHit> & hitVec);  
  void addCaloTower(const SHCaloTower& caloTower){caloTowers_.addTower(caloTower);}
  void addMCParticle(const SHMCParticle& mcPart);
  void addTrigInfo(const SHTrigInfo& trigInfo);
  // void addL1Cand(const SHL1Cand& cand);
  //void addL1Cand(const TLorentzVector& p4,int type);
  void addMuon(const reco::Muon& mu);
  void addMuon(const SHMuon& mu);
  void addVertex(const reco::Vertex& vtx);
  //usefull for copying an event
  void addCaloHits(const SHEvent& rhs);
  void addCaloHits(const SHCaloHitContainer& hits){caloHits_ = hits;}
  void addCaloTowers(const SHEvent& rhs);
  void addCaloTowers(const SHCaloTowerContainer& towers){caloTowers_ = towers;}
  void addIsolInfo(const SHEvent& rhs);
  
  void addPreShowerCluster(const SHPreShowerCluster& clus);

 
  int nrPUInteractions()const{return getPUSum().nrInteractionsByBx(0);}
  int nrPUInteractionsNeg()const{return getPUSum().nrInteractionsByBx(-1);}
  int nrPUInteractionsPos()const{return getPUSum().nrInteractionsByBx(1);}
  int nrTruePUInteractions()const{return getPUSum().nrTrueInteractions();}
  void setRunnr(int iRunnr){runnr_=iRunnr;}
  void setEventnr(int iEventnr){eventnr_=iEventnr;}
  void setIsMC(bool iIsMC){isMC_=iIsMC;}
  void setDatasetCode(int iDatasetCode){datasetCode_=iDatasetCode;}
  void setWeight(double iWeight){weight_=iWeight;}
  void setMet(const SHMet& iMet){metData_=iMet;}
  void setPFMet(const SHMet& iMet){pfMet_=iMet;}
  void setGenEventPtHat(double iPtHat){genEventPtHat_=iPtHat;}
  void setL1Bits(const TBits& bits){l1Bits_=bits;}
  void setBX(int iBx){bx_=iBx;}
  void setOrbitNumber(int iOrb){orbNr_=iOrb;}
  void setTime(unsigned long long iTime){time_ = iTime;}
  void setLumiSec(int iLumiSec){lumiSec_=iLumiSec;}
  void setVertex(const TVector3& vert){vertex_=vert;}
  void setNrVertices(int iNrVertices){nrVertices_=iNrVertices;}
  void setBeamSpot(const TVector3& iBS){beamSpot_=iBS;}
  void setPreScaleCol(int iPreScaleCol){preScaleCol_=iPreScaleCol;}
  void setEleRhoCorr(float iRho){eleRhoCorr_=iRho;}
  void setRhoCorr(float iRho){rhoCorr_=iRho;}
  void setFlags(int iFlags){flags_=iFlags;}
  void setConfigMD5SumStr(std::string val){configMD5SumStr_=std::move(val);}
  virtual void copyEventPara(const SHEvent& rhs);
  virtual void clear();
  void clearTrigs(){trigArray_.Delete();}


  const SHSuperCluster* getSuperClus(int clusNr)const;
  const SHPreShowerCluster* getPreShowerClus(int clusNr)const;
  const SHMCParticle* getMCParticle(int partNr)const;
  const SHElectron* getElectron(int eleNr)const;
  ///const SHIsolCluster* getIsolClus(int clusNr)const;
  //const SHIsolSuperCluster* getIsolSuperClus(int clusNr)const;
  const SHJet* getJet(int jetNr)const;
  SHJet* getJet(int jetNr);
  const SHMuon* getMuon(int muNr)const;
  const SHVertex* getVertex(int vertNr)const;

  const SHIsolTrack* getIsolTrk(int trkNr)const;
  SHElectron* getElectron(int eleNr); //allows the event to modify the electron
  const SHTrigInfo* getTrigInfo(int trigNr)const; 

  //const SHL1Cand* getL1Cand(int candNr)const;

  int getSuperClusIndx(float rawNrgy,float eta,float phi)const; //exactly matches based on energy, calorimeter eta, phi returns -1 if not found
  //  int getIsolClusIndx(float rawNrgy,float eta,float phi)const; //exactly matches based on energy, calorimeter eta, phi returns -1 if not found

  int nrSuperClus()const{return superClusArray_.GetLast()+1;} 
  int nrPreShowerClus()const{return preShowerClusArray_.GetLast()+1;}
  int nrElectrons()const{return electronArray_.GetLast()+1;}
  int nrMCParticles()const{return mcPartArray_.GetLast()+1;}
  int nrJets()const{return jetArray_.GetLast()+1;}
  // int nrIsolClus()const{return isolClusArray_.GetLast()+1;}
  // int nrIsolSuperClus()const{return isolSuperClusArray_.GetLast()+1;}
  int nrIsolTrks()const{return isolTrkArray_.GetLast()+1;}
  int nrTrigs()const{return trigArray_.GetLast()+1;}
  //  int nrL1Cands()const{return l1CandArray_.GetLast()+1;}
  int nrMuons()const{return muArray_.GetLast()+1;}
  int nrVertices()const{return vertexArray_.GetLast()+1;}
  int nrGoodVertices()const;

  int runnr()const{return runnr_;}
  int eventnr()const{return eventnr_;}
  int datasetCode()const{return datasetCode_;}
  float weight()const{return weight_;}
  const SHMet& metData()const{return metData_;}
  SHMet& metData(){return metData_;} 
  const SHMet& met()const{return metData_;}
  SHMet& met(){return metData_;} 
  const SHMet& pfMet()const{return pfMet_;}
  SHMet& pfMet(){return pfMet_;}
  bool isMC()const{return isMC_;}
  const SHCaloHitContainer& getCaloHits()const{return caloHits_;}
  SHCaloHitContainer& getCaloHits(){return caloHits_;}
  const SHCaloTowerContainer& getCaloTowers()const{return caloTowers_;}
  SHCaloTowerContainer& getCaloTowers(){return caloTowers_;}
  const SHPFCandContainer& getPFCands()const{return pfCands_;}
  SHPFCandContainer& getPFCands(){return pfCands_;} 
  const SHPFClusterContainer& getPFClusters()const{return pfClusters_;}
  SHPFClusterContainer& getPFClusters(){return pfClusters_;}
  const SHGenInfo& getGenInfo()const{return genInfo_;}
  SHGenInfo& getGenInfo(){return genInfo_;}
  const SHTrigSummary& getTrigSum()const{return trigSum_;}
  SHTrigSummary& getTrigSum(){return trigSum_;}
  const SHPileUpSummary& getPUSum()const{return puSum_;}
  SHPileUpSummary& getPUSum(){return puSum_;}
  const SHJetMETExtra& getJetMETExtra()const{return jetMETExtra_;}
  SHJetMETExtra& getJetMETExtra(){return jetMETExtra_;}

  TClonesArray& getIsolTrks(){return isolTrkArray_;} //needed for SHEventReader to know where this is memory wise
  SHGainSwitchInfo& getGSInfo(){return gainSwitchInfo_;} 
  const SHGainSwitchInfo& getGSInfo()const{return gainSwitchInfo_;} 

  TClonesArray& getPreShowerClusters(){return preShowerClusArray_;}
  const SHRecHitsToClustersMap& getRecHitClusMap()const{return recHitClusMap_;}
  SHRecHitsToClustersMap& getRecHitClusMap(){return recHitClusMap_;}
  double genEventPtHat()const{return genEventPtHat_;}
  const TBits& l1Bits()const{return l1Bits_;}
  int bx()const{return bx_;}
  int lumiSec()const{return lumiSec_;}
  int orbNr()const{return orbNr_;}
  unsigned long long time()const{return time_;}
  const TVector3& vertex()const{return vertex_;}
  //int nrVertices()const{return nrVertices_;}
  int preScaleCol()const{return preScaleCol_;}
  float eleRhoCorr()const{return eleRhoCorr_;}
  float rhoCorr()const{return rhoCorr_;}
  //first function gets the triggers passed for the event
  //second to functions get the triggers passed for a particlar object
  // int getTrigCode()const;
  // int getTrigCode(const TLorentzVector& p4)const{return getTrigCode(p4.Eta(),p4.Phi(),p4.Eta(),p4.Phi());}
  // int getTrigCode(double detEta,double detPhi,double eta,double phi)const; 

  //some objects have a temporary transisent data cache which root doesnt
  //override when it fills them
  //so it needs to be manually cleared (very annoying)
  virtual void flushTempData()const;
  
  //a tempory function, this removes all the duplicate electrons in the event
  void removeDupEles(std::vector<int>& dupEleNrs);
  void dropTrackerOnlyEles();
  // void rmFEtCorrFromEles();
  void fixElesNrgy();
  

  void printTruth(int nrLines=-1)const;

  //protected:
  bool passTrig(const std::string& trigName,const TLorentzVector& p4)const{return passTrig(trigName,p4.Eta(),p4.Phi());} 
  const SHTrigInfo* getTrig(const std::string& trigName)const;
  bool passL1Trig(const std::string& trigName,double eta,double phi)const;
  bool passTrig(const std::string& trigName,double eta,double phi)const; 
  TLorentzVector getTrigObj(const std::string& trigName,double eta,double phi)const; 
  bool passTrig(const std::string& trigName)const;
  // bool passTrig(const std::string& trigName);
  void printTrigs()const;
  void printTrigsPassed()const;

 private:
 

  SHSuperCluster* getSuperClus_(int clusNr); //allows the event to modify the electron
  float fEtCorr_(float et,int type)const; //little naughty, shouldnt be part of the class

  ClassDef(SHEvent,27) 

};
  
  
#endif
