#include "SHarper/SHNtupliser/interface/MathFuncs.hh"

const double MathFuncs::kPi = 2*asin(1.);
std::vector<double> MathFuncs::_logFacLookUp;
const double MathFuncs::kSqrt2Pi=sqrt(2*kPi);
TF1 MathFuncs::_probFunc("probFunc",MathFuncs::smearedProb,0,1000,4);
TF1 MathFuncs::_gausFunc("gausFunc",MathFuncs::gausFunc,-1000,1000,2);

double MathFuncs::detEtaFromEvnt(double evntEta,double z0)
{
  double zcentroid = 185.4;
  if(evntEta<0) zcentroid*=-1;
  double evntCotTheta = sinh(evntEta);
  double detCotTheta = zcentroid/(zcentroid-z0) * evntCotTheta;
  return asinh(detCotTheta);
}

double MathFuncs::etaToTheta(double eta)
{
  if(eta<0) return -2*atan(exp(eta));
  else return 2*atan(exp(-1*eta));
}

double MathFuncs::thetaToEta(double theta)
{
  //first bounds check theta to get into -pi/2 - pi/2 range
  while( fabs(theta) > MathFuncs::kPi/2.){
    if(theta>0) theta-=MathFuncs::kPi;
    else theta+=MathFuncs::kPi;
  }
  //now check sign
  if(theta<0) return log(tan(fabs(theta/2.)));
  else return -1.*log(tan(theta/2.));
}

double MathFuncs::phi(double px,double py)
{
  if(px!=0) return atan2(py,px); // pass as y , x
  else if(py!=0) return py > 0 ? kPi/2 : -kPi/2;
  else return 0.;
}

//return in range from 0 to 2*pi
double MathFuncs::degreeToRad(double degree)
{
  double rad = degree/180 * kPi;
  return normAngleRange(rad,0,2*kPi);

}

//casts the angle into the range specified adding the range difference to do it
double MathFuncs::normAngleRange(double angle,double minRange,double maxRange)
{
  double normedAngle = angle; 
  double range = maxRange-minRange;
  while (normedAngle<minRange) normedAngle+=range;
  while (normedAngle>maxRange) normedAngle-=range;
  return normedAngle;
}

//range is either 2pi or 360 but anything else could work
double MathFuncs::angleDiff(double angle1,double angle2,double range)
{
  double normedAngle1 = normAngleRange(angle1,0,range);
  double normedAngle2 = normAngleRange(angle2,0,range);
  double diff = normedAngle1 -normedAngle2;
  if(diff>range/2.) diff = range-diff;
  if(diff<-range/2.) diff = -range-diff;


  return diff;
}

bool MathFuncs::isAngleDiffInRadRange(double angleDiff,double minAngle,double maxAngle)
{
  double normedMinAngle = normAngleRange(minAngle,0,2*kPi);
  double normedMaxAngle = normAngleRange(maxAngle,0,2*kPi);
  if(fabs(angleDiff)>normedMinAngle && fabs(angleDiff)<normedMaxAngle) return true;
  else return false;

}

// double MathFuncs::angleDegMinDiff(double angle1,double angle2)
// {
  
//   double diff = angle1-angle2;
//   while (diff<-180) diff+=180;
//   while (diff>180) diff-=180;
//   return diff;
// }

int MathFuncs::getCoordQuadrant(double xCoord,double yCoord)
{
  int quadrantNr=0;
  if(xCoord>0){
    if(yCoord>0) quadrantNr=1; //x=+ve, y=+ve 0 < phi < pi/2
    else quadrantNr = 4; //x=+ve, y=-ve 3/2 < phi < 2*pi
  }else{
    if(yCoord>0) quadrantNr=2; //x=-ve, y=+ve pi/2 <phi< pi
    else quadrantNr=3; //x=-ve, y=-ve pi < phi < 3/2 pi
  }
  return quadrantNr;
} 

//returns phi in range from 0-2pi from the x and y coordinates
double MathFuncs::getAngleFromCoord(double xCoord,double yCoord)
{
  
  int quadrantNr=getCoordQuadrant(xCoord,yCoord);
  
  double angle = atan(yCoord/xCoord);
  // std::cout <<" angle "<<angle<<" x "<<xCoord<<" y "<<yCoord<<" quadrant "<<quadrantNr;
  //now need to convert angle from range of -pi/2 to pi/2 into 0 to 2*pi
  switch(quadrantNr){
  case 1:
    break; //quadrant 1 is already fine
  case 2:
    angle += MathFuncs::kPi;
    break;
  case 3:
    angle += MathFuncs::kPi;
    break;
  case 4:
    angle += 2*MathFuncs::kPi;
    break;
  default:
    angle = 999.;
  }
  // std::cout <<" corr angle "<<angle<<std::endl;

  return angle;
}


//Using the polar form of the Box-Muller transformation to generate gausian values
//Intensionally discarding one of my randomly generated gausian values, inefficent 
//but as as the number of generated events is small can get away with this. 
double MathFuncs::randGaus()
{  
  double x1, x2, w, y1;//, y2;  
  do {
    x1 = 2.0 * (double)rand()/((double)RAND_MAX+1.) - 1.0;
    x2 = 2.0 * (double)rand()/((double)RAND_MAX+1.) - 1.0;
    w = x1 * x1 + x2 * x2;
  } while ( w >= 1.0 );
  
  w = sqrt( (-2.0 * log( w ) ) / w );
  y1 = x1 * w;
  // y2 = x2 * w;
  
  return y1;
}


double MathFuncs::logFactorial(int n)
{
  if(n>((int)_logFacLookUp.size())-1) _fillLogFacLookup(n);
  return _logFacLookUp[n];
}

