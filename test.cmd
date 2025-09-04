@echo off
REM ————————————————
REM  LaunchGame.bat
REM  .uproject을 Standalone Game 모드로 실행하는 배치 파일
REM ————————————————

REM 1) 언리얼 엔진 편집기 경로 설정
set "UE_EDITOR=E:\Epic Games\UE_5.2\Engine\Binaries\Win64\UnrealEditor.exe"

REM 2) 실행할 .uproject 파일 (현재 배치파일이 있는 폴더 기준 상대경로 사용)
set "PROJECT_UPROJ=%~dp0Project_S.uproject"

REM 3) (선택) 게임 모드로 실행하기 위한 옵션
set "GAME_FLAGS=-game -log"

REM 4) 실제 실행
"%UE_EDITOR%" "%PROJECT_UPROJ%" %GAME_FLAGS%

REM 5) 창이 바로 닫히지 않도록 잠시 대기
pause