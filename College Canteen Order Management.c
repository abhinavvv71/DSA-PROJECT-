#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Added missing header for time()

#define MAX_ITEMS 10

// Structure to represent a food item
struct FoodItem {
    int id;
    char name[50];
    float price;
};

// Structure to represent an order
struct Order {
    int orderId;
    char customerName[50];
    struct FoodItem items[MAX_ITEMS];
    int quantities[MAX_ITEMS];
    int numItems;
    float totalAmount;
};

// Function Prototypes
void adminMenu();
void userMenu();
void addFoodItem();
void displayMenu();
void viewOrders();
void deleteOrder();
void placeOrder();

// Global file pointers
FILE *menuFile, *orderFile;

int main() {
    int choice;

    while (1) {
        printf("\n===== COLLEGE CANTEEN ORDER MANAGEMENT SYSTEM =====\n");
        printf("               Welcome to Our Canteen!\n");
        printf("---------------------------------------------------\n");
        printf("1. Admin Login\n");
        printf("2. User Login\n");
        printf("3. Exit\n");
        printf("---------------------------------------------------\n");
        printf("Please enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Clear invalid input buffer
            continue;
        }

        switch (choice) {
            case 1:
                adminMenu();
                break;
            case 2:
                userMenu();
                break;
            case 3:
                printf("\nThank you for using our system. Goodbye!\n\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }

    return 0;
}

void adminMenu() {
    int choice;
    while (1) {
        printf("\n--- Admin Panel ---\n");
        printf("1. Add New Food Item\n");
        printf("2. Display Menu\n");
        printf("3. View All Orders\n");
        printf("4. Delete an Order\n");
        printf("5. Back to Main Menu\n");
        printf("---------------------\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                addFoodItem();
                break;
            case 2:
                displayMenu();
                break;
            case 3:
                viewOrders();
                break;
            case 4:
                deleteOrder();
                break;
            case 5:
                return;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }
}

void userMenu() {
    int choice;
    while (1) {
        printf("\n--- User Panel ---\n");
        printf("Welcome, Customer!\n");
        printf("1. Display Menu\n");
        printf("2. Place an Order\n");
        printf("3. Back to Main Menu\n");
        printf("------------------\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                displayMenu();
                break;
            case 2:
                placeOrder();
                break;
            case 3:
                return;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }
}

void addFoodItem() {
    struct FoodItem item;
    menuFile = fopen("menu.dat", "ab"); // Open in append binary mode

    if (menuFile == NULL) {
        printf("\nError: Could not open menu file.\n");
        return;
    }

    printf("\n--- Add New Food Item ---\n");
    printf("Enter Food Item ID: ");
    scanf("%d", &item.id);
    printf("Enter Food Item Name: ");
    scanf("%49s", item.name); // Limit size to prevent buffer overflow
    printf("Enter Food Item Price: ");
    scanf("%f", &item.price);

    fwrite(&item, sizeof(struct FoodItem), 1, menuFile);
    printf("\nFood item added successfully!\n");

    fclose(menuFile);
}

void displayMenu() {
    struct FoodItem item;
    menuFile = fopen("menu.dat", "rb"); // Open in read binary mode

    if (menuFile == NULL) {
        printf("\nMenu is empty or file not found! Please ask admin to add items.\n");
        return;
    }

    printf("\n------------------- CANTEEN MENU -------------------\n");
    printf("%-10s %-20s %-10s\n", "ID", "Name", "Price (INR)");
    printf("----------------------------------------------------\n");

    while (fread(&item, sizeof(struct FoodItem), 1, menuFile) == 1) {
        printf("%-10d %-20s %-10.2f\n", item.id, item.name, item.price);
    }
    printf("----------------------------------------------------\n");

    fclose(menuFile);
}

void placeOrder() {
    struct Order order;
    struct FoodItem item;
    int itemId, quantity, i = 0, found = 0;
    char moreItems;

    // Get a unique order ID using current timestamp
    order.orderId = (int)time(NULL);
    
    printf("\nEnter your Name: ");
    scanf("%49s", order.customerName);

    order.numItems = 0;
    order.totalAmount = 0.0;
    
    displayMenu(); // Show menu before ordering

    do {
        if (order.numItems >= MAX_ITEMS) {
            printf("\nMaximum items limit (%d) reached for a single order!\n", MAX_ITEMS);
            break;
        }

        menuFile = fopen("menu.dat", "rb");
        if (menuFile == NULL) {
            printf("\nSorry, the menu is not available right now.\n");
            return;
        }

        printf("\nEnter the Food ID of the item you want to order: ");
        scanf("%d", &itemId);
        
        found = 0;
        rewind(menuFile); // Start reading from the beginning of the file
        while (fread(&item, sizeof(struct FoodItem), 1, menuFile) == 1) {
            if (item.id == itemId) {
                found = 1;
                printf("Enter quantity for %s: ", item.name);
                scanf("%d", &quantity);

                order.items[order.numItems] = item;
                order.quantities[order.numItems] = quantity;
                order.totalAmount += item.price * quantity;
                order.numItems++;
                break;
            }
        }

        if (!found) {
            printf("\nInvalid Food ID! Please select a valid item from the menu.\n");
        }
        fclose(menuFile);

        printf("Do you want to add another item? (y/n): ");
        scanf(" %c", &moreItems);

    } while (moreItems == 'y' || moreItems == 'Y');

    if (order.numItems == 0) {
        printf("\nNo items selected. Order cancelled.\n");
        return;
    }

    // Save order
    orderFile = fopen("orders.dat", "ab");
    if (orderFile == NULL) {
        printf("\nError: Could not save the order.\n");
        return;
    }
    fwrite(&order, sizeof(struct Order), 1, orderFile);
    fclose(orderFile);

    // --- Generate and Display Bill ---
    printf("\n------------- YOUR BILL -------------\n");
    printf("Order ID: %d\n", order.orderId);
    printf("Customer Name: %s\n", order.customerName);
    printf("--------------------------------------\n");
    printf("%-20s %-10s %-10s\n", "Item", "Quantity", "Price");
    printf("--------------------------------------\n");
    for (i = 0; i < order.numItems; i++) {
        printf("%-20s %-10d %-10.2f\n", order.items[i].name, order.quantities[i], order.items[i].price * order.quantities[i]);
    }
    printf("--------------------------------------\n");
    printf("TOTAL AMOUNT: %.2f INR\n", order.totalAmount);
    printf("--------------------------------------\n");
    printf("Thank you for your order! Enjoy your meal!\n\n");
}

void viewOrders() {
    struct Order order;
    int i;
    orderFile = fopen("orders.dat", "rb");

    if (orderFile == NULL) {
        printf("\nNo orders have been placed yet.\n");
        return;
    }

    printf("\n--- All Placed Orders ---\n");

    while (fread(&order, sizeof(struct Order), 1, orderFile) == 1) {
        printf("\n----------------------------------\n");
        printf("Order ID: %d\n", order.orderId);
        printf("Customer Name: %s\n", order.customerName);
        printf("Total Amount: %.2f INR\n", order.totalAmount);
        printf("--- Items ---\n");
        for (i = 0; i < order.numItems; i++) {
            printf("  - %s (Qty: %d)\n", order.items[i].name, order.quantities[i]);
        }
        printf("----------------------------------\n");
    }

    fclose(orderFile);
}

void deleteOrder() {
    int orderIdToDelete;
    int found = 0;
    struct Order order;
    FILE *tempFile;

    printf("\nEnter the Order ID to delete: ");
    scanf("%d", &orderIdToDelete);

    orderFile = fopen("orders.dat", "rb");
    if (orderFile == NULL) {
        printf("\nNo orders to delete.\n");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if (tempFile == NULL) {
        printf("\nError: Could not open temporary file.\n");
        fclose(orderFile);
        return;
    }

    while (fread(&order, sizeof(struct Order), 1, orderFile) == 1) {
        if (order.orderId != orderIdToDelete) {
            fwrite(&order, sizeof(struct Order), 1, tempFile);
        } else {
            found = 1; // Mark that we found and skipped the order
        }
    }

    fclose(orderFile);
    fclose(tempFile);

    // Replace the old file with the new one
    remove("orders.dat");
    rename("temp.dat", "orders.dat");

    if (found) {
        printf("\nOrder with ID %d has been deleted successfully.\n", orderIdToDelete);
    } else {
        printf("\nOrder with ID %d not found.\n", orderIdToDelete);
    }
}