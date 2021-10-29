#!/usr/bin/env python
import os
import sys

def ConvertEraToYear(era):
  if '2016' in era: return '2016preVFP'
  elif '2017' in era: return '2017'
  elif '2018' in era: return '2018'
  print('ERROR: wrong era [era=%s]'%era)
  return ''
  
  
def GetMuonTriggerFromEra(era):
  if '2016' in era: return 'HLT_IsoTkMu22_eta2p1'
  if '2017' in era: return 'HLT_IsoMu27'
  if '2018' in era: return 'HLT_IsoMu24'
  print('ERROR: wrong era [era=%s]'%era)
  return None

def GetJSON(era):
  if '2016' in era: return 'Cert_271036-284044_13TeV_PromptReco_Collisions16_JSON.txt'
  if '2017' in era: return 'Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt'
  if '2018' in era: return 'Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt'
  print('ERROR: wrong era [era=%s]'%era)
  return ''
  
def GetEraFromPath(path):
  isData = True if '/data/' in path else False
  runs=[elem for elem in path.split('/') if 'Run' in elem]
  if not len(runs):
    print 'ERROR: cannot extract era from the file path'
    return ''
  
  if isData: return runs[0].replace('Run',''), True
  runs = runs[0].split('UL')
  if len(runs)<1:
    print 'ERROR: cannot extract the year from the mc file path'
    print 'runs = ',runs
    return ''
  
  else: return '20'+runs[1][:2], False
 