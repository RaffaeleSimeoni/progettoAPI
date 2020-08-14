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
    char *line;    //puntatore al primo elemento di un array di string contenente le righe cambiate
    int time;     //n è il n° di righe cambiate
    int i;     //puntatore al primo elem di un array contenente i numeri delle righe cambiate
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
void insertTimeNode(int time,int ind,timeList *list);
docNode* pickANode(int ind, docList *list);
static inline char *read(char *row, FILE *fp);
void clearList(timeList *list);
timeNode* rebuildList(timeNode *tNode, docList *list,timeList *UndoList,timeList *RedoList,int timeToReach);
void moveTimeNode(timeNode *node, timeList *dest,timeList *source);
void clearDocList(docList *list,docNode *node);
void changeTimeNode(timeNode *node,char *line);
timeNode* reDelete(timeNode *tNode, docList *DocList,timeList *UndoList,timeList *RedoList,int timeToReach);

int T ;
int main() {
    T = 1; //tempo corrente in cui mi trovo
    char *line;
    line = (char*) malloc(1025*sizeof(char));
    char command[100];
    timeList UndoList, RedoList;
    docList DocList;
    init_timeList(&UndoList);
    init_timeList(&RedoList);
    init_docList(&DocList);
    int end = 1;
    int timeToReach;




    do{
        read(command,stdin);
        if (strcmp(command,".") == '0'){
            read(command,stdin);
        }
        char ind1[100],ind2[100];
        char *token;
        int i1,i2;
        int cont = 0;

        switch (command[strlen(command)-1]) {
            case 'c':
                token = strtok(command, ",");
                strcpy(ind1,token);
                token = strtok(NULL,"c");
                strcpy(ind2,token);

                i1 = atoi(ind1);
                i2 = atoi(ind2);


                for(int i=i1; i<=i2; i++){  //cicla sugli indirizzi da modificare
                    read(line,stdin);
                    insertTimeNode(T,i,&UndoList);
                    if(strcmp(line,".") != 0) {
                        if (i > DocList.count) {     //la linea non è presente nel doc ma ne va creata una nuova
                            UndoList.tailer->line = NULL;
                            //strcpy(UndoList.tailer->line[cont],NULL );   //la line dell'undo rimane a NULL perchè prima non c'era niente
                            insertDocNode(&DocList,line);   //ATTENZIONE: insertDocNote richiede una stringa, non un puntatore. controlla se funziona lo stesso

                        } else {   //si va a modificare una riga esistente
                            docNode *temp = pickANode(i, &DocList);
                            changeTimeNode(UndoList.tailer,temp->line);
                            //strcpy(UndoList.tailer->line[cont], temp->line);  PROVA STRDUP
                            changeDocNode(temp, line);   //prende il nodo i e gli cambia la line con quella letta da stdin

                        }
                    }
                }
                clearList(&RedoList);
                T++;
                read(line,stdin);
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
                    //insertTimeNode(i2-i1, &UndoList);
                    //UndoList.tailer->deleted = 1;
                }

                if(i1==0 && i2==0){     //caso particolare di delete, non fa nulla ma va contata
                    insertTimeNode(T,0,&UndoList);
                    changeTimeNode(UndoList.tailer," ");
                    UndoList.tailer->deleted = 1;
                }
                else if(i1 == 1 && i2<DocList.count){   //si parte a cancellare dal primo nodo
                    for(int i=i1; i<=i2; i++){   //cicla sugli indirizzi da cancellare
                        insertTimeNode(T,i,&UndoList);
                        changeTimeNode(UndoList.tailer,temp1->line);
                        UndoList.tailer->deleted = 1;
                        temp1 = temp1->next;
                        free(temp1->prev);
                        temp1->prev = NULL;   //cancellando partendo dal primo nodo non ci sarà un prev
                        DocList.count--;
                    }
                    DocList.header=temp1;
                }
                else if(i1>1 && i2<DocList.count){   //si cancella senza toccare gli estremi della lista
                    temp2 = pickANode(i1-1,&DocList);
                    for(int i=i1; i<=i2; i++){
                        insertTimeNode(T,i,&UndoList);
                        changeTimeNode(UndoList.tailer,temp1->line);
                        UndoList.tailer->deleted = 1;
                        temp1 = temp1->next;
                        free(temp1->prev);
                        temp1->prev = temp2;
                        DocList.count--;
                    }
                    temp2->next = temp1;
                }
                else if(i1>1 && i2>=DocList.count){   //si cancella arrivando fino all'ultimo nodo
                    temp2 = pickANode(i1-1,&DocList);
                    DocList.tailer = temp2;
                    for(int i=i1; i<=i2; i++){
                        if(i<DocList.count) {
                            insertTimeNode(T,i,&UndoList);
                            changeTimeNode(UndoList.tailer,temp1->line);
                            UndoList.tailer->deleted = 1;
                            temp1 = temp1->next;
                            free(temp1->prev);
                            temp1->prev = temp2;
                            DocList.count--;
                        }
                        if(i==DocList.count){
                            insertTimeNode(T,i,&UndoList);
                            changeTimeNode(UndoList.tailer,temp1->line);
                            UndoList.tailer->deleted = 1;
                            temp2->next = NULL;
                            free(temp1);
                            DocList.count--;
                        }

                    }

                }
                else if(i1==1 && i2>=DocList.count){   //viene cancellata tutta la lista
                    temp1 = DocList.header;
                    for(int i=i1;i<=i2;i++){
                        if(i>=1 && i<DocList.count){
                            insertTimeNode(T,i,&UndoList);
                            changeTimeNode(UndoList.tailer,temp1->line);
                            UndoList.tailer->deleted = 1;
                            temp1 = temp1->next;
                            free(temp1->prev);
                            temp1->prev = NULL;   //cancellando partendo dal primo nodo non ci sarà un prev
                            cont++;
                        }
                        if(i==DocList.count){
                            insertTimeNode(T,i,&UndoList);
                            changeTimeNode(UndoList.tailer,temp1->line);
                            UndoList.tailer->deleted = 1;
                            free(temp1);
                        }
                    }
                    DocList.header = NULL;
                    DocList.tailer = NULL;
                    DocList.count = 0;

                }

                clearList(&RedoList);
                T++;
                break;
            case 'u':
                token = strtok(command, "u");
                strcpy(ind1,token);
                i1 = atoi(ind1);    //numero di undo da eseguire
                timeToReach = T - 1 - i1;
                if(timeToReach < 1){
                    timeToReach = 0;
                }

                timeNode *current = UndoList.tailer;
                timeNode *prev;
                while (current != NULL && current->time > timeToReach){
                    if(current->deleted == 1) {  //va eseguita l'undo di una delete
                       current = rebuildList(current,&DocList,&UndoList,&RedoList,timeToReach);
                    }else{   //va eseguita una change per riportare la riga (ancora esistente) al vecchio valore
                        docNode *temp = pickANode(current->i,&DocList);
                        if(current->line == NULL) {  //vuol dire che si fa l'undo di un change che ha aggiunto la riga (prima del change c'era NULL, non un'altra riga)
                            free(current->line);   //serve? è gia NULL
                            int new_size = strlen(temp->line);
                            current->line = (char*)calloc(new_size,sizeof(char));   //prima di eliminare il docNode aggiorno il valore (mi servirà in caso di redo)
                            strcpy(current->line,temp->line);
                            //free(current->line);
                            //current->line = strdup(temp->line);
                            clearDocList(&DocList,temp);
                        }else{   //si fa l'undo di una modifica ad una riga già esistente
                            char cambio[1025];
                            strcpy(cambio,temp->line);    //backup riga prima di fare undo
                            strcpy(temp->line, current->line);   //aggiorna riga
                            //current->line = realloc(current->line,strlen(cambio));
                            //strcpy(current->line,cambio);
                            free(current->line);
                            current->line = strdup(cambio);
                        }
                        prev = current->prev;
                        moveTimeNode(current,&RedoList,&UndoList);
                        current = prev;
                    }
                }
                T = timeToReach+1;

                break;
            case 'r':
                token = strtok(command, "u");
                strcpy(ind1,token);
                i1 = atoi(ind1);    //numero di undo da eseguire
                timeToReach = T + i1;
                if(timeToReach < 1){
                    timeToReach = 0;
                }

                if(RedoList.count > 0){
                    current = RedoList.tailer;
                    while (current != NULL && current->time < timeToReach){
                        if(current->deleted == 1) {  //va ri-eseguita la delete
                            //rieseguo delete
                            current = reDelete(current,&DocList,&UndoList,&RedoList,timeToReach);
                        }else{  //va eseguita una change per riportare la riga (ancora esistente) al valore prima della undo
                            if(current->i > DocList.count){  //l'undo ha tolto una riga rimettendola a NULL
                                insertDocNode(&DocList,current->line);
                                current->line = NULL;
                            }else{   //l'undo ha riportato la riga al vecchio valore (non NULL) devo rieseguire la change
                                docNode *temp = pickANode(current->i,&DocList);
                                char cambio[1025];
                                strcpy(cambio,temp->line);
                                strcpy(temp->line,current->line);
                                strcpy(current->line,cambio);
                            }
                            timeNode *prev2 = current->prev;
                            moveTimeNode(current,&UndoList,&RedoList);
                            current = prev2;
                        }
                    }
                }

                T = timeToReach;
                /*
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


                */
                break;
            case 'p':
                token = strtok(command, ",");
                strcpy(ind1,token);
                token = strtok(NULL,"p");
                strcpy(ind2,token);

                i1 = atoi(ind1);
                i2 = atoi(ind2);

                if(i1 == 0 && i2 == 0) {
                    printf(".\n");       //sostituire con operazioni per file
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
                        if(temp != NULL && temp->next != NULL) {
                            temp = temp->next;
                        }
                    }
                }

                break;
            case 'q':
                clearDocList(&DocList,DocList.header);
                clearList(&UndoList);
                clearList(&RedoList);
                free(line);
                end = 0;
                break;
            default:
                printf("wrong command!!\n");
                break;

        }



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

