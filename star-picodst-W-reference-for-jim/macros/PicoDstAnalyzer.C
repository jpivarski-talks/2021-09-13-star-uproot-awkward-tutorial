/**
 * \brief Example of how to read a file (list of files) using StPicoEvent classes
 *
 * RunPicoDstAnalyzer.C is an example of reading STAR picoDst format.
 * One can use either picoDst file or a list of picoDst files (inFile.lis or
 * inFile.list) as an input, and preform physics analysis
 *
 * When using ROOT5, one needs to run RunAnalyzer.C macro when run processing.
 * This will handle libraries loading, etc.
 *
 * \author Grigory Nigmatkulov
 * \date May 29, 2018
 * \email nigmatkulov@gmail.com
 */

// This is needed for calling standalone classes (not needed on RACF)
#define _VANILLA_ROOT_

// C++ headers
#include <iostream>

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TBrowser.h"

// PicoDst headers
#include "../StPicoDstReader.h"
#include "../StPicoDst.h"
#include "../StPicoEvent.h"
#include "../StPicoTrack.h"
#include "../StPicoBTofHit.h"
#include "../StPicoBTowHit.h"
#include "../StPicoEmcTrigger.h"
#include "../StPicoBTofPidTraits.h"
#include "../StPicoBEmcPidTraits.h"

// Load libraries (for ROOT_VERSTION_CODE >= 393215)
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) 
R__LOAD_LIBRARY(../libStPicoDst)
#endif

// inFile - is a name of name.picoDst.root file or a name
//          of a name.lis(t) files that contains a list of
//          name1.picoDst.root files

