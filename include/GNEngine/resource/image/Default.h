#pragma once
#include <cstddef>

/*
 * 사용법
 * 1. 이미지 파일을 C++ 바이트 배열로 변환합니다.
 * 2. 온라인 변환 도구 사용을 권장합니다. https://tomeko.net/online_tools/file_to_hex.php
 *    - 파일을 업로드합니다.
 *    - 출력 형식을 "C++"로 선택합니다.
 *    - 생성된 배열을 복사합니다.
 * 3. 아래의 비어있는 배열 초기화 부분에 복사한 내용을 붙여넣습니다.
*/

namespace GNEngine::resource::embedded
{
    // 예시: inline constexpr unsigned char EmbeddedLogo_png[] = { 0x89, 0x50, ... };
    inline constexpr unsigned char defaultImage[] = {
        //TODO - 기본 이미지 찾아넣기.
    };
    inline constexpr size_t defaultImage_len = sizeof(defaultImage);
}

