/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019-2023 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */

#include "RmlUi_Platform_SDL.h"
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Input.h>

// ============================================================================
// SystemInterface_SDL Implementation
// ============================================================================

SystemInterface_SDL::SystemInterface_SDL()
    : window_(nullptr)
    , cursor_default_(nullptr)
    , cursor_move_(nullptr)
    , cursor_pointer_(nullptr)
    , cursor_resize_(nullptr)
    , cursor_cross_(nullptr)
    , cursor_text_(nullptr)
    , cursor_unavailable_(nullptr)
{
    start_time_ = SDL_GetPerformanceCounter();
}

SystemInterface_SDL::~SystemInterface_SDL() {
    if (cursor_default_) SDL_FreeCursor(cursor_default_);
    if (cursor_move_) SDL_FreeCursor(cursor_move_);
    if (cursor_pointer_) SDL_FreeCursor(cursor_pointer_);
    if (cursor_resize_) SDL_FreeCursor(cursor_resize_);
    if (cursor_cross_) SDL_FreeCursor(cursor_cross_);
    if (cursor_text_) SDL_FreeCursor(cursor_text_);
    if (cursor_unavailable_) SDL_FreeCursor(cursor_unavailable_);
}

void SystemInterface_SDL::SetWindow(SDL_Window* window) {
    window_ = window;

    // Initialize cursors
    cursor_default_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    cursor_move_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    cursor_pointer_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    cursor_resize_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
    cursor_cross_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    cursor_text_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    cursor_unavailable_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
}

double SystemInterface_SDL::GetElapsedTime() {
    Uint64 now = SDL_GetPerformanceCounter();
    double elapsed = (double)(now - start_time_) / (double)SDL_GetPerformanceFrequency();
    return elapsed;
}

void SystemInterface_SDL::SetMouseCursor(const Rml::String& cursor_name) {
    if (!window_) return;

    SDL_Cursor* cursor = cursor_default_;

    if (cursor_name == "move")
        cursor = cursor_move_;
    else if (cursor_name == "pointer" || cursor_name == "hand")
        cursor = cursor_pointer_;
    else if (cursor_name == "resize")
        cursor = cursor_resize_;
    else if (cursor_name == "cross")
        cursor = cursor_cross_;
    else if (cursor_name == "text")
        cursor = cursor_text_;
    else if (cursor_name == "unavailable")
        cursor = cursor_unavailable_;

    if (cursor)
        SDL_SetCursor(cursor);
}

void SystemInterface_SDL::SetClipboardText(const Rml::String& text) {
    SDL_SetClipboardText(text.c_str());
}

void SystemInterface_SDL::GetClipboardText(Rml::String& text) {
    char* raw_text = SDL_GetClipboardText();
    if (raw_text) {
        text = raw_text;
        SDL_free(raw_text);
    }
}

// ============================================================================
// RmlSDL Namespace - Input Event Processing
// ============================================================================

static SystemInterface_SDL* g_system_interface = nullptr;

bool RmlSDL::Initialize(SDL_Window* window) {
    if (!g_system_interface) {
        g_system_interface = new SystemInterface_SDL();
        g_system_interface->SetWindow(window);
        Rml::SetSystemInterface(g_system_interface);
    }
    return true;
}

void RmlSDL::Shutdown() {
    if (g_system_interface) {
        Rml::SetSystemInterface(nullptr);
        delete g_system_interface;
        g_system_interface = nullptr;
    }
}

