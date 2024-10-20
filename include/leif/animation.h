#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef float (*lf_animation_func_t)(float t);

typedef struct lf_animation_t lf_animation_t;

typedef struct {
  float start, end, duration;
  lf_animation_func_t easing;  
} lf_animation_keyframe_t;

struct lf_animation_t {
  float* target;          
  float elapsed_time;
  bool active; 

  lf_animation_t* next;

  lf_animation_keyframe_t* keyframes;
  uint32_t n_keyframes;
  uint32_t i_keyframes;

  bool looping;
};

lf_animation_t* lf_animation_create(
    lf_animation_t** head,
    float *target,
    lf_animation_keyframe_t keyframes[],
    uint32_t n_keyframes,
    bool looping
    );

void lf_animation_update(lf_animation_t* anim, float dt);

void lf_animation_interrupt(lf_animation_t* head, float* target);
