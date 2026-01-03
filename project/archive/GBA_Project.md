### 🗺️ CS200 복습 겸 GBA 포팅 통합 로드맵

### 1단계: 커널과 수학적 기초 (Weeks 2-3 복습)

**[핵심 개념: Window, Input, Linear Algebra]**

- **CS200 복습:** `SDL_Init`을 통한 OS 컨텍스트 생성과 $3 \times 3$ 행렬을 이용한 SRT 변환 학습.

- **GBA 구현:**
  
  1. **Context:** `REG_DISPCNT`(`0x4000000`)에 직접 값을 써서 화면 모드(Mode 3)를 켭니다. 이것이 `SDL_CreateWindow`의 로우레벨 버전입니다.
  
  2. **Input:** `REG_KEYINPUT`(`0x4000130`)의 비트를 검사하여 `Input` 클래스를 포팅합니다.
  
  3. **Fixed-Point Math:** GBA는 FPU가 없으므로 모든 `float` 행렬 연산을 **고정 소수점(Fixed-point)** 기반 정수 연산으로 교체합니다.

- **🎯 목표:** 화면 중앙에 `fixed` 좌표를 가진 사각형을 띄우고 방향키로 이동시키기.

---

### 2단계: 래스터라이저와 SDF의 재해석 (Weeks 4-5 복습)

**[핵심 개념: Immediate Mode, SDF, Font]**

- **CS200 복습:** `glDrawArrays`의 원리와 SDF(Signed Distance Field)를 이용한 수학적 도형 그리기 학습.

- **GBA 구현:**
  
  1. **Software Rasterizer:** GPU 쉐이더가 없으므로 **Bresenham 알고리즘**(선)과 **Midpoint Circle 알고리즘**(원)을 직접 CPU로 구현합니다.
  
  2. **SDF 대체:** 실시간 `length(pos)` 계산은 GBA CPU에 너무 무겁습니다. PC에서 배운 SDF의 원리를 이해하되, 결과물은 타일 데이터로 미리 구워내는 전략을 세웁니다.

- **🎯 목표:** `DrawLine`, `DrawCircle` 함수를 직접 만들어 화면에 복잡한 도형 그리기.

---

### 3단계: 최적화와 아키텍처 (Week 6 복습)

**[핵심 개념: Batch Rendering, Instancing]**

- **CS200 복습:** 드로우 콜을 줄이기 위한 `BatchRenderer2D`와 `InstancedRenderer2D`의 구조적 차이 학습.

- **GBA 구현:**
  
  1. **Mode 0 전환:** 비트맵 모드를 버리고 타일/스프라이트 기반의 Mode 0로 전환합니다.
  
  2. **OAM Manager:** GBA의 하드웨어 스프라이트(OAM)는 사실상 **하드웨어 가속 인스턴싱**입니다.
  
  3. **Shadow OAM & DMA:** PC의 `Flush()` 개념을 도입하여, VBlank 기간에 RAM의 스프라이트 데이터를 **DMA**로 VRAM에 한 번에 쏘는 구조를 만듭니다.

- **🎯 목표:** 100개 이상의 스프라이트를 프레임 드랍 없이 화면에 뿌리기.

---

### 4단계: 공간 변환과 뷰포트 (Weeks 7-8 복습)

**[핵심 개념: Camera, Viewport, Depth, Post-processing]**

- **CS200 복습:** `ViewMatrix`를 통한 카메라 이동과 프레임버퍼를 이용한 후처리 효과 학습.

- **GBA 구현:**
  
  1. **Hardware Scroll:** 카메라 행렬 곱셈 대신 `REG_BGxHOFS` 레지스터를 조작하여 비용 없는 배경 스크롤을 구현합니다.
  
  2. **Priority System:** OpenGL의 `glDepthFunc` 대신 GBA의 **Priority(0~3)** 레지스터를 사용하여 레이어 앞뒤 관계를 설정합니다.
  
  3. **Hardware SFX:** 쉐이더 대신 **Mosaic** 및 **Blending** 레지스터를 사용하여 후처리 효과를 재현합니다.

- **🎯 목표:** 광활한 맵을 카메라로 탐험하고 모자이크 효과 연출하기.

---

### 5단계: 시스템 통합과 폴리싱 (Weeks 9-13 복습)

**[핵심 개념: Game State, Advanced Systems]**

- **CS200 복습:** `GameStateManager`를 통한 씬 전환과 복잡한 게임 로직 구조 학습.

- **GBA 구현:**
  
  1. **State Machine:** PC 엔진의 구조를 본떠 GBA에서도 씬 전환 시스템을 구축합니다.
  
  2. **Profiling:** Tracy로 확인했던 병목 지점(예: 과도한 메모리 할당)이 GBA에서 발생하지 않도록 **정적 객체 풀(Static Pool)**을 최종 점검합니다.

