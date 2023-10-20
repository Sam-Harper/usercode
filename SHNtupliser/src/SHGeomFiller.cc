#include "SHarper/SHNtupliser/interface/SHGeomFiller.h"


#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/Transition.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

#include "TVector3.h"

SHGeomFiller::SHGeomFiller(edm::ConsumesCollector&& cc):
  calGeomToken_(cc.esConsumes<edm::Transition::BeginRun>()),
  calTowersConstitsToken_(cc.esConsumes<edm::Transition::BeginRun>())
{ 
  
}

void SHGeomFiller::initRun(const edm::EventSetup& setup)
{
  calGeometry_= setup.getHandle(calGeomToken_);
  calTowersConstits_= setup.getHandle(calTowersConstitsToken_);  
}

void SHGeomFiller::fillEcalGeom(SHCaloGeom& ecalGeom)
{
  fillEcalGeomBarrel(ecalGeom);
  fillEcalGeomEndcap(ecalGeom);
}

void SHGeomFiller::fillHcalGeom(SHCaloGeom& hcalGeom)
{
  fillHcalGeomBarrel(hcalGeom);
  fillHcalGeomEndcap(hcalGeom);
}

void SHGeomFiller::fillEcalGeomBarrel(SHCaloGeom& ecalGeom)
{
  TVector3 pos;
   for(int etaNr=-85;etaNr<=85;etaNr++){
    for(int phiNr=1;phiNr<=360;phiNr++){
      if(EBDetId::validDetId(etaNr,phiNr)){
	EBDetId detId(etaNr,phiNr);
	getCellPosition(detId,pos);
	std::vector<TVector3> corners;
	getCellCorners(detId,corners);
	ecalGeom.setCellPos(detId.rawId(),pos,calTowersConstits_->towerOf(detId).rawId(),corners);
      }
    }//end phi loop
  }//end eta loop
}

void SHGeomFiller::fillEcalGeomEndcap(SHCaloGeom& ecalGeom)
{
  TVector3 pos;
  for(int xNr=1;xNr<=100;xNr++){
    for(int yNr=1;yNr<=100;yNr++){
      for(int zSide=-1;zSide<=1;zSide+=2){
	if(EEDetId::validDetId(xNr,yNr,zSide)){
	  EEDetId detId(xNr,yNr,zSide);
	  getCellPosition(detId,pos);   
	  std::vector<TVector3> corners;
	  getCellCorners(detId,corners);
	  ecalGeom.setCellPos(detId.rawId(),pos,calTowersConstits_->towerOf(detId).rawId(),corners);
	  //std::cout <<"EEHash x "<<xNr<<" y "<<yNr<<" zSide "<<zSide<<" hash "<<detId.hashedIndex()<<std::endl;
	}
      }//z side loop
    }//end phi loop
  }//end eta loop
}

void SHGeomFiller::fillHcalGeomBarrel(SHCaloGeom& hcalGeom)
{
  TVector3 pos;
  for(int etaNr=-16;etaNr<=16;etaNr++){
    for(int phiNr=1;phiNr<=72;phiNr++){
      for(int depth =1;depth<=2;depth++){
	if(DetIdTools::isValidHcalBarrelId(etaNr,phiNr,depth)){
	  HcalDetId detId(HcalBarrel,etaNr,phiNr,depth);
	  getCellPosition(detId,pos); 
	  std::vector<TVector3> corners;
	  getCellCorners(detId,corners);
	  hcalGeom.setCellPos(detId.rawId(),pos,calTowersConstits_->towerOf(detId).rawId(),corners);
	}
      }//end depth
    }//end phi loop
  }//end eta loop
  
}

void SHGeomFiller::fillHcalGeomEndcap(SHCaloGeom& hcalGeom)
{
  TVector3 pos;
  for(int iEtaAbs=16;iEtaAbs<=29;iEtaAbs++){
    for(int side=0;side<=1;side++){
      int iEta = iEtaAbs*(2*side-1);
      for(int iPhi=1;iPhi<=72;iPhi++){
	for(int depth=1;depth<=8;depth++){
	  if(DetIdTools::isValidHcalEndcapId(iEta,iPhi,depth)){
	    HcalDetId detId(HcalEndcap,iEta,iPhi,depth);
	    TVector3 pos;
	    getCellPosition(detId,pos); 
	    std::vector<TVector3> corners;
	    getCellCorners(detId,corners);
	    hcalGeom.setCellPos(detId.rawId(),pos,calTowersConstits_->towerOf(detId).rawId(),corners);
	  }
	}//end depth loop
      }//end iphi loop
    }//end side loop
  }//end iEtaAbs loop
}


