#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NOTES 100
#define MAX_TEXT_LENGTH 200
#define NOTES_FILE "notes.txt"

// Coduri ANSI pentru culoare
#define COLOR_RESET   "\033[0m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_GREEN   "\033[1;32m"

typedef struct {
    int day;
    int month;
    int year;
    int hour;
    int minute;
    char text[MAX_TEXT_LENGTH];
} Note;

Note notes[MAX_NOTES];
int noteCount = 0;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void loadNotes() {
    FILE *file = fopen(NOTES_FILE, "r");
    if (file == NULL) {
        printf("Fisierul de notite nu exista. Va fi creat la prima salvare.\n");
        return;
    }

    noteCount = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) && noteCount < MAX_NOTES) {
        if (strncmp(line, "NOTA:", 5) == 0) {
            Note newNote;
            fscanf(file, "Data: %d/%d/%d\n", &newNote.day, &newNote.month, &newNote.year);
            fscanf(file, "Ora: %d:%d\n", &newNote.hour, &newNote.minute);
            fgets(line, sizeof(line), file);
            if (strncmp(line, "Text: ", 6) == 0) {
                strncpy(newNote.text, line + 6, MAX_TEXT_LENGTH);
                newNote.text[strcspn(newNote.text, "\n")] = '\0';
            } else {
                newNote.text[0] = '\0';
            }
            notes[noteCount++] = newNote;
        }
    }

    fclose(file);
}

void saveNotes() {
    FILE *file = fopen(NOTES_FILE, "w");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului pentru salvare!\n");
        return;
    }

    for (int i = 0; i < noteCount; i++) {
        fprintf(file, "NOTA:\n");
        fprintf(file, "Data: %02d/%02d/%04d\n", notes[i].day, notes[i].month, notes[i].year);
        fprintf(file, "Ora: %02d:%02d\n", notes[i].hour, notes[i].minute);
        fprintf(file, "Text: %s\n\n", notes[i].text);
    }

    fclose(file);
}

void pauseAndClear() {
    printf("\nApasa Enter pentru a continua...");
    getchar();
    getchar();
    clearScreen();
}

void addNote() {
    clearScreen();
    if (noteCount >= MAX_NOTES) {
        printf("Numarul maxim de notite a fost atins.\n");
        pauseAndClear();
        return;
    }

    Note newNote;

    printf("Introdu data (zi luna an): ");
    scanf("%d %d %d", &newNote.day, &newNote.month, &newNote.year);

    printf("Introdu ora (ora minut): ");
    scanf("%d %d", &newNote.hour, &newNote.minute);

    printf("Introdu textul notitei: ");
    getchar();
    fgets(newNote.text, MAX_TEXT_LENGTH, stdin);
    newNote.text[strcspn(newNote.text, "\n")] = '\0';

    notes[noteCount++] = newNote;
    saveNotes();
    printf("Notita a fost adaugata cu succes!\n");
    pauseAndClear();
}

void viewNotesByDate() {
    clearScreen();
    int day, month, year;
    printf("Introdu data pentru vizualizare (zi luna an): ");
    scanf("%d %d %d", &day, &month, &year);

    bool found = false;

    for (int i = 0; i < noteCount; i++) {
        if (notes[i].day == day && notes[i].month == month && notes[i].year == year) {
            printf("\n%sData:%s %02d/%02d/%04d  ", COLOR_YELLOW, COLOR_RESET,
                   notes[i].day, notes[i].month, notes[i].year);
            printf("%sOra:%s %02d:%02d\n", COLOR_CYAN, COLOR_RESET,
                   notes[i].hour, notes[i].minute);
            printf("%sText:%s %s\n", COLOR_GREEN, COLOR_RESET, notes[i].text);
            found = true;
        }
    }

    if (!found) {
        printf("Nu exista notite pentru data specificata.\n");
    }
    pauseAndClear();
}