- **🎯 목표:** 완벽한 게임 루프와 씬 전환이 포함된 GBA 게임 데모 완성.

### 🗺️ GBA 베어메탈 엔진 포팅: 상세 계획표 (The Master Plan)

이 계획표는 **[CS200의 개념]** 을 **[GBA의 하드웨어 특성]** 으로 번역하여 구현하는 구체적인 지침서입니다.

---

### 1단계: 엔진의 심장 (The Heart)

**[관련: Week 2, 3 / HW 1 (Input, Window)]**

가장 먼저 할 일은 OS 없이 기계를 깨우고, 나의 명령(입력)을 듣게 만드는 것입니다.

- **CS200 논리:** `SDL_Init`으로 OS에게 "창 좀 줘"라고 요청하고, `While` 루프에서 이벤트를 기다림.

- **GBA 구현 (Mode 3):**
  
  1. **Context 초기화:** `main()` 함수 시작과 동시에 `REG_DISPCNT` 주소(`0x4000000`)에 `0x403` (Mode 3 + BG2 On) 값을 씀. 이것이 `SDL_CreateWindow`입니다.
  
  2. **Game Loop:** `while(1)` 무한 루프 생성. OS가 없으므로 프로그램은 절대 종료되지 않아야 합니다.
  
  3. **Input Polling:** HW 1의 `Input` 클래스를 포팅합니다. `SDL_GetKeyboardState` 대신 `REG_KEYINPUT`(`0x4000130`)의 비트 값을 읽어 `IsKeyDown`을 구현합니다.

- **🎯 목표:** GBA 에뮬레이터 화면을 빨간색으로 채우고, 버튼을 누르면 파란색으로 변하게 만드세요.

### 2단계: 데이터의 구조화 (The Structure)

**[관련: Week 3, 10 / HW 2, 3 (Vertex, Model)]**

GPU에게 데이터를 설명하는 `VertexLayout`을 제거하고, CPU가 이해하기 쉬운 `struct`로 데이터를 정의합니다.

- **CS200 논리:** `VertexLayout` 클래스로 `float` 데이터가 메모리에 어떻게 깔려있는지 OpenGL에게 설명함.

- **GBA 구현 (Fixed Point):**
  
  1. **자료형 재정의:** GBA(ARM7)는 실수(float) 연산이 매우 느립니다. OpenGL의 `float` 좌표를 **고정 소수점(Fixed Point)** 정수로 변환해야 합니다.
     
     - `typedef int fixed;` (예: 하위 8비트는 소수부, 상위 24비트는 정수부)
  
  2. **Vertex 구조체:** HW 2의 유연한 레이아웃 대신, 엄격한 구조체를 선언합니다.
     
     ```c
     typedef struct { fixed x, y; u16 color; } Vertex;
     ```

- **🎯 목표:** 화면 중앙 좌표를 `fixed` 변수로 정의하고, 점 하나를 찍으세요.

### 3단계: 소프트웨어 래스터라이저 (The Rasterizer)

**[관련: Week 4, 5 / HW 4, 5 (Immediate, SDF)]**

GPU(쉐이더)가 없으므로, 수학 공식을 이용해 점을 찍어 도형을 그리는 함수를 직접 짭니다. 태경 님이 가장 재미있어할 **"밑바닥 구현"** 단계입니다.

- **CS200 논리:** `SDF` 쉐이더나 `glDrawArrays`를 쓰면 GPU가 알아서 픽셀을 채워줌.

- **GBA 구현 (Software Algo):**
  
  1. **DrawPixel:** VRAM 배열 `u16* videoBuffer`의 특정 인덱스에 색상 값을 넣는 함수 구현. (Bounds Check 필수)
  
  2. **Line Drawing:** HW 4의 `DrawLine`을 **브레즌햄(Bresenham) 알고리즘**으로 구현. (실수 연산 없이 정수 덧셈만으로 선을 긋는 알고리즘)
  
  3. **Circle Drawing:** HW 5의 `SDF` 원리(`length(pos)`)를 사용하는 대신, **Midpoint Circle Algorithm**을 사용하여 CPU 부하 없이 원을 그립니다.

- **🎯 목표:** `DrawLine`, `DrawRect`, `DrawCircle` 함수를 만들어 화면에 그림판처럼 도형을 그리세요.

### 4단계: 하드웨어 가속의 시작 (The Batching)

**[관련: Week 7, 9 / HW 6 (Batch, Instancing)]**

여기서 **Mode 3(비트맵)** 를 버리고 **Mode 0(타일/스프라이트)** 로 넘어갑니다. 이것이 GBA 개발의 **"Batch Rendering"** 입니다.

