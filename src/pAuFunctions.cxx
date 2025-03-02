//  pAuFunctions.cxx
//  Veronica Verkest       May 15, 2019

#include "pAuFunctions.hh"
#include "pAu_HT_jetParameters.hh"

namespace pAuAnalysis {

  void ApplyTrackingEfficiency( std::vector<fastjet::PseudoJet> chgPart, std::string efficiencyFile ) {  // BAD!! DO NOT USE!!

    int ptBin, etaBin;
    const int nBins = 10;
    double pt, eta, phi, e, px, py, pz, effic, corrPt;
    TString prefix = "eff_s_bin_1_10_bbc__";

    double etaLo[nBins] = { -1.0, -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8 };
    double etaHi[nBins] = { -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0 };

    TH1D *hEffic[nBins];
    
    TFile *ef = new TFile( efficiencyFile.c_str(), "READ" );
    
    for ( int j=0; j<nBins; ++j ) {   // pull in efficiency histograms
      TString name = prefix; name += (j+1); name += "_"; name += (j+1); name += "_eta";
      hEffic[j] = (TH1D*)ef->Get( name );
    }
    
    std::vector<fastjet::PseudoJet> uncorrPart = chgPart;  // move uncorrected UE particles to new PJ vector
    chgPart.clear();
    
    for (int i=0; i<uncorrPart.size(); ++i) {
      if (  (uncorrPart[i].pt()<0.2) || (uncorrPart[i].pt()>15.0)  ) {
	std::cerr<<"Charged Track pT = "<<uncorrPart[i].pt()<<"   --  setting to 0.0"<<std::endl;
      }
      else {
	pt = uncorrPart[i].pt();
	eta = uncorrPart[i].eta();
	phi = uncorrPart[i].phi();
	e = uncorrPart[i].e();

	etaBin = 999;
	for ( int j=0; j<nBins; ++j ) {     // find efficiency histogram corresponding to track eta
	  if ( (eta>=etaLo[j]) && (eta<=etaHi[j]) ) { etaBin = j; continue; }
	}
	if ( etaBin==999 ) { std::cerr<<"error finding charged track eta bin; eta = "<<eta<<std::endl; continue; }
	for ( int j=0; j<72; ++j ) { ptBin = hEffic[etaBin]->FindBin( pt ); }    // find histogram bin corresponding to track pt

	effic = hEffic[etaBin]->GetBinContent( ptBin );
	corrPt = pt/effic;

	px = corrPt*cos( phi );
	py = corrPt*sin( phi );
	pz = corrPt*cosh( eta );

	fastjet::PseudoJet corrPart = fastjet::PseudoJet( px, py, pz, e );

	chgPart.push_back( corrPart );
      }
    }    
  }
  
  
  void BackGroundEstimationAndPlots( std::vector<fastjet::PseudoJet> chgPart, std::vector<fastjet::PseudoJet> neuPart, fastjet::PseudoJet leadJet,
				     TH3D *PartPtDEtaDPhi, TH3D *PartPtEtaPhi, TH3D *UE, double &chgSum, double &neuSum ) {

    chgSum = 0;
    neuSum = 0;
    
    for (int i=0; i<chgPart.size(); ++i) {
      double deltaPhi = chgPart[i].delta_phi_to( leadJet );
      double dPhi = chgPart[i].delta_phi_to( leadJet );
      double dEta = leadJet.eta() - chgPart[i].eta();
      PartPtDEtaDPhi->Fill( chgPart[i].pt(), dEta, dPhi );
      PartPtEtaPhi->Fill( leadJet.pt(), chgPart[i].eta(), chgPart[i].phi() );
      UE->Fill( chgPart[i].pt(), chgPart[i].eta(), chgPart[i].phi() );
      chgSum+=chgPart[i].pt();
    }

    for (int i=0; i<neuPart.size(); ++i) {
      double deltaPhi = neuPart[i].delta_phi_to( leadJet );
      double dPhi = neuPart[i].delta_phi_to( leadJet );
      double dEta = leadJet.eta() - neuPart[i].eta();
      PartPtDEtaDPhi->Fill( neuPart[i].pt(), dEta, dPhi );
      PartPtEtaPhi->Fill( neuPart[i].pt(), neuPart[i].eta(), neuPart[i].phi() );
      UE->Fill( neuPart[i].pt(), neuPart[i].eta(), neuPart[i].phi() );
      neuSum+=neuPart[i].pt();
    }

  }

  
  void CalculateUEsubtractedChargedRho( std::vector<fastjet::PseudoJet> chgPart, double &chgEast_Sum, double &chgMid_Sum, double &chgWest_Sum ) {
    double etaLoEast = -1.0;
    double etaLoMid = -0.3;
    double etaLoWest = 0.3;
    double etaHiEast = -0.3;
    double etaHiMid = 0.3;
    double etaHiWest = 1.0;
    double UEeta;

    chgEast_Sum=0;
    chgMid_Sum=0;
    chgWest_Sum=0;

    
    for ( int i=0; i<chgPart.size(); ++i ) {
      UEeta = chgPart[i].eta();
      if ( UEeta>=etaLoMid && UEeta<etaHiMid ) { chgMid_Sum+= chgPart[i].pt(); }
      else if ( UEeta>=etaLoEast && UEeta<=etaHiEast ) { chgEast_Sum+= chgPart[i].pt(); }
      else if ( UEeta>etaLoWest && UEeta<=etaHiWest ) { chgWest_Sum+= chgPart[i].pt(); }
      else { std::cerr<<"error with chg UE particle eta"<<std::endl; }
    }
  }
  
  
  // void CalculateRhoByChargeAndEta( std::vector<fastjet::PseudoJet> chgPart, std::vector<fastjet::PseudoJet> neuPart, double &chgEast_Sum, double &chgMid_Sum, double &chgWest_Sum, double &neuEast_Sum, double &neuMid_Sum, double &neuWest_Sum, TH3D *hChg, TH3D *hNeu ) {

