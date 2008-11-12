#ifndef SHARPER_SHNTUPLISER_SHEVENTHELPER
#define SHARPER_SHNTUPLISER_SHEVENTHELPER

//class: SHEventHelper
//Description: a class which converts a heep::Event to a SHEvent
//
//Implimentation: 
// 
//Author: Sam Harper

//note: as of 16/09/08, we dont have full functionality, some functions are empty

#include "SHarper/SHNtupliser/interface/SHCaloHit.hh"

#include "DataFormats/DetId/interface/DetId.h"

#include <vector>


namespace heep{
  class Event;
}

class SHEvent;

class SHEventHelper {

private:
  int datasetCode_; //an internal code to my ntuples which only means something to me
  float eventWeight_; //the weight of the event, again only really means something to me
  bool isMC_;// if we are running on mc or not
  int nrGenPartToStore_;

  //storage for SHCaloHits so we dont have to keep reallocating the memory 
  mutable std::vector<SHCaloHit> ecalHitVec_;
  mutable std::vector<SHCaloHit> hcalHitVec_;
  
  //magic numbers, number of hcal cells (need to fix)
  static const int kNrHcalBarrelCrys_=1296*2;
  static const int kNrHcalEndcapCrys_=1296*2;
  
public:
  explicit SHEventHelper(int datasetCode=0,float eventWeight=1.0);
  
  //the two modifiers
  void setDatasetCode(int datasetCode){datasetCode_=datasetCode;}
  void setEventWeight(float weight){eventWeight_=weight;}

  //the main master function which calls all the others
  void makeSHEvent(const heep::Event & heepEvent, SHEvent& shEvent)const;
 
  void addEventPara(const heep::Event& heepEvent, SHEvent& shEvent)const;
  void addElectrons(const heep::Event& heepEvent, SHEvent& shEvent)const;
  void addSuperClusters(const heep::Event& heepEvent, SHEvent& shEvent)const;
  void addCaloHits(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addEcalHits(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addHcalHits(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addTrigInfo(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addJets(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addIsolTrks(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addIsolClus(const heep::Event& heepEvent,SHEvent& shEvent)const{}
  void addMet(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addMCParticles(const heep::Event& heepEvent,SHEvent& shEvent)const;

private:
  //the hashing functions for vector positions
  int ecalHitHash_(const DetId detId)const;
  int hcalHitHash_(const DetId detId)const; 
  
  //makes the ecal and hcalHitVec the correct size, puts the correct detId in the correct position and -999.s the energies
  void initEcalHitVec_()const;
  void initHcalHitVec_()const;
  
  

};

#endif
