# 📘 CS200 OpenGL Engine to GBA Porting: Comprehensive Technical Report

**Project:** CS200 2D Engine Optimization & GBA Porting  
**Author:** Taekyung Ho  
**Date:** 2026 Winter  
**Target Architecture:** Game Boy Advance (ARM7TDMI, 16.78MHz, No FPU, No OS)

---

## 📑 Executive Summary

본 보고서는 CS200(Computer Graphics I) 과정의 Week 2~13 및 HW 1~8을 통해 구축된 **C++/OpenGL 2D 게임 엔진**의 아키텍처를 분석하고, 이를 **GBA(Game Boy Advance)** 임베디드 하드웨어 환경으로 이식(Porting)하기 위한 기술적 로드맵을 제시한다.

PC 환경(High-Level API)에서 학습한 **그래픽스 파이프라인, 수학적 변환, 메모리 관리, 성능 최적화(Batch/Instancing)** 기법들이 GBA의 **하드웨어 레지스터(Low-Level Hardware)**와 어떻게 유기적으로 연결되는지 규명하고, 최종적인 포팅 전략을 수립한다.

---

## Part 1. CS200 Engine Architecture Breakdown (HW 1 ~ 8)

우리가 구축한 엔진은 **"입력 -> 로직 -> 렌더링"**의 순환 구조를 가지며, 과제가 진행됨에 따라 점진적으로 고도화되었다.

### 🧩 Phase 1: The Foundation (기반 구축)
* **HW 1: Window & Input**
    * **내용:** OS로부터 창을 할당받고(`SDL`), 사용자 입력(`Input`)을 폴링하며, 무한 루프(`Update Loop`)를 돌리는 구조.
    * **핵심:** 게임 엔진의 심장인 `Game Loop`의 탄생.
* **HW 2: Math & Transform**
    * **내용:** 3x3 행렬, 벡터 연산 구현. SRT(Scale-Rotate-Translate) 변환.
    * **핵심:** 물체를 화면 어디에 어떻게 그릴지 결정하는 수학적 기반.

### 🎨 Phase 2: Visual Expression (시각화)
* **HW 4: Textures & UVs**
    * **내용:** 이미지 파일 로딩, 텍스처 좌표(UV) 매핑, Alpha Blending.
    * **핵심:** 단순 도형이 아닌 '그림'을 그리기 시작. (GBA의 Tile/Sprite 개념의 시초)
* **HW 5: Advanced Rendering (SDF, Text, FBO)**
    * **내용:** 수학적 도형(SDF), 동적 텍스트 렌더링(Font Atlas), 오프스크린 렌더링(Framebuffer).
    * **핵심:** 픽셀을 직접 조작하거나 텍스처를 동적으로 생성하는 고급 기법.

### 🚀 Phase 3: Optimization & Scale (최적화 및 확장)
* **HW 6: Batch & Instancing**
    * **내용:** Draw Call 오버헤드를 줄이기 위한 기법.
        * **Batch:** CPU가 버텍스를 합침 (CPU Bound).
        * **Instancing:** GPU가 같은 모델을 여러 번 그림 (GPU Bound).
    * **핵심:** **GBA 이식의 핵심.** GBA의 OAM(Object Attribute Memory)은 하드웨어 레벨의 Instancing이다.
* **HW 7: Camera & World**
    * **내용:** View Matrix 도입, 화면 분할(Viewport), 거대한 월드 탐험.
    * **핵심:** "보이는 것"과 "존재하는 것"의 분리. (GBA의 BG Scroll 기능)

### ✨ Phase 4: Polish (마무리)
* **HW 8: Depth & Post-Processing**
    * **내용:** Z-Sorting(깊이 정렬), MSAA, 감마 보정, 쉐이더 이펙트.
    * **핵심:** 렌더링 후처리(Post-FX)를 통한 시각적 품질 향상.

---

