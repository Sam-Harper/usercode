
def swapCollection(process,oldCollection,newCollection):
    for pathName in process.pathNames().split():
        path = getattr(process,pathName)
        for moduleName in path.moduleNames():
            if moduleName != newCollection:
                module = getattr(process,moduleName)
                for paraName in module.parameters_():
                    para = module.getParameter(paraName)
                    if type(para).__name__=="InputTag":
                        if para.getModuleLabel()==oldCollection:
                            para.setModuleLabel(newCollection)
                            
 #print paraName,type(para).__name__,para.getModuleLabel()
                    if type(para).__name__=="VInputTag":
                        for tag in para:
                            if tag.getModuleLabel()==oldCollection:
                                tag.setModuleLable(newCollection)


def swapCollectionModuleAndProductLabels(process,oldModuleLabel,oldProdInstLabel,newModuleLabel,newProdInstLabel):
    for pathName in process.pathNames().split():
        path = getattr(process,pathName)
        for moduleName in path.moduleNames():
            if moduleName != newModuleLabel:
                module = getattr(process,moduleName)
                for paraName in module.parameters_():
                    para = module.getParameter(paraName)
                    if type(para).__name__=="InputTag":
                        if para.getModuleLabel()==oldModuleLabel:
                            if para.getProductInstanceLabel()==oldProdInstLabel:
                                para.setModuleLabel(newModuleLabel)
                                para.setProductInstanceLabel(newProdInstLabel)
                            
 #print paraName,type(para).__name__,para.getModuleLabel()
                    if type(para).__name__=="VInputTag":
                        for tag in para:
                            if tag.getModuleLabel()==oldModuleLabel:
                                if tag.getProductInstanceLabel()==oldProdInstLabel:
                                    tag.setModuleLable(newModuleLable)
                                    tag.setProductLabel(newProductLabel)
