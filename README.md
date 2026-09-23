# TestAwait

A minimal standalone application that exercises the
[SimpleAwait](https://github.com/noelanderson/SimpleAwait) cooperative-coroutine
library on a **Raspberry Pi Pico 2 (RP2350, Arm)**.

It extends the quick-start example from the SimpleAwait README: three independent
blink `Task`s plus two worker `Task`s that each wake on their own interval and
print, over `Serial`, how many milliseconds actually elapsed since they last ran.

## What it does

- Three `blink(pin, period_on_ms, period_off_ms)` tasks toggle the onboard LED
  (on 500 ms / off 1000 ms), GP16 (500 ms / 500 ms), and GP17 (300 ms / 300 ms)
  independently, each using `co_await delay_ms(...)`.
- `worker("worker A", 1350)` and `worker("worker B", 1200)` wake every 1350 ms and
  1200 ms respectively and print the real elapsed time (measured with `millis()`),
  so you can watch the two independent cadences interleave on the serial monitor.
- `loop()` just pumps `poll()`; all control flow is ordinary sequential coroutine
  code, not a hand-written `millis()` state machine.

Expected serial output (115200 baud):

```text
worker B: 1200 ms since last run
worker A: 1350 ms since last run
worker B: 1201 ms since last run
worker B: 1200 ms since last run
worker A: 1350 ms since last run
...
```

## Target

| | |
|---|---|
| Board | Raspberry Pi Pico 2 |
| MCU | RP2350 (Arm Cortex-M33) |
| Core | [arduino-pico](https://github.com/earlephilhower/arduino-pico) |
| FQBN | `rp2040:rp2040:rpipico2` |

The arduino-pico core compiles at `-std=gnu++23` by default, which satisfies
SimpleAwait's C++20 requirement — no extra build flags are needed.

## The library dependency

SimpleAwait is consumed as a git submodule at [`lib/SimpleAwait`](lib/SimpleAwait),
pinned to a released tag. Clone with submodules:

```sh
git clone --recurse-submodules <this-repo-url>
# or, if already cloned:
git submodule update --init --recursive
```

## Build

With [arduino-cli](https://arduino.github.io/arduino-cli/):

```sh
# one-time: install the RP2040/RP2350 core
arduino-cli config init
arduino-cli config add board_manager.additional_urls \
  https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
arduino-cli core update-index
arduino-cli core install rp2040:rp2040

# compile for the Pico 2 (RP2350, Arm) and export the binaries next to the sketch
arduino-cli compile --fqbn rp2040:rp2040:rpipico2 --library lib/SimpleAwait --export-binaries .
```

> **Where's the `.uf2`?** 
> Passing `--export-binaries` (as above) writes the artifacts to
> `build/rp2040.rp2040.rpipico2/`, giving you `build/rp2040.rp2040.rpipico2/TestAwait.ino.uf2`.
>
> Alternatively use `--output-dir dist` to collect them into `dist/`. 
>
> (Both `build/` and `*.uf2` are git-ignored.)

To flash, either let arduino-cli do it over USB:

```sh
arduino-cli compile --fqbn rp2040:rp2040:rpipico2 --library lib/SimpleAwait --upload --port <PORT> .
```

or hold **BOOTSEL** while plugging in the board and drag the exported
`TestAwait.ino.uf2` onto the `RP2350` mass-storage drive.

## Prebuilt UF2 (releases)

Tagged [releases](https://github.com/noelanderson/TestAwait/releases) attach a
prebuilt, flashable `.uf2` for the Pico 2, built in CI by
[`.github/workflows/release.yml`](.github/workflows/release.yml). To flash without
building anything yourself:

1. Download `TestAwait-<tag>-rpipico2.uf2` from the latest release.
2. Hold **BOOTSEL** while plugging the Pico 2 into USB; it mounts as an `RP2350`
   mass-storage drive.
3. Copy the `.uf2` onto that drive — the board reboots and runs immediately.
4. Open the USB serial monitor at 115200 baud to watch the two workers report.

To cut a new release, push a `v*` tag (for example `git tag v1.0.0 && git push
origin v1.0.0`); the workflow compiles the sketch and publishes the `.uf2`.

## CI

[`.github/workflows/ci.yml`](.github/workflows/ci.yml) compiles the sketch for
`rp2040:rp2040:rpipico2` on every push and pull request, checking out the
SimpleAwait submodule first.

## License

The SimpleAwait library is MIT-licensed by Noel Anderson. This example sketch is
provided under the same terms.