timeNode* reDelete(timeNode *tNode, docList *DocList,timeList *UndoList,timeList *RedoList,int timeToReach){
    int nodoPrima = tNode->i-1;

    docNode *temp = pickANode(tNode->i,DocList);
    docNode *temp2;
    docNode *primo;
    if(nodoPrima != 0){
        primo = temp->prev;
    }
    timeNode *next;
    while(tNode != NULL && tNode->deleted == 1 && tNode->time < timeToReach && temp != NULL){
        //aggiorno e sposto il nodo da redoList a undoList
        next = tNode->prev;
        changeTimeNode(RedoList->tailer,temp->line);
        moveTimeNode(tNode,UndoList,RedoList);
        tNode = next;

        temp2 = temp->next;
        temp->prev = NULL;
        temp->next = NULL;
        free(temp);
        DocList->count--;
        temp = temp2;
    }

    if(nodoPrima == 0){
        DocList->header = temp;
    }else{
        temp->prev = primo;
        primo->next = temp;
    }

    return tNode;
}

timeNode* rebuildList(timeNode *tNode, docList *list,timeList *UndoList,timeList *RedoList,int timeToReach){
    int cont = 0;
    docList *tempList;
    timeNode *prev;
    tempList = (docList*)malloc(sizeof(docList));
    init_docList(tempList);  //magari problema con templist come puntatore
    int nodoPrima; //indice del nodo a cui ci si deve attaccare lato testa


    while(tNode != NULL && tNode->deleted == 1 && tNode->time > timeToReach){  //cicla sul numero di righe eliminate con la delete e crea una lista temporanea con gli n elementi eliminati
        //------------
        docNode *new_node;
        docNode *temp;
        new_node = (docNode*) calloc(1,sizeof(docNode));

        new_node->prev = NULL;

        if (tempList->count == 0)
        {
            new_node->next = NULL;
            tempList->header = tempList->tailer = new_node;

        }else{
            new_node->next = tempList->header;
            temp = tempList->header;
            temp->prev = new_node;
            tempList->header = new_node;
        }

        strcpy(new_node->line,tNode->line);      //copia nella line del nodo la striga letta in input
        tempList->count++;
        //-----------
        prev = tNode->prev;
        //devo aggiornare la linea di tnode prima di spostarlo in RedoList
        moveTimeNode(tNode,RedoList,UndoList);
        nodoPrima = tNode->i-1;
        tNode = prev;
    }

    //int nodoDopo = tNode->i;  //indice del nodo a cui ci si deve attaccare lato coda

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
    return tNode;
}