## Part 2. Organic Relationships (유기적 연결성 분석)

각 과제는 독립적이지 않으며, 다음과 같은 **세 가지 축**으로 유기적으로 연결된다.

### 🔗 1. The Rendering Pipeline Flow
> **Immediate (HW4) -> Batch (HW6) -> Instanced (HW6/GBA)**

* **초기 (Immediate):** `Draw()` 호출마다 GPU와 통신했다. 이는 비효율적이다.
* **발전 (Batch):** CPU가 데이터를 모아서 한 번에 보냈다. 하지만 매 프레임 CPU가 수학 연산을 해야 했다.
* **완성 (Instanced):** 위치/속성만 GPU에 넘기고 계산을 위임했다.
* **GBA 연결성:** GBA는 CPU(16MHz)가 매우 약하므로, **Instancing 기법(OAM)**만이 유일한 생존 전략이다. PC에서 Instancing을 구현해본 경험은 GBA의 스프라이트 관리 로직으로 직결된다.

### 🔗 2. Coordinate Systems Evolution
> **Local -> World -> View -> Clip -> Screen**

* **HW 2:** 객체 자신의 좌표(Local)를 정의했다.
* **HW 7:** 카메라(View)가 생기면서, 월드 좌표를 카메라 기준 좌표로 변환했다.
* **HW 8:** 뷰포트와 깊이(Depth)가 추가되며 최종 화면(Screen)에 그려지는 순서가 중요해졌다.
* **GBA 연결성:** GBA는 3D 파이프라인이 없지만, **Affine Matrix Registers (`REG_PA`~`PD`)**를 통해 회전/확대를 구현한다. HW 7의 View Matrix 로직은 GBA의 배경 스크롤 값(`HOFS`, `VOFS`) 계산 로직으로 변환된다.

### 🔗 3. Memory & Resource Management
> **Load -> Cache -> Upload -> Sort**

* **HW 4:** 이미지를 램에 올리고(TextureManager) 관리했다.
* **HW 6:** 렌더링 데이터를 버퍼(VBO)에 올리는 타이밍을 조절했다.
* **HW 8:** 투명 객체를 위해 그리기 순서를 정렬(Sorting)했다.
* **GBA 연결성:** GBA는 VRAM이 96KB로 매우 작다. HW 4의 텍스처 매니저는 **Tile Memory Manager**가 되며, HW 8의 Z-Sorting은 GBA의 **Priority Register (0~3)** 및 스프라이트 우선순위 관리 로직이 된다.

---

## Part 3. GBA Porting Strategy (이식 마스터 플랜)

위 분석을 바탕으로 CS200 엔진을 GBA 베어메탈로 변환하는 구체적인 전략이다.

### 🛠️ 1. Hardware Mapping Table (API 치환표)

| CS200 Concept (High-Level) | GBA Hardware (Low-Level) | Implementation Strategy |
| :--- | :--- | :--- |
| **Window / Init** (`SDL_Init`) | **Display Control** (`REG_DISPCNT`) | Mode 3 (Bitmap) or Mode 0 (Tile) 설정 |
| **Input** (`SDL_PollEvent`) | **Key Input** (`REG_KEYINPUT`) | 비트마스크 연산으로 `IsKeyDown` 구현 |
| **Texture** (`BindTexture`) | **Tile Memory** (VRAM `0x6000000`) | 이미지를 8x8 타일로 변환하여 메모리 복사 (`dmaCopy`) |
| **Sprite** (`DrawQuad`) | **OAM** (Obj Attr Memory `0x7000000`) | `InstancedRenderer`의 버퍼를 OAM 영역으로 `Flush` |
| **Camera** (`ViewMatrix`) | **BG Scroll** (`REG_BGxHOFS`) | 카메라 위치 `(x, y)`를 스크롤 레지스터에 대입 |
| **Rotation** (`ModelMatrix`) | **Affine Params** (`REG_PA`~`PD`) | 2x2 회전 행렬 값을 레지스터에 기록 |
| **Post-Process** (Shader) | **SFX Registers** (`REG_BLDCNT`) | Alpha Blending, Mosaic, Fade 효과 설정 |
| **FrameRate** (`Timer`) | **VBlank Interrupt** (`REG_IF`) | 하드웨어 수직 동기화(VSync) 인터럽트 대기 |

