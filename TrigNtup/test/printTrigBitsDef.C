void printTrigBitsDef(const std::string& filename="")
{
  TDirectory* dir =NULL;
  if(filename.empty()) dir = gDirectory;
  else dir = TFile::Open(filename.c_str(),"READ");
  
  int bit1=0x1;
  int nrBitsInWord=32;
     
  std::vector<string>* filterNamesPtr = (std::vector<string>*)gDirectory->Get("trigFilterNames");
  std::vector<string>* pathNamesPtr = (std::vector<string>*)gDirectory->Get("trigPathNames");
  if(filterNamesPtr!=NULL){
    std::cout <<"filter bits "<<std::endl;
    std::vector<string> filterNames = *filterNamesPtr;
    for(size_t i=0;i<filterNames.size();i++){
      std::string test = filterNames[i];
      int bit = bit1;
      bit = bit <<(i%nrBitsInWord);
      int word = i/nrBitsInWord;
      if(!test.empty()) std::cout <<i<<" "<<test.c_str()<<" "<<word<<" "<<std::hex<<" "<<bit<<std::dec<<std::endl;
    }
  }
   if(pathNamesPtr!=NULL){
    std::cout <<"path bits "<<std::endl;
    std::vector<string> pathNames = *pathNamesPtr;
    for(size_t i=0;i<pathNames.size();i++){
      std::string test = pathNames[i]; 
      int bit = bit1;
      bit = bit <<(i%nrBitsInWord);
      int word = i/nrBitsInWord;

      if(!test.empty()) std::cout <<i<<" "<<test.c_str()<<" "<<word<<" "<<std::hex<<" "<<bit<<std::dec<<std::endl;
    }
  }
  if(!filename.empty()) delete dir;

}
