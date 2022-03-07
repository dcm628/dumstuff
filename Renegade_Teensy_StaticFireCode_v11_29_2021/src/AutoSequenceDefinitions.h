#ifndef AUTOSEQUENCEDEFINITIONS_H
#define AUTOSEQUENCEDEFINITIONS_H

#include "AutoSequence.h"
#include <array>

// Define the number of autosequences here
#define NUM_AUTOSEQUENCES 1


AutoSequence IgnitionAutoSequence(15000, 2); // X seconds in micros to set the countdownStart

std::array<AutoSequence*, NUM_AUTOSEQUENCES> autoSequenceArray{&IgnitionAutoSequence};


#endif