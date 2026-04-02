#include "../../include/station_simulator.h"

// Phase durations are modeled in seconds to make logs/demo timing intuitive.
static const int PHASE_CONFIRM_TICKS = 10;
static const int PHASE_PACK_TICKS = 10;
static const int PHASE_SHIP_TICKS = 5;
// Keep 1 second per tick so phase timing maps directly to real-time expectation.
static const int TICK_DURATION_MS = 1000;
// Scenario mode injects orders in small batches to make preemption moments visible.
static const int SCENARIO_BATCH_SIZE = 2;
static int g_station_tick_counter = 0;

typedef struct ScheduledOrderNode {
    // arrival_slot preserves deterministic order when timestamps are equal.
    int arrival_slot;
    char created_at[40];
    Order order;
    struct ScheduledOrderNode* next;
} ScheduledOrderNode;

static const char* getPhaseName(int phase) {
    if (phase == STATION_PHASE_CONFIRM) return "XAC NHAN";
    if (phase == STATION_PHASE_PACK) return "DONG GOI";
    if (phase == STATION_PHASE_SHIP) return "VAN CHUYEN";
    return "RANH";
}

static int getQueueSize(const PriorityQueue& queue) {
    int count = 0;
    for (OrderNode* node = queue.head; node != NULL; node = node->next) {
        count++;
    }
    return count;
}

static const char* getPriorityLabelByRank(int rank) {
    if (rank == 1) return "HOA TOC";
    if (rank == 2) return "VIP";
    return "TIEU CHUAN";
}

static bool dequeuePriorityOrder(PriorityQueue& queue, Order* out_order) {
    if (queue.head == NULL || out_order == NULL) {
        return false;
    }

    // Queue head is always the current highest-priority candidate.
    OrderNode* node = queue.head;
    queue.head = node->next;
    if (queue.head == NULL) {
        queue.tail = NULL;
    }

    *out_order = node->info;
    delete node;
    return true;
}

static void startOrderAtStation(Station* station, const Order* order) {
    if (station == NULL || order == NULL) return;

    // Copy order data into station-owned memory so queue operations remain independent.
    station->currentOrder = new Order(*order);
    strcpy(station->currentOrder->status, "Dang xac nhan");
    station->currentPhase = STATION_PHASE_CONFIRM;
    station->ticksRemaining = PHASE_CONFIRM_TICKS;
}

