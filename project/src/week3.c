#include <stdlib.h>	 // abs() 사용

#include "../include/fixed.h"
#include "../include/gba.h"

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
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;  // x 이동 방향
	int sy = (y0 < y1) ? 1 : -1;  // y 이동 방향
	int err = dx - dy;			  // 오차 누적 값

	while (1) {
		draw_pixel(x0, y0, color);

		if (x0 == x1 && y0 == y1) break;  // 도착

		int e2 = 2 * err;
		

		// x축으로 이동할지 결정
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		// y축으로 이동할지 결정
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}