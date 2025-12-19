#pragma once

enum class RenderLayer {
    BACKGROUND_FAR, // ex. Sky
    BACKGROUND_MID, 
    BACKGROUND_NEAR, 
    GAME_OBJECT, 
    CRUCIAL_GAME_OBJECT, // ex. Player
    FOREGROUND, // gress
    VERY_CLOSE_FOREGROUND, // 
    GAME_EFFECT,
    CRUCIAL_GAME_EFFECT,
    UI,
    UI_EFFECT, // ex. 
    SCENE_EFFECT, // Fade effect
    CRUCIAL_UI,  // Mouse Pointer
    COUNT // A number of Layer
};

