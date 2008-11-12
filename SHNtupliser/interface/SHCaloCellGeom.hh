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

class SHCaloCellGeom {
public:
  struct CellEdges{
  public:
    float minEta,maxEta,minPhi,maxPhi;
    CellEdges();
    CellEdges(float minEtaInput,float maxEtaInput,
	      float minPhiInput, float maxPhiInput);
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
  CellEdges frontEdges_;
  CellEdges rearEdges_;


public:
  SHCaloCellGeom();
  SHCaloCellGeom(int detId,const TVector3& pos,int towerId);
  SHCaloCellGeom(int detId,const TVector3& pos,int towerId,const CellEdges& front,const CellEdges& rear);
  virtual ~SHCaloCellGeom(){}
  
  //modifiers (necessary as I make the vector in one go and then fill in the cell information
  void setCellGeom(int detId,const TVector3& pos,int towerId);
  void setCellGeom(int detId,const TVector3& pos,int towerId,const CellEdges& front,const CellEdges& rear);
  //accessors
  const TVector3& pos()const{return pos_;}
  float eta()const{return eta_;}
  float phi()const{return phi_;}
  float sinTheta()const{return pos().Pt()/pos().Mag();}
  int detId()const{return detId_;}
  int towerId()const{return towerId_;}
  const CellEdges& frontEdges()const{return frontEdges_;}
  const CellEdges& rearEdges()const{return rearEdges_;}

  ClassDef(SHCaloCellGeom,3)

};

#endif