  //   double etaLoEast = -1.0;
  //   double etaLoMid = -0.3;
  //   double etaLoWest = 0.3;
  //   double etaHiEast = -0.3;
  //   double etaHiMid = 0.3;
  //   double etaHiWest = 1.0;
  //   double UEeta;

  //   chgEast_Sum=0;
  //   chgMid_Sum=0;
  //   chgWest_Sum=0;
  //   neuEast_Sum=0;
  //   neuMid_Sum=0;
  //   neuWest_Sum=0;

    
  //   for ( int i=0; i<chgPart.size(); ++i ) {
  //     UEeta = chgPart[i].eta();
  //     hChg->Fill( chgPart[i].pt(), UEeta, chgPart[i].phi() );
  //     if ( UEeta>=etaLoMid && UEeta<etaHiMid ) { chgMid_Sum+= chgPart[i].pt(); }
  //     else if ( UEeta>=etaLoEast && UEeta<=etaHiEast ) { chgEast_Sum+= chgPart[i].pt(); }
  //     else if ( UEeta>etaLoWest && UEeta<=etaHiWest ) { chgWest_Sum+= chgPart[i].pt(); }
  //     else { std::cerr<<"error with chg UE particle eta"<<std::endl; }
  //   }
    
  //   for ( int i=0; i<neuPart.size(); ++i ) {
  //     UEeta = neuPart[i].eta();
  //     hNeu->Fill( neuPart[i].pt(), UEeta, neuPart[i].phi() );
  //     if ( UEeta>=etaLoMid && UEeta<etaHiMid ) { neuMid_Sum+= neuPart[i].pt(); }
  //     else if ( UEeta>=etaLoEast && UEeta<=etaHiEast ) { neuEast_Sum+= neuPart[i].pt(); }
  //     else if ( UEeta>etaLoWest && UEeta<=etaHiWest ) { neuWest_Sum+= neuPart[i].pt(); }
  //     else { std::cerr<<"error with neu UE particle eta"<<std::endl; }
  //   }


  // }


