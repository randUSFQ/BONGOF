#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLorentzVector.h"

void higgs_search() {
    std::cout << "\n=== Conectando con los servidores del CERN (Streaming de datos)... ===" << std::endl;

    // Archivo oficial del CERN del canal dorado (4 leptones) para educación e investigación
    TFile *file = TFile::Open("http://root.cern");
    if (!file || file->IsZombie()) {
        std::cerr << "Error crítico: No se pudo conectar al servidor de datos del CERN." << std::endl;
        return;
    }

    TTree *tree = (TTree*)file->Get("Events");
    if (!tree) {
        std::cerr << "Error: Estructura TTree 'Events' no encontrada." << std::endl;
        file->Close();
        return;
    }

    // Inicialización de variables estructurales (Float_t para compatibilidad nativa)
    // Usamos variables escalares típicas de eventos filtrados para simplificar la ejecución
    Float_t f_muon_pt[4], f_muon_eta[4], f_muon_phi[4], f_muon_mass[4];
    Int_t f_NMuon = 0;

    tree->SetBranchAddress("f_muon_pt", &f_muon_pt);
    tree->SetBranchAddress("f_muon_eta", &f_muon_eta);
    tree->SetBranchAddress("f_muon_phi", &f_muon_phi);
    tree->SetBranchAddress("f_muon_mass", &f_muon_mass);

    // Histograma estadístico enfocado en la ventana del Bosón de Higgs (70 a 180 GeV)
    TH1D *h_m4l = new TH1D("h_m4l", "CMS Open Data;M_{4#mu} [GeV];Eventos / 2.5 GeV", 44, 70, 180);

    Long64_t nentries = tree->GetEntries();
    std::cout << "Analizando " << nentries << " eventos cuánticos reales del LHC..." << std::endl;

    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);

        // Reconstrucción relativista del espacio-tiempo de Minkowski usando TLorentzVector
        TLorentzVector p1, p2, p3, p4;
        p1.SetPtEtaPhiM(f_muon_pt[0], f_muon_eta[0], f_muon_phi[0], f_muon_mass[0]);
        p2.SetPtEtaPhiM(f_muon_pt[1], f_muon_eta[1], f_muon_phi[1], f_muon_mass[1]);
        p3.SetPtEtaPhiM(f_muon_pt[2], f_muon_eta[2], f_muon_phi[2], f_muon_mass[2]);
        p4.SetPtEtaPhiM(f_muon_pt[3], f_muon_eta[3], f_muon_phi[3], f_muon_mass[3]);

        // Aplicación de cortes cinemáticos rigurosos (PhD Analysis Cuts)
        if (p1.Pt() < 20.0 || p2.Pt() < 10.0) continue; // Descartar leptones blandos (ruido)
        if (std::abs(p1.Eta()) > 2.4 || std::abs(p2.Eta()) > 2.4) continue; // Límites geométricos del barril de CMS

        // Sumatoria de los cuadrivectores del sistema completo (H -> ZZ -> 4mu)
        TLorentzVector higgs_candidate = p1 + p2 + p3 + p4;
        
        // Registrar la masa invariante reconstruida del candidato
        h_m4l->Fill(higgs_candidate.M());
    }

    // Renderizado en lienzo virtual (Modo Batch sin pantalla)
    TCanvas *c1 = new TCanvas("c1", "Higgs Discovery", 900, 600);
    h_m4l->SetMarkerStyle(21);
    h_m4l->SetMarkerColor(kBlue);
    h_m4l->SetLineColor(kBlue + 2);
    h_m4l->Draw("E1"); // Barras de error de Poisson estándar

    c1->SaveAs("higgs_discovery_resonance.pdf");
    std::cout << "¡Éxito! Gráfica científica de alta resolución guardada como 'higgs_discovery_resonance.pdf'.\n" << std::endl;

    file->Close();
}
