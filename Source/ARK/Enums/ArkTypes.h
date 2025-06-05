#pragma once
UENUM(Blueprintable)
enum class EItemType : uint8
{
	Resource,
	Equipable,
	Armor,
	Consumable,
	Buildable
};

UENUM(Blueprintable)
enum class EItemRarity : uint8
{
	Common,
	UnCommon,
	Rare,
	Epic,
	Legendary
};

UENUM(Blueprintable)
enum class EArmorType : uint8
{
	Helmet,
	Chest,
	Pants,
	Boots
};