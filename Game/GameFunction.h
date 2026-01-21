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
    
    bool (*object_alive)(void *);
    void (*CompleteTask)(void *, monoString *, bool, bool, bool, bool);
    void (*OverrideOrthographicSize)(void *, float);
    int16_t (*GetRoleType)(void *);
    int16_t (*GGDRole_GetType)(void *);
};
game_sdk_t *game_sdk = new game_sdk_t();

void game_sdk_t::init() {
    this->get_type = (void* (*)(monoString *))getRealOffset(ENCRYPTOFFSET("0x5471A20"));
    this->find_objects = (monoArray<void *> *(*)(void *))getRealOffset(ENCRYPTOFFSET("0x644D04C"));
    this->position = (Vector3 (*)(void *))getRealOffset(ENCRYPTOFFSET("0x6458B6C"));
    this->transform = (void* (*)(void *))getRealOffset(ENCRYPTOFFSET("0x644041C"));
    this->camera = (void* (*)())getRealOffset(ENCRYPTOFFSET("0x63EC7DC"));
    this->world_to_viewport = (Vector3 (*)(void *, Vector3))getRealOffset(ENCRYPTOFFSET("0x63EC1C8"));
    this->CompleteTask = (void (*)(void *, monoString *, bool, bool, bool, bool))getRealOffset(ENCRYPTOFFSET("0x2501D08"));
    this->OverrideOrthographicSize = (void (*)(void *, float))getRealOffset(ENCRYPTOFFSET("0x2526480"));
    this->GetRoleType = (int16_t (*)(void *))getRealOffset(ENCRYPTOFFSET("0x2542AEC"));
    this->GGDRole_GetType = (int16_t (*)(void *))getRealOffset(ENCRYPTOFFSET("0x239ADB8"));
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

void apply_instant_task() {
    if (!Vars.InstantTask) return;

    auto playerType = game_sdk->get_type(il2cpp_string_new("Handlers.GameHandlers.PlayerHandlers.PlayableEntity, Assembly-CSharp"));
    if (!playerType) return;

    const auto &playerArray = game_sdk->find_objects(playerType);
    if (!playerArray) return;

    void *localPlayer = nullptr;
    for (const auto &player : playerArray->toCPPlist()) {
        if (!player) continue;
        if (FIELDFUNCTION(bool, player, 0x98)) {
            localPlayer = player;
            break;
        }
    }

    if (!localPlayer) return;

    auto tasksHandlerType = game_sdk->get_type(il2cpp_string_new("Handlers.GameHandlers.TaskHandlers.TasksHandler, Assembly-CSharp"));
    if (!tasksHandlerType) return;

    const auto &handlersArray = game_sdk->find_objects(tasksHandlerType);
    if (!handlersArray) return;

    for (const auto &handler : handlersArray->toCPPlist()) {
        if (!handler) continue;
        if (FIELDFUNCTION(void *, handler, 0x10) != localPlayer) continue;

        void *tasksList = FIELDFUNCTION(void *, handler, 0x38);
        if (!tasksList) continue;

        auto *tasksItems = FIELDFUNCTION(monoArray<void *> *, tasksList, 0x10);
        int tasksCount = FIELDFUNCTION(int, tasksList, 0x18);
        if (!tasksItems || tasksCount <= 0) continue;

        for (int i = 0; i < tasksCount; i++) {
            void *task = tasksItems->at(i);
            if (!task) continue;
            auto *taskId = FIELDFUNCTION(monoString *, task, 0x10);
            if (taskId) game_sdk->CompleteTask(handler, taskId, false, false, false, false);
        }
        break;
    }
    Vars.InstantTask = false;
}

void apply_no_kill_cooldown() {
    if (!Vars.NoKillCooldown) return;

    auto type = game_sdk->get_type(il2cpp_string_new("UICooldownButton, Assembly-CSharp"));
    if (!type) return;

    static void (*ForceInteractable)(void *, bool) = nullptr;
    static void (*ForceInteractableEvenInCooldown)(void *, bool) = nullptr;

    if (!ForceInteractable) {
        ForceInteractable = reinterpret_cast<void (*)(void *, bool)>(getRealOffset(ENCRYPTOFFSET("0x1F12048")));
        ForceInteractableEvenInCooldown = reinterpret_cast<void (*)(void *, bool)>(getRealOffset(ENCRYPTOFFSET("0x1F12050")));
    }

    const auto &buttonArray = game_sdk->find_objects(type);
    if (!buttonArray) return;

    for (const auto &button : buttonArray->toCPPlist()) {
        if (!button) continue;
        FIELDFUNCTION(bool, button, 0x98) = false;
        FIELDFUNCTION(bool, button, 0x99) = false;
        FIELDFUNCTION(bool, button, 0x118) = true;
        FIELDFUNCTION(bool, button, 0x119) = true;
        FIELDFUNCTION(float, button, 0x9C) = 0.0f;
        if (ForceInteractable) ForceInteractable(button, true);
        if (ForceInteractableEvenInCooldown) ForceInteractableEvenInCooldown(button, true);
    }
}

void apply_camera_zoom() {
    if (!Vars.CameraZoom) return;

    auto localPlayerType = game_sdk->get_type(il2cpp_string_new("Handlers.GameHandlers.PlayerHandlers.LocalPlayer, Assembly-CSharp"));
    if (!localPlayerType) return;

    const auto &localPlayerArray = game_sdk->find_objects(localPlayerType);
    if (!localPlayerArray) return;

    for (const auto &localPlayer : localPlayerArray->toCPPlist()) {
        if (localPlayer) {
            game_sdk->OverrideOrthographicSize(localPlayer, Vars.CameraSize);
            break;
        }
    }
}

void apply_wallhack() {
    if (!Vars.Wallhack) return;

    static void (*set_is_trigger)(void *, bool) = nullptr;
    if (!set_is_trigger) set_is_trigger = reinterpret_cast<void (*)(void *, bool)>(getRealOffset(ENCRYPTOFFSET("0x650C2E0")));
    if (!set_is_trigger) return;

    auto type = game_sdk->get_type(il2cpp_string_new("Handlers.GameHandlers.PlayerHandlers.PlayableEntity, Assembly-CSharp"));
    if (!type) return;

    const auto &playerarray = game_sdk->find_objects(type);
    if (!playerarray) return;

    for (const auto &player : playerarray->toCPPlist()) {
        if (!player) continue;
        if (!FIELDFUNCTION(bool, player, 0x98)) continue;

        void *bodyCollider = FIELDFUNCTION(void *, player, 0x2D0);
        void *playerCollider = FIELDFUNCTION(void *, player, 0x2D8);
        void *wallCheckCollider = FIELDFUNCTION(void *, player, 0x2E0);

        if (bodyCollider) set_is_trigger(bodyCollider, true);
        if (playerCollider) set_is_trigger(playerCollider, true);
        if (wallCheckCollider) set_is_trigger(wallCheckCollider, true);
        break;
    }
}

void apply_nofog() {
    if (!Vars.NoFog) return;

    auto type = game_sdk->get_type(il2cpp_string_new("Handlers.GameHandlers.PlayerHandlers.PlayableEntity, Assembly-CSharp"));
    if (!type) return;

    const auto &playerarray = game_sdk->find_objects(type);
    if (!playerarray) return;

    for (const auto &player : playerarray->toCPPlist()) {
        if (player && FIELDFUNCTION(bool, player, 0x98)) {
            FIELDFUNCTION(bool, player, 0xF4) = false;
            break;
        }
    }
}

void apply_sabotage_spam() {
    if (!Vars.SabotageSpam) return;

    auto type = game_sdk->get_type(il2cpp_string_new("Handlers.GameHandlers.SpecialBehaviour.SabotageHandlers.SabotageButtonHandler, Assembly-CSharp"));
    if (!type) return;

    static void (*ResetCooldown)(void *, bool) = nullptr;
    if (!ResetCooldown) ResetCooldown = reinterpret_cast<void (*)(void *, bool)>(getRealOffset(ENCRYPTOFFSET("0x24E82C8")));

    const auto &buttonArray = game_sdk->find_objects(type);
    if (!buttonArray) return;

    for (const auto &button : buttonArray->toCPPlist()) {
        if (!button) continue;
        FIELDFUNCTION(float, button, 0x74) = 0.0f;
        FIELDFUNCTION(bool, button, 0x71) = true;
        FIELDFUNCTION(bool, button, 0x70) = false;
        if (ResetCooldown) ResetCooldown(button, false);
    }
}

void render_lop() {
    apply_speedhack();
    apply_instant_task();
    apply_wallhack();
    apply_camera_zoom();
    apply_nofog();
    apply_no_kill_cooldown();
    apply_sabotage_spam();
}