#ifdef __linux__ 
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "./constants.h"
#include <time.h>
#include <stdbool.h>
#include <unistd.h> 

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int game_is_running = FALSE;

struct gameObject {
	float x;
	float y;
	float width;
	float height;
};
typedef struct gameObject game_object;

struct mouse
{
	int x,y;
	bool left,fLeft;
} mouse_control;

struct no {
	game_object v;
	struct no* next;
};
typedef struct no* node;



//#define BOXS_LENGHT 1
//game_object Boxs[BOXS_LENGHT];

struct sBox
{
	int x,y,status;
	node selected;
} selectedBox;


SDL_Rect *objectToRect(game_object go){
	SDL_Rect *sdlR = (SDL_Rect *)malloc(sizeof(SDL_Rect));
	*sdlR = (SDL_Rect){
		(int)go.x,
		(int)go.y,
		(int)go.width,
		(int)go.height
	};
	return sdlR;
}

bool CollPointRect(game_object rect, int x, int y){
	if (MAX(rect.x, (rect.x + rect.width)) > x && MIN(rect.x, (rect.x + rect.width)) < x &&  MAX(rect.y, rect.y + rect.height) > y && MIN(rect.y, rect.y + rect.height) < y )
	{
		return true;
	}
	return false;
	
}



int initialize_window(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error SDL init\n");
		return FALSE;
	}

	window = SDL_CreateWindow(
		NULL,
		10/*SDL_WINDOWPOS_CENTERED*/,
		10/*SDL_WINDOWPOS_CENTERED*/,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0/*SDL_WINDOW_BORDERLESS*/
	);
	if (!window) {
		fprintf(stderr, "Error creating SDL Window. \n");
		return FALSE;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL Renderer.\n");
		return FALSE;
	}


	return TRUE;
}

void destroy_window() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void process_input() {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		game_is_running = FALSE;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			game_is_running = FALSE;
		}
		else {

		}
		break;
	case SDL_KEYUP:

		break;
	// case SDL_MOUSEBUTTONDOWN:
	// 	if(event.button.button == SDL_BUTTON_LEFT){
	// 		mouse_control.fLeft = true;
	// 		mouse_control.left = true;
  	// 	}
	// 	//printf("\t\t\t\t\t\td");
    //     break;
	// case SDL_MOUSEBUTTONUP:
	// 	if(event.button.button == SDL_BUTTON_LEFT && mouse_control.left){
	// 		mouse_control.left = false;
	// 		mouse_control.fLeft = false;
  	// 	}
	// 	//printf("\t\t\t\t\t\tu");
    //     break;
	}
	Uint32 ms = SDL_GetMouseState(&mouse_control.x, &mouse_control.y);
	if((ms & 1) == 1){//left clicl
		if(mouse_control.left)
			mouse_control.fLeft = false;
		else
			mouse_control.fLeft = true;
		
		mouse_control.left = true;
	}
	else if(mouse_control.left){
		mouse_control.fLeft = false;
		mouse_control.left = false;
	}
	printf("\t\t\t\t\t\t%d", ms);

}

node newNode(game_object go){
    node n = (node)malloc(sizeof(node)); 
	n->v = go;
    n->next = NULL; 
	return n;
}

node addLastNode(node r, game_object go) 
{ 
    r->next = newNode(go);
    //r->next->p = r; 
    return r->next; 
} 
node setNode(node r, game_object go){
	r = newNode(go);
	return r;
}

node root;

void setup() {
	root = newNode((game_object){0,0,0,0});

	addLastNode(root,(game_object){
		100,100,150,75
	});
	addLastNode(root->next,(game_object){
		100,300,150,75
	});
	
	srand(time(NULL));//random
}

int last_frame_time = 0;
float delta_time = 0;
void update() {
	//En son update yaptığından bu yana FRAME_TARGET_TIME kadar zaman geçmesini bekle
	//while(!SDL_TICKS_PASSED(SDL_GetTicks(),last_frame_time + FRAME_TARGET_TIME));
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);//bir fps e göresaniyede beklemesi gereken tik sayısını hesaplar
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) SDL_Delay(time_to_wait);
	//En son update yaptığından bu yana ne kadar zaman geçti(FRAME_TARGET_TIME kadar)
	//delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
	last_frame_time = SDL_GetTicks();
		
    node temp = root; 
	
	printf("\r left:%d  FLeft:%d sid: %d \t", (int)mouse_control.left, (int)mouse_control.fLeft, selectedBox.status);
	if(mouse_control.fLeft){//bir yere itıklandığı anda
		mouse_control.fLeft = false;
		do
		{
			temp = temp->next;
			printf("as");
			fflush(stdout);
			if(CollPointRect( temp->v, mouse_control.x,mouse_control.y)){
				if(selectedBox.status == -1){//ilk secme
					selectedBox.status = 1;
					selectedBox.selected = temp;
					selectedBox.x = temp->v.x - mouse_control.x;
					selectedBox.y = temp->v.y - mouse_control.y;
				}
				break;
			}

		}while (temp->next != NULL);//hepsini konturol ed
	}
	if(mouse_control.left){//basılı tutma
		if(selectedBox.status >= 0){//secili
			selectedBox.selected->v.x = mouse_control.x + selectedBox.x;
			selectedBox.selected->v.y = mouse_control.y + selectedBox.y;
		}
	}
	else if(selectedBox.status != -1){//secim bırakıldı
		selectedBox.status = -1;
	}
	
}

void render() {
	SDL_SetRenderDrawColor(renderer, 30, 27, 25, 20);
	SDL_RenderClear(renderer);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	
    node temp = root; 
	do
	{
		temp = temp->next;
		SDL_RenderFillRect(renderer, objectToRect(temp->v));
	}while (temp->next != NULL);
	

	SDL_RenderPresent(renderer);
}



int main(int argc, char* args[]) {
	#ifdef __linux__ 
		printf("\e[?25l");//hiden corsor
	#elif _WIN32
		// windows 
	#endif

	game_is_running = initialize_window();
	printf("game_is_running : %d\n", game_is_running);

	//Game Loop
	setup();
	
	SDL_Delay(1000);
	while (game_is_running) {

		process_input();
		update();
		render();

		fflush(stdout);
		
	}

	destroy_window();

	printf("Game is stoping...\n");

	#ifdef __linux__ 
		printf("\e[?25h");//show corsor
	#elif _WIN32
		// windows 
	#endif
	return 0;
}

