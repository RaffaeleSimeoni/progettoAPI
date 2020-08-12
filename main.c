#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char String[1025] ;

typedef struct docListNode{
    char line[1025];
    struct docListNode *next;
    struct docListNode *prev;

} docNode;


typedef struct timeLineListNode{
    char **line;    //puntatore al primo elemento di un array di string contenente le righe cambiate
    int n;     //n è il n° di righe cambiate
    int *i;     //puntatore al primo elem di un array contenente i numeri delle righe cambiate
    int deleted;
    struct timeLineListNode *next;
    struct timeLineListNode *prev;

} timeNode;


typedef struct list2 {

    int count;   // tiene traccia di quanti nodi sono presenti all'interno della lista
    timeNode *header;    // puntatore al primo nodo della lista
    timeNode *tailer;// puntatore all'ultimo nodo della lista

} timeList;

typedef struct list1 {

    int count;   // tiene traccia di quanti nodi sono presenti all'interno della lista
    docNode *header;    // puntatore al primo nodo della lista
    docNode *tailer;    // puntatore all'ultimo nodo della lista

} docList;


void init_timeList(timeList *list2);
void init_docList(docList *list);
void insertDocNode(docList *list,String line);
void changeDocNode(docNode *node, String l);
void insertTimeNode(int dim,timeList *list);
docNode* pickANode(int ind, docList *list);
static inline char *read(char *row, FILE *fp);
void clearList(timeList *list);
void rebuildList(timeNode *tNode, docList *list);
void moveTimeNode(timeNode *node, timeList *list);
void clearDocList(docList *list,docNode *node);

