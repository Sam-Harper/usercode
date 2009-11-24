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

#include "TFile.h"
#include "TTree.h"

void filterHcalHits(const SHEvent* event,double maxDR,const SHCaloHitContainer& inputHits,SHCaloHitContainer& outputHits);

SHNtupliser::SHNtupliser(const edm::ParameterSet& iPara):
  evtHelper_(),heepEvt_(),shEvtHelper_(),shEvt_(NULL),evtTree_(NULL),outFile_(NULL),nrTot_(0),nrPass_(0)
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
  
  GeomFuncs::loadCaloGeom(ecalGeom,hcalGeom);
}

void SHNtupliser::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
 //  std::cout <<"here"<<std::endl;
//   int nrProd = heep::listAllProducts<l1extra::L1EmParticle>(iEvent,"SHNtupliser");
//   std::cout <<"nr products "<<nrProd<<std::endl;
//   edm::Handle<l1extra::L1EmParticleCollection> test1,test2;
//   edm::InputTag tag1("hltL1extraParticles","Isolated");
//   iEvent.getByLabel(tag1,test1);
//   const std::vector<l1extra::L1EmParticle>& parts1 = *test1;
//   for(size_t i=0;i<parts1.size();i++){
//     std::cout <<"isol i "<<i<<" et "<<parts1[i].et()<<" eta "<<parts1[i].eta()<<std::endl;
//   }
//   edm::InputTag tag2("hltL1extraParticles","NonIsolated");
//   iEvent.getByLabel(tag2,test2);
//   const std::vector<l1extra::L1EmParticle>& parts2 = *test2;
//   for(size_t i=0;i<parts2.size();i++){
//     std::cout <<"non isol i "<<i<<" et "<<parts2[i].et()<<" eta "<<parts2[i].eta()<<" "<<*(parts2[i].gctEmCand())<<std::endl;
//   }
//   //make the heep event (see easy isnt it)
  
  evtHelper_.makeHeepEvent(iEvent,iSetup,heepEvt_);
 

  
  
 //  if(heepEvt_.eventnr()==6567){
//     edm::Handle<CaloTowerCollection> towerHandle;
//     iEvent.getByLabel("towerMaker",towerHandle);
//     const CaloTowerCollection& towers = *towerHandle;
     
//     EgammaTowerIsolation isol(0.3,0.15,0,1,&towers);

//   std::cout <<"H/E pre-sel removed eles "<<std::endl;
//   const std::vector<reco::GsfElectron>& eles = heepEvt_.gsfEles();
//   for(size_t eleNr=0;eleNr<eles.size();eleNr++){
   
//     const reco::GsfElectron& ele = eles[eleNr];
//     std::cout <<"ele nr "<<eleNr<<" et "<<ele.et()<<" eta "<<ele.eta()<<" phi "<<ele.phi()<<" h/e "<<ele.hadronicOverEm()<<ele.isEcalDriven()<<std::endl;
//   }
  
//   edm::Handle<reco::GsfElectronCollection> gsfEleWithHEHandle;
//   iEvent.getByLabel("gsfElectrons",gsfEleWithHEHandle);
//   const std::vector<reco::GsfElectron>& elesWithHE = *gsfEleWithHEHandle;
//   std::cout <<"H/E preselected electrons "<<std::endl;
//   for(size_t eleNr=0;eleNr<elesWithHE.size();eleNr++){
 
//     const reco::GsfElectron& ele = elesWithHE[eleNr];
//     std::cout <<"ele nr "<<eleNr<<" et "<<ele.et()<<" eta "<<ele.eta()<<" phi "<<ele.phi()<<" h/e "<<ele.hadronicOverEm()<<" ecal driven "<<ele.isEcalDriven()<<std::endl;
//   }


//   }



//   edm::Handle< L1GlobalTriggerReadoutRecord > gtRecord;
//   iEvent.getByLabel( edm::InputTag("gtDigis"), gtRecord);
//   const DecisionWord dWord = gtRecord->decisionWord();  // this will get the decision word *before* masking disabled bits

//   bool l1SingleEG15 = menu->gtAlgorithmResult( "L1_SingleEG8", dWord);

//   std::cout <<"L1 "<<l1SingleEG15<<std::endl;

  //isolation test
  //const edm::View<pat::Electron>& eles = heepEvt_.patElectrons();
  //const std::vector<heep::Ele>& eles = heepEvt_.heepElectrons();
  // for(size_t eleNr=0;eleNr<eles.size();eleNr++){
  // std::cout <<"em isolation "<<eles[eleNr].ecalIso()<<" eta "<<eles[eleNr].eta()<<" et "<<eles[eleNr].et()<<std::endl;

  //} 

  //even easier to convert from heep to shEvt

  nrTot_++;
  const std::vector<reco::GsfElectron>& eles = heepEvt_.gsfEles();
  if(eles.size()>=1){
    shEvtHelper_.makeSHEvent(heepEvt_,*shEvt_);
    nrPass_++;
    //shEvt_->getCaloHits().clearEcal();
      
    //SHCaloHitContainer outputHits;
    //filterHcalHits(shEvt_,0.6,shEvt_->getCaloHits(),outputHits);
    //shEvt_->addCaloHits(outputHits);
    
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
