#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4996)
#define MAX_CARD_HAVE 13
#define nMAX 26
#define MAX_SPEECH_DATA 300      //컴퓨터가 할 수 있는 대사를 정해둠

typedef struct card_list {
	int num;
	char color;
	bool masked;        //두 플레이어는 서로의 카드를 볼수 없게 한다.
	bool reveal;        //상대방 카드의 값을 맞춘 경우 카드를 쓰러트려 정보를 공개하고 masked값을 0으로 준다
	bool wasJoker;
	int onceSuspect;
}card;

void BubbleSort(card *Arr, int arrLen);
void Print(card *shuffledDeck, int arrLen);
/*--------------------------------------*/
void beforePlay();
void ShuffleDeck(card *shuffledDeck);
void reshuffle(card *shuffledDeck);
void playerSet();
int mainDisplay();
static int selection=2;

card player[MAX_CARD_HAVE];
card computer[MAX_CARD_HAVE];
static int playerCards = 3;       //플레이어가 시작할때의 뽑아야할 카드개수
static int computerCards = 3;     //컴퓨터가 시작할때의 뽑아야할 카드개수
static int drawCards = 5;         //두 플레이어가 총 6장의 카드를 뽑았음. 배열저장이어서 -1

void play();
void status(char whoTurn);
void status_mask_value(card *shuffledDeck, int arrLen);
void Draw_cards(int drawCards, card *who, int whoCards, card *shuffledDeck);
int Player_Turn(card *player, card *shuffledDeck);
int Computer_Turn(card *computer, card *shuffledDeck);
card shuffledDeck[26];
/*---------------THINK------------------*/
void AI(card *computer, int computerCards);
void playerThink(card *player, int playerCards);
card lastCardInfo;
card AlreadyKnow[26];
card AlreadyKnow_whiteArr[13];
card AlreadyKnow_blackArr[13];
//card guess_player_did_not_have;
static int whiteArr_count = 0;
static int blackArr_count = 0;
static int playerLife = 3;
static int computerLife = 3;
int moreAsk;
static int oneMore_token=0;
static int GO_percent=14;           //드러나는 카드의 개수가 증가할수록 컴퓨터가 한번 더할 확률이 증가

char comTalk [MAX_SPEECH_DATA][100];
static int speechCount=0;
void loadComSpeech();
/*--------------------------------------*/