int T = 1;
int main() {
    //int T = 1; //tempo corrente in cui mi trovo
    char *line;
    line = (char*) malloc(1025*sizeof(char));
    char command[100];
    timeList UndoList, RedoList;
    docList DocList;
    init_timeList(&UndoList);
    init_timeList(&RedoList);
    init_docList(&DocList);
    int end = 1;




    do{
        read(command,stdin);
        if (strcmp(command,".") == '0'){
            read(command,stdin);
        }
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

                int cont = 0;
                /*
                 * OPERAZIONI SULLA TIMELINE
                 */
                insertTimeNode(i2-i1,&UndoList);

                /*
                 * OPERAZIONI SUL DOCUMENTO
                 */
                for(int i=i1; i<=i2; i++){
                    read(line,stdin);
                    if(strcmp(line,".") != 0) {
                        if (i > DocList.count) {     //la linea non è presente nel doc ma ne va creata una nuova
                            UndoList.tailer->i[cont] = i;
                            UndoList.tailer->line[cont] = NULL;
                            //strcpy(UndoList.tailer->line[cont],NULL );   //la line dell'undo rimane a NULL perchè prima non c'era niente
                            insertDocNode(&DocList,line);   //ATTENZIONE: insertDocNote richiede una stringa, non un puntatore. controlla se funziona lo stesso

                        } else {
                            docNode *temp = pickANode(i, &DocList);
                            UndoList.tailer->i[cont] = i;
                            UndoList.tailer->line[cont] = (char*)malloc(strlen(line)*sizeof(char));
                            UndoList.tailer->line[cont] = strdup(temp->line);
                            //strcpy(UndoList.tailer->line[cont], temp->line);  PROVA STRDUP
                            changeDocNode(temp, line);   //prende il nodo i e gli cambia la line con quella letta da stdin

                        }
                        cont++;
                    }
                }
                clearList(&RedoList);
                break;

            case 'd':
                token = strtok(command, ",");
                strcpy(ind1,token);
                token = strtok(NULL,"d");
                strcpy(ind2,token);

                i1 = atoi(ind1);
                i2 = atoi(ind2);
                cont = 0;

                docNode *temp1;
                docNode *temp2;
                if(i1>0 && i1<=DocList.count) {
                    temp1 = pickANode(i1, &DocList);
                    insertTimeNode(i2-i1, &UndoList);
                    UndoList.tailer->deleted = 1;
                }

                if(i1 == 1 && i2<DocList.count){   //si parte a cancellare dal primo nodo
                    for(int i=i1; i<=i2; i++){
                        UndoList.tailer->i[cont] = i;
                        UndoList.tailer->line[cont] = (char*)malloc(strlen(line)*sizeof(char));
                        UndoList.tailer->line[cont] = strdup(temp1->line);
                        //strcpy(UndoList.tailer->line[cont], temp1->line);  PROVA STRDUP
                        temp1 = temp1->next;
                        free(temp1->prev);
                        temp1->prev = NULL;   //cancellando partendo dal primo nodo non ci sarà un prev
                        cont++;
                    }
                    DocList.header=temp1;
                }

                if(i1>1 && i2<DocList.count){   //si cancella senza toccare gli estremi della lista
                    temp2 = pickANode(i1-1,&DocList);
                    for(int i=i1; i<=i2; i++){
                        UndoList.tailer->i[cont] = i;
                        UndoList.tailer->line[cont] = (char*)malloc(strlen(line)*sizeof(char));
                        UndoList.tailer->line[cont] = strdup(temp1->line);
                        //strcpy(UndoList.tailer->line[cont], temp1->line); PROVA STRDUP
                        temp1 = temp1->next;
                        free(temp1->prev);
                        temp1->prev = temp2;
                        cont++;
                    }
                    temp2->next = temp1;
                }

                if(i1>1 && i2>=DocList.count){   //si cancella arrivando fino all'ultimo nodo
                    temp2 = pickANode(i1-1,&DocList);
                    DocList.tailer = temp2;
                    for(int i=i1; i<=i2; i++){
                        if(i<DocList.count) {
                            UndoList.tailer->i[cont] = i;
                            UndoList.tailer->line[cont] = (char*)malloc(strlen(line)*sizeof(char));
                            UndoList.tailer->line[cont] = strdup(temp1->line);
                            //strcpy(UndoList.tailer->line[cont], temp1->line);  //PROVA STRDUP
                            temp1 = temp1->next;
                            free(temp1->prev);
                            temp1->prev = temp2;
                        }
                        if(i==DocList.count){
                            UndoList.tailer->i[cont] = i;
                            UndoList.tailer->line[cont] = (char*)malloc(strlen(line)*sizeof(char));
                            UndoList.tailer->line[cont] = strdup(temp1->line);
                            //strcpy(UndoList.tailer->line[cont], temp1->line);  PROVA STRDUP   SE FUNZIONA FALLO ANCHE NEI CASI RESTANTI
                            temp2->next = NULL;
                            free(temp1);
                        }
                        cont++;
                    }

                }

                if(i1==1 && i2>=DocList.count){   //viene cancellata tutta la lista
                    insertTimeNode(DocList.count, &UndoList);
                    UndoList.tailer->deleted = 1;
                    temp1 = DocList.header;
                    for(int i=i1;i<=i2;i++){
                        if(i>=1 && i<DocList.count){
                            UndoList.tailer->i[cont] = i;
                            UndoList.tailer->line[cont] = (char*)malloc(strlen(line)*sizeof(char));
                            strcpy(UndoList.tailer->line[cont], temp1->line);
                            temp1 = temp1->next;
                            free(temp1->prev);
                            temp1->prev = NULL;   //cancellando partendo dal primo nodo non ci sarà un prev
                            cont++;
                        }
                        if(i==DocList.count){
                            UndoList.tailer->i[cont] = i;
                            UndoList.tailer->line[cont] = (char*)malloc(strlen(line)*sizeof(char));
                            strcpy(UndoList.tailer->line[cont], temp1->line);
                            free(temp1);
                        }
                    }
                    DocList.header = NULL;
                    DocList.tailer = NULL;
                    DocList.count = 0;

                }

                DocList.count = DocList.count - (i2-i1) -1;

                if(i1==i2==0){     //caso particolare di delete, non fa nulla ma va contata
                    insertTimeNode(1, &UndoList);
                    UndoList.tailer->deleted = 1;
                    UndoList.tailer->i[cont] = 0;    //non è stato eliminato niente ma va contata come undo
                    DocList.count++;
                }
                clearList(&RedoList);
                break;
            case 'u':
                token = strtok(command, "u");
                strcpy(ind1,token);
                i1 = atoi(ind1);    //numero di undo da eseguire
                if(i1>UndoList.count){
                    i1 = UndoList.count;
                }

                for(int i=1;i<=i1;i++){   //cicla sul numero degli undo
                    timeNode *current = UndoList.tailer;
                    insertTimeNode(UndoList.tailer->n,&RedoList);
                    if(current->deleted == 1){  //va eseguita l'undo di una delete
                       rebuildList(current,&DocList);
                    }else{   //va eseguita una change per riportare la riga (ancora esistente) al vecchio valore
                        cont = 0;
                        docNode *temp = pickANode(current->i[current->n-1],&DocList);
                        docNode *eliminaDa;
                        for(int t=current->n;t>0;t--){   //cicla sul numero di righe presenti nel nodo undo
                            if(current->line[t] == NULL) {  //vuol dire che si fa l'undo di un change che ha aggiunto la riga (prima del change c'era NULL, non un'altra riga)
                                eliminaDa = temp;
                                current->line[t] = strdup(temp->line);  //prima di eliminare il docNode aggiorno il valore (mi servirà in caso di redo)
                            }else{   //si fa l'undo di una modifica ad una riga già esistente
                                char cambio[1025];
                                strcpy(cambio,temp->line);
                                strcpy(temp->line, current->line[t]);
                                strcpy(current->line[t],cambio);
                            }
                            if(temp->prev != NULL) {
                                temp = temp->prev;
                            }else{
                                temp = NULL;
                            }
                        }
                        clearDocList(&DocList,eliminaDa);
                        /*cont = current->n -1;
                        docNode *temp = pickANode(current->i[cont],&DocList);       //DA CAMBIARE: parto dall'ultimo e vengo indietro
                        for(int t=1;t<=current->n;t++){
                            if(current->line[cont] == NULL){  //vuol dire che si fa l'undo di un change che ha aggiunto la riga (prima del change c'era NULL, non un'altra riga)
                                if(DocList.count <= 1){   //si fa l'undo per rimuovere la prima change
                                    DocList.header = DocList.tailer = NULL;
                                }else {
                                    temp->prev->next = NULL;
                                    DocList.tailer = temp->prev;
                                }
                                current->line[cont] = temp->line;
                                DocList.count--;
                                free(temp);

                            }else {
                                char cambio[1025];
                                strcpy(cambio,temp->line);
                                strcpy(temp->line, current->line[cont]);
                                strcpy(current->line[cont],cambio);
                            }
                            cont--;
                            //temp = temp->next;
                        }
                         */
                    }

                    //devo poi spostare l'undo appena fatto nella RedoList
                    UndoList.tailer = current->prev;
                    moveTimeNode(current,&RedoList);
                    UndoList.count--;
                }

                break;
            case 'r':
                token = strtok(command, "r");
                strcpy(ind1,token);
                i1 = atoi(ind1);    //numero di redo da eseguire
                if(i1>RedoList.count){
                    i1 = RedoList.count;
                }

                if(i1 != 0) {
                    for (int i = 1; i <= i1; i++) {   //cicla sul numero dei redo
                        timeNode *current = RedoList.tailer;
                        docNode *temp;
                        if (current->deleted == 1) {  //va eseguita la redo di una delete
                            //ri-eseguo la delete
                        } else {   //va eseguita una change per riportare la riga al vecchio valore
                            cont = 0;
                            for (int t = 0; t < current->n; t++) {   //cicla sul numero di elementi del nodo della redo
                                if (current->i[cont] > DocList.count) {  //la redo deve scrivere su una riga non esistente (va creato un DocNode)
                                    insertDocNode(&DocList," ");
                                    strcpy(DocList.tailer->line,current->line[cont]);
                                    current->line[cont] = NULL;      //aggiorno il nodo che poi andrà spostato nella UndoList
                                } else {    //la redo cambia una riga esistente
                                    if (cont == 0) {
                                        temp = pickANode(current->i[cont], &DocList);
                                    } else {
                                        temp = temp->next;
                                    }
                                    char cambio[1025];
                                    strcpy(cambio, temp->line);
                                    strcpy(temp->line, current->line[cont]);
                                    strcpy(current->line[cont], cambio);
                                }
                                cont++;
                            }

                        }
                        //devo poi spostare il Redo appena fatto nella UndoList
                        RedoList.tailer = current->prev;   //PROBLEMA: current.prev è NULL, perchè?
                        moveTimeNode(current, &UndoList);
                        RedoList.count--;
                    }
                }



                break;
            case 'p':
                token = strtok(command, ",");
                strcpy(ind1,token);
                token = strtok(NULL,"p");
                strcpy(ind2,token);

                i1 = atoi(ind1);
                i2 = atoi(ind2);

                if(i1 == '0' && i2 == '0') {
                    printf("\n.");       //sostituire con operazioni per file
                }else {
                    docNode *temp;
                    if(i1<=DocList.count) {
                        temp = pickANode(i1, &DocList);
                    }
                    for (int i = i1; i <= i2; i++) {
                        if (i > DocList.count) {
                            printf(".\n");      //sostituire con operazioni per file
                        } else {
                            printf("%s\n", temp->line);      //sostituire con operazioni per file
                        }
                        if(temp->next != NULL) {
                            temp = temp->next;
                        }
                    }
                }

                break;
            case 'q':
                end = 0;
                break;
            default:
                printf("wrong command!!\n");
                break;

        }
        T++;


    }while(end==1);

    //change(1,2,1,line,DocList,TimeList);

    //inizializzo le due liste per il Documento e la timeline


    return 0;
}



