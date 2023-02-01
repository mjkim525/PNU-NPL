#include "DetectorConstruction.hh"
#include "ParameterContainer.hh"

#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Orb.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

DetectorConstruction::DetectorConstruction(ParameterContainer* par)
: G4VUserDetectorConstruction(),
	PC(par)
{
}

DetectorConstruction::~DetectorConstruction()
{ }

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
		// Get nist material manager
		G4NistManager* nist = G4NistManager::Instance();

		// Option to switch on/off checking of volumes overlaps
		G4bool checkOverlaps = true;

		G4ThreeVector fZero(0, 0, 0);
		G4double fSTPTemp = 273.15 * kelvin;
		G4double fLabTemp = fSTPTemp + 20 * kelvin;

		G4Element* elH = new G4Element("Hydrogen", "H", 1., 1.00794 * g/mole);
		G4Element* elC = new G4Element("Carbon", "C", 6., 12.011 * g/mole);
		G4Element* elO = new G4Element("Oxygen", "O", 8., 16.000 * g/mole);

		// World
		G4int worldID = PC -> GetParInt("WorldID");
		G4double world_sizeX = PC -> GetParDouble("World_sizeX");
		G4double world_sizeY = PC -> GetParDouble("World_sizeY");
		G4double world_sizeZ = PC -> GetParDouble("World_sizeZ");
		G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");

		G4Box* solidWorld =    
			new G4Box("World",0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);
		G4LogicalVolume* logicWorld =                         
			new G4LogicalVolume(solidWorld,world_mat,"World");
		G4VPhysicalVolume* physWorld = 
			new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,worldID,checkOverlaps);  

		// Box
	if (PC->GetParBool("BoxIn"))
	{

		G4int boxID = PC -> GetParInt("BoxID");
		G4double box_sizeX = PC -> GetParDouble("Box_sizeX");
		G4double box_sizeY = PC -> GetParDouble("Box_sizeY");
		G4double box_sizeZ = PC -> GetParDouble("Box_sizeZ");
		G4Material* mat_box = nist -> FindOrBuildMaterial("G4_Pb");

		G4Box* solidBox =
			new G4Box("Box",0.5*box_sizeX,0.5*box_sizeY,0.5*box_sizeZ);
		G4LogicalVolume* logicBox = 
			new G4LogicalVolume(solidBox,mat_box,"Box");
		new G4PVPlacement(0,G4ThreeVector(),logicBox,"Box",logicWorld,false,boxID,checkOverlaps);

		G4VisAttributes* attBox = new G4VisAttributes(G4Colour(G4Colour::Gray()));
		attBox -> SetVisibility(true);
		attBox -> SetForceWireframe(true);
		logicBox -> SetVisAttributes(attBox);
    }

		// Collimator
	if (PC->GetParBool("CollimatorIn"))
	{
		G4Material* Collmat = new G4Material("Acrylic",1.19*g/cm3,3,kStateSolid, 293.15*kelvin);
		Collmat -> AddElement(elC,5);
		Collmat -> AddElement(elH,8);
		Collmat -> AddElement(elO,2);

		G4int	 CollID	   = PC ->GetParInt("CollID");
		G4double CollDimX  = PC ->GetParDouble("CollDimX");	// one brick [] 
		G4double CollDimY  = PC ->GetParDouble("CollDimY");
		G4double CollDimZ  = PC ->GetParDouble("CollDimZ");
		G4double CollslitX = PC ->GetParDouble("CollslitX");
		G4double CollslitY = PC ->GetParDouble("CollslitY");
		G4double CollPosZ  = PC ->GetParDouble("CollPosZ");

		//Volumes
		G4Box* solidBoxColl = new G4Box("solidBoxColl", CollDimX/2., CollDimY/2., CollDimZ/2.);

		G4Box* solidSubCollX = new G4Box("solidSubCollX", CollslitX/2., CollDimY/2. , CollDimZ/2.);
		G4Box* solidSubCollY = new G4Box("solidSubCollY", CollDimX/2. , CollslitY/2., CollDimZ/2.);
		G4SubtractionSolid* solidCollX = new G4SubtractionSolid("solidCollX", solidBoxColl, solidSubCollX, 0, fZero);
		G4SubtractionSolid* solidCollY = new G4SubtractionSolid("solidCollY", solidBoxColl, solidSubCollY, 0, fZero);
		G4LogicalVolume* logicCollX = new G4LogicalVolume(solidCollX, Collmat, "logicCollimatorX");
		G4LogicalVolume* logicCollY = new G4LogicalVolume(solidCollY, Collmat, "logicCollimatorY");

		//vis attributes
		G4VisAttributes* attColl = new G4VisAttributes(G4Colour(G4Colour::Gray()));
		G4VisAttributes* attColl1 = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attColl->SetVisibility(true);
		attColl->SetForceWireframe(true);
		attColl1->SetVisibility(true);
		attColl1->SetForceWireframe(true);
		logicCollX->SetVisAttributes(attColl);
		logicCollY->SetVisAttributes(attColl1);

		//Position
		G4ThreeVector posCollX(0, 0, CollPosZ + CollDimZ/2.);
		G4ThreeVector posCollY(0, 0, CollPosZ + CollDimZ/2. + CollDimZ);
		new G4PVPlacement(0, posCollX, logicCollY, "CollimatorX", logicWorld, false, CollID, checkOverlaps);
		new G4PVPlacement(0, posCollY, logicCollX, "CollimatorY", logicWorld, false, CollID, checkOverlaps);
	}



	return physWorld;
}
