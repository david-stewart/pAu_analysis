
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/BackgroundEstimatorBase.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include "fastjet/tools/Filter.hh"
#include "fastjet/tools/Pruner.hh"

#include "TROOT.h"
#include "TRandom.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TH3.h"
#include "TClonesArray.h"
#include "TLatex.h"
#include "TMathText.h"
#include "TProfile.h"

// TStarJetPico
#include "TStarJetPicoReader.h"
#include "TStarJetPicoEvent.h"
#include "TStarJetPicoEventHeader.h"
#include "TStarJetPicoEventCuts.h"
#include "TStarJetPicoPrimaryTrack.h"
#include "TStarJetPicoTower.h"
#include "TStarJetPicoTrackCuts.h"
#include "TStarJetPicoTowerCuts.h"
#include "TStarJetVectorContainer.h"
#include "TStarJetVector.h"
#include "TStarJetPicoTriggerInfo.h"
#include "TStarJetPicoUtils.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "bemc_helper.h"

#ifndef pAuFunctions_hh
#define pAuFunctions_hh

namespace pAuAnalysis {

  const int RefMultCut = 0;
  const int MinNFitPointsCut = 20;

  const double vzCut = 30.0;   // |Vz|<=30 cm      // changed!
  const double VertexZDiffCut = 3.0;  // changed!
  const double MaxEventPtCut = 30.0;
  const double MaxEventEtCut = 30.0;
  const double MinEventEtCut = 0.2;
  const double DCACut = 1.0;
  const double FitOverMaxPointsCut = 0.52;
  const double MaxPtCut = 30.0;
  const double MaxEtCut = 30.0;
  
  const double R = 0.4;
  const double VertexZCut = vzCut;
  const double etaCut = 1.0;       // |eta|<=1
  const double partMinPt = 0.2;      //  particle Pt >= 0.2 GeV
  const double jetMinPt = 2.0;      //  Jet Pt >= 2.0 GeV
  const double qpi = 3.141592653589793238462643383279502884197/4;

  const double AREA = 4*(fastjet::pi - 2);   // (  2 in eta  ) X (  2*( pi-1 - 1 ) in phi  )

  void ApplyTrackingEfficiency( std::vector<fastjet::PseudoJet> chgPart, std::string efficiencyFile );  //BAD!!
  
  void BackGroundEstimationAndPlots( std::vector<fastjet::PseudoJet> chgPart, std::vector<fastjet::PseudoJet> neuPart, fastjet::PseudoJet leadJet,
				     TH3D *PartPtDEtaDPhi, TH3D *PartPtEtaPhi, TH3D *UE, double &chgSum, double &neuSum );

  void CalculateUEsubtractedChargedRho( std::vector<fastjet::PseudoJet> chgPart, double &chgEast_Sum, double &chgMid_Sum, double &chgWest_Sum );
  
  void CalculateRhoByChargeAndEta( std::vector<fastjet::PseudoJet> chgPart, std::vector<fastjet::PseudoJet> neuPart, double &chgEast_Sum, double &chgMid_Sum, double &chgWest_Sum, double &neuEast_Sum, double &neuMid_Sum, double &neuWest_Sum );
  
  int CountTowers( TList *selectedtowers );

  std::vector<fastjet::PseudoJet> GatherBackground ( fastjet::PseudoJet trigJet, TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & ueParticles );

  std::vector<fastjet::PseudoJet> GatherCharged ( TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & rawParticles );

  std::vector<fastjet::PseudoJet> GatherChargedUEwithEfficiency ( fastjet::PseudoJet trigJet, TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & chgParticles,  std::string efficiencyFile );
  
  std::vector<fastjet::PseudoJet> GatherNeutral ( TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & rawParticles );
    
  std::vector<fastjet::PseudoJet> GatherParticles ( TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & rawParticles );

  std::vector<fastjet::PseudoJet> GatherChargedUE (  fastjet::PseudoJet trigJet, TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & chgParticles );

  std::vector<fastjet::PseudoJet> GatherNeutralUE (  fastjet::PseudoJet trigJet, TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & newParticles );

  void GetHeaderInfo( TStarJetPicoEventHeader* Header, int &Nglobal, int &Nvertices, int &ref_mult, int &Nprimary, double &BBC_CoincidenceRate,
		      double &vpdVz, double &BBC_EastRate, double &BBC_WestRate, double &BBC_AdcSumEast );

  void InitReader( TStarJetPicoReader & reader, TChain* chain, int nEvents );

  double UEsubtraction( fastjet::PseudoJet leadjet, std::string UEcorrFile, double BBCEsum );
  
  bool UseHTevent( TStarJetPicoEventHeader* Header, TStarJetPicoEvent* Event, double vz_cut, double vz );

  bool UseMBevent( TStarJetPicoEventHeader* Header, TStarJetPicoEvent* Event, double vz_cut, double vz );

  bool UseTriggerTower( int TriggerTowerId );

}

#endif