  void CalculateRhoByChargeAndEta( std::vector<fastjet::PseudoJet> chgPart, std::vector<fastjet::PseudoJet> neuPart, double &chgEast_Sum, double &chgMid_Sum, double &chgWest_Sum, double &neuEast_Sum, double &neuMid_Sum, double &neuWest_Sum ) {

    double etaLoEast = -1.0;
    double etaLoMid = -0.3;
    double etaLoWest = 0.3;
    double etaHiEast = -0.3;
    double etaHiMid = 0.3;
    double etaHiWest = 1.0;
    double UEeta;

    chgEast_Sum=0;
    chgMid_Sum=0;
    chgWest_Sum=0;
    neuEast_Sum=0;
    neuMid_Sum=0;
    neuWest_Sum=0;

    
    for ( int i=0; i<chgPart.size(); ++i ) {
      UEeta = chgPart[i].eta();
      if ( UEeta>=etaLoMid && UEeta<etaHiMid ) { chgMid_Sum+= chgPart[i].pt(); }
      else if ( UEeta>=etaLoEast && UEeta<=etaHiEast ) { chgEast_Sum+= chgPart[i].pt(); }
      else if ( UEeta>etaLoWest && UEeta<=etaHiWest ) { chgWest_Sum+= chgPart[i].pt(); }
      else { std::cerr<<"error with chg UE particle eta"<<std::endl; }
    }
    
    for ( int i=0; i<neuPart.size(); ++i ) {
      UEeta = neuPart[i].eta();
      if ( UEeta>=etaLoMid && UEeta<etaHiMid ) { neuMid_Sum+= neuPart[i].pt(); }
      else if ( UEeta>=etaLoEast && UEeta<=etaHiEast ) { neuEast_Sum+= neuPart[i].pt(); }
      else if ( UEeta>etaLoWest && UEeta<=etaHiWest ) { neuWest_Sum+= neuPart[i].pt(); }
      else { std::cerr<<"error with neu UE particle eta"<<std::endl; }
    }


  }

  
  int CountTowers( TList *selectedtowers ) {

    TStarJetPicoTower *tow;
    int n_towers = 0;
    for (int i=0; i<selectedtowers->GetEntries(); ++i) {
      tow = (TStarJetPicoTower *) selectedtowers->At(i);
      if ( fabs(tow->GetEta())<=etaCut ) { n_towers+=1; }
    }
    return n_towers;
  }


  std::vector<fastjet::PseudoJet> GatherCharged ( TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & rawParticles ){
    for ( int i=0; i < container->GetEntries() ; ++i ) {
      TStarJetVector* sv = container->Get(i);
      if ( sv->GetCharge() != 0 ) {
	fastjet::PseudoJet current = fastjet::PseudoJet( *sv );
	current.set_user_index( sv->GetCharge() );
	if ( std::abs(current.eta()) > etaCut )      { continue; }  // removes particles with |eta|>1
	if ( current.pt() < partMinPt )      { continue; }  // removes particles with pt<0.2GeV

	rawParticles.push_back(current);
      }
    }
    return rawParticles;
  }


