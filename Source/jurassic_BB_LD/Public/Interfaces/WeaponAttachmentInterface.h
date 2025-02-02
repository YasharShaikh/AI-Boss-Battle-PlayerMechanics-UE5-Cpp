#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponAttachmentInterface.generated.h"

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
	AT_Magazine UMETA(DisplayName = "Magazine"),
	AT_Scope UMETA(DisplayName = "Scope"),
	AT_Barrel UMETA(DisplayName = "Barrel"),
	AT_Grip UMETA(DisplayName = "Grip")
};

UINTERFACE(MinimalAPI, Blueprintable)
class UWeaponAttachmentInterface : public UInterface
{
	GENERATED_BODY()
};

class JURASSIC_BB_LD_API IWeaponAttachmentInterface
{
	GENERATED_BODY()

public:
	/** Attach the attachment to the weapon */
	virtual void AttachToWeapon(class AWeaponPickup* Weapon) = 0;

	/** Detach the attachment from the weapon */
	virtual void DetachFromWeapon() = 0;

	/** Get the attachment type */
	virtual EAttachmentType GetAttachmentType() const = 0;
};
