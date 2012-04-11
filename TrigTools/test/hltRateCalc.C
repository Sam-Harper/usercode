#include "TBits.h"
#include "TList.h"
#include "TObjString.h"
#include "TTree.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
void convertTListToASaneContainerFormat(TList* listOfPathNames,std::vector<std::string>& pathNames);
void getPreScales(const std::vector<std::string>& pathNames,const std::string& preScaleFilename,
		  std::vector<std::pair<std::string,int> >& pathNamesAndPreScales,bool removeVersions=true);
bool analyseTrigResult(const TBits& bits,std::vector<int>& trigCounts,std::vector<std::vector<int> >& trigOverlaps);
void applyPrescales(TBits& bits,const std::vector<std::pair<std::string,int> >& pathNamesAndPS,const int entryNr);
void postProcess(const std::vector<std::pair<std::string,int> >& pathNamesAndPS,const std::vector<int>& trigCounts,const std::vector<std::vector<int> >& trigOverlaps,float rateSF=1);

template <class T1,class T2,typename Comp=std::less<T1> > struct PairSortByFirst { 
  Comp comp;
  bool operator()(const std::pair<T1,T2>& lhs,const std::pair<T1,T2>&rhs){return comp(lhs.first,rhs.first);}
  bool operator()(const T1& lhs,const std::pair<T1,T2>&rhs){return comp(lhs,rhs.first);}
  bool operator()(const std::pair<T1,T2>& lhs,const T1 &rhs){return comp(lhs.first,rhs);}
  bool operator()(const T1& lhs,const T1 &rhs){return comp(lhs,rhs);}
};
template <class T1,class T2,typename Comp=std::less<T2> > struct PairSortBySecond {
  Comp comp;
  bool operator()(const std::pair<T1,T2>& lhs,const std::pair<T1,T2>&rhs){return comp(lhs.second,rhs.second);}
  bool operator()(const T2& lhs,const std::pair<T1,T2>&rhs){return comp(lhs,rhs.second);}
  bool operator()(const std::pair<T1,T2>& lhs,const T2 &rhs){return comp(lhs.second,rhs);}
  bool operator()(const T2& lhs,const T2 &rhs){return comp(lhs,rhs);}
};

struct TrigRateResults {  
  std::vector<std::pair<std::string,int> > pathNamesAndPreScales;
  std::vector<int> trigCounts;
  std::vector<std::vector<int> > trigOverlaps;

  size_t getTrigIndex(const std::string& pathName);
 
  void printOverlaps(size_t index);  
  void printOverlaps(const std::string& pathName){printOverlaps(getTrigIndex(pathName));}
  void printTopOverlaps(size_t index,size_t nrToPrint=10);
  void printTopOverlaps(const std::string& pathName,size_t nrToPrint=10){printTopOverlaps(getTrigIndex(pathName),nrToPrint);}
};
TrigRateResults calRate(TTree* tree,const std::string& preScaleFilename,float rateSF=1.);

#include "HistFuncs.hh"
TrigRateResults calRate(const std::string& filename, const std::string& preScaleFilename,float rateSF=1.)
{
  TTree* tree = HistFuncs::makeChain("trigResultTree",filename.c_str());
  return calRate(tree,preScaleFilename,rateSF);
}

void removeHLTVersion(std::string& hltName)
{
  size_t lastUnderScore = hltName.find_last_of("_");
  if(lastUnderScore!=std::string::npos) hltName.erase(lastUnderScore); 
}

