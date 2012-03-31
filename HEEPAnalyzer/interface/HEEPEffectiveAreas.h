#ifndef SHARPER_HEEPANALYZER_EFFECTIVEAREAS
#define SHARPER_HEEPANALYZER_EFFECTIVEAREAS

//A simple little class to hold the effective areas of the isolation cones needed to apply the rho correction

#include <cmath>

namespace edm{
  class ParameterSet;
}

namespace heep {
  class EffectiveAreas {
  private:
    float trackerBarrel_;
    float trackerEndcap_;
    float ecalBarrel_;
    float ecalEndcap_;
    float hcalBarrel_;
    float hcalEndcap_;
  public:
    EffectiveAreas():
      trackerBarrel_(0.),trackerEndcap_(0.),
      ecalBarrel_(0.),ecalEndcap_(0.),
      hcalBarrel_(0.),hcalEndcap_(0.){}
    explicit EffectiveAreas(const edm::ParameterSet& iConfig);  
    EffectiveAreas& operator=(const edm::ParameterSet& iConfig);
    
    float trackerBarrel()const{return trackerBarrel_;}
    float trackerEndcap()const{return trackerEndcap_;}
    float ecalBarrel()const{return ecalBarrel_;}
    float ecalEndcap()const{return ecalEndcap_;}
    float hcalBarrel()const{return hcalBarrel_;}
    float hcalEndcap()const{return hcalEndcap_;}

    float tracker(float eta)const{return fabs(eta)<1.479 ? trackerBarrel() : trackerEndcap();}
    float ecal(float eta)const{return fabs(eta)<1.479 ? ecalBarrel() : ecalEndcap();}
    float hcal(float eta)const{return fabs(eta)<1.479 ? hcalBarrel() : hcalEndcap();} 
  };
}


#endif
