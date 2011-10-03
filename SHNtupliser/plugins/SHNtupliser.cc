#include "SHarper/SHNtupliser/interface/SHNtupliser.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"
#include "SHarper/SHNtupliser/interface/GeomFuncs.hh"
#include "SHarper/SHNtupliser/interface/SHGeomFiller.h"
#include "SHarper/SHNtupliser/interface/TrigDebugObjHelper.h"
#include "SHarper/SHNtupliser/interface/SHTrigObjContainer.hh"

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
  evtHelper_(),heepEvt_(),shEvtHelper_(),shEvt_(NULL),evtTree_(NULL),outFile_(NULL),nrTot_(0),nrPass_(0),initGeom_(false),trigDebugHelper_(NULL),shTrigObjs_(NULL),shTrigObjs2ndTrig_(NULL),shEvt2ndTrig_(NULL)
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
  minNrJetToPass_ = iPara.getParameter<int>("minNrJetToPass");
  
  shEvtHelper_.setDatasetCode(datasetCode);
  shEvtHelper_.setEventWeight(eventWeight);
 
  useHLTDebug_ = iPara.getParameter<bool>("useHLTDebug");
  compTwoMenus_ = iPara.getParameter<bool>("compTwoMenus");
  hltTag_ = iPara.getParameter<std::string>("hltProcName");
  secondHLTTag_ = iPara.getParameter<std::string>("secondHLTTag");
  if(useHLTDebug_){
    trigDebugHelper_ = new TrigDebugObjHelper(iPara);
  }
}

SHNtupliser::~SHNtupliser()
{
  if(shEvt_) delete shEvt_;
  if(outFile_) delete outFile_;
  if(trigDebugHelper_) delete trigDebugHelper_;
  if(shTrigObjs_) delete shTrigObjs_;
}

void SHNtupliser::beginJob()
{
  shEvt_= new SHEvent;
  std::cout <<"opening file "<<outputFilename_.c_str()<<std::endl;
 outFile_ = new TFile(outputFilename_.c_str(),"RECREATE");
  evtTree_= new TTree("evtTree","Event Tree");
  evtTree_->Branch("EventBranch","SHEvent",&shEvt_,32000,2);
  
  if(compTwoMenus_){
    shEvt2ndTrig_ = new SHEvent;
    evtTree_->Branch("Event2ndTrig","SHEvent",&shEvt2ndTrig_,32000,2);
  }
  if(useHLTDebug_) {
    shTrigObjs_ = new SHTrigObjContainer;
    evtTree_->Branch("HLTDebugObjs","SHTrigObjContainer",&shTrigObjs_,32000,2); 
    
    if(compTwoMenus_){
      shTrigObjs2ndTrig_ = new SHTrigObjContainer;
      evtTree_->Branch("HLTDebugObjs2","SHTrigObjContainer",&shTrigObjs2ndTrig_,32000,2);
    }

  }

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
  std::cout <<"end begin run "<<std::endl;
}


void SHNtupliser::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  
  evtHelper_.makeHeepEvent(iEvent,iSetup,heepEvt_);
 
  //even easier to convert from heep to shEvt



  nrTot_++;
  //  std::cout <<"analysing "<<std::endl;
    shEvtHelper_.makeSHEvent(heepEvt_,*shEvt_);
    // std::cout <<"made even "<<std::endl;
    if(useHLTDebug_) trigDebugHelper_->fillDebugTrigObjs(iEvent,shTrigObjs_);
    if(compTwoMenus_){ //ugly hack alert...
      shEvt2ndTrig_->clear();
      if(useHLTDebug_){
	trigDebugHelper_->setHLTTag(secondHLTTag_);
	trigDebugHelper_->fillDebugTrigObjs(iEvent,shTrigObjs2ndTrig_);
	trigDebugHelper_->setHLTTag(hltTag_);
      }
      shEvtHelper_.addEventPara(heepEvt_,*shEvt2ndTrig_);
      edm::Handle<trigger::TriggerEvent> trigEvt2nd;
      edm::Handle<edm::TriggerResults> trigResults2nd;
      iEvent.getByLabel(edm::InputTag("hltTriggerSummaryAOD","",secondHLTTag_),trigEvt2nd);
      iEvent.getByLabel(edm::InputTag("TriggerResults","",secondHLTTag_),trigResults2nd);
      const edm::TriggerNames& trigNames2nd = iEvent.triggerNames(*trigResults2nd);
      shEvtHelper_.addTrigInfo(*trigEvt2nd,*trigResults2nd,trigNames2nd,*shEvt2ndTrig_);
    }

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
    //   shEvt_->getCaloHits().clear();
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



//detPhi,detEta = SC eta/phi (HLT uses SC eta/phi except for the EleID trigger which uses Ele eta/phi)
//maxDeltaR = size of cone to match in (only works for HLT, not L1...)
//filterName = name of filter, note unless you have HLT debug, only last filter is stored
//hltTag = the process name of the HLT, usually HLT but may be different if the HLT was re-run
//note this is whether an object passes the filter, NOT if the filter was passed (the two are different in the case of multi object filters)
bool passFilter(const edm::Event& iEvent,float detEta,float detPhi,std::string filterName,const std::string hltTag,const double maxDeltaR=0.1)
{    
  
  edm::Handle<trigger::TriggerEvent> trigEvt;
  iEvent.getByLabel("hltTriggerSummaryAOD",hltTag,trigEvt);
  

  size_t filterNrInEvt = trigEvt->filterIndex(edm::InputTag(filterName,"",hltTag).encode());
  if(filterNrInEvt<trigEvt->sizeFilters()){ //filter found in event

    const trigger::Keys& trigKeys = trigEvt->filterKeys(filterNrInEvt);  //trigger::Keys is actually a vector<uint16_t> holding the position of trigger objects in the trigger collection passing the filter
    const trigger::TriggerObjectCollection & trigObjColl(trigEvt->getObjects());
    for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){ //we now have access to all trigger objects passing filter
      float trigObjEta = trigObjColl[*keyIt].eta();
      float trigObjPhi = trigObjColl[*keyIt].phi();
      if (reco::deltaR(detEta,detPhi,trigObjEta,trigObjPhi) < maxDeltaR){
	return true;
      }//end dR<maxDeltaR trig obj match test
    }//end loop over all objects passing filter
  }//check filter is present in event
  return false;
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