int main()
{
	/*------------------------------------규칙------------------------------------*/
	//처음 시작할때 플레이어는 4장의 카드를 받고 시작한다.
	//시작할때 조커를 받을 경우 안 나올때까지 덱에 다시 넗고 새로운 패를 받는다.
	//자신의 차례가 될 때마다 카드를 한 장씩 받는다.
	/*----------------------------------------------------------------------------*/
	mainDisplay();
	beforePlay();
	
	play();
	printf("GMAEOVER..\n\n");
	printf("results: ");
	if (playerLife>computerLife) puts("PLAYER WIN!!");
	else if (playerLife<computerLife) puts("COMPUTER WIN!!");
	else puts("NO WINNER!");
	
	sleep(2);
	puts("-------------------------------Press Enter Button-------------------------------");
	getchar();
	getchar();
	
	return 0;
}
int mainDisplay()
{
    printf("\x1b[40m\n");
    puts("┏-----------------------------------------------------------------------------┓");
    puts("┃      ■ ■ ■         ■      ■       ■   ■ ■ ■   ■     ■    ■ ■ ■   ■ ■ ■      ┃");
    puts("┃      ■     ■      ■ ■      ■     ■      ■     ■ ■   ■   ■          ■        ┃");
    puts("┃      ■     ■     ■   ■      ■   ■       ■     ■  ■  ■   ■          ■        ┃");
    puts("┃      ■     ■    ■ ■ ■ ■      ■ ■        ■     ■   ■ ■   ■          ■        ┃");
    puts("┃      ■ ■ ■     ■       ■      ■       ■ ■ ■   ■     ■    ■ ■ ■   ■ ■ ■      ┃"); 
    puts("┃                                                                             ┃");
    puts("┃                    ■ ■ ■     ■ ■ ■     ■ ■ ■     ■ ■ ■ ■                    ┃");
    puts("┃                   ■         ■     ■    ■     ■   ■                          ┃");
    puts("┃                   ■         ■     ■    ■     ■   ■ ■ ■                      ┃");
    puts("┃                   ■         ■     ■    ■     ■   ■                          ┃");
    puts("┃                    ■ ■ ■     ■ ■ ■     ■ ■ ■     ■ ■ ■ ■      ver 1.8       ┃");
    puts("┗-----------------------------------------------------------------------------┛");
    printf("\x1b[0m\n");sleep(1);
    printf("\x1b[1m\n");
    puts("-------------------------------Press Enter Button-------------------------------");
    printf("\x1b[0m\n");
    getchar();
    int row=2, col=34, arraylength=2, width=5, menulength=2;
    const char *testarray[]={" P  L  A  Y ", " E  X  I  T "};
    initscr();
    noecho();
    keypad(stdscr,TRUE);
    
    printw("--------------------------------------MENU--------------------------------------");
    RESELECT:
    selection=barmenu(testarray,row,col,arraylength,width,menulength,2);
    
    switch(selection)
    {
        case 0: break;
        case 1: exit(1);
        default: goto RESELECT;
        
    }
    
    refresh();
    endwin();
    

}
int barmenu(const char **array,const int row, const int col, const int arraylength, const int width, int menulength, int selection)
{
    mvprintw(9, 0, "--------------------------------------------------------------------------------");
    int counter,offset=0,ky=0;
    char formatstring[15];
    curs_set(0);
        
    if (arraylength < menulength) menulength=arraylength;
            
    if (selection > menulength) offset=selection-menulength+1;
    
    sprintf(formatstring,"%%-%ds",width); // remove - sign to right-justify the menu items
    
    while(ky != 27)
    {
        for (counter=0; counter < menulength; counter++)
        {
            if (counter+offset==selection) attron(A_REVERSE);
            mvprintw(row+counter,col,formatstring,array[counter+offset]);
            attroff(A_REVERSE);
            switch(selection)
            {
                case 0: mvprintw(7,32,"Play single with conputer     "); break;
                case 1: mvprintw(7,32,"quit this game..              "); break;
                default: mvprintw(7,32,"select the menu list");
            }

        }
            
        ky=getch();
    
        switch(ky)
        {
            case KEY_UP:
                if (selection)
                {
                    selection--;
                    if (selection < offset) offset--;
                }break;
            
            case KEY_DOWN:
                if (selection < arraylength-1)
                {
                    selection++;
                    if (selection > offset+menulength-1) offset++;
                }break;
            
            case KEY_HOME: selection=0; offset=0; break;
            
            case KEY_END:
                selection=arraylength-1;
                offset=arraylength-menulength;
                break;
            
            case KEY_PPAGE:
                selection-=menulength;
                if (selection < 0) selection=0;
                offset-=menulength;
                if (offset < 0) offset=0;
                break;
            
            case KEY_NPAGE:
                selection+=menulength;
                if (selection > arraylength-1) selection=arraylength-1;
                offset+=menulength;
                if (offset > arraylength-menulength) offset=arraylength-menulength;
                break;
            
            
            case 10: return selection; break;   //enter
            
            case KEY_F(1): return -1;// function key 1
                            
            case 27: 
                ky=getch();
                if (ky == 27)
                {
                    curs_set(0);
                    mvaddstr(9,77,"   ");
                    return -1;
                }
                else if (ky=='[') {getch();getch();}
                else ungetch(ky);
        }
    }
    return -1;
}
void beforePlay()
{
	ShuffleDeck(shuffledDeck);
	loadComSpeech();
	playerSet();
}

