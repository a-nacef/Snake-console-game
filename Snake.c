#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // malloc
#include <conio.h>  //kbhit()
#include <unistd.h> //sleep function 
#include <windows.h> //SetConsolecursorposition (clears screen, only writes changes)
#include <ctype.h> // ToUpper()

#define width 60   //number of columns
#define height 49  // number of lines

int GameOver = 0;
char MoveOption = 'N';
char ContinueOption = 'Y';
char AutoMoveDirection = 'Z';
char BorderOption = 'N';
int score = 0;


//Global variables for the generate fly function
int flag = 0;
int sum = 0;
int buffer = 8;


//Global variables for the array + snake struct
int i=0;
char a[70][70];
int j=0;
typedef struct _snake{
       char symbol;
       struct _snake* prev;
       int row_coordinate;
       int column_coordinate;
}snake;

typedef snake* p;

p head;


//Global variables for the flies' coordinates hash-table
//this is a linked list of fly coordinates, the map will be divided into 4 quadrants; each fly's coordinate will be hashed into one of 4 quadrants

typedef struct _Fly{
        int x;
        int y;
        struct _Fly* next; 

}fly;

typedef fly* flyptr;

flyptr hashtable[4];


// function declarations
void GetGameParameters();
void BoundaryInit();
void SnakeInit();
void draw();
void GenerateFly();
void move(char);
void automove();
void MoveBody(p, int, int);
void ClearScreen();
int HashCoordinates(int,int);
void Logic();
void AddFly();
int CheckCoordinates();
void AddToSnake();
flyptr RemoveFly(int, int);
int BorderCheck();
void BorderCross();
void BodyCheck();
void DestroyTable();
void DestroySnake();



int main(){
    printf("Welcome to Snake!\n");
    printf("Use ZQSD keys to move!\n");
    GetGameParameters();
    printf("\nPlease set the display to full screen\n");
    printf("Game is starting..");
    sleep(3);
    system("cls");

    while(ContinueOption != 'N' && ContinueOption != 'n'){
    BoundaryInit();
    SnakeInit();
    while(!GameOver){
        draw();
        sleep(0.5);
        ClearScreen();
        GenerateFly();
        if(toupper(MoveOption) == 'N'){
        if(kbhit()){
           move(toupper(getch()));
        }
        }
        else{
            if(kbhit()){
              AutoMoveDirection = toupper(getch()); 
            }
            automove();
        }
    }
    system("cls");
    printf("Game over!\n");
    printf("Your score was \t %d\n", score);
    printf("Play again? Y/N\n");    
    scanf("%c", &ContinueOption);

    if(ContinueOption == 'Y' || ContinueOption == 'y'){
        GetGameParameters();
        DestroySnake();
        DestroyTable();
        AutoMoveDirection = 'Z';
        system("cls");
        GameOver = 0;
    }
  }
    return 0;
}


void GetGameParameters(){
    printf("Enable auto-movement? Y/N\n");
    do{
    scanf("%c",&MoveOption);    
    }while(MoveOption != 'Y' && MoveOption != 'N' && MoveOption != 'n' && MoveOption != 'y');

    printf("\nEnable wall crossing? Y/N \n");
    do{
    scanf("%c",&BorderOption);    
    }while(BorderOption != 'Y' && BorderOption != 'N' && BorderOption != 'n' && BorderOption != 'y');
}



void BoundaryInit(){
    printf("Initializing boundary:\n");
        for (int i = 0; i<height + 1; i++){
        for (int j = 0; j<width + 1;j++){
            a[i][j] = ' ';
        }
    }
    int i=0;
    //Initializes the map as a 2D array with a slight sleep delay for visual effects
    for (int i=0;i<width;i++){
        sleep(0.1);
        a[height][i] = '#';
        while(i<height){
        a[i][width] = '#';
        break;
        }
        a[0][i] = '#';
        a[i][0] = '#';
    }
    
}


