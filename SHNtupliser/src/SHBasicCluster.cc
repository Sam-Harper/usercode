#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
  
#include "SHarper/SHNtupliser/interface/LogErr.hh"
#include <algorithm>
ClassImp(SHBasicCluster)

SHBasicCluster::SHBasicCluster():
  totNrgy_(0.),
  nrCrys_(0),
  eMaxId_(0),
  e2ndId_(0),
  eta_(0.),
  phi_(0.),
  hitDetIds_(),
  seedId_(0),
  hitInfo_(0)
{

}


SHBasicCluster::SHBasicCluster(const SHBasicCluster& rhs):
  totNrgy_(rhs.totNrgy_),
  nrCrys_(rhs.nrCrys_),
  eMaxId_(rhs.eMaxId_),
  e2ndId_(rhs.e2ndId_),
  eta_(rhs.eta_),
  phi_(rhs.phi_),
  hitDetIds_(), //intensionally not copying, we can remake it later if required
  seedId_(rhs.seedId_),
  hitInfo_(rhs.hitInfo_)
{
  
}

SHBasicCluster::SHBasicCluster(float nrgy,int seedId,int e2ndId,float eta,float phi,const std::vector<int>& hits):
  totNrgy_(nrgy),
  nrCrys_(hits.size()),
  eMaxId_(0),
  e2ndId_(e2ndId),
  eta_(eta),
  phi_(phi),
  hitDetIds_(),
  seedId_(seedId)
{
  hitInfo_ = packHits_(seedId_,hits);

}


int SHBasicCluster::packHits_(int seedId,const std::vector<int>& hits)
{
  if(DetIdTools::isEcalBarrel(seedId)){
    int maxIPhiPos=0;
    int maxIPhiNeg=0;
    int seedIPhi = DetIdTools::iPhiBarrel(seedId);
    for(size_t hitNr=0;hitNr<hits.size();hitNr++){
      int hitIPhi = DetIdTools::iPhiBarrel(hits[hitNr]);
      int dIPhi = hitIPhi-seedIPhi;
      while(dIPhi<=-180) dIPhi+=360;
      while(dIPhi>180) dIPhi-=360;
      
      if(dIPhi>0 && dIPhi>maxIPhiPos) maxIPhiPos=dIPhi;
      else if(dIPhi<0 && dIPhi*-1>maxIPhiNeg) maxIPhiNeg=dIPhi*-1;
      
    }
    int nrNegSteps=maxIPhiNeg;
    int nrPosSteps=maxIPhiPos;

    if(nrNegSteps<0 || nrNegSteps>17 || nrPosSteps<0 || nrPosSteps>17){
      LogErr <<" warning phi steps out of bounds: neg "<<nrNegSteps << " pos "<<nrPosSteps<<" seed "<<seedIPhi<<std::endl;
    }
    
    int packedHits=0x0;
    packedHits |= nrNegSteps<<1;
    packedHits |= nrPosSteps<<6;
    return packedHits;
  }else if(DetIdTools::isEcalEndcap(seedId)){
    int packedHits=0x1;
    
    DetIdTools::EcalNavigator nav(seedId);
    int bitNr=1;
    for(int iY=-2;iY<=2;iY++){
      for(int iX=-2;iX<=2;iX++){
	int hitId = nav.getIdAtPos(iX,iY);
	if(hitId!=0 && std::find(hits.begin(),hits.end(),hitId)!=hits.end()){
	  packedHits |= (0x1<<bitNr);
	  
	}
	bitNr++;
      }
    }
    return packedHits;

  }else return 0;

}

bool SHBasicCluster::unpackHits_(bool forceUnpack)const
{
  if(!hitDetIds_.empty() && !forceUnpack) return false;
  
  hitDetIds_.clear();
  hitDetIds_.reserve(nrCrys_);

  if((hitInfo_&0x1)==0){ //barrel
    int nrNegSteps = (hitInfo_ >> 1) &0x1F;
    int nrPosSteps = (hitInfo_ >> 6) &0x1F;
    
    DetIdTools::EcalNavigator nav(seedId_);
    for(int phiNr=nrNegSteps;phiNr<=nrPosSteps;phiNr++){
      for(int etaNr=-2;etaNr<=2;etaNr++){
	hitDetIds_.push_back(nav.getIdAtPos(etaNr,phiNr));
      }
    }
  }else { //endcap
    DetIdTools::EcalNavigator nav(seedId_); 
    int bitNr=1;
    for(int ix=-2;ix<=2;ix++){
      for(int iy=-2;iy<=2;iy++){
	if((hitInfo_&(0x1<<bitNr))==1){
	  hitDetIds_.push_back(nav.getIdAtPos(ix,iy));
	}
	bitNr++;
      }
    }
  }
  return true;
}
