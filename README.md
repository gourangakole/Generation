# Generation

1) Run GEN-SIM production on lxbatch (CMSSW_9_3_X 2017 MC_v2 campaign):

    * scram project CMSSW_9_3_4
    * cd CMSSW_9_3_4/src/
    * cmsenv
    * git clone  git@github.com:bmarzocc/Generation.git
    * cd Generation
    * git checkout 93X
    * scram b -j 5
    * launch step:
   
      * cd ProduceGENSIM/lxbatch/
      * perl launchJobs_lxbatch_GEN-SIM.pl params_lxbatch_GEN-SIM.CFG
      * sh lancia.sh
      
    * Parameters settings in "params_lxbatch_GEN-SIM.CFG":
   
      * BASEDir: absolute path of the lxbatch directory
      * JOBdir: directory to store job directories
      * JOBCfgTemplate: name of the python to launch
      * HEPMCinput: name and path of the gridpack to be run
      * OUTPUTSAVEPath: path of the output files (EOS directories as default)
      * OUTPUTFILEName: name of the output file.root
      * EVENTSNumber: total number of events to generate
      * EVENTSPerjob: number of events per job
      * EXEName: name of the job executable
      * SCRAM_ARCH: scram_arch
      * X509_USER_PROXY: path of the proxy (before launching better do: export X509_USER_PROXY=/afs/cern.ch/work//XXX/x509up_XXX)
      * QUEUE: lxbatch queue
      
    * NOTE: Any new GEN-SIM step cfg (different MC production campaign) can be adapted in order to work with this package. Change/add the following lines
    
      ...
      ```Pythonscript
      process.maxEvents = cms.untracked.PSet(
         input = cms.untracked.int32(EVENTSperJOB)
      )
      ```
      ...
      ```Pythonscript
      
      import SimGeneral.Configuration.ThrowAndSetRandomRun as ThrowAndSetRandomRun
      ThrowAndSetRandomRun.throwAndSetRandomRun(process.source,[(JOBID,1)])

      process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",

         externalLHEProducer = cms.PSet(
            initialSeed = cms.untracked.uint32(SEED1),
            engineName = cms.untracked.string('HepJamesRandom')
         ),
         generator = cms.PSet(
            initialSeed = cms.untracked.uint32(SEED2),
            engineName = cms.untracked.string('HepJamesRandom')
         ),
         VtxSmeared = cms.PSet(
            initialSeed = cms.untracked.uint32(SEED3),
            engineName = cms.untracked.string('HepJamesRandom')
         ),
         g4SimHits = cms.PSet(
            initialSeed = cms.untracked.uint32(SEED4),
            engineName = cms.untracked.string('HepJamesRandom')
         )

      )
      ```
      ...
      ```Pythonscript
      process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
         SelectEvents = cms.untracked.PSet(
            SelectEvents = cms.vstring('generation_step')
         ),
         compressionAlgorithm = cms.untracked.string('LZMA'),
         compressionLevel = cms.untracked.int32(9),
         dataset = cms.untracked.PSet(
            dataTier = cms.untracked.string('GEN-SIM'),
            filterName = cms.untracked.string('')
         ),
         eventAutoFlushCompressedSize = cms.untracked.int32(20971520),
         fileName = cms.untracked.string('file:OUTPUTFILEName.root'),
         outputCommands = process.RAWSIMEventContent.outputCommands,
         splitLevel = cms.untracked.int32(0)
      )
      ```
      ...
      ```Pythonscript
      process.externalLHEProducer = cms.EDProducer("ExternalLHEProducer",
         nEvents = cms.untracked.uint32(EVENTSperJOB),
         outputFile = cms.string('cmsgrid_final.lhe'),
         scriptName = cms.FileInPath('GeneratorInterface/LHEInterface/data/run_generic_tarball_cvmfs.sh'),
         numberOfParameters = cms.uint32(1),
         args = cms.vstring('GRIDPACK')
      )
      ```
      ...
      
2) Run GEN analysis (whichever release):
    
    * scram project CMSSW_9_3_4
    * cd CMSSW_9_3_4/src/
    * cmsenv
    * git clone  git@github.com:bmarzocc/Generation.git
    * cd Generation
    * scram b -j 5
    * cd Generation/GenStudies
    * GenParticlesAnalysis python/GenParticlesAnalysis_cfg.py