void GenerateFly(){
    int x;
    int y;
    flag++;
    if (flag == buffer){
        x= (rand()%(height-1)) + 1;
        y= (rand()%(width-1)) + 1;
        a[x][y] = '@';
        AddFly(x,y);
        flag = 0;
        sum++;
        if (sum > 5){
            buffer = 2*sum;
        }
        else {
            buffer = 8;
        }
    }
}


void draw(){
    for (int i = 0; i<height+1; i++){
    for (int j = 0; j<width+1;j++){
           printf("%c", a[i][j]);
        }
        printf("\n");
    }
}


void SnakeInit(){
    head = (snake*) malloc(sizeof(snake));
    head->symbol = 'o';
    head->row_coordinate = rand()%49;
    head->column_coordinate = rand()%58;
    head->prev = NULL;
    a[head->row_coordinate][head->column_coordinate] = head->symbol;
}


void move(char direction){
    int x,y;
    if(direction == 'Z' || direction == 'S' || direction == 'D' ||  direction == 'Q' ) {
        a[head->row_coordinate][head->column_coordinate] = ' ';
        x = head->row_coordinate;
        y = head->column_coordinate;
    switch(direction){
        case ('Z'):
            head->row_coordinate--;
            break;
        case 'S':
            head->row_coordinate++;
            break;
        case 'D': 
            head->column_coordinate++;
            break;
        case 'Q':
            head->column_coordinate--;
            break;        
    }
    BodyCheck(head->row_coordinate,head->column_coordinate);
    a[head->row_coordinate][head->column_coordinate] = head->symbol;
    Logic();
    if(head->prev != NULL){
    MoveBody(head->prev,x,y);
    }
   }

}


void automove(){
    sleep(0.5);
    int x,y;
    a[head->row_coordinate][head->column_coordinate] = ' ';
    x = head->row_coordinate;
    y = head->column_coordinate;
    switch(AutoMoveDirection){
        case ('Z'):
            head->row_coordinate--;
            break;
        case 'S':
            head->row_coordinate++;
            break;
        case 'D': 
            head->column_coordinate++;
            break;
        case 'Q':
            head->column_coordinate--;
            break;        
    }
    BodyCheck(head->row_coordinate,head->column_coordinate);
    a[head->row_coordinate][head->column_coordinate] = head->symbol;
    Logic();
    if(head->prev != NULL){
    MoveBody(head->prev,x,y);
    }
}    
    

void ClearScreen()
{	
COORD cursorPosition;	
cursorPosition.X = 0;	
cursorPosition.Y = 0;	
SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}


int HashCoordinates(int x, int y){
    if (x<=(height/2)){
        if(y<=(width/2)){
            return 0;
        }
        else {
            return 1;
        }
    }
    else {
       if(y<=(width/2)){
            return 2;
        }
        else {
            return 3;
        }
    }
    
}

//Perform game logic on the head's movement
void Logic(){
    if (BorderCheck()){
        if(toupper(BorderOption) == 'N'){
          GameOver = 1;
        }
        else {
           BorderCross();    
        }
    }
    if(CheckCoordinates(head->row_coordinate, head->column_coordinate)==1){
       sum--;       
       AddToSnake();
    }
}


void AddFly(int x_, int y_){
     flyptr parcours;
     flyptr new = malloc(sizeof(fly));
     new->x  = x_;
     new->y  = y_;
     new->next = NULL;
     int index =  HashCoordinates(new->x, new->y);
     if(hashtable[index] == NULL){
         hashtable[index] = new;
     }
     else{
         parcours = hashtable[index];
         while(parcours->next != NULL){
              parcours = parcours->next;
         }
         parcours->next = new;
     }
}

// Verify if the new coordinates corresponds to a fly entry in the hashtable, if yes, call RemoveFly() with the the cell's order in the list and 
//the hashtable index as parameters
int CheckCoordinates(int x_, int y_){
    int index = HashCoordinates(x_,y_);
    int CellNumber = -1;
    flyptr parcours = hashtable[index];
    flyptr pparcours = parcours;
    if (parcours==NULL){ 
        return 0;
    }
    else{
        while (parcours!=NULL){
             if(x_==parcours->x && y_== parcours->y){
                 score++;
                 CellNumber++;
                 hashtable[index] = RemoveFly(CellNumber, index);
                 return 1;
             }
             parcours = parcours->next;
        }
    return 0;
}
}


