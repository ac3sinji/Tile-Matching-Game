# Tile Matching Game (ImGui UI Skeleton)

C++20, CMake, SDL2, Dear ImGui 기반의 데스크톱 Immediate Mode GUI 프레임 프로젝트입니다.

## 구성
- `CMakeLists.txt`: CMake 빌드 설정
- `src/main.cpp`: `AppUI` 생성 및 `run()` 호출
- `src/ui/App.hpp`, `src/ui/App.cpp`: SDL + ImGui 초기화/루프 및 패널 UI

## 동작 개요
- SDL 비디오 초기화
- `1500x950` 창 생성
- `SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC` 렌더러 생성
- Dear ImGui context 생성 및 Dark 테마 적용
- `imgui_impl_sdl2`, `imgui_impl_sdlrenderer2` 백엔드 초기화
- 메인 루프에서 SDL 이벤트 처리 후 ImGui 프레임 렌더링
- 패널 3개 표시
  - `Controls`
  - `Generation Logs`
  - `Viewer`
- 한국어 폰트 로드 시도 후 실패 시 기본 폰트로 fallback

## 빌드/실행
```bash
cmake -S . -B build
cmake --build build -j
./build/tile_matching_ui
```

Windows의 경우 실행 파일 경로는 생성된 빌드 구성(예: `build/Debug/`)에 따라 달라질 수 있습니다.
