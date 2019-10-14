


import FWCore.ParameterSet.Config as cms
import argparse
import importlib
import sys
import random

def get_all_input_tags(pset):
    input_tag_names = set()
    names = pset.parameterNames_()
    for para_name in names:
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
                #oddly iterating over it normally
                #wipes out parameters after untracked paras
                for index in range(0,len(para)):
                    input_tag_names |= get_all_input_tags(para[index])
    return input_tag_names                      


def get_mod_dependents(process,mod_data):
    mod = getattr(process,mod_data['name'])
    input_tag_names = get_all_input_tags(mod)
    mod_data['deps'] = []
    for dep_name in input_tag_names:
        mod_data['deps'].append({'name' : dep_name})
        try:
            get_mod_dependents(process,mod_data['deps'][-1])
        except AttributeError:
            if dep_name not in ["rawDataCollector","listfeds"]:
                print "error modname is not present",dep_name

def get_mod_dependents2(process,mod_name,mods_deps=None):
    if not mods_deps:
        mods_deps = {}

    #we check if the product is produced in this config file or was from another step
    if not hasattr(process,mod_name): 
        return
        
    print mod_name
    if mod_name not in mods_deps:
        mods_deps[mod_name] = []

    mod = getattr(process,mod_name)
    print mod.dumpPython()
    input_tag_names = get_all_input_tags(mod)
    for dep_name in input_tag_names:
        if hasattr(process,dep_name) and dep_name not in ['earlyMuons']:
            mods_deps[mod_name].append(dep_name)
            if dep_name not in mods_deps:
                get_mod_dependents2(process,dep_name,mods_deps)

    
    return mods_deps

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
    
def sort_with_depth_1st_visit(mod_name,mods_deps,perm_marks,temp_marks,no_marks,sorted_list):
    if mod_name in perm_marks:
        return
    if mod_name in temp_marks:
        raise Exception("...has circular deps...")
    temp_marks.append(mod_name)
    no_marks.remove(mod_name)
    for dep_name in  mods_deps[mod_name]:
        sort_with_depth_1st_visit(dep_name,mods_deps,perm_marks,temp_marks,no_marks,sorted_list)
    temp_marks.remove(mod_name)
    perm_marks.append(mod_name)
#    sorted_list.insert(0,mod_name)
    sorted_list.append(mod_name)
    


def sort_with_depth_1st(mods_deps):
    sorted_list = []
    perm_marks = []
    temp_marks = []
    no_marks = mods_deps.keys()
    nr_mods = len(mods_deps.keys())
    
    sorted_list = []

    while len(perm_marks)!=nr_mods:
        sort_with_depth_1st_visit(random.choice(no_marks),mods_deps,perm_marks,temp_marks,no_marks,sorted_list)
    return sorted_list
    

def get_modules_as_string(mods):
    sorted_keys = mods.keys()
    sorted_keys.sort()
    out_str = "\n".join("process.{key} = {mod}".format(key=key,mod=mods[key].dumpPython()) for key in sorted_keys)
    out_str = out_str.format(**mods)
    return out_str

def print_mod_direct_dependents(process,module_name):

    mod = getattr(process,module_name)
    input_tag_names = get_all_input_tags(mod)
    print module_name+" ("+mod.type_()+"):"
    for name in input_tag_names:
        print "    "+name+":"
        
def check_for_circular_dep(dep_chain,mods_deps):
    #if dep_chain[0] in mods_deps[dep_chain[-1]]:
     #   print "{} {} depend on each other".format(dep_chain[0],dep_chain[-1])
        #print dep_chain
        #dep_chain.pop()
      #  return True

    for dep_name in mods_deps[dep_chain[-1]]:
        if dep_name not in dep_chain:
            dep_chain.append(dep_name)
        else:
            print "circ dep found",dep_chain,dep_name
            return True

        if check_for_circular_dep(dep_chain,mods_deps):
            dep_chain.pop()
            return True
        dep_chain.pop()
        
    return False


def main():
    parser = argparse.ArgumentParser(description='prints the dependent modules of a module')
    parser.add_argument('config',help="config file to read")
    parser.add_argument('--module_name','-m',help='module name to print dependents of' )
    parser.add_argument('--out_file','-o',default=None,help='output file')
    
    args = parser.parse_args()

    cmssw_cfg = importlib.import_module(args.config)
    
    process = getattr(cmssw_cfg,"process")
    print get_all_input_tags(getattr(process,args.module_name))
  #  return
 #   mod_data = {'name' : args.module_name}
#    get_mod_dependents(process,mod_data)

    mods_deps = get_mod_dependents2(process,args.module_name)
    print mods_deps 

    for mod_name in mods_deps:
        check_for_circular_dep([mod_name],mods_deps)
    
    sorted_mods = sort_with_depth_1st(mods_deps)
    print sorted_mods
    
    cfg_header ="""
import FWCore.ParameterSet.Config as cms

process = cms.Process("HLT")

    """

    print mods_deps[sorted_mods[-1]]
    if args.out_file:
        with open(args.out_file,'w') as f:
            f.write(cfg_header)
            f.write("\n#ES Sources\n"+get_modules_as_string(process.es_sources))
            f.write("\n#ES Producers\n"+get_modules_as_string(process.es_producers))
            f.write("\n#ES PSets\n"+get_modules_as_string(process.psets))
            
            for mod_name in sorted_mods:
                mod = getattr(process,mod_name)
                f.write("\nprocess.{}={}".format(mod_name,mod.dumpPython()))
            f.write("\n")
            
            seq_str = "process.hlt_seq = cms.Sequence({})\n".format(" +\n{}".format(" "*3).join("process.{}".format(x) for x in sorted_mods))
            f.write(seq_str)
   
if __name__ == "__main__":
    main()
