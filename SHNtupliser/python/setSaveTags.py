def setSaveTags(process,pathName,saveTagsValue):
    path = getattr(process,pathName)
    for filterName in path.moduleNames():
        filt = getattr(process,filterName)
        if type(filt).__name__=="EDFilter":
            if filt.hasParameter("saveTags"):
                param=filt.getParameter("saveTags")
                param.setValue(saveTagsValue)
           # else:
           #     print filterName,"has no save tags field"

def printSaveTagFilterNames(process,pathName):
    print pathName,
    hasSaveTags=False
    path = getattr(process,pathName)
    for filterName in path.moduleNames():
        filt = getattr(process,filterName)
        if type(filt).__name__=="EDFilter":
            if filt.hasParameter("saveTags"):
                param=filt.getParameter("saveTags")
                if param.value()==True:
                    if hasSaveTags==False:
                        print 
                    hasSaveTags=True
                    print "     ",filterName
    if hasSaveTags==False:
        print " has NO saved filters"

filtersNeverALastPath=["HLTPMMassFilter",]
filtersToIgnoreIfTheyInclude=["HLTPMMassFilter","HLT2MuonMuonDZ","HLT2PhotonPhotonDZ"]
    
def isLastFilterEG(process,path,filterNameToCheck):
    for filterName in path.moduleNames():
        filt =getattr(process,filterName)
        if type(filt).__name__=="EDFilter":
            if "candTag" in filt.parameterNames_():
                
                param=filt.getParameter("candTag")
                if param.getModuleLabel()==filterNameToCheck:
                    return False
    return True

def isLastFilter(process,path,filterNameToCheck):
    if filterNameToCheck.find("hltL1sL1")==0:
        #L1 filter, they always should be saved
        return True
    
    for filterName in path.moduleNames():
        
        filt =getattr(process,filterName)
        if filt.type_() in filtersToIgnoreIfTheyInclude:
#            print "ignoring ",filterName
            continue
        if type(filt).__name__=="EDFilter":
            for paraName in filt.parameterNames_():
                
                param=filt.getParameter(paraName)
                if type(param).__name__=="InputTag":
                    if param.getModuleLabel()==filterNameToCheck:
                        return False
                if type(param).__name__=="VInputTag":
                #    print filterName
                    for index in range(0,len(param)):
                        #print index,param[index]
                        if param[index]==filterNameToCheck:
                            return False
                
    return True

def printSaveTagValFilterNames(process,pathName):
    outputStr=pathName
    dodgySaveTags=False
    path = getattr(process,pathName)
    for filterName in path.moduleNames():
        filt = getattr(process,filterName)
        if type(filt).__name__=="EDFilter":
            if filt.hasParameter("saveTags"):
                param=filt.getParameter("saveTags")
                isLast = isLastFilter(process,path,filterName)
                ignoreFilt= filt.type_() in filtersNeverALastPath
                if isLast and param.value()==False and not ignoreFilt:
                    dodgySaveTags=True
                    outputStr+="\n    "+" last filter "+filterName+" saveTags = "+str(param.value())
    if(dodgySaveTags):
        print outputStr
            
def printSaveTagFilterNamesAllPaths(process):
    for pathName in process.pathNames().split():
        printSaveTagValFilterNames(process,pathName)


#def isEgammaFilter(filterName):
#    if
        

