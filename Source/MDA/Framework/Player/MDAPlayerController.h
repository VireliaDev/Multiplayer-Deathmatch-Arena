// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MDAPlayerController.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class MDA_API AMDAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> PlayerInputMappingContext;
	
	//Forwarding to ability system for ability activation
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
protected:
	virtual void SetupInputComponent() override;
	
	
	
};