void AddToSnake(){
     p parcours = head;
     p parcours2;

     p new = (snake*)malloc(sizeof(snake));
     new->symbol = 'o';
     new->prev = NULL;
     
     if(parcours->prev == NULL){ 
        //MoveBody() will set the new cell's coordinates appropriately, we're just initalizing them here since they're local vars
        new->row_coordinate = head->row_coordinate;
        new->column_coordinate = head->column_coordinate;
        parcours->prev = new;
     }
    else{
    while(parcours->prev != NULL){
          parcours2 = parcours;
          parcours = parcours->prev;
     }

    new->column_coordinate = parcours->column_coordinate;
    new->row_coordinate = parcours->row_coordinate;

    parcours->prev = new;
    a[new->row_coordinate][new->column_coordinate] = new->symbol;
    }
}

//Recursively move the snake's body by moving each cell to the preceding cell's coordinates (x_, y_)
void MoveBody(p cell, int x_, int y_){
     int x = cell->row_coordinate;
     int y = cell->column_coordinate;
     if(cell != NULL){
        cell->row_coordinate = x_;
        cell->column_coordinate = y_;
        a[x_][y_] = 'o'; 
     }
     if(cell->prev != NULL){
        MoveBody(cell->prev,x,y);
     }
     else {
         a[x][y] = ' ';
     }
}


flyptr RemoveFly(int cellnumber_, int index_){
       flyptr Head = hashtable[index_];
       flyptr parcours = hashtable[index_];
       flyptr pparcours = NULL;
       if(cellnumber_ == 0){
          pparcours = parcours->next;
          free(parcours);
          return pparcours;
       }
       else{
           for (int i = 0; i<cellnumber_;i++){
                pparcours = parcours;
                parcours = parcours->next;
           }
           pparcours->next = parcours->next;
           free(parcours);
           return Head;
       }
}


// Verify that the snake hasn't hit a border
int BorderCheck(){
    if((head->row_coordinate == 0 || head -> row_coordinate == height)||(head->column_coordinate == 0 || head -> column_coordinate == width)){
        return 1;
    }
    return 0;
}


//Verify that the head hasn't intersected with the body
void BodyCheck(int x_, int y_){
    p parcours;
    if(head->prev != NULL){
    parcours = head->prev;
    while(parcours->prev != NULL){
          parcours = parcours->prev;
          if(x_ == parcours->row_coordinate && y_ == parcours->column_coordinate){
             GameOver = 1;
          }
    }
    }
}

void BorderCross(){
    //coordinates where the head met the wall    
    int x =  head->row_coordinate;
    int y = head->column_coordinate; 

    // Adjust coordinates to make the snake come out of the other side
    if(head->row_coordinate == height){
        head->row_coordinate = 1;
    }
    else if(head->row_coordinate == 0){
        head->row_coordinate = height-1;
    }
    
    if(head->column_coordinate == width){
        head->column_coordinate = 1;
    }
    else if(head->column_coordinate == 0){
        head->column_coordinate = width-1;
    }
    a[head->row_coordinate][head->column_coordinate] = head->symbol; //Move the head to new coordinates
    a[x][y] = '#'; // Rebuilds the entry point

}


void DestroyTable(){
    flyptr parcours, pparcours;
    for(int i=0; i<4;i++){
        //delete linked list at each table entry then sets the pointer back to null
        parcours = hashtable[i];
        while(parcours!=NULL){
             pparcours = parcours;
             parcours = parcours->next;
             free(pparcours);
        }
        hashtable[i] = NULL;
    }
}


void DestroySnake(){
     p parcours, pparcours;
     parcours = head;
     //Deletes snake linked lists then reinitalizes it
     while(parcours != NULL){
          pparcours = parcours;
          parcours = parcours->prev;
          free(pparcours);
     }
     head = NULL; 
     SnakeInit();
}