  std::vector<fastjet::PseudoJet> GatherNeutral ( TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & rawParticles ){
    for ( int i=0; i < container->GetEntries() ; ++i ) {
      TStarJetVector* sv = container->Get(i);
      if ( sv->GetCharge() == 0 ) {
	fastjet::PseudoJet current = fastjet::PseudoJet( *sv );
	current.set_user_index( sv->GetCharge() );
	if ( std::abs(current.eta()) > etaCut )      { continue; }  // removes particles with |eta|>1
	if ( current.pt() < partMinPt )      { continue; }  // removes particles with pt<0.2GeV

	rawParticles.push_back(current);
      }
    }
    return rawParticles;
  }

  
  std::vector<fastjet::PseudoJet> GatherParticles ( TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & rawParticles ){
    for ( int i=0; i < container->GetEntries() ; ++i ) {
      TStarJetVector* sv = container->Get(i);
      fastjet::PseudoJet current = fastjet::PseudoJet( *sv );
      current.set_user_index( sv->GetCharge() );
      if ( std::abs(current.eta()) > etaCut )      { continue; }  // removes particles with |eta|>1
      if ( current.pt() < partMinPt )      { continue; }  // removes particles with pt<0.2GeV

      rawParticles.push_back(current);
    }
    return rawParticles;
  }

  
  std::vector<fastjet::PseudoJet> GatherBackground ( fastjet::PseudoJet trigJet, TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & ueParticles ){
    for ( int i=0; i < container->GetEntries() ; ++i ) {
      TStarJetVector* sv = container->Get(i);
      fastjet::PseudoJet current = fastjet::PseudoJet( *sv );

      double absDeltaPhi = fabs( current.delta_phi_to( trigJet ) );
      if ( absDeltaPhi < 1.0  ||  absDeltaPhi > 2.14159265 ) { continue; }       //  1 < delta phi < ( pi - 1 )
      
      if ( std::abs(current.eta()) > etaCut )      { continue; }  // removes particles with |eta|>1
      if ( current.pt() < partMinPt )      { continue; }  // removes particles with pt<0.2GeV

      current.set_user_index( sv->GetCharge() );

      ueParticles.push_back(current);
    }
    return ueParticles;
  }

  
  std::vector<fastjet::PseudoJet> GatherChargedUE ( fastjet::PseudoJet trigJet, TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & chgParticles ){
    for ( int i=0; i < container->GetEntries() ; ++i ) {
      TStarJetVector* sv = container->Get(i);
      fastjet::PseudoJet current = fastjet::PseudoJet( *sv );
      if ( sv->GetCharge() == 0 ) { continue; }

      double absDeltaPhi = fabs( current.delta_phi_to( trigJet ) );
      if ( absDeltaPhi < 1.0  ||  absDeltaPhi > 2.14159265 ) { continue; }       //  1 < delta phi < ( pi - 1 )
      
      if ( std::abs(current.eta()) > etaCut )      { continue; }  // removes particles with |eta|>1
      if ( current.pt() < partMinPt )      { continue; }  // removes particles with pt<0.2GeV

      current.set_user_index( sv->GetCharge() );

      chgParticles.push_back(current);
    }
    return chgParticles;
  }


