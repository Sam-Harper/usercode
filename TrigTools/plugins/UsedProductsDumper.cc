
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/UsedProducts.h"

class UsedProductsDumper : public edm::EDAnalyzer {

private:

  
public:
  explicit UsedProductsDumper(const edm::ParameterSet&){}
  ~UsedProductsDumper(){}
  
  virtual void beginJob(){}
  virtual void analyze(const edm::Event& ,const edm::EventSetup& ){}
  virtual void endJob(){ UsedProducts::print();}
 
};




DEFINE_FWK_MODULE(UsedProductsDumper);
