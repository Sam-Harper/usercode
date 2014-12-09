
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "TTree.h"


class MCEMuTruthFilter : public edm::EDFilter {

private:
  int nrElesRequired_;
  int nrMuonsRequired_;
  edm::InputTag genParticlesTag_;

  int nrPass_;
  int nrTot_;
  
public:
  explicit MCEMuTruthFilter(const edm::ParameterSet& para);
  ~MCEMuTruthFilter(){}
  
  virtual bool filter(edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();
};

MCEMuTruthFilter::MCEMuTruthFilter(const edm::ParameterSet& para):nrPass_(0),nrTot_(0)

{
  genParticlesTag_ = para.getParameter<edm::InputTag>("genParticlesTag");
  nrElesRequired_=para.getParameter<int>("nrElesRequired");
  nrMuonsRequired_=para.getParameter<int>("nrMuonsRequired");
}

bool MCEMuTruthFilter::filter(edm::Event& event,const edm::EventSetup& setup)
{
  edm::Handle<reco::GenParticleCollection> genParticles;
  event.getByLabel(genParticlesTag_,genParticles);
  bool pass=false;
  if(!genParticles.isValid()) pass=true;
  int nrEles=0;
  int nrMuons=0;

  for(auto part : *genParticles){
    if(part.status()==1){
      if(abs(part.pdgId())==11 && part.p4().Et()>30) nrEles++;
      if(abs(part.pdgId())==13 && part.p4().Et()>30) nrMuons++;
    }
  }

  if(nrEles>=nrElesRequired_ && nrMuons>=nrMuonsRequired_) pass=true;
  
  nrTot_++;
  if(pass){
    nrPass_++;
    return true;
  }else return false;
  
}
void MCEMuTruthFilter::endJob()
{ 
//  edm::Service<TFileService> fs;
//  fs->file().cd();
  //quick and dirty hack as writing ints directly isnt working
//  TTree* tree = new TTree("preFilterEventCountTree","Event count");
//  tree->Branch("nrPass",&nrPass_,"nrPass/I");
 // tree->Branch("nrTot",&nrTot_,"nrTot/I");
 // tree->Fill();
  
}  




DEFINE_FWK_MODULE(MCEMuTruthFilter);
