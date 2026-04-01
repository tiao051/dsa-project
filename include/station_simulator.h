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
    Order* currentOrder;
    int currentPhase;
    int ticksRemaining;

    bool isIdle() const {
        return currentOrder == NULL;
    }
} Station;

void initStation(Station* station, int id);
bool hasActiveStations(const Station stations[], int n);
void runOneTick(Station stations[], int n, PriorityQueue& queue);

#endif
