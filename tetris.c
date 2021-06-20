#include "tetris.h"

static struct sigaction act, oact;
int B, count_global = 0;
int Max;

Node2 *Root;
Node2* maxScoreP;

RecNode *recRoot;

int main(){
	int exit=0;
	createRankList();
	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_RANK: rank(); break;
		case RECOMMEND: recommendedPlay(); break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2] = rand() % 7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	
	//11-12 
	Root = (Node2*)malloc(sizeof(Node2));
	Root->level = 0;
	Root->accumulatedScore = 0;
	for(i=0;i<HEIGHT;i++)
		for (j = 0; j < WIDTH; j++) {
			Root->recField[i][j] = field[i][j];
			Root->recField_Block[i][j] = 0;
		}
	Root->childsize = 40;
	Root->child = (Node2**)malloc(sizeof(Node2*)*Root->childsize);
	Root->curBlockID = nextBlock[0];
	Root->parent = NULL;
	Root->recBlockX = 0;
	Root->recBlockY = 0;
	Root->recBlockRotate = 0;
	maxScoreP = (Node2*)malloc(sizeof(Node2));
	Max = recommend(Root);

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(8, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(14,WIDTH+10);
	printw("SCORE");
	DrawBox(15,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(16,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move(9 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				if (tile != '/') {
					attron(A_REVERSE);
					printw("%c", tile);
					attroff(A_REVERSE);
				}
				else
					printw("%c", tile);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	//addch(ACS_ULCORNER);
	addch('o');
	for (i = 0; i < width; i++)
		addch('-');
		//addch(ACS_HLINE);
	//addch(ACS_URCORNER);
	addch('o');
	for(j=0;j<height;j++){
		move(y+j+1,x);
		//addch(ACS_VLINE);
		addch('|');
		move(y+j+1,x+width+1);
		//addch(ACS_VLINE);
		addch('|');
	}
	move(y+j+1,x);
	//addch(ACS_LLCORNER);
	addch('o');
	for (i = 0; i < width; i++)
		addch('-');
		//addch(ACS_HLINE);
	addch('o');
		//addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	int i, j;
	for (i = 0; i < BLOCK_HEIGHT; i++) {
		for (j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				if (i + blockY + 1 > HEIGHT || blockY + i <0 || j + blockX + 1 > WIDTH || j + blockX < 0) {
					return 0;
				}
				if (f[blockY + i][blockX + j]) {
					return 0;
				}
			}
		}
	}
	return 1;
	
	// user code
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j;
	int temp_Rotate, tempX, tempY;
	temp_Rotate = blockRotate; tempY = blockY; tempX = blockX; //현재블록정보 저장
	switch (command) {    //1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
		case KEY_UP:    
			blockRotate = (blockRotate+3)%4;
			break;
		case KEY_LEFT:
			blockX++;
			break;
		case KEY_RIGHT:
			blockX--;
			break;
		case KEY_DOWN:
			blockY--;
			break;
		default:
			break; //이전 블록정보로 되돌림
		}
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				move(i + blockY + 1, j + blockX + 1);
				printw(".");
			}
		}
	}
	int shadowY = blockY;
	while (CheckToMove(f, currentBlock, blockRotate, shadowY, blockX))
		shadowY++;
	shadowY--;
	for (int k = 0; k < 4; k++) {
		for (int _j = 0; _j < 4; _j++) {
			if(f[k+shadowY][_j+blockX] != 1 && k+shadowY<22 && _j+blockX<10&&_j+blockX>=0)
			{
				move(k + shadowY + 1, _j + blockX + 1);
				printw(".");
			}
		}
	}
	//3. 새로운 블록 정보를 그린다.
	DrawBlockWithFeatures(tempY, tempX, currentBlock, temp_Rotate);
	//DrawShadow(tempY, tempX, currentBlock, temp_Rotate);
	//DrawBlock(tempY, tempX, currentBlock, temp_Rotate, ' ');
	blockRotate = temp_Rotate; blockX = tempX; blockY = tempY;
	return;
}

