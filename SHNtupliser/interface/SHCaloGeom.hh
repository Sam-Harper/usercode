#ifndef SHCALOGEOM
#define SHCALOGEOM

//Stores the geometry of each cell of the ecal or hcal
//each cell is stored at the index which its detId hashes to 
//using the standard hashing functions

#include "SHarper/SHNtupliser/interface/SHCaloCellGeom.hh"
#include "SHarper/SHNtupliser/interface/DetIdTools.hh"

#include "TVector3.h"
#include <vector>

class SHCaloGeom {
public:
  enum detector {ECAL,HCAL,TOWER};
  
  int detCode_; //either Hcal, Ecal or Tower, its for an idiot check to when we pass a det id, we are passing the right 
  std::vector<SHCaloCellGeom> cellGeoms_;
  
  static const SHCaloCellGeom nullCell_;//!

public:
  SHCaloGeom(int detCode=ECAL); //default to ECAL
  virtual ~SHCaloGeom(){}

  //modifier (adds a cell)
  void setCellPos(int detId,const TVector3& pos,int towerId);
  void setCellPos(int detId,const TVector3& pos,int towerId,
		  const SHCaloCellGeom::CellEdges& front,
		  const SHCaloCellGeom::CellEdges& rear);
  //accessors (first 4 look up via detId, last looks up by indx)
  //unless otherwise stated, all lookups are by detId rather than indx
  const TVector3& cellPos(int detId)const{return getCell(detId).pos();}
  double cellEta(int detId)const{return getCell(detId).eta();}
  double cellPhi(int detId)const{return getCell(detId).phi();}
  inline const SHCaloCellGeom& getCell(int detId)const;

  const SHCaloCellGeom& getCellByIndx(int indx)const{return cellGeoms_[indx];}
  int nrCellsStored()const{return cellGeoms_.size();}
 
  //note: this allways returns the cell with a depth of 1, hcal will need to get the cells behind it manually
  const SHCaloCellGeom& getClosestCell(const TVector3& centre)const{return getClosestCell(centre.Eta(),centre.Phi());}
  const SHCaloCellGeom& getClosestCell(double eta,double phi)const;
  //returns a vector with references to all the cells sharing eta/phi (ie the hcal depths)
  //returned by value as it will have max 3 entries
  std::vector<SHCaloCellGeom> getCellsInEtaPhiTower(int detId)const; 
  std::vector<SHCaloCellGeom> getCellsInEtaPhiTower(const TVector3& centre)const{return getCellsInEtaPhiTower(centre.Eta(),centre.Phi());}
  std::vector<SHCaloCellGeom> getCellsInEtaPhiTower(double eta,double phi)const{return getCellsInEtaPhiTower(getClosestCell(eta,phi).detId());}
  

  float phiDistToCellBoundary(const TVector3& centre)const;
  float etaDistToCellBoundary(const TVector3& centre)const;


  ClassDef(SHCaloGeom,1)
  
};

//this really needs to be here for inlining
const SHCaloCellGeom& SHCaloGeom::getCell(int detId)const
{
  int indx=-2;
   if(detCode_==ECAL) indx = DetIdTools::getHashEcal(detId);
   else if(detCode_==HCAL) indx = DetIdTools::getHashHcal(detId);
   if(indx>=0 && indx<(int)cellGeoms_.size())  return cellGeoms_[indx];
   else {
     std::cout <<"SHCaloGeom::getCell : Warning indx "<<indx<<" is invalid, detid "<<std::hex<<detId<<std::dec<<" detCode "<<detCode_<<std::endl;
     return nullCell_;
   }
}


#endif