void ShuffleDeck(card *shuffledDeck)
{
	srand((unsigned int)time(NULL));//srand로 초기화
	card cardList[26]; //= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, '-', 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, '-'};
	
	for (int i = 0; i<2; i++)
	{
		for (int j = 0; j<13; j++)
		{
			if (i == 0)
			{
				if (j == 12) cardList[j].num = '-';
				else cardList[j].num = j;

				cardList[j].color = 'B';
				cardList[j].masked=true;
				cardList[j].reveal=false;
				
			}
			if (i == 1)
			{
				if (j == 12) cardList[j + 13].num = '-';
				else cardList[j + 13].num = j;

				cardList[j + 13].color = 'W';
				cardList[j].masked=true;
				cardList[j].reveal=false;
			}
		}
	}
	//'-'는 ascii코드로 45

	int cardDeck[nMAX] = { 0, };                        //생성된 덱의 주소를 저장할 변수
	int bCheckExistOfNum[26] = { false, };              //생성된 번호가 중복인지 체크할 변수(인덱스)

	for (int i = 0; i<nMAX;     )                       //번호가 모두 생성될 때까지 반복한다..
	{
		int nTemp = rand() % 26;                        //0~25 범위의 번호를 생성한다.
		if (bCheckExistOfNum[nTemp] == false)           //중복 여부 판단
		{
		    bCheckExistOfNum[nTemp] = true;         //번호가 중복 생성되지 않게 존재 여부를 true로 한다.
            cardDeck[i] = nTemp;                    //범위가 0~25 인 값의 주소
            shuffledDeck[i]=cardList[cardDeck[i]];  //무작위 덱 생성
            ++i;                                    //증감 연산을 for문에서 하지 않고 여기서 한다.
		}
	}
	
	/*---------카드 셔플이 잘 되어있는지 확인할 때 사용할 것----------*/
	printf("\t\t\t\t무작위 덱 생성중...\n");
	//Print(shuffledDeck, nMAX);
	puts("");sleep(1);

	printf("\t\t\t\t조커 위치 변경중...\n");
	reshuffle(shuffledDeck);
	//Print(shuffledDeck, nMAX);
	for(int i=0; i<nMAX; i++)
	{
	    shuffledDeck[i].masked=true;
	    shuffledDeck[i].reveal=false;
	    if(shuffledDeck[i].masked==false) printf("%d ", i);
	    if(shuffledDeck[i].reveal==true) printf("%d ", i);
	    
	}
	puts("");sleep(1);
	/*----------------------------------------------------------------*/
}

