#include "MDAGameplayTags.h"

namespace GameTags
{
	//////Input Tags//////
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,				  "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look,				  "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump,				  "InputTag.Jump");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Crouch,				  "InputTag.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint,				  "InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Aim,				  "InputTag.Aim");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Reload,				  "InputTag.Ability.Reload");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Shoot,				  "InputTag.Ability.Shoot");

	
	//////Movement//////
	//State
	UE_DEFINE_GAMEPLAY_TAG(Movement_State,				  "Movement.State");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_Idle,				  "Movement.State.Idle");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_Walking,				  "Movement.State.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_Falling,				  "Movement.State.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_Sprinting,				  "Movement.State.Sprinting");
	
}