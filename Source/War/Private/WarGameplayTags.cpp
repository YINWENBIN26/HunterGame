// Fill out your copyright notice in the Description page of Project Settings.

#include "WarGameplayTags.h"
namespace WarGameplayTags
{
	/** Input Tags */
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipSword, "InputTag.EquipSword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequipSword, "InputTag.UnequipSword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_LightAttack_Sword, "InputTag.LightAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_HeavyAttack_Sword, "InputTag.HeavyAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Roll, "InputTag.Roll");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SwitchTarget, "InputTag.SwitchTarget");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialWeaponAbility_Light, "InputTag.SpecialWeaponAbility.Light");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialWeaponAbility_Heavy, "InputTag.SpecialWeaponAbility.Heavy");
	
	
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable, "InputTag.Toggleable");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_TragetLock, "InputTag.Toggleable.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_Rage, "InputTag.Toggleable.Rage");


	
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MustBeHeld, "InputTag.MustBeHeld");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MustBeHeld_Defense, "InputTag.MustBeHeld.Defense");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sealing_Ten, "InputTag.Sealing.Ten");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sealing_Chi, "InputTag.Sealing.Chi");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sealing_Jin, "InputTag.Sealing.Jin");


	
	/** Player tags */
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Sword, "Player.Ability.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_UnEquip_Sword, "Player.Ability.UnEquip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Light_Sword, "Player.Ability.Attack.Light.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Heavy_Sword, "Player.Ability.Attack.Heavy.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_HitPause, "Player.Ability.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Roll, "Player.Ability.Roll");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Defense, "Player.Ability.Defense");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_TargetLock, "Player.Ability.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Rage, "Player.Ability.Rage");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Light, "Player.Ability.SpecialWeaponAbility.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Heavy, "Player.Ability.SpecialWeaponAbility.Heavy");

	
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Sealing_Ten, "Player.Ability.Sealing.Ten");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Sealing_Chi, "Player.Ability.Sealing.Chi");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Sealing_Jin, "Player.Ability.Sealing.Jin");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Ninjutsu_Fire, "Player.Ability.Ninjutsu.Fire");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_CoolDown_SpecialSKill_Light, "Player.CoolDown.SpecialSKill.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_CoolDown_SpecialSKill_Heavy, "Player.CoolDown.SpecialSKill.Heavy");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Sword, "Player.Weapon.Sword");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Sword, "Player.Event.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Sword, "Player.Event.Unequip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_HitPause, "Player.Event.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Left, "Player.Event.SwitchTarget.Left");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Right, "Player.Event.SwitchTarget.Right");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_AOE, "Player.Event.AOE");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_JumpToFinisher, "Player.Status.JumpToFinisher");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rolling, "Player.Status.Rolling");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Defense, "Player.Status.Defense");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_TargetLock, "Player.Status.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Activating, "Player.Status.Rage.Activating");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Active, "Player.Status.Rage.Active");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_Full, "Player.Status.Rage.Full");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rage_None, "Player.Status.Rage.None");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Light, "Player.SetByCaller.AttackType.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Heavy, "Player.SetByCaller.AttackType.Heavy");
	
	/** Enemy tags */
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Melee, "Enemy.Ability.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Ranged, "Enemy.Ability.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_SummonEnemies, "Enemy.Ability.SummonEnemies");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_RushAttack, "Enemy.Ability.RushAttack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_King_Ability_GroupAttack, "Enemy.King.Ability.GroupAttack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_King_Ability_GroupMove, "Enemy.King.Ability.GroupMove");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_FollowMove, "Enemy.Ability.FollowMove");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon, "Enemy.Weapon");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_SummonEnemies, "Enemy.Event.SummonEnemies");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Strafing, "Enemy.Status.Strafing");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Run, "Enemy.Status.Run");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_UnderAttack, "Enemy.Status.UnderAttack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_RunAway, "Enemy.Ability.RunAway");

	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Wolf_Status_Relaxed, "Enemy.Wolf.Status.Relaxed");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Wolf_Status_Alert, "Enemy.Wolf.Status.Alert");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Wolf_Status_Attack, "Enemy.Wolf.Status.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Wolf_Status_FollowMoving, "Enemy.Wolf.Status.FollowMoving");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Wolf_Status_FollowMoving_AtRight, "Enemy.Wolf.Status.FollowMoving.AtRight");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Wolf_Status_FollowMoving_AtLeft, "Enemy.Wolf.Status.FollowMoving.AtLeft");

	
	
	/** Shared Tags */

	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_SpawnProjectile, "Shared.Event.SpawnProjectile");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_AbilityDone, "Shared.Event.AbilityDone");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Dead, "Shared.Status.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_MeleeHit_Front, "Shared.Status.MeleeHit.Front");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_MeleeHit_Left, "Shared.Status.MeleeHit.Left");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_MeleeHit_Right, "Shared.Status.MeleeHit.Right");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_MeleeHit_Back, "Shared.Status.MeleeHit.Back");

	
	/** Game Data tags **/
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_GameMap,"GameData.Level.GameMap");
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_MainMenuMap,"GameData.Level.MainMenuMap");

}

