


import FWCore.ParameterSet.Config as cms
import argparse
import importlib
import sys
                
                
def get_all_input_tags(pset):
    input_tag_names = set()
    for para_name in pset.parameterNames_():
        if hasattr(pset,para_name):
            para = getattr(pset,para_name)
            para_type = para.pythonTypeName()
            if para_type == "cms.InputTag":
                if para.getModuleLabel()!='':
                    input_tag_names.add(para.getModuleLabel())
            if para_type == "cms.VInputTag":
                for entry in para:
                    if type(entry)==cms.InputTag:
                        input_tag_names.add(entry.getModuleLabel())
                    else:
                        input_tag_names.add(entry)
            if para_type == "cms.PSet":
                input_tag_names |= get_all_input_tags(para)
            if para_type == "cms.VPSet":
                for pset in para:
                    input_tag_names |= get_all_input_tags(pset)
    return input_tag_names

def print_mod_dependents(process,module_name,depth,indent_depth):
    mod = getattr(process,module_name)
    input_tag_names = get_all_input_tags(mod)

    indent_mod = ""
    for i in range(1,indent_depth):
        indent_mod += "    "
    indent_submod = ""
    for i in range(0,indent_depth):
        indent_submod += "    "

    print indent_mod+module_name+" ("+mod.type_()+"):"
    for name in input_tag_names:
        if depth>0:
            try:
                print_mod_dependents(process,name,depth-1,indent_depth+1)
            except AttributeError:
                if type(name) == cms.InputTag:
                    print indent_submod+name.getModuleLabel()
                else:
                    print indent_submod+name   
    
        else:
            if type(name) == cms.InputTag:
                print indent_submod+name.getModuleLabel()
            else:
                print indent_submod+name
            

def print_mod_direct_dependents(process,module_name):

    mod = getattr(process,module_name)
    input_tag_names = get_all_input_tags(mod)
    print module_name+" ("+mod.type_()+"):"
    for name in input_tag_names:
        print "    "+name+":"
        
    

def main():
    parser = argparse.ArgumentParser(description='prints the dependent modules of a module')
    parser.add_argument('config',help="config file to read")
    parser.add_argument('--module_name','-m',help='module name to print dependents of' )
    parser.add_argument('--depth','-d',help="depth to print module deps",type=int)
    args = parser.parse_args()

    mod = importlib.import_module(args.config)
    
    process = getattr(mod,"process")
    print_mod_dependents(process,args.module_name,args.depth,1)
    

if __name__ == "__main__":
    main()
