# Tile Matching Game

Visual Studio 2022에서 바로 열고 실행할 수 있는 C++ 콘솔 타일 매칭 게임 프로젝트입니다.

## 구성
- `TileMatchingGame.sln`: Visual Studio 2022 솔루션 파일
- `TileMatchingGame/TileMatchingGame.vcxproj`: C++ 프로젝트 파일 (`v143`, `x64`, `Debug/Release`)
- `TileMatchingGame/main.cpp`: 게임 로직

## 실행 방법 (Visual Studio 2022)
1. Visual Studio 2022에서 `TileMatchingGame.sln` 파일을 엽니다.
2. 솔루션 구성은 `Debug | x64`를 선택합니다.
3. `F5` (디버깅 시작) 또는 `Ctrl + F5` (디버깅 없이 시작)로 실행합니다.

## 게임 방법
- 보드의 같은 문자를 가진 타일 2개를 선택하면 제거됩니다.
- 입력 형식: `행 열` (예: `1 2`)
- 모든 타일을 제거하면 게임이 끝납니다.
