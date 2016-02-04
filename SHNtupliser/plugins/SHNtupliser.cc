#include "SHarper/SHNtupliser/interface/SHNtupliser.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"
#include "SHarper/SHNtupliser/interface/GeomFuncs.hh"
#include "SHarper/SHNtupliser/interface/SHGeomFiller.h"
#include "SHarper/SHNtupliser/interface/TrigDebugObjHelper.h"
#include "SHarper/SHNtupliser/interface/SHTrigObjContainer.hh"
#include "SHarper/SHNtupliser/interface/SHPFCandContainer.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSumMaker.h"
#include "SHarper/SHNtupliser/interface/PFFuncs.h"
#include "SHarper/SHNtupliser/interface/GenFuncs.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPDebug.h"


#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
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
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "TFile.h"
#include "TTree.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h" 
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
void filterHcalHits(const SHEvent* event,double maxDR,const SHCaloHitContainer& inputHits,SHCaloHitContainer& outputHits);
void filterEcalHits(const SHEvent* event,double maxDR,const SHCaloHitContainer& inputHits,SHCaloHitContainer& outputHits);
void filterCaloTowers(const SHEvent* event,double maxDR,const SHCaloTowerContainer& inputHits,SHCaloTowerContainer& outputHits);

void fillPFClustersECAL(const SHEvent* event,double maxDR,SHPFClusterContainer& shPFClusters,const std::vector<reco::PFCluster>& pfClusters,const std::vector<reco::SuperCluster>& scEB,const std::vector<reco::SuperCluster>& scEE);
void fillPFClustersHCAL(const SHEvent* event,double maxDR,SHPFClusterContainer& shPFClusters,const std::vector<reco::PFCluster>& pfClusters);
int getSCSeedCrysId(uint pfSeedId,const std::vector<reco::SuperCluster>& superClusters);

void dumpPFInfo(const edm::ValueMap<std::vector<reco::PFCandidateRef> >& isoMaps,const edm::Handle<std::vector<reco::GsfElectron> >& eleHandle);

void SHNtupliser::initSHEvent()
{
  if(shEvt_) delete shEvt_;
  shEvt_ = new SHEvent;
}

void SHNtupliser::fillTree()
{
  shEvtTree_.fill();
}

SHNtupliser::SHNtupliser(const edm::ParameterSet& iPara):
  shEvt_(nullptr),shEvtTree_(shEvt_),outFile_(nullptr),nrTot_(0),nrPass_(0),initGeom_(false)
{
  evtHelper_.setup(iPara,consumesCollector(),*this);
  shEvtHelper_.setup(iPara,consumesCollector());
  shEvtTree_.setup(iPara);

  outputFilename_ = iPara.getParameter<std::string>("outputFilename");
  outputGeom_ = iPara.getParameter<bool>("outputGeom");
 
}

SHNtupliser::~SHNtupliser()
{
  if(shEvt_) delete shEvt_;
}

void SHNtupliser::beginJob()
{
  initSHEvent();
 
  std::cout <<"opening file "<<outputFilename_.c_str()<<std::endl;
  edm::Service<TFileService> fs;
  outFile_ = &fs->file();
  outFile_->cd();
  shEvtTree_.makeTree("evtTree");
 
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
  evtHelper_.makeHeepEvent(run,iSetup,heepEvt_);
  std::cout <<"end begin run "<<std::endl;


}

void SHNtupliser::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  if(fillSHEvent(iEvent,iSetup)) fillTree();
} 

bool SHNtupliser::fillSHEvent(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  evtHelper_.makeHeepEvent(iEvent,iSetup,heepEvt_);
 
  //even easier to convert from heep to shEvt
  //std::cout <<"converting eventing" <<std::endl;
  
  nrTot_++;
 
  
  shEvtHelper_.makeSHEvent(heepEvt_,*shEvt_);
  
  SHCaloHitContainer outputHits;
  filterHcalHits(shEvt_,0.5,shEvt_->getCaloHits(),outputHits);  
  filterEcalHits(shEvt_,0.5,shEvt_->getCaloHits(),outputHits);
  shEvt_->addCaloHits(outputHits);
    
  return true;

}


void dumpPFInfo(const edm::ValueMap<std::vector<reco::PFCandidateRef> > & isoMaps,const edm::Handle<std::vector<reco::GsfElectron> >& eleHandle)
{
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    reco::GsfElectronRef ele(eleHandle,eleNr);
    const std::vector<reco::PFCandidateRef>& isoCands =  isoMaps[ele];
    std::cout <<"electron "<<eleNr<<" et "<<ele->et()<<" eta "<<ele->eta()<<std::endl;
    for(size_t candNr=0;candNr<isoCands.size();candNr++){
      std::cout <<"cand "<<candNr<<" / "<<isoCands.size()<<" "<<(*isoCands[candNr])<<std::endl;
    }
  }
  
}




