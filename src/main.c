#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEATS_PER_CATEGORY 5
#define CATEGORY_COUNT 3
#define INPUT_SIZE 128

typedef struct {
    const char *name;
    const char *state_file;
    int first_seat;
    double price;
    int seats[SEATS_PER_CATEGORY];
} Category;

static Category categories[CATEGORY_COUNT] = {
    {"VIP", "data/vip-seats.txt", 1, 450.0, {0}},
    {"Category 1", "data/category-one-seats.txt", 6, 300.0, {0}},
    {"Category 2", "data/category-two-seats.txt", 11, 180.0, {0}}
};

static void trim_newline(char *text) {
    text[strcspn(text, "\r\n")] = '\0';
}

static int read_number(const char *prompt, int *number) {
    char input[INPUT_SIZE];
    char *end;
    long value;

    printf("%s", prompt);
    if (!fgets(input, sizeof(input), stdin)) {
        return 0;
    }
    errno = 0;
    value = strtol(input, &end, 10);
    while (isspace((unsigned char)*end)) {
        end++;
    }
    if (errno || end == input || *end != '\0') {
        printf("Please enter a whole number.\n");
        return -1;
    }
    *number = (int)value;
    return 1;
}

static int load_category(Category *category) {
    FILE *file = fopen(category->state_file, "r");
    int i;

    if (!file) {
        printf("Could not open %s. Starting with empty seats.\n", category->state_file);
        memset(category->seats, 0, sizeof(category->seats));
        return 0;
    }
    for (i = 0; i < SEATS_PER_CATEGORY; i++) {
        if (fscanf(file, "%d", &category->seats[i]) != 1) {
            category->seats[i] = 0;
        }
        category->seats[i] = category->seats[i] ? 1 : 0;
    }
    fclose(file);
    return 1;
}

static int save_category(const Category *category) {
    FILE *file = fopen(category->state_file, "w");
    int i;

    if (!file) {
        printf("Could not save seat state to %s.\n", category->state_file);
        return 0;
    }
    for (i = 0; i < SEATS_PER_CATEGORY; i++) {
        fprintf(file, "%d\n", category->seats[i]);
    }
    fclose(file);
    return 1;
}

static void load_all_categories(void) {
    int i;
    for (i = 0; i < CATEGORY_COUNT; i++) {
        load_category(&categories[i]);
    }
}

static void print_seat_map(void) {
    int category_index;
    int seat_index;

    puts("\n========== SEAT MAP ==========");
    for (category_index = 0; category_index < CATEGORY_COUNT; category_index++) {
        const Category *category = &categories[category_index];
        printf("%s (seats %d-%d, %.0f TL)\n", category->name, category->first_seat,
               category->first_seat + SEATS_PER_CATEGORY - 1, category->price);
        for (seat_index = 0; seat_index < SEATS_PER_CATEGORY; seat_index++) {
            printf("  Seat %2d: %s\n", category->first_seat + seat_index,
                   category->seats[seat_index] ? "reserved" : "available");
        }
    }
    puts("==============================\n");
}

static void print_ticket(const Category *category, int seat_number, const char *first_name,
                         const char *last_name) {
    puts("\n========== RESERVATION CONFIRMED ==========");
    printf("Guest: %s %s\n", first_name, last_name);
    printf("Category: %s\n", category->name);
    printf("Seat: %d\n", seat_number);
    printf("Price: %.0f TL\n", category->price);
    puts("No personal information is written to disk.\n");
}

static void reserve_seat(void) {
    char first_name[INPUT_SIZE];
    char last_name[INPUT_SIZE];
    int category_choice;
    int seat_number;
    int status;
    Category *category;
    int index;

    print_seat_map();
    status = read_number("Choose category (1: VIP, 2: Category 1, 3: Category 2): ", &category_choice);
    if (status <= 0) return;
    if (category_choice < 1 || category_choice > CATEGORY_COUNT) {
        puts("That category does not exist.\n");
        return;
    }
    category = &categories[category_choice - 1];
    status = read_number("Choose a seat number: ", &seat_number);
    if (status <= 0) return;
    index = seat_number - category->first_seat;
    if (index < 0 || index >= SEATS_PER_CATEGORY) {
        printf("Choose a seat between %d and %d for %s.\n\n", category->first_seat,
               category->first_seat + SEATS_PER_CATEGORY - 1, category->name);
        return;
    }
    if (category->seats[index]) {
        puts("That seat is already reserved.\n");
        return;
    }

    printf("First name: ");
    if (!fgets(first_name, sizeof(first_name), stdin)) return;
    trim_newline(first_name);
    printf("Last name: ");
    if (!fgets(last_name, sizeof(last_name), stdin)) return;
    trim_newline(last_name);
    if (!first_name[0] || !last_name[0]) {
        puts("First and last name cannot be empty.\n");
        return;
    }

    category->seats[index] = 1;
    if (!save_category(category)) {
        category->seats[index] = 0;
        return;
    }
    print_ticket(category, seat_number, first_name, last_name);
}

int main(void) {
    int choice;
    int status;

    load_all_categories();
    puts("Theater Ticket Reservation System");
    puts("Seat availability is persisted in the data directory.\n");
    for (;;) {
        puts("1. View seat map");
        puts("2. Reserve a seat");
        puts("3. Exit");
        status = read_number("Select an option: ", &choice);
        if (status == 0) break;
        if (status < 0) continue;
        switch (choice) {
            case 1: print_seat_map(); break;
            case 2: reserve_seat(); break;
            case 3: puts("Goodbye."); return 0;
            default: puts("Choose 1, 2, or 3.\n"); break;
        }
    }
    return 0;
}
