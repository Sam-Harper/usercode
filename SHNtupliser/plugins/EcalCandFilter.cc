
#include "FWCore/Framework/interface/global/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
namespace {
  float getThres(const std::vector<double>& bins,const std::vector<double>& thres,float val)
  {
    const auto res = std::equal_range(bins.begin(),bins.end(),val);
    const size_t thresBinNr = std::distance(bins.begin(),res.second)-1;
    return thresBinNr<thres.size() ? thres[thresBinNr] : std::numeric_limits<float>::max();
  }
}
class EcalCandFilter : public edm::global::EDFilter<> {

private:
  std::vector<double> etThresholds_;
  std::vector<double> etaBinsForEtThresholds_;
  const edm::EDGetTokenT<std::vector<reco::RecoEcalCandidate> > ecalCandsToken_;
public:
  explicit EcalCandFilter(const edm::ParameterSet& para);
  ~EcalCandFilter(){}
  
  virtual bool filter(edm::StreamID id,edm::Event& event,const edm::EventSetup& setup)const override;
  virtual void endJob()override;
private:
  void validateEtThresholds()const;
  

};

EcalCandFilter::EcalCandFilter(const edm::ParameterSet& para):
  etThresholds_(para.getParameter<std::vector<double>>("etThresholds")),
  etaBinsForEtThresholds_(para.getParameter<std::vector<double>>("etaBinsForEtThresholds")),
  ecalCandsToken_(consumes<std::vector<reco::RecoEcalCandidate>>(para.getParameter<edm::InputTag>("ecalCands")))
{
  validateEtThresholds();
}

void EcalCandFilter::validateEtThresholds() const {
  if(etaBinsForEtThresholds_.size()!=etThresholds_.size()+1){
    throw cms::Exception("ConfigError") <<" error etaBinsForEtThresholds (size = "<<etaBinsForEtThresholds_.size()<<") must be equal to etThresholds.size()+1 (size = "<<etThresholds_.size()<<")" <<std::endl;
  }
  for(size_t binNr=0;binNr+1<etaBinsForEtThresholds_.size();binNr++){
    if(etaBinsForEtThresholds_[binNr]>=etaBinsForEtThresholds_[binNr+1]){
      throw cms::Exception("ConfigError") <<" error etBinsForEtThresholds must be set in increasing order bin "<<binNr<<" has value "<<etaBinsForEtThresholds_[binNr]<<" while bin "<<binNr+1 << " has value "<<etaBinsForEtThresholds_[binNr+1]<<std::endl;
    }
  }
}
bool EcalCandFilter::filter(edm::StreamID id,edm::Event& event,const edm::EventSetup& setup)const
{   
  auto ecalCands = event.getHandle(ecalCandsToken_);
  int nrCands = 0;
  //  std::cout <<"start event "<<std::endl;
  for(auto& cand : *ecalCands){
    //    std::cout <<"cand "<<cand.et()<<" eta "<<cand.eta()<<" phi "<<cand.phi()<<" e "<<cand.energy()<<" raw e "<<cand.superCluster()->rawEnergy()<<std::endl;
    const float threshold = getThres(etaBinsForEtThresholds_,etThresholds_,std::abs(cand.eta()));
    if(cand.et()>threshold) nrCands++;
  }
  //std::cout <<"nr cands "<<nrCands<<std::endl;
  return nrCands>=1;
  
}
void EcalCandFilter::endJob()
{ 
  
}  



DEFINE_FWK_MODULE(EcalCandFilter);
