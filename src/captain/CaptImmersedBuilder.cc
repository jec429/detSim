#include "CaptImmersedBuilder.hh"
#include "CaptDriftRegionBuilder.hh"
#include "CaptPMTAssemblyBuilder.hh"

#include "DSimBuilder.hh"

#include <DSimLog.hh>

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>

class CaptImmersedMessenger
    : public DSimBuilderMessenger {
private:
    CaptImmersedBuilder* fBuilder;

public:
    CaptImmersedMessenger(CaptImmersedBuilder* c) 
        : DSimBuilderMessenger(c,"Control the immersed geometry."),
          fBuilder(c) {

    };

    virtual ~CaptImmersedMessenger() {
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        DSimBuilderMessenger::SetNewValue(cmd,val);
    };
};

void CaptImmersedBuilder::Init(void) {
    SetMessenger(new CaptImmersedMessenger(this));
    SetSensitiveDetector("cryo","segment");

    AddBuilder(new CaptDriftRegionBuilder("Drift",this));
    AddBuilder(new CaptPMTAssemblyBuilder("PMTAssembly",this));

    /// Set the drift region parameters for CAPTAIN.
    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    drift.SetApothem(1037.5*mm);
    drift.SetDriftLength(1000*mm);
    drift.SetWirePlaneSpacing(3.18*mm);
}

CaptImmersedBuilder::~CaptImmersedBuilder() {};

double CaptImmersedBuilder::GetRadius() {
    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    double radius = drift.GetRadius() + 2*cm;
    return radius;
}

double CaptImmersedBuilder::GetHeight() {
    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    CaptPMTAssemblyBuilder& pmts = Get<CaptPMTAssemblyBuilder>("PMTAssembly");
    double height = drift.GetHeight() + pmts.GetHeight();
    return height;
}

G4LogicalVolume *CaptImmersedBuilder::GetPiece(void) {

    G4LogicalVolume* logVolume 
        = new G4LogicalVolume(new G4Tubs(GetName(),
                                         0.0, GetRadius(), GetHeight()/2, 
                                         0*degree, 360*degree),
                              FindMaterial("Argon_Liquid"),
                              GetName());
    logVolume->SetVisAttributes(GetColor(logVolume));
    if (GetSensitiveDetector()) {
        logVolume->SetSensitiveDetector(GetSensitiveDetector());
    }

    G4ThreeVector center(0.0,0.0,GetHeight()/2);

    /// All the space above the drift region.
    center -= G4ThreeVector(0.0,0.0,0.0);

    // Put in the drift region.
    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    G4LogicalVolume* logDrift = drift.GetPiece();
    center -= G4ThreeVector(0.0,0.0,drift.GetHeight()/2);
    fOffset = center + drift.GetOffset();
    new G4PVPlacement(NULL,                    // rotation.
                      center,                  // position
                      logDrift,                // logical volume
                      logDrift->GetName(),     // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    
    // Put in the field cage.
    double fieldInner[] = {drift.GetApothem()+10*mm, drift.GetApothem()+10*mm};
    double fieldOuter[] = {drift.GetApothem()+13*mm, drift.GetApothem()+13*mm};
    double fieldPlane[] = {-drift.GetHeight()/2,
                           drift.GetHeight()/2-drift.GetGridPlaneOffset()};

    G4LogicalVolume *logFieldCage
	= new G4LogicalVolume(new G4Polyhedra(GetName()+"/FieldCage",
                                              90*degree, 450*degree,
                                              6, 2,
                                              fieldPlane,
                                              fieldInner, fieldOuter),
                              FindMaterial("FR4_Copper"),
                              GetName()+"/FieldCage");
    logFieldCage->SetVisAttributes(GetColor(logFieldCage));

    new G4PVPlacement(NULL,                    // rotation.
                      center,                  // position
                      logFieldCage,            // logical volume
                      logFieldCage->GetName(), // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.
    
    // Put in the PMT mounting
    CaptPMTAssemblyBuilder& pmts = Get<CaptPMTAssemblyBuilder>("PMTAssembly");
    center -= G4ThreeVector(0.0,0.0,drift.GetHeight()/2);
    center -= G4ThreeVector(0.0,0.0,pmts.GetHeight()/2);
    G4LogicalVolume* logPMTS = pmts.GetPiece();
    new G4PVPlacement(NULL,                    // rotation.
                      center,                  // position
                      logPMTS,            // logical volume
                      logPMTS->GetName(), // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    return logVolume;
}
