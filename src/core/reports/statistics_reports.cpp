#include "../../../include/order_manager.h"

typedef struct DailyRevenue {
	char day[11];
	long long total;
} DailyRevenue;

typedef struct MonthlyRevenue {
	char month[8];
	long long total;
} MonthlyRevenue;

static void trimNewline(char* s) {
	if (s == NULL) return;
	s[strcspn(s, "\r\n")] = '\0';
}

static int addDailyRevenue(DailyRevenue arr[], int count, const char* day, long long value) {
	for (int i = 0; i < count; i++) {
		if (strcmp(arr[i].day, day) == 0) {
			arr[i].total += value;
			return count;
		}
	}
	strcpy(arr[count].day, day);
	arr[count].total = value;
	return count + 1;
}

static int addMonthlyRevenue(MonthlyRevenue arr[], int count, const char* month, long long value) {
	for (int i = 0; i < count; i++) {
		if (strcmp(arr[i].month, month) == 0) {
			arr[i].total += value;
			return count;
		}
	}
	strcpy(arr[count].month, month);
	arr[count].total = value;
	return count + 1;
}

void reportRevenueByDayMonth() {
	FILE* f = fopen("data/order_revenue_log.txt", "rt");
	if (f == NULL) {
		showErrorMessage("[!] Chua co du lieu doanh thu theo ngay/thang.");
		printf("\n\t\t\t\t\t\tGoi y: Hay xu ly don hang de tao du lieu doanh thu.");
		return;
	}

	DailyRevenue day_arr[2048];
	MonthlyRevenue month_arr[512];
	int day_count = 0;
	int month_count = 0;

	char line[256];
	while (fgets(line, sizeof(line), f) != NULL) {
		char day[11];
		int order_id = 0;
		long long revenue = 0;

		trimNewline(line);
		if (sscanf(line, "%10[^,],%d,%lld", day, &order_id, &revenue) != 3) {
			continue;
		}

		char month[8];
		strncpy(month, day, 7);
		month[7] = '\0';

		day_count = addDailyRevenue(day_arr, day_count, day, revenue);
		month_count = addMonthlyRevenue(month_arr, month_count, month, revenue);
	}
	fclose(f);

	if (day_count == 0) {
		showErrorMessage("[!] Chua co giao dich hoan tat de thong ke doanh thu.");
		return;
	}

	printf("\n\t\t\t\t\t\t========== DOANH THU THEO NGAY ==========");
	printf("\n\t\t\t\t\t\t| %-12s | %-18s |", "Ngay", "Tong doanh thu");
	printf("\n\t\t\t\t\t\t|--------------|--------------------|");
	for (int i = 0; i < day_count; i++) {
		printf("\n\t\t\t\t\t\t| %-12s | %18lld |", day_arr[i].day, day_arr[i].total);
	}

	printf("\n\n\t\t\t\t\t\t========== DOANH THU THEO THANG ==========");
	printf("\n\t\t\t\t\t\t| %-10s | %-18s |", "Thang", "Tong doanh thu");
	printf("\n\t\t\t\t\t\t|------------|--------------------|");
	for (int i = 0; i < month_count; i++) {
		printf("\n\t\t\t\t\t\t| %-10s | %18lld |", month_arr[i].month, month_arr[i].total);
	}
	printf("\n");
}

void reportOrderProcessingPerformance() {
	FILE* f = fopen("data/orders.txt", "rt");
	if (f == NULL) {
		showErrorMessage("[!] Khong mo duoc data/orders.txt");
		return;
	}

	int processed_count = 0;
	int pending_file_count = 0;
	int total = 0;
	fscanf(f, "%d\n", &total);

	char line[512];
	while (fgets(line, sizeof(line), f) != NULL) {
		Order order;
		int priority = 0;
		int shipping_method = 0;

		int parsed = sscanf(line, "%d,%99[^,],%99[^,],%d,%lld,%d,%d,%99[^\n]",
			&order.id,
			order.customer_name,
			order.product_name,
			&order.quantity,
			&order.price,
			&priority,
			&shipping_method,
			order.status);

		if (parsed != 8) continue;
		trimString(order.status);
		if (_stricmp(order.status, "Hoan thanh") == 0 || _stricmp(order.status, "Da giao cho DVVC") == 0) {
			processed_count++;
		}
		else {
			pending_file_count++;
		}
	}
	fclose(f);

	int pending_queue_count = 0;
	for (OrderNode* node = order_queue.head; node != NULL; node = node->next) {
		pending_queue_count++;
	}

	printf("\n\t\t\t\t\t\t========== THONG KE HIEU SUAT ==========");
	printf("\n\t\t\t\t\t\tTong so don trong he thong: %d", total);
	printf("\n\t\t\t\t\t\tSo don da xu ly: %d", processed_count);
	printf("\n\t\t\t\t\t\tSo don ton dong (theo file): %d", pending_file_count);
	printf("\n\t\t\t\t\t\tSo don ton dong trong hang doi: %d", pending_queue_count);
}

