#include "../../include/order_manager.h"

void createInventory(Product inventory[MAXSIZE], int* nSP) {

	if (*nSP >= MAXSIZE) {
		printf("\n\t\t\t\t\t\tKHO DA DAY, KHONG THE THEM!");
		return;
	}

	int c;
	while ((c = getchar()) != '\n' && c != EOF);

	// Auto-generate product ID based on file
	generateNextProductId(&inventory[*nSP].id);

	// Validate product name
	do {
		printf("\n\t\t\t\t\t\tNhap ten san pham: ");
		fgets(inventory[*nSP].name, sizeof(inventory[*nSP].name), stdin);
		inventory[*nSP].name[strcspn(inventory[*nSP].name, "\n")] = '\0';

		if (strlen(inventory[*nSP].name) == 0) {
			setColor(4); printf("\t\t\t\t\t\t[!] Ten san pham khong duoc de trong!\n"); setColor(7);
			continue;
		}

		bool exists = false;
		for (int i = 0; i < *nSP; i++) {
			if (_stricmp(inventory[i].name, inventory[*nSP].name) == 0) {
				exists = true;
				break;
			}
		}

		if (exists) {
			setColor(4); printf("\t\t\t\t\t\t[!] SAN PHAM DA TON TAI TRONG KHO!\n"); setColor(7);
			inventory[*nSP].name[0] = '\0'; // Clear name to trigger re-entry
		}
	} while (strlen(inventory[*nSP].name) == 0);

	// Validate stock quantity
	do {
		printf("\n\t\t\t\t\t\tNhap so luong san pham: ");
		if (scanf("%d", &inventory[*nSP].stock_quantity) != 1) {
			while (getchar() != '\n'); // Clear buffer
			continue;
		}
		getchar();

		if (inventory[*nSP].stock_quantity <= 0) {
			setColor(4); printf("\t\t\t\t\t\t[!] So luong phai lon hon 0!\n"); setColor(7);
		}
		else if (inventory[*nSP].stock_quantity > 1000) { // Assume 1000 units as the hard stock cap per product.
			setColor(4); printf("\t\t\t\t\t\t[!] So luong qua lon (Toi da 1000)!\n"); setColor(7);
			inventory[*nSP].stock_quantity = 0; // Trigger re-entry
		}
	} while (inventory[*nSP].stock_quantity <= 0);

	// Validate price
	do {
		printf("\n\t\t\t\t\t\tNhap don gia: ");
		scanf("%lld", &inventory[*nSP].price);
		getchar();

		if (inventory[*nSP].price <= 0) {
			setColor(4); printf("\t\t\t\t\t\t[!] Don gia phai lon hon 0!\n"); setColor(7);
		}
	} while (inventory[*nSP].price <= 0);

	inventory[*nSP].sold_quantity = 0;
	(*nSP)++;

	printf("\n\t\t\t\t\t\t->THEM KHO HANG THANH CONG");
	printf("\n\t\t\t\t\t\tMa san pham: %d", inventory[*nSP - 1].id);
	printf("\n\t\t\t\t\t\tTen san pham: %s", inventory[*nSP - 1].name);
	printf("\n\t\t\t\t\t\tSo luong: %d", inventory[*nSP - 1].stock_quantity);
	printf("\n\t\t\t\t\t\tDon gia: %lld", inventory[*nSP - 1].price);

	// Save product to file
	appendProductToFile("data/inventory.txt", &inventory[*nSP - 1]);
}

void importStock(Product inventory[MAXSIZE], int* product_count) {
	int product_id, additional_quantity;

	int c;
	while ((c = getchar()) != '\n' && c != EOF);

	printf("\n\t\t\t\t\t\tNHAP ID SAN PHAM DE THEM VAO KHO: ");
	if (scanf("%d", &product_id) != 1) {
		while (getchar() != '\n');
		printf("\n\t\t\t\t\t\tID SAN PHAM KHONG HOP LE!");
		return;
	}

	int index = findProductById(inventory, *product_count, product_id);

	if (index == -1) {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY SAN PHAM!");
		while (getchar() != '\n');
		return;
	}
	else {
		// Display product name to confirm
		printf("\n\t\t\t\t\t\tSAN PHAM CHUAN BI DUOC THEM VAO: %s", inventory[index].name);
		
		printf("\n\t\t\t\t\t\tSO LUONG THEM VAO KHO: ");
		if (scanf("%d", &additional_quantity) != 1) {
			while (getchar() != '\n');
			printf("\n\t\t\t\t\t\tSO LUONG KHONG HOP LE!");
			return;
		}
		while (getchar() != '\n');

		if (additional_quantity <= 0) {
			printf("\n\t\t\t\t\t\tSO LUONG KHONG HOP LE!");
			return;
		}
		else {
			if (inventory[index].stock_quantity + additional_quantity > 1000) {
				setColor(4); printf("\n\t\t\t\t\t\tTON KHO VUOT QUA GIOI HAN (1000)!"); setColor(7);
				return;
			}
			inventory[index].stock_quantity += additional_quantity;
			printf("\n\t\t\t\t\t\tTHANH CONG! %s, SO LUONG TON: %d", inventory[index].name, inventory[index].stock_quantity);
			
			// Save updated inventory to file
			saveInventoryToFile("data/inventory.txt", inventory, *product_count);
		}
	}
}

