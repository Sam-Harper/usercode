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
  minEtToPassEvent_ = iPara.getParameter<double>("minEtToPassEvent");
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
    std::cout <<"writing geom "<<std::endl;
    SHGeomFiller geomFiller(iSetup);  
    SHCaloGeom ecalGeom(SHCaloGeom::ECAL);
    SHCaloGeom hcalGeom(SHCaloGeom::HCAL);
    geomFiller.fillEcalGeom(ecalGeom);
    geomFiller.fillHcalGeom(hcalGeom);
    if(outputGeom_){
      outFile_->WriteObject(&ecalGeom,"ecalGeom");
      outFile_->WriteObject(&hcalGeom,"hcalGeom");
    }
    GeomFuncs::loadCaloGeom(ecalGeom,hcalGeom);
    initGeom_=true;
  }
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
 

  

//   const std::vector<reco::SuperCluster>& superClusEB = heepEvt_.superClustersEB(); 
//   const std::vector<reco::SuperCluster>& superClusEE = heepEvt_.superClustersEE();
 
//   for(size_t scNr=0;scNr<superClusEB.size();scNr++){
//     const reco::SuperCluster& sc = superClusEB[scNr];
//     DetId seedId =EcalClusterTools::getMaximum(*sc.seed(),heepEvt_.ebHitsFull()).first;
//     oldSigmaIEtaIEta_ = EcalClusterTools::localCovariances(*sc.seed(),heepEvt_.ebHitsFull(),heepEvt_.handles().caloTopology.product())[0];
//     newSigmaIEtaIEta_ = EcalClusterTools::localCovariances(seedId,heepEvt_.ebHitsFull())[0];
//     affectedByCaloNavBug_=0;
//     scNrgy_=sc.energy();
//     scEta_=sc.eta();
//     scPhi_=sc.phi();
//     scEt_=sc.energy()*sin(sc.position().theta());
//     scTree_->Fill();
   
//   }
 
//   for(size_t scNr=0;scNr<superClusEE.size();scNr++){
//     const reco::SuperCluster& sc = superClusEE[scNr];
//     DetId seedId =EcalClusterTools::getMaximum(*sc.seed(),heepEvt_.eeHitsFull()).first;
//     oldSigmaIEtaIEta_ = EcalClusterTools::localCovariances(*sc.seed(),heepEvt_.eeHitsFull(),heepEvt_.handles().caloTopology.product())[0];
//     newSigmaIEtaIEta_= EcalClusterTools::localCovariances(seedId,heepEvt_.eeHitsFull())[0];
//     affectedByCaloNavBug_=0;
//     for(int ix=-2;ix<=2;ix++){
//       for(int iy=-2;iy<=2;iy++){
// 	EEDetId eeSeedId(seedId);
// 	if(eeSeedId.offsetBy(ix,iy)==DetId(0)) affectedByCaloNavBug_=1;
//       }
//     }
//     scNrgy_=sc.energy();
//     scEta_=sc.eta();
//     scPhi_=sc.phi();
//     scEt_=sc.energy()*sin(sc.position().theta());
//     scTree_->Fill();
   
    
//   }


  //even easier to convert from heep to shEvt

  nrTot_++;
  // const std::vector<reco::GsfElectron>& eles = heepEvt_.gsfEles();
  // if(eles.size()>=1){
 

    shEvtHelper_.makeSHEvent(heepEvt_,*shEvt_);

  
      bool passEt=false;
    for(int eleNr=0;eleNr<shEvt_->nrElectrons();eleNr++){
       if(shEvt_->getElectron(eleNr)->et()>minEtToPassEvent_){
 	passEt=true;
 	break;
       }
     }
   
    
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
    
    if(passEt){
      nrPass_++;
        evtTree_->Fill();
      //}
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