void init_timeList(timeList *list) {

    list->count = 0;
    list->header = list->tailer = NULL;     // sia la testa che la coda puntano inizialmente al primo nodo

}

void init_docList(docList *list) {

    list->count = 0;
    list->header = list->tailer = NULL;     // sia la testa che la coda puntano inizialmente al primo nodo

}


void insertDocNode(docList *list,String l){
    // creo il nuovo nodo e gli alloco uno spazio di memoria
    docNode *new_node;
    docNode *temp;
    new_node = (docNode*) malloc(sizeof(docNode));
    //temp = (docNode*) malloc(sizeof(docNode));

    new_node->next = NULL;

    if (list->count == 0)
    {
        new_node->prev = NULL;
        list->header = list->tailer = new_node;

    }else{
        new_node->prev = list->tailer;
        //list->tailer->next = new_node;
        temp = list->tailer;      //ATTENZIONE: list->tailer->next = new_node non funziona
        temp->next = new_node;
        list->tailer = new_node;
        //free(temp);
    }

    strcpy(new_node->line,l);      //copia nella line del nodo la striga letta in input
    list->count++;
}

void rebuildList(timeNode *tNode, docList *list){
    int nodoPrima = tNode->i[0]-1; //indice del nodo a cui ci si deve attaccare lato testa
    //int nodoDopo = tNode->i[tNode->n-1]+1;  //indice del nodo a cui ci si deve attaccare lato coda
    //docNode *temp = pickANode(nodoPrima,list);
    //docNode *temp2 = pickANode(nodoDopo,list);
    if(tNode->deleted == 1){  //va eseguita l'undo di una delete
        int cont = 0;
        docList *tempList;
        tempList = (docList*)malloc(sizeof(docList));
        init_docList(tempList);
        for(int t=1;t<=tNode->n;t++){   //cicla sul numero di righe eliminate con la delete e crea una lista temporanea con gli n elementi eliminati
            insertDocNode(tempList,tNode->line[cont]);
        }

        if(nodoPrima==0 && list->count>0){  //la tempList va reinserita prima del primo nodo in docList
            docNode *temp2 = pickANode(nodoPrima+1,list);
            tempList->tailer->next = temp2;   //attacca l'ultimo nodo in coda
            temp2->prev = tempList->tailer;
            list->header = tempList->header;
        }

        if(nodoPrima>0 && nodoPrima<list->count){  //la tempList va reinserita senza toccare gli estremi di docList
            docNode *temp = pickANode(nodoPrima,list);
            docNode *temp2 = pickANode(nodoPrima+1,list);
            tempList->header->prev = temp;   //attacca il primo nodo in testa
            temp->next = tempList->header;

            tempList->tailer->next = temp2;   //attacca l'ultimo nodo in coda
            temp2->prev = tempList->tailer;
        }

        if(nodoPrima>0 && nodoPrima>=list->count){  //la tempList sfora la coda di docList
            docNode *temp = pickANode(nodoPrima,list);
            tempList->header->prev = temp;   //attacca il primo nodo in testa
            temp->next = tempList->header;
            list->tailer = tempList->tailer;
        }

        if(nodoPrima==0 && nodoPrima>=list->count){
            list->header = tempList->header;
            list->tailer = tempList->tailer;
        }

        list->count = list->count + tempList->count;
        free(tempList);
    }
}