### 🔬 2. Performance Optimization Strategy (From Tracy Analysis)

**Tracy Profiler**를 통해 분석한 결과(CPU 부동소수점 연산 병목, 메모리 할당 오버헤드)를 GBA에서 다음과 같이 해결한다.

1.  **Math Conversion (Float → Fixed Point):**
    * **문제:** GBA는 FPU(부동소수점 유닛)가 없다. `float` 연산은 소프트웨어 에뮬레이션으로 처리되어 매우 느리다.
    * **해결:** CS200의 `Matrix`, `Vec2` 클래스를 **Q16.16 고정 소수점(Fixed Point)** 기반으로 재작성한다. (`float` 덧셈 100 cycle -> `int` 덧셈 1 cycle)

2.  **Memory Management (New/Delete → Static/Pooling):**
    * **문제:** GBA는 OS가 없어 힙 메모리 관리(`malloc`) 오버헤드가 크고 단편화 위험이 있다.
    * **해결:** HW 6의 `std::vector` 재할당 구조를 **Object Pool (정적 배열)** 구조로 변경한다. `ParticleSystem` 등은 미리 최대 개수만큼 배열을 잡아두고 재사용한다.

3.  **Rendering Path (Batching → OAM Direct Access):**
    * **문제:** 매 프레임 버퍼를 업로드하는 비용.
    * **해결:** **Shadow OAM** 기법 사용. RAM에 있는 미러링 버퍼에 데이터를 쓰고, VBlank 기간에 **DMA(Direct Memory Access)**로 VRAM에 초고속 전송한다.

### 📅 3. Step-by-Step Porting Roadmap

1.  **Step 1 (Core):** `main()` 진입점 생성, `REG_DISPCNT` 설정, 무한 루프 구현. (화면을 빨갛게 만들기)
2.  **Step 2 (Input):** `Input` 클래스 포팅. 버튼을 누르면 화면 색 바꾸기.
3.  **Step 3 (Math):** `FixedPoint` 클래스 구현 및 `Math` 라이브러리 교체.
4.  **Step 4 (Render):** `OAM Manager` 구현. 스프라이트 하나 화면에 띄우기.
5.  **Step 5 (Logic):** `GameStateManager` 이식 및 `Update()` 루프 연결.
6.  **Step 6 (Polish):** 배경 스크롤(Camera), 사운드, 특수 효과(Mosaic/Blend) 적용.

---

## 🎯 Conclusion

CS200의 여정은 단순히 OpenGL 사용법을 익히는 것이 아니라, **"실시간 렌더링 엔진이 데이터를 어떻게 처리하는가"**에 대한 본질을 이해하는 과정이었다.

* **HW 1~5**는 "무엇을 그릴 것인가(What)"를 정의했다.
* **HW 6**은 "어떻게 효율적으로 그릴 것인가(How)"를 가르쳐주었다. (Instancing)
* **HW 7~8**은 "어떻게 아름답게 보여줄 것인가(Polish)"를 다루었다.

이제 우리는 이 모든 지식을 **GBA**라는 제약된 환경 속에서 **하드웨어 가속(Hardware Acceleration)**이라는 날카로운 무기로 다듬어낼 것이다. Tracy로 분석된 병목 지점들은 GBA 포팅 시 최우선 최적화 타겟이 되며, OpenGL의 Instancing 로직은 GBA의 OAM 제어 로직으로 완벽하게 변환될 것이다.

**준비는 끝났다. 이제 베어메탈의 세계로 진입한다.**