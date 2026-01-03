// input_test.c
// GBA Bare-metal Programming: Input, Movement, and Rendering
// ---------------------------------------------------------

#include <stdbool.h> // bool 타입을 사용하기 위한 표준 헤더

// ---------------------------------------------------------
// 1. 자료형 및 레지스터 정의 (Memory Mapped I/O)
// ---------------------------------------------------------
typedef unsigned short u16;
typedef unsigned int   u32;

#define REG_BASE        0x04000000
#define VRAM            ((volatile u16*)0x06000000)

// [레지스터 매핑]
// volatile 키워드 필수: 하드웨어에 의해 값이 언제든 바뀔 수 있으므로, 
// 컴파일러가 최적화(캐싱)하지 않고 매번 메모리 주소에서 직접 읽도록 강제함.
#define REG_DISPCNT     (*(volatile u32*)(REG_BASE + 0x0000)) // 디스플레이 제어
#define REG_VCOUNT      (*(volatile u16*)(REG_BASE + 0x0006)) // 수직 라인 카운터 (Scanline)
#define REG_KEYINPUT    (*(volatile u16*)(REG_BASE + 0x0130)) // 키 입력 상태

// [설정 상수]
#define MODE_3          0x0003 // 비트맵 모드 (240x160, 16bit Color)
#define BG2_ENABLE      0x0400 // 배경 레이어 2 활성화
#define SCREEN_W        240
#define SCREEN_H        160

// ---------------------------------------------------------
// 2. 색상 및 키 매크로
// ---------------------------------------------------------

// [RGB 매크로]
// GBA는 15bit 색상(0BBBBBGGGGGRRRRR)을 사용함.
// 각 채널 범위: 0 ~ 31
#define RGB(r, g, b)    ((u16)((r) | ((g) << 5) | ((b) << 10)))

// [기본 색상]
#define COLOR_BLACK     RGB(0,  0,  0)
#define COLOR_WHITE     RGB(31, 31, 31)
#define COLOR_RED       RGB(31, 0,  0)
#define COLOR_GREEN     RGB(0,  31, 0)
#define COLOR_BLUE      RGB(0,  0,  31)
#define COLOR_GOLD      RGB(31, 25, 0)

// [키 마스크]
// 각 비트가 버튼 하나에 대응됨.
#define KEY_A           0x0001
#define KEY_B           0x0002
#define KEY_SELECT      0x0004
#define KEY_START       0x0008
#define KEY_RIGHT       0x0010
#define KEY_LEFT        0x0020
#define KEY_UP          0x0040
#define KEY_DOWN        0x0080
#define KEY_R           0x0100
#define KEY_L           0x0200

// ---------------------------------------------------------
// 3. 전역 상태 변수 (Global State)
// ---------------------------------------------------------

// 플레이어 정보
// 초기 위치를 화면 중앙으로 설정 (Pivot 보정: 크기의 절반만큼 뺌)
int p_x = (SCREEN_W / 2) - 5;
int p_y = (SCREEN_H / 2) - 5;
int p_w = 10;
int p_h = 10;
int speed = 2;

// ---------------------------------------------------------
// 4. 유틸리티 함수
// ---------------------------------------------------------

// 화면 전체 지우기 (초기화 및 배경 변경 시 사용)
void clear_screen(u16 color) {
    for (int i = 0; i < SCREEN_W * SCREEN_H; ++i) {
        VRAM[i] = color;
    }
}

// 사각형 그리기 (클리핑 포함)
void draw_rect(int x, int y, int w, int h, u16 color) {
    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            // Object Space -> Screen Space 변환
            int draw_x = x + col;
            int draw_y = y + row;

            // [클리핑 (Clipping)]
            // 화면 밖의 좌표에 쓰려고 하면 메모리 침범(Memory Corruption) 발생 위험.
            // 유효한 좌표 범위 내에서만 그리기 수행.
            if (draw_x >= 0 && draw_x < SCREEN_W && draw_y >= 0 && draw_y < SCREEN_H) {
                VRAM[draw_y * SCREEN_W + draw_x] = color;
            }
        }
    }
}