  std::vector<fastjet::PseudoJet> GatherChargedUEwithEfficiency ( fastjet::PseudoJet trigJet, TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & chgParticles,  std::string efficiencyFile ){

    int ptBin, etaBin;
    const int nBins = 10;
    double pt, eta, effic;
    TString prefix;
    
    if (efficiencyFile == "src/trackeffic.root") { prefix = "eff_s_bin_1_10_bbc__"; }
    else if (efficiencyFile == "src/trackeffic_loEA.root") { prefix = "eff_s_bin_1_3_bbc__"; }
    else if (efficiencyFile == "src/trackeffic_hiEA.root") { prefix = "eff_s_bin_7_10_bbc__"; }
    else { std::cerr<<"efficFile not found"<<std::endl; }

    double etaLo[nBins] = { -1.0, -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8 };
    double etaHi[nBins] = { -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0 };

    TH1D *hEffic[nBins];
    
    TFile *ef = new TFile( efficiencyFile.c_str(), "READ" );
    
    for ( int j=0; j<nBins; ++j ) {   // pull in efficiency histograms
      TString name = prefix; name += (j+1); name += "_"; name += (j+1); name += "_eta";
      hEffic[j] = (TH1D*)ef->Get( name );
    }
    
    for ( int i=0; i < container->GetEntries() ; ++i ) {
      TStarJetVector* sv = container->Get(i);
      fastjet::PseudoJet current = fastjet::PseudoJet( *sv );
      if ( sv->GetCharge() == 0 ) { continue; }

      double absDeltaPhi = fabs( current.delta_phi_to( trigJet ) );
      if ( absDeltaPhi < 1.0  ||  absDeltaPhi > 2.14159265 ) { continue; }       //  1 < delta phi < ( pi - 1 )
      
      if ( std::abs(current.eta()) > etaCut )      { continue; }  // removes particles with |eta|>1
      if ( current.pt() < partMinPt )      { continue; }  // removes particles with pt<0.2GeV

      current.set_user_index( sv->GetCharge() );



      if (  (current.pt()<0.2) || (current.pt()>15.0)  ) {
	std::cerr<<"Charged Track pT = "<<current.pt()<<"   --  setting to 0.0"<<std::endl;
      }
      else {
	pt = current.pt();
	eta = current.eta();

	if (pt>3.) { pt = 3.; }

	etaBin = 999;
	for ( int j=0; j<nBins; ++j ) {     // find efficiency histogram corresponding to track eta
	  if ( (eta>=etaLo[j]) && (eta<=etaHi[j]) ) { etaBin = j; continue; }
	}
	if ( etaBin==999 ) { std::cerr<<"error finding charged track eta bin; eta = "<<eta<<std::endl; continue; }
	ptBin = hEffic[etaBin]->FindBin( pt );    // find histogram bin corresponding to track pt

	effic = hEffic[etaBin]->GetBinContent( ptBin );
      }

      // std::cout<<"pT: "<<current.pt()<<"  effic="<<effic<<endl;
      current/=effic;
      //std::cout<<"    corr pT:"<<current.pt()<<std::endl;  //debug
      
      chgParticles.push_back(current);
    }
    return chgParticles;
  }

  
  std::vector<fastjet::PseudoJet> GatherNeutralUE ( fastjet::PseudoJet trigJet, TStarJetVectorContainer<TStarJetVector> * container , std::vector<fastjet::PseudoJet> & neuParticles ){
    for ( int i=0; i < container->GetEntries() ; ++i ) {
      TStarJetVector* sv = container->Get(i);
      fastjet::PseudoJet current = fastjet::PseudoJet( *sv );
      if ( sv->GetCharge() != 0 ) { continue; }

      double absDeltaPhi = fabs( current.delta_phi_to( trigJet ) );
      if ( absDeltaPhi < 1.0  ||  absDeltaPhi > 2.14159265 ) { continue; }       //  1 < delta phi < ( pi - 1 )
      
      if ( std::abs(current.eta()) > etaCut )      { continue; }  // removes particles with |eta|>1
      if ( current.pt() < partMinPt )      { continue; }  // removes particles with pt<0.2GeV

      current.set_user_index( sv->GetCharge() );

      neuParticles.push_back(current);
    }
    return neuParticles;
  }


  void GetHeaderInfo( TStarJetPicoEventHeader* Header, int &Nglobal, int &Nvertices, int &ref_mult, int &Nprimary, double &BBC_CoincidenceRate,
		      double &vpdVz, double &BBC_EastRate, double &BBC_WestRate, double &BBC_AdcSumEast ) {
    Nglobal = Header->GetNGlobalTracks();						Nvertices = Header->GetNumberOfVertices();
    ref_mult = Header->GetReferenceMultiplicity();					Nprimary =  Header->GetNOfPrimaryTracks();
    BBC_CoincidenceRate = Header->GetBbcCoincidenceRate();		vpdVz = Header->GetVpdVz();
    BBC_EastRate = Header->GetBbcEastRate();					BBC_WestRate = Header->GetBbcWestRate();
    BBC_AdcSumEast = Header->GetBbcAdcSumEast();
  }