#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
void SHNtupliser::endRun(edm::Run const& iRun, edm::EventSetup const&)
{
  // edm::Handle< GenRunInfoProduct > genInfoProduct;
  // iRun.getByLabel("generator", genInfoProduct );
  // if(genInfoProduct.isValid()) {
  //   std::cout <<" cross-section "<<genInfoProduct->internalXSec().value()<<std::endl;
  // }
  
}


void SHNtupliser::endJob()
{ 
  outFile_->cd();
  //quick and dirty hack as writing ints directly isnt working
  TTree* tree = new TTree("eventCountTree","Event count");
  tree->Branch("nrPass",&nrPass_,"nrPass/I");
  tree->Branch("nrTot",&nrTot_,"nrTot/I");
  tree->Fill();

  std::cout <<"job ended "<<std::endl;
}

void filterHcalHits(const SHEvent* event,double maxDR,const SHCaloHitContainer& inputHits,SHCaloHitContainer& outputHits)
{

  std::vector<std::pair<float,float> > eleEtaPhi;
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    eleEtaPhi.push_back(std::make_pair(ele->detEta(),ele->detPhi()));
  }
  
  // outputHits.clear();
  double maxDR2 = maxDR*maxDR;
  for(size_t hitNr=0;hitNr<inputHits.nrHcalHitsStored();hitNr++){
    int detId = inputHits.getHcalHitByIndx(hitNr).detId();
    double cellEta=0,cellPhi=0;
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

void filterEcalHits(const SHEvent* event,double maxDR,const SHCaloHitContainer& inputHits,SHCaloHitContainer& outputHits)
{

  std::vector<std::pair<float,float> > eleEtaPhi;
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    eleEtaPhi.push_back(std::make_pair(ele->detEta(),ele->detPhi()));
  }
  
  //outputHits.clear();
  double maxDR2 = maxDR*maxDR;
  for(size_t hitNr=0;hitNr<inputHits.nrEcalHitsStored();hitNr++){
    int detId = inputHits.getEcalHitByIndx(hitNr).detId();
    double cellEta=0,cellPhi=0;
    GeomFuncs::getCellEtaPhi(detId,cellEta,cellPhi);
    
    bool accept =false;
    for(size_t eleNr=0;eleNr<eleEtaPhi.size();eleNr++){
      if(MathFuncs::calDeltaR2(eleEtaPhi[eleNr].first,eleEtaPhi[eleNr].second,
			       cellEta,cellPhi)<maxDR2){
	accept=true;
	break;
      }
    }//end ele loop
    if(accept) outputHits.addHit(inputHits.getEcalHitByIndx(hitNr));
    
  }//end hit loop


}
  
void filterCaloTowers(const SHEvent* event,double maxDR,const SHCaloTowerContainer& inputHits,SHCaloTowerContainer& outputHits)
{

  std::vector<std::pair<float,float> > eleEtaPhi;
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    eleEtaPhi.push_back(std::make_pair(ele->detEta(),ele->detPhi()));
  }
  
  outputHits.clear();
  double maxDR2 = maxDR*maxDR;
  for(size_t hitNr=0;hitNr<inputHits.nrCaloTowersStored();hitNr++){
    float towerEta = inputHits.getCaloTowerByIndx(hitNr).eta();
    float towerPhi = inputHits.getCaloTowerByIndx(hitNr).phi(); 
  
    bool accept =false;
    for(size_t eleNr=0;eleNr<eleEtaPhi.size();eleNr++){
      if(MathFuncs::calDeltaR2(eleEtaPhi[eleNr].first,eleEtaPhi[eleNr].second,
			       towerEta,towerPhi)<maxDR2){
	accept=true;
	break;
      }
    }//end ele loop
    if(accept) outputHits.addTower(inputHits.getCaloTowerByIndx(hitNr));
    
  }//end hit loop


}


void addPFClustersVec(std::vector<const reco::CaloCluster*>& clus,const std::vector<reco::SuperCluster>& scs)
{
  for(auto& sc : scs){
    for(reco::CaloCluster_iterator clusIt  = sc.clustersBegin();clusIt!=sc.clustersEnd();++clusIt){

      //    const reco::PFCluster* pfClus = dynamic_cast<const reco::PFCluster*>(&**clusIt);
      // std::cout<<typeid(**clusIt).name()<<" pf "<<pfClus<<std::endl;
      clus.push_back((&**clusIt));
    }
  }
}



//define this as a plug-in
DEFINE_FWK_MODULE(SHNtupliser);