TrigRateResults calRate(TTree* tree,const std::string& preScaleFilename,float rateSF)
{
  bool removeVersions=true;

  TBits* trigBits = new TBits;
  tree->SetBranchAddress("trigBits",&trigBits);  
 
  std::vector<std::string> pathNames;
  if(tree->GetEntries()!=0){ //little hack to get access to the user info
    tree->LoadTree(0); 
    TList* userInfo = tree->GetTree()->GetUserInfo();
    TIter userInfoIter(userInfo);
    convertTListToASaneContainerFormat(static_cast<TList*>(userInfoIter()),pathNames);
  }
  if(removeVersions) std::for_each(pathNames.begin(),pathNames.end(),removeHLTVersion);
  std::cout <<"there are "<<pathNames.size()<<" possible paths defined "<<std::endl;
  std::cout <<"getting prescales "<<std::endl;
  std::vector<std::pair<std::string,int> > pathNamesAndPreScales;
  getPreScales(pathNames,preScaleFilename,pathNamesAndPreScales,removeVersions); //the idea is that these are additional prescales added into 
  std::cout <<"got prescales "<< pathNamesAndPreScales.size()<<std::endl;
  std::vector<int> trigCounts(pathNamesAndPreScales.size(),0); //I almost folded this into overlaps but its just easier and safer to have it seperate
  std::vector<std::vector<int> > trigOverlaps(pathNamesAndPreScales.size());
  for(size_t pathNr=0;pathNr<pathNamesAndPreScales.size();pathNr++){
    std::vector<int> tempVec(pathNamesAndPreScales.size(),0);
    trigOverlaps[pathNr].swap(tempVec);
  }

  
  int nrEntries = tree->GetEntries();
  int nrPass=0;
 
  for(int entryNr=0;entryNr<nrEntries;entryNr++){
    tree->GetEntry(entryNr);    
    applyPrescales(*trigBits,pathNamesAndPreScales,entryNr);    
    if(analyseTrigResult(*trigBits,trigCounts,trigOverlaps)) nrPass++;
  }
 
  postProcess(pathNamesAndPreScales,trigCounts,trigOverlaps,rateSF);
   
  std::cout <<"total number passing "<<nrPass*rateSF<<std::endl; 
 
  delete trigBits;

  TrigRateResults results;
  results.pathNamesAndPreScales.swap(pathNamesAndPreScales);
  results.trigCounts.swap(trigCounts);
  results.trigOverlaps.swap(trigOverlaps);
  return results;
}

//I admit, I had to go and have a shower after writing this function as I suddenly felt unclean...
void convertTListToASaneContainerFormat(TList* listOfPathNames,std::vector<std::string>& pathNames)
{
  TIter listIt(listOfPathNames);
  for(TObjString* strPointer = dynamic_cast<TObjString*>(listIt.Next());
      strPointer!=0;strPointer= dynamic_cast<TObjString*>(listIt.Next())){
    std::string pathName(strPointer->String().Data()); 
    pathNames.push_back(pathName);
  }
}

void getPreScales(const std::vector<std::string>& pathNames,const std::string& preScaleFilename,
		  std::vector<std::pair<std::string,int> >& pathNamesAndPreScales,bool removeVersions)
{
  std::vector<std::pair<std::string,int> > tempNamesAndPS;
  
  if(!preScaleFilename.empty()){
    std::ifstream file(preScaleFilename.c_str());
    if(file.bad()){
      std::cout <<"file: "<<preScaleFilename<<" not found "<<std::endl;
    } else if(file.eof()){
      std::cout <<"file: "<<preScaleFilename<<" found but empty "<<std::endl;
    }

    if(!file.bad()){
      while(!file.eof()){
	
	char tempBuffer[1024];
	file.getline(tempBuffer,1024);
       	std::string path(strtok(tempBuffer," "));
	if(removeVersions) removeHLTVersion(path);
	std::stringstream preScaleStr;
       	preScaleStr<<strtok(NULL," ");
	int preScale;
	preScaleStr>>preScale;
	
	tempNamesAndPS.push_back(std::make_pair<std::string,int>(path,preScale));
      }
    }
  }
  
  std::sort(tempNamesAndPS.begin(),tempNamesAndPS.end(),PairSortByFirst<std::string,int>());

  for(size_t pathNr=0;pathNr<pathNames.size();pathNr++){
    int preScale=0;
    typedef std::vector<std::pair<std::string,int> >::const_iterator ConstIt;
    std::pair<ConstIt,ConstIt> result = std::equal_range(tempNamesAndPS.begin(),tempNamesAndPS.end(),
							 pathNames[pathNr],PairSortByFirst<std::string,int>());
    
    if(result.first!=result.second){ //found it
      if(result.first-result.second>1) std::cout <<"Warning multiple entries found for "<<pathNames[pathNr]<<std::endl;
      preScale=result.first->second;
    }
    pathNamesAndPreScales.push_back(std::make_pair(pathNames[pathNr],preScale));
  }
  
}
bool analyseTrigResult(const TBits& bits,std::vector<int>& trigCounts,std::vector<std::vector<int> >& trigOverlaps)
{
  bool anyPathPasses=false;
  for(size_t pathNr=0;pathNr<trigOverlaps.size();pathNr++){
    if(bits.TestBitNumber(pathNr)){
      anyPathPasses=true;
      trigCounts[pathNr]++;
      for(size_t overlapPathNr=0;overlapPathNr<trigOverlaps[pathNr].size();overlapPathNr++){
 	if(bits.TestBitNumber(overlapPathNr)) trigOverlaps[pathNr][overlapPathNr]++;
      }
    }
  }
  return anyPathPasses;
}


