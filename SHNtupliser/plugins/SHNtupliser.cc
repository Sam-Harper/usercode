#include "SHarper/SHNtupliser/interface/SHNtupliser.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"

#include "SHarper/SHNtupliser/interface/SHGeomFiller.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPDebug.h"

#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TFile.h"
#include "TTree.h"


SHNtupliser::SHNtupliser(const edm::ParameterSet& iPara):
  evtHelper_(),heepEvt_(),shEvtHelper_(),shEvt_(NULL),evtTree_(NULL),outFile_(NULL)
{
  evtHelper_.setup(iPara);
  
  outputFilename_ = iPara.getParameter<std::string>("outputFilename");
  
  double eventWeight = iPara.getParameter<double>("sampleWeight");
  int datasetCode = iPara.getParameter<int>("datasetCode");  
  shEvtHelper_.setDatasetCode(datasetCode);
  shEvtHelper_.setEventWeight(eventWeight);
}

SHNtupliser::~SHNtupliser()
{
  if(shEvt_) delete shEvt_;
  if(outFile_) delete outFile_;
}

void SHNtupliser::beginJob(const edm::EventSetup& iSetup)
{
  shEvt_= new SHEvent;
  outFile_ = new TFile(outputFilename_.c_str(),"RECREATE");
  evtTree_= new TTree("evtTree","Event Tree");
  evtTree_->Branch("EventBranch","SHEvent",&shEvt_,32000,2);

  //write out calogeometry
  SHGeomFiller geomFiller(iSetup);  
  SHCaloGeom ecalGeom(SHCaloGeom::ECAL);
  SHCaloGeom hcalGeom(SHCaloGeom::HCAL);
  geomFiller.fillEcalGeom(ecalGeom);
  geomFiller.fillHcalGeom(hcalGeom);
  outFile_->WriteObject(&ecalGeom,"ecalGeom");
  outFile_->WriteObject(&hcalGeom,"hcalGeom");
  

}

void SHNtupliser::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  //heep::listAllProducts<reco::TrackCollection>(iEvent,"SHNtupliser");


  //make the heep event (see easy isnt it)
  evtHelper_.makeHeepEvent(iEvent,iSetup,heepEvt_);

  //isolation test
  //const edm::View<pat::Electron>& eles = heepEvt_.patElectrons();
  //const std::vector<heep::Ele>& eles = heepEvt_.heepElectrons();
  // for(size_t eleNr=0;eleNr<eles.size();eleNr++){
  // std::cout <<"em isolation "<<eles[eleNr].ecalIso()<<" eta "<<eles[eleNr].eta()<<" et "<<eles[eleNr].et()<<std::endl;

  //} 

  //even easier to convert from heep to shEvt
  shEvtHelper_.makeSHEvent(heepEvt_,*shEvt_);
  
  //almost trivial
  evtTree_->Fill();
}






void SHNtupliser::endJob()
{
  outFile_->Write();
  outFile_->Close();
  delete outFile_;
  outFile_=NULL;
  evtTree_=NULL; //it is owned by the file, once closed its gone 
  delete shEvt_;
  shEvt_=NULL;
}


//define this as a plug-in
DEFINE_FWK_MODULE(SHNtupliser);
