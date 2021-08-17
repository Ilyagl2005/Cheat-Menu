/*
	Author: Grinch_
	Copyright GPLv3 2019-2021
	Required:
		DirectX 9 SDK
		Plugin SDK
		Build Tools 2019 (v142)
		Windows SDK
*/

#pragma once
#include "Animation.h"
#include "Hook.h"
#include "Menu.h"
#include "Teleport.h"
#include "Player.h"
#include "Ped.h"
#include "Vehicle.h"
#include "Weapon.h"
#include "Game.h"

#ifdef GTASA
#include "Visual.h"
class CheatMenu : Hook, Animation, Game, Menu, Ped, Player, Teleport, Vehicle, Visual, Weapon
#elif GTAVC
class CheatMenu : Hook, Animation, Game, Menu, Player, Ped, Teleport, Vehicle, Weapon
#endif

{
private:
#ifdef GTASA
	inline static CallbackTable header
	{
		{"Teleport", &Teleport::Draw}, {"Player", &Player::Draw}, {"Ped", &Ped::Draw},
		{"Animation", &Animation::Draw}, {"Vehicle", &Vehicle::Draw}, {"Weapon", &Weapon::Draw},
		{"Game", &Game::Draw}, {"Visual", &Visual::Draw}, {"Menu", &Menu::Draw}
	};
#elif GTAVC
	inline static CallbackTable header
	{
		{"Teleport", &Teleport::Draw}, {"Player", &Player::Draw}, {"Ped", &Ped::Draw},
		{"Dummy", nullptr}, {"Vehicle", &Vehicle::Draw}, {"Weapon", &Weapon::Draw}, 
		{"Game", &Game::Draw}, {"Menu", &Menu::Draw}
	};
#endif

	static void ApplyStyle();
	static void DrawWindow();

public:
	CheatMenu();
};