void changeDocNode(docNode *node, String l){
    strcpy(node->line,l);

}


void insertTimeNode(int time,int ind,timeList *list){
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
    new_node->time = time;
    new_node->i = ind;
    new_node->deleted = 0;
    //new_node->line = (char *)malloc(sizeof(char)*strlen(line));     //alloca spazio per le stringhe
    //strcpy(new_node->line,line);
    list->count++;
}
void changeTimeNode(timeNode *node,char *line){
    //node->line = (char *)malloc(sizeof(char)*strlen(line));     //alloca spazio per le stringhe
    //strcpy(node->line,line);
    node->line = strdup(line);
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
        free(current->line);
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

    if(node->prev == NULL){  //si sta cancellando a partire dal primo nodo
        list->header = NULL;
        list->tailer = NULL;
    }else{
        list->tailer = node->prev;
        list->tailer->next = NULL;
    }

    while (node != NULL){
        next = node->next;
        free(node);
        node = next;
        list->count--;
    }
}

void moveTimeNode(timeNode *node, timeList *dest,timeList *source){
    timeNode *temp = node->prev;
    if(dest->count == 0){
        dest->header = node;
        dest->tailer = node;
        node->prev = NULL;
        node->next = NULL;
    }else{
        node->prev = dest->tailer;
        dest->tailer->next = node;
        dest->tailer = node;
    }
    dest->count++;

    if(source->count == 1){
        source->header = source->tailer = NULL;
    }else{
        source->tailer = temp;
        temp->next = NULL;
    }
    source->count--;
}