void applyPrescales(TBits& bits,const std::vector<std::pair<string,int> >& pathNamesAndPS,const int entryNr)
{
 
  for(size_t bitNr=0;bitNr<pathNamesAndPS.size();bitNr++){
    if(bits.TestBitNumber(bitNr)){  
      if(pathNamesAndPS[bitNr].second==0 || entryNr%pathNamesAndPS[bitNr].second) bits.SetBitNumber(bitNr,false); //failed prescale so set to false
    }
  }
}

void postProcess(const std::vector<std::pair<string,int> >& pathNamesAndPS,const std::vector<int>& trigCounts,const std::vector<std::vector<int> >& trigOverlaps,float rateSF)
{
  for(size_t pathNr=0;pathNr<pathNamesAndPS.size();pathNr++){
    if(pathNamesAndPS[pathNr].second>0){
      std::cout <<pathNamesAndPS[pathNr].first<<" (ps="<<pathNamesAndPS[pathNr].second<<") : "<<trigCounts[pathNr]*rateSF<<std::endl;
    }
  }
  

}

void TrigRateResults::printOverlaps(size_t index)
{
  if(index>=pathNamesAndPreScales.size()){
    std::cout <<"index larger than number of paths "<<pathNamesAndPreScales.size()<<std::endl;
  }else{
    for(size_t pathNr=0;pathNr<trigOverlaps[index].size();pathNr++){
      std::cout <<pathNamesAndPreScales[pathNr].first<<" (ps="<<pathNamesAndPreScales[pathNr].second<<") : overlap "<<trigOverlaps[index][pathNr]<<" ("<<trigOverlaps[index][pathNr]/static_cast<float>(trigCounts[index])*100.<<"%)"<<std::endl;
    }
  }
}

size_t TrigRateResults::getTrigIndex(const std::string& pathName)
{
  
  for(size_t pathNr=0;pathNr<pathNamesAndPreScales.size();pathNr++){
    if(pathName==pathNamesAndPreScales[pathNr].first){
      return pathNr;
    }
  }
  return pathNamesAndPreScales.size();
}

void TrigRateResults::printTopOverlaps(size_t index,size_t nrToPrint)
{
  
  std::vector<std::pair<std::string,int> > pathNamesAndOverlaps;
  if(index>=pathNamesAndPreScales.size()){
    std::cout <<"index larger than number of paths "<<pathNamesAndPreScales.size()<<std::endl;
  }else{
    
    for(size_t pathNr=0;pathNr<trigOverlaps[index].size();pathNr++){
      pathNamesAndOverlaps.push_back(std::make_pair(pathNamesAndPreScales[pathNr].first,trigOverlaps[index][pathNr]));
    }
    std::sort(pathNamesAndOverlaps.begin(),pathNamesAndOverlaps.end(),PairSortBySecond<std::string,int,std::greater<int> >()); 
    
    for(size_t pathNr=0;pathNr<pathNamesAndOverlaps.size() && pathNr<nrToPrint;pathNr++){
      std::cout <<pathNamesAndOverlaps[pathNr].first<<" : overlap "<<pathNamesAndOverlaps[pathNr].second<<" ("<<pathNamesAndOverlaps[pathNr].second/static_cast<float>(trigCounts[index])*100.<<"%)"<<std::endl;
    }
  }
}