static void transitionToNextPhase(Station* station) {
    if (station == NULL || station->currentOrder == NULL) return;

    if (station->currentPhase == STATION_PHASE_CONFIRM) {
        // CONFIRM finished: move to PACK and lock out preemption from now on.
        station->currentPhase = STATION_PHASE_PACK;
        station->ticksRemaining = PHASE_PACK_TICKS;
        strcpy(station->currentOrder->status, "Dang dong goi");
        return;
    }

    if (station->currentPhase == STATION_PHASE_PACK) {
        // PACK finished: move to SHIP.
        station->currentPhase = STATION_PHASE_SHIP;
        station->ticksRemaining = PHASE_SHIP_TICKS;
        strcpy(station->currentOrder->status, "Dang van chuyen");
        return;
    }

    // SHIP finished: order is complete and should leave active station set.
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

static void clearPendingQueue(PriorityQueue& queue) {
    Order ignored;
    // Scenario execution should be deterministic; clear leftover runtime queue first.
    while (dequeuePriorityOrder(queue, &ignored)) {}
}

static bool assignOneIdleStation(Station stations[], int n, PriorityQueue& queue) {
    // Greedy dispatch is intentional: fill idle capacity immediately each tick
    // so waiting time is minimized before considering preemption.
    for (int i = 0; i < n; i++) {
        if (stations[i].isIdle()) {
            Order next_order;
            if (dequeuePriorityOrder(queue, &next_order)) {
                startOrderAtStation(&stations[i], &next_order);
                printf("\n\t\t\t\t\t\t[Phan cong] Tram %d nhan don %d (uu tien %d)",
                    stations[i].id,
                    stations[i].currentOrder->id,
                    getOrderPriorityRank(stations[i].currentOrder));

                printf(" - %s",
                    getPriorityLabelByRank(getOrderPriorityRank(stations[i].currentOrder)));
                return true;
            }
        }
    }
    return false;
}

static bool preemptOneStationIfNeeded(Station stations[], int n, PriorityQueue& queue, int preempt_flags[]) {
    if (queue.head == NULL) {
        return false;
    }

    // The waiting candidate is always queue.head because queue is kept priority-sorted.
    const Order* waiting_order = &queue.head->info;
    int waiting_priority = getOrderPriorityRank(waiting_order);

    int selected_index = -1;
    int selected_station_priority = -1;

    for (int i = 0; i < n; i++) {
        Station* station = &stations[i];
        if (station->currentOrder == NULL) {
            continue;
        }
        if (station->currentPhase != STATION_PHASE_CONFIRM) {
            // Phase 2 and 3 are locked and cannot be preempted.
            continue;
        }

        int station_priority = getOrderPriorityRank(station->currentOrder);
        if (waiting_priority >= station_priority) {
            continue;
        }

        // Why choose the lowest-priority CONFIRM-phase victim:
        // this maximizes fairness while still guaranteeing the highest benefit
        // from replacing a weak-priority job with a stronger one.
        if (selected_index == -1 || station_priority > selected_station_priority) {
            selected_index = i;
            selected_station_priority = station_priority;
        }
    }

    if (selected_index == -1) {
        return false;
    }

    Order promoted_order;
    if (!dequeuePriorityOrder(queue, &promoted_order)) {
        return false;
    }

    Station* victim_station = &stations[selected_index];
    // Return interrupted order back to queue so it can continue later.
    Order preempted_order = *victim_station->currentOrder;
    strcpy(preempted_order.status, "Cho dong goi");
    enqueueOrder(&queue, preempted_order);

    delete victim_station->currentOrder;
    victim_station->currentOrder = NULL;
    startOrderAtStation(victim_station, &promoted_order);
    preempt_flags[selected_index] = 1;

    printf("\n\t\t\t\t\t\t[CHEN UU TIEN] Tram %d: don %d (uu tien %d) -> don %d (uu tien %d)",
        victim_station->id,
        preempted_order.id,
        getOrderPriorityRank(&preempted_order),
        promoted_order.id,
        getOrderPriorityRank(&promoted_order));
    printf(" [%s -> %s]",
        getPriorityLabelByRank(getOrderPriorityRank(&preempted_order)),
        getPriorityLabelByRank(getOrderPriorityRank(&promoted_order)));

    return true;
}

void initStation(Station* station, int id) {
    if (station == NULL) return;

    station->id = id;
    station->currentOrder = NULL;
    station->currentPhase = STATION_PHASE_IDLE;
    station->ticksRemaining = 0;
}

void resetStationTickCounter() {
    g_station_tick_counter = 0;
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
    g_station_tick_counter++;

    printf("\n\n\t\t\t\t\t\t===== NHIP %d =====", g_station_tick_counter);

    int preempt_flags[PACKING_STATION_COUNT] = { 0 };

    // Step 1 (scheduling):
    // 1) dispatch to idle stations, 2) run global preemption pass.
    // This ordering avoids unnecessary preemption when free capacity still exists.
    while (assignOneIdleStation(stations, n, queue)) {}
    while (preemptOneStationIfNeeded(stations, n, queue, preempt_flags)) {}

    for (int i = 0; i < n; i++) {
        Station* station = &stations[i];

        if (station->isIdle()) {
            printf("\n\t\t\t\t\t\t[Tram %d] Queue con lai: %d", station->id, getQueueSize(queue));
            continue;
        }

        // Step 2 (execution): advance exactly one second per station for this tick.
        station->ticksRemaining--;
        printf("\n\t\t\t\t\t\t[Tram %d] Dang %s - Don %d - giay con lai phase: %d",
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

        printf("\n\t\t\t\t\t\t[Tram %d] Co chen uu tien: %s | Queue con lai: %d",
            station->id,
            preempt_flags[i] ? "CO" : "KHONG",
            getQueueSize(queue));
    }

    if (TICK_DURATION_MS > 0) {
        Sleep(TICK_DURATION_MS);
    }
}

static PriorityLevel mapPriorityLevelFromInput(int level) {
    if (level == 1) return PRIORITY_EXPRESS;
    if (level == 2) return PRIORITY_VIP;
    return PRIORITY_NORMAL;
}

static int getPriorityLevelForDisplay(const Order* order) {
    if (order == NULL) return 3;
    return getOrderPriorityRank(order);
}

static ScheduledOrderNode* createScheduledOrderNode(int arrival_slot, const char* created_at, const Order* order) {
    if (created_at == NULL || order == NULL) return NULL;

    ScheduledOrderNode* node = new ScheduledOrderNode();
    if (node == NULL) return NULL;

    node->arrival_slot = arrival_slot;
    strcpy(node->created_at, created_at);
    node->order = *order;
    node->next = NULL;
    return node;
}

static int compareScheduleKey(const ScheduledOrderNode* lhs, const ScheduledOrderNode* rhs) {
    // Scenario input is consumed by timestamp first; ties keep arrival slot, then order id.
    int ts_cmp = strcmp(lhs->created_at, rhs->created_at);
    if (ts_cmp != 0) {
        return ts_cmp;
    }
    if (lhs->arrival_slot != rhs->arrival_slot) {
        return lhs->arrival_slot - rhs->arrival_slot;
    }
    return lhs->order.id - rhs->order.id;
}

static void insertScheduledOrderByTimestamp(ScheduledOrderNode** head, ScheduledOrderNode* new_node) {
    if (head == NULL || new_node == NULL) return;

    if (*head == NULL || compareScheduleKey(new_node, *head) < 0) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    ScheduledOrderNode* cur = *head;
    while (cur->next != NULL && compareScheduleKey(cur->next, new_node) <= 0) {
        cur = cur->next;
    }

    new_node->next = cur->next;
    cur->next = new_node;
}

static void freeScheduledOrders(ScheduledOrderNode* head) {
    while (head != NULL) {
        ScheduledOrderNode* next = head->next;
        delete head;
        head = next;
    }
}

static int loadScenarioFile(const char* scenario_file, ScheduledOrderNode** out_head) {
    if (scenario_file == NULL || out_head == NULL) return 0;

    FILE* file_ptr = fopen(scenario_file, "rt");
    if (file_ptr == NULL) {
        setColor(4);
        printf("\n\t\t\t\t\t\tKhong mo duoc file kich ban: %s", scenario_file);
        setColor(7);
        return 0;
    }

    char line[512];
    int loaded = 0;
    int line_slot = 0;
    while (fgets(line, sizeof(line), file_ptr) != NULL) {
        trimString(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        int arrival_slot = 0;
        int order_id = 0;
        int quantity = 0;
        long long price = 0;
        int priority_level = 3;
        char created_at[40] = { 0 };
        char customer_name[100] = { 0 };
        char product_name[100] = { 0 };

        // Supported formats:
        // 1) arrival_slot,timestamp,id,customer,product,qty,price,priority
        // 2) timestamp,id,customer,product,qty,price,priority
        int parsed = sscanf(line, "%d,%39[^,],%d,%99[^,],%99[^,],%d,%lld,%d",
            &arrival_slot,
            created_at,
            &order_id,
            customer_name,
            product_name,
            &quantity,
            &price,
            &priority_level);

        if (parsed != 8) {
            parsed = sscanf(line, "%39[^,],%d,%99[^,],%99[^,],%d,%lld,%d",
                created_at,
                &order_id,
                customer_name,
                product_name,
                &quantity,
                &price,
                &priority_level);
            if (parsed == 7) {
                // Auto-assign arrival_slot when omitted, preserving file order.
                line_slot++;
                arrival_slot = line_slot;
            }
        }

        if ((parsed != 8 && parsed != 7) || quantity <= 0) {
            continue;
        }

        trimString(created_at);
        trimString(customer_name);
        trimString(product_name);

        Order order;
        order.id = order_id;
        strcpy(order.customer_name, customer_name);
        strcpy(order.product_name, product_name);
        order.quantity = quantity;
        order.price = price;
        order.priority = mapPriorityLevelFromInput(priority_level);
        order.shipping_method = (priority_level == 1) ? SHIPPING_EXPRESS : SHIPPING_STANDARD;
        strcpy(order.status, "Cho dong goi");

        ScheduledOrderNode* node = createScheduledOrderNode(arrival_slot, created_at, &order);
        if (node == NULL) {
            continue;
        }

        insertScheduledOrderByTimestamp(out_head, node);
        loaded++;
    }

    fclose(file_ptr);
    return loaded;
}

void runPackagingScenarioFromFile(OrderQueue* queue, const char* scenario_file) {
    if (queue == NULL) return;

    int station_count = PACKING_STATION_COUNT;
    if (station_count <= 0) {
        showErrorMessage("[!] Cau hinh PACKING_STATION_COUNT khong hop le!");
        return;
    }

    ScheduledOrderNode* scheduled_head = NULL;
    int loaded = loadScenarioFile(scenario_file, &scheduled_head);
    if (loaded <= 0) {
        showErrorMessage("[!] Kich ban rong hoac sai dinh dang.");
        return;
    }

    setColor(11);
    printf("\n\t\t\t\t\t\tChay kich ban tu file: %s", scenario_file);
    printf("\n\t\t\t\t\t\tDa nap %d don, sap theo thoi gian tang dan", loaded);
    printf("\n\t\t\t\t\t\tMoi nhip dua toi da %d don vao he thong", SCENARIO_BATCH_SIZE);
    setColor(7);

    Station stations[PACKING_STATION_COUNT];
    for (int i = 0; i < station_count; i++) {
        initStation(&stations[i], i + 1);
    }

    resetCompletedOrderHistory();
    resetStationTickCounter();
    // Scenario mode owns the queue lifecycle for deterministic demos.
    clearPendingQueue(*queue);

    while (scheduled_head != NULL || !isOrderQueueEmpty(queue) || hasActiveStations(stations, station_count)) {
        int arrival_count = 0;
        // Feed a bounded number of new orders each tick to emulate a live stream
        // and keep state transitions readable during demos.
        while (scheduled_head != NULL && arrival_count < SCENARIO_BATCH_SIZE) {
            enqueueOrder(queue, scheduled_head->order);
            printf("\n\t\t\t\t\t\t[Don moi] TG %s | Don %d | Uu tien %d (%s) | %s",
                scheduled_head->created_at,
                scheduled_head->order.id,
                getPriorityLevelForDisplay(&scheduled_head->order),
                getPriorityLabelByRank(getPriorityLevelForDisplay(&scheduled_head->order)),
                scheduled_head->order.customer_name);

            ScheduledOrderNode* old_head = scheduled_head;
            scheduled_head = scheduled_head->next;
            delete old_head;
            arrival_count++;
        }

        if (arrival_count == 0) {
            printf("\n\t\t\t\t\t\t[Don moi] Khong co don moi");
        }

        runOneTick(stations, station_count, *queue);
    }

    saveOrderQueueWithHistory("data/orders.txt", queue);
    freeScheduledOrders(scheduled_head);

    setColor(2);
    printf("\n\n\t\t\t\t\t\tHOAN TAT CHAY KICH BAN UU TIEN!");
    setColor(7);
}
