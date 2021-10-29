#!/usr/bin/env python
from PhysicsTools.NanoAODTools.postprocessing.framework.postprocessor import PostProcessor
from importlib import import_module
import os
import sys
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
from CMSAnalysis.ElectronTriggerSF.helpers import *
from CMSAnalysis.ElectronTriggerSF.Trigger_analysis import *

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser(usage="%prog [options] outputDir inputFiles")
    parser.add_option("--bi", "--branch-selection-input", dest="branchsel_in",
                      type="string", default=None, help="Branch selection input")
    parser.add_option("--bo", "--branch-selection-output", dest="branchsel_out",
                      type="string", default=None, help="Branch selection output")
    parser.add_option("-N", "--max-entries", dest="maxEntries", type="long", default=None,
                      help="Maximum number of entries to process from any single given input tree")
    parser.add_option("--first-entry", dest="firstEntry", type="long", default=0,
                      help="First entry to process in the three (to be used together with --max-entries)")
    parser.add_option("-I", "--import", dest="imports", type="string", default=[], action="append",
                      nargs=2, help="Import modules (python package, comma-separated list of ")
    parser.add_option("-b", "--btagAlgo", dest="btagAlgo",
                      type="string", help="Btag algorithm: [DeepJet, DeepCSV]")
    parser.add_option("-w", "--wp", dest="btagID", type="int", default=0,
                      help="btagging working point (0:loose, 1:medium, 2:tight)")


    (opt, args) = parser.parse_args()
    
    if len(args) < 2:
        parser.print_help()
        sys.exit(1)
    outdir = args[0]
    args = args[1:]
    
    cmssw = os.getenv('CMSSW_BASE')
  
    #get input data
    era, isData = GetEraFromPath(args[0])
    year = ConvertEraToYear(era)
    cut = GetMuonTriggerFromEra(era) if isData else None
    json = cmssw+'/src/CMSAnalysis/ElectronTriggerSF/data/json/'+GetJSON(era) if isData else None
    branches_in = opt.branchsel_in
    branches_out = opt.branchsel_out
    if 'ElectronTriggerSF' not in branches_in: branches_in=cmssw+'/src/CMSAnalysis/ElectronTriggerSF/'+branches_in
    if 'ElectronTriggerSF' not in branches_out: branches_out=cmssw+'/src/CMSAnalysis/ElectronTriggerSF/'+branches_out
    
    print "isData:",isData
    print "year:", year
    if isData: print "era:", era
    print "output directory:", outdir
    print "using json:", json
    print "branches:", branches_out
    print "btagAlgo:", opt.btagAlgo
    print "btagID:", {0:"LooseID",1:"MediumID",2:"TightID"}[opt.btagID]
    print "preselection trigger:", cut
    
    #select proper module          
    analysis_module = TriggerAnalysisModules[year][opt.btagAlgo][opt.btagID]
    
    if not analysis_module:
      print "ERROR: missing input parameter"
      print "year:",year 
      print "btagAlgo:",opt.btagAlgo
      print "btagID:",opt.btagI
  
    p = PostProcessor(outdir, args,
                      cut=cut,
                      branchsel=branches_in,
                      modules=[analysis_module()],
                      jsonInput=json,
                      maxEntries=opt.maxEntries,
                      firstEntry=opt.firstEntry,
                      outputbranchsel=branches_out)
    p.run()