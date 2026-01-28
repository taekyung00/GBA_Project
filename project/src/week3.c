#include "../include/fixed.h"
#include "../include/gba.h"
#include <math.h> // sin, cos 사용 (나중에는 LUT로 대체)

// 절댓값 함수 (stdlib 없이 구현)
int my_abs(int n) { return (n < 0) ? -n : n; }

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
    int dx = my_abs(x1 - x0);
    int dy = my_abs(y1 - y0);
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
// 2. 수학 엔진 (Math Engine) - 여기가 핵심!
// =========================================================================

// 점 구조체
typedef struct {
    fixed x, y;
} Vec2;

// 2D 변환 행렬 구조체 (3x3 for Affine)
/*
   [ m00 m01 m02 ]   [ scale/rot  scale/rot  transX ]
   [ m10 m11 m12 ] = [ scale/rot  scale/rot  transY ]
   [ m20 m21 m22 ]   [    0           0        1    ]
*/
typedef struct {
    fixed m[3][3];
} Mat3;

// 행렬과 벡터 곱하기 (Apply Matrix to Vertex)
// CS200 Vertex Shader의 "gl_Position = uModel * vec4(pos, 1.0)" 역할
Vec2 mat3_mul_vec2(Mat3 mat, Vec2 v) {
    Vec2 result;
    // x' = m00*x + m01*y + m02*1
    result.x = fix_mul(mat.m[0][0], v.x) + fix_mul(mat.m[0][1], v.y) + mat.m[0][2];
    // y' = m10*x + m11*y + m12*1
    result.y = fix_mul(mat.m[1][0], v.x) + fix_mul(mat.m[1][1], v.y) + mat.m[1][2];
    return result;
}
// =========================================================================
// 2. 메인 게임 루프
// =========================================================================
int main() {
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    
    // 초기화
    clear_screen(RGB(0, 0, 0));

    // 1. 십자가 테스트 (직선)
    draw_line(120, 10, 120, 150, RGB(10, 10, 10)); // 어두운 회색 세로선
    draw_line(10, 80, 230, 80, RGB(10, 10, 10));   // 어두운 회색 가로선

    // 2. 삼각형 테스트 (Wireframe)
    // 화면 중앙에 빨간 삼각형 그리기
    int p1_x = 120, p1_y = 30;
    int p2_x = 80,  p2_y = 100;
    int p3_x = 160, p3_y = 100;

    draw_triangle(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, RGB(31, 0, 0));

    // 3. 겹쳐진 삼각형 (파란색)
    draw_triangle(120, 130, 80, 60, 160, 60, RGB(0, 0, 31));

    while (1) {
        // 아직은 움직임 없음 (Static)
    }

    return 0;
}