void updateInventory(Product inventory[MAXSIZE], int* product_count) {
	int product_id;
	AmountType new_price, old_price;

	int c;
	while ((c = getchar()) != '\n' && c != EOF);

	printf("\n\t\t\t\t\t\tNhap ID san pham can cap nhat: ");
	if (scanf("%d", &product_id) != 1) {
		while (getchar() != '\n');
		printf("\n\t\t\t\t\t\tID SAN PHAM KHONG HOP LE!");
		return;
	}

	int find = findProductById(inventory, *product_count, product_id);

	if (find == -1){
		printf("\n\t\t\t\t\t\tKHONG TIM THAY SAN PHAM!");
		while (getchar() != '\n');
		return;
	}
	else {
		// Display product name to confirm
		printf("\n\t\t\t\t\t\tSAN PHAM CHUAN BI DUOC CAP NHAT: %s", inventory[find].name);
		
		old_price = inventory[find].price;
		printf("\n\t\t\t\t\t\tNhap don gia moi: ");
		if (scanf("%lld", &new_price) != 1) {
			while (getchar() != '\n');
			printf("\n\t\t\t\t\t\tDON GIA KHONG HOP LE!");
			return;
		}
		while (getchar() != '\n');
		
		// Validate price
		if (new_price < 0) {
			setColor(4); printf("\n\t\t\t\t\t\tDON GIA KHONG DUOC AM!"); setColor(7);
			return;
		}
		else if (new_price == 0) {
			setColor(4); printf("\n\t\t\t\t\t\tDON GIA PHAI LON HON 0!"); setColor(7);
			return;
		}
		else if (new_price == old_price) {
			setColor(3); printf("\n\t\t\t\t\t\tDON GIA MOI BANG DON GIA CU!"); setColor(7);
			return;
		}
		else {
			inventory[find].price = new_price;
			printf("\n\t\t\t\t\t\tTHANH CONG! %s, DON GIA MOI: %lld", inventory[find].name, inventory[find].price);
			
			// Save updated inventory to file
			saveInventoryToFile("data/inventory.txt", inventory, *product_count);
		}
	}
}

void displayInventory(Product inventory[MAXSIZE], int product_count) {
	printf("\n\t\t\t\t------------------------------------DANH SACH KHO HANG---------------------------------\n");
	printProductHeader();
	for (int i = 0; i < product_count; i++)
	{
		printf("\t\t\t\t| SP%-14d | %-25s | %-12d | %-10lld | %8d |\n",
			inventory[i].id,
			inventory[i].name,
			inventory[i].stock_quantity,
			inventory[i].price,
			inventory[i].sold_quantity);
	}
	printf("\t\t\t\t---------------------------------------------------------------------------------------\n");
}

void deleteProduct(Product inventory[MAXSIZE], int* product_count) {
	int product_id;
	char confirm;
	char product_name[100];

	int c;
	while ((c = getchar()) != '\n' && c != EOF);

	printf("\n\t\t\t\t\t\tNhap ID san pham can xoa: ");
	if (scanf("%d", &product_id) != 1) {
		while (getchar() != '\n');
		printf("\n\t\t\t\t\t\tID SAN PHAM KHONG HOP LE!");
		return;
	}

	int find = findProductById(inventory, *product_count, product_id);

	if (find == -1) {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY SAN PHAM!");
		while (getchar() != '\n');
		return;
	}
	else {
		// Display product to confirm deletion
		printf("\n\t\t\t\t\t\tSAN PHAM CAN XOA: %s", inventory[find].name);
		confirm = readYesNoChoice("\n\t\t\t\t\t\tBAN CO CHAC CHAN MUON XOA? (Y/N): ");

		if (confirm != 'Y' && confirm != 'y') {
			printf("\n\t\t\t\t\t\tHUY BO XOA SAN PHAM!");
			return;
		}

		// Save product name before deletion
		strcpy(product_name, inventory[find].name);

		// Shift all products after the deleted one
		for (int i = find; i < *product_count - 1; i++) {
			inventory[i] = inventory[i + 1];
		}

		(*product_count)--;
		printf("\n\t\t\t\t\t\tTHANH CONG! SAN PHAM %s DA DUOC XOA", product_name);

		// Save updated inventory to file
		saveInventoryToFile("data/inventory.txt", inventory, *product_count);
		decreaseProductCountInFile("data/inventory.txt");
	}
}