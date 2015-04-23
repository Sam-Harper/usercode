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
            

def printSaveTagFilterNamesAllPaths(process):
    for pathName in process.pathNames().split():
        printSaveTagFilterNames(process,pathName)
        
  
