#include "SHarper/SHNtupliser/interface/SHL1Event.hh"

ClassImp(SHL1Event)


SHL1Event::SHL1Event():
  l1ClusArray_("SHL1Cluster",12),
  l1CaloTowers_(),
  l1RhoMedian_(0.),
  l1RhoRTMean_(0.),
  l1RhoTLTMean_(0.)
{


}
  
SHL1Event::~SHL1Event()
{
  l1ClusArray_.Delete();
}

void SHL1Event::clear()
{
  SHEvent::clear();
  l1RhoMedian_=0.;
  l1RhoRTMean_=0.;
  l1RhoTLTMean_=0.;
  l1CaloTowers_.clear();
  l1ClusArray_.Delete();
  
}

void SHL1Event::addL1Clus(const SHL1Cluster& clus)
{
  new(l1ClusArray_[nrL1Clus()]) SHL1Cluster(clus);
}

const SHL1Cluster* SHL1Event::getL1Clus(int clusNr)const
{
  SHL1Cluster* clus = (SHL1Cluster*) l1ClusArray_[clusNr];
  return clus;
}


void SHL1Event::copyEventPara(const SHEvent& rhs)
{
  //first check if its really a SHL1Event and if so copy the L1 stuff
  const SHL1Event* rhsL1 = dynamic_cast<const SHL1Event*>(&rhs);
  if(rhsL1){
    copyEventPara(*rhsL1);
  }else{
    //it is an SHEvent, so copy that and zero out our extra data
    l1RhoMedian_=0.;
    l1RhoRTMean_=0.;
    l1RhoTLTMean_=0.;
    
    SHEvent::copyEventPara(rhs);
  }
}

void SHL1Event::copyEventPara(const SHL1Event& rhs)
{
  l1RhoMedian_=rhs.l1RhoMedian_;
  l1RhoRTMean_=rhs.l1RhoRTMean_;
  l1RhoTLTMean_=rhs.l1RhoTLTMean_;
  
  SHEvent::copyEventPara(rhs);

}

void SHL1Event::flushTempData()const
{
  SHEvent::flushTempData();
  l1CaloTowers_.flushIndxTable();
}
