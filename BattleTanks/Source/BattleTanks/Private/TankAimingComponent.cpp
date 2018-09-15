// Fill out your copyright notice in the Description page of Project Settings.

#include "TankAimingComponent.h"
#include "TankTurret.h"
#include "TankBarrel.h"
#include "Projectile.h"

// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

void UTankAimingComponent::Initialize(UTankBarrel* BarrelToSet, UTankTurret* TurretToSet)
{
	if (!ensure(BarrelToSet)) { return; }
	if (!ensure(TurretToSet)) { return; }

	Barrel = BarrelToSet;
	Turret = TurretToSet;
}


void UTankAimingComponent::AimAt(FVector HitLocation)
{
	if (!ensure(Barrel)) { return; }

	FVector OutLaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));
	FCollisionResponseParams CollisionResponse;
	TArray<AActor*> ActorsToIgnore;

	bool bSuggestionWorked = UGameplayStatics::SuggestProjectileVelocity(this, OutLaunchVelocity,StartLocation, HitLocation, LaunchSpeed, false, 0,0,ESuggestProjVelocityTraceOption::DoNotTrace,  CollisionResponse, ActorsToIgnore);

	if (bSuggestionWorked) // Calculate the OutLaunchVelocity
	{
		FVector AimDirection = OutLaunchVelocity.GetSafeNormal();
		MoveBarrelTowards(AimDirection);
	}
}

void UTankAimingComponent::Fire()
{
	bool isReloaded = (FPlatformTime::Seconds() - LastFireTime) > ReloadTimeInSeconds;

	if (!ensure(Barrel && ProjectileBlueprint)) { return; }

	if (isReloaded)
	{
		FVector SocketLocation = Barrel->GetSocketLocation(FName("Projectile"));
		FRotator SocketRotation = Barrel->GetSocketRotation(FName("Projectile"));
		// Spawn Projectile at the Socket Location of the Barrel
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileBlueprint, SocketLocation, SocketRotation);
		Projectile->LaunchProjectile(LaunchSpeed);

		LastFireTime = FPlatformTime::Seconds();
	}
}

void UTankAimingComponent::MoveBarrelTowards(FVector AimDirecion)
{
	if (!ensure(Barrel && Turret)) { return; }
	// Work-out difference between current barrel rotation, and AimDirection
	FRotator BarrelRotator = Barrel->GetForwardVector().Rotation();
	FRotator AimAsRotator = AimDirecion.Rotation();

	FRotator DeltaRotator = AimAsRotator - BarrelRotator;

	

	Barrel->Elevate(DeltaRotator.Pitch);	
	Turret->RotateAround(DeltaRotator.Yaw);
	// Move the barrel the right amount this frame

	// Given a max elevation speed, and the frame time

}

