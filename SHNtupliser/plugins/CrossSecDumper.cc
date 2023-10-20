
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h" 
#include "FWCore/Framework/interface/Run.h" 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "TTree.h"


class CrossSecDumper : public edm::one::EDAnalyzer<> {

private:


  
  
public:
  explicit CrossSecDumper(const edm::ParameterSet& para);
  ~CrossSecDumper(){}
  
  virtual void beginJob();
  virtual void analyze(const edm::Event& event,const edm::EventSetup& setup);
  virtual void endRun(edm::Run const& iRun, edm::EventSetup const&);
 
};

CrossSecDumper::CrossSecDumper(const edm::ParameterSet& para)

{
 
}

void CrossSecDumper::beginJob()
{
  
}

void CrossSecDumper::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
}

void CrossSecDumper::endRun(edm::Run const& iRun, edm::EventSetup const&)
{ 
   edm::Handle< GenRunInfoProduct > genInfoProduct;
  iRun.getByLabel("generator", genInfoProduct );
  if(genInfoProduct.isValid()) {
    std::cout <<" cross-section int"<<genInfoProduct->internalXSec().value()<<std::endl;
    std::cout <<" cross-section ext LO "<<genInfoProduct->externalXSecLO().value()<<std::endl;  
    std::cout <<" cross-section ext NLO "<<genInfoProduct->externalXSecNLO().value()<<std::endl;

  }
  
}  



DEFINE_FWK_MODULE(CrossSecDumper);
