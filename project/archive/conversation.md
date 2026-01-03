### 1. GBA 개발 기초 & 트러블슈팅

- **C23 표준 이슈:** 최신 컴파일러(`-std=c23`)에서는 `bool`이 예약어입니다. `typedef int bool;` 대신 `#include <stdbool.h>`를 사용해야 합니다.

- **빌드 파이프라인 (Build Pipeline):**
  
  1. **Compile & Link (`gcc`):** 소스 코드 $\rightarrow$ `.elf` (메타데이터 포함 실행 파일)
  
  2. **Object Copy (`objcopy`):** `.elf` $\rightarrow$ `.gba` (순수 바이너리 추출)
  
  3. **Header Fix (`gbafix`):** 닌텐도 로고 및 체크섬 삽입 (실기 구동 필수)

- **아키텍처 패턴:** `main.c` 하나에 몰아넣기보다 **HAL(하드웨어 추상화) / Engine / Game Logic**으로 계층을 나누는 것이 좋습니다.

### 2. 콘솔 아키텍처별 개발 난이도 비교 (Hierarchy)

태경 님이 앞으로 밟아나갈(혹은 피해야 할) 테크 트리입니다.

| **비교군**                       | **승자 (추천)**  | **이유 (CS 관점)**                                                                                                        |
| ----------------------------- | ------------ | --------------------------------------------------------------------------------------------------------------------- |
| **GBA vs GB/SFC**             | **GBA**      | **Sweet Spot.** GB/SFC는 어셈블리 필수, 뱅크 스위칭, 비트플레인 그래픽 등 제약이 너무 심함. GBA는 C언어와 선형 메모리를 지원하는 "가장 이상적인 임베디드 학습 도구".          |
| **GBA vs NDS**                | **GBA**      | **Simplicity.** NDS는 화면 하나만 써도 ARM9 + ARM7 듀얼 코어 동기화, 복잡한 메모리 뱅크 매핑, 전원 관리가 필요함. GBA가 구조 이해에 훨씬 유리.                   |
| **GBA vs Arduboy**            | **GBA**      | **System vs MCU.** Arduboy(8-bit AVR)는 2.5KB RAM의 극한 최적화(Bit packing) 훈련용. GBA(32-bit ARM)는 본격적인 시스템/엔진 아키텍처 학습용.     |
| **3D 입문 (GC vs PS1 vs Xbox)** | **GameCube** | **Modern & Bare-metal.** Xbox는 너무 PC 같고, PS1은 고정 소수점/Z-버퍼 부재로 수학적 난이도가 최상(Hell). GC는 현대적 파이프라인(TEV)과 하드웨어 제어의 맛이 공존함. |

### 3. 실기(Real Hardware) vs 에뮬레이션

"코드가 돌아간다고 끝이 아니다"라는 것을 배웠습니다.

- **네이티브 실행 (Native Execution):**
  
  - **3DS (`open_agb_firm`):** 에뮬레이터가 아님. 3DS 내부의 GBA 호환 코어(ARM7)를 직접 구동. 신뢰도 최상.
  
  - **NDS (`nds-bootstrap`):** 마찬가지로 NDS 하드웨어 칩셋을 직접 사용하며 I/O만 후킹.

- **실기 이슈:**
  
  - **세이브 에러:** 하드웨어는 세이브 파일 크기나 타입(SRAM/Flash)에 민감함. 헤더 패치(`gbafix -p`)와 더미 `.sav` 파일이 필요할 수 있음.
  
  - **색감/잔상:** PC 모니터와 실기 액정의 감마값, 반응 속도 차이로 인한 모션 블러 존재.

- **개발 워크플로우 (Best Practice):**
  
  - **개발 중:** 3DS의 `ftpd`를 이용한 **FTP 무선 전송** (SD카드 보호).
  
  - **최종 검증:** GBA 실기 + 이지플래시 + SD카드 이동 (혹은 Multiboot 케이블).

### 4. 물리 엔진 & CS 심화

- **물리 엔진의 역사:**
  
  - **GBA/PS1:** FPU(부동소수점 유닛) 부재로 Box2D 같은 리얼 물리는 불가능. 고정 소수점 기반의 단순 로직 사용.
  
  - **Gen 7 (PS3/X360):** 멀티코어와 강력한 FPU 등장 이후 물리 시뮬레이션이 표준이 됨.

- **학습 전략:** "물리 수학" 공부는 PC에서 하고, "최적화" 공부는 콘솔 포팅을 통해 할 것.

- **리버스 엔지니어링:**
  
  - 소스 코드가 없는 바이너리를 `Ghidra`(정적 분석)와 `mGBA Debugger`(동적 분석)로 뜯어보는 것은 최고의 공부.
  
  - 핵심은 **메모리 헌팅(값 변화 추적) $\rightarrow$ 워치포인트 설정 $\rightarrow$ 어셈블리 로직 파악**.

### 5. 커리어 & 포트폴리오 (면접 팁)

이 경험을 닌텐도나 게임 회사 면접에서 어필하는 방법입니다.

- **핵심 키워드:** 절대 "닥터", "롬 해킹" 같은 단어 쓰지 말 것.
  
  - $\rightarrow$ **"Target Device Verification (타겟 장비 검증)"**
  
  - $\rightarrow$ **"Native Execution Environment (네이티브 실행 환경)"**
  
  - $\rightarrow$ **"Cross-validation using real hardware (실기를 통한 교차 검증)"**
  
  - $\rightarrow$ **"Low-level Optimization (저수준 최적화)"**

- **가치:** 상용 엔진 없이 베어메탈 환경에서 렌더링 파이프라인과 메모리를 직접 제어해 본 경험은 신입 엔지니어로서 **Top-tier** 역량임.
