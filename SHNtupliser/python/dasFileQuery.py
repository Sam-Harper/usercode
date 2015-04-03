#stolen from HLTrigger/Configuration/python/Tools/dasFileQuery.py
#also parts stolen  from edmPickEvents.py

import sys
import json
import das_client

def dasFileQuery(dataset):
  query   = 'dataset dataset=%s' % dataset
  host    = 'https://cmsweb.cern.ch'      # default
  idx     = 0                             # default
  limit   = 0                             # unlimited
  debug   = 0                             # default
  thr     = 300                           # default
  ckey    = ""                            # default
  cert    = ""                            # default
  jsondict = das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)

  # check if the pattern matches none, many, or one dataset
  if not jsondict['data'] or not jsondict['data'][0]['dataset']:
    sys.stderr.write('Error: the pattern "%s" does not match any dataset\n' % dataset)
    sys.exit(1)
    return []
  elif len(jsondict['data']) > 1:
    sys.stderr.write('Error: the pattern "%s" matches multiple datasets\n' % dataset)
    for d in jsondict['data']:
      sys.stderr.write('    %s\n' % d['dataset'][0]['name'])
    sys.exit(1)
    return []
  else:
    # expand the dataset name
    dataset = jsondict['data'][0]['dataset'][0]['name']
    query = 'file dataset=%s' % dataset
    jsondict = das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)
    # parse the results in JSON format, and extract the list of files
    files = sorted( f['file'][0]['name'] for f in jsondict['data'] )
    return files


class Event (dict):

    dataset = None
    import re
    splitRE = re.compile (r'[\s:,]+')
    def __init__ (self, line, **kwargs):
        pieces = Event.splitRE.split (line.strip())
        try:
            self['run']     = int( pieces[0] )
            self['lumi']    = int( pieces[1] )
            self['event']   = int( pieces[2] )
            self['dataset'] =  Event.dataset
        except:
            raise RuntimeError, "Can not parse '%s' as Event object" \
                  % line.strip()
        if not self['dataset']:
            print "No dataset is defined for '%s'.  Aborting." % line.strip()
            raise RuntimeError, 'Missing dataset'

    def __getattr__ (self, key):
        return self[key]

    def __str__ (self):
        return "run = %(run)i, lumi = %(lumi)i, event = %(event)i, dataset = %(dataset)s"  % self


  
def getFileNames (event):
  
  query = "file dataset=%(dataset)s run=%(run)i lumi=%(lumi)i | grep file.name" % event
  host    = 'https://cmsweb.cern.ch'      # default
  idx     = 0                             # default
  limit   = 0                             # unlimited
  debug   = 0                             # default
  thr     = 300                           # default
  ckey    = ""                            # default
  cert    = ""                            # default
  jsondict = das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)
  count=1
  while jsondict['status']!='ok' and count<=10:
    jsondict = das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)
    count=count+1

  files=[]

  if jsondict['status']!='ok':
    print "das lookup of ",query," failed ",count," times"
  else:
    try:
      files = sorted( f['file'][0]['name'] for f in jsondict['data'] )
    except:
      print "no files found for ",query
      
  return files
  
