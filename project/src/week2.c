// =========================================================================
// GBA Engine Core: Week 2 Implementation
// -------------------------------------------------------------------------
// CS200 개념 매핑:
// 1. Window Init -> REG_DISPCNT 설정
// 2. Input -> REG_KEYINPUT 폴링
// 3. Math -> Fixed Point 연산
// 4. Render -> VRAM 직접 접근 (Software Rasterizer 기초)
// =========================================================================

#include "../include/gba.h"   // 하드웨어 레지스터 및 타입
#include "../include/fixed.h" // 고정 소수점 수학 라이브러리

// -------------------------------------------------------------------------
// 데이터 구조 (Data Structure)
// -------------------------------------------------------------------------
// CS200의 VAO/VBO 대신 구조체를 사용하여 데이터를 정의합니다.
typedef struct {
    fixed x, y;   // 물리 좌표 (고정 소수점)
    u16 color;    // 색상 데이터
} Vertex;

// -------------------------------------------------------------------------
// 렌더링 함수 (Renderer)
// -------------------------------------------------------------------------

// 화면 전체 지우기
void clear_screen(u16 color) {
    // VRAM은 1차원 배열처럼 접근 가능합니다.
    for (int i = 0; i < SCREEN_W * SCREEN_H; ++i) {
        VRAM[i] = color;
    }
}

// 사각형 그리기 (Software Rasterizer)
// 물리 좌표(Fixed)를 받아서 화면 좌표(Int)로 변환해 그립니다.
void draw_rect(fixed fx, fixed fy, int w, int h, u16 color) {
    // [Rendering Stage]: Fixed -> Int 변환 (하드웨어는 고정 소수점을 모름) 
    int x = FIX_TO_INT(fx); // 
    int y = FIX_TO_INT(fy);

    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            int draw_x = x + col;
            int draw_y = y + row;

            // 화면 범위 체크 (Clipping)
            if (draw_x >= 0 && draw_x < SCREEN_W && draw_y >= 0 && draw_y < SCREEN_H) {
                VRAM[draw_y * SCREEN_W + draw_x] = color;
            }
        }
    }
}

// -------------------------------------------------------------------------
// 메인 게임 루프 (Game Loop)
// -------------------------------------------------------------------------
int main() {
    // [1. Window Initialization]
    // OS가 없으므로 하드웨어 레지스터를 직접 설정하여 화면 모드를 켭니다.
    REG_DISPCNT = MODE_3 | BG2_ENABLE;

    // [2. Data Initialization]
    // 정수를 고정 소수점으로 변환하여 물리 세계로 진입합니다.
    Vertex player = {
        INT_TO_FIX(SCREEN_W / 2 - 8), // 화면 중앙 (x)
        INT_TO_FIX(SCREEN_H / 2 - 8), // 화면 중앙 (y)
        COLOR_BLUE
    };

    fixed speed = INT_TO_FIX(2); // 이동 속도: 2.0 pixel/frame

    // 무한 루프 (OS로 복귀하지 않음)
    while (1) {
        // -------------------------------------------------
        // [Step 1] Input & Update (Physics World)
        // -------------------------------------------------
        
        // 키 입력 레지스터 읽기 (Active Low: 누르면 0)
        u16 keys = REG_KEYINPUT;

        // 고정 소수점 연산으로 좌표 갱신 
        if (!(keys & KEY_UP))    player.y -= speed;
        if (!(keys & KEY_DOWN))  player.y += speed;
        if (!(keys & KEY_LEFT))  player.x -= speed;
        if (!(keys & KEY_RIGHT)) player.x += speed;

        // -------------------------------------------------
        // [Step 2] Sync (Timing)
        // -------------------------------------------------
        // VBlank(화면 그리기 휴식 시간)를 기다립니다.
        sync_vblank();

        // -------------------------------------------------
        // [Step 3] Render (Visual World)
        // -------------------------------------------------
        
        // 1. 화면 지우기 (배경색)
        clear_screen(COLOR_BLACK);

        // 2. 플레이어 그리기
        // 이 시점에 Fixed -> Int 변환이 일어납니다.
        draw_rect(player.x, player.y, 16, 16, player.color);
    }

    return 0;
}