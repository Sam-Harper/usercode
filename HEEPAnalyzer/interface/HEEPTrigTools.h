#ifndef SHARPER_HEEPANALYZER_HEEPTRIGTOOLS
#define SHARPER_HEEPANALYZER_HEEPTRIGTOOLS



#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPTrigCodes.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"

namespace heep {
  namespace trigtools {
    heep::TrigCodes::TrigBitSet getHLTFiltersPassed(const std::vector<std::string>& filters,edm::Handle<trigger::TriggerEvent> trigEvt,const std::string& hltTag);
    //now this could be made a templated function very easily and might be in the future
    void setHLTFiltersObjPasses(std::vector<heep::Ele>& particles,const std::vector<std::string>& filters,edm::Handle<trigger::TriggerEvent> trigEvt,const std::string& hltTag,const double maxDeltaR,const double maxPtDiffRel);
  }
}


#endif
