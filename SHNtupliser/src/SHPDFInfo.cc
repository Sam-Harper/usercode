#include "SHarper/SHNtupliser/interface/SHPDFInfo.hh"

ClassImp(SHPDFInfo)

SHPDFInfo::SHPDFInfo(const std::pair<int,int> iId,const std::pair<double,double> iX,
		     const std::pair<double,double>& iXPDF,float iScalePDF):
  id_(iId),x_(iX.first,iX.second),xPDF_(iXPDF.first,iXPDF.second),
  scalePDF_(iScalePDF)
{



}
