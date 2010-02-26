#ifndef SHARPER_SHNTUPLISER_MATHFUNCS
#define SHARPER_SHNTUPLISER_MATHFUNCS

//A collection of Mathematical functions
//No function is allowed to depend on anything else in my code
//ie root classes are okay... but not any of my classes
//this may split into stats and normal math funcs
//followers of my code will recognise that all these funcs were in AnaFuncs untill recently

#include "TVector3.h"
#include "TLorentzVector.h"
#include "TF1.h"

#include <vector>
#include <string>
#include <list>
#include <cmath>

class MathFuncs {

private:
  static std::vector<double> _logFacLookUp;
  static TF1 _probFunc;
  static TF1 _gausFunc;


  MathFuncs(){}
  ~MathFuncs(){}
public:
  
  static const double kPi; 
  static const double kSqrt2Pi;


  static double randGaus();
  static double randNr(double min,double max);
  static double factorial(int n);
  static double logFactorial(int n);
  static double power(double x,int n);
  static int power(int x,int n);
  static double poisson(int n,double expect);
  
  //calculate the probability of getting a value from minN (inclusive) to maxN (exclusive) for a given expectation (and error)
  static double intPoissonProb(int minN,int maxN,double expect);
  static double intPoissonProb(int minN,int maxN,double expect,double expectErr); 
  static double smearedProb(double *x,double *para);

  static double detEtaFromEvnt(double evntEta,double z0);

  inline static double gaus(double x,double mean=0.,double sigma=1.){return exp(-.5*(x-mean)*(x-mean)/(sigma*sigma))/(kSqrt2Pi*sigma);}
  inline static double PI(){return kPi;}
  static double intGaus(double mean,double sigma);
  static double intGaus(double mean,double sigma,double min,double max);
  static double gausFunc(double *x,double *para){return gaus(*x,para[0],para[1]);}

  static double etaToTheta(double eta);
  static double thetaToEta(double theta);

  static double phi(double px,double py);

  //return in range from -pi to pi
  static double degreeToRad(double degree);
  static double normAngleRange(double angle,double minRange,double maxRange);
  static double angleDiff(double angle1,double angle2,double range);
  static double angleRadDiff(double angle1,double angle2){return angleDiff(angle1,angle2,2*kPi);}
  static double angleDegDiff(double angle1,double angle2){return angleDiff(angle1,angle2,360);}
  static bool isAngleDiffInRadRange(double angleDiff,double minAngle,double maxAngle);
  static double getAngleFromCoord(double xCoord,double yCoord); //returns angle in range from 0-2pi from the x and y coordinates
  static int getCoordQuadrant(double xCoord,double yCoord); //returns the quadrant the point is in from 1 (0-pi/2) to 4 (3/2pi-2*pi)
  static double calDeltaR(const TLorentzVector& a, const TLorentzVector& b){return sqrt(calDeltaR2(a.Eta(),a.Phi(),b.Eta(),b.Phi()));}
  static double calDeltaR(const TVector3& a, const TVector3& b){return sqrt(calDeltaR2(a.Eta(),a.Phi(),b.Eta(),b.Phi()));}
  static double calDeltaR2(double eta1,double phi1, const TLorentzVector& p4){return calDeltaR2(eta1,phi1,p4.Eta(),p4.Phi());}
  static double calDeltaR2(double eta1,double phi1, const TVector3& p3){return calDeltaR2(eta1,phi1,p3.Eta(),p3.Phi());}
  static double calDeltaR2(double eta1,double phi1, double eta2,double phi2);  static double deltaPhi(double phi1,double phi2){
    double dPhi = phi1-phi2;
    if(dPhi>kPi) dPhi -= 2*kPi;
    if(dPhi<=-kPi) dPhi += 2*kPi;
    return dPhi;
  }
  static double getMedian(std::list<double> &list); //assumes list is sorted
  static double getUpConfidIntValue(std::list<double> &list,double confInt){return getWeightedListValue(list,(1+confInt)*0.5*list.size());}//assumes list is sorted 
  static double getLowConfidIntValue(std::list<double> &list,double confInt){return getWeightedListValue(list,(1-confInt)*0.5*list.size());}//assumes list is sorted
  static double getWeightedListValue(std::list<double> &list,double listNr);
  
  static double getMedian(std::vector<double> &vector); //assumes vector is sorted
  static double getUpConfidIntValue(std::vector<double> &vector,double confInt){return getWeightedListValue(vector,(1+confInt)*0.5*vector.size());}//assumes vector is sorted 
  static double getLowConfidIntValue(std::vector<double> &vector,double confInt){return getWeightedListValue(vector,(1-confInt)*0.5*vector.size());}//assumes vector is sorted
  static double getWeightedListValue(std::vector<double> &vector,double vectorNr);

private:
  static void _fillLogFacLookup(int n);


};

#endif