bool RmlSDL::ProcessEvent(Rml::Context* context, SDL_Event& event) {
    if (!context)
        return false;

    bool result = true;

    switch (event.type) {
    case SDL_MOUSEMOTION:
        result = context->ProcessMouseMove(event.motion.x, event.motion.y, GetKeyModifiers((SDL_Keymod)SDL_GetModState()));
        break;

    case SDL_MOUSEBUTTONDOWN:
        result = context->ProcessMouseButtonDown(event.button.button - 1, GetKeyModifiers((SDL_Keymod)SDL_GetModState()));
        break;

    case SDL_MOUSEBUTTONUP:
        result = context->ProcessMouseButtonUp(event.button.button - 1, GetKeyModifiers((SDL_Keymod)SDL_GetModState()));
        break;

    case SDL_MOUSEWHEEL:
        result = context->ProcessMouseWheel(float(-event.wheel.y), GetKeyModifiers((SDL_Keymod)SDL_GetModState()));
        break;

    case SDL_KEYDOWN: {
        Rml::Input::KeyIdentifier key = ConvertKey(event.key.keysym.sym);
        int key_modifier = GetKeyModifiers((SDL_Keymod)event.key.keysym.mod);

        // Handle text input for printable keys
        if (key == Rml::Input::KI_UNKNOWN && event.key.keysym.sym >= 32 && event.key.keysym.sym < 127) {
            result = context->ProcessTextInput((Rml::Character)event.key.keysym.sym);
        } else {
            result = context->ProcessKeyDown(key, key_modifier);
        }
        break;
    }

    case SDL_KEYUP: {
        Rml::Input::KeyIdentifier key = ConvertKey(event.key.keysym.sym);
        int key_modifier = GetKeyModifiers((SDL_Keymod)event.key.keysym.mod);
        result = context->ProcessKeyUp(key, key_modifier);
        break;
    }

    case SDL_TEXTINPUT:
        result = context->ProcessTextInput(Rml::String(&event.text.text[0]));
        break;

    default:
        break;
    }

    return result;
}

// ============================================================================
// Input Conversion Utilities
// ============================================================================