  void InitReader( TStarJetPicoReader & reader, TChain* chain, int nEvents ) {
    
    // set the chain
    reader.SetInputChain( chain );
    // apply hadronic correction - subtract 100% of charged track energy from towers
    // reader.SetApplyFractionHadronicCorrection( false );
    reader.SetFractionHadronicCorrection( 0.9999 );
    reader.SetRejectTowerElectrons( kFALSE );
    
    // Event and track selection
    // -------------------------
    
    TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
    evCuts->SetVertexZCut ( VertexZCut );
    evCuts->SetRefMultCut( RefMultCut );
    evCuts->SetMaxEventPtCut( MaxEventPtCut );
    evCuts->SetMaxEventEtCut( MaxEventEtCut );
    evCuts->SetMinEventEtCut( MinEventEtCut );
    evCuts->SetVertexZDiffCut( VertexZDiffCut );
    
    // Tracks cuts
    TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
    trackCuts->SetDCACut( DCACut );
    trackCuts->SetMinNFitPointsCut( MinNFitPointsCut );
    trackCuts->SetFitOverMaxPointsCut( FitOverMaxPointsCut );
    trackCuts->SetMaxPtCut ( MaxPtCut );

    
    std::cout << "Using these track cuts:" << std::endl;
    std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
    std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
    std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
    
    // Towers
    TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
    towerCuts->SetMaxEtCut( MaxEtCut );
    //towerCuts->AddBadTowers( "src/dummy_tower_list.txt" );
    //towerCuts->AddBadTowers( "/nfs/rhi/STAR/Data/P16id/resources/bad_towers_pAu2015.list" );
    towerCuts->AddBadTowers( "src/bad_towers_pAu2015_NEW.list" );

    std::cout << "Using these tower cuts:" << std::endl;
    std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl;
    std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl;
    std::cout << "  Vz cut = " << evCuts->GetVertexZCut() << std::endl;
    std::cout << "  Vz diff cut = " << evCuts->GetVertexZDiffCut() << std::endl;
    
    // V0s: Turn off
    reader.SetProcessV0s(false);
    
    // Initialize the reader
    reader.Init( nEvents ); //runs through all events with -1
  }

  double UEsubtraction( fastjet::PseudoJet leadjet, std::string UEcorrFile, double BBCEsum ) {

    TFile *UEfile = new TFile( UEcorrFile.c_str() , "READ" );

    double eta_Lo[3] = { -1.0, -0.3, 0.3 };
    double eta_Hi[3] = { -0.3, 0.3, 1.0 };
    
    double rhoValue;

    TH1D* EastRhoProfile = (TH1D*)UEfile->Get("hEastRhoByBBCEsum_pfx");
    int nEastProfBins = EastRhoProfile->GetNbinsX();
    TH1D* MidRhoProfile = (TH1D*)UEfile->Get("hMidRhoByBBCEsum_pfx");
    int nMidProfBins = MidRhoProfile->GetNbinsX();
    TH1D* WestRhoProfile = (TH1D*)UEfile->Get("hWestRhoByBBCEsum_pfx");
    int nWestProfBins = WestRhoProfile->GetNbinsX();

    if ( leadjet.eta() >= eta_Lo[0]  &&  leadjet.eta() <= eta_Hi[0] ) {  //EAST
      for ( int j=0; j<nEastProfBins; ++j ) {
    	double BBCElo = EastRhoProfile->GetBinLowEdge(j);
    	double BBCEhi = BBCElo + EastRhoProfile->GetBinWidth(j);
    	if ( BBCEsum >= BBCElo &&  BBCEsum <= BBCEhi ) { rhoValue = EastRhoProfile->GetBinContent(j); }
      }
    }
    else if ( leadjet.eta() >= eta_Lo[1]  &&  leadjet.eta() <= eta_Hi[1] ) {  //MID
      for ( int j=0; j<nMidProfBins; ++j ) {
    	double BBCElo = MidRhoProfile->GetBinLowEdge(j);
    	double BBCEhi = BBCElo + MidRhoProfile->GetBinWidth(j);
    	if ( BBCEsum >= BBCElo &&  BBCEsum <= BBCEhi ) { rhoValue = MidRhoProfile->GetBinContent(j); }
      }
    }
    else if ( leadjet.eta() >= eta_Lo[2]  &&  leadjet.eta() <= eta_Hi[2] ) {  //WEST
      for ( int j=0; j<nWestProfBins; ++j ) {
    	double BBCElo = WestRhoProfile->GetBinLowEdge(j);
    	double BBCEhi = BBCElo + WestRhoProfile->GetBinWidth(j);
    	if ( BBCEsum >= BBCElo &&  BBCEsum <= BBCEhi ) { rhoValue = WestRhoProfile->GetBinContent(j); }
      }
    }
    else { std::cerr<<"error with finding lead jet eta"<<std::endl; }
    
    double leadPtCorr = leadjet.pt() - leadjet.area()*rhoValue;

    UEfile->Close();
    
    return leadPtCorr;
  }