// 수직 동기화 (VSync)
// 화면 갱신 중에 VRAM을 건드리면 '티어링(Tearing)' 현상 발생.
// 전자총이 그림을 다 그리고 처음으로 돌아가는 VBlank 구간(160~227)을 기다림.
void sync_vblank() {
    // 1. 이미 VBlank 중이라면 다음 프레임까지 대기 (Edge Trigger 유도)
    while (REG_VCOUNT >= 160);
    // 2. VBlank가 시작될 때까지 대기
    while (REG_VCOUNT < 160);
}

// ---------------------------------------------------------
// 5. 메인 함수 (Game Loop)
// ---------------------------------------------------------
int main() {
    // 하드웨어 초기화
    REG_DISPCNT = MODE_3 | BG2_ENABLE;

    // 초기 렌더링
    u16 background_color = COLOR_BLACK;
    clear_screen(background_color);
    draw_rect(p_x, p_y, p_w, p_h, COLOR_BLUE);

    while (1) {
        // --------------------------------------
        // [Step 1] 입력 처리 및 상태 갱신 (Update)
        // --------------------------------------
        
        // 잔상 처리를 위해 이동 전 좌표 기억
        int old_x = p_x;
        int old_y = p_y;

        // 키 입력 레지스터 읽기
        u16 key_state = REG_KEYINPUT;

        // 1-1. 배경 색상 변경 로직
        // GBA 버튼은 Active Low 방식 (눌림=0, 뗌=1).
        // 따라서 비트 마스킹 결과가 0일 때가 '눌린 상태'임.
        u16 new_bg_color = background_color;

        if      ( !(key_state & KEY_A) )      new_bg_color = COLOR_RED;
        else if ( !(key_state & KEY_B) )      new_bg_color = COLOR_GOLD;
        else if ( !(key_state & KEY_L) )      new_bg_color = COLOR_GREEN;
        else if ( !(key_state & KEY_R) )      new_bg_color = COLOR_WHITE;
        else if ( !(key_state & KEY_SELECT) ) new_bg_color = COLOR_BLACK;

        // [최적화 핵심]
        // 매 프레임 clear_screen을 호출하면 CPU 부하가 심해짐.
        // 상태(색상)가 실제로 바뀌었을 때만 화면 전체 갱신 수행.
        if (new_bg_color != background_color) {
            background_color = new_bg_color;
            clear_screen(background_color);
            // 배경을 지우면 플레이어도 지워지므로 즉시 다시 그림
            draw_rect(p_x, p_y, p_w, p_h, COLOR_BLUE);
        }

        // 1-2. 이동 로직
        if ( !(key_state & KEY_UP) )    p_y -= speed;
        if ( !(key_state & KEY_DOWN) )  p_y += speed;
        if ( !(key_state & KEY_LEFT) )  p_x -= speed;
        if ( !(key_state & KEY_RIGHT) ) p_x += speed;

        // 화면 밖으로 나가지 않도록 좌표 고정 (Clamping)
        if (p_x < 0) p_x = 0;
        if (p_x > SCREEN_W - p_w) p_x = SCREEN_W - p_w;
        if (p_y < 0) p_y = 0;
        if (p_y > SCREEN_H - p_h) p_y = SCREEN_H - p_h;


        // --------------------------------------
        // [Step 2] 타이밍 동기화 (Sync)
        // --------------------------------------
        sync_vblank();


        // --------------------------------------
        // [Step 3] 렌더링 (Render)
        // --------------------------------------
        
        // [더티 렉탱글 (Dirty Rectangle)]
        // 화면 전체를 지우지 않고, 움직임이 발생한 부분만 수정함.
        if (p_x != old_x || p_y != old_y) {
            // 1. 이전 위치 지우기: 현재 배경색으로 덮어씀 (잔상 제거)
            draw_rect(old_x, old_y, p_w, p_h, background_color);
            
            // 2. 새 위치 그리기: 플레이어 색상으로 그림
            draw_rect(p_x, p_y, p_w, p_h, COLOR_BLUE);
        }
    }

    return 0;
}