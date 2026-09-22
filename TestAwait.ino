// TestAwait — a standalone app that exercises the SimpleAwait library on a
// Raspberry Pi Pico 2 (RP2350, Arm).
//
// This is the quick-start example from the SimpleAwait README: one cooperative
// blink Task plus two worker Tasks that each wake on their own interval and
// print, over Serial, how many milliseconds actually elapsed since they last ran.
//
//   Target : rp2040:rp2040:rpipico2  (Raspberry Pi Pico 2, RP2350 Arm)
//   Library: lib/SimpleAwait          (git submodule)

#include <SimpleAwait.h>

// The Pico 2's onboard LED is GPIO 25; the core defines LED_BUILTIN for this
// board, but fall back just in case the sketch is retargeted.
#ifndef LED_BUILTIN
#define LED_BUILTIN 25
#endif

using namespace simpleawait;

Task<void> blink() {
    while (true) {
        digitalWrite(LED_BUILTIN, HIGH);
        co_await delay_ms(500);
        digitalWrite(LED_BUILTIN, LOW);
        co_await delay_ms(500);
    }
}

// Each worker wakes on its own interval and reports how long it actually slept.
Task<void> worker(const char* name, uint32_t period_ms) {
    uint32_t last = millis();
    while (true) {
        co_await delay_ms(period_ms);
        uint32_t now = millis();
        Serial.print(name);
        Serial.print(": ");
        Serial.print(now - last);
        Serial.println(" ms since last run");
        last = now;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    spawn(blink());
    spawn(worker("worker A", 1350));
    spawn(worker("worker B", 1200));
}

void loop() { poll(); }
