#pragma once

#include "CoreMinimal.h"
#include "StateData.generated.h"

UENUM(BlueprintType)
enum class EObjectState : uint8
{
	EOS_NONE		UMETA(DisplayName = "NONE"),
	EOS_HIGHLIGHT	UMETA(DisplayName = "HIGHLIGHT"),
	EOS_PULL		UMETA(DisplayName = "PULL"),
	EOS_HOLD		UMETA(DisplayName = "HOLD"),
	EOS_PUSH		UMETA(DisplayName = "PUSH"),
};

UENUM(BlueprintType)
enum class EPickState : uint8
{
	EPS_NONE		UMETA(DisplayName = "NONE"),
	EPS_HIGHLIGHT	UMETA(DisplayName = "HIGHLIGHT"),	// when player is looking at the object
	EPS_PICK		UMETA(DisplayName = "PICK"),		// when player presses the interact button --->Attach to player
	EPS_DROP		UMETA(DisplayName = "DROP"),		//when player presses drop button ---->deattaches to player
};


