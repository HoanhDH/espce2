#include "Menu.hpp"

#include "core/engine/cache/Cache.hpp"
#include "core/geometry/Raycast.hpp"
#include "gui/renderer/Renderer.hpp" // Circular dependency
#include "gui/renderer/window/Window.hpp" // Circular dependency
#include "gui/frontend/menu/TriggerHotkey.hpp"
#include "practice/ClickPractice.hpp"
#include "assets/fonts/Icons.h"

namespace {
enum Tab {
    PLAYER,
    WORLD,
    SETTINGS,
    TRIGGER_DEMO
};

struct TabItem
{
    Tab id;
    std::string label;
};

const TabItem tabs[] =
{
    { Tab::PLAYER, std::string(Icons::PERSON) + " Player" },
    { Tab::WORLD, std::string(Icons::GLOBE) + " World" },
    { Tab::SETTINGS, std::string(Icons::SETTINGS) + " Settings" },
    { Tab::TRIGGER_DEMO, std::string(Icons::SETTINGS) + " Trigger Demo" }
};
}

bool Menu::Init() {
	return GetInstance().InitImpl();
}

void Menu::Render() {
	return GetInstance().RenderImpl();
}

void Menu::RenderStartupHelp() {
	return GetInstance().RenderStartupHelpImpl();
}

ImVec2 Menu::GetPos() {
	return GetInstance().pos;
}

ImVec2 Menu::GetSize() {
	return GetInstance().size;
}

bool Menu::InitImpl() {
	SetupStyles();

	LOGF(INFO, "Successfully initialized menu...");
	return true;
}

void Menu::RenderImpl() {
	if (!isSetup)
		return;

	const ImVec2 screen = ImGui::GetIO().DisplaySize;
	static auto color_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None;

#ifdef _DEBUG
	static auto title = "Hoanh Custom | Overlay [DEV]";
#else
	static auto title = "Hoanh Custom | Overlay";
#endif

	ImGui::SetNextWindowSize(ImVec2(600, 370), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(screen.x / 2 - 300, screen.y / 2 - 150), ImGuiCond_FirstUseEver);

	ImGui::GetWindowPos();
	if (ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
		this->pos = ImGui::GetWindowPos();
		this->size = ImGui::GetWindowSize();

		static int active_tab = 0;

		if (ImGui::BeginChild("##main_split"))
		{
			auto size = ImGui::GetContentRegionAvail();

			ImGui::BeginChild("##tab_buttons", ImVec2(120, size.y), true);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.1f, 0.5f));
				for (const auto& tab : tabs)
				{
					bool is_active = (active_tab == tab.id);

					if (is_active)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
					}

					if (ImGui::Button(tab.label.c_str(), ImVec2(-1, 28)))
						active_tab = tab.id;

					if (is_active) 
						ImGui::PopStyleColor(3);
				}
				ImGui::PopStyleVar(1);

				auto space = ImGui::GetContentRegionAvail().y;
				ImGui::SetCursorPosY(ImGui::GetCursorPos().y + space - 16.f * 3);

				ImGui::Dummy(ImVec2(11, 0)); ImGui::SameLine();
				ImGui::TextLinkOpenURL(Icons::DISCORD, "https://discord.gg/pRew8ZDkyp");
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(11, 0)); ImGui::SameLine();
				ImGui::TextLinkOpenURL(Icons::GITHUB, "https://github.com/IMXNOOBX/cs2-external-esp");
				ImGui::Separator();

				ImGui::Checkbox("Enable", &cfg::enabled);
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("##tab_content", ImVec2(0, size.y), true);
			{
				if (active_tab == Tab::PLAYER)
				{
					ImGui::Text("Visuals");
					ImGui::Separator();

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Box", &cfg::esp::box);
						ImGui::BeginDisabled(!cfg::esp::box);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team box color", cfg::esp::colors::box_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy box color", cfg::esp::colors::box_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Skeleton", &cfg::esp::skeleton);
						ImGui::BeginDisabled(!cfg::esp::skeleton);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team skeleton color", cfg::esp::colors::skeleton_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy skeleton color", cfg::esp::colors::skeleton_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Head Tracker", &cfg::esp::head_tracker);
						ImGui::BeginDisabled(!cfg::esp::head_tracker);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team head tracker color", cfg::esp::colors::tracker_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy head tracker color", cfg::esp::colors::tracker_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Tracers", &cfg::esp::tracers);
						ImGui::BeginDisabled(!cfg::esp::tracers);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team tracer color", cfg::esp::colors::tracer_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy tracer color", cfg::esp::colors::tracer_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Health", &cfg::esp::health);
						if (cfg::esp::health)
							ImGui::Checkbox("Health Number", &cfg::esp::health_number);
						ImGui::Checkbox("Armor", &cfg::esp::armor);

						ImGui::Checkbox("Spotted", &cfg::esp::spotted);
						ImGui::SetItemTooltip("Esp will only be visible if the player has been spotted by you");

						ImGui::Checkbox("Show Team", &cfg::esp::team);
					}
					ImGui::EndGroup();

					//ImGui::SameLine();
					ImGui::Spacing();

					ImGui::Text("Flags");
					ImGui::Separator();

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Flashed", &cfg::esp::flags::flashed);
						ImGui::BeginDisabled(!cfg::esp::flags::flashed);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team flashed color", cfg::esp::colors::flags::flashed_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy flashed color", cfg::esp::colors::flags::flashed_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Reloading", &cfg::esp::flags::reloading);
						ImGui::BeginDisabled(!cfg::esp::flags::reloading);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team reloading color", cfg::esp::colors::flags::reloading_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy reloading color", cfg::esp::colors::flags::reloading_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Defusing", &cfg::esp::flags::defusing);
						ImGui::BeginDisabled(!cfg::esp::flags::defusing);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team defusing color", cfg::esp::colors::flags::defusing_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy defusing color", cfg::esp::colors::flags::defusing_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Scoped", &cfg::esp::flags::scoped);
						ImGui::BeginDisabled(!cfg::esp::flags::scoped);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team scoped color", cfg::esp::colors::flags::scoped_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy scoped color", cfg::esp::colors::flags::scoped_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();

						ImGui::Checkbox("Has C4", &cfg::esp::flags::has_c4);
						ImGui::BeginDisabled(!cfg::esp::flags::has_c4);
						{
							ImGui::SameLine();
							ImGui::ColorEdit4("Team C4 color", cfg::esp::colors::flags::c4_team.data(), color_flags);
							ImGui::SameLine();
							ImGui::ColorEdit4("Enemy C4 color", cfg::esp::colors::flags::c4_enemy.data(), color_flags);
						}
						ImGui::EndDisabled();
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					{
						ImGui::Checkbox("Name", &cfg::esp::flags::name);
						ImGui::Checkbox("Money", &cfg::esp::flags::money);
						ImGui::Checkbox("Weapon", &cfg::esp::flags::weapon);
						ImGui::Checkbox("Ammo", &cfg::esp::flags::ammo);
						ImGui::Checkbox("Ping", &cfg::esp::flags::ping);
					}
					ImGui::EndGroup();
				}
				else if (active_tab == Tab::WORLD)
				{
					ImGui::Text("Bomb");
					ImGui::Separator();
					{
						ImGui::Checkbox("Bomb ESP", &cfg::esp::bomb);
						ImGui::SameLine();
						ImGui::ColorEdit4("Bomb color", cfg::esp::colors::bomb.data(), color_flags);
					}
					ImGui::Checkbox("Bomb Location", &cfg::world::bomb::location);
					ImGui::Checkbox("Bomb Timer", &cfg::world::bomb::timer);

					ImGui::Spacing();

					ImGui::Text("Spectator list");
					ImGui::Separator();

					ImGui::Checkbox("Enable", &cfg::world::spectators::enabled);
					if (cfg::world::spectators::enabled) {
						ImGui::Checkbox("Detailed", &cfg::world::spectators::detailed);
						ImGui::Checkbox("Only Self", &cfg::world::spectators::self_only);
						ImGui::SetItemTooltip("Only display users spectating you");
					}

					ImGui::Spacing();

					ImGui::Text("Misc");
					ImGui::Separator();

					ImGui::Checkbox("Crosshair", &cfg::world::crosshair::enabled);
					ImGui::Checkbox("Velocity Graph", &cfg::world::velocity::enabled);
				#ifdef _DEBUG // Part of the velocity graph for developers
					if (cfg::world::velocity::enabled) {
						ImGui::SliderInt("Sample rate", &cfg::world::velocity::sample_rate, 1, 100);
						ImGui::SliderFloat("Sample length", &cfg::world::velocity::sample_length, 1, 20, "%.1f");
					}
				#endif
					ImGui::Spacing();

					ImGui::Text("Radar");
					ImGui::Separator();

					ImGui::Checkbox("Radar", &cfg::world::radar::enabled);
					ImGui::BeginDisabled(!cfg::world::radar::enabled);
					{
						ImGui::SameLine();
						ImGui::SliderFloat("Range", &cfg::world::radar::range, 100.f, 8000.f, "%.0f u");
						ImGui::Checkbox("Disable Rotation", &cfg::world::radar::no_rotate);
					}
					ImGui::EndDisabled();
				}
				else if (active_tab == Tab::TRIGGER_DEMO)
				{
					ImGui::TextUnformatted("Trigger Demo - local preview");
					ImGui::Separator();
					ImGui::Checkbox("Enable prototype", &cfg::trigger_demo::enabled);
					ImGui::Checkbox("Show FIRED on bot head / body alignment", &cfg::trigger_demo::live_head_feedback);
					ImGui::Checkbox("Require game-spotted signal (approximate)", &cfg::trigger_demo::require_spotted);
					ImGui::TextDisabled("Spotted is NOT proof of clear line of sight through walls.");
					ImGui::TextDisabled("The game may mark bots spotted by other players.");
					ImGui::TextDisabled("Read-only visual alert; never clicks or fires a weapon.");
					const char* zones[] = { "Head", "Body", "Head + Body" };
					ImGui::Combo("Target zone", &cfg::trigger_demo::zone, zones, IM_ARRAYSIZE(zones));
					const char* modes[] = { "Hold", "Toggle" };
					ImGui::Combo("Activation mode", &cfg::trigger_demo::activation_mode, modes, IM_ARRAYSIZE(modes));
					ImGui::SliderInt("FIRED display delay (ms)", &cfg::trigger_demo::delay_ms, 0, 500);
					ImGui::TextDisabled("Wait before showing FIRED (0 = immediate).");

					static bool capturing_key = false;
					const ImGuiKey current_key = static_cast<ImGuiKey>(cfg::trigger_demo::hotkey);
					if (ImGui::Button(capturing_key ? "Press a key (Esc cancels)..." : "Assign hotkey"))
						capturing_key = true;
					ImGui::SameLine();
					ImGui::Text("%s", ImGui::GetKeyName(current_key));
					if (capturing_key) {
						if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
							capturing_key = false;
						} else if (ImGui::IsMouseClicked(3)) {
							cfg::trigger_demo::hotkey = static_cast<int>(ImGuiKey_MouseX1);
							capturing_key = false;
						} else if (ImGui::IsMouseClicked(4)) {
							cfg::trigger_demo::hotkey = static_cast<int>(ImGuiKey_MouseX2);
							capturing_key = false;
						} else {
							for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k) {
								const auto key = static_cast<ImGuiKey>(k);
								if (k >= static_cast<int>(ImGuiKey_MouseLeft) &&
									k <= static_cast<int>(ImGuiKey_MouseWheelY))
									continue;
								if (!ImGui::IsKeyPressed(key, false)) continue;
								if (key == ImGuiKey_Insert || key == ImGuiKey_End ||
									key == ImGuiKey_RightShift || !trigger_hotkey::ToVirtualKey(key))
									continue;
								cfg::trigger_demo::hotkey = k;
								capturing_key = false;
								break;
							}
						}
					}
					static bool toggle_active = false;
					static int previous_mode = 0;
					static int previous_hotkey = static_cast<int>(ImGuiKey_F6);
					static int demo_count = 0;
					static double demo_due_time = 0.0;
					static bool demo_pending = false;
					static bool previous_demo_match = false;
					static double demo_fired_until = 0.0;
					if (!cfg::trigger_demo::enabled || previous_mode != cfg::trigger_demo::activation_mode ||
						previous_hotkey != cfg::trigger_demo::hotkey) {
						toggle_active = false;
						demo_pending = false;
						previous_demo_match = false;
						demo_fired_until = 0.0;
					}
					previous_mode = cfg::trigger_demo::activation_mode;
					previous_hotkey = cfg::trigger_demo::hotkey;
					const ImGuiKey active_key = static_cast<ImGuiKey>(cfg::trigger_demo::hotkey);
					const bool key_held = !capturing_key && ImGui::IsKeyDown(active_key);
					const bool key_pressed = !capturing_key && ImGui::IsKeyPressed(active_key, false);
					if (cfg::trigger_demo::enabled && cfg::trigger_demo::activation_mode == 1 && key_pressed)
						toggle_active = !toggle_active;
					const bool active = cfg::trigger_demo::enabled &&
						(cfg::trigger_demo::activation_mode == 0 ? key_held : toggle_active);
					ImGui::Text("Key state: %s | Mode: %s", key_held ? "Pressed" : "Released", active ? "Active" : "Inactive");
					ImGui::TextDisabled("Live FIRED alert uses the assigned hotkey when CS2 is focused.");
					ImGui::TextDisabled("This menu preview is separate from the in-game alert.");

					ImGui::Separator();
					static bool practice_failed = false;
					ImGui::BeginDisabled(!cfg::trigger_demo::enabled || capturing_key);
					if (ImGui::Button("Open offline click practice")) {
						const click_practice::Settings settings{
							trigger_hotkey::ToVirtualKey(active_key), ImGui::GetKeyName(active_key),
							cfg::trigger_demo::delay_ms, cfg::trigger_demo::zone,
							cfg::trigger_demo::activation_mode == 1
						};
						practice_failed = !click_practice::Run(Window::hwnd, settings);
						demo_pending = false;
						previous_demo_match = false;
						toggle_active = false;
					}
					ImGui::EndDisabled();
					if (practice_failed) ImGui::TextUnformatted("Could not open the practice window.");
					ImGui::TextUnformatted("Practice preview (simulated coordinates)");
					static float sample_x = 0.5f;
					static float sample_y = 0.5f;
					ImGui::SliderFloat("Dummy X", &sample_x, 0.0f, 1.0f);
					ImGui::SliderFloat("Dummy Y", &sample_y, 0.0f, 1.0f);
					const float dx = std::abs(sample_x - 0.5f);
					const bool head_match = dx < 0.05f && std::abs((sample_y - 0.12f) - 0.5f) < 0.05f;
					const bool body_match = dx < 0.10f && std::abs((sample_y + 0.07f) - 0.5f) < 0.12f;
					const bool zone_match = cfg::trigger_demo::zone == 0 ? head_match :
						(cfg::trigger_demo::zone == 1 ? body_match : (head_match || body_match));
					const double now = ImGui::GetTime();
					if (!active) {
						demo_pending = false;
						previous_demo_match = false;
						demo_fired_until = 0.0;
					} else {
						if (zone_match && !previous_demo_match && !demo_pending) {
							demo_pending = true;
							demo_due_time = now + cfg::trigger_demo::delay_ms / 1000.0;
						}
						previous_demo_match = zone_match;
						if (demo_pending && now >= demo_due_time) {
							++demo_count; // Visual simulation only, never emits mouse or keyboard input.
							demo_pending = false;
							demo_fired_until = now + 0.45;
						}
					}
					ImGui::Text("Demo status: %s", demo_pending ? "WAITING (display delay)" :
						(now < demo_fired_until ? "FIRED (preview only)" :
						(active && zone_match ? "MATCH (preview only)" : "Idle")));
					ImGui::Text("Simulated activations: %d", demo_count);
					if (ImGui::Button("Reset counter"))
						demo_count = 0;

					ImGui::Separator();
					ImGui::TextUnformatted("Raycast laboratory (simulated obstacle)");
					ImGui::TextDisabled("Uses test geometry, NOT collision data from CS2.");
					static bool demo_wall_enabled = true;
					static float demo_wall_height = 1.35f;
					static float demo_wall_offset = 0.0f;
					ImGui::Checkbox("Place test wall", &demo_wall_enabled);
					ImGui::SliderFloat("Wall height", &demo_wall_height, 0.0f, 2.5f, "%.2f m");
					ImGui::SliderFloat("Wall lateral offset", &demo_wall_offset, -2.0f, 2.0f, "%.2f m");
					const demo_raycast::Point3 eye{0.0f, 0.0f, 1.60f};
					const demo_raycast::Point3 target_head{10.0f, 0.0f, 1.60f};
					const demo_raycast::Point3 target_body{10.0f, 0.0f, 1.05f};
					const demo_raycast::Box3 wall{
						{4.75f, demo_wall_offset - 0.65f, 0.0f},
						{5.25f, demo_wall_offset + 0.65f, demo_wall_height}
					};
					const bool head_clear = !demo_wall_enabled ||
						demo_raycast::ClearLineOfSight(eye, target_head, wall);
					const bool body_clear = !demo_wall_enabled ||
						demo_raycast::ClearLineOfSight(eye, target_body, wall);
					ImGui::Text("Test head visibility: %s", head_clear ? "CLEAR" : "BLOCKED");
					ImGui::Text("Test body visibility: %s", body_clear ? "CLEAR" : "BLOCKED");
					ImGui::TextDisabled("Independent raycast test; not wired into live game detection.");

					ImGui::Separator();
					ImGui::TextUnformatted("Live opponent recognition (read-only)");
					ImGui::TextDisabled("Uses current CS2 player snapshot; never clicks or aims.");
					const Snapshot game_snapshot = Cache::CopySnapshot();
					const bool local_ready = game_snapshot.local.index >= 0 &&
						(game_snapshot.local.team == 2 || game_snapshot.local.team == 3);
					if (!local_ready) {
						ImGui::TextUnformatted("Status: waiting for local player/team data...");
					} else {
						int opponents_alive = 0;
						int enemy_bots_alive = 0;
						for (const auto& participant : game_snapshot.players) {
							if (!participant.alive || participant.localplayer)
								continue;
							if (participant.team != 2 && participant.team != 3)
								continue;
							if (participant.team == game_snapshot.local.team)
								continue;
							++opponents_alive;
							if (participant.bot)
								++enemy_bots_alive;
						}
						ImGui::Text("Opponents alive: %d", opponents_alive);
						ImGui::Text("Enemy bots alive: %d", enemy_bots_alive);
						ImGui::Text("Detection: %s", opponents_alive > 0 ? "OPPONENTS FOUND" : "No opponents in snapshot");
						ImGui::TextDisabled("Counts update from game data; these do not validate line of sight.");
					}
				}
				else if (active_tab == Tab::SETTINGS)
				{
					ImGui::Text("Misc");
					ImGui::Separator();

					if (ImGui::Checkbox("Streamproof", &cfg::settings::streamproof))
					{
						Window::SetAffinity(
							Window::hwnd,
							cfg::settings::streamproof ? WindowAffinity::Invisible : WindowAffinity::Disabled
						);
					}

					ImGui::Checkbox("Watermark", &cfg::settings::watermark);

					if (ImGui::Checkbox("VSync", &cfg::settings::vsync))
						Window::vsync = cfg::settings::vsync;

					ImGui::Checkbox("Free CPU", &cfg::settings::free_cpu);
					ImGui::SetItemTooltip("Let the CPU sleep to Free Resources\nNOTE: might cause performance issues in lower end computers!");

					ImGui::Text("Notes");
					ImGui::Separator();
					ImGui::TextWrapped(
						"If you experience bad performance/lag try the following:\n"
						"\t- Disable ESP VSync: Look up > VSync: Un-Check\n"
						"\t- Disable VSync in game: ...Advanced Video > V-Sync: Disabled\n"
						"\t- Last Resort: Disable \"Free CPU\" option, it will inpact on your overall performace, but improve latency\n"
					);

#ifdef _DEBUG
					ImGui::Text("Dev");
					ImGui::Separator();

					if (ImGui::Checkbox("Console", &cfg::dev::console))
						if (!cfg::dev::console) LogHelper::Free();

					static int key_out;
					if (ImGui::Button("Open Menu Key"))
					{
						for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; i++)
						{
							if (ImGui::IsKeyPressed((ImGuiKey)i))
							{
								key_out = i;
								LOGF(VERBOSE, "Changed the open menu key to {}", key_out);
								break;
							}
						}
					}

					ImGui::SliderInt("Cache Refresh Rate", &cfg::dev::cache_refresh_rate, 0, 100, "%dms");
					ImGui::Checkbox("Force Show Flags", &cfg::dev::force_show_flags);
#endif
				}
			}
			ImGui::EndChild();
			ImGui::EndChild();
		}

	}

	ImGui::End();
}

void Menu::SetupStyles() {
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

	style.Colors[ImGuiCol_WindowBg] = ImColor(10, 10, 10);
	style.Colors[ImGuiCol_ChildBg] = ImColor(10, 10, 10);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImColor(50, 50, 50);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	style.Colors[ImGuiCol_FrameBg] = ImColor(75, 75, 75);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);

	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.75f, 0.18f, 1.00f);

	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.75f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.86f, 0.38f, 1.00f);

	style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.48f, 0.34f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.64f, 0.45f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.29f, 0.30f, 0.31f, 0.95f);

	style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.38f, 0.28f, 0.11f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.75f, 0.18f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.75f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	style.FrameBorderSize = 1.0f;

	// Window & Frame
	style.WindowRounding = 12.f;
	style.ChildRounding = 10.f;

	style.FrameRounding = 5.f;
	style.PopupRounding = 5.f;

	style.GrabRounding = 3.f;

	auto& io = ImGui::GetIO();

	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f);

	ImFontConfig merge_icon_cfg{};
	merge_icon_cfg.FontDataOwnedByAtlas = false;
	merge_icon_cfg.MergeMode = true;
	merge_icon_cfg.GlyphOffset = Vec2_t(0, 3.5f);

	// the icons will use the size specified when getting added so it ignores the base size
	static const ImWchar icon_ranges[] = { 0xE100, 0xE108, 0 };
	io.Fonts->AddFontFromMemoryTTF(icons_font, icons_font_len, 20.f, &merge_icon_cfg, icon_ranges);
}

void Menu::RenderStartupHelpImpl() {
	static bool has_opened_menu = false;

	if (has_opened_menu)
		return;

	auto& io = ImGui::GetIO();
	auto screen = io.DisplaySize;
	auto d = ImGui::GetBackgroundDrawList();

	if (Renderer::IsOpen())
		has_opened_menu = true;

	auto help = "To OPEN the menu, Use Insert or Right Shift keys"
		"\n\t\t\t\tTo CLOSE, press the End key";
	auto size = ImGui::CalcTextSize(help);

	d->AddText(
		ImVec2(screen.x / 2 - size.x / 2, 80),
		IM_COL32(255, 255, 255, 255),
		help
	);
}