//_________________
void PicoDstAnalyzer(const Char_t *inFile = "../files/st_physics_12126101_raw_3040006.picoDst.root") {

  std::cout << "Hi! Lets do some physics, Master!" << std::endl;
  
  StPicoDstReader* picoReader = new StPicoDstReader(inFile);
  picoReader->Init();

  //Long64_t events2read = picoReader->chain()->GetEntries();
  
  // This is a way if you want to spead up IO
  std::cout << "Explicit read status for some branches" << std::endl;
  picoReader->SetStatus("*",1);
  picoReader->SetStatus("Event",1);
  picoReader->SetStatus("Track",1);
  picoReader->SetStatus("BTofHit",1);
  picoReader->SetStatus("BTofPidTraits",1);
  picoReader->SetStatus("BEmcPidTraits",1);
  //picoReader->SetStatus("BTowHit",0);
  //picoReader->SetStatus("EmcTrigger",0);
  //picoReader->SetStatus("TrackCovMatrix",1);
  std::cout << "Status has been set" << std::endl;

  std::cout << "Now I know what to read, Master!" << std::endl;

  if( !picoReader->chain() ) {
    std::cout << "No chain has been found." << std::endl;
  }
  Long64_t eventsInTree = picoReader->tree()->GetEntries();
  std::cout << "eventsInTree: "  << eventsInTree << std::endl;
  Long64_t events2read = picoReader->chain()->GetEntries();

  std::cout << "Number of events to read: " << events2read
            << std::endl;

  // Histogramming
  // Event
  TH1F *hRefMult = new TH1F("hRefMult",
                            "Reference multiplicity;refMult",
                            500, -0.5, 499.5);
  TH2F *hVtxXvsY = new TH2F("hVtxXvsY",
                            "hVtxXvsY",
                            200,-10.,10.,200,-10.,10.);
  TH1F *hVtxZ = new TH1F("hVtxZ","hVtxZ",
                         140, -70., 70.);

  // Track
  TH1F *hGlobalPtot = new TH1F("hGlobalPtot",
                               "Global track momentum;p (GeV/c)",
                               100, 0., 50. );
  TH1F *hGlobalPtotCut = new TH1F("hGlobalPtotCut",
                                  "Global track momentum after cut;p (GeV/c)",
                                  100, 0., 50. );
  TH1F *hPrimaryPtot = new TH1F("hPrimaryPtot",
                                "Primary track momentum;p (GeV/c)",
                                100, 0., 50. );
  TH1F *hPrimaryPtotCut = new TH1F("hPrimaryPtotCut",
                                   "Primary track momentum after cut;p (GeV/c)",
                                   100, 0., 50. );
  TH1F *hTransvMomentum = new TH1F("hTransvMomentum",
                                   "Track transverse momentum;p_{T} (GeV/c)",
                                   200, 0., 50.);
  TH1F *hPtMax1 = new TH1F("hPtMax1",
                                   ";p_{Tmax} (GeV/c)",
                                   200, 0., 50.);
  TH1F *hPtMax2 = new TH1F("hPtMax2",
                                   ";p_{Tmax} (GeV/c)",
                                   200, 0., 50.);
  TH1F *hM = new TH1F("hM",
                                   ";M (GeV/c)",
                                   200, 0., 100.);
  TH2F *hGlobalPhiVsPt[2];
  for(int i=0; i<2; i++) {
    hGlobalPhiVsPt[i] = new TH2F(Form("hGlobalPhiVsPt_%d",i),
                                 Form("#phi vs. p_{T} for charge: %d;p_{T} (GeV/c);#phi (rad)", (i==0) ? 1 : -1),
                                 300, 0., 3.,
                                 630, -3.15, 3.15);
  }
  TH1F *hNSigmaPion = new TH1F("hNSigmaPion",
                               "n#sigma(#pi);n#sigma(#pi)",
                               400, -10., 10.);
  TH1F *hNSigmaPionZ = new TH1F("hNSigmaPionZ",
                               "n#sigma(#pi);n#sigma(#pi)",
                               400, -10., 10.);
  TH1F *hNSigmaElectron = new TH1F("hNSigmaElectron",
                                   "n#sigma(e);n#sigma(e)",
                                   400,-10.,10.);
  TH1F *hNSigmaElectronZ = new TH1F("hNSigmaElectronZ",
                                   "n#sigma(e);n#sigma(e)",
                                   400,-10.,10.);
  TH1F *hNSigmaKaon = new TH1F("hNSigmaKaon",
                               "n#sigma(K);n#sigma(K)",
                               400, -10., 10.);
  TH1F *hNSigmaKaonZ = new TH1F("hNSigmaKaonZ",
                               "n#sigma(K);n#sigma(K)",
                               400, -10., 10.);
  TH1F *hNSigmaProton = new TH1F("hNSigmaProton",
                                 "n#sigma(p);n#sigma(p)",
                                 400, -10., 10.);
  TH1F *hNSigmaProtonZ = new TH1F("hNSigmaProtonZ",
                                 "n#sigma(p);n#sigma(p)",
                                 400, -10., 10.);

  TH2F *hPt_vs_TofBeta = new TH2F("hPt_vs_TofBeta",
                                   ";p_{T};#beta",
                                   160, -2., 2.,
                                   200, 0., 2.);
  TH2F *hPtot_vs_TofBeta = new TH2F("hPtot_vs_TofBeta",
                                   ";p_{T};#beta",
                                   160, -2., 2.,
                                   200, 0., 2.);
  TH2F *hPt_vs_TofBetaInv = new TH2F("hPt_vs_TofBetaInv",
                                   ";p_{T};#beta",
                                   160, -2., 2.,
                                   200, 0., 2.);
  TH2F *hPt_vs_TofMass = new TH2F("hPt_vs_TofMass",
                                   ";p_{T};m=p_{T} (#beta^{-2} - 1)^{0.5}",
                                   160, -50., 50.,
                                   200, 0., 2.);
  TH2F *hNSigmaElectron_vs_TofBeta = new TH2F("hNSigmaElectron_vs_TofBeta",
                                   ";n#sigma(e);#beta",
                                   100,-10.,10.,
                                   500, 0., 2.);
  TH2F *hNSigmaElectron_vs_TofMass = new TH2F("hNSigmaElectron_vs_TofMass",
                                   ";n#sigma(e);m=p_{T} (#beta^{-2} - 1)^{0.5}",
                                   100,-10.,10.,
                                   200, 0., 2.);
  TH2F *hNSigmaElectron_vs_TofMassZ = new TH2F("hNSigmaElectron_vs_TofMassZ",
                                   ";n#sigma(e);m=p_{T} (#beta^{-2} - 1)^{0.5}",
                                   100,-10.,10.,
                                   200, 0., 2.);

  TH2F *hPt_vs_BtowE = new TH2F("hPt_vs_BtowE",
                                   ";p_{T};E1",
                                   80, 0., 50.,
                                   80, 0., 50.);
  TH2F *hPt_vs_BtowE2 = new TH2F("hPt_vs_BtowE2",
                                   ";p_{T};E2",
                                   80, 0., 50.,
                                   80, 0., 50.);
  TH2F *hPt_vs_BtowE3 = new TH2F("hPt_vs_BtowE3",
                                   ";p_{T};E3",
                                   80, 0., 50.,
                                   80, 0., 50.);
    
  // TofPidTrait
  TH1F *hTofBeta = new TH1F("hTofBeta",
                            "BTofPidTraits #beta;#beta",
                            2000, 0., 2.);

  // TofHit
  TH1F *hBTofTrayHit = new TH1F("hBTofTrayHit","BTof tray number with the hit",
                                120, -0.5, 119.5);


  // Loop over events
  for(Long64_t iEvent=0; iEvent<events2read; iEvent++) {

    std::cout << "Working on event #[" << (iEvent+1)
              << "/" << events2read << "]" << std::endl;

    Bool_t readEvent = picoReader->readPicoEvent(iEvent);
    if( !readEvent ) {
      std::cout << "Something went wrong, Master! Nothing to analyze..."
                << std::endl;
      break;
    }

    // Retrieve picoDst
    StPicoDst *dst = picoReader->picoDst();

    // Retrieve event information
    StPicoEvent *event = dst->event();
    if( !event ) {
      std::cout << "Something went wrong, Master! Event is hiding from me..."
                << std::endl;
      break;
    }

    //if (event->ranking() < 1e6) continue;

    hRefMult->Fill( event->refMult() );

    TVector3 pVtx = event->primaryVertex();
    hVtxXvsY->Fill( event->primaryVertex().X(), event->primaryVertex().Y() );
    hVtxZ->Fill( event->primaryVertex().Z() );

    // Track analysis
    Int_t nTracks = dst->numberOfTracks();

    StPicoTrack *max1 = 0, *max2 = 0;
    
    // Track loop
    for(Int_t iTrk=0; iTrk<nTracks; iTrk++) {

      // Retrieve i-th pico track
      StPicoTrack *picoTrack = dst->track(iTrk);

    for(Int_t iTrk=0; iTrk<nTracks; iTrk++) {

      // Retrieve i-th pico track
      //StPicoTrack *picoTrack2 = dst->track(iTrk);
#define SQR(x) ((x) * (x))
      //double M = sqrt(SQR(picoTrack->pMom().Mag() + picoTrack2->pMom().Mag()) - SQR((picoTrack->pMom() + picoTrack2->pMom()).Mag()));
    }

    if (picoTrack->isBemcTrack()) {
      StPicoBEmcPidTraits *trait = dst->bemcPidTraits( picoTrack->bemcPidTraitsIndex() );

      hPt_vs_BtowE->Fill(picoTrack->pMom().Pt(), trait->btowE());
      hPt_vs_BtowE2->Fill(picoTrack->pMom().Pt(), trait->btowE2());
      hPt_vs_BtowE3->Fill(picoTrack->pMom().Pt(), trait->btowE3());

      double r = picoTrack->pMom().Pt() / trait->btowE();

      if ((r > 0.666) && (r < 1.333)) {
      if (max1 && (max1->pMom().Mag() < picoTrack->pMom().Pt())) {
        max2 = max1;
        max1 = picoTrack;
      } else if (max2 && (max2->pMom().Mag() < picoTrack->pMom().Pt())) {
        max2 = picoTrack;
      }
      if (!max1) {
        max1 = picoTrack;
      } else if (!max2) {
        max2 = picoTrack;
      }
      }
    }

      hGlobalPtot->Fill( picoTrack->gMom().Mag() );
      if( picoTrack->isPrimary() ) {
        hPrimaryPtot->Fill( picoTrack->pMom().Mag() );
      }
      
      // Simple single-track cut
      if( picoTrack->gMom().Mag() < 0.1 ||
          picoTrack->gDCA(pVtx).Mag()>50. ) {
        continue;
      } 

      hGlobalPtotCut->Fill( picoTrack->gMom().Mag() );
      if( picoTrack->isPrimary() ) {
        hPrimaryPtotCut->Fill( picoTrack->pMom().Mag() );
      }
      if( picoTrack->charge() > 0 ) {
        hGlobalPhiVsPt[0]->Fill( picoTrack->gMom().Pt(),
                                 picoTrack->gMom().Phi() );
      }
      else {
        hGlobalPhiVsPt[1]->Fill( picoTrack->gMom().Pt(),
                                 picoTrack->gMom().Phi() );	
      }
      hNSigmaElectron->Fill( picoTrack->nSigmaElectron() );
      hNSigmaPion->Fill( picoTrack->nSigmaPion() );
      hNSigmaKaon->Fill( picoTrack->nSigmaKaon() );
      hNSigmaProton->Fill( picoTrack->nSigmaProton() );
      
      hTransvMomentum->Fill( picoTrack->gMom().Pt() );

      // Check if track has TOF signal
      if( picoTrack->isTofTrack() ) {
        // Retrieve corresponding trait
        StPicoBTofPidTraits *trait = dst->btofPidTraits( picoTrack->bTofPidTraitsIndex() );
        if( !trait ) {
          std::cout << "O-oh... No BTofPidTrait # " << picoTrack->bTofPidTraitsIndex()
                    << " for track # " << iTrk << std::endl;
          std::cout << "Check that you turned on the branch!" << std::endl;
          continue;
        }
        // Fill beta
        hTofBeta->Fill( trait->btofBeta() );

      if( picoTrack->isPrimary() ) {
        hPt_vs_TofBeta->Fill(picoTrack->pMom().Pt() * picoTrack->charge(), trait->btofBeta());
        hPtot_vs_TofBeta->Fill(picoTrack->pMom().Mag() * picoTrack->charge(), trait->btofBeta());
        hNSigmaElectron_vs_TofBeta->Fill(picoTrack->nSigmaElectron(), trait->btofBeta());

        double tof_mass = picoTrack->pMom().Mag() * sqrt(1 / trait->btofBeta() / trait->btofBeta() - 1);
        hPt_vs_TofMass->Fill(picoTrack->pMom().Mag() * picoTrack->charge(), tof_mass);
        hNSigmaElectron_vs_TofMass->Fill(picoTrack->nSigmaElectron(), tof_mass);
      }
      } //if( isTofTrack() )
      
    } //for(Int_t iTrk=0; iTrk<nTracks; iTrk++)

    if (max1) hPtMax1->Fill(max1->pMom().Mag());
    if (max2) hPtMax2->Fill(max2->pMom().Mag());
    if (max1 && max2) {
      StPicoBEmcPidTraits *trait1 = dst->bemcPidTraits( max1->bemcPidTraitsIndex() );
      StPicoBEmcPidTraits *trait2 = dst->bemcPidTraits( max2->bemcPidTraitsIndex() );

      //double M = max1->pMom().Mag() + max2->pMom().Mag();
      // Rescaling by E/pt doesn't work as E has some cut above
      auto p1 = max1->pMom();// * (trait1->btowE() / max1->pMom().Mag());
      auto p2 = max2->pMom();// * (trait2->btowE() / max2->pMom().Mag());

      double M = sqrt(SQR(p1.Mag() + p2.Mag()) - SQR((p1 + p2).Mag()));
      std::cerr << max1 << "\t" << max2 << std::endl;
      p1.Print();
      p2.Print();
      std::cerr << SQR(p1.Mag() + p2.Mag()) << "\t" << SQR((p1 + p2).Mag()) << "\t" << M << std::endl;

      hM->Fill(M);
      if (M > 80) {
        hNSigmaElectronZ->Fill( max1->nSigmaElectron() );
        hNSigmaElectronZ->Fill( max2->nSigmaElectron() );
        hNSigmaPionZ->Fill( max1->nSigmaPion() );
        hNSigmaPionZ->Fill( max2->nSigmaPion() );
        hNSigmaKaonZ->Fill( max1->nSigmaKaon() );
        hNSigmaKaonZ->Fill( max2->nSigmaKaon() );
        hNSigmaProtonZ->Fill( max1->nSigmaProton() );
        hNSigmaProtonZ->Fill( max2->nSigmaProton() );
#define TTT(picoTrack) \
      if( picoTrack->isTofTrack() ) { \
        StPicoBTofPidTraits *trait = dst->btofPidTraits( picoTrack->bTofPidTraitsIndex() ); \
      if( picoTrack->isPrimary() ) { \
        double tof_mass = picoTrack->pMom().Mag() * sqrt(1 / trait->btofBeta() / trait->btofBeta() - 1); \
        hNSigmaElectron_vs_TofMassZ->Fill(picoTrack->nSigmaElectron(), tof_mass); \
      } \
      }

        TTT(max1);
        TTT(max2)

      }
    }

    // Hit analysis
    Int_t nBTofHits = dst->numberOfBTofHits();
    //std::cout << "Number of btofHits in event: " << nBTofHits << std::endl;

    // Hit loop
    for(Int_t iHit=0; iHit<nBTofHits; iHit++) {

      StPicoBTofHit *btofHit = dst->btofHit(iHit);
      if( !btofHit ) continue;
      hBTofTrayHit->Fill( btofHit->tray() );
    } //for(Int_t iHit=0; iHit<nBTofHits; iHit++)



  } //for(Long64_t iEvent=0; iEvent<events2read; iEvent++)

  picoReader->Finish();

  std::cout << "I'm done with analysis. We'll have a Nobel Prize, Master!"
            << std::endl;

  new TBrowser();
}