void reshuffle(card *shuffledDeck)
{
	/*reshuffle becauseof joker'-'*/
	int ChangeJokerPos = 0;
	int noJokerCount = 0;
	card change;
	while (noJokerCount<8)
	{
		for (int i = 0; i<8; i++)      //두 플레이어가 뽑는 카드는 8장이다.
		{
			//조커가 있을 경우 안뽑은 카드를 덱에서 가져와 서로 위치를 바꾼다
			if (shuffledDeck[i].num == '-')
			{
				ChangeJokerPos = rand() % 18 + 8;
				change = shuffledDeck[i];
				shuffledDeck[i] = shuffledDeck[ChangeJokerPos];
				shuffledDeck[ChangeJokerPos] = change;
			}
			else noJokerCount++;
		}
	}
}
static int whoFirst;
void playerSet()
{
    puts("\t\t\t\t카드 나누는중...");
	int PPDC = 0;      //PlayerPreDrawCards
	int CPDC = 0;      //ComputerPreDrawCards
	whoFirst = rand() % 2;
	
	for (int i = 0; i<playerCards + computerCards; i++)
	{
		switch (whoFirst)
		{
		    case 0:
	    	{
	    		if (i % 2 == 0) { player[PPDC] = shuffledDeck[i]; player[PPDC].masked==true; player[PPDC].reveal==false; PPDC++; }
	    		else { computer[CPDC] = shuffledDeck[i]; computer[CPDC].masked==true; computer[CPDC].reveal==false; CPDC++; }
	    	}break;
	    	case 1:
	    	{
	    		if (i % 2 == 0) { computer[CPDC] = shuffledDeck[i]; computer[CPDC].masked==true; computer[CPDC].reveal==false; CPDC++; }
	    		else { player[PPDC] = shuffledDeck[i]; player[PPDC].masked==true; player[PPDC].reveal==false; PPDC++; }
	    	}break;
		}
		GO_percent--;
	}
	for (int i = 0; i<computerCards; i++)      //사전에 컴퓨터가 뽑은 카드의 정보를 인지하도록 한다.
	{
		if (computer[i].color=='W') { AlreadyKnow_whiteArr[whiteArr_count++] = computer[i]; }
		else { AlreadyKnow_blackArr[blackArr_count++] = computer[i]; }
	}
	
	
	/*---------------------------------------------------------------------------------------------------------*/
	BubbleSort(player, playerCards);
	BubbleSort(computer, computerCards);
	//printf("player      Card:"); Print(player, playerCards);
	//printf("computer    Card:"); Print(computer, computerCards);
	/*---------------------------------------------------------------------------------------------------------*/
	sleep(1);
}