void changeDocNode(docNode *node, String l){
    strcpy(node->line,l);

}


void insertTimeNode(int dim,timeList *list){
    // creo il nuovo nodo e gli alloco uno spazio di memoria
    timeNode *new_node;
    new_node = (timeNode*) malloc(sizeof(timeNode));
    new_node->next = NULL;

    if (list->count == 0)
    {
        new_node->prev = NULL;
        list->header = list->tailer = new_node;

    }else{
        new_node->prev = list->tailer;
        list->tailer->next = new_node;
        list->tailer = new_node;
    }

    //strcpy(new_node->line,l);      //copia nella line del nodo la striga letta in input
    new_node->n = dim +1;
    new_node->i = (int*)malloc((new_node->n)*sizeof(int));     //alloca spazio per l'array degli indici riga cambiati
    new_node->line = (char **)calloc((new_node->n),sizeof(char));     //alloca spazio per l'array delle stringhe
    list->count++;
}

docNode* pickANode(int ind, docList *list){
    docNode *temp;
    if (ind <= list->count/2) {
        temp = list->header;
        for (int i = 1; i < ind; i++) {    //se il nodo è cancellato (deleted = 1) salta direttamente a quello dopo senza contarlo
                temp = temp->next;
        }
    } else{
        temp = list->tailer;
        for(int i=list->count; i > ind; i--){
                temp = temp->prev;
        }
    }
    return temp;
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

void clearList(timeList *list){
    timeNode *current = list->header;
    timeNode *next;

    while (current != NULL){
        int num = current->n;
        for(int i=0;i<num;i++){   //libera tutte le stringhe puntate dal vettore line[]
            free(current->line[i]);
        }
        next = current->next;
        free(current);
        current = next;
    }

    list->header = NULL;
    list->tailer = NULL;
    list->count = 0;
}

void clearDocList(docList *list,docNode *node){   //libera la docList a partire dal nodo passato (compreso)
    docNode *next;

    if(list->count == 1){  //si sta cancellando a partire dal primo nodo
        list->header = NULL;
        list->tailer = NULL;
    }else{
        list->tailer = node->prev;
    }

    while (node != NULL){
        next = node->next;
        free(node);
        node = next;
        list->count--;
    }
}

void moveTimeNode(timeNode *node, timeList *list){
    if(list->count == 0){
        list->header = node;
        list->tailer = node;
        node->prev = NULL;
        node->next = NULL;
    }else{
        node->prev = list->tailer;
        list->tailer->next = node;
        list->tailer = node;
    }
    list->count++;
}





