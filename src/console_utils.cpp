#include "../include/order_manager.h"

void clearInputBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

void showErrorMessage(const char* message) {
	setColor(4);
	printf("\n\t\t\t\t\t\t%s", message);
	setColor(7);
}
