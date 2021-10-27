def rmPaths(process,pathsToKeep):
    for pathName in process.pathNames().split():
        if pathName.find("HLT_")==0 and pathName not in pathsToKeep:
            path = getattr(process,pathName)
            print("removing path ",pathName)
            for moduleName in path.moduleNames():
                notAllCopiesRemoved=True
                while notAllCopiesRemoved:
                    notAllCopiesRemoved = path.remove(getattr(process,moduleName))
