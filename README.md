# ESPHomeLightEffects
A collection of custom light effects I made for ESPHome. These are used in stair lighting, where a single addressable LED strip runs along the stairs. 

## Installation
Copy the relevant .h files to the ESPHome configuration folder next to your YAML file.
A subfolder should also work, change the includes accordingly.

## Smooth wipe
```yaml
esphome:
  includes:
    - effect_smoothwipe.h

light:
  - platform: ... # light component of choice, eg neopixelbus or fastled_clockless
    ...other settings here

    # To turn the effect off automatically at the end of the wipe, specify an id
    # and use it in the calls to id() below. This allows you to use the effect
    # again with a new color without having to reset it.
    id: ledstrip
    effects:
      - addressable_lambda:
          name: "Wipe up"
          update_interval: 12ms
          lambda: |-
            if (initial_run)
              # Second parameter is the transition length.
              # Third parameter the size of the leading edge. The larger, the smoother the transition.
              # Fourth parameter determines if the effect is applied in reverse.
              smoothWipeInit(it, 1000, 128, false);
            
            if (smoothWipe(it, current_color))
              // Automatically turn effect off so it can be called again
              id(ledstrip).turn_on().set_effect(0).perform();

      - addressable_lambda:
          name: "Wipe down"
          update_interval: 12ms
          lambda: |-
            if (initial_run)
              smoothWipeInit(it, 1000, 128, true);
            
            if (smoothWipe(it, current_color))
              // Automatically turn effect off so it can be called again
              id(ledstrip).turn_on().set_effect(0).perform();
```