void MathFuncs::_fillLogFacLookup(int n)
{
  if(_logFacLookUp.size()==0) _logFacLookUp.push_back(0);
  for(unsigned i=_logFacLookUp.size();((int)i)<=n;i++){   
    _logFacLookUp.push_back(_logFacLookUp[i-1] + log((double)i));
  }
}


double MathFuncs::poisson(int n,double expect)
{
 
  double nLogFac = logFactorial(n);
  double logProb = -expect + n*log(expect) - nLogFac;
  return exp(logProb);

  
}

double MathFuncs::intPoissonProb(int minN,int maxN,double expect)
{
  if(expect<=0) return expect;
  double totalProb = 0.;
  for(int i=minN;i<maxN;i++){
    totalProb += poisson(i,expect);
  }
  return totalProb;
}

//para[0] = start value
//para[1] = end (data) value
//para[2] = bkg ground expect
//para[3] = bkg error
double MathFuncs::smearedProb(double *x,double *para)
{
  int paraInt[2];
  paraInt[0] = static_cast<int>(para[0]);
  paraInt[1] = static_cast<int>(para[1]);
  double prob = MathFuncs::intPoissonProb(paraInt[0],paraInt[1],*x)*MathFuncs::gaus(*x,para[2],para[3]);
 // std::cout <<"expect "<<*x<<" mean expect "<<para[2]<<" prob "<<prob<<std::endl;
 return prob;
}

//with errors now
double MathFuncs::intPoissonProb(int minN,int maxN,double expect,double expectErr)
{
  if(expectErr==0) return intPoissonProb(minN,maxN,expect);

  double para[4];
  para[0] = minN;
  para[1] = maxN;
  para[2] = expect;
  para[3] = expectErr;
  //para[3] =0;
  //need to work out the integration limits as roots too dumb to do it
  double minInt = expect-3*expectErr;
  double maxInt = expect+3*expectErr;
  if(minInt<0) minInt=0; //constain to being non-negative

  return _probFunc.Integral(minInt,maxInt,para)/intGaus(expect,expectErr,minInt,maxInt);
}

double MathFuncs::getMedian(std::list<double> &list)
{
  double median=0.;
  std::list<double>::iterator listIt = list.begin();
   if(list.size() % 2 !=0) {
    int middleNr = (list.size()+1)/2;
    for(int i=0;i<middleNr;i++) listIt++;
    median = *listIt;
  }else{ //even number, take median as halfway between the two middle values
    int middleNr = (list.size()+1)/2;
    for(int i=0;i<middleNr;i++) listIt++;
    median= *listIt;
    listIt++;
    median+= *listIt;
    median/=2.;
  }
   return median;
}

double MathFuncs::getMedian(std::vector<double> &vector)
{
  double median=0.;
  
  //odd number, definate median
  if(vector.size() % 2 !=0) {
    int middleNr = (vector.size()+1)/2;
    median = vector[middleNr];
  }else{ //even number, take median as halfway between the two middle values
    int middleNr = (vector.size()+1)/2;
    median= vector[middleNr];
    if(middleNr+1 <(int) vector.size()) median+= vector[middleNr+1];
    median/=2.;
  }
  return median;
}

double MathFuncs::getWeightedListValue(std::list<double> &list,double listNr)
{
  if(listNr>list.size() || listNr<0) return -999;
  else if(listNr==list.size()) return list.back();
  else if(listNr==0) return list.front();

  double tempListNr; //just a tempory value to hold the int part of list nr before it is cast into an interger
  double fracNr = std::modf(listNr,&tempListNr); //decomposing the listNr into frac and int parts
  int intNr = (int) tempListNr;
  
  std::list<double>::iterator listIt = list.begin();
  for(int i=0;i<intNr;i++) listIt++; //incrementing the list pointer to the element we want
  double lowerValue = *listIt;
  listIt++;
  double upperValue = *listIt;

  return lowerValue*(1-fracNr) + upperValue*fracNr;
}

double MathFuncs::getWeightedListValue(std::vector<double> &vector,double index)
{
  if(index>=vector.size() || index<0) return -999;
  else if(index==vector.size()-1) return vector.back();
  else if(index==0) return vector.front();

  double tempIndex; //just a tempory value to hold the int part of the index before it is cast into an interger
  double fracNr = std::modf(index,&tempIndex); //decomposing the index into frac and int parts
  int intNr = (int) tempIndex;
  
  double lowerValue = vector[intNr];
  double upperValue = vector[intNr+1];

  return lowerValue*(1-fracNr) + upperValue*fracNr;
}



double MathFuncs::randNr(double min,double max)
{
  return (double)rand()/(double) RAND_MAX * (max-min) + min;
}

//calcualates the factorial of an integer number
double MathFuncs::factorial(int n)
{
  double result=1.;
  for(int i=0;i<n;i++){
    result *= n-i;
  }
  return result;
}




double MathFuncs::calDeltaR2(double eta1,double phi1, double eta2,double phi2)
{
  double dEta = eta1-eta2;
  double dPhi = angleRadDiff(phi1,phi2);
  return dEta*dEta + dPhi*dPhi;
}


double MathFuncs::intGaus(double mean,double sigma)
{
  return intGaus(mean,sigma,mean-3*sigma,mean+3*sigma);
}

double MathFuncs::intGaus(double mean,double sigma,double min,double max)
{
  double para[2];
  para[0] = mean;
  para[1] = sigma;
  return _gausFunc.Integral(min,max,para);
}

double MathFuncs::power(double x,int n)
{
  double result = 1.;
  for(int i=0;i<n;i++){
    result *=x;
  }
  return result;
}

int MathFuncs::power(int x,int n)
{
  int result =1;
  for(int i=0;i<n;i++){
    result *=x;
  }
  return result;
}

