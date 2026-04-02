#ifndef STATION_SIMULATOR_H
#define STATION_SIMULATOR_H

#include "order_manager.h"

enum StationPhase {
    STATION_PHASE_IDLE = 0,
    STATION_PHASE_CONFIRM = 1,
    STATION_PHASE_PACK = 2,
    STATION_PHASE_SHIP = 3
};

typedef OrderQueue PriorityQueue;

typedef struct Station {
    int id;
    // currentOrder == NULL means station is available for dispatch in this tick.
    Order* currentOrder;
    // currentPhase drives preemption rules (only CONFIRM can be interrupted).
    int currentPhase;
    // Remaining seconds in currentPhase; decremented once per tick.
    int ticksRemaining;

    bool isIdle() const {
        return currentOrder == NULL;
    }
} Station;

void initStation(Station* station, int id);
bool hasActiveStations(const Station stations[], int n);
void resetStationTickCounter();
void runOneTick(Station stations[], int n, PriorityQueue& queue);
void runPackagingScenarioFromFile(OrderQueue* queue, const char* scenario_file);

#endif
