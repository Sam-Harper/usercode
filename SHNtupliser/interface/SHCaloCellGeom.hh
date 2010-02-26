#ifndef SHCALOCELLGEOM
#define SHCALOCELLGEOM

//stores the geometry of the calorimeter cells
//sacrifices size for speed, ie stores eta and phi and a TVector3 which increases size but also saves calculating eta each time
//more of a struct rather than a class...

//incidently this class and its parent is outside the normal version
//control (ie SH v7) because it is read in via a seperate file so the geom 
//can be updated without invalidating previously make ntuples


#include "TVector3.h"

#include <iostream>
#include <cmath>
#include <vector>

class SHCaloCellGeom {
public:
  struct CellEdges{
  public:
    float minEta,maxEta,minPhi,maxPhi;
    CellEdges();
    CellEdges(float minEtaInput,float maxEtaInput,
	      float minPhiInput, float maxPhiInput);
    virtual ~CellEdges(){}
    void fill(float minEtaInput,float maxEtaInput,
	      float minPhiInput, float maxPhiInput);
    void clear(); //sets all variables to -999.
    ClassDef(CellEdges,1)
  };

private:
  float eta_;
  float phi_;
  TVector3 pos_;
  int detId_;
  int towerId_; //usefull to know what CaloTower it is in

  //cell edges, currently optional to fill, in the future will not be
  //possibly made redundant by corners (it definately is redundant now but whether catching eta makes speed sense)
  CellEdges frontEdges_;
  CellEdges rearEdges_;

  std::vector<TVector3> corners_;
  TVector3 rearPos_; //redundant but usefull for speed

  //transiant variables for speed
  mutable float sinTheta_; //!
  mutable int detIdParityCheck_; //! a duplicate of the det id field to ensure that root doesnt change the data under us (ie root doesnt use constructors so I have no way of knowing when root loads a new cell in this memory location and it wont change the transistant data members so by having this I can check for it)
public:
  SHCaloCellGeom();
  SHCaloCellGeom(int detId,const TVector3& pos,int towerId);
  SHCaloCellGeom(int detId,const TVector3& pos,int towerId,const std::vector<TVector3>& corners);
  virtual ~SHCaloCellGeom(){}
  
  //modifiers (necessary as I make the vector in one go and then fill in the cell information
  void setCellGeom(int detId,const TVector3& pos,int towerId);
  void setCellGeom(int detId,const TVector3& pos,int towerId,const std::vector<TVector3>& corners);
 
  //accessors
  const TVector3& pos()const{return pos_;}
  TVector3 pos(float depth)const;
  float eta()const{return eta_;}
  float eta(float depth)const;
  float phi()const{return phi_;}
  float sinTheta()const{checkAndSetSinTheta_();return sinTheta_;}
  int detId()const{return detId_;}
  int towerId()const{return towerId_;}
  const CellEdges& frontEdges()const{return frontEdges_;}
  const CellEdges& rearEdges()const{return rearEdges_;}
  TVector3 cell3Vec()const{return (rearPos_-pos_).Unit();}
  float cellOPTheta()const;
  const std::vector<TVector3>& corners()const{return corners_;}

private:
  void checkAndSetSinTheta_()const{if(detIdParityCheck_!=detId_) {sinTheta_=pos().Pt()/pos().Mag();detIdParityCheck_=detId_;}}
  void setEdgesFromCorners_(const std::vector<TVector3>& corners);
  void setRearPos_(const std::vector<TVector3>& corners);

  ClassDef(SHCaloCellGeom,4)

};

#endif
