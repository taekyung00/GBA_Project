// =========================================================================
// GBA Engine Core: Week 2 Implementation (Final)
// =========================================================================

#include "../include/fixed.h"  // fixed_zero, fixed_screen_w 등이 여기에 정의됨
#include "../include/gba.h"    // 하드웨어 레지스터 및 타입

// 매크로 상수는 대문자가 관례이지만, 편의상 소문자로 쓰신 부분 존중합니다.
#define PLAYER_W 16
#define PLAYER_H 16

// -------------------------------------------------------------------------
// 데이터 구조
// -------------------------------------------------------------------------
typedef struct {
    fixed x, y;   
    u16 color;    
} Vertex;

// -------------------------------------------------------------------------
// 렌더링 함수
// -------------------------------------------------------------------------
void clear_screen(u16 color) {
    u32 double_color = color | (color << 16);
    u32* vram_ptr = (u32*)VRAM;
    for (int i = 0; i < (SCREEN_W * SCREEN_H) / 2; ++i) {
        vram_ptr[i] = double_color;
    }
}

void draw_rect(fixed fx, fixed fy, int w, int h, u16 color) {
    int x = FIX_TO_INT(fx);
    int y = FIX_TO_INT(fy);

    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            int draw_x = x + col;
            int draw_y = y + row;

            if (draw_x >= 0 && draw_x < SCREEN_W && draw_y >= 0 && draw_y < SCREEN_H) {
                VRAM[draw_y * SCREEN_W + draw_x] = color;
            }
        }
    }
}

// -------------------------------------------------------------------------
// 메인 게임 루프
// -------------------------------------------------------------------------
int main() {

    // [Debug Trap]
    #ifdef DEBUG_MODE
        volatile int debug_wait = 1;
        while (debug_wait) { }
    #endif

    // [1. Window Initialization]
    REG_DISPCNT = MODE_3 | BG2_ENABLE;

    // [2. Data Initialization]
    Vertex player = {
        INT_TO_FIX(SCREEN_W / 2 - PLAYER_W / 2),
        INT_TO_FIX(SCREEN_H / 2 - PLAYER_H / 2),
        COLOR_BLUE
    };

    fixed speed = INT_TO_FIX(2);

    // 초기 렌더링
    u16 background_color = COLOR_BLACK;
    clear_screen(background_color);
    draw_rect(player.x, player.y, PLAYER_W, PLAYER_H, player.color);

    while (1) {
        // [Step 1] Input & Update
        fixed old_x = player.x;
        fixed old_y = player.y;

        u16 keys = REG_KEYINPUT;
        u16 new_bg_color = background_color;

        // 배경색 변경
        if      ( !(keys & KEY_A) )      new_bg_color = COLOR_RED;
        else if ( !(keys & KEY_B) )      new_bg_color = COLOR_GOLD;
        else if ( !(keys & KEY_L) )      new_bg_color = COLOR_GREEN;
        else if ( !(keys & KEY_R) )      new_bg_color = COLOR_WHITE;
        else if ( !(keys & KEY_SELECT) ) new_bg_color = COLOR_BLACK;

        if (new_bg_color != background_color) {
            background_color = new_bg_color;
            clear_screen(background_color);
            draw_rect(player.x, player.y, PLAYER_W, PLAYER_H, player.color);
        }

        // 이동 로직
        if (!(keys & KEY_UP))    player.y -= speed;
        if (!(keys & KEY_DOWN))  player.y += speed;
        if (!(keys & KEY_LEFT))  player.x -= speed;
        if (!(keys & KEY_RIGHT)) player.x += speed;

        // -------------------------------------------------
        // [Clamping] 화면 밖으로 나가지 않도록 좌표 고정
        // -------------------------------------------------
        if (player.x < fixed_zero) player.x = fixed_zero;
        if (player.y < fixed_zero) player.y = fixed_zero;
        
        // 오른쪽/아래쪽 벽 체크
        // 주의: player_w/h는 정수이므로 INT_TO_FIX 변환 필요
        if (player.x > fixed_screen_w - INT_TO_FIX(PLAYER_W)) 
            player.x = fixed_screen_w - INT_TO_FIX(PLAYER_W);
            
        if (player.y > fixed_screen_h - INT_TO_FIX(PLAYER_H)) 
            player.y = fixed_screen_h - INT_TO_FIX(PLAYER_H);

        // [Step 2] Sync
        sync_vblank();

        // [Step 3] Render
        if (old_x != player.x || old_y != player.y) {
            draw_rect(old_x, old_y, PLAYER_W, PLAYER_H, background_color);
            draw_rect(player.x, player.y, PLAYER_W, PLAYER_H, player.color);
        }
    }

    return 0;
}