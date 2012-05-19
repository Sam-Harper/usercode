
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "TTree.h"


class EGammaFilter : public edm::EDFilter {

private:
  edm::InputTag phoTag_;
  int nrPhosRequired_;
  float phoEtCut_;
  edm::InputTag eleTag_;
  int nrElesRequired_;
  float eleEtCut_;

  int nrPass_;
  int nrTot_;
  
public:
  explicit EGammaFilter(const edm::ParameterSet& para);
  ~EGammaFilter(){}
  
  virtual bool filter(edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();
  bool passPho(edm::Event& event)const;
  bool passEle(edm::Event& event)const;
};

EGammaFilter::EGammaFilter(const edm::ParameterSet& para):nrPass_(0),nrTot_(0)

{
  eleTag_=para.getParameter<edm::InputTag>("eleTag");
  phoTag_=para.getParameter<edm::InputTag>("phoTag");
  nrElesRequired_=para.getParameter<int>("nrElesRequired");
  nrPhosRequired_=para.getParameter<int>("nrPhosRequired");
  phoEtCut_ = para.getParameter<double>("phoEtCut");
  eleEtCut_ = para.getParameter<double>("eleEtCut");
}

bool EGammaFilter::filter(edm::Event& event,const edm::EventSetup& setup)
{ 
  nrTot_++;
  if(passPho(event) && passEle(event)){
    nrPass_++;
    return true;
  }else return false;
  
}
void EGammaFilter::endJob()
{ 
  edm::Service<TFileService> fs;
  fs->file().cd();
  //quick and dirty hack as writing ints directly isnt working
  TTree* tree = new TTree("preFilterEventCountTree","Event count");
  tree->Branch("nrPass",&nrPass_,"nrPass/I");
  tree->Branch("nrTot",&nrTot_,"nrTot/I");
  tree->Fill();
  
}  
bool EGammaFilter::passPho(edm::Event& event)const
{
  if(nrPhosRequired_<=0) return true; //automatically passes
  
  edm::Handle<reco::PhotonCollection> phoHandle;
  event.getByLabel(phoTag_,phoHandle);
  
  int nrPhos=0;
  for(size_t phoNr=0;phoNr<phoHandle->size();phoNr++){
    const reco::Photon& pho = (*phoHandle)[phoNr];
    if(pho.et()>phoEtCut_) nrPhos++;
  }
  
  return nrPhos>=nrPhosRequired_;
}

bool EGammaFilter::passEle(edm::Event& event)const
{
  if(nrElesRequired_<=0) return true; //automatically passes
  
  edm::Handle<reco::GsfElectronCollection> eleHandle;
  event.getByLabel(eleTag_,eleHandle);
  
  int nrEles=0;
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    const reco::GsfElectron& ele = (*eleHandle)[eleNr];
    if(ele.et()/ele.energy()*ele.superCluster()->energy()>eleEtCut_ && ele.ecalDrivenSeed()) nrEles++;
  }
  
  return nrEles>=nrElesRequired_;
}



DEFINE_FWK_MODULE(EGammaFilter);