//note the reason this converts a GlobalPoint to TVector3 is that my framework into which this is going doesnt
//understand what a GlobalPoint is
void SHGeomFiller::getCellPosition(const DetId &detId,TVector3 &pos)
{
  const CaloSubdetectorGeometry* subDetGeom =  calGeometry_->getSubdetectorGeometry(detId);
  std::shared_ptr<const CaloCellGeometry> cellGeom = subDetGeom!=nullptr ? subDetGeom->getGeometry(detId) : std::shared_ptr<const CaloCellGeometry>();
  if(cellGeom!=nullptr){
    const GlobalPoint &gpPos =cellGeom->getPosition();
    pos.SetXYZ(gpPos.x(),gpPos.y(),gpPos.z());
  }else{
    if(detId.rawId()!=0) edm::LogInfo("SHGeomFiller") <<"getCellPosition Warning : Geometry not found for "<<std::hex<<detId.rawId()<<std::dec<<", subDetGeom "<<subDetGeom<<" cellGeom "<<cellGeom;
    pos.SetXYZ(0.,0.,0.);
  }
}

//this is not guaranteed to work for every geometry as I havnt fully thought it through but it should be fine for the HB,HE,HO
void SHGeomFiller::getCellEdges(const DetId& detId,SHCaloCellGeom::CellEdges& frontEdges,SHCaloCellGeom::CellEdges& rearEdges)
{
  const CaloSubdetectorGeometry* subDetGeom =  calGeometry_->getSubdetectorGeometry(detId);
  std::shared_ptr<const CaloCellGeometry> cellGeom = subDetGeom!=nullptr ? subDetGeom->getGeometry(detId) : std::shared_ptr<const CaloCellGeometry>();
  if(cellGeom!=nullptr){
    const EZArrayFL<GlobalPoint>& corners = cellGeom->getCorners();
    if(corners[0].eta()==corners[2].eta() || corners[0].phi()==corners[2].phi()) edm::LogInfo("SHGeomFiller") <<"getCellEdges: Warning corner structure has changed, edges will be incorrectly filled";
    //std::cout <<"nr corners "<<corners.size()<<" det id "<<detId.det()<<" sub det "<<detId.subdetId()<<std::endl;
    // for(size_t i=0;i<corners.size();i++) std::cout <<"corner "<<i<<" eta "<<corners[i].eta()<<" phi "<<corners[i].phi()<<std::endl;
    
    float minEtaFront = std::min(corners[0].eta(),corners[2].eta());
    float maxEtaFront = std::max(corners[0].eta(),corners[2].eta());
    float minPhiFront = std::min(corners[0].phi(),corners[2].phi());
    float maxPhiFront = std::max(corners[0].phi(),corners[2].phi());
    
    float minEtaRear = std::min(corners[4].eta(),corners[6].eta());
    float maxEtaRear = std::max(corners[4].eta(),corners[6].eta());
    float minPhiRear = std::min(corners[4].phi(),corners[6].phi());
    float maxPhiRear = std::max(corners[4].phi(),corners[6].phi());

    frontEdges.fill(minEtaFront,maxEtaFront,minPhiFront,maxPhiFront);
    rearEdges.fill(minEtaRear,maxEtaRear,minPhiRear,maxPhiRear);
  }else{
    LogErr <<"null geom for id "<<detId.det()<<" "<<detId.subdetId()<<" detId "<<detId.rawId()<<std::endl;
    if(detId.rawId()!=0) edm::LogInfo("SHGeomFiller")<<"getCellEdges Warning : Geometry not found for "<<std::hex<<detId.rawId()<<std::dec<<", subDetGeom "<<subDetGeom<<" cellGeom "<<cellGeom;
    frontEdges.clear();
    rearEdges.clear();
  }

}


void SHGeomFiller::getCellCorners(const DetId& detId,std::vector<TVector3>& cornerVec)
{
  const CaloSubdetectorGeometry* subDetGeom =  calGeometry_->getSubdetectorGeometry(detId);
  std::shared_ptr<const CaloCellGeometry> cellGeom = subDetGeom!=nullptr ? subDetGeom->getGeometry(detId) : std::shared_ptr<const CaloCellGeometry>();
  if(subDetGeom==nullptr) LogErr<<" null sub det geom for id "<<detId.det()<<" "<<detId.subdetId()<<" detId "<<detId.rawId()<<std::endl;
  if(cellGeom!=nullptr){
    const EZArrayFL<GlobalPoint>& cmsswCorners = cellGeom->getCorners();
    
    if(cmsswCorners.size()!=8) std::cout <<"SHGeomFiller: getCornerVec : Error corner vec has "<<cmsswCorners.size()<<" corners, not 8 "<<std::endl;
    cornerVec.clear();
    cornerVec.resize(cmsswCorners.size());
    for(size_t cornerNr=0;cornerNr<cmsswCorners.size();cornerNr++){
      cornerVec[cornerNr].SetXYZ(cmsswCorners[cornerNr].x(),cmsswCorners[cornerNr].y(),cmsswCorners[cornerNr].z());
    }
  }else{
    LogErr <<"null geom for id "<<detId.det()<<" "<<detId.subdetId()<<" detId "<<detId.rawId();
    if(detId.det()==DetId::Hcal){
      std::cout <<" "<<HcalDetId(detId)<<std::endl;
    }
    if(detId.rawId()!=0) edm::LogInfo("SHGeomFiller")<<"getCornerVec Warning : Geometry not found for "<<std::hex<<detId.rawId()<<std::dec<<", subDetGeom "<<subDetGeom<<" cellGeom "<<cellGeom;
    cornerVec.clear();
    cornerVec.resize(8);
  }

}
