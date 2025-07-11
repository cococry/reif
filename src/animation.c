#include "../include/leif/animation.h"
#include "../include/leif/util.h"
#include "../include/leif/win.h"

#include <math.h>
#include <stdlib.h>

lf_animation_keyframe_t keyframe_create(
  lf_animation_keyframe_t kf,
  float* target
) {
  (void)target; 
  float remaining_distance = fabsf(kf.end - (*target));
  if (remaining_distance <= 0.0f) return (lf_animation_keyframe_t){0};

  float full_distance = fabsf(kf.end - kf.start);
  if(full_distance <= 0.0f)  return (lf_animation_keyframe_t){0};

  float remaining_time = MAX(kf.duration, kf.duration * (remaining_distance / full_distance)); 
  return (lf_animation_keyframe_t){
    .start = *target, 
    .end = kf.end, 
    .duration = remaining_time,
    .easing = kf.easing,
  };
}

lf_animation_t*
lf_animation_create(
    lf_animation_t** head,
    float *target,
    lf_animation_keyframe_t keyframes[],
    uint32_t n_keyframes,
    bool looping
) {
    if (!head || !target || n_keyframes == 0 || !keyframes)
        return NULL;

    lf_animation_t* anim = malloc(sizeof(*anim));
    if (!anim)
        return NULL;

    anim->target = target;
    anim->i_keyframes = 0;
    anim->elapsed_time = 0.0f;
    anim->active = true;
    anim->looping = looping;
    anim->finish_cb = NULL;
    anim->tick_cb = NULL;
    anim->user_data = NULL;

    // Allocate keyframes
    anim->keyframes = NULL;
    anim->n_keyframes = 0;

    if (n_keyframes == 1) {
        lf_animation_keyframe_t keyframe = keyframe_create(keyframes[0], target);
        if (keyframe.duration > 0.0f) {
            anim->keyframes = malloc(sizeof(lf_animation_keyframe_t));
            if (!anim->keyframes) {
                free(anim);
                return NULL;
            }
            anim->keyframes[0] = keyframe;
            anim->n_keyframes = 1;
        } else {
            free(anim);
            return NULL;
        }
    } else {
        anim->keyframes = malloc(sizeof(lf_animation_keyframe_t) * n_keyframes);
        if (!anim->keyframes) {
            free(anim);
            return NULL;
        }
        for (uint32_t i = 0; i < n_keyframes; ++i)
            anim->keyframes[i] = keyframes[i];
        anim->n_keyframes = n_keyframes;
    }

    anim->next = *head;
    *head = anim;

    lf_windowing_set_wait_events(false);
    lf_windowing_wake_up_event_loop();

    return anim;
}

void 
lf_animation_update(lf_animation_t* anim, float dt) {
  if(!anim || !anim->keyframes || anim->n_keyframes <= 0) return;

  anim->elapsed_time += dt;

  if(anim->tick_cb) {
    anim->tick_cb(anim, anim->user_data);
  }

  lf_animation_keyframe_t* kf = &anim->keyframes[anim->i_keyframes];

  if(kf->duration <= 0.0f || !kf->easing) return;

  float t = anim->elapsed_time / kf->duration;

  if (t >= 1.0f) {
    *anim->target = kf->end; 
    anim->elapsed_time = 0.0f;

    if (anim->i_keyframes + 1 < anim->n_keyframes) {
      anim->i_keyframes++;
    } else {
      if(anim->looping) {
        anim->i_keyframes = 0;
      } else {
        anim->active = false;
        if(anim->finish_cb) {
          anim->finish_cb(anim, anim->user_data);
        }
        lf_windowing_set_wait_events(true);
      }
    }
  } else {
    float eased_t = kf->easing(t);
    *anim->target = kf->start + (kf->end - kf->start) * eased_t;
  }
}


void 
lf_animation_interrupt(lf_animation_t* head, float* target) {
  if(!head || !target) return;
  lf_animation_t* anim = head;
  while(anim) {
    if(anim->target == target) {
      anim->active = false;
    }
    anim = anim->next;
  }
}
