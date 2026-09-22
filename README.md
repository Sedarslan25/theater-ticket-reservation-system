# Theater Ticket Reservation System

A console-based C application for managing a small theater's seat inventory across VIP, Category 1, and Category 2 sections. It validates input, prevents double-booking, and persists seat availability to text files.

> **Kısa Türkçe özet:** VIP ve iki farklı kategori için koltuk durumunu yöneten, mükerrer rezervasyonu engelleyen ve veriyi dosyada saklayan C dilinde konsol uygulaması.

## Features

- 15 seats divided into three ticket categories.
- Clear seat map with availability and prices.
- Validates menu, category, and seat-number input.
- Blocks duplicate reservations.
- Persists only seat state - it does not store guest names or identity information.
- Handles missing or malformed seat-state files safely.

## Build and run

The project uses standard C and has no third-party dependencies.

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/main.c -o theater-ticket-system
./theater-ticket-system
```

Run the executable from the repository root so it can access the `data/` directory.

## Data files

Each category's five-seat availability is stored in `data/`. A line with `0` means available and `1` means reserved. The program updates these files after a successful reservation.

To restore the included sample state after experimenting, replace the data files with the versions tracked by Git.

## Original project scope

This repository was cleaned and documented from an academic console reservation project. The code was improved for safer input handling, consistent naming, and clearer persistence behavior while retaining the original ticket-category and seat-reservation scope.

## Tech

C11, standard library, file I/O
