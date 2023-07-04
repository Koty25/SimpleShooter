// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Gun.h"
#include "SimpleShooterGameModeBase.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	GetMesh()->HideBoneByName(TEXT("weapon_r"), PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
}

bool AShooterCharacter::IsDead()
{
	return Health <= 0.f;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveSideways"), this, &AShooterCharacter::MoveSideways);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &AShooterCharacter::LookUpDown);
	PlayerInputComponent->BindAxis(TEXT("LookUpDownRate"), this, &AShooterCharacter::LookUpDownRate);
	PlayerInputComponent->BindAxis(TEXT("LookLeftRight"), this, &AShooterCharacter::LookLeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookLeftRightRate"), this, &AShooterCharacter::LookLeftRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("ShootGun"), IE_Pressed, this, &AShooterCharacter::ShootGun);

}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float DamageToApply =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageToApply = FMath::Min(Health,DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	if (IsDead())
	{
		ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageToApply;
}


void AShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::LookUpDown(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void AShooterCharacter::LookUpDownRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::MoveSideways(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookLeftRight(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void AShooterCharacter::LookLeftRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::ShootGun()
{
	Gun->PullTrigger();
}
