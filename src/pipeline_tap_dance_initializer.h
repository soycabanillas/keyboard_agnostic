#pragma once

#include <stddef.h>
#include <stdint.h>
#include "pipeline_tap_dance.h"
#include "platform_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

pipeline_tap_dance_action_config_t* createbehaviouraction(uint8_t repetitions, td_customlayer_action_t action, platform_keycode_t keycode, uint8_t layer);
pipeline_tap_dance_behaviour_t* createbehaviour(platform_keycode_t keycodemodifier, pipeline_tap_dance_action_config_t* actions[], size_t actionslength);

#ifdef __cplusplus
}
#endif
