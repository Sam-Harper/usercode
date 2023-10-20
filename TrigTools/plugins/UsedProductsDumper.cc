
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"

//#define BUILDUSEDPRODDUMPER
#ifdef BUILDUSEDPRODDUMPER

#include "FWCore/Framework/interface/UsedProducts.h"

class UsedProductsDumper : public edm::one::EDAnalyzer<> {

private:

  
public:
  explicit UsedProductsDumper(const edm::ParameterSet&){}
  ~UsedProductsDumper(){}
  
  virtual void beginJob(){}
  virtual void analyze(const edm::Event& ,const edm::EventSetup& ){}
  virtual void endJob(){ UsedProducts::print();}
 
};



DEFINE_FWK_MODULE(UsedProductsDumper);

#endif
