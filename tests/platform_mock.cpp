#include "../src/platform_interface.h"
#include "platform_mock.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <cstdint>

// Mock implementation of platform interface for testing

// MockPlatformState method implementations
MockPlatformState::MockPlatformState() : timer(0), current_layer(0), next_token(1),
                     last_key_event_time(0), last_sent_key(0),
                     last_registered_key(0), last_unregistered_key(0),
                     last_selected_layer(0) {}

void MockPlatformState::record_key_event(platform_keycode_t keycode, bool pressed) {
    key_events.push_back({keycode, pressed, timer});
    last_key_event_time = timer;
    if (pressed) {
        pressed_keys.insert(keycode);
    } else {
        pressed_keys.erase(keycode);
    }
}

void MockPlatformState::advance_timer(platform_time_t ms) {
    timer += ms;
}

void MockPlatformState::reset() {
    timer = 0;
    current_layer = 0;
    next_token = 1;
    last_key_event_time = 0;

    key_events.clear();
    pressed_keys.clear();
    send_key_calls.clear();
    register_key_calls.clear();
    unregister_key_calls.clear();
    layer_select_calls.clear();

    last_sent_key = 0;
    last_registered_key = 0;
    last_unregistered_key = 0;
    last_selected_layer = 0;
}

void MockPlatformState::print_state() const {
    printf("=== MOCK STATE ===\n");
    printf("Current time: %u ms\n", timer);
    printf("Current layer: %u\n", current_layer);
    printf("Time since last key event: %u ms\n", timer - last_key_event_time);

    printf("Currently pressed keys (%zu): ", pressed_keys.size());
    for (auto keycode : pressed_keys) {
        printf("%u ", keycode);
    }
    printf("\n");

    printf("Key event history (%zu events):\n", key_events.size());
    for (size_t i = 0; i < key_events.size(); i++) {
        const auto& event = key_events[i];
        if (i == 0) {
            printf("  %u ms: Key %u %s (first event)\n",
                   event.timestamp, event.keycode,
                   event.pressed ? "PRESS" : "RELEASE");
        } else {
            platform_time_t time_diff = event.timestamp - key_events[i-1].timestamp;
            printf("  %u ms: Key %u %s (+%u ms)\n",
                   event.timestamp, event.keycode,
                   event.pressed ? "PRESS" : "RELEASE", time_diff);
        }
    }
    printf("==================\n");
}

int MockPlatformState::send_key_calls_count() const { return send_key_calls.size(); }
int MockPlatformState::register_key_calls_count() const { return register_key_calls.size(); }
int MockPlatformState::unregister_key_calls_count() const { return unregister_key_calls.size(); }
int MockPlatformState::layer_select_calls_count() const { return layer_select_calls.size(); }
int MockPlatformState::key_events_count() const { return key_events.size(); }
size_t MockPlatformState::pressed_keys_count() const { return pressed_keys.size(); }

bool MockPlatformState::is_key_pressed(platform_keycode_t keycode) const {
    return pressed_keys.find(keycode) != pressed_keys.end();
}

// Global mock state
MockPlatformState g_mock_state;

// Mock key operations
void platform_send_key(platform_keycode_t keycode) {
    printf("MOCK: Send key %u\n", keycode);
    g_mock_state.send_key_calls.push_back(keycode);
    g_mock_state.last_sent_key = keycode;
    // Record as press then release
    g_mock_state.record_key_event(keycode, true);
    g_mock_state.record_key_event(keycode, false);
}

void platform_register_key(platform_keycode_t keycode) {
    printf("MOCK: Register key %u\n", keycode);
    g_mock_state.register_key_calls.push_back(keycode);
    g_mock_state.last_registered_key = keycode;
    g_mock_state.record_key_event(keycode, true);
}

void platform_unregister_key(platform_keycode_t keycode) {
    printf("MOCK: Unregister key %u\n", keycode);
    g_mock_state.unregister_key_calls.push_back(keycode);
    g_mock_state.last_unregistered_key = keycode;
    g_mock_state.record_key_event(keycode, false);
}

// Mock layer operations
void platform_layer_select(uint8_t layer) {
    printf("MOCK: Layer select %u\n", layer);
    g_mock_state.layer_select_calls.push_back(layer);
    g_mock_state.current_layer = layer;
    g_mock_state.last_selected_layer = layer;
}

// Mock time operations
void platform_wait_ms(platform_time_t ms) {
    g_mock_state.advance_timer(ms);
}

platform_time_t platform_timer_read(void) {
    return g_mock_state.timer;
}

platform_time_t platform_timer_elapsed(platform_time_t last) {
    return g_mock_state.timer - last;
}

// Mock deferred execution
platform_deferred_token platform_defer_exec(uint32_t delay_ms, void (*callback)(void*), void* data) {
    printf("MOCK: Defer exec for %u ms\n", delay_ms);
    (void)callback;
    (void)data;
    return g_mock_state.next_token++;
}

bool platform_cancel_deferred_exec(platform_deferred_token token) {
    printf("MOCK: Cancel deferred exec token %u\n", token);
    return true;
}

// Mock memory operations
void* platform_malloc(size_t size) {
    return malloc(size);
}

void platform_free(void* ptr) {
    free(ptr);
}

// Test utilities
void mock_advance_timer(platform_time_t ms) {
    g_mock_state.advance_timer(ms);
}

void mock_reset_timer(void) {
    g_mock_state.timer = 0;
}

void mock_set_layer(uint8_t layer) {
    g_mock_state.current_layer = layer;
}

void mock_print_state(void) {
    g_mock_state.print_state();
}

void reset_mock_state(void) {
    g_mock_state.reset();
}