void reportOutOfStockHighDemandProducts() {
	int demand[MAXSIZE] = { 0 };

	FILE* f = fopen("data/orders.txt", "rt");
	if (f == NULL) {
		showErrorMessage("[!] Khong mo duoc data/orders.txt");
		return;
	}

	int total = 0;
	fscanf(f, "%d\n", &total);

	char line[512];
	while (fgets(line, sizeof(line), f) != NULL) {
		Order order;
		int priority = 0;
		int shipping_method = 0;

		int parsed = sscanf(line, "%d,%99[^,],%99[^,],%d,%lld,%d,%d,%99[^\n]",
			&order.id,
			order.customer_name,
			order.product_name,
			&order.quantity,
			&order.price,
			&priority,
			&shipping_method,
			order.status);

		if (parsed != 8) continue;
		trimString(order.product_name);

		for (int i = 0; i < product_count; i++) {
			if (_stricmp(inventory[i].name, order.product_name) == 0) {
				demand[i] += order.quantity;
				break;
			}
		}
	}
	fclose(f);

	int candidate_idx[MAXSIZE];
	int candidate_count = 0;
	for (int i = 0; i < product_count; i++) {
		if (inventory[i].stock_quantity == 0 && demand[i] > 0) {
			candidate_idx[candidate_count++] = i;
		}
	}

	if (candidate_count == 0) {
		printf("\n\t\t\t\t\t\tKhong co san pham chay hang (dat mua > 0 va ton kho = 0).");
		return;
	}

	for (int i = 0; i < candidate_count - 1; i++) {
		for (int j = i + 1; j < candidate_count; j++) {
			if (demand[candidate_idx[i]] < demand[candidate_idx[j]]) {
				int tmp = candidate_idx[i];
				candidate_idx[i] = candidate_idx[j];
				candidate_idx[j] = tmp;
			}
		}
	}

	printf("\n\t\t\t\t\t\t================ SAN PHAM CHAY HANG ================");
	printf("\n\t\t\t\t\t\t| %-8s | %-25s | %-12s | %-10s |", "MaSP", "TenSP", "Luong dat", "Ton kho");
	printf("\n\t\t\t\t\t\t|----------|---------------------------|--------------|------------|");
	for (int k = 0; k < candidate_count; k++) {
		int i = candidate_idx[k];
		printf("\n\t\t\t\t\t\t| %-8d | %-25s | %-12d | %-10d |",
			inventory[i].id,
			inventory[i].name,
			demand[i],
			inventory[i].stock_quantity);
	}
	printf("\n");
}

void reportExpressVsStandardRatio() {
	FILE* f = fopen("data/orders.txt", "rt");
	if (f == NULL) {
		showErrorMessage("[!] Khong mo duoc data/orders.txt");
		return;
	}

	int express_count = 0;
	int standard_count = 0;
	int total = 0;
	fscanf(f, "%d\n", &total);

	char line[512];
	while (fgets(line, sizeof(line), f) != NULL) {
		Order order;
		int priority = 0;
		int shipping_method = 0;
		int parsed = sscanf(line, "%d,%99[^,],%99[^,],%d,%lld,%d,%d,%99[^\n]",
			&order.id,
			order.customer_name,
			order.product_name,
			&order.quantity,
			&order.price,
			&priority,
			&shipping_method,
			order.status);

		if (parsed != 8) continue;
		if (shipping_method == SHIPPING_EXPRESS) express_count++;
		else standard_count++;
	}
	fclose(f);

	int sum = express_count + standard_count;
	if (sum == 0) {
		printf("\n\t\t\t\t\t\tChua co don hang de thong ke.");
		return;
	}

	double express_ratio = (double)express_count * 100.0 / (double)sum;
	double standard_ratio = (double)standard_count * 100.0 / (double)sum;

	printf("\n\t\t\t\t\t\t========== TI LE DON HOA TOC / TIEU CHUAN ==========");
	printf("\n\t\t\t\t\t\tTong don thong ke: %d", sum);
	printf("\n\t\t\t\t\t\t- Hoa toc   : %d don (%.2f%%)", express_count, express_ratio);
	printf("\n\t\t\t\t\t\t- Tieu chuan: %d don (%.2f%%)", standard_count, standard_ratio);
}
