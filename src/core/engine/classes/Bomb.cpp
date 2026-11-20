#include "Bomb.hpp"

#include "core/engine/Engine.hpp"
#include "core/offsets/Dumper.hpp"

bool Bomb::PlantedDataAvailable() {
	return offsets::plantedC4 != 0;
}

bool Bomb::Update() {

	auto p = Engine::GetProcess();

	if (!p) {
		*this = Bomb{};
		prev_is_planted = false;
		return false;
	}

	auto client = Engine::GetClient();

	// Resolve the C4 carrier via the global weaponC4 pointer
	// client.base + weaponC4 -> ptr to C4 entity -> m_hOwnerEntity (0x520) = pawn handle
	carrier = 0;
	auto c4_ptr = offsets::weaponC4 ? p->read<uintptr_t>(client.base + offsets::weaponC4) : 0;
	if (c4_ptr) {
		if (auto e = p->read<uintptr_t>(c4_ptr))
			carrier = (uintptr_t)p->read<int>(e + 0x520 /* Magic Offset? */);
	}

	this->address = offsets::plantedC4 ? p->read<uintptr_t>(client.base + offsets::plantedC4) : 0;
	this->is_planted = (this->address != 0);

	if (!this->is_planted) {
		this->site = BombSite::Unknown;
		this->time_left = 0.f;
		this->pos = {};
		Bomb::prev_is_planted = false;
		return true;
	}

	auto site = p->read<uint32_t>(this->address + offsets::bomb::m_nBombSite);
	this->site = (site == 1) ? BombSite::B : BombSite::A;

	auto node = p->read<uintptr_t>(this->address + offsets::pawn::m_pGameSceneNode);

	if (node)
		this->pos = p->read<Vec3_t>(node + offsets::bomb::m_vecAbsOrigin);

	if (!Bomb::prev_is_planted) 
		plant_time = std::time(nullptr);

	this->time_left = 41 - (std::time(nullptr) - plant_time);

	Bomb::prev_is_planted = true;
	return true;
}

std::time_t Bomb::plant_time{};
bool Bomb::prev_is_planted = false;
