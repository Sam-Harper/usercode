#include "SHarper/SHNtupliser/interface/SHGeomFiller.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTowerConstituentsMap.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"

#include "SHarper/SHNtupliser/interface/HackedFuncs.h"

#include "TVector3.h"

SHGeomFiller::SHGeomFiller(const edm::EventSetup& setup)
{
  //setup.get<IdealGeometryRecord>().get(calGeometry_);
  //setup.get<IdealGeometryRecord>().get(calTowersConstits_);
  edm::ESHandle<CaloGeometry> calGeometryHandle;
  edm::ESHandle<CaloTowerConstituentsMap> calTowersConstitsHandle;
  setup.get<CaloGeometryRecord>().get(calGeometryHandle);
  setup.get<IdealGeometryRecord>().get(calTowersConstitsHandle);
  
  calGeometry_=calGeometryHandle.product();
  calTowersConstits_=calTowersConstitsHandle.product();
  
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
	ecalGeom.setCellPos(detId.rawId(),pos,calTowersConstits_->towerOf(detId).rawId());
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
	  ecalGeom.setCellPos(detId.rawId(),pos,calTowersConstits_->towerOf(detId).rawId());
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
	if(HackedFuncs::validHcalDetId(HcalBarrel,etaNr,phiNr,depth)){
	  HcalDetId detId(HcalBarrel,etaNr,phiNr,depth);
	  getCellPosition(detId,pos); 
	  SHCaloCellGeom::CellEdges frontEdges,rearEdges;
	  getCellEdges(detId,frontEdges,rearEdges);
	  hcalGeom.setCellPos(detId.rawId(),pos,calTowersConstits_->towerOf(detId).rawId(),frontEdges,rearEdges);
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
	for(int depth=1;depth<=3;depth++){
	  if(HackedFuncs::validHcalDetId(HcalEndcap,iEta,iPhi,depth)){
	    HcalDetId detId(HcalEndcap,iEta,iPhi,depth);
	    TVector3 pos;
	    getCellPosition(detId,pos);
	    SHCaloCellGeom::CellEdges frontEdges,rearEdges;
	    getCellEdges(detId,frontEdges,rearEdges);
	    hcalGeom.setCellPos(detId.rawId(),pos,calTowersConstits_->towerOf(detId).rawId(),frontEdges,rearEdges);
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
  const CaloCellGeometry* cellGeom = subDetGeom!=NULL ? subDetGeom->getGeometry(detId) : NULL;
  if(cellGeom!=NULL){
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
  const CaloCellGeometry* cellGeom = subDetGeom!=NULL ? subDetGeom->getGeometry(detId) : NULL;
  if(cellGeom!=NULL){
    const EZArrayFL<GlobalPoint>& corners = cellGeom->getCorners();
    if(corners[0].eta()==corners[2].eta() || corners[0].phi()==corners[2].phi()) edm::LogInfo("SHGeomFiller") <<"getCellEdges: Warning corner structure has changed, edges will be incorrectly filled";
    frontEdges.fill(corners[2].eta(),corners[0].eta(),corners[2].phi(),corners[0].phi());
    rearEdges.fill(corners[6].eta(),corners[4].eta(),corners[6].phi(),corners[4].phi());
  }else{
    if(detId.rawId()!=0) edm::LogInfo("SHGeomFiller")<<"getCellEdges Warning : Geometry not found for "<<std::hex<<detId.rawId()<<std::dec<<", subDetGeom "<<subDetGeom<<" cellGeom "<<cellGeom;
    frontEdges.clear();
    rearEdges.clear();
  }

}