//11-12
void BlockDown(int sig){
	// user code
	int check;
	check = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX);
	if (check) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else
	{
		if (blockY == -1) {
			gameOver = TRUE;
		}
		else {
			score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score += DeleteLine(field);
			blockRotate = 0; blockY = -1; blockX = WIDTH / 2 - 2;
			nextBlock[0] = nextBlock[1];
			DrawField();
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() % 7;
			Max = recommend(Root);
			/*for (int i = 0; i<HEIGHT; i++)
				for (int j = 0; j < WIDTH; j++) {
					Root->recField[i][j] = field[i][j];
				}*/
			DrawNextBlock(nextBlock);
			PrintScore(score);
		}
	}
	timed_out = 0;
	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j,touched = 0;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if (block[currentBlock][blockRotate][i][j] == 1) {
				f[blockY + i][blockX + j] = 1;
				if (f[blockY + i + 1][blockX + j] == 1 || blockY + i + 1 > 21) {
					touched++;
				}
			}
	return touched*2;
	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int i, another_i, j, count = 0;
	for (i = 0; i < HEIGHT; i++) {
		int check = 1;
		for (j = 0; j < WIDTH; j++) {
			if (f[i][j] == 0)
				check = 0;
		}
		if (check) {
			count++;
			for (another_i = i-1 ; another_i >= 0; another_i--) {
				for (j = 0; j < WIDTH; j++) {
					f[another_i+1][j] = f[another_i][j];
				}
			}
		}
	}
	return count*count*100;
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}
void DrawShadow(int y, int x, int blockID,int blockRotate){
	while (CheckToMove(field, blockID, blockRotate, y, x))
		y++;
	DrawBlock(y-1, x, blockID, blockRotate, '/');
	// user code
}

//11-12
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate)
{
	DrawShadow(y, x, blockID, blockRotate);
	DrawRecommend(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	// user code
	int readscore,i = 0;
	char readname[NAMELEN];
	FILE* fp = fopen("rank.txt", "r");
	if (fp) {
		fscanf(fp, "%d", &score_number);
		Rank = (Node*)malloc(sizeof(Node)*score_number);
		for(i=0;i<score_number;i++) {
			fscanf(fp, "%s %d", Rank[i].name, &Rank[i].score);
		}
	}
	else
		return;
	fclose(fp);
	return;
}

void rank(){
	// user code
	int X = 1, Y = score_number, ch, i, j;
	char str[NAMELEN];
	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	ch = wgetch(stdscr);
	if (ch == '1') {
		printw("X: ");
		echo();
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);
		noecho();
		printw("         name          |    score    \n");
		printw("-------------------------------------\n");
		if (X > Y || score_number == 0 || X > score_number)
			mvprintw(8, 0, "search failure: no rank in the list\n");
		else{
			for (j = X ; j <= Y; j++) {
				printw("%d:      %s                 |    %d    \n", j , Rank[j-1].name, Rank[j-1].score);
			}
		}
	}
	else if (ch == '2') {
		printw("Input the name: ");
		echo();
		scanw("%s", str);
		noecho();
		printw("         name          |    score    \n");
		printw("-------------------------------------\n");
		for (j = 0; j < score_number; j++) //랭크 노드를 전체 탐색
		{
			if (!strcmp(str, Rank[j].name)) { // 탐색중에 이 노드들 중에 strcmp를 이용하여 
				//입력받은 이름과 같은 이름을 지닌 노드만 출력한다.
				printw("   %s                 |       %d\n", str, Rank[j].score);
			}
		}
	}
	else if (ch == '3')
	{
		int target;
		printw("Input the rank: ");
		echo();
		scanw("%d", &target); //삭제하고 싶은 랭킹의 번호를 입력받는다.
		if (target > score_number || target<=0) {
			printw("Failure: htere is no such a rank");
			getch();
			return;
		}
		else {
			for (i = target - 1; i < score_number; i++) //입력받은 랭크(-1을 한 이유는 배열의 인덱스이므로
				//랭크는 1부터 시작하나 인덱스는 0부터 시작하기 때문이다.
				Rank[i] = Rank[i + 1];//해당 인덱스를 지우고 뒤의 노드들을 앞으로 땡겨오는 작업
			score_number--;//총 갯수를 하나 줄인다.
			writeRankFile(); //수정된 랭크 정보를 다시 파일에 쓴다.
			printw("!the rrank is deleted!");
		}
	}
	getch();
}

void newRank(int score){
	// user code
	char new_name[NAMELEN+1];
	char *default_name = "Anonymous";
	int i, j;
	int _score = score;
	clear();
	printw("Input your NAME\n");
	echo();
	scanw("%s", new_name);
	if (strcmp(new_name,"\0") == 0)
	{
		strcpy(new_name, default_name); //이름 입력이 없을 경우 디폴트값으로 이름 저장
	}
	noecho();
	
		score_number++;
		Node* temp = (Node*)malloc(sizeof(Node)*score_number);
		for (j = 0; j < score_number - 1; j++) {
			temp[j].score = Rank[j].score;
			strcpy(temp[j].name, Rank[j].name);
		}
		temp[score_number - 1].score = score;
		strcpy(temp[score_number - 1].name, new_name);
		for (i = score_number - 2; i >= 0 && _score >= temp[i].score; i--) //score_number-1자리는 비어잇으므로 쓰면 메모리 오류나옴
		{
			temp[i + 1].score = temp[i].score;
			strcpy(temp[i + 1].name, temp[i].name);
		}
		strcpy(temp[i + 1].name, new_name);
		temp[i + 1].score = _score;
		free(Rank);
		Rank = (Node*)malloc(sizeof(Node)*score_number);
		for (j = 0; j < score_number; j++)
		{
			Rank[j].score = temp[j].score;
			strcpy(Rank[j].name, temp[j].name);
		}
	
	writeRankFile();
}

