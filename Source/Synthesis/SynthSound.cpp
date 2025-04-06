#include "SynthSound.h"

SynthSound::SynthSound() {}

bool SynthSound::appliesToNote(int) {
    return true;
}

bool SynthSound::appliesToChannel(int) {
    return true;
}
