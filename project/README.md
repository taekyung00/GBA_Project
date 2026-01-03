# GBA Development Environment Setup

이 프로젝트는 **DevkitPro** 툴체인을 사용하여 GBA 롬(.gba)을 빌드합니다.
Makefile의 이식성(Portability)을 위해, 컴파일러 경로는 하드코딩되지 않고 시스템 환경 변수 `DEVKITPRO`를 참조하도록 작성되었습니다.

따라서 빌드하기 전에 아래 과정을 통해 환경 변수가 올바르게 설정되어 있는지 확인해야 합니다.

## 1. 환경 변수 확인 (Verify Environment Variable)

터미널(PowerShell 또는 CMD)을 열고 아래 명령어를 입력하여 경로가 출력되는지 확인합니다.

```powershell
echo $env:DEVKITPRO
# 성공 시 예시 출력: C:\devkitPro
# 실패 시: 아무것도 출력되지 않음
```

### Command Prompt (CMD)

```DOS
echo %DEVKITPRO%
# 성공 시 예시 출력: C:\devkitPro
# 실패 시: %DEVKITPRO% 라고 그대로 출력됨
```

경로가 올바르게 출력된다면 설정이 완료된 것입니다. 아무것도 출력되지 않는다면 **2. 환경 변수 설정**을 진행해 주세요.

---

## 2. 환경 변수 설정 (Set Environment Variable)

Windows 환경에서 `DEVKITPRO` 변수를 영구적으로 등록하는 방법입니다.

1. **[시스템 환경 변수 편집]** 실행
   
   - `Win` 키를 누르고 "시스템 환경 변수 편집" (Edit the system environment variables) 검색 및 실행.

2. **[환경 변수(N)...]** 버튼 클릭
   
   - 창 하단의 '환경 변수' 버튼을 클릭합니다.

3. **시스템 변수(S)에 새로 만들기**
   
   - '사용자 변수'가 아닌, 아래쪽의 **'시스템 변수'** 섹션에서 **[새로 만들기(W)...]** 를 클릭합니다.

4. **변수 값 입력**
   
   - **변수 이름(Variable name):** `DEVKITPRO`
   
   - **변수 값(Variable value):** `C:\devkitPro`
     
     - *(주의: 본인의 DevkitPro 설치 경로가 다르다면 해당 경로를 입력하세요. 예: D:\devkitPro)*

5. **확인 및 재부팅**
   
   - 모든 창에서 [확인]을 눌러 저장합니다.
   
   - **중요:** 이미 열려있던 터미널(VS Code 등)에는 반영되지 않습니다. **터미널이나 VS Code를 완전히 껐다가 다시 켜주세요.**

---

## 3. 빌드 테스트 (Build Test)

설정이 완료되었다면, 프로젝트 폴더에서 아래 명령어로 빌드를 테스트할 수 있습니다.

```powershell
# main.c 파일을 빌드하여 main.gba 생성
make main

# 빌드된 파일 삭제 (Clean)
make clean
```