Rml::Input::KeyIdentifier RmlSDL::ConvertKey(int sdl_key) {
    using Rml::Input;

    switch (sdl_key) {
    case SDLK_UNKNOWN: return Input::KI_UNKNOWN;
    case SDLK_SPACE: return Input::KI_SPACE;
    case SDLK_0: return Input::KI_0;
    case SDLK_1: return Input::KI_1;
    case SDLK_2: return Input::KI_2;
    case SDLK_3: return Input::KI_3;
    case SDLK_4: return Input::KI_4;
    case SDLK_5: return Input::KI_5;
    case SDLK_6: return Input::KI_6;
    case SDLK_7: return Input::KI_7;
    case SDLK_8: return Input::KI_8;
    case SDLK_9: return Input::KI_9;
    case SDLK_a: return Input::KI_A;
    case SDLK_b: return Input::KI_B;
    case SDLK_c: return Input::KI_C;
    case SDLK_d: return Input::KI_D;
    case SDLK_e: return Input::KI_E;
    case SDLK_f: return Input::KI_F;
    case SDLK_g: return Input::KI_G;
    case SDLK_h: return Input::KI_H;
    case SDLK_i: return Input::KI_I;
    case SDLK_j: return Input::KI_J;
    case SDLK_k: return Input::KI_K;
    case SDLK_l: return Input::KI_L;
    case SDLK_m: return Input::KI_M;
    case SDLK_n: return Input::KI_N;
    case SDLK_o: return Input::KI_O;
    case SDLK_p: return Input::KI_P;
    case SDLK_q: return Input::KI_Q;
    case SDLK_r: return Input::KI_R;
    case SDLK_s: return Input::KI_S;
    case SDLK_t: return Input::KI_T;
    case SDLK_u: return Input::KI_U;
    case SDLK_v: return Input::KI_V;
    case SDLK_w: return Input::KI_W;
    case SDLK_x: return Input::KI_X;
    case SDLK_y: return Input::KI_Y;
    case SDLK_z: return Input::KI_Z;
    case SDLK_BACKSPACE: return Input::KI_BACK;
    case SDLK_TAB: return Input::KI_TAB;
    case SDLK_CLEAR: return Input::KI_CLEAR;
    case SDLK_RETURN: return Input::KI_RETURN;
    case SDLK_PAUSE: return Input::KI_PAUSE;
    case SDLK_ESCAPE: return Input::KI_ESCAPE;
    case SDLK_DELETE: return Input::KI_DELETE;
    case SDLK_KP_0: return Input::KI_NUMPAD0;
    case SDLK_KP_1: return Input::KI_NUMPAD1;
    case SDLK_KP_2: return Input::KI_NUMPAD2;
    case SDLK_KP_3: return Input::KI_NUMPAD3;
    case SDLK_KP_4: return Input::KI_NUMPAD4;
    case SDLK_KP_5: return Input::KI_NUMPAD5;
    case SDLK_KP_6: return Input::KI_NUMPAD6;
    case SDLK_KP_7: return Input::KI_NUMPAD7;
    case SDLK_KP_8: return Input::KI_NUMPAD8;
    case SDLK_KP_9: return Input::KI_NUMPAD9;
    case SDLK_KP_ENTER: return Input::KI_NUMPADENTER;
    case SDLK_KP_MULTIPLY: return Input::KI_MULTIPLY;
    case SDLK_KP_PLUS: return Input::KI_ADD;
    case SDLK_KP_MINUS: return Input::KI_SUBTRACT;
    case SDLK_KP_PERIOD: return Input::KI_DECIMAL;
    case SDLK_KP_DIVIDE: return Input::KI_DIVIDE;
    case SDLK_UP: return Input::KI_UP;
    case SDLK_DOWN: return Input::KI_DOWN;
    case SDLK_RIGHT: return Input::KI_RIGHT;
    case SDLK_LEFT: return Input::KI_LEFT;
    case SDLK_INSERT: return Input::KI_INSERT;
    case SDLK_HOME: return Input::KI_HOME;
    case SDLK_END: return Input::KI_END;
    case SDLK_PAGEUP: return Input::KI_PRIOR;
    case SDLK_PAGEDOWN: return Input::KI_NEXT;
    case SDLK_F1: return Input::KI_F1;
    case SDLK_F2: return Input::KI_F2;
    case SDLK_F3: return Input::KI_F3;
    case SDLK_F4: return Input::KI_F4;
    case SDLK_F5: return Input::KI_F5;
    case SDLK_F6: return Input::KI_F6;
    case SDLK_F7: return Input::KI_F7;
    case SDLK_F8: return Input::KI_F8;
    case SDLK_F9: return Input::KI_F9;
    case SDLK_F10: return Input::KI_F10;
    case SDLK_F11: return Input::KI_F11;
    case SDLK_F12: return Input::KI_F12;
    case SDLK_F13: return Input::KI_F13;
    case SDLK_F14: return Input::KI_F14;
    case SDLK_F15: return Input::KI_F15;
    case SDLK_NUMLOCKCLEAR: return Input::KI_NUMLOCK;
    case SDLK_CAPSLOCK: return Input::KI_CAPITAL;
    case SDLK_SCROLLLOCK: return Input::KI_SCROLL;
    case SDLK_RSHIFT: return Input::KI_RSHIFT;
    case SDLK_LSHIFT: return Input::KI_LSHIFT;
    case SDLK_RCTRL: return Input::KI_RCONTROL;
    case SDLK_LCTRL: return Input::KI_LCONTROL;
    case SDLK_RALT: return Input::KI_RMENU;
    case SDLK_LALT: return Input::KI_LMENU;
    default: return Input::KI_UNKNOWN;
    }
}

int RmlSDL::GetKeyModifiers(SDL_Keymod sdl_mods) {
    int modifiers = 0;

    if (sdl_mods & KMOD_CTRL)
        modifiers |= Rml::Input::KM_CTRL;
    if (sdl_mods & KMOD_SHIFT)
        modifiers |= Rml::Input::KM_SHIFT;
    if (sdl_mods & KMOD_ALT)
        modifiers |= Rml::Input::KM_ALT;
    if (sdl_mods & KMOD_GUI)
        modifiers |= Rml::Input::KM_META;
    if (sdl_mods & KMOD_NUM)
        modifiers |= Rml::Input::KM_NUMLOCK;
    if (sdl_mods & KMOD_CAPS)
        modifiers |= Rml::Input::KM_CAPSLOCK;

    return modifiers;
}
