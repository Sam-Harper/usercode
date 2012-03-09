

#include "SHarper/TrigTools/interface/HOverEComparer.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/AssociationMap.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"

#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "FWCore/Framework/interface/MakerMacros.h"
//temp for debugging
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"

//
// constructors and destructor
//
HOverEComparer::HOverEComparer(const edm::ParameterSet& iConfig) : tree_(0), file_(0)
{
  candTag_            = iConfig.getParameter< edm::InputTag > ("candTag");
  
 
  newHadTag_ = iConfig.getParameter< edm::InputTag >("newHadTag");
  oldHadTag_ = iConfig.getParameter< edm::InputTag >("oldHadTag");
  


  std::string outFilename = iConfig.getParameter<std::string>("outFilename");
  if(!outFilename.empty()){
    file_= new TFile(outFilename.c_str(),"RECREATE");
    tree_ = new TTree("hltEleTree","tree");
    tree_->Branch("evt",&evtInfo_,EvtInfoStruct::contents().c_str());
    tree_->Branch("hltEle",&eleData_,EleDataStruct::contents().c_str());
    tree_->Branch("eleNr",&eleNr_,"eleNr/I");
   
  }
  
}


HOverEComparer::~HOverEComparer()
{
  if(file_) {
    file_->Write();
    delete file_;
  
  }
}


// ------------ method called to produce the data  ------------
void
HOverEComparer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
 

  edm::Handle<trigger::TriggerFilterObjectWithRefs> PrevFilterOutput;

  iEvent.getByLabel (candTag_,PrevFilterOutput);

  std::vector<edm::Ref<reco::RecoEcalCandidateCollection> > recoecalcands;
  PrevFilterOutput->getObjects(trigger::TriggerCluster, recoecalcands);
 
   //debugging 
 
  //edm::InputTag dEtaInputTag("hltL1GsfTrackVars","Deta");
  //edm::InputTag dPhiInputTag("hltL1GsfTrackVars","Dphi"); 
  edm::Handle<reco::RecoEcalCandidateIsolationMap> oldHandle;
  iEvent.getByLabel(oldHadTag_,oldHandle);
  edm::Handle<reco::RecoEcalCandidateIsolationMap> newHandle;
  iEvent.getByLabel(newHadTag_,newHandle); 

 

 
  evtInfo_.fill(iEvent);
 


  // std::cout <<"start of event "<<iEvent.id().event()<<std::endl;
  for (unsigned int i=0; i<recoecalcands.size(); i++) {

    edm::Ref<reco::RecoEcalCandidateCollection> ref = recoecalcands[i];
    reco::SuperClusterRef superClusRef = ref->superCluster();
   
    reco::RecoEcalCandidateIsolationMap::const_iterator oldIt = oldHandle->find(ref );
    reco::RecoEcalCandidateIsolationMap::const_iterator newIt = newHandle->find(ref );
    float oldHad=-1;
    float newHad=-1;
    if(oldIt!=oldHandle->end()) oldHad=oldIt->val;
    if(newIt!=newHandle->end()) newHad=newIt->val;

    eleData_.fill(*superClusRef,newHad,oldHad);
    eleNr_=i;
    if(tree_) tree_->Fill();
    
  }//end loop over candidates
 
  

}


void HOverEComparer::EleDataStruct::fill(const reco::SuperCluster& superClus,float iNewHad,float iOldHad)
{
  nrgy = superClus.energy();
  et = superClus.energy()*sin(superClus.position().theta());
  detEta = superClus.eta();
  detPhi = superClus.phi();
  
  newHad = iNewHad;
  oldHad = iOldHad;
}
void HOverEComparer::EvtInfoStruct::fill(const edm::Event& event)
{
  runnr = event.id().run();
  lumiSec = event.luminosityBlock();
  eventnr = event.id().event();
  bx = event.bunchCrossing();
}


//define this as a plug-in
DEFINE_FWK_MODULE(HOverEComparer);