3) Run DIGI-RECO production on lxbatch (CMSSW_9_4_X 2017 MC_v2 campaign):

    * scram project CMSSW_9_4_0
    * cd CMSSW_9_4_0/src/
    * cmsenv
    * git clone  git@github.com:bmarzocc/Generation.git
    * cd Generation
    * git checkout 94X
    * scram b -j 5
    * launch step:
   
      * cd ProduceDIGIRECO/lxbatch/
      * perl launchJobs_lxbatch_step1_step2.pl params_lxbatch_step1_step2.CFG
      * sh lancia.sh
      
    * Parameters settings in "params_lxbatch_step1_step2.CFG":
   
      * BASEDir: absolute path of the lxbatch directory
      * JOBdir: directory to store job directories
      * JOBCfgTemplate1: name of the step1 python to launch
      * JOBCfgTemplate2: name of the step2 python to launch
      * LISTOFSamples: directory of input GEN-SIM
      * OUTPUTSAVEPath: path of the output files (EOS directories as default)
      * OUTPUTFILEName: name of the output file.root
      * EXEName1: name of the step1 job executable
      * EXEName2: name of the step2 job executable
      * SCRAM_ARCH: scram_arch
      * X509_USER_PROXY: path of the proxy (before launching better do: export X509_USER_PROXY=/afs/cern.ch/work//XXX/x509up_XXX)
      * QUEUE: lxbatch queue
      * JOBModulo: number of root files to read per job
      
    * NOTE1: Any new DIGI-RECO step1 cfg (different MC production campaign) can be adapted in order to work with this package. Change/add the folowing lines
    
      * Be careful that the "process.mix.input.fileNames = cms.untracked.vstring([])" string of PU minimum bias sample is long enough, so you don't use the same PU for each job
  
      ...
      ```Pythonscript
      process.source = cms.Source("PoolSource",
         dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
         fileNames = cms.untracked.vstring([LISTOFFILES]),
         inputCommands = cms.untracked.vstring('keep *', 
            'drop *_genParticles_*_*', 
            'drop *_genParticlesForJets_*_*', 
            'drop *_kt4GenJets_*_*', 
            'drop *_kt6GenJets_*_*', 
            'drop *_iterativeCone5GenJets_*_*', 
            'drop *_ak4GenJets_*_*', 
            'drop *_ak7GenJets_*_*', 
            'drop *_ak8GenJets_*_*', 
            'drop *_ak4GenJetsNoNu_*_*', 
            'drop *_ak8GenJetsNoNu_*_*', 
            'drop *_genCandidatesForMET_*_*', 
            'drop *_genParticlesForMETAllVisible_*_*', 
            'drop *_genMetCalo_*_*', 
            'drop *_genMetCaloAndNonPrompt_*_*', 
            'drop *_genMetTrue_*_*', 
            'drop *_genMetIC5GenJs_*_*'),
         secondaryFileNames = cms.untracked.vstring()
      )
      ```
      ...
      ```Pythonscript
      
      process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
         compressionAlgorithm = cms.untracked.string('LZMA'),
         compressionLevel = cms.untracked.int32(9),
         dataset = cms.untracked.PSet(
            dataTier = cms.untracked.string('GEN-SIM-RAW'),
            filterName = cms.untracked.string('')
         ),
         eventAutoFlushCompressedSize = cms.untracked.int32(20971520),
         fileName = cms.untracked.string('file:step0.root'),
         outputCommands = process.RAWSIMEventContent.outputCommands,
         splitLevel = cms.untracked.int32(0)
      )
      ```
      ...
      ```Pythonscript
     
      import random
      process.RandomNumberGeneratorService.generator.initialSeed = cms.untracked.uint32(SEED)
      random.seed(process.RandomNumberGeneratorService.generator.initialSeed.value())
      random.shuffle(process.mix.input.fileNames)
      ```
      
    * NOTE2: Any new DIGI-RECO step2 cfg can be adapted in order to work with this package. Change/add the folowing lines
    
      * Input name the same as step1 output
      
      ```Pythonscript
      process.source = cms.Source("PoolSource",
            fileNames = cms.untracked.vstring('file:step0.root'),
            secondaryFileNames = cms.untracked.vstring()
      )
      ```
      ...
      ```Pythonscript
    
      process.MINIAODSIMoutput = cms.OutputModule("PoolOutputModule",
            compressionAlgorithm = cms.untracked.string('LZMA'),
            compressionLevel = cms.untracked.int32(4),
            dataset = cms.untracked.PSet(
               dataTier = cms.untracked.string('MINIAODSIM'),
               filterName = cms.untracked.string('')
            ),
            dropMetaData = cms.untracked.string('ALL'),
            eventAutoFlushCompressedSize = cms.untracked.int32(-900),
            fastCloning = cms.untracked.bool(False),
            fileName = cms.untracked.string('file:OUTPUTFile.root'),
            ...
      ```
      ...

4) Publish samples (whichever release):

    * scram project CMSSW_9_4_0
    * cd CMSSW_9_4_0/src/
    * cmsenv
    * git clone  git@github.com:bmarzocc/Generation.git
    * cd Generation
    * scram b -j 5
    * cd ProduceDIGIRECO/lxbatch/
    * Mody accordingly and submit on crab3: crab_cfg_publish.py
