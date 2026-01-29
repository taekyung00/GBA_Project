#include "../include/fixed.h"
#include "../include/gba.h"
#include "../include/math_2d.h"



// =========================================================================
// 1. 그래픽 엔진 (Software Rasterizer)
// =========================================================================

// 안전하게 점 찍기 (화면 밖 체크)
void draw_pixel(int x, int y, u16 color) {
	if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
		VRAM[y * SCREEN_W + x] = color;
	}
}

// 화면 지우기
void clear_screen(u16 color) {
	u32 double_color = color | (color << 16);
	u32* vram_ptr = (u32*)VRAM;
	for (int i = 0; i < (SCREEN_W * SCREEN_H) / 2; ++i) {
		vram_ptr[i] = double_color;
	}
}

// [핵심] 브레즌햄 직선 알고리즘 (Bresenham's Line Algorithm)
// 정수 덧셈/뺄셈만으로 직선을 그리는 마법의 함수
void draw_line(int x0, int y0, int x1, int y1, u16 color) {
    int dx = math_abs(x1 - x0);
    int dy = math_abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    
    // 초기 오차: dx - dy (힘의 균형)
    int err = dx - dy; 

    while (1) {
        draw_pixel(x0, y0, color);

        if (x0 == x1 && y0 == y1) break; // 도착

        int e2 = 2 * err; // 2배 뻥튀기 (반올림 방지)

        // Y축 힘보다 덜 치우쳤으면 -> X 이동
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        // X축 힘보다 덜 치우쳤으면 -> Y 이동
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// [응용] 삼각형 그리기 (Wireframe)
void draw_triangle_wire(int x1, int y1, int x2, int y2, int x3, int y3, u16 color) {
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x3, y3, color);
    draw_line(x3, y3, x1, y1, color);
}

// =========================================================================
// 3. 메인 게임 루프
// =========================================================================
int main() {
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    
    // 삼각형 모델 (Local Space, 중심점 0,0)
    Vec2 local_model[3] = {
        { INT_TO_FIX(0),   INT_TO_FIX(-20) }, 
        { INT_TO_FIX(-15), INT_TO_FIX(15)  }, 
        { INT_TO_FIX(15),  INT_TO_FIX(15)  } 
    };

    // 초기 상태 (Transform)
    Transform player_tf;
    player_tf.position.x = INT_TO_FIX(120); // 화면 중앙
    player_tf.position.y = INT_TO_FIX(80);
    player_tf.scale      = INT_TO_FIX(1);   // 1.0배
    player_tf.angle      = 0.0f;


    while (1) {
        // [Input] 입력 처리
        u16 keys = REG_KEYINPUT;

        // 이동 (D-Pad)
        if (!(keys & KEY_UP))    player_tf.position.y -= INT_TO_FIX(2);
        if (!(keys & KEY_DOWN))  player_tf.position.y += INT_TO_FIX(2);
        if (!(keys & KEY_LEFT))  player_tf.position.x -= INT_TO_FIX(2);
        if (!(keys & KEY_RIGHT)) player_tf.position.x += INT_TO_FIX(2);

        // 회전 (L/R 버튼)
        if (!(keys & KEY_L))     player_tf.angle -= 0.05f;
        if (!(keys & KEY_R))     player_tf.angle += 0.05f;

        // 크기 (A/B 버튼)
        if (!(keys & KEY_A))     player_tf.scale += INT_TO_FIX(1) / 10; // +0.1
        if (!(keys & KEY_B))     player_tf.scale -= INT_TO_FIX(1) / 10; // -0.1
        
        // 크기 제한 (너무 작아지면 안보임)
        if (player_tf.scale < INT_TO_FIX(1)/5) player_tf.scale = INT_TO_FIX(1)/5;


        // [Render] 그리기
        sync_vblank();             // 동기화
        clear_screen(RGB(0,0,0));  // 지우기

        // 변환 및 그리기 루프
        Vec2 drawn_v[3];
        for(int i=0; i<3; ++i) {
            drawn_v[i] = apply_transform(local_model[i], player_tf);
        }

        // 와이어프레임 그리기
        draw_triangle_wire(
            FIX_TO_INT(drawn_v[0].x), FIX_TO_INT(drawn_v[0].y),
            FIX_TO_INT(drawn_v[1].x), FIX_TO_INT(drawn_v[1].y),
            FIX_TO_INT(drawn_v[2].x), FIX_TO_INT(drawn_v[2].y),
            RGB(0, 31, 0) // 녹색
        );

        // UI 가이드 (십자가)
        draw_line(120, 75, 120, 85, RGB(5,5,5));
        draw_line(115, 80, 125, 80, RGB(5,5,5));
    }
    return 0;
}