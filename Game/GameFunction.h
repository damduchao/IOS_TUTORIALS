#pragma once
#include <vector>
#include <cmath>

#define FIELDFUNCTION(type, object, offset) *(type *)((uint64_t)object + offset)
#define const_field(type, object, offset) *reinterpret_cast<type *>(reinterpret_cast<uintptr_t>(object) + offset)

// --- Global Variables ---
struct Vars_t {
    bool ProcessPlayers, Speedhack;
    float iIndex;
    float SpeedMultiplier = 2.0f;
    bool Lines, OutOfScreen, Radar, NoDelay, Box;
    bool InstantTask;
    bool CameraZoom;
    float CameraSize = 5.0f;
    bool Wallhack, NoFog, NoKillCooldown, SabotageSpam;
    bool ESP_Box = true, ESP_Name = true, ESP_Distance = true;
    bool ESP_Status = true, ESP_Role = true, ESP_Lines = true;
    bool ESP_Vent = false, ESP_Ghost = true, ESP_Bodies = true;
    bool SeeGhosts = false;
    float ESP_MaxDistance = 100.0f;
} Vars;

std::vector<void *> players;

class game_sdk_t {
public:
    void init();

    void* (*get_type)(monoString *);
    monoArray<void *> *(*find_objects)(void *);
    void* (*transform)(void *);
    void* (*camera)();
    
    Vector3 (*position)(void *);
    Vector3 (*forward)(void *);
    Quaternion (*rotation)(void *);
    Vector3 (*world_to_viewport)(void *, Vector3);
};
game_sdk_t *game_sdk = new game_sdk_t();

void game_sdk_t::init() {
    this->get_type = Resolve::Get("mscorlib.dll")->Get("Type", "System")->Get<Resolve::Method>("GetType", { "System.String" })->Cast<void*, monoString*>();
    this->find_objects = Resolve::Get("UnityEngine.CoreModule.dll")->Get("Object", "UnityEngine")->Get<Resolve::Method>("FindObjectsOfType", { "System.Type" })->Cast<monoArray<void *> *, void*>();
    this->position = Resolve::Get("UnityEngine.CoreModule.dll")->Get("Transform", "UnityEngine")->Get<Resolve::Method>("get_position")->Cast<Vector3, void*>();
    this->transform = Resolve::Get("UnityEngine.CoreModule.dll")->Get("Component", "UnityEngine")->Get<Resolve::Method>("get_transform")->Cast<void*, void*>();
    this->camera = Resolve::Get("UnityEngine.CoreModule.dll")->Get("Camera", "UnityEngine")->Get<Resolve::Method>("get_main")->Cast<void*>();
    this->world_to_viewport = Resolve::Get("UnityEngine.CoreModule.dll")->Get("Camera", "UnityEngine")->Get<Resolve::Method>("WorldToViewportPoint", { "UnityEngine.Vector3" })->Cast<Vector3, void*, Vector3>();
}

void apply_speedhack() {
    if (!Vars.Speedhack || Vars.SpeedMultiplier <= 1.0f) return;

    static void* (*get_component_transform)(void *) = nullptr;
    static Vector3 (*get_position)(void *) = nullptr;
    static void (*set_position)(void *, Vector3) = nullptr;
    static Vector3 last_position = {0, 0, 0};
    static bool initialized = false;

    if (!get_component_transform) {
        get_component_transform = reinterpret_cast<void* (*)(void *)>(getRealOffset(ENCRYPTOFFSET("0x644041C")));
        get_position = reinterpret_cast<Vector3 (*)(void *)>(getRealOffset(ENCRYPTOFFSET("0x6458B6C")));
        set_position = reinterpret_cast<void (*)(void *, Vector3)>(getRealOffset(ENCRYPTOFFSET("0x6458CA0")));
    }

    if (!get_component_transform || !get_position || !set_position) return;

    auto type = game_sdk->get_type(il2cpp_string_new("Handlers.GameHandlers.PlayerHandlers.PlayableEntity, Assembly-CSharp"));
    if (!type) return;

    const auto &playerarray = game_sdk->find_objects(type);
    if (!playerarray) return;

    for (const auto &player : playerarray->toCPPlist()) {
        if (!player) continue;

        bool isLocal = FIELDFUNCTION(bool, player, 0x98);
        if (!isLocal) continue;

        void *transform = get_component_transform(player);
        if (!transform) break;

        Vector3 current_pos = get_position(transform);
        if (initialized) {
            float delta_x = current_pos.x - last_position.x;
            float delta_y = current_pos.y - last_position.y;

            if ((delta_x != 0.0f || delta_y != 0.0f) && fabs(delta_x) < 5.0f && fabs(delta_y) < 5.0f) {
                float boost = Vars.SpeedMultiplier - 1.0f;
                current_pos.x += delta_x * boost;
                current_pos.y += delta_y * boost;
                set_position(transform, current_pos);
            }
        }
        last_position = current_pos;
        initialized = true;
        break;
    }
}

void render_lop() {
    apply_speedhack();
}