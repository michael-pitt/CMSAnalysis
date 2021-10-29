# CMSAnalysis

## Installation instructions

```
cmsrel CMSSW_10_6_27
cd CMSSW_10_6_27/src
cmsenv

#This package
cd $CMSSW_BASE/src
git clone git@github.com:michael-pitt/CMSAnalysis.git
scram b -j 8
```

## Table of content

The repository contains several tools/packages to do analysis with [CMSSW](https://cms-sw.github.io/)

### 1. ElectronTriggerSF

The package used to extract electron trigger efficiency correction based on CMSSW tools.