  bool UseHTevent( TStarJetPicoEventHeader* Header, TStarJetPicoEvent* Event, double vz_cut, double vz ) {
    if (Header->GetRunId() >= 16142059 && Header->GetRunId() <= 16149001) { return false; }    //TEMPORARILY SKIPPING THESE RUNS
    else if (Header->GetRunId() == 16135031 || Header->GetRunId() == 16135032) { return false; }
    else if ( abs(vz) > vz_cut ) { return false; }
    else if (!(Header->HasTriggerId(500205) || Header->HasTriggerId(500215))) {return false;}   //  ONLY SELECT HT TRIGGER EVENTS
    else if ( Header->GetBbcAdcSumEast() > 64000 ) { return false; }

    TStarJetPicoTriggerInfo *trig;
    double trigTowEt;

    int trigTow = 0;
    for ( int i=0; i<Event->GetTrigObjs()->GetEntries(); ++i ) {  // loop through all triggers
      trig = (TStarJetPicoTriggerInfo *)Event->GetTrigObj(i);
      
      if ( trig->isBHT2() ) {                                    // check if trig is BHT2
	
	int trigTowId = trig->GetId();                           // get ID of trigger tower

	if ( !UseTriggerTower( trigTowId ) ) { continue; }        // check trig tower against bad tower list
	else {

	  trigTow = 0;
	  trigTowEt = 0;
	  
	  for ( int j=0; j<Event->GetTowers()->GetEntries(); ++j ) {  // USE GetTowers TO FIND TOWER INFO ASSOCIATED WITH TRIGGER!
	    if ( Event->GetTower(j)->GetId() == trigTowId && Event->GetTower(j)->GetEt()>=5.40  && Event->GetTower(j)->GetEt()<30.00 ) {
	      // FIND TOWER ASSOCIATED WITH TRIGGER AND ENSURE TRIG TOWER HAS 5.40 <= Et <= 30
	      if ( trigTow==0 ) {
		trigTowEt = Event->GetTower(j)->GetEt();  // once FIRST trig tower is found, assign it's Et to "trigTowEt"
	      }
	      else {                         // FOR ALL SUBSEQUENT TOWERS, if its Et is greater than "trigTowEt", set "trigTowEt" to
		if ( Event->GetTower(j)->GetEt() > trigTowEt ) {
		  //std::cerr<<
		  trigTowEt = Event->GetTower(j)->GetEt();
		  trigTow+=1;                               // (add to counter)
		}  // this tower's Et
	      }
	    }
	  }
	  
	}
      }
      else { continue; }
    }
    if ( trigTow==0 ) { return false; }  // if no triggers meet requirements, do not use event!
    
    else return true;
  }

  
  bool UseMBevent( TStarJetPicoEventHeader* Header, TStarJetPicoEvent* Event, double vz_cut, double vz ) {
    if (Header->GetRunId() >= 16142059 && Header->GetRunId() <= 16149001) { return false; }    //TEMPORARILY SKIPPING THESE RUNS
    else if (Header->GetRunId() == 16135031 || Header->GetRunId() == 16135032) { return false; }
    else if ( abs(vz) > vz_cut ) { return false; }
    else if ( !(Header->HasTriggerId(500008) || Header->HasTriggerId(500018))) {return false;}   //  ONLY SELECT MB TRIGGER EVENT
    else if ( Header->GetBbcAdcSumEast() > 64000 ) { return false; }    
    else return true;
  }
  

