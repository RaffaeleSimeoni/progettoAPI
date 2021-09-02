#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Page {
    int size;
    char **lines;
}Page;

typedef struct Pages {
    int size;
    struct Page **pages;
}Book;

static inline Book *newBook() {
    Book *book;

    book = malloc(sizeof(struct Pages));

    book->size = 0;
    book->pages = NULL;

    return book;
}

static inline Page *newPage() {
    Page *page;

    page = malloc(sizeof(struct Page));
    page->size = 0;
    page->lines = NULL;

    return page;
}

// size = 10

/*
book->pages = realloc(book->pages, sizeof(struct Page *) * (book->size + 1));
book->pages[book->size++] = copyPage(generalPage);
 */

static inline Page *copyPage(Page *page) {
    Page *new = malloc(sizeof(struct Page));

    new->size = page->size;
    new->lines = malloc(sizeof(char**) * page->size);

    for (int i = 0; i < page->size; i++) {
        char *line = malloc(sizeof(char) * (strlen(page->lines[i]) + 1));
        strcpy(line, page->lines[i]);
        new->lines[i] = line;
    }

    return new;
}

static inline void addPage(Book *book,Page *page){
    book->pages = realloc(book->pages, sizeof(struct Page *) * (book->size + 1));
    book->pages[book->size++] = copyPage(page);
}

static inline char *read(char *row, FILE *fp) {
    char c = fgetc(fp);
    int i = 0;

    while (c != '\n' && c != EOF) {
        row[i] = c;
        i++;
        c = fgetc(fp);
    }

    row[i] = 0;
    return row;
}

static inline int pushLine(Page *page, char *line) {
    if (page == NULL) return 0;

    char *newLine = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(newLine, line);

    if (page->size == 0) {
        page->lines = malloc(sizeof(char**));
    } else {
        page->lines = realloc(page->lines, sizeof(char**) * (page->size + 1));
    }

    page->lines[page->size] = newLine;
    page->size++;

    return 1;
}

static inline int delete(Page *page, int ind1, int ind2) {   //passa ind1-1 e ind2-1
    if (ind1 > ind2 || ind1 < 0 || page == NULL || ind1 >= page->size) return 0;
    if (ind2 >= page->size) ind2 = page->size - 1;

    int space = (ind2 - ind1) + 1;
    page->size -= space;

    for (int i = ind1; i <= ind2;i++) {
        free(page->lines[i]);
    }

    for (int i = ind1; i < page->size;i++) {
        page->lines[i] = page->lines[i + space];
    }

    page->lines = realloc(page->lines, sizeof(char**) * page->size);

    return 1;
}

static inline int change(Page *page,int ind,char *line){
    if (page == NULL) return 0;

    if(ind == page->size+1){  //linea non esistente, va aggiunta
        pushLine(page,line);
    }else{
        char *newLine = malloc(sizeof(char) * (strlen(line) + 1));
        strcpy(newLine, line);
        free(page->lines[ind-1]);
        page->lines[ind-1] = newLine;
    }
    return 1;
}

static inline void moveTo(int num,Book *source,Book *dest){   //al posto che spostare una pagina alla volta le sposta tutte assieme
    dest->pages = realloc(dest->pages, sizeof(struct Page *) * (dest->size + num));  //aggiunge spazio per 1 pagine alla dest e ci aggangia page
    //dest->pages[dest->size++] = page;
    int i=0;
    while (i<num){
        Page *page = source->pages[source->size-1];
        dest->pages[dest->size++] = page;
        source->size--;
        i++;
    }

    source->pages = realloc(source->pages, sizeof(struct Page *) * source->size);
}

static inline Page *undo(int num,Book *undoBook,Book *redoBook){
    if(num > undoBook->size-1) num = undoBook->size-1;

    if(num > 0) moveTo(num,undoBook,redoBook);

    Page *newCurrentPage = copyPage(undoBook->pages[undoBook->size-1]);  //caso undoBook->size == 0 ??
    return newCurrentPage;
}

static inline Page *redo(int num,Book *redoBook,Book *undoBook){
    if(num > redoBook->size) num = redoBook->size;

    if(num != 0) moveTo(num,redoBook,undoBook);

    Page *newCurrentPage = copyPage(undoBook->pages[undoBook->size-1]);
    return newCurrentPage;
}

static inline void resetBook(Book *book){
    for(int i=0; i<book->size; i++){   //libera tutte le linee di tutte le pagine
        for(int t=0; t<book->pages[i]->size;t++){
            free(book->pages[i]->lines[t]);
        }
        free(book->pages[i]->lines);
        free(book->pages[i]);
    }
    book->size = 0;
    free(book->pages);
    book->pages = NULL;
}

void freePage(Page *page){
    int i = 0;
    while (i<page->size){
        free(page->lines[i]);
        i++;
    }
    page->size = 0;
    free(page->lines);

    free(page);
}

int main() {
    char command[100];
    char line[1025];

    int count = 0;
    int del = 0;
    int exec = 0;

    Book *undoBook = newBook();
    Book *redoBook = newBook();
    Page *currentPage = newPage();

    addPage(undoBook,currentPage);
    int end = 1;

    do{
        read(command,stdin);
        char ind1[100],ind2[100];
        char *token;
        int i1,i2;
        switch (command[strlen(command)-1]) {
            case 'c':
                token = strtok(command, ",");
                strcpy(ind1,token);
                token = strtok(NULL,"c");
                strcpy(ind2,token);

                i1 = atoi(ind1);
                i2 = atoi(ind2);
                if (i1 < currentPage->size) exec = 1;
                for(int i=i1;i<=i2;i++){
                    read(line,stdin);
                    change(currentPage,i,line);
                }
                if (del == 0) {
                    addPage(undoBook, currentPage);
                    resetBook(redoBook);
                }
                read(line,stdin);
                count++;
                break;

            case 'd':
                token = strtok(command, ",");
                strcpy(ind1,token);
                token = strtok(NULL,"d");
                strcpy(ind2,token);

                i1 = atoi(ind1);
                i2 = atoi(ind2);

                delete(currentPage,i1-1,i2-1);
                addPage(undoBook,currentPage);
                resetBook(redoBook);
                exec = 1;
                break;
            case 'u':
                token = strtok(command, "u");
                strcpy(ind1,token);
                i1 = atoi(ind1);    //numero di undo da eseguire

                freePage(currentPage);
                currentPage = undo(i1,undoBook,redoBook);
                exec = 1;
                break;
            case 'r':
                token = strtok(command, "r");
                strcpy(ind1,token);
                i1 = atoi(ind1);

                freePage(currentPage);
                currentPage = redo(i1,redoBook,undoBook);
                exec = 1;
                break;
            case 'p':
                token = strtok(command, ",");
                strcpy(ind1,token);
                token = strtok(NULL,"p");
                strcpy(ind2,token);

                i1 = atoi(ind1);
                i2 = atoi(ind2);

                for(int i=i1;i<=i2;i++){
                    if(i==0 || i>currentPage->size)
                        fputs(".\n", stdout);
                    else {
                        fputs(currentPage->lines[i - 1], stdout);
                        fputc('\n', stdout);
                    }
                }


                break;
            case 'q':
                /*
                freePage(currentPage);
                resetBook(undoBook);
                resetBook(redoBook);

                free(undoBook);
                free(redoBook); */

                end = 0;
                break;
            default:
                printf("wrong command!!\n");
                break;

        }

        if (del == 0 && exec == 0 && count > 1900) {
            resetBook(undoBook);
            resetBook(redoBook);
            del = 1;
        }

    }while (end == 1);

    return 0;
}





