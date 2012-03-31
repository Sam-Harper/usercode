#include "SHarper/HEEPAnalyzer/interface/HEEPEffectiveAreas.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

heep::EffectiveAreas::EffectiveAreas(const edm::ParameterSet& iConfig)
{
  trackerBarrel_ = iConfig.getParameter<double>("trackerBarrel");
  trackerEndcap_ = iConfig.getParameter<double>("trackerEndcap");
  ecalBarrel_ = iConfig.getParameter<double>("ecalBarrel");
  ecalEndcap_ = iConfig.getParameter<double>("ecalEndcap");
  hcalBarrel_ = iConfig.getParameter<double>("hcalBarrel");
  hcalEndcap_ = iConfig.getParameter<double>("hcalEndcap");
  
}

heep::EffectiveAreas& heep::EffectiveAreas::operator=(const edm::ParameterSet& iConfig)
{
  trackerBarrel_ = iConfig.getParameter<double>("trackerBarrel");
  trackerEndcap_ = iConfig.getParameter<double>("trackerEndcap");
  ecalBarrel_ = iConfig.getParameter<double>("ecalBarrel");
  ecalEndcap_ = iConfig.getParameter<double>("ecalEndcap");
  hcalBarrel_ = iConfig.getParameter<double>("hcalBarrel");
  hcalEndcap_ = iConfig.getParameter<double>("hcalEndcap");
  
  return *this;


}
