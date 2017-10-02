import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.ioFilesOpt = cms.PSet(

    ##input file
    inputFiles = cms.vstring('root://eoscms.cern.ch//store/user/bmarzocc/GluGluHtoGX/GEN/GluGluHToGX_MCRUN2_71_V1_TuneCUETP8M1_13TeV_pythia8_GEN.root'),
    
    ## base output directory: default output/
    outputDir = cms.string(''),

    ## base output: default GenParticlesAnalysis.root 
    outputFile = cms.string(''),
     
    ## maxEvents
    maxEvents = cms.untracked.int32(-1)
)

process.genParticlesOpt = cms.PSet(
    
    genParticles = cms.vstring(
       'photon 22 1 25',
       'dark_photon 5000022 1 25',
       'higgs 25 62 25'
    ),
    
    sumParticles = cms.vstring(
       '1 2'
    ),

    genVariables = cms.vstring(
        'energy 100 0. 600.',
        'p 100 0. 600.',
        'mass 100 0 200',
        'pt 100 0. 300.',
        'eta 100 -6. 6.',
        'phi 100 -3.5 3.5',
        'mt 100 0. 300.',
        'et 100 0. 300', 
       #'et2 100 0. 500',
       #'px 100 0. 500.',
       #'py 100 0. 500.',
       #'pz 100 0. 500.',
       #'y 100 0. -10. 10.'
    )
)

process.genJetOpt = cms.PSet(
    
    jetTypes = cms.vstring(
       'ak4GenJets',
       'ak5GenJets',
       #'ak8GenJets'
    ),

    jetVariables = cms.vstring(
        'energy 100 0. 600.',
        'p 100 0. 600.',
        'mass 100 0 200',
        'pt 100 0. 300.',
        'eta 100 -8. 8.',
        'phi 100 -3.5 3.5',
        'emEnergy 100 0. 500.',
        'hadEnergy 100 0. 500.',
        'invisibleEnergy 100 0. 500.',
        'auxiliaryEnergy 100 0. 500.',
        'mt 100 0. 300.',
        'et 100 0. 300', 
       #'et2 100 0. 500',
       #'px 100 0. 500.',
       #'py 100 0. 500.',
       #'pz 100 0. 500.',
       #'y 100 0. -10. 10.'
    )   
)

process.genMETOpt = cms.PSet(
    
    metTypes = cms.vstring(
       'genMetCalo',
       'genMetCaloAndNonPrompt'
    ),   
    
    metVariables = cms.vstring(
       'pt 100 0. 500.',
       'phi 100 -3.5 3.5',
       'NeutralEMEtFraction 100 0. 1.',
       'NeutralEMEt 100 0. 200.',
       'ChargedEMEtFraction 100 0. 1.',
       'ChargedEMEt 100 0. 200.',
       'NeutralHadEtFraction 100 0. 1.',
       'NeutralHadEt 100 0. 200.',
       'ChargedHadEtFraction 100 0. 1.',
       'ChargedHadEt 100 0. 200.',
       'MuonEtFraction 100 0. 1.',
       'MuonEt 100 0. 200.',
       'InvisibleEtFraction 100 0. 1.',
       'InvisibleEt 100 0. 200.',
    )   
)
