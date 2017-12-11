import FWCore.ParameterSet.Config as cms

process = cms.Process("PUBLISH")

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/user/bmarzocc/ttHToGX_RunIIFall17/MiniAODSIM/ttHToGX_M_125_TuneCUEP8M1_13TeV_pythia8_RunIIFall17_MiniAODSIM_1.root'))

# Output definition
process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
                                         splitLevel = cms.untracked.int32(0),
                                         outputCommands = cms.untracked.vstring("keep *"),
                                         fileName = cms.untracked.string("PUBLISH_ttHToGX_M_125_TuneCUEP8M1_13TeV_pythia8_RunIIFall17_MiniAODSIM_1.root")
)

process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)
process.schedule = cms.Schedule(process.RECOSIMoutput_step)