void editNote() {
    clearScreen();
    int day, month, year, hour, minute;
    printf("Introdu data si ora notitei de editat (zi luna an ora minut): ");
    scanf("%d %d %d %d %d", &day, &month, &year, &hour, &minute);

    for (int i = 0; i < noteCount; i++) {
        if (notes[i].day == day && notes[i].month == month && notes[i].year == year &&
            notes[i].hour == hour && notes[i].minute == minute) {

            printf("Notita curenta: %sText:%s %s\n", COLOR_GREEN, COLOR_RESET, notes[i].text);
            printf("Introdu noul text: ");
            getchar();
            fgets(notes[i].text, MAX_TEXT_LENGTH, stdin);
            notes[i].text[strcspn(notes[i].text, "\n")] = '\0';

            char choice;
            printf("Doresti sa modifici data si ora? (d/n): ");
            scanf(" %c", &choice);

            if (choice == 'd' || choice == 'D') {
                printf("Introdu noua data (zi luna an): ");
                scanf("%d %d %d", &notes[i].day, &notes[i].month, &notes[i].year);
                printf("Introdu noua ora (ora minut): ");
                scanf("%d %d", &notes[i].hour, &notes[i].minute);
            }

            saveNotes();
            printf("Notita a fost actualizata cu succes!\n");
            pauseAndClear();
            return;
        }
    }

    printf("Nu s-a gasit notita cu data si ora specificate.\n");
    pauseAndClear();
}

void deleteNote() {
    clearScreen();
    int day, month, year, hour, minute;
    printf("Introdu data si ora notitei de sters (zi luna an ora minut): ");
    scanf("%d %d %d %d %d", &day, &month, &year, &hour, &minute);

    for (int i = 0; i < noteCount; i++) {
        if (notes[i].day == day && notes[i].month == month && notes[i].year == year &&
            notes[i].hour == hour && notes[i].minute == minute) {

            printf("Stergi notita: %s\"%s\"%s? (d/n): ", COLOR_GREEN, notes[i].text, COLOR_RESET);
            char confirm;
            scanf(" %c", &confirm);

            if (confirm == 'd' || confirm == 'D') {
                for (int j = i; j < noteCount - 1; j++) {
                    notes[j] = notes[j + 1];
                }

                noteCount--;
                saveNotes();
                printf("Notita a fost stearsa cu succes!\n");
            } else {
                printf("Stergere anulata.\n");
            }
            pauseAndClear();
            return;
        }
    }

    printf("Nu s-a gasit notita cu data si ora specificate.\n");
    pauseAndClear();
}

void listAllNotes() {
    clearScreen();
    if (noteCount == 0) {
        printf("Nu exista notite salvate.\n");
    } else {
        printf("\n=== Toate notitele ===\n");
        for (int i = 0; i < noteCount; i++) {
            printf("\nNotita #%d\n", i + 1);
            printf("%sData:%s %02d/%02d/%04d  ", COLOR_YELLOW, COLOR_RESET,
                   notes[i].day, notes[i].month, notes[i].year);
            printf("%sOra:%s %02d:%02d\n", COLOR_CYAN, COLOR_RESET,
                   notes[i].hour, notes[i].minute);
            printf("%sText:%s %s\n", COLOR_GREEN, COLOR_RESET, notes[i].text);
        }
    }
    pauseAndClear();
}

void displayMenu() {
    clearScreen();
    printf("\n=== Notite ===\n");
    printf("1. Adaugare notita\n");
    printf("2. Vizualizare notite pe data\n");
    printf("3. Listare toate notitele\n");
    printf("4. Editare notita\n");
    printf("5. Stergere notita\n");
    printf("6. Iesire\n");
    printf("Alege o optiune: ");
}

int main() {
    loadNotes();

    int choice;

    do {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addNote();
                break;
            case 2:
                viewNotesByDate();
                break;
            case 3:
                listAllNotes();
                break;
            case 4:
                editNote();
                break;
            case 5:
                deleteNote();
                break;
            case 6:
                clearScreen();
                printf("Salvare notite si inchidere aplicatie...\n");
                break;
            default:
                printf("Optiune invalida!\n");
                pauseAndClear();
        }
    } while (choice != 6);

    return 0;
}