- **CS200 논리:** `BatchRenderer`에 쿼드를 쌓았다가(AddQuad) 한 번에 전송(Flush)하여 드로우콜을 줄임.

- **GBA 구현 (OAM Manager):**
  
  1. **Sprite = Instance:** GBA의 하드웨어 스프라이트 하나하나가 HW 9의 `Instance`입니다. 위치(x, y)와 타일 인덱스만 다르고 같은 그림을 공유합니다.
  
  2. **OAM Buffer (Shadow OAM):** VRAM(OAM)에 직접 쓰지 않고, RAM에 `StartBatch`처럼 버퍼를 만듭니다. 게임 로직은 이 버퍼만 건드립니다.
  
  3. **DMA 전송 (Flush):** 화면이 다 그려진 후 쉬는 시간(**VBlank**)에 DMA(Direct Memory Access)를 이용해 RAM 버퍼를 OAM으로 한 방에 복사합니다. 이것이 완벽한 `Flush()` 구현입니다.

- **🎯 목표:** 캐릭터 스프라이트 100개를 화면에 띄우고 프레임 드랍 없이 움직이세요.

### 5단계: 카메라와 월드 (The Camera)

**[관련: Week 12 / HW 7 (Camera)]**

행렬 곱셈(Matrix Multiplication)을 제거하고, 하드웨어 스크롤 레지스터를 제어합니다.

- **CS200 논리:** `ViewMatrix`를 만들어서 쉐이더에 보내고, 모든 정점에 곱함.

- **GBA 구현 (Hardware Scroll):**
  
  1. **Global Scroll:** 배경(Background)을 움직이는 건 `REG_BG0HOFS`, `REG_BG0VOFS` 레지스터에 값만 넣으면 끝입니다. (비용 0)
  
  2. **Camera Class:** HW 7의 `Camera` 클래스를 가져오되, `GetViewMatrix()` 대신 `ApplyToRegister()` 함수를 만듭니다.
  
  3. **Culling (최적화):** 카메라 화면 밖의 스프라이트는 OAM 버퍼에 넣지 않도록(Hide) 예외 처리합니다.

- **🎯 목표:** 광활한 타일맵 배경 위를 카메라가 돌아다니고, 캐릭터가 그 위를 걷게 만드세요.

### 6단계: 특수 효과와 마무으리 (The Effects)

**[관련: Week 13 / HW 8 (Post-process, Depth)]**

엔진의 시각적 완성도를 높이는 단계입니다. 쉐이더 대신 **SFX 레지스터**를 씁니다.

- **CS200 논리:** FBO를 만들고 Fragment Shader에서 픽셀 색을 조작(블러, 흑백 등). 깊이 테스트(Depth Test)로 앞뒤 구분.

- **GBA 구현 (SFX Registers):**
  
  1. **Priority (Depth 대체):** 배경과 스프라이트의 `Priority` 속성(0~3)을 조절하여 Z-Sorting을 구현합니다.
  
  2. **Mosaic (Pixelize):** HW 8의 픽셀라이즈 효과는 `REG_MOSAIC` 레지스터 설정 한 줄이면 하드웨어가 처리해 줍니다.
  
  3. **Blending (Alpha):** 반투명 효과나 페이드 인/아웃은 `REG_BLDCNT`와 `Alpha Coefficients`를 조작하여 구현합니다.

- **🎯 목표:** 캐릭터가 풀숲(우선순위 높음) 뒤로 숨고, 화면이 서서히 어두워지는(Fade out) 연출을 넣으세요.

---

### 📅 추천 일정 (Pre-semester Roadmap)

태경 님의 다음 학기(3D 그래픽스) 시작 전까지 이 과정을 끝내면 완벽합니다.

1. **Week 1:** [1단계 & 2단계] 개발 환경 구축(DevkitPro), 점 찍기, 구조체 정의.

2. **Week 2:** [3단계] 선/원 그리기 알고리즘 구현 (소프트웨어 렌더러 완성).

3. **Week 3:** [4단계] 타일 모드(Mode 0) 전환, OAM 스프라이트 띄우기 (배칭 구현).

4. **Week 4:** [5단계 & 6단계] 맵 스크롤, 카메라 구현, 특수 효과 적용.

**[저장된 로드맵 요약]**

- **Step 1:** 엔진 진입점 & 입력 (Mode 3, Input Polling)

- **Step 2:** 데이터 구조화 (Fixed Point, Struct)

- **Step 3:** 소프트웨어 래스터라이저 (Bresenham, Midpoint)

- **Step 4:** 하드웨어 배칭 (Mode 0, OAM, DMA)

- **Step 5:** 하드웨어 카메라 (Scroll Reg, Affine)

- **Step 6:** 하드웨어 특수효과 (Mosaic, Blend, Priority)
