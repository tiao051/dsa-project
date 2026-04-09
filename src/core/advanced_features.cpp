#include "../../include/order_manager.h"

#include <chrono>

static Order makeBenchmarkOrder(int id, int priority) {
	Order order;
	order.id = id;
	strcpy(order.customer_name, "BenchCustomer");
	strcpy(order.product_name, "BenchProduct");
	order.quantity = 1;
	order.price = 100000;
	order.priority = (PriorityLevel)priority;
	order.shipping_method = (order.priority == PRIORITY_EXPRESS) ? SHIPPING_EXPRESS : SHIPPING_STANDARD;
	strcpy(order.status, "Cho dong goi");
	return order;
}

static int dequeueLinkedForBenchmark(OrderQueue* q, Order* out_order) {
	if (q == NULL || q->head == NULL) return 0;

	OrderNode* node = q->head;
	if (out_order != NULL) {
		*out_order = node->info;
	}

	q->head = node->next;
	if (q->head == NULL) q->tail = NULL;
	delete node;
	return 1;
}

static void clearLinkedForBenchmark(OrderQueue* q) {
	Order temp;
	while (dequeueLinkedForBenchmark(q, &temp)) {}
}

static double runArrayBenchmarkMs(int order_count) {
	Order* arr = new Order[order_count + 10];
	int size = 0;
	int id = 1000;
	Order temp;

	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < order_count; i++) {
		Order order = makeBenchmarkOrder(id++, PRIORITY_NORMAL);
		int pos = size;
		while (pos > 0 && (int)arr[pos - 1].priority > (int)order.priority) {
			arr[pos] = arr[pos - 1];
			pos--;
		}
		arr[pos] = order;
		size++;
	}
	for (int i = 0; i < order_count; i++) {
		if (size > 0) {
			temp = arr[0];
			for (int j = 1; j < size; j++) {
				arr[j - 1] = arr[j];
			}
			size--;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	delete[] arr;
	return std::chrono::duration<double, std::milli>(end - start).count();
}

static double runLinkedBenchmarkMs(int order_count) {
	OrderQueue q;
	initOrderQueue(&q);
	int id = 1000;
	Order temp;

	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < order_count; i++) {
		Order order = makeBenchmarkOrder(id++, PRIORITY_NORMAL);
		enqueueOrder(&q, order);
	}

	for (int i = 0; i < order_count; i++) {
		dequeueLinkedForBenchmark(&q, &temp);
	}

	auto end = std::chrono::high_resolution_clock::now();
	clearLinkedForBenchmark(&q);
	return std::chrono::duration<double, std::milli>(end - start).count();
}

void runQueueBenchmarkComparison() {
	const int order_count = 100000;

	setColor(11);
	printf("\n\t\t\t\t\t\t=== BENCHMARK QUEUE: MANG vs DSLK ===");
	setColor(7);
	printf("\n\t\t\t\t\t\tKich ban mac dinh: 1,000,000 dat hang + 1,000,000 xu ly");
	printf("\n\t\t\t\t\t\tMuc uu tien: toan bo la Tieu chuan");
	double arr_ms = runArrayBenchmarkMs(order_count);
	double list_ms = runLinkedBenchmarkMs(order_count);

	double list_speedup = (list_ms > 0.0) ? arr_ms / list_ms : 0.0;
	double arr_speedup = (arr_ms > 0.0) ? list_ms / arr_ms : 0.0;

	printf("\n\n\t\t\t\t\t\t[Ket qua thoi gian]");
	printf("\n\t\t\t\t\t\t- Queue MANG : %.2f ms", arr_ms);
	printf("\n\t\t\t\t\t\t- Queue DSLK : %.2f ms", list_ms);

	if (arr_ms > list_ms) {
		printf("\n\t\t\t\t\t\t=> DSLK nhanh hon khoang %.2fx voi cach cai dat nay.", list_speedup);
	}
	else {
		printf("\n\t\t\t\t\t\t=> MANG nhanh hon khoang %.2fx voi cach cai dat nay.", arr_speedup);
	}
}
