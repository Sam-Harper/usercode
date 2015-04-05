#ifndef SHARPER_TRIGTOOLS_ECALRECHITCOLLCOMPARER
#define SHARPER_TRIGTOOLS_ECALRECHITCOLLCOMPARER

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"

#include "TTree.h"

#include <vector>
#include <string>
#include <iostream>

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}

class EcalRecHitCollComparer : public edm::EDAnalyzer {
private:

  std::vector< edm::EDGetTokenT<EcalRecHitCollection> > coll1Token_;
  std::vector< edm::EDGetTokenT<EcalRecHitCollection> > coll2Token_;
  std::vector<edm::InputTag> coll1Tag_;
  std::vector<edm::InputTag> coll2Tag_;
  std::string treeName_;
  int verbose_;
  int datasetCode_;
  static constexpr int kNrColls_=3;
  int nrColl1_[kNrColls_];
  int nrColl2_[kNrColls_];
  int matchCode_[kNrColls_];
  int globalMatch_;
  trigtools::EvtInfoStruct evtInfo_;

  TTree* tree_; //we dont own this

 
public:
  explicit EcalRecHitCollComparer(const edm::ParameterSet& iPara);
  ~EcalRecHitCollComparer(){}
  void beginJob() override;
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)override;
  //  virtual bool filter(edm::Event& iEvent,const edm::EventSetup& iSetup)override;


};

EcalRecHitCollComparer::EcalRecHitCollComparer(const edm::ParameterSet& iPara)
{
  coll1Tag_=iPara.getParameter<std::vector<edm::InputTag>>("collection1");
  coll2Tag_=iPara.getParameter<std::vector<edm::InputTag>>("collection2");
  
  for(auto& tag : coll1Tag_) coll1Token_.push_back(consumes<EcalRecHitCollection>(tag));
  for(auto& tag : coll2Tag_) coll2Token_.push_back(consumes<EcalRecHitCollection>(tag));
  treeName_=iPara.getParameter<std::string>("treeName");
  verbose_=iPara.getParameter<int>("verbose");
  datasetCode_=iPara.getParameter<int>("datasetCode");
}

void EcalRecHitCollComparer::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree(treeName_.c_str(),"tree");
  tree_->Branch("nrColl1",nrColl1_,"eb/I:ee:es");
  tree_->Branch("nrColl2",nrColl2_,"eb/I:ee:es");
  tree_->Branch("matchCode",matchCode_,"eb/I:ee:es");
  tree_->Branch("globalMatch",&globalMatch_,"globalMatch/I");
  tree_->Branch("evt",&evtInfo_,evtInfo_.contents().c_str());
  tree_->Branch("datasetCode",&datasetCode_,"datasetCode/I");
}

void EcalRecHitCollComparer::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  globalMatch_=1;
  for(size_t collNr=0;collNr<3;collNr++){
    edm::Handle<EcalRecHitCollection> coll1;
    edm::Handle<EcalRecHitCollection> coll2;
    
    iEvent.getByToken(coll1Token_[collNr],coll1);
    iEvent.getByToken(coll2Token_[collNr],coll2);
    nrColl1_[collNr]=coll1->size();
    nrColl2_[collNr]=coll2->size();
    matchCode_[collNr]=0;

  //  bool different=false;
    for(auto& hitInColl1 : *coll1){
      auto hitInColl2 = coll2->find(hitInColl1.id());
      if(hitInColl2==coll2->end()){
	if(verbose_) std::cout <<"hit "<<hitInColl1<<" was in collection "<<coll1Tag_[collNr].encode()<< "but not in collection "<<coll2Tag_[collNr].encode()<<std::endl;
	//different=true;
	matchCode_[collNr]|=0x1;
	globalMatch_=0;
      }
    }
    
    for(auto& hitInColl2 : *coll2){
      auto hitInColl1 = coll1->find(hitInColl2.id());
      if(hitInColl1==coll1->end()){
	if(verbose_) std::cout <<"hit "<<hitInColl2<<" was in collection "<<coll2Tag_[collNr].encode()<< "but not in collection "<<coll1Tag_[collNr].encode()<<std::endl;
	// different=true;
	matchCode_[collNr]|=0x2;
	globalMatch_=0;
      }
    }
  //if(!different) std::cout <<"perfect match "<<coll1Tag_.encode()<<" "<<coll1->size()<<" "<<coll2Tag_.encode()<<" "<<coll2->size()<<std::endl;
  //  perfectMatch_=!different;
  }
  evtInfo_.fill(iEvent);


  tree_->Fill();
  

}
DEFINE_FWK_MODULE(EcalRecHitCollComparer);
#endif
