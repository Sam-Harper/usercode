
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "TTree.h"


class MCTruthFilter : public edm::EDFilter {

private:
  int pid_;
  edm::InputTag genParticlesTag_;

  int nrPass_;
  int nrTot_;
  
public:
  explicit MCTruthFilter(const edm::ParameterSet& para);
  ~MCTruthFilter(){}
  
  virtual bool filter(edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();
};

MCTruthFilter::MCTruthFilter(const edm::ParameterSet& para):nrPass_(0),nrTot_(0)

{
  genParticlesTag_ = para.getParameter<edm::InputTag>("genParticlesTag");
  pid_=para.getParameter<int>("pid");
}

bool MCTruthFilter::filter(edm::Event& event,const edm::EventSetup& setup)
{
  bool pass=false;

  edm::Handle<LHEEventProduct> lheEventHandle;
  event.getByLabel("externalLHEProducer",lheEventHandle);
  if(!lheEventHandle.isValid()) pass=true;
  else{
    const lhef::HEPEUP& hepeup = lheEventHandle->hepeup();
    int nrParts  = hepeup.NUP;
    for(int partNr=0;partNr<nrParts;partNr++){
      //  std::cout <<partNr<<" pid "<<hepeup.IDUP[partNr]<<" stats "<<hepeup.ISTUP[partNr]<<" px "<<hepeup.PUP[partNr][0]<<" "<<hepeup.PUP[partNr][1]<<" "<<hepeup.PUP[partNr][2]<<" "<<hepeup.PUP[partNr][3]<<std::endl;
      if(abs(hepeup.IDUP[partNr])==pid_) pass=true;
      
    }
  }
  //  std::cout <<"pass "<<std::endl;
  nrTot_++;
  if(pass){
    nrPass_++;
    return true;
  }else return false;
  
}
void MCTruthFilter::endJob()
{ 
//  edm::Service<TFileService> fs;
//  fs->file().cd();
  //quick and dirty hack as writing ints directly isnt working
//  TTree* tree = new TTree("preFilterEventCountTree","Event count");
//  tree->Branch("nrPass",&nrPass_,"nrPass/I");
 // tree->Branch("nrTot",&nrTot_,"nrTot/I");
 // tree->Fill();
  
}  




DEFINE_FWK_MODULE(MCTruthFilter);
