#ifndef MATH_2D_H
#define MATH_2D_H

#include "fixed.h" // fixed 타입 사용

// =========================================================================
// 1. 자료형 정의 (Data Structures)
// =========================================================================

// 2D 벡터 (위치, 속도 등)
typedef struct {
    fixed x, y;
} Vec2;

// 3x3 행렬 (이동, 회전, 크기 변환용)
// GBA는 2D지만 동차 좌표계(Homogeneous coordinates)를 위해 3x3 사용
typedef struct {
    fixed m[3][3];
} Mat3;

// 트랜스폼 컴포넌트 (게임 오브젝트의 기본 속성)
typedef struct {
    Vec2 position;  // 위치
    fixed scale;    // 크기 (1.0 = FIX_SCALE)
    float angle;    // 회전 (라디안)
} Transform;

// =========================================================================
// 2. 함수 선언 (Prototypes)
// =========================================================================

// 벡터 생성 헬퍼
static inline Vec2 vec2_new(fixed x, fixed y) {
    Vec2 v = {x, y};
    return v;
}

// 기본 수학 유틸
int math_abs(int n);

// 행렬 연산
Mat3 mat3_identity();                   // 단위 행렬 반환
Mat3 mat3_rotate(float angle_rad);      // 회전 행렬 생성
Vec2 mat3_mul_vec2(Mat3 mat, Vec2 v);   // 행렬 x 벡터 곱셈

// SRT 변환 (Scale -> Rotate -> Translate) 통합 적용
// 최적화된 함수: 행렬을 직접 만들지 않고 수식으로 바로 계산
Vec2 apply_transform(Vec2 v, Transform t);
#endif