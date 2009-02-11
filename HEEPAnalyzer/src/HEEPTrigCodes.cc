#include "SHarper/HEEPAnalyzer/interface/HEEPTrigCodes.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

heep::TrigCodes::TrigBitSetMap heep::TrigCodes::trigBitSetMap_;

void heep::TrigCodes::setCodes(std::vector<std::string>& filterNames)
{
  if(trigBitSetMap_.size()!=0){
    edm::LogWarning("heep::TrigCodes") <<" Warning trigBitSetMap already filled ";
  }else{
    for(size_t i=0;i<filterNames.size();i++) trigBitSetMap_.setCode(filterNames[i].c_str(),i);
    trigBitSetMap_.sort();
  }
}

void heep::TrigCodes::TrigBitSetMap::setCode(const std::string& descript,int bitNr)
{
  if(bitNr<maxNrBits()){
    TrigBitSet code;
    code.set(bitNr);
    setCode(descript,code);
  }else{
    edm::LogWarning("heep::TrigCodes::TrigBitSetMap") <<" Warning, trying to store at bit "<<bitNr<<" but max nr bits is "<<maxNrBits();
  }
}

void heep::TrigCodes::TrigBitSetMap::setCode(const std::string& descript,TrigBitSet code)
{
  bool found=false;
  for(size_t i=0;i<codeDefs_.size() && !found;i++){
    if(codeDefs_[i].first.compare(descript)==0) found=true;
  }
  if(!found) codeDefs_.push_back(std::make_pair<std::string,TrigBitSet>(descript,code));
}



heep::TrigCodes::TrigBitSet heep::TrigCodes::TrigBitSetMap::getCode(const std::string& descript)const
{ 
  //first copy the character string to a local array so we can manipulate it
  char localDescript[512];
  strcpy(localDescript,descript.c_str());
  
  TrigBitSet code; 
  char* codeKey = strtok(localDescript,":");
  //  std::map<std::string,int> ::const_iterator mapIt;
  while(codeKey!=NULL){
    bool found=false;

    for(size_t i=0;i<codeDefs_.size() && !found;i++){
      if(codeDefs_[i].first.compare(codeKey)==0){
 	found=true;
 	code |= codeDefs_[i].second;

       }
    }
   
    if(!found)  edm::LogError("heep::TrigCodes::TrigBitSetMap") <<"heep::TrigCodes::TrigBitSetMap::getCode : Error, Key "<<codeKey<<" not found";
    codeKey = strtok(NULL,":"); //getting new substring
    
  }
  return code;
}

//when I get around to bench marking it to ensure its not too slow, both getCode functions will use this function with the other using boost to create a std::ector<std::string>
heep::TrigCodes::TrigBitSet heep::TrigCodes::TrigBitSetMap::getCode(const std::vector<std::string>& descripts)const
{ 
  
  TrigBitSet code; 
  for(size_t descriptNr=0;descriptNr<descripts.size();descriptNr++){
    const std::string& codeKey = descripts[descriptNr];
    bool found=false;
    for(size_t i=0;i<codeDefs_.size() && !found;i++){
      if(codeDefs_[i].first.compare(codeKey)==0){
 	found=true;
 	code |= codeDefs_[i].second;
	
       }
    }
    
    if(!found)  edm::LogError("heep::TrigCodes::TrigBitSetMap") <<"heep::TrigCodes::TrigBitSetMap::getCode : Error, Key "<<codeKey<<" not found";
    
  }
  return code;
}


bool heep::TrigCodes::TrigBitSetMap::keyComp(const std::pair<std::string,TrigBitSet>& lhs,const std::pair<std::string,TrigBitSet>& rhs)
{
  return lhs.first < rhs.first;
}

void heep::TrigCodes::TrigBitSetMap::getCodeName(TrigBitSet code,std::string& id)const
{
  id.clear();
  for(size_t i=0;i<codeDefs_.size();i++){ 
    if((code&codeDefs_[i].second)==codeDefs_[i].second){
      if(!id.empty()) id+=":";//seperating entries by a ':'
      id+=codeDefs_[i].first;
    }
    
  }
 
}

void heep::TrigCodes::TrigBitSetMap::printCodes()
{
  edm::LogInfo("heep::TrigCodes::TrigBitSetMap") <<"trig bits defined";
  for(size_t i=0;i<codeDefs_.size();i++) edm::LogInfo("heep::TrigCodes::TrigBitSetMap") <<" key : "<<codeDefs_[i].first<<" bit "<<codeDefs_[i].second;
}
