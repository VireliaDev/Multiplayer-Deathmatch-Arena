#include "MDAGameplayTags.h"

namespace GameTags
{
	//////Input Tags//////
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,				    "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look,				    "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump,				    "InputTag.Jump");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Crouch,				    "InputTag.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint,				    "InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Aim,				    "InputTag.Aim");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Reload,		    "InputTag.Ability.Reload");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Shoot,		    "InputTag.Ability.Shoot");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_EquipPrimary,   "InputTag.Ability.EquipPrimary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_EquipSecondary, "InputTag.Ability.EquipSecondary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UI_WeaponMenu,          "InputTag.UI.WeaponMenu")

	
	//////Movement//////
	//State
	UE_DEFINE_GAMEPLAY_TAG(Movement_State,				  "Movement.State");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_Sprinting,      "Movement.State.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_Crouching,      "Movement.State.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_Aiming,         "Movement.State.Aiming");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_Mantling,       "Movement.State.Mantling");
	UE_DEFINE_GAMEPLAY_TAG(Movement_State_SprintRecovery, "Movement.State.SprintRecovery");
	
	
	////Player State////
	UE_DEFINE_GAMEPLAY_TAG(State_Dead,					  "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_Equipping,				  "State.Equipping");
	
	////Events////
	UE_DEFINE_GAMEPLAY_TAG(Event_Weapon_EquipPrimary,					  "Event.Weapon.EquipPrimary");
}