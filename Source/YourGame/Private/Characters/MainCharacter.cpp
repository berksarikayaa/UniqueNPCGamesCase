#include "Characters/MainCharacter.h"
#include "Interfaces/InteractionInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AMainCharacter::AMainCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Karakter ayarları
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    // Hareket bileşeni ayarları
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 500.f;
    GetCharacterMovement()->AirControl = 0.2f;
}

void AMainCharacter::BeginPlay()
{
    Super::BeginPlay();
    InitializeHotbar();
}

void AMainCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    PerformInteractionTrace();
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    
    BindHotbarInputs();
}

void AMainCharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMainCharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AMainCharacter::PerformInteractionTrace()
{
    FVector Location;
    FRotator Rotation;
    GetController()->GetPlayerViewPoint(Location, Rotation);

    FVector Start = Location;
    FVector End = Start + (Rotation.Vector() * InteractionDistance);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    FHitResult Hit;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionTraceRadius);

    bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, Sphere, QueryParams);

    if (bHit)
    {
        if (Hit.GetActor()->Implements<UInteractionInterface>())
        {
            LastInteractableActor = Hit.GetActor();
            return;
        }
    }

    LastInteractableActor = nullptr;
}

bool AMainCharacter::AddItemToHotbar(const FHotbarItem& NewItem)
{
    if (HotbarItems.Num() < MaxHotbarSlots)
    {
        HotbarItems.Add(NewItem);
        return true;
    }
    return false;
}

void AMainCharacter::UseHotbarItem(int32 SlotIndex)
{
    if (HotbarItems.IsValidIndex(SlotIndex) && HotbarItems[SlotIndex].bIsUsable)
    {
        OnHotbarItemUsed(HotbarItems[SlotIndex], SlotIndex);
    }
}

void AMainCharacter::InitializeHotbar()
{
    HotbarItems.Empty();
    HotbarItems.SetNum(MaxHotbarSlots);
}

void AMainCharacter::BindHotbarInputs()
{
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
        {
            // Enhanced Input kullanılıyorsa burada input mapping yapılabilir
        }
        else
        {
            // Legacy input system için hotbar tuşları
            for (int32 i = 0; i < MaxHotbarSlots; i++)
            {
                FString ActionName = FString::Printf(TEXT("UseHotbarSlot%d"), i + 1);
                PlayerInputComponent->BindAction(*ActionName, IE_Pressed, this, &AMainCharacter::UseHotbarItem, i);
            }
        }
    }
} 