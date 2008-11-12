#include "SHarper/SHNtupliser/interface/SHCaloCellGeom.hh"

ClassImp(SHCaloCellGeom)
ClassImp(SHCaloCellGeom::CellEdges)

SHCaloCellGeom::SHCaloCellGeom():
  eta_(-999.),
  phi_(-999.),
  pos_(-999.,-999.,-999.),
  detId_(0),
  towerId_(0),
  frontEdges_(-999.,-999.,-999.,-999.),
  rearEdges_(-999.,-999.,-999.,-999.)
{

}

SHCaloCellGeom::SHCaloCellGeom(int detId,const TVector3& pos,int towerId):
  eta_(pos.Eta()),
  phi_(pos.Phi()),
  pos_(pos),
  detId_(detId),  
  towerId_(towerId),
  frontEdges_(-999.,-999.,-999.,-999.),
  rearEdges_(-999.,-999.,-999.,-999.)
{


}

SHCaloCellGeom::SHCaloCellGeom(int detId,const TVector3& pos,int towerId,
			       const CellEdges& front,const CellEdges& rear):
  eta_(pos.Eta()),
  phi_(pos.Phi()),
  pos_(pos),
  detId_(detId),   
  towerId_(towerId),
  frontEdges_(front),
  rearEdges_(rear)
{


}

void SHCaloCellGeom::setCellGeom(int detId,const TVector3& pos,int towerId)
{
  eta_=pos.Eta();
  phi_=pos.Phi();
  pos_=pos;
  detId_=detId;
  towerId_=towerId;
  frontEdges_.clear(); //I want to ensure that this struct is set into a null state
  rearEdges_.clear();
}

void SHCaloCellGeom::setCellGeom(int detId,const TVector3& pos,int towerId,const CellEdges& front,const CellEdges& rear)
{
  eta_=pos.Eta();
  phi_=pos.Phi();
  pos_=pos;
  detId_=detId;
  towerId_=towerId;
  frontEdges_=front;
  rearEdges_=rear;
}

SHCaloCellGeom::CellEdges::CellEdges():
  minEta(-999.),maxEta(-999.),
  minPhi(-999.),maxPhi(-999.)
{

}

SHCaloCellGeom::CellEdges::CellEdges(float minEtaInput,float maxEtaInput,
				     float minPhiInput,float maxPhiInput):
  minEta(minEtaInput),maxEta(maxEtaInput),
  minPhi(minPhiInput),maxPhi(maxPhiInput)
{

}

void SHCaloCellGeom::CellEdges::fill(float minEtaInput,float maxEtaInput,
				     float minPhiInput,float maxPhiInput)
{
  minEta = minEtaInput;
  maxEta = maxEtaInput;
  minPhi = minPhiInput;
  maxPhi = maxPhiInput;
  if(minEta>maxEta){
    std::cout<<" SHCaloCellGeom::CellEdges::fill : Warning minEta  is greater than maxEta  : minEta "<<minEta<<" maxEta "<<maxEta<<" minPhi "<<minPhi<<" maxPhi "<<maxPhi<<std::endl;
  }
}


void SHCaloCellGeom::CellEdges::clear()
{
  minEta=-999.;
  maxEta=-999.;
  minPhi=-999.;
  maxPhi=-999.;
}