void play()
{
	puts("");
	puts("--------------------------------------PLAY--------------------------------------");
	sleep(2);
	system("clear");
	if (whoFirst == 0) Player_Turn(player, shuffledDeck);          //누가 먼저 할지 정하고 서로의 함수를 불러서 조건문으로 끝냄
	else Computer_Turn(computer, shuffledDeck);
}
int Player_Turn(card *player, card *shuffledDeck)
{
	printf("player Turn\t");
	puts("1 Draw!\n");
	playerCards++;//n번째 카드이므로 n-1배열에 삽입
	playerLife++;
	
	player[playerCards - 1] = shuffledDeck[++drawCards];
	player[playerCards - 1].masked = true;
	player[playerCards - 1].reveal = false;
	
	if (player[playerCards - 1].num == '-')
	{
		int jokerValue; player[playerCards - 1].wasJoker=true;
	back:
		printf("NOW: "); Print(player, playerCards);
		printf("%c Joker!  insert value what U want: ", player[playerCards - 1].color);
		scanf("%d", &jokerValue);
		if (0 <= jokerValue && jokerValue <= 11) player[playerCards - 1].num = jokerValue;
		else { printf("value is only 0~11\n"); sleep(1); goto back; }
		system("clear");
	}
	lastCardInfo = player[playerCards - 1];
	printf("\x1b[40mplayer  cards:%3d \tcard left:%3d\tdraw card:%2d%c\t\t", playerCards, 26 - (drawCards + 1), player[playerCards - 1].num, player[playerCards - 1].color);
	printf("Life[ "); for (int i = 0; i<playerLife; i++) printf("\x1b[31m♥ \x1b[39m"); puts("]\x1b[49m");

GO:
	status('P');
	playerThink(player, playerCards);
	if(playerLife<=0 || computerLife<=0) goto GAMEEND;
	if (moreAsk)
	{
	REVALUE:
		printf("GO?(1) or STOP?(0): ");
		scanf("%d", &moreAsk);
		switch(moreAsk)
		{
		    case 1: goto GO;
		    case 0: puts("player STOPped.."); break;
		    default: puts("invalid value!"); goto REVALUE;
		}
	}
	GAMEEND:
	
	status('P');

	system("clear");
	if ((26 - (drawCards + 1))>0 && playerLife>0 && computerLife>0) Computer_Turn(computer, shuffledDeck);
	else return 1;
}
void playerThink(card *player, int playerCards)
{
	int ask_x_pos = 0;
	int ask_card_num = 0;
REINPUT:
	moreAsk = 0;
	printf("player ask x pos & number: "); scanf("%d %d", &ask_x_pos, &ask_card_num);
	if (0<ask_x_pos && ask_x_pos <= computerCards);        //x좌표는 1~computerCards까지 
	else { printf("x-pos is now only 1~%d\n", computerCards); goto REINPUT; }
	if (0 <= ask_card_num && ask_card_num <= 11);        //x값은 0~11까지 
	else { printf("card value is only 0~11\n"); goto REINPUT; }

	printf("player ask x pos: %d is %d%c right?\n\n", ask_x_pos, ask_card_num, computer[ask_x_pos - 1].color);
	sleep(2);
	if (computer[ask_x_pos - 1].num == ask_card_num)
	{
	    if(computer[ask_x_pos - 1].reveal==false)
	    {
	        computer[ask_x_pos - 1].masked = false; computer[ask_x_pos - 1].reveal = true;
		    computerLife--;
	        moreAsk = 1;
	        printf("Exactly match! computer open seleted card: %d%c\n", computer[ask_x_pos - 1].num, computer[ask_x_pos - 1].color);
	    }
		else
	    {
	        puts("You already choose opened card..."); goto ALREADYREVEALED;
	    }
	}
	else
	{
	    ALREADYREVEALED:
		lastCardInfo.masked = false; lastCardInfo.reveal = true;
		if (lastCardInfo.color=='W') AlreadyKnow_whiteArr[whiteArr_count++] = lastCardInfo;
		else AlreadyKnow_blackArr[blackArr_count++] = lastCardInfo;
		moreAsk = 0;
		for (int i = 0; i<playerCards; i++)
		{
			if (lastCardInfo.num == player[i].num && lastCardInfo.color == player[i].color && lastCardInfo.masked != player[i].masked)
				{player[i]= lastCardInfo; break;}
		}
		
		printf("Wrong match! player open last card: %d%c    ", lastCardInfo.num, lastCardInfo.color); puts("Life--");
		//printf("computer guess you didn\'t have that card: %d%c\n", ask_card_num, computer[ask_x_pos - 1].color);
		playerLife--;
		GO_percent--;
	}
	/*----------------------------------------------------------------------------------*/
	sleep(1);
}
int Computer_Turn(card *computer, card *shuffledDeck)
{
	int jokerValue;
	printf("computer: ");puts(comTalk[rand()%speechCount]);
	printf("computer Turn\t");
	puts("1 Draw!\n");
	computerCards++;
	computerLife++;
	computer[computerCards - 1] = shuffledDeck[++drawCards];
	computer[computerCards - 1].masked = true;
	computer[computerCards - 1].reveal = false;
	if (computer[computerCards - 1].color=='W') AlreadyKnow_whiteArr[whiteArr_count++] = computer[computerCards - 1];
	else AlreadyKnow_blackArr[blackArr_count++] = computer[computerCards - 1];
	if (computer[computerCards - 1].num == '-')
	{
	    computer[computerCards - 1].wasJoker=true;
		//printf("NOW: ");Print(computer, computerCards);
		//printf("%c Joker!  computer insert value...\n", computer[computerCards-1].color);
		//sleep(2);
		jokerValue = (int)rand() % 12;
		computer[computerCards - 1].num = jokerValue;
		//printf("changed joker value: %d\n\n", computer[computerCards-1].num);
	}
	lastCardInfo = computer[computerCards - 1];
	const char *BnW[2]; BnW[0] = "■"; BnW[1] = "□";
	printf("computer cards: %d    card left: %d    draw card:%d%c\t\t", computerCards, 26 - (drawCards + 1), computer[computerCards - 1].num, computer[computerCards - 1].color);
	//printf("\x1b[40mcomputer cards:%3d\tcard left:%3d\tdraw card:%4s\t\t", computerCards, 26-(drawCards+1), computer[computerCards-1].color=='B' ? BnW[0] : BnW[1]);
	printf("Life[ "); for (int i = 0; i<computerLife; i++) printf("\x1b[34m♥ \x1b[39m"); puts("]\x1b[49m");
	status('C');
	//puts("컴퓨터가 이미 알고 있는 숫자");
	//BubbleSort(AlreadyKnow_whiteArr, whiteArr_count);
	//BubbleSort(AlreadyKnow_blackArr, blackArr_count);
	
// 	for(int i=0; i<whiteArr_count; i++)
// 	{
// 	    printf("%d%c ",AlreadyKnow_whiteArr[i].num, AlreadyKnow_whiteArr[i].color);
// 	}
// 	for(int i=0; i<blackArr_count; i++)
// 	{
// 	    printf("%d%c ",AlreadyKnow_blackArr[i].num, AlreadyKnow_blackArr[i].color);
// 	}
	puts("");
	COMPUTERGO:
	AI(computer, computerCards);
	
	if(playerLife<=0 || computerLife<=0) goto GAMEEND;
	if(oneMore_token)
	{
	    	int oneMore=rand()%abs(GO_percent);
	    	if(oneMore==0){ puts("computer GO again"); goto COMPUTERGO;}
	    	else puts("computer STOPped..");
	}
	GAMEEND:
	
	status('C');
	system("clear");
	if ((26 - (drawCards + 1))>0 && computerLife>0 && playerLife>0) Player_Turn(player, shuffledDeck);
	else return 1;
}
void AI(card *computer, int computerCards)
{
	/*-------------------------------------------------computer가 플레이어 카드를 추론하는 과정--------------------------------------------*/
	//1. 따라서 난수로 n번째 배열의 값을 추측하되 자신이 갖고있지 않는 카드를 토대로 추론한다. 
	//만약 지정한 카드의 정보가 //1-1.틀렸을 경우. 찍었던 카드의 값, 색을 저장하고 이후에 같은 카드를 찍을 때 같은 숫자가 나오지 않도록 한다.
	//1-2 맞았을 경우. 찍었던 카드 정보를 모두 출력(masked=false, reveal=true)한다.
	//2. 추론하려는 카드가 공개된 두 카드의 사이에 있을 경우(ex. 7 ?  ?  10)  검색을 하되 입력 요건 값을 난수로 둔다.
	//3. 조커때문에 각 색마다 같은 숫자가 두개인 것이 있다. 조커카드를 뽑으면 해당 카드가 중복된다는 것을 저장한다.
	/*-------------------------------------------------------------------------------------------------------------------------------------*/
	int comPick;

	int whichCard = 0;        //비공개된 카드가 n개면 n/2 번째를 기준으로 왼쪽부터 맞출지, 오른쪽부터 맞출지 생각함
	int determine = 0;
	int retry = 1;
	puts("computer think...");

	/*----------------------------------------------------PHASE1----------------------------------------------------*/
	//플레이어의 카드가 6개 이하일때 사용
	//배치된 카드의 중앙을 기준으로 왼쪽(작은 수) 또는 오른쪽(큰 수) 에 배치된 카드의 값을 적절한 값을 생각하여 추론한다 
	/*----------------------------------------------------PHASE2----------------------------------------------------*/
	//자신이 이미 갖고있는 값을 상대방의 카드를 확인할때 사용하지 않는다.
	/*----------------------------------------------------PHASE3----------------------------------------------------*/
	//상대방의 카드를 전부 알고있다고 가정하고 하나씩 맞춘다.
	int s;
	DO_NOT_REPEAT_SAME_MISTAKE:
	if(playerCards<5 || playerCards==6)
	{
	    do
		{
			retry = 0;
			
			do
			{
			    retry=0;
			    comPick =rand()%playerCards;
			    
			    if(player[comPick].reveal==true || player[comPick].masked==false) retry=1;
			}while(retry);
			
			
			

			if(player[comPick].wasJoker==true)
			{
			    if(player[comPick].color=='W')
			    {
			        for(int i=0; i<whiteArr_count; i++)
			        {
			            if(AlreadyKnow_whiteArr[i].num == player[comPick].num) {AlreadyKnow_whiteArr[i].num='-';break;}
			        }
			    }
			    else
			    {
			        for(int i=0; i<blackArr_count; i++)
			        {
			            if(AlreadyKnow_blackArr[i].num == player[comPick].num) {AlreadyKnow_blackArr[i].num='-';break;}
			        }
			    }
			    retry=1;
			}
			else
			{
			    if(12%playerCards==0) determine = rand()%(12/playerCards)+comPick*(12/playerCards);     //1을 안 더해주면 11을 고려하지 못함
			    else                                                                                    //12/5 는 10, 11카드를 넣을 공간이 없어서 만듬
			    {
			        if(comPick==4) determine = rand()%4+comPick*(12/playerCards);
			        else determine = rand()%(12/playerCards)+comPick*(12/playerCards);
			    }
			    
			    for (s = 0; s<whiteArr_count; s++)
			    {
			        if (AlreadyKnow_whiteArr[s].num == determine) { retry = 1; break; }
			        else continue;
			    }
			    for (s = 0; s<blackArr_count; s++)
			    {
			    	if (AlreadyKnow_blackArr[s].num == determine) { retry = 1; break; }
			    	else continue;
			    }
			}
			
		} while (retry);
	}
	else if(6<playerCards && playerCards<9)
	{
	    for(int j=0; j<playerCards; j++)
		{
		    if(player[j].masked==true && player[j].reveal==false) {comPick=j; break;}
		}
	    do
		{
			retry = 0;
			if(player[comPick].wasJoker==true)
			{
			    if(player[comPick].color=='W')
			    {
			        for(int j=0; j<whiteArr_count; j++)
			        {
			            if(AlreadyKnow_whiteArr[j].num == player[comPick].num) {AlreadyKnow_whiteArr[j].num='-';break;}
			        }
			    }
			    
			    else
			    {
			        for(int j=0; j<blackArr_count; j++)
			        {
			            if(AlreadyKnow_blackArr[j].num == player[comPick].num) {AlreadyKnow_blackArr[j].num='-';break;}
			        }
			    }
			    retry=1;
			    break;
			}
		}while(retry);
		do
		{
		    retry=0;
			if(comPick==(playerCards-1))
			{
			    do
			    {
			        determine=rand()%12; 
			    }while(determine>=player[comPick-1].num);
			}
			else
			{
			    switch(comPick)
			    {
			        case 0: determine=rand()%player[comPick+1].num; break;
			        default: determine=rand()%(player[comPick+1].num - (player[comPick-1].num) +1)+player[comPick-1].num;
			    }
			    if(player[comPick].color=='W')
			    {
			        for (s = 0; s<whiteArr_count; s++)
			        {
			            if (AlreadyKnow_whiteArr[s].num == determine) { retry = 1; break; }
			            else continue;
			        }
			    }
			    else
			    {
			        for (s = 0; s<blackArr_count; s++)
			        {
			            if (AlreadyKnow_blackArr[s].num == determine) { retry = 1; break; }
			            else continue;
			        }
			    }
			}
		} while (retry);
	}
	else
	{
	    for(int j=0; j<playerCards; j++)
		{
		    if(player[j].masked==true && player[j].reveal==false) {comPick=j; break;}
		}
		
		determine=player[comPick].num;
	}
	if(determine==player[comPick].onceSuspect) goto DO_NOT_REPEAT_SAME_MISTAKE;
	
	printf("computer ask x pos: %d is %d%c right?\n\n", comPick + 1, determine, player[comPick].color);
	sleep(2);
	if (player[comPick].num == determine)
	{
	    
	    
	        player[comPick].masked = false; player[comPick].reveal = true;

		    if (player[comPick].color=='W') AlreadyKnow_whiteArr[whiteArr_count++] = player[comPick];
		    else AlreadyKnow_blackArr[blackArr_count++] = player[comPick];

		    playerLife--;
		    printf("Exactly match! player open seleted card: %d%c\n", player[comPick].num, player[comPick].color);
		    oneMore_token=1;
		    GO_percent--;
	}
	else
	{
	    player[comPick].onceSuspect == determine;
		lastCardInfo.masked = false; lastCardInfo.reveal = true;
		computerLife--;
		for (int i = 0; i<computerCards; i++)
		{
			if (lastCardInfo.num == computer[i].num && lastCardInfo.color == computer[i].color && lastCardInfo.masked != computer[i].masked)
				{computer[i] = lastCardInfo; break;}
		}
		printf("Wrong match! computer open last card: %d%c    ", lastCardInfo.num, lastCardInfo.color); puts("Life--\n");
		oneMore_token=0;
	}
	sleep(1);
}

