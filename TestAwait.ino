// TestAwait — a standalone app that exercises the SimpleAwait library on a
// Raspberry Pi Pico 2 (RP2350, Arm).
//
//
//   Target : rp2040:rp2040:rpipico2  (Raspberry Pi Pico 2, RP2350 Arm)
//   Library: lib/SimpleAwait          (git submodule)

#include <SimpleAwait.h>

#if !defined(LED_BUILTIN)
    #define LED_BUILTIN 25
#endif // LED_BUILTIN 

#define GP16 16
#define GP17 17

using namespace simpleawait;

Task<void> blink(int pin, uint32_t period_on_ms, uint32_t period_off_ms) {
    while (true) {
        digitalWrite(pin, HIGH);
        co_await delay_ms(period_on_ms);
        digitalWrite(pin, LOW);
        co_await delay_ms(period_off_ms);
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
    pinMode(GP16, OUTPUT);
    pinMode(GP17, OUTPUT);

    spawn(blink(LED_BUILTIN, 500, 1000));
    spawn(blink(GP16, 500, 500));
    spawn(blink(GP17, 300, 300));
    spawn(worker("worker A", 1350));
    spawn(worker("worker B", 1200));
}

void loop() { 
    poll(); 
}
