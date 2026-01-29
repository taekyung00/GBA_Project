#include "../include/math_2d.h"
#include <math.h> // sin, cos 사용 (나중에는 LUT로 대체), 직접 링킹해주어야함!!!!
//LDFLAGS    := $(ARCH) -specs=gba.specs -lm << -lm 추가

// 절댓값 함수
int math_abs(int n) {
    return (n < 0) ? -n : n;
}

// 단위 행렬 (아무런 변화가 없는 행렬)
Mat3 mat3_identity() {
    Mat3 mat;
    // 1 0 0
    // 0 1 0
    // 0 0 1
    mat.m[0][0] = FIX_ONE; mat.m[0][1] = 0;       mat.m[0][2] = 0;
    mat.m[1][0] = 0;       mat.m[1][1] = FIX_ONE; mat.m[1][2] = 0;
    mat.m[2][0] = 0;       mat.m[2][1] = 0;       mat.m[2][2] = FIX_ONE;
    return mat;
}

// 회전 행렬 생성
Mat3 mat3_rotate(float angle_rad) {
    Mat3 mat;
    fixed c = FLOAT_TO_FIX(cos(angle_rad));
    fixed s = FLOAT_TO_FIX(sin(angle_rad));

    mat.m[0][0] = c;  mat.m[0][1] = -s; mat.m[0][2] = 0;
    mat.m[1][0] = s;  mat.m[1][1] = c;  mat.m[1][2] = 0;
    mat.m[2][0] = 0;  mat.m[2][1] = 0;  mat.m[2][2] = FIX_ONE;
    return mat;
}

// 행렬 x 벡터
Vec2 mat3_mul_vec2(Mat3 mat, Vec2 v) {
    Vec2 result;
    result.x = fix_mul(mat.m[0][0], v.x) + fix_mul(mat.m[0][1], v.y) + mat.m[0][2];
    result.y = fix_mul(mat.m[1][0], v.x) + fix_mul(mat.m[1][1], v.y) + mat.m[1][2];
    return result;
}

// SRT 변환 적용 (최적화 버전)
// M = T * R * S
Vec2 apply_transform(Vec2 v, Transform t) {
    Vec2 result;
    
    // 1. 삼각함수 준비
    fixed c = FLOAT_TO_FIX(cos(t.angle));
    fixed s = FLOAT_TO_FIX(sin(t.angle));

    // 2. Scale & Rotate 적용
    // x' = (x * sx * cos) - (y * sy * sin)
    // y' = (x * sx * sin) + (y * sy * cos)
    
    // 스케일 먼저 적용
    fixed sx_x = fix_mul(v.x, t.scale);
    fixed sy_y = fix_mul(v.y, t.scale);

    fixed rotated_x = fix_mul(sx_x, c) - fix_mul(sy_y, s);
    fixed rotated_y = fix_mul(sx_x, s) + fix_mul(sy_y, c);

    // 3. Translate (이동) 적용
    result.x = rotated_x + t.position.x;
    result.y = rotated_y + t.position.y;

    return result;
}