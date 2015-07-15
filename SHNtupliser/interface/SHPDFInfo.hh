#ifndef SHEVENT_SHPDFINFO_HH
#define SHEVENT_SHPDFINFO_HH


#include "Rtypes.h"

#include <utility>


class SHPDFInfo {

private:
  //I thought about making them doubles like CMSSW PdfInfo
  //but decided to stick with floats as that what I use everywhere
  std::pair<int,int> id_;
  std::pair<float,float> x_;
  std::pair<float,float> xPDF_;
  float scalePDF_;

public: 
  SHPDFInfo():scalePDF_(0){}
  //to allow easy filling from CMSSW PdfInfo
  SHPDFInfo(const std::pair<int,int> iId,const std::pair<double,double> iX,
	    const std::pair<double,double>& iXPDF,float iScalePDF);
    
  virtual ~SHPDFInfo(){}

  //think I'll return by value as they are small
  std::pair<int,int> id()const{return id_;}
  std::pair<float,float> x()const{return x_;}
  std::pair<float,float> xPDF()const{return xPDF_;}
  float scalePDF()const{return scalePDF_;}

  ClassDef(SHPDFInfo,1)

};


#endif
