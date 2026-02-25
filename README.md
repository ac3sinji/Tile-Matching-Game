# Tile Matching Game

Visual Studio 2022에서 바로 열고 실행할 수 있는 C++ Win32 프로젝트입니다.

## 구성
- `TileMatchingGame.sln`: Visual Studio 2022 솔루션 파일
- `TileMatchingGame/TileMatchingGame.vcxproj`: C++ 프로젝트 파일 (`v143`, `x64`, `Debug/Release`)
- `TileMatchingGame/main.cpp`: Win32 진입점 + 기본 보드 렌더링/입력 처리
- `TileMatchingGame/TileDomain.h/.cpp`: 타일 도메인 타입과 공용 헬퍼
- `TileMatchingGame/BoardState.h/.cpp`: 3x3 보드 상태 관리 클래스
- `TileMatchingGame/TileSpawner.h/.cpp`: 타일 스폰 클래스
- `TileMatchingGame/GameCore.h/.cpp`: 게임 턴/배치/승리 판정을 담당하는 코어 클래스

## 현재 구현 범위
- 객체 지향 구조로 핵심 로직을 클래스 파일로 분리
- Win32 화면에 플레이어 보드/상태 텍스트 렌더링
- 타이머 기반 턴 진행 및 마우스 클릭 타일 배치 처리
- 3x3 보드 상태 관리 (`BoardState`)
- 레벨 시작 시 플레이어/AI의 서로 다른 목표 타일 할당 (`GameCore::AssignTargets`)
- 타일 스폰 로직 (`TileSpawner`, `GameCore::SpawnTurnTile`)
  - 플레이어 목표 타일/조커: 플레이어 입력 대기
  - AI 목표 타일: AI 보드의 빈 칸에 자동 배치
- 타일 배치 유효성 검사 (`BoardState::PlaceTile`)
  - 좌표 범위 검사
  - 빈 칸 검사
  - 목표 타일 또는 조커 여부 검사
- 잘못된 타일 배치 시 피드백 플래그(`showInvalidMoveFeedback_`) 제공
- 매 배치 후 승리 조건 확인 (`GameCore::EvaluateWinner`)

## 실행 방법 (Visual Studio 2022)
1. Visual Studio 2022에서 `TileMatchingGame.sln` 파일을 엽니다.
2. 솔루션 구성은 `Debug | x64`를 선택합니다.
3. `F5` (디버깅 시작) 또는 `Ctrl + F5` (디버깅 없이 시작)로 실행합니다.

## 다음 단계 제안
- 레벨 선택 화면, 프로필 이미지, 목표 타일 표시 UI 개선
- 승리 시 폭죽/팝업 연출 추가
