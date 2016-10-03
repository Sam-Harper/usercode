#include "SHarper/SHNtupliser/interface/SHVertex.hh"

ClassImp(SHVertex)
#include <iostream>
int SHVertex::packData(const bool isValid,const int ndof,const int nrTrks)
{
  int packedData=0;
  packedData |= (isValid&kIsValidMask) << kIsValidOffset;
  packedData |= (ndof&kNDOFMask) <<kNDOFOffset;
  packedData |= (nrTrks&kNrTrksMask) << kNrTrksOffset;
  return packedData;
}
