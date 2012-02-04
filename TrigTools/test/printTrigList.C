void printTrigList(){
  std::vector<string>* filterDefPointer = (std::vector<string>*)gDirectory->Get("filterDefs");
  if(filterDefPointer==NULL) return;
  std::vector<string> filterDefRef = *filterDefPointer;
  for(size_t i=0;i<filterDefRef.size();i++){
    std::string test = filterDefRef[i];
    std::cout <<i<<" "<<test.c_str()<<std::endl;
  }


}
