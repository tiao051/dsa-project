#include "../../include/order_manager.h"

void clearInputBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

void showErrorMessage(const char* message) {
	setColor(4);
	printf("\n\t\t\t\t\t\t%s", message);
	setColor(7);
}

char readYesNoChoice(const char* prompt) {
	char confirm;

	while (1) {
		if (prompt != NULL) {
			printf("%s", prompt);
		}

		if (scanf(" %c", &confirm) != 1) {
			clearInputBuffer();
			showErrorMessage("[!] Vui long nhap Y hoac N!");
			continue;
		}

		clearInputBuffer();
		if (confirm == 'Y' || confirm == 'y' || confirm == 'N' || confirm == 'n') {
			return confirm;
		}

		showErrorMessage("[!] Vui long nhap Y hoac N!");
	}
}
