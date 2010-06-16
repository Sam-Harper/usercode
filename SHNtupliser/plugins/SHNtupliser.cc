#include "SHarper/SHNtupliser/interface/SHNtupliser.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"
#include "SHarper/SHNtupliser/interface/GeomFuncs.hh"
#include "SHarper/SHNtupliser/interface/SHGeomFiller.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPDebug.h"

#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "TFile.h"
#include "TTree.h"

void filterHcalHits(const SHEvent* event,double maxDR,const SHCaloHitContainer& inputHits,SHCaloHitContainer& outputHits);

SHNtupliser::SHNtupliser(const edm::ParameterSet& iPara):
  evtHelper_(),heepEvt_(),shEvtHelper_(),shEvt_(NULL),evtTree_(NULL),outFile_(NULL),nrTot_(0),nrPass_(0),initGeom_(false)
{
  evtHelper_.setup(iPara);
  shEvtHelper_.setup(iPara);

  outputFilename_ = iPara.getParameter<std::string>("outputFilename");
  
  double eventWeight = iPara.getParameter<double>("sampleWeight");
  int datasetCode = iPara.getParameter<int>("datasetCode");  
  outputGeom_ = iPara.getParameter<bool>("outputGeom");
   
  minSCEtToPass_ = iPara.getParameter<double>("minSCEtToPass");
  minNrSCToPass_ = iPara.getParameter<int>("minNrSCToPass");
  
  minJetEtToPass_ = iPara.getParameter<double>("minJetEtToPass");
  minNrJetToPass_ = iPara.getParameter<double>("minNrJetTopass");
  
  shEvtHelper_.setDatasetCode(datasetCode);
  shEvtHelper_.setEventWeight(eventWeight);

}

SHNtupliser::~SHNtupliser()
{
  if(shEvt_) delete shEvt_;
  if(outFile_) delete outFile_;
}

void SHNtupliser::beginJob()
{
  shEvt_= new SHEvent;
  std::cout <<"opening file "<<outputFilename_.c_str()<<std::endl;
 outFile_ = new TFile(outputFilename_.c_str(),"RECREATE");
  evtTree_= new TTree("evtTree","Event Tree");
  evtTree_->Branch("EventBranch","SHEvent",&shEvt_,32000,2);
  // scTree_=new TTree("scTree","tree");
  // scTree_->Branch("sc",&oldSigmaIEtaIEta_,"oldSigmaIEtaIEta/F:newSigmaIEtaIEta:affectedByCaloNavBug:scNrgy:scEta:scPhi:scEt");


 
 
} 

void SHNtupliser::beginRun(const edm::Run& run,const edm::EventSetup& iSetup)
{ 
  std::cout <<"begin run "<<initGeom_<<std::endl;
  if(!initGeom_){
  //write out calogeometry
   
    SHGeomFiller geomFiller(iSetup);  
    SHCaloGeom ecalGeom(SHCaloGeom::ECAL);
    SHCaloGeom hcalGeom(SHCaloGeom::HCAL);
    geomFiller.fillEcalGeom(ecalGeom);
    geomFiller.fillHcalGeom(hcalGeom);
    if(outputGeom_){
      std::cout <<"writing geom "<<std::endl;
      outFile_->WriteObject(&ecalGeom,"ecalGeom");
      outFile_->WriteObject(&hcalGeom,"hcalGeom");
    }
    GeomFuncs::loadCaloGeom(ecalGeom,hcalGeom);
    initGeom_=true;
  }
}


void SHNtupliser::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  
  evtHelper_.makeHeepEvent(iEvent,iSetup,heepEvt_);
 
  //even easier to convert from heep to shEvt

  nrTot_++;
 
    shEvtHelper_.makeSHEvent(heepEvt_,*shEvt_);

  
    bool passSC=false;

    int nrSCPassing=0;
    for(int scNr=0;scNr<shEvt_->nrSuperClus();scNr++){
      if(shEvt_->getSuperClus(scNr)->et()>minSCEtToPass_){
 	nrSCPassing++;
      }
    }
    if(nrSCPassing>=minNrSCToPass_) passSC=true;
    
    
    bool passJet=false;
    int nrJetPassing=0;
    for(int jetNr=0;jetNr<shEvt_->nrJets();jetNr++){
      if(shEvt_->getJet(jetNr)->et()>minJetEtToPass_){
	nrJetPassing++;
      }
    }
    if(nrJetPassing>=minNrJetToPass_) passJet=true;
    
    


//     //drop all calo hits
//     shEvt_->getCaloHits().clear();
//     bool passEt=false;
//     for(int eleNr=0;eleNr<shEvt_->nrElectrons();eleNr++){
//       if(shEvt_->getElectron(eleNr)->et()>15){
// 	passEt=true;
// 	break;
//       }
//     }
   
    // SHCaloHitContainer outputHits;
    //filterHcalHits(shEvt_,0.6,shEvt_->getCaloHits(),outputHits);
    //shEvt_->addCaloHits(outputHits);
    
    if(passSC || passJet){
      nrPass_++;
      evtTree_->Fill();
    }
}






void SHNtupliser::endJob()
{ 
  outFile_->cd();
  //quick and dirty hack as writing ints directly isnt working
  TTree* tree = new TTree("eventCountTree","Event count");
  tree->Branch("nrPass",&nrPass_,"nrPass/I");
  tree->Branch("nrTot",&nrTot_,"nrTot/I");
  tree->Fill();
  
  //outFile_->WriteObject(&nrPass_,"nrPass");
  //outFile_->WriteObject(&nrTot_,"nrTot");
  outFile_->Write();
  outFile_->Close();
  delete outFile_;
  outFile_=NULL;
  evtTree_=NULL; //it is owned by the file, once closed its gone 
  delete shEvt_;
  shEvt_=NULL;
}

void filterHcalHits(const SHEvent* event,double maxDR,const SHCaloHitContainer& inputHits,SHCaloHitContainer& outputHits)
{

  std::vector<std::pair<float,float> > eleEtaPhi;
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    eleEtaPhi.push_back(std::make_pair(ele->detEta(),ele->detPhi()));
  }
  
  outputHits.clear();
  double maxDR2 = maxDR*maxDR;
  for(size_t hitNr=0;hitNr<inputHits.nrHcalHitsStored();hitNr++){
    int detId = inputHits.getHcalHitByIndx(hitNr).detId();
    double cellEta,cellPhi;
    GeomFuncs::getCellEtaPhi(detId,cellEta,cellPhi);
    
    bool accept =false;
    for(size_t eleNr=0;eleNr<eleEtaPhi.size();eleNr++){
      if(MathFuncs::calDeltaR2(eleEtaPhi[eleNr].first,eleEtaPhi[eleNr].second,
			       cellEta,cellPhi)<maxDR2){
	accept=true;
	break;
      }
    }//end ele loop
    if(accept) outputHits.addHit(inputHits.getHcalHitByIndx(hitNr));
    
  }//end hit loop


}
  



//define this as a plug-in
DEFINE_FWK_MODULE(SHNtupliser);
