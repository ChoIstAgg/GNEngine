#pragma once
#include "../GNEngine_API.h"

#include <AL/al.h>

struct Position {
    float x, y, z;
};

enum class SoundPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};


/*
 * 濡쒕뱶???ъ슫???곗씠???먯껜瑜??섑??대뒗 ?먯썝 ?대옒?ㅼ엫.
 * OpenAL 踰꾪띁 ID? ?ㅽ뀒?덉삤 ?щ? ?깆쓽 ?뺣낫瑜??닿퀬 ?덉쓬.
 * SoundManager???섑빐 愿由щ릺硫? SoundComponent?먯꽌 ??媛앹껜瑜?李몄“?섏뿬 ?ъ슜??
 */
class GNEngine_API Sound {
public:
    /*
     * Sound 媛앹껜瑜??앹꽦?섍퀬 硫ㅻ쾭 蹂?섎? 珥덇린?뷀븿.
     */
    Sound(ALuint monoBuffer, ALuint stereoBufferRight, bool isStereo);

    /*
     * ?뚮㈇?먯뿉???좊떦??OpenAL 踰꾪띁瑜??뺣━??
     */
    ~Sound();

    // 蹂듭궗 諛??대룞??湲덉??섏뿬 SoundManager瑜??듯븳 怨듭쑀留??덉슜??
    Sound(const Sound&) = delete;
    Sound& operator=(const Sound&) = delete;
    Sound(Sound&&) = delete;
    Sound& operator=(Sound&&) = delete;

    ALuint getMonoBuffer() const { return monoBuffer_; }
    ALuint getStereoBufferRight() const { return stereoBufferRight_; }
    bool isStereo() const { return isStereo_; }

private:
    ALuint monoBuffer_;
    ALuint stereoBufferRight_;
    bool isStereo_ = false;
};



