#include "../../include/station_simulator.h"

static const int PHASE_CONFIRM_TICKS = 1;
static const int PHASE_PACK_TICKS = 1;
static const int PHASE_SHIP_TICKS = 1;

static const char* getPhaseName(int phase) {
    if (phase == STATION_PHASE_CONFIRM) return "XAC NHAN";
    if (phase == STATION_PHASE_PACK) return "DONG GOI";
    if (phase == STATION_PHASE_SHIP) return "VAN CHUYEN";
    return "IDLE";
}

static int getQueueSize(const PriorityQueue& queue) {
    int count = 0;
    for (OrderNode* node = queue.head; node != NULL; node = node->next) {
        count++;
    }
    return count;
}

static bool dequeuePriorityOrder(PriorityQueue& queue, Order* out_order) {
    if (queue.head == NULL || out_order == NULL) {
        return false;
    }

    OrderNode* node = queue.head;
    queue.head = node->next;
    if (queue.head == NULL) {
        queue.tail = NULL;
    }

    *out_order = node->info;
    delete node;
    return true;
}

static bool hasHigherPriorityWaiting(const PriorityQueue& queue, const Order* current_order) {
    if (current_order == NULL || queue.head == NULL) {
        return false;
    }

    int queued_priority = getOrderPriorityRank(&queue.head->info);
    int current_priority = getOrderPriorityRank(current_order);
    return queued_priority < current_priority;
}

static void startOrderAtStation(Station* station, const Order* order) {
    if (station == NULL || order == NULL) return;

    station->currentOrder = new Order(*order);
    strcpy(station->currentOrder->status, "Dang xac nhan");
    station->currentPhase = STATION_PHASE_CONFIRM;
    station->ticksRemaining = PHASE_CONFIRM_TICKS;
}

static void transitionToNextPhase(Station* station) {
    if (station == NULL || station->currentOrder == NULL) return;

    if (station->currentPhase == STATION_PHASE_CONFIRM) {
        station->currentPhase = STATION_PHASE_PACK;
        station->ticksRemaining = PHASE_PACK_TICKS;
        strcpy(station->currentOrder->status, "Dang dong goi");
        return;
    }

    if (station->currentPhase == STATION_PHASE_PACK) {
        station->currentPhase = STATION_PHASE_SHIP;
        station->ticksRemaining = PHASE_SHIP_TICKS;
        strcpy(station->currentOrder->status, "Dang van chuyen");
        return;
    }

    strcpy(station->currentOrder->status, "Da giao cho DVVC");
    printf("\n\t\t\t\t\t\t[Tram %d] Hoan tat don %d (%s) - Doanh thu +%lld",
        station->id,
        station->currentOrder->id,
        station->currentOrder->customer_name,
        station->currentOrder->price * (long long)station->currentOrder->quantity);

    processCompletedOrder(station->currentOrder);

    delete station->currentOrder;
    station->currentOrder = NULL;
    station->currentPhase = STATION_PHASE_IDLE;
    station->ticksRemaining = 0;
}

void initStation(Station* station, int id) {
    if (station == NULL) return;

    station->id = id;
    station->currentOrder = NULL;
    station->currentPhase = STATION_PHASE_IDLE;
    station->ticksRemaining = 0;
}

bool hasActiveStations(const Station stations[], int n) {
    for (int i = 0; i < n; i++) {
        if (stations[i].currentOrder != NULL) {
            return true;
        }
    }
    return false;
}

void runOneTick(Station stations[], int n, PriorityQueue& queue) {
    static int tick_counter = 0;
    tick_counter++;

    printf("\n\n\t\t\t\t\t\t===== TICK %d =====", tick_counter);

    for (int i = 0; i < n; i++) {
        Station* station = &stations[i];
        int was_preempted = 0;

        if (station->isIdle()) {
            Order next_order;
            if (dequeuePriorityOrder(queue, &next_order)) {
                startOrderAtStation(station, &next_order);
                printf("\n\t\t\t\t\t\t[Tram %d] Nhan don %d - vao phase %s",
                    station->id,
                    station->currentOrder->id,
                    getPhaseName(station->currentPhase));
            }
            else {
                printf("\n\t\t\t\t\t\t[Tram %d] Idle (khong co don cho)", station->id);
            }
        }

        if (!station->isIdle() &&
            station->currentPhase == STATION_PHASE_CONFIRM &&
            hasHigherPriorityWaiting(queue, station->currentOrder)) {

            Order preempted_order = *station->currentOrder;
            strcpy(preempted_order.status, "Cho dong goi");
            enqueueOrder(&queue, preempted_order);

            Order higher_priority_order;
            if (dequeuePriorityOrder(queue, &higher_priority_order)) {
                delete station->currentOrder;
                station->currentOrder = NULL;
                startOrderAtStation(station, &higher_priority_order);
                was_preempted = 1;

                printf("\n\t\t\t\t\t\t[Tram %d] PREEMPT don %d -> don %d",
                    station->id,
                    preempted_order.id,
                    station->currentOrder->id);
            }
        }

        if (station->isIdle()) {
            printf("\n\t\t\t\t\t\t[Tram %d] Queue con lai: %d", station->id, getQueueSize(queue));
            continue;
        }

        station->ticksRemaining--;
        printf("\n\t\t\t\t\t\t[Tram %d] Dang %s - Don %d - ticks con lai phase: %d",
            station->id,
            getPhaseName(station->currentPhase),
            station->currentOrder->id,
            station->ticksRemaining);

        if (station->ticksRemaining <= 0) {
            transitionToNextPhase(station);
            if (!station->isIdle()) {
                printf("\n\t\t\t\t\t\t[Tram %d] Chuyen sang phase %s",
                    station->id,
                    getPhaseName(station->currentPhase));
            }
        }

        printf("\n\t\t\t\t\t\t[Tram %d] Preempt: %s | Queue con lai: %d",
            station->id,
            was_preempted ? "CO" : "KHONG",
            getQueueSize(queue));
    }
}
