// draw_single_color.c

// GBA의 기본 데이터 타입 정의 (편의상)
typedef unsigned short u16; // 16비트 (픽셀 컬러, 레지스터 크기)
typedef unsigned int   u32; // 32비트 (메모리 주소 연산용)

// [메모리 주소 정의]
// CS 핵심: Memory Mapped I/O (MMIO)
// 하드웨어 제어는 특정 메모리 주소에 값을 쓰는 것으로 이루어집니다.
#define REG_BASE 0x04000000

// volatile 키워드: "컴파일러야, 최적화하지 마!"
// 이 변수는 하드웨어에 의해 언제든 값이 바뀔 수 있으므로, 
// 캐시된 값을 쓰지 말고 매번 실제 메모리 주소를 읽고 써야 함을 명시.
#define REG_DISPCNT (*(volatile u32*)(REG_BASE + 0x0000))

// 비디오 메모리(VRAM) 시작 주소
// 포인터가 u16*인 이유: GBA의 픽셀 하나가 16비트(2바이트)이기 때문.
#define VRAM        ((volatile u16*)0x06000000)

// [상수 정의]
// 모드 3: 240x160 해상도, 각 픽셀을 직접 제어하는 비트맵 모드.
// (더블 버퍼링 불가능, 싱글 버퍼만 사용)
#define MODE_3      0x0003

// 배경 2(BG2) 레이어 활성화 (모드 3는 구조상 BG2만 사용 가능)
#define BG2_ENABLE  0x0400

// [컬러 매크로]
// GBA는 15비트 컬러(BGR 555)를 사용합니다. (최상위 1비트는 사용 안 함)
// 포맷: xBBBBBGGGGGRRRRR
// 0x001F (00000 00000 11111) -> Red 채널만 꽉 채움
#define COLOR_RED   0x001F
#define COLOR_BLUE  0x7C00
#define COLOR_GREEN 0x03E0

// 화면 너비와 높이
#define SCREEN_W    240
#define SCREEN_H    160

int main() {
    // 1. 하드웨어 설정: 비디오 모드 3 + BG2 켜기
    // 비트 마스킹(Bitmasking)을 통해 필요한 비트만 1로 설정합니다.
    // REG_DISPCNT는 상태 레지스터이므로 한 번 설정하면 전원을 끌 때까지 유지됩니다.
    REG_DISPCNT = MODE_3 | BG2_ENABLE;

    // 2. 화면 그리기 (렌더링)
    // VRAM은 1차원 배열처럼 접근 가능합니다.
    // 화면 전체(240 * 160 픽셀)를 순회하며 값을 채워 넣습니다.
    // CPU가 VRAM에 값을 쓰는 순간, 즉시 LCD 컨트롤러가 화면에 색을 뿌립니다.
    for (int i = 0; i < SCREEN_W * SCREEN_H; ++i) {
        VRAM[i] = COLOR_BLUE;  // 모든 픽셀을 파란색으로 설정
    }

    // 3. 무한 루프 (프로그램 종료 방지)
    // 임베디드 프로그램은 OS로 복귀하는 개념이 없으므로,
    // main 함수가 끝나면 시스템이 멈춥니다. 따라서 무한 루프로 잡아둡니다.
    while (1) {
        // 나중에는 여기서 키 입력과 게임 로직을 처리하게 됩니다.
    }

    return 0;
}