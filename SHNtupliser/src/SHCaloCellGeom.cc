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
  rearEdges_(-999.,-999.,-999.,-999.), 
  corners_(8),
  rearPos_(-999.,-999.,-999.),
  sinTheta_(999.),
  detIdParityCheck_(0)
{
  checkAndSetSinTheta_();
}

SHCaloCellGeom::SHCaloCellGeom(int detId,const TVector3& pos,int towerId):
  eta_(pos.Eta()),
  phi_(pos.Phi()),
  pos_(pos),
  detId_(detId),  
  towerId_(towerId),
  frontEdges_(-999.,-999.,-999.,-999.),
  rearEdges_(-999.,-999.,-999.,-999.), 
  corners_(8),
  rearPos_(-999.,-999.,-999.),
  sinTheta_(999.),
  detIdParityCheck_(0)
{
  checkAndSetSinTheta_();

}

SHCaloCellGeom::SHCaloCellGeom(int detId,const TVector3& pos,int towerId,
			       const std::vector<TVector3>& corners):
  eta_(pos.Eta()),
  phi_(pos.Phi()),
  pos_(pos),
  detId_(detId),   
  towerId_(towerId),
  frontEdges_(-999.,-999.,-999.,-999.), //reset in function below
  rearEdges_(-999.,-999.,-999.,-999.),//reset in function below
  corners_(corners), 
  rearPos_(-999.,-999.,-999.), //reset in function below
  sinTheta_(999.),
  detIdParityCheck_(0)
{
  setEdgesFromCorners_(corners);
  setRearPos_(corners);
  checkAndSetSinTheta_();

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
  corners_.clear();
  corners_.resize(8);
  rearPos_.SetXYZ(-999.,-999.,-999.),
  checkAndSetSinTheta_();
}

void SHCaloCellGeom::setCellGeom(int detId,const TVector3& pos,int towerId,const std::vector<TVector3>& corners)
{
  eta_=pos.Eta();
  phi_=pos.Phi();
  pos_=pos;
  detId_=detId;
  towerId_=towerId;
  corners_ = corners;
  setEdgesFromCorners_(corners);
  setRearPos_(corners);
  checkAndSetSinTheta_();
}

float SHCaloCellGeom::eta(float depth)const
{
  TVector3 depthPos = pos_ + (rearPos_-pos_).Unit()*depth;
  return depthPos.Eta();
}

TVector3 SHCaloCellGeom::pos(float depth)const
{
  TVector3 depthPos = pos_ + (rearPos_-pos_).Unit()*depth;
  return depthPos;

}

float SHCaloCellGeom::cellOPTheta()const
{
  double posT = pos_.Pt();
  double rearT = rearPos_.Pt();
  
  return atan((rearT-posT)/(rearPos_.Z()-pos_.Z()));
}



void SHCaloCellGeom::setEdgesFromCorners_(const std::vector<TVector3>& corners)
{
  float minEtaFront = std::min(corners[0].Eta(),corners[2].Eta());
  float maxEtaFront = std::max(corners[0].Eta(),corners[2].Eta());
  float minPhiFront = std::min(corners[0].Phi(),corners[2].Phi());
  float maxPhiFront = std::max(corners[0].Phi(),corners[2].Phi());
  
  float minEtaRear = std::min(corners[4].Eta(),corners[6].Eta());
  float maxEtaRear = std::max(corners[4].Eta(),corners[6].Eta());
  float minPhiRear = std::min(corners[4].Phi(),corners[6].Phi());
  float maxPhiRear = std::max(corners[4].Phi(),corners[6].Phi());
  
  frontEdges_.fill(minEtaFront,maxEtaFront,minPhiFront,maxPhiFront);
  rearEdges_.fill(minEtaRear,maxEtaRear,minPhiRear,maxPhiRear);
}

void SHCaloCellGeom::setRearPos_(const std::vector<TVector3>& corners)
{
  rearPos_ = corners[4]+corners[5]+corners[6]+corners[7];
  rearPos_ *= .25;
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
