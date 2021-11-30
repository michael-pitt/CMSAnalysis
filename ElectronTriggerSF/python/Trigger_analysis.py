#!/usr/bin/env python
import os, sys
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


from PhysicsTools.NanoAODTools.postprocessing.framework.eventloop import Module
from PhysicsTools.NanoAODTools.postprocessing.framework.datamodel import Collection
from CMSAnalysis.ElectronTriggerSF.objectSelector import ElectronSelector, MuonSelector
from CMSAnalysis.ElectronTriggerSF.helpers import deltaR

class TriggerAnalysis(Module):
    def __init__(self, year, btagAlgo, btagID):
        self.bjetDeepJetWP = { # loose, medium, tight
                      '2016preVFP' : [0.0614, 0.3093, 0.7221], #https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation2016Legacy
                      '2016postVFP' : [0.0480, 0.2489, 0.6377], #https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16postVFP
                      '2017': [0.0532, 0.3040, 0.7476], #https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL17
                      '2018': [0.0490, 0.2783, 0.7100] #https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL18
                      }
        self.bjetDeepCSVWP = {
                      '2016preVFP' : [0.2217, 0.6321, 0.8953],
                      '2016postVFP' : [0.1918, 0.5847, 0.8767],
                      '2017': [0.1355, 0.4506, 0.7738],
                      '2018': [0.1208, 0.4168, 0.7665]
                      }
        self.bjetWP = self.bjetDeepJetWP[year][btagID] if btagAlgo=='DeepJet' else self.bjetDeepCSVWP[year][btagID]
        self.btagAlgo = btagAlgo
		
        pass

    def beginJob(self):
        pass

    def endJob(self):
        pass

    def beginFile(self, inputFile, outputFile, inputTree, wrappedOutputTree):
    
        self.out = wrappedOutputTree
        self.out.branch("nano_nBJets",     "I");
        self.out.branch("nano_nElectrons", "I");
        self.out.branch("nano_ElID",       "I",  lenVar = "nano_nElectrons");
        self.out.branch("nano_ElPT",       "F",  lenVar = "nano_nElectrons");
        self.out.branch("nano_ElEta",      "F",  lenVar = "nano_nElectrons");
        self.out.branch("nano_ElMatch",    "O",  lenVar = "nano_nElectrons");
        self.out.branch("nano_HT" ,        "F");
        self.out.branch("nano_nJets",      "I");
        self.out.branch("nano_JetPT",      "F",  lenVar = "nano_nJets");
        self.out.branch("nano_JetEta",     "F",  lenVar = "nano_nJets");

        
    def endFile(self, inputFile, outputFile, inputTree, wrappedOutputTree):
        pass

    def triggerMatched(self, object, trigger_objects, type, dR = 0.3):
        for trig_obj in trigger_objects:
            if abs(trig_obj.id) != type: continue
            if deltaR(object,trig_obj) < dR: return True
        return False
	
    def selectElectrons(self, event, selector, triggerObjects):
        ## access a collection in nanoaod and create a new collection based on this

        event.selectedElectrons = []
        electrons = Collection(event, "Electron")
        for el in electrons:
            if not selector.evalElectron(el): continue
            		
            #trigger matching
            isMatched = self.triggerMatched(el, triggerObjects, 11)
            setattr(el, 'trigMatch', isMatched) 
			
            #ID flags:
            idflag = 0
            if el.mvaFall17V2Iso_WPL:  idflag += (1 << 0)
            if el.mvaFall17V2Iso_WP90: idflag += (1 << 1)
            if el.mvaFall17V2Iso_WP80: idflag += (1 << 2)
            if el.cutBased==2: idflag += (1 << 3)
            if el.cutBased==3: idflag += (1 << 4)
            if el.cutBased==4: idflag += (1 << 5)
            if not idflag: continue
            setattr(el, 'id', idflag)
            
            event.selectedElectrons.append(el)
        # sort collection
        event.selectedElectrons.sort(key=lambda x: x.pt, reverse=True)
        

    def selectMuons(self, event, selector):
        ## access a collection in nanoaod and create a new collection based on this

        event.selectedMuons = []
        muons = Collection(event, "Muon")
        for mu in muons:
            if selector.evalMuon(mu):
               event.selectedMuons.append(mu)
        # sort collection
        event.selectedMuons.sort(key=lambda x: x.pt, reverse=True)


    def selectAK4Jets(self, event):
        ## Selected jets: pT>25, |eta|<4.7, pass tight ID
        
        event.selectedAK4Jets = []
        ak4jets = Collection(event, "Jet")
        for j in ak4jets:

            if j.pt<25 : 
                continue

            if abs(j.eta) > 4.7:
                continue
            
            #require tight (2^1) or tightLepVeto (2^2) [https://twiki.cern.ch/twiki/bin/view/CMS/JetID#nanoAOD_Flags]
            if j.jetId<4 :   # tight PU ID and tight Lep Veto
                continue
                
            #check overlap with selected leptons which are considered to be isolated 
            deltaR_to_leptons=[ j.p4().DeltaR(lep.p4()) for lep in event.selectedMuons+event.selectedElectrons  ]
            hasLepOverlap=sum( [dR<0.4 for dR in deltaR_to_leptons] )
            if hasLepOverlap>0: continue

            event.selectedAK4Jets.append(j)
            
        event.selectedAK4Jets.sort(key=lambda x: x.pt, reverse=True)

    def analyze(self, event):
        """process event, return True (go to next module) or False (fail, go to next event)"""
		
        #initiate selector tools:
        triggerObjects = Collection(event, "TrigObj")
        elSel = ElectronSelector(minPt = 30)
        muSel = MuonSelector(minPt = 30, id = "tight")

        # apply object selection
        self.selectMuons(event, muSel)
        self.selectElectrons(event, elSel, triggerObjects)
        self.selectAK4Jets(event)
		       
        #apply event pre-selection (emu (OS), muon trigger matched, >=1 bjet):
        if len(event.selectedMuons)!=1: return False
        if len(event.selectedElectrons)!=1: return False
		
        #trigger matching
        if not self.triggerMatched(event.selectedMuons[0], triggerObjects, 13): return False
        
        #OS leptons
        if event.selectedElectrons[0].charge==event.selectedMuons[0].charge: return False
		
        if len(event.selectedAK4Jets)<2: return False
        nbjets=0; ht=0
        for jet in event.selectedAK4Jets:
          ht+=jet.pt
          flabtag = jet.btagDeepFlavB if self.btagAlgo=='DeepJet' else jet.btagDeepB
          if flabtag > self.bjetWP: nbjets+=1
        if nbjets<1: return False
        
        ## store branches
        el_id=[el.id for el in event.selectedElectrons]
        el_pt=[el.pt for el in event.selectedElectrons]
        el_eta=[el.eta for el in event.selectedElectrons]
        el_match=[el.trigMatch for el in event.selectedElectrons]
        jet_pt=[jet.pt for jet in event.selectedAK4Jets]
        jet_eta=[jet.eta for jet in event.selectedAK4Jets]
        
        self.out.fillBranch("nano_nJets" ,    len(event.selectedAK4Jets))
        self.out.fillBranch("nano_nBJets",    nbjets)
        self.out.fillBranch("nano_nElectrons",len(event.selectedElectrons))
        self.out.fillBranch("nano_ElID" ,     el_id)
        self.out.fillBranch("nano_ElPT" ,     el_pt)
        self.out.fillBranch("nano_ElEta" ,    el_eta)
        self.out.fillBranch("nano_ElMatch",   el_match)
        self.out.fillBranch("nano_JetPT" ,    jet_pt)
        self.out.fillBranch("nano_JetEta",    jet_eta)
        self.out.fillBranch("nano_HT" ,       ht)

        return True

# define modules using the syntax 'name = lambda : constructor' to avoid
# having them loaded when not needed
TriggerAnalysisModules={}
for year in ['2016preVFP','2016postVFP','2017','2018']:
  TriggerAnalysisModules[year]={}
  for btagAlgo in ['DeepJet','DeepCSV']:
    TriggerAnalysisModules[year][btagAlgo]={}
    for btagID in [0,1,2]: 
      TriggerAnalysisModules[year][btagAlgo][btagID] = lambda : TriggerAnalysis(year=year, btagAlgo=btagAlgo, btagID=btagID)

