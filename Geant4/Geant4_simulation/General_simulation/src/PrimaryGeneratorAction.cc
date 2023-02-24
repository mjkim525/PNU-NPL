#include "PrimaryGeneratorAction.hh"
#include "ParameterContainer.hh"

#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction(ParameterContainer* par)
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0),
	PC(par)
{
	fParticleGun  = new G4ParticleGun();

	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName = PC -> GetParString("Beam_particle");
	G4ParticleDefinition* particle
		= particleTable->FindParticle(particleName);
	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
	fParticleGun->SetParticleEnergy(PC->GetParDouble("Beam_energy")*MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	if (PC->GetParBool("Square_beam"))
	{
	   for(G4int n=0; n<PC -> GetParInt("NperEvent"); n++)
		{
			G4double x0 = 
				(PC->GetParDouble("Beam_x0")+PC->GetParDouble("Beam_dx")*(G4UniformRand()-0.5)) * mm;
			G4double y0 = 
				(PC->GetParDouble("Beam_y0")+PC->GetParDouble("Beam_dy")*(G4UniformRand()-0.5)) * mm;
			G4double z0 = 
				(PC->GetParDouble("Beam_z0")+PC->GetParDouble("Beam_dz")*(G4UniformRand()-0.5)) * mm;

			fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

			fParticleGun->GeneratePrimaryVertex(anEvent);
		}
	}

	if (PC->GetParBool("Radial_beam"))
	{

   	for(G4int n=0; n<PC -> GetParInt("NperEvent"); n++)
		{
			G4double z0 = (PC->GetParDouble("Beam_z0") + PC->GetParDouble("Beam_dz")*(G4UniformRand()-0.5)) * mm;

			// Add beam radius
			G4double beamRadius = PC->GetParDouble("Beam_radius")*((G4UniformRand())) * mm;
			G4double phi = 2. * CLHEP::pi *(G4UniformRand());
			G4double x0 = beamRadius * cos(phi);
			G4double y0 = beamRadius * sin(phi);

			fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
			fParticleGun->GeneratePrimaryVertex(anEvent);
		}
	}

}
