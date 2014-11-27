#include <cmath>

bool isSingleSpike(float e1,float e4e1,float subDet)
{
  float minEtToApplyCleaningEB=10;
  float minEtToApplyCleaningEE=10;
  
  float aEB = 0.04;
  float aEE = 0.02;
  float bEB = -0.024;
  float bEE = -0.0125;
 
  float a = subDet==0 ? aEB : aEE;
  float b = subDet==0 ? bEB : bEE;

  float minEtToApplyCleaning = subDet==0 ? minEtToApplyCleaningEB : minEtToApplyCleaningEE;

  if(e1<minEtToApplyCleaning) return false; //energy below threshold
  
  float e4e1Thres = a*log10(e1)+b;
  if(e4e1<e4e1Thres) return true;// is a spike
  else return false;

}

bool isDoubleSpike(float e1,float e6e2,float subDet)
{
  float minEtToApplyCleaningEB=10;
  float minEtToApplyCleaningEE=99999; //its not normally applied in barrel
  
  float e6e2ThresEB = 0.04; 
  float e6e2ThresEE = 0.04;

  float minEtToApplyCleaning = subDet==0 ? minEtToApplyCleaningEB : minEtToApplyCleaningEE;
  float e6e2Thres = subDet==0 ? e6e2ThresEB : e6e2ThresEE;
  
  if(e1<minEtToApplyCleaning) return false; //energy below threshold
 
  if(e6e2<e6e2Thres) return true;// is a spike
  else return false;

}
