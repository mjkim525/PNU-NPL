#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "ParameterContainer.hh"

#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4HadronicProcessStore.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

#include <string>

int main(int argc,char** argv)
{
	G4UIExecutive* ui = 0;
	if ( argc == 1 ) 
		ui = new G4UIExecutive(argc, argv);

	G4RunManager* runManager = new G4RunManager;

	G4String parameters = "Parameters.conf";
	ParameterContainer* PC = new ParameterContainer(parameters);

	// initialize the physics list
	G4String physListStr = PC -> GetParString("PhysicsList");
	G4PhysListFactory* physListFac = new G4PhysListFactory();
	G4VModularPhysicsList* physicsList = physListFac ->GetReferencePhysList(physListStr.c_str());
	runManager->SetUserInitialization(physicsList);
	G4HadronicProcessStore::Instance() -> SetVerbose(0);
  
	// the random seed
	G4int seed = PC -> GetParInt("RandomSeed");
	if(seed == 0)
		G4Random::setTheSeed(time(0));
	else
		G4Random::setTheSeed(seed);

	// User action initialization
	runManager->SetUserInitialization(new DetectorConstruction(PC));
	runManager->SetUserInitialization(new ActionInitialization(PC));

	// Initialize visualization
	//
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// Process macro or start UI session
	if ( PC -> GetParBool("Visualisation") == false) 
	{ 
		// batch mode
		G4String command = "/control/execute ";
		if(argc ==1)
		{
			G4String fileName = PC -> GetParString("MacroFile");
			UImanager->ApplyCommand(command+fileName);
		}
		if(argc >=2)
		{
			G4String macroFile = argv[1];
			UImanager->ApplyCommand(command+macroFile);
		}
	} else { 
		// interactive mode
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
		delete ui;
	}

	delete visManager;
	delete runManager;
}
