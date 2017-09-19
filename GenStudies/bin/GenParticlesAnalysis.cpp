// system include files
#include <memory>
#include <iostream>
#include <fstream>

#include "FWCore/FWLite/interface/FWLiteEnabler.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "PhysicsTools/Utilities/macros/setTDRStyle.C"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDConsumerBase.h"

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/LooperFactory.h"
#include "FWCore/Framework/interface/ESProducerLooper.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESProducts.h"
#include "FWCore/Framework/interface/Event.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"

#include "FPCanvasStyle.C"
#include "setStyle.C"

#include "TSystem.h"
#include "TFile.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TProfile2D.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <set>
#include <assert.h>
#include <time.h>

#include <TMath.h>
#include <Math/VectorUtil.h>

using namespace std;

void chooseParticles(vector<string>& genParticles_, vector<string>& name, vector<int>& pdgId, vector<int>& status, vector<int>& mother)
{
     for(unsigned int iPart = 0; iPart < genParticles_.size(); iPart++ ) {

         istringstream splitter(genParticles_.at(iPart));
         vector<string> tokens{istream_iterator<string>{splitter},istream_iterator<string>{}};
         name.push_back(tokens.at(0));
         if( tokens.size() > 1 )
         { pdgId.push_back(stoi(tokens.at(1))); }
         if( tokens.size() > 2 )
         { status.push_back(stoi(tokens.at(2))); }
         if( tokens.size() > 3 )
         { mother.push_back(stoi(tokens.at(3))); }
     }
}

void setVariables(vector<string>& genVariables_, vector<string>& var, vector<int>& nbins, vector<float>& min, vector<float>& max)
{
     for(unsigned int iVar = 0; iVar < genVariables_.size(); iVar++ ) {

         istringstream splitter(genVariables_.at(iVar));
         vector<string> tokens{istream_iterator<string>{splitter},istream_iterator<string>{}};
         var.push_back(tokens.at(0));
         if( tokens.size() > 1 )
         { nbins.push_back(stoi(tokens.at(1))); }
         if( tokens.size() > 2 )
         { min.push_back(stof(tokens.at(2))); }
         if( tokens.size() > 3 )
         { max.push_back(stof(tokens.at(3))); }
     }
}

void drawHisto(TH1F* h, string unit = "GeV", string outputDir = "output")
{
     h->SetLineColor(kBlack);
     h->SetLineWidth(2);
     
     // plotting
     setStyle();

     int nbins = h->GetNbinsX();
     float xmin = h->GetBinCenter(1)-h->GetBinWidth(1)/2;
     float xmax = h->GetBinCenter(nbins)+h->GetBinWidth(nbins)/2;
     float ymax = 1.1*h->GetBinContent(h->GetMaximumBin());
     TH2F* H2 = new TH2F("H2","",nbins,xmin,xmax,1000,0.,ymax);
     H2->GetXaxis()->SetTitle((string(h->GetName())+" ("+unit+")").c_str());
     H2->GetYaxis()->SetTitle("events");
     
     TCanvas* c1 = new TCanvas("c1","c1",1);
     FPCanvasStyle(c1);
     H2->Draw();
     h->Draw("H,same");
     c1->SaveAs((outputDir+"/"+string(h->GetName())+".png").c_str());
     c1->SaveAs((outputDir+"/"+string(h->GetName())+".pdf").c_str());

     TCanvas* c2 = new TCanvas("c2","c2",1);
     FPCanvasStyle(c2);
     c2->SetLogy();
     H2->Draw();
     h->Draw("H,same");
     c2->SaveAs((outputDir+"/"+string(h->GetName())+"_log.png").c_str());
     c2->SaveAs((outputDir+"/"+string(h->GetName())+"_log.pdf").c_str());

     delete c1;
     delete c2;
     delete H2;
}

