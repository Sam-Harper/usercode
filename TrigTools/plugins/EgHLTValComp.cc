

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"
#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DataFormats/Math/interface/deltaR.h"
#include <iostream>

//this is a simple class which simulates another trigger based on the already calculated trigger object P4s and filters on that

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"


#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/Common/interface/AssociationMap.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"

#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"


#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "TTree.h"

#include <vector>
#include <string>


 

class EgHLTValComp : public edm::one::EDAnalyzer<> {
public:
  using RecoEcalCandMap = edm::AssociationMap<edm::OneToValue<std::vector<reco::RecoEcalCandidate>, float > >;
  using CandSeedVec =  std::vector<std::pair<reco::RecoEcalCandidateRef,const reco::ElectronSeed*> >;
 
private:
  
  TTree* tree_;
  trigtools::EvtInfoStruct evtInfo_; 
  trigtools::P4Struct candP4_;
  float var1_;
  float var2_;
  int passHLT_;
  std::string treeName_;

  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> candToken_;
  edm::EDGetTokenT<RecoEcalCandMap> var1Token_,var2Token_;
  edm::EDGetTokenT<trigger::TriggerFilterObjectWithRefs> filterToken_;
  
public:
  explicit EgHLTValComp(const edm::ParameterSet& iPara);
  ~EgHLTValComp(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
  
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
  			
  
  float getVal(reco::RecoEcalCandidateRef cand,
		 edm::Handle<RecoEcalCandMap> hademValues);

  void fillTree(const reco::RecoEcalCandidateRef& cand,
		edm::Handle<RecoEcalCandMap>& var1Values,
		edm::Handle<RecoEcalCandMap>& var2Values,
		const std::vector<edm::Ref<reco::RecoEcalCandidateCollection> >& candsPass);
};



EgHLTValComp::EgHLTValComp(const edm::ParameterSet& iPara)
{
  treeName_ = iPara.getParameter<std::string>("treeName");
  setToken(candToken_,iPara,"cands");
  setToken(var1Token_,iPara,"var1");
  setToken(var2Token_,iPara,"var2");  
  setToken(filterToken_,iPara,"hltFilter");  
}

void EgHLTValComp::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree(treeName_.c_str(),"hlt trigger");
  tree_->Branch("evt",&evtInfo_,trigtools::EvtInfoStruct::contents().c_str());
  tree_->Branch("candP4",&candP4_,trigtools::P4Struct::contents().c_str());
  tree_->Branch("var1",&var1_,"var1/F");
  tree_->Branch("var2",&var2_,"var2/F");
  tree_->Branch("passHLT",&passHLT_,"passHLT/I");
  

}
template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token)
{
  edm::Handle<T> handle;
  iEvent.getByToken(token,handle);
  return handle;
}
  

		
float EgHLTValComp::getVal(reco::RecoEcalCandidateRef cand,
			   edm::Handle<RecoEcalCandMap> values)
{
  auto valIt = values->find(cand);
  if(valIt!=values->end()) return valIt->val;
  else return -1;
}
	


		      

bool isInCollection(const reco::RecoEcalCandidateRef& candRef,
		  const std::vector<edm::Ref<reco::RecoEcalCandidateCollection> >& candsPass)	      
{
  return std::find(candsPass.begin(),candsPass.end(),candRef)!=candsPass.end();
}

void EgHLTValComp::fillTree(const reco::RecoEcalCandidateRef& cand,
			    edm::Handle<RecoEcalCandMap>& var1Values,
			    edm::Handle<RecoEcalCandMap>& var2Values,
			    const std::vector<edm::Ref<reco::RecoEcalCandidateCollection> >& candsPass)	      
{
  candP4_.fill(cand->p4());
  passHLT_ = isInCollection(cand,candsPass);
  var1_ = getVal(cand,var1Values);
  var2_ = getVal(cand,var2Values);
  tree_->Fill();

}

void EgHLTValComp::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto cands = getHandle(iEvent,candToken_);
  auto var1Values = getHandle(iEvent,var1Token_);
  auto var2Values = getHandle(iEvent,var2Token_);
  auto hltFilter = getHandle(iEvent,filterToken_);
 
  std::vector<edm::Ref<reco::RecoEcalCandidateCollection> > candsPassingFilter;
  if(hltFilter.isValid()){
    hltFilter->getObjects(trigger::TriggerCluster,candsPassingFilter);
    if(candsPassingFilter.empty()) hltFilter->getObjects(trigger::TriggerPhoton,candsPassingFilter);
  }
  
  evtInfo_.fill(iEvent);
  for(size_t candNr=0;candNr<cands->size();candNr++){
    auto candRef = edm::Ref<reco::RecoEcalCandidateCollection>(cands,candNr);
    fillTree(candRef,var1Values,var2Values,candsPassingFilter);
  }
  
}



//define this as a plug-in
DEFINE_FWK_MODULE(EgHLTValComp);
