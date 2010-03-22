#include "SHarper/HEEPAnalyzer/interface/HEEPAnalyzerBarePAT.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "TH1D.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

HEEPAnalyzerBarePAT::HEEPAnalyzerBarePAT(const edm::ParameterSet& iPara):
  cuts_(iPara),nrPass_(0),nrFail_(0)
{
  eleLabel_=iPara.getParameter<edm::InputTag>("eleLabel");
}

void HEEPAnalyzerBarePAT::beginJob()
{
  edm::Service<TFileService> fs;
  massHist_ = fs->make<TH1D>("massHist","Di-Electron Mass;M_{ee} (GeV/c^{2});# Events / 5 GeV/c^{2}",290,50,1500); 
}

//this analyser shows you how to apply the heep selection to pat electrons straight out of pat
//it will also work if you replace  const edm::View<pat::Electron>& eles  by const reco::GsfElectronCollection& eles
//the energy correction will have no effect if the energy correction is already applied
void HEEPAnalyzerBarePAT::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  edm::Handle<edm::View<pat::Electron> > eleHandle;
  iEvent.getByLabel(eleLabel_,eleHandle);
  const edm::View<pat::Electron>& eles = *(eleHandle.product());

  //do what ever you want
  //count the number that pass / fail cuts
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    if(cuts_.passCuts(eles[eleNr])) nrPass_++;
    else nrFail_++;
  }
  
  //lets fill a mass hist
  for(size_t ele1Nr=0;ele1Nr<eles.size();ele1Nr++){
    for(size_t ele2Nr=ele1Nr+1;ele2Nr<eles.size();ele2Nr++){
      const pat::Electron& ele1 = eles[ele1Nr];
      const pat::Electron& ele2 = eles[ele2Nr];
      int ele1CutCode = cuts_.getCutCode(ele1);
      int ele2CutCode = cuts_.getCutCode(ele2);
     
      if(ele1CutCode==0x0 && ele2CutCode==0x0 && !(ele1.isEE() && ele2.isEE())){ //EB-EB, EE-EE
	math::XYZTLorentzVector ele1P4 = ele1.p4()*ele1.caloEnergy()/ele1.energy();
	math::XYZTLorentzVector ele2P4 = ele2.p4()*ele2.caloEnergy()/ele2.energy();	
	float mass = (ele1P4+ele2P4).mag();
	massHist_->Fill(mass);
      }
    }
  }
  
}


void HEEPAnalyzerBarePAT::endJob()
{
 
  edm::LogInfo("HEEPAnalyzerBarePAT") <<"Nr pass "<<nrPass_<<" nr fail "<<nrFail_<<" eff "<<static_cast<float>(nrPass_)/static_cast<float>(nrFail_+nrPass_);
}

//define this as a plug-in
DEFINE_ANOTHER_FWK_MODULE(HEEPAnalyzerBarePAT);