void status(char whoTurn)
{
	BubbleSort(player, playerCards);
	BubbleSort(computer, computerCards);
	puts("--------------------------------------------------------------------------------");
	printf("player      Card:"); if (whoTurn == 'P') Print(player, playerCards); else status_mask_value(player, playerCards);
	printf("computer    Card:"); if(whoTurn=='C') status_mask_value(computer, computerCards); else status_mask_value(computer, computerCards);
	//printf("computer    Card:"); if (whoTurn == 'C') Print(computer, computerCards); else status_mask_value(computer, computerCards);
	puts("--------------------------------------------------------------------------------");
	puts("");
	sleep(1);
}
void status_mask_value(card *shuffledDeck, int arrLen)
{
	for (int i = 0; i<arrLen; i++)
	{
		if (shuffledDeck[i].masked == true)
		{
			if (shuffledDeck[i].color == 'B') printf("    □");
			else printf("    ■");
		}
		else
		{
		    if(shuffledDeck[i].reveal==true) printf("%4d%c", shuffledDeck[i].num, shuffledDeck[i].color);
			else printf("%4d%c", shuffledDeck[i].num, shuffledDeck[i].color);
		}
	}
	puts("");
}
void Print(card *shuffledDeck, int arrLen)
{
	for (int i = 0; i<arrLen; i++)     //생성된 덱
	{   //조커가 있으면 문자출력, 없으면 정수 출력

		if (shuffledDeck[i].num == '-') printf("%4c%c", shuffledDeck[i].num, shuffledDeck[i].color);
		else if(shuffledDeck[i].reveal==true && shuffledDeck[i].masked== false) printf("\x1b[1m\x1b[36m%4d%c\x1b[0m", shuffledDeck[i].num, shuffledDeck[i].color);
		else printf("%4d%c", shuffledDeck[i].num, shuffledDeck[i].color);
	}
	puts("");
}
void loadComSpeech()
{
    FILE * pFile = fopen ("computerSpeech.txt", "r");
    
    if (pFile == NULL)
	{
		printf("입력 파일을 열 수 없습니다.\n");
		exit(1);
	}
    
    while(!feof(pFile))
    {
        fgets(comTalk[speechCount], 100, pFile);
        speechCount++;
    }
    fclose (pFile);
    puts("\t\t\t\t컴퓨터 대본 짜는중...\n");
}
void BubbleSort(card *Arr, int arrLen)
{
	bool flag;
	card temp;
	for (int i = 0; i<arrLen - 1; i++)
	{
		flag = false;
		for (int j = 0; j<arrLen - (i + 1); j++)
		{
			if (Arr[j].num > Arr[j + 1].num)       //꺽쇠 방향으로 오름차순, 내림차순 결정
			{
				temp = Arr[j];      //값 교환 부분
				Arr[j] = Arr[j + 1];
				Arr[j + 1] = temp;
				flag = true;
			}
		}
		if (flag == false) break;
	}
}