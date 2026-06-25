# Guitar Pedal Rack - Effects Reference Manual

**Complete DSP Effects Documentation**  
**Version:** 1.0.0  
**Date:** June 22, 2026

---

## Table of Contents

1. [Dynamics](#dynamics)
   - [Noise Gate](#1-noise-gate)
   - [Compressor](#2-compressor)
2. [Drive/Distortion](#drivedistortion)
   - [Tube Overdrive](#3-tube-overdrive)
   - [Distortion](#4-distortion)
3. [Amp Simulation](#amp-simulation)
   - [Amp Simulator](#5-amp-simulator)
4. [Cabinet](#cabinet)
   - [Cabinet IR](#6-cabinet-ir)
5. [Modulation](#modulation)
   - [Chorus](#7-chorus)
6. [Time-Based Effects](#time-based-effects)
   - [Delay](#8-delay)
   - [Reverb](#9-reverb)
7. [Utility](#utility)
   - [EQ](#10-eq)

---

## Dynamics

### 1. Noise Gate

**Purpose:** Eliminates unwanted noise and hum when you're not playing

**Algorithm:** Threshold-based gate with hysteresis to prevent chattering

**Parameters:**

- **Threshold:** -80 dB to 0 dB
  - Controls when the gate opens
  - Lower = more aggressive gating
  - Typical: -50 dB to -40 dB
- **Ratio:** 1:1 to 20:1
  - Amount of gain reduction
  - Higher = harder gate
  - Typical: 10:1
- **Attack:** 0.1 ms to 100 ms
  - How fast the gate opens
  - Too fast = clicks
  - Typical: 1 ms
- **Release:** 10 ms to 1000 ms
  - How fast the gate closes
  - Too fast = choppy
  - Typical: 50-100 ms

**Use Cases:**

- High-gain tones (eliminate amp hiss between notes)
- Recording (clean up silent passages)
- Live performance (prevent feedback)

**Tips:**

- Set threshold just above noise floor
- Use longer release for sustained notes
- Use shorter attack for fast picking

---

### 2. Compressor

**Purpose:** Controls dynamics, adds sustain, evens out volume

**Algorithm:** RMS/Peak detection with VCA or Optical modes

**Parameters:**

- **Mode:** VCA or Optical
  - **VCA:** Fast, transparent, studio-style
  - **Optical:** Slower, warmer, vintage-style
- **Threshold:** -60 dB to 0 dB
  - Level where compression starts
  - Lower = more compression
  - Typical: -20 dB to -10 dB
- **Ratio:** 1:1 to 10:1
  - Amount of compression
  - Higher = more squashed
  - Typical: 3:1 to 4:1
- **Attack:** 0.1 ms to 100 ms
  - How fast compressor responds
  - Fast = tighter control
  - Slow = preserves transients
  - Typical: 5-10 ms
- **Release:** 10 ms to 1000 ms
  - How fast compression releases
  - Fast = pumping effect
  - Slow = smooth
  - Typical: 100-200 ms
- **Makeup Gain:** 0 dB to +30 dB
  - Compensates for volume loss
  - Adjust to match bypassed level

- **Soft Knee:** On/Off
  - Smooth compression curve
  - More transparent

- **Detection:** RMS or Peak
  - **RMS:** Average level, smoother
  - **Peak:** Transient control, faster

**Use Cases:**

- Chicken pickin' (country, fast attack/release)
- Sustain (lead guitar, high ratio)
- Clean rhythm (transparent, low ratio)
- Funk (adds punch and tightness)

**Tips:**

- Start with 3:1 ratio, adjust threshold
- Use VCA for clean tones
- Use Optical for warm, vintage tones
- Don't over-compress (aim for 3-6 dB reduction)

---

## Drive/Distortion

### 3. Tube Overdrive

**Purpose:** Warm, mid-focused overdrive (Tube Screamer style)

**Algorithm:** Asymmetric soft clipping with mid-frequency boost, 4x oversampling

**Parameters:**

- **Drive:** 0.0 to 1.0
  - Amount of gain/distortion
  - Low = clean boost
  - High = saturated overdrive
  - Gain range: 1x to 30x
- **Tone:** 0.0 to 1.0
  - Controls mid-frequency emphasis
  - 0.0 = darker, scooped
  - 1.0 = brighter, more mid-boost
  - Typical: 0.4-0.7 (classic TS sound)
- **Level:** 0.0 to 1.0
  - Output volume
  - Compensates for drive gain
  - Typical: 0.7-0.9

**Character:**

- Asymmetric clipping (warm, tube-like)
- Mid-frequency boost (cuts through mix)
- Smooth compression
- Slightly compressed high-end

**Use Cases:**

- Blues solos (drive 0.4-0.6)
- Classic rock rhythm (drive 0.5-0.7)
- Boost into amp (drive 0.2-0.4, level 0.9)
- Lead tones (drive 0.7-0.9)

**Tips:**

- Stack with amp distortion for thick lead tones
- Use as clean boost (drive 0.1, level 0.9)
- Keep tone at 0.5 for classic sound
- Great before high-gain amps

---

### 4. Distortion

**Purpose:** Aggressive, hard clipping distortion (DS-1/RAT style)

**Algorithm:** Hard clipping with pre-emphasis, resonant filter, 4x oversampling

**Parameters:**

- **Drive:** 0.0 to 1.0
  - Amount of gain/distortion
  - Exponential scaling: 1x to 100x
  - 0.0 = clean
  - 0.5 = moderate crunch
  - 1.0 = extreme saturation
- **Tone:** 0.0 to 1.0
  - Filter frequency sweep
  - 0.0 = dark, muffled
  - 0.5 = balanced
  - 1.0 = bright, cutting
- **Level:** 0.0 to 1.0
  - Output volume
  - Compensates for drive
  - Typical: 0.6-0.8
- **Filter:** 0.0 to 1.0
  - Resonant low-pass filter
  - Adds character and sculpting
  - Higher = more resonance
  - Typical: 0.5

**Character:**

- Hard clipping (aggressive, edgy)
- Pre-emphasis (boosts highs before clipping)
- Resonant filter (tonal shaping)
- More harmonics than overdrive

**Use Cases:**

- Heavy metal rhythm (drive 0.7-0.9, tone 0.6)
- Punk rock (drive 0.8-1.0, tone 0.5-0.7)
- Garage rock (drive 0.6-0.8, tone 0.4)
- Aggressive leads (drive 0.9, tone 0.7)

**Tips:**

- Use higher drive for crushing rhythm tones
- Adjust filter for unique character
- Stack with noise gate to clean up
- Great for drop-tuned guitars

---

## Amp Simulation

### 5. Amp Simulator

**Purpose:** Realistic guitar amplifier simulation

**Algorithm:** Cascaded gain stages, reactive tone stack, power amp saturation

**Parameters:**

- **Model:** Clean, Crunch, or High Gain
  - **Clean (Fender-style):**
    - Class A/B simulation
    - Bright, chimey character
    - Touch-sensitive dynamics
    - Low gain, transparent
  - **Crunch (Marshall-style):**
    - British tone stack
    - Mid-focused voicing
    - Power amp compression
    - Moderate gain
  - **High Gain (Mesa-style):**
    - Multiple gain stages
    - Tight low-end
    - Scooped mids
    - High saturation

- **Gain:** 0.0 to 10.0
  - Preamp gain
  - Higher = more distortion
  - Clean: 1-3
  - Crunch: 4-6
  - High Gain: 7-10
- **Bass:** 0.0 to 10.0
  - Low-frequency control
  - Typical: 5-7
- **Mid:** 0.0 to 10.0
  - Midrange control
  - Typical: 4-6 (scooped), 6-8 (present)
- **Treble:** 0.0 to 10.0
  - High-frequency control
  - Typical: 5-7
- **Presence:** 0.0 to 10.0
  - Upper-midrange/high control
  - Adds bite and clarity
  - Typical: 4-6

**Use Cases:**

- **Clean Model:**
  - Jazz (gain 2, mid 7, bright tone)
  - Country (gain 3, treble 8)
  - Funk rhythm (gain 2-3, scooped mids)
- **Crunch Model:**
  - Classic rock (gain 5, mid 6)
  - Blues rock (gain 4-5, treble 6)
  - AC/DC-style rhythm (gain 6)
- **High Gain Model:**
  - Metal rhythm (gain 8-9, tight bass)
  - Modern rock leads (gain 7-8)
  - Djent (gain 9-10, scooped)

**Tips:**

- Start with all EQ at 5 (noon)
- Adjust bass first, then mid, then treble
- Use presence for cut in the mix
- Lower gain sounds more realistic

---

## Cabinet

### 6. Cabinet IR

**Purpose:** Realistic speaker cabinet simulation via impulse response

**Algorithm:** Zero-latency convolution

**Parameters:**

- **IR File:** Built-in or custom WAV file
  - Default: High-quality 4x12 cabinet
  - Load custom IRs: 44.1kHz-96kHz, mono/stereo
- **Mic Type:** Dynamic, Condenser, or Ribbon
  - **Dynamic (SM57-style):**
    - Focused, mid-present
    - Great for rock/metal
  - **Condenser (U87-style):**
    - Detailed, full-range
    - Great for recording
  - **Ribbon (R121-style):**
    - Smooth, dark
    - Great for warm tones
- **Mic Position:** 0.0 to 1.0
  - 0.0 = on-axis (bright, direct)
  - 0.5 = slightly off-axis (balanced)
  - 1.0 = off-axis (darker, more room)
- **Room:** 0.0 to 1.0
  - Amount of room ambience
  - 0.0 = close-mic'd
  - 1.0 = room sound

**Use Cases:**

- Direct recording (no physical cab needed)
- Silent practice (headphones)
- Consistent tone (no mic placement issues)
- Multiple cab options

**Tips:**

- Start with dynamic mic, on-axis
- Experiment with off-axis for warmth
- Use room sparingly (0.2-0.4)
- Load custom IRs for specific tones

---

## Modulation

### 7. Chorus

**Purpose:** Adds depth, width, and shimmer to your tone

**Algorithm:** BBD analog-style pitch modulation with multiple voices

**Parameters:**

- **Rate:** 0.0 to 1.0
  - LFO speed: 0.1 Hz to 5 Hz
  - Low = slow swirl
  - High = fast warble
  - Typical: 0.3-0.5 (1-2 Hz)
- **Depth:** 0.0 to 1.0
  - Modulation amount: 0 to 10 ms
  - Low = subtle doubling
  - High = pronounced chorus
  - Typical: 0.5-0.7
- **Mix:** 0.0 to 1.0
  - Dry/wet balance
  - 0.0 = all dry
  - 1.0 = all wet
  - Typical: 0.3-0.5
- **Voices:** 1 to 4
  - Number of chorus voices
  - More voices = richer, denser
  - 1 = vibrato
  - 2 = classic chorus
  - 4 = lush, thick

**Character:**

- BBD-style analog warmth
- Phase-spread LFOs (voices don't align)
- Linear interpolation (smooth modulation)

**Use Cases:**

- Clean arpeggios (rate 0.4, depth 0.6, 2 voices)
- 80s leads (rate 0.3, depth 0.8, 3 voices)
- Ambient pads (rate 0.2, depth 0.9, 4 voices)
- Thickening (rate 0.5, depth 0.3, 2 voices)

**Tips:**

- Lower depth for subtle doubling
- Higher voices for lush, synthetic sound
- Combine with reverb for ambient tones
- Use vibrato (1 voice, high depth)

---

## Time-Based Effects

### 8. Delay

**Purpose:** Echoes and rhythmic repeats

**Algorithm:** Multi-mode delay with character modeling

**Parameters:**

- **Mode:** Digital, Analog, or Tape
  - **Digital:**
    - Clean, pristine repeats
    - No degradation
    - Great for rhythmic patterns
  - **Analog:**
    - BBD character
    - Subtle modulation
    - Warmer, darker repeats
  - **Tape:**
    - Wow & flutter (pitch instability)
    - Saturation
    - Vintage, lo-fi character

- **Time:** 0.0 to 1.0
  - Delay time: 10 ms to 2000 ms
  - 0.0-0.05 = slap (10-100ms)
  - 0.1-0.3 = eighth notes (200-600ms)
  - 0.5+ = long echoes (1000ms+)
- **Feedback:** 0.0 to 1.0
  - Amount of regeneration
  - 0.0 = single repeat
  - 0.5 = moderate repeats
  - 0.95 = maximum (prevents runaway)
- **Mix:** 0.0 to 1.0
  - Dry/wet balance
  - 0.0 = all dry
  - 1.0 = all wet
  - Typical: 0.3-0.5
- **Damping:** 0.0 to 1.0
  - Low-pass filter on repeats
  - Low = bright repeats
  - High = dark, warm repeats
  - Typical: 0.7
- **Ping-Pong:** On/Off
  - Stereo alternating delays
  - Creates wide soundstage

**Use Cases:**

- **Slapback** (rockabilly):
  - Time: 0.05-0.15 (50-150ms)
  - Feedback: 0.1
  - Mode: Tape
- **Dotted 8th** (U2-style):
  - Time: 0.3 (adjust to tempo)
  - Feedback: 0.6
  - Mode: Digital
  - Ping-pong: On
- **Ambient** (soundscapes):
  - Time: 0.8-1.0
  - Feedback: 0.8
  - Damping: 0.9
  - Mode: Tape
- **Rhythmic** (modern rock):
  - Time: 0.25 (quarter note)
  - Feedback: 0.4
  - Mode: Analog

**Tips:**

- Sync delay time to song tempo
- Use damping for warmer, less harsh repeats
- Tape mode for vintage vibe
- Ping-pong for wide stereo image

---

### 9. Reverb

**Purpose:** Adds space, depth, and ambience

**Algorithm:** JUCE high-quality reverb with mode-specific character

**Parameters:**

- **Mode:** Spring, Plate, Hall, or Shimmer
  - **Spring:**
    - Short decay, bright
    - Metallic character
    - Vintage amp-style
  - **Plate:**
    - Dense, bright
    - Smooth, musical
    - Studio-quality
  - **Hall:**
    - Large, spacious
    - Natural, long decay
    - Concert hall simulation
  - **Shimmer:**
    - Octave-up feedback
    - Ethereal, ambient
    - Large space

- **Size:** 0.0 to 1.0
  - Room size
  - Low = small room
  - High = large hall
  - Typical: 0.4-0.6
- **Damping:** 0.0 to 1.0
  - High-frequency absorption
  - Low = bright reverb
  - High = dark, warm reverb
  - Typical: 0.5-0.7
- **Mix:** 0.0 to 1.0
  - Dry/wet balance
  - Typical: 0.2-0.4
  - Ambient: 0.6-0.8
- **Width:** 0.0 to 1.0
  - Stereo spread
  - Low = narrow
  - High = wide stereo
  - Typical: 0.8-1.0

**Use Cases:**

- **Spring** (surf rock):
  - Size: 0.3
  - Damping: 0.3
  - Mix: 0.3
- **Plate** (leads):
  - Size: 0.5
  - Damping: 0.4
  - Mix: 0.3
- **Hall** (ambient):
  - Size: 0.8
  - Damping: 0.6
  - Mix: 0.5
- **Shimmer** (pads):
  - Size: 0.9
  - Damping: 0.3
  - Mix: 0.7

**Tips:**

- Less is more (don't over-reverb)
- Use damping to control brightness
- Hall for solos, spring for rhythm
- Shimmer for ambient soundscapes

---

## Utility

### 10. EQ

**Purpose:** Surgical tone shaping and problem fixing

**Algorithm:** 4-band parametric EQ + high-pass/low-pass filters

**Parameters:**

#### **Band 1 (Low - Default 100Hz):**

- **Frequency:** 20 Hz to 20 kHz
- **Q:** 0.1 to 10.0
- **Gain:** -15 dB to +15 dB
- **Use:** Control low-end thump, remove mud

#### **Band 2 (Low-Mid - Default 500Hz):**

- **Frequency:** 20 Hz to 20 kHz
- **Q:** 0.1 to 10.0
- **Gain:** -15 dB to +15 dB
- **Use:** Control warmth, body, remove muddiness

#### **Band 3 (High-Mid - Default 2kHz):**

- **Frequency:** 20 Hz to 20 kHz
- **Q:** 0.1 to 10.0
- **Gain:** -15 dB to +15 dB
- **Use:** Control presence, bite, reduce harshness

#### **Band 4 (High - Default 8kHz):**

- **Frequency:** 20 Hz to 20 kHz
- **Q:** 0.1 to 10.0
- **Gain:** -15 dB to +15 dB
- **Use:** Control air, sparkle, reduce sizzle

#### **High-Pass Filter:**

- **Frequency:** 20 Hz to 1 kHz
- **Type:** 2nd-order Butterworth
- **Use:** Remove subsonic rumble, tighten low-end

#### **Low-Pass Filter:**

- **Frequency:** 1 kHz to 20 kHz
- **Type:** 2nd-order Butterworth
- **Use:** Remove harsh highs, warm tone

**Use Cases:**

- **Remove Mud:**
  - Band 2: -4 dB at 400Hz, Q 1.5
- **Add Presence:**
  - Band 3: +3 dB at 2.5kHz, Q 2.0
- **Tighten Low-End:**
  - HP Filter: 80 Hz
  - Band 1: -2 dB at 150Hz, Q 1.0
- **Reduce Harshness:**
  - Band 3: -3 dB at 3kHz, Q 3.0
- **Add Air:**
  - Band 4: +2 dB at 10kHz, Q 1.0

**Tips:**

- Start with all bands at 0 dB
- Cut before boosting
- Use narrow Q (high value) for problem fixing
- Use wide Q (low value) for tonal shaping
- HP filter at 80Hz for tight, modern tones

---

## Signal Chain Recommendations

### **Classic Rock Rhythm:**

```
Noise Gate → Compressor (VCA) → Tube Overdrive → Crunch Amp → Cabinet (Dynamic, on-axis) → Spring Reverb
```

### **Metal Rhythm:**

```
Noise Gate → Distortion → High Gain Amp → Cabinet (Dynamic) → EQ (scoop 500Hz) → Delay (short)
```

### **Clean Ambient:**

```
Compressor (Optical) → Chorus (4 voices) → Clean Amp → Cabinet (Condenser) → Delay (Tape, long) → Hall Reverb
```

### **Lead Solo:**

```
Tube Overdrive → Crunch Amp → Cabinet → EQ (presence boost) → Delay (Digital, dotted 8th) → Plate Reverb
```

### **Blues:**

```
Compressor → Tube Overdrive (light) → Clean Amp → Cabinet (Dynamic, off-axis) → Delay (Analog, slap) → Spring Reverb
```

---

**This completes the reference manual for all 10 effects!** 🎸

**Document Created:** June 22, 2026  
**Total Effects:** 10  
**Pages:** Professional Reference Guide