int main(int argc, char** argv)
{

    gSystem->Load( "libFWCoreFWLite" );
    FWLiteEnabler::enable();

    if( argc < 2 ) {
        cout << "Usage : " << argv[0] << " [parameters.py]" << endl;
        return 0;
    }
    if( !edm::readPSetsFrom( argv[1] )->existsAs<edm::ParameterSet>( "process" ) ) {
        cout << " ERROR: ParametersSet 'process' is missing in your configuration file"
             << endl;
        return 0;
    }

    const edm::ParameterSet &process         = edm::readPSetsFrom( argv[1] )->getParameter<edm::ParameterSet>( "process" );
    const edm::ParameterSet &filesOpt        = process.getParameter<edm::ParameterSet>( "ioFilesOpt" );
    const edm::ParameterSet &genParticlesOpt = process.getParameter<edm::ParameterSet>( "genParticlesOpt" );
    const edm::ParameterSet &genJetOpt       = process.getParameter<edm::ParameterSet>( "genJetOpt" );
    const edm::ParameterSet &genMETOpt       = process.getParameter<edm::ParameterSet>( "genMETOpt" );

   //Read config
    vector<string> inputFiles_ = filesOpt.getParameter< vector<string> >( "inputFiles" );

    string outputDir_ = filesOpt.getParameter<string>( "outputDir" );
    if( outputDir_ == "" ) outputDir_ = "output/"; 
   
    string outputFile_ = filesOpt.getParameter<string>( "outputFile" );
    if( outputFile_ == "" ) outputFile_ = "GenParticlesAnalysis.root"; 

    TFile* outFile = new TFile((outputDir_+"/"+outputFile_).c_str(),"RECREATE");
    
    int maxEvents_ = filesOpt.getUntrackedParameter<int>( "maxEvents" );
    int ievt = 0; 
    
    vector<string> genParticles_ = genParticlesOpt.getParameter< vector<string> >( "genParticles" );
    vector<string> name_;
    vector<int> pdgId_;
    vector<int> status_;
    vector<int> mother_;
    chooseParticles(genParticles_, name_, pdgId_, status_, mother_);
    
    vector<string> genVariables_ = genParticlesOpt.getParameter< vector<string> >( "genVariables" );
    vector<string> genVar_;
    vector<int> genNbins_;
    vector<float> genMin_; 
    vector<float> genMax_;
    setVariables(genVariables_, genVar_, genNbins_, genMin_, genMax_);

    vector<string> jetTypes_ = genJetOpt.getParameter< vector<string> >( "jetTypes" );
    vector<string> jetVariables_ = genJetOpt.getParameter< vector<string> >( "jetVariables" );
    vector<string> jetVar_;
    vector<int> jetNbins_;
    vector<float> jetMin_; 
    vector<float> jetMax_;
    setVariables(jetVariables_, jetVar_, jetNbins_, jetMin_, jetMax_);

    vector<string> metTypes_ = genMETOpt.getParameter< vector<string> >( "metTypes" );
    vector<string> metVariables_ = genMETOpt.getParameter< vector<string> >( "metVariables" );
    vector<string> metVar_;
    vector<int> metNbins_;
    vector<float> metMin_; 
    vector<float> metMax_;
    setVariables(metVariables_, metVar_, metNbins_, metMin_, metMax_);

    //Set genHistos
    vector< vector<TH1F*> > genHistos;
    genHistos.resize(genParticles_.size());
    for(unsigned int iPart=0; iPart<genParticles_.size(); iPart++)
        genHistos[iPart].resize(genVariables_.size());

    for(unsigned int iPart=0; iPart<genParticles_.size(); iPart++)
        for(unsigned int iVar=0; iVar<genVariables_.size(); iVar++)
            genHistos.at(iPart).at(iVar) = new TH1F((name_.at(iPart)+"_"+genVar_.at(iVar)).c_str(),"",genNbins_.at(iVar),genMin_.at(iVar),genMax_.at(iVar));
        
    //Set metHistos
    vector< vector<TH1F*> > metHistos;
    metHistos.resize(metTypes_.size());
    for(unsigned int iMet=0; iMet<metTypes_.size(); iMet++)
        metHistos[iMet].resize(metVariables_.size());

    for(unsigned int iMet=0; iMet<metTypes_.size(); iMet++)
        for(unsigned int iVar=0; iVar<metVariables_.size(); iVar++)
            metHistos.at(iMet).at(iVar) = new TH1F((metTypes_.at(iMet)+"_"+metVar_.at(iVar)).c_str(),"",metNbins_.at(iVar),metMin_.at(iVar),metMax_.at(iVar));
    
    //Set jetHistos
    vector< vector<TH1F*> > jetHistos;
    jetHistos.resize(jetTypes_.size());
    for(unsigned int iJet=0; iJet<jetTypes_.size(); iJet++)
        jetHistos[iJet].resize(jetVariables_.size());

    for(unsigned int iJet=0; iJet<jetTypes_.size(); iJet++)
        for(unsigned int iVar=0; iVar<jetVariables_.size(); iVar++)
            jetHistos.at(iJet).at(iVar) = new TH1F((jetTypes_.at(iJet)+"_"+jetVar_.at(iVar)).c_str(),"",jetNbins_.at(iVar),jetMin_.at(iVar),jetMax_.at(iVar));
    
    
    cout << "--- Reading entries ---" << endl;
    for(unsigned int iFile=0; iFile<inputFiles_.size(); ++iFile){

        TFile* inFile = TFile::Open(inputFiles_[iFile].c_str());
        if(inFile){

           fwlite::Event ev(inFile);   
           for(ev.toBegin(); !ev.atEnd(); ++ev, ++ievt){
                      
               edm::EventBase const & event = ev;
	       if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;

               if(ievt%1000==0) cout << "--- Reading entry = " << ievt << endl;

               // Handle to the GenParticle collection
	       edm::Handle< vector<reco::GenParticle> > genParticles;
	       event.getByLabel(string("genParticles"), genParticles);
               
               if(ievt%1000==0) cout << "--- Reading genParticles ---" << endl;
               for(vector<reco::GenParticle>::const_iterator part=genParticles->begin(); part!=genParticles->end(); ++part)    
                   for(unsigned int iPart = 0; iPart<genParticles_.size(); iPart++)
                   {
                       if(part->pdgId() != pdgId_[iPart] || part->status() != status_[iPart] || (mother_[iPart] != 0 && part->mother()->pdgId() != mother_[iPart])) continue;
                       for(unsigned int iVar=0; iVar<genVariables_.size(); iVar++)
                       {
                           if(genVar_.at(iVar) == "mass") genHistos[iPart][iVar]->Fill(part->mass()); 
                           if(genVar_.at(iVar) == "mt") genHistos[iPart][iVar]->Fill(part->mt());  
                           if(genVar_.at(iVar) == "energy") genHistos[iPart][iVar]->Fill(part->energy());  
                           if(genVar_.at(iVar) == "et") genHistos[iPart][iVar]->Fill(part->et());
                           if(genVar_.at(iVar) == "et2") genHistos[iPart][iVar]->Fill(part->et2());  
                           if(genVar_.at(iVar) == "p") genHistos[iPart][iVar]->Fill(part->p());  
                           if(genVar_.at(iVar) == "px") genHistos[iPart][iVar]->Fill(part->px()); 
                           if(genVar_.at(iVar) == "py") genHistos[iPart][iVar]->Fill(part->py()); 
                           if(genVar_.at(iVar) == "pz") genHistos[iPart][iVar]->Fill(part->pz());  
                           if(genVar_.at(iVar) == "pt") genHistos[iPart][iVar]->Fill(part->pt());
                           if(genVar_.at(iVar) == "y") genHistos[iPart][iVar]->Fill(part->y()); 
                           if(genVar_.at(iVar) == "eta") genHistos[iPart][iVar]->Fill(part->eta());
                           if(genVar_.at(iVar) == "phi") genHistos[iPart][iVar]->Fill(part->phi());  
                       } 
                   }
               
               for(unsigned int iJet = 0; iJet<jetTypes_.size(); iJet++)
               {
                   // Handle to the GenParticle collection
	           edm::Handle< vector<reco::GenJet> > genJets;
	           event.getByLabel(jetTypes_.at(iJet), genJets);
                   
                   if(ievt%1000==0) cout << "--- Reading " << jetTypes_.at(iJet) << " ---" << endl;
                   for(vector<reco::GenJet>::const_iterator jet=genJets->begin(); jet!=genJets->end(); ++jet)    
                   {
                       for(unsigned int iVar=0; iVar<jetVariables_.size(); iVar++)
                       {
                           if(jetVar_.at(iVar) == "mass") jetHistos[iJet][iVar]->Fill(jet->mass()); 
                           if(jetVar_.at(iVar) == "mt") jetHistos[iJet][iVar]->Fill(jet->mt());  
                           if(jetVar_.at(iVar) == "energy") jetHistos[iJet][iVar]->Fill(jet->energy());  
                           if(jetVar_.at(iVar) == "emEnergy") jetHistos[iJet][iVar]->Fill(jet->emEnergy());  
                           if(jetVar_.at(iVar) == "hadEnergy") jetHistos[iJet][iVar]->Fill(jet->hadEnergy());  
                           if(jetVar_.at(iVar) == "invisibleEnergy") jetHistos[iJet][iVar]->Fill(jet->invisibleEnergy());  
                           if(jetVar_.at(iVar) == "auxiliaryEnergy") jetHistos[iJet][iVar]->Fill(jet->auxiliaryEnergy());  
                           if(jetVar_.at(iVar) == "et") jetHistos[iJet][iVar]->Fill(jet->et());
                           if(jetVar_.at(iVar) == "et2") jetHistos[iJet][iVar]->Fill(jet->et2());  
                           if(jetVar_.at(iVar) == "p") jetHistos[iJet][iVar]->Fill(jet->p());  
                           if(jetVar_.at(iVar) == "px") jetHistos[iJet][iVar]->Fill(jet->px()); 
                           if(jetVar_.at(iVar) == "py") jetHistos[iJet][iVar]->Fill(jet->py()); 
                           if(jetVar_.at(iVar) == "pz") jetHistos[iJet][iVar]->Fill(jet->pz());  
                           if(jetVar_.at(iVar) == "pt") jetHistos[iJet][iVar]->Fill(jet->pt());
                           if(jetVar_.at(iVar) == "y") jetHistos[iJet][iVar]->Fill(jet->y()); 
                           if(jetVar_.at(iVar) == "eta") jetHistos[iJet][iVar]->Fill(jet->eta());
                           if(jetVar_.at(iVar) == "phi") jetHistos[iJet][iVar]->Fill(jet->phi());   
                       } 
                   }
               }
               
               for(unsigned int iMet=0; iMet<metTypes_.size(); iMet++)
               {
                   // Handle to the GenMETCollection collection 
                   edm::Handle<reco::GenMETCollection> genMet;
                   event.getByLabel(metTypes_.at(iMet), genMet); 
                   const reco::GenMETCollection *genmetcol = genMet.product();
                   const reco::GenMET  *genMetCalo = &(genmetcol->front());

                   if(ievt%1000==0) cout << "--- Reading " << metTypes_.at(iMet) << " ---" << endl;
                   for(unsigned int iVar=0; iVar<metVariables_.size(); iVar++)
                   {
                       if(metVar_.at(iVar) == "et") metHistos[iMet][iVar]->Fill(genMetCalo->et());
                       if(metVar_.at(iVar) == "pt") metHistos[iMet][iVar]->Fill(genMetCalo->pt()); 
                       if(metVar_.at(iVar) == "phi") metHistos[iMet][iVar]->Fill(genMetCalo->phi()); 
                       if(metVar_.at(iVar) == "NeutralEMEtFraction") metHistos[iMet][iVar]->Fill(genMetCalo->NeutralEMEtFraction()); 
                       if(metVar_.at(iVar) == "NeutralEMEt") metHistos[iMet][iVar]->Fill(genMetCalo->NeutralEMEt()); 
                       if(metVar_.at(iVar) == "ChargedEMEtFraction") metHistos[iMet][iVar]->Fill(genMetCalo->ChargedEMEtFraction()); 
                       if(metVar_.at(iVar) == "ChargedEMEt") metHistos[iMet][iVar]->Fill(genMetCalo->ChargedEMEt()); 
                       if(metVar_.at(iVar) == "NeutralHadEtFraction") metHistos[iMet][iVar]->Fill(genMetCalo->NeutralHadEtFraction()); 
                       if(metVar_.at(iVar) == "NeutralHadEt") metHistos[iMet][iVar]->Fill(genMetCalo->NeutralHadEt()); 
                       if(metVar_.at(iVar) == "ChargedHadEtFraction") metHistos[iMet][iVar]->Fill(genMetCalo->ChargedHadEtFraction()); 
                       if(metVar_.at(iVar) == "ChargedHadEt") metHistos[iMet][iVar]->Fill(genMetCalo->ChargedHadEt()); 
                       if(metVar_.at(iVar) == "MuonEtFraction") metHistos[iMet][iVar]->Fill(genMetCalo->MuonEtFraction()); 
                       if(metVar_.at(iVar) == "MuonEt") metHistos[iMet][iVar]->Fill(genMetCalo->MuonEt()); 
                       if(metVar_.at(iVar) == "InvisibleEtFraction") metHistos[iMet][iVar]->Fill(genMetCalo->InvisibleEtFraction()); 
                       if(metVar_.at(iVar) == "InvisibleEt") metHistos[iMet][iVar]->Fill(genMetCalo->InvisibleEt()); 
                   } 
               }
	   }   

        }
    }

    outFile->cd();
    for(unsigned int iPart = 0; iPart<genParticles_.size(); iPart++)
        for(unsigned int iVar = 0; iVar<genVariables_.size(); iVar++)
        {
            genHistos[iPart][iVar]->Write();
            drawHisto(genHistos[iPart][iVar]);
        }
    for(unsigned int iJet = 0; iJet<jetTypes_.size(); iJet++)
        for(unsigned int iVar = 0; iVar<jetVariables_.size(); iVar++)
        {
            jetHistos[iJet][iVar]->Write();
            drawHisto(jetHistos[iJet][iVar]);
        }
    for(unsigned int iMet = 0; iMet<metTypes_.size(); iMet++)
        for(unsigned int iVar = 0; iVar<metVariables_.size(); iVar++)
        {
            metHistos[iMet][iVar]->Write();
            if(string(metHistos[iMet][iVar]->GetName()).find("Fraction") !=std::string::npos) drawHisto(metHistos[iMet][iVar],"");
            else drawHisto(metHistos[iMet][iVar]);
        }
    outFile->Close();
}