  bool UseTriggerTower( int TriggerTowerId ) {
    
    int badTows[454] = { 31, 34, 59, 95, 96, 106, 113, 120, 134, 139, 157, 160, 175, 214, 224, 257, 266, 267, 275, 280, 282, 286, 287, 308, 360, 368, 385, 389, 395, 405, 410, 426, 433, 474, 479, 483, 484, 504, 509, 533, 541, 555, 561, 562, 563, 564, 585, 603, 615, 616, 627, 633, 638, 649, 650, 653, 657, 671, 673, 674, 680, 681, 693, 721, 722, 725, 740, 750, 753, 754, 755, 756, 758, 768, 773, 774, 775, 776, 779, 784, 790, 793, 794, 795, 796, 799, 801, 813, 814, 815, 816, 817, 822, 832, 835, 837, 840, 844, 846, 857, 860, 875, 880, 882, 893, 897, 899, 903, 916, 936, 939, 941, 946, 953, 954, 956, 986, 989, 993, 1005, 1012, 1020, 1023, 1026, 1027, 1028, 1045, 1046, 1048, 1057, 1063, 1080, 1081, 1085, 1100, 1104, 1120, 1125, 1128, 1130, 1132, 1142, 1154, 1158, 1159, 1160, 1171, 1180, 1183, 1184, 1187, 1189, 1190, 1197, 1200, 1202, 1204, 1207, 1214, 1217, 1219, 1220, 1221, 1224, 1232, 1233, 1237, 1238, 1244, 1256, 1257, 1263, 1280, 1283, 1294, 1301, 1306, 1312, 1313, 1318, 1329, 1341, 1348, 1353, 1354, 1369, 1375, 1378, 1388, 1400, 1401, 1405, 1407, 1409, 1434, 1439, 1440, 1448, 1475, 1486, 1537, 1563, 1564, 1567, 1574, 1575, 1588, 1592, 1597, 1599, 1602, 1612, 1654, 1668, 1679, 1705, 1709, 1720, 1728, 1753, 1762, 1765, 1766, 1773, 1776, 1789, 1807, 1823, 1840, 1856, 1866, 1877, 1878, 1879, 1880, 1921, 1945, 1952, 1976, 1983, 1984, 2027, 2032, 2043, 2051, 2066, 2073, 2077, 2092, 2093, 2097, 2104, 2107, 2111, 2141, 2160, 2162, 2168, 2175, 2176, 2177, 2190, 2193, 2194, 2195, 2196, 2197, 2213, 2214, 2215, 2216, 2217, 2223, 2233, 2234, 2235, 2236, 2253, 2254, 2255, 2256, 2299, 2300, 2303, 2305, 2313, 2340, 2390, 2391, 2392, 2414, 2415, 2417, 2439, 2459, 2476, 2493, 2520, 2569, 2580, 2589, 2590, 2633, 2697, 2737, 2749, 2822, 2834, 2863, 2865, 2874, 2929, 2953, 2954, 2955, 2961, 2969, 2973, 2974, 2975, 2976, 2981, 2993, 2994, 2995, 3005, 3020, 3063, 3070, 3071, 3146, 3160, 3167, 3186, 3233, 3234, 3235, 3236, 3253, 3254, 3255, 3256, 3263, 3273, 3274, 3275, 3276, 3293, 3294, 3295, 3296, 3299, 3300, 3337, 3354, 3355, 3356, 3360, 3362, 3385, 3407, 3436, 3451, 3473, 3481, 3492, 3493, 3494, 3495, 3498, 3504, 3513, 3515, 3544, 3584, 3588, 3611, 3666, 3668, 3670, 3678, 3679, 3682, 3690, 3692, 3702, 3718, 3720, 3725, 3737, 3738, 3739, 3741, 3769, 3777, 3822, 3831, 3834, 3838, 3840, 3859, 3861, 3984, 4006, 4013, 4017, 4018, 4019, 4047, 4053, 4057, 4079, 4099, 4104, 4130, 4169, 4177, 4217, 4223, 4302, 4331, 4350, 4355, 4357, 4405, 4440, 4458, 4460, 4469, 4480, 4495, 4496, 4500, 4518, 4534, 4563, 4595, 4659, 4677, 4678, 4712, 4742, 4743, 4744, 4762, 4763, 4764, 4766, 4768, 4778, 4781, 4782, 4783, 4784 };
    
    for( int i=0; i<454; ++i ) {
      if( badTows[i]==TriggerTowerId ) { return false; }
    }
    return true;
  }

  
}