void writeRankFile(){
	// user code
	int i;
	int num;

	FILE* fp = fopen("rank.txt", "w");
	fprintf(fp,"%d\n", score_number);
	for (i = 0; i < score_number; i++) {
		fprintf(fp, "%s", Rank[i].name);
		fprintf(fp," %d\n", Rank[i].score);
	}
	
	fclose(fp);
}
///////////////////////////////////////////////////////////////////////////



void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(maxScoreP->recBlockY,maxScoreP->recBlockX, blockID, maxScoreP->recBlockRotate,'R');
}

int recommend(Node2 *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int i, j, k, l;
	int nowscore = 0;
	int recY;
	int n = 0;
	for (i = 0; i<HEIGHT; i++)
		for (j = 0; j < WIDTH; j++) {
			root->recField[i][j] = field[i][j];
		}
	for (i = 0; i < 4; i++) {
		for (j = -1; j < WIDTH-1; j++) { //여기서 j는 BlockX의 역할
			if(root->level ==0)
			{
				nowscore = 0;
			}
			n = (i + 1)*(j + 1) - 1;
			root->child[n] = (Node2*)malloc(sizeof(Node2));
			root->child[n]->parent = root;
			for (k = 0; k < HEIGHT; k++) {
				for (l = 0; l < WIDTH; l++)
					root->child[n]->recField[k][l] = root->recField[k][l];
			}
			root->child[n]->level = root->level + 1;
			root->child[n]->curBlockID = nextBlock[root->child[n]->level - 1];
			recY = 0;
			while (CheckToMove(root->child[n]->recField, nextBlock[root->child[n]->level - 1], i, recY, j))
				recY++;
			recY--;
			root->child[n]->recBlockX = j;
			root->child[n]->recBlockY = recY;
			root->child[n]->recBlockRotate = i;
			if (root->child[n]->level < 3) {
				root->child[n]->child = (Node2**)malloc(sizeof(Node2*) * 40);
				nowscore += recommend(root->child[n]);
				nowscore += AddBlockToField(root->child[n]->recField, nextBlock[root->child[n]->level - 1], i, recY, j);
				nowscore += DeleteLine(root->child[n]->recField);
				if (root->child[n]->level == 2)
					return nowscore;
			}
			else {
				nowscore = AddBlockToField(root->child[n]->recField, nextBlock[root->child[n]->level - 1], i, recY, j);
				nowscore += DeleteLine(root->child[n]->recField);
				//mvprintw(0, 30, "%d", root->child[n]->level);
				return nowscore;
			}
			if ( max < nowscore) {
				max = nowscore;
				maxScoreP->recBlockX = root->child[n]->recBlockX;
				maxScoreP->recBlockY = root->child[n]->recBlockY;
				maxScoreP->recBlockRotate = root->child[n]->recBlockRotate;
				//mvprintw(0, 20, "%d %d %d", max, nowscore, root->child[n]->curBlockID);
				//mvprintw(1, 20, "%d %d %d", nextBlock[0], nextBlock[1], nextBlock[2]);
			}
		}
	}
	
	// user code
	mvprintw(40, 20, "%d", n);
	return max;
}

void recommendedPlay(){
	// user code
	int command = NOTHING, Max,i;
	clear();
	act.sa_handler = fill;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();
	do{
		if (timed_out == 0) {
			alarm(1);
			timed_out = 1;
		}
		command = GetCommand();
		if (ProcessCommand(command) == QUIT) {
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
}

//alarm(0);
void fill(int sig) {
	int Max;
	if (maxScoreP->recBlockY == -1)
		gameOver = TRUE;
	else {
		Max = recommend(Root);
		score += AddBlockToField(field, nextBlock[0], maxScoreP->recBlockRotate, maxScoreP->recBlockY, maxScoreP->recBlockX);

		score += DeleteLine(field);
		blockRotate = 0; blockY = -1; blockX = WIDTH / 2 - 2;
		DrawField();
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;
		DrawNextBlock(nextBlock);
		PrintScore(score);
	}
	timed_out = 0;
}