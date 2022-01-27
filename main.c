#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

#define GETCONTEXT  ((struct loopContext*)arg)

SDL_Texture* loadImage(const char* fName, SDL_Renderer* renderer, SDL_Rect* imgprops);

struct loopContext {
    SDL_Renderer* renderer;
    SDL_Texture* backImg;
    SDL_Rect bakPos;
    SDL_Texture* texImg;
    SDL_Rect textProp;
    SDL_Texture* starImg;
    SDL_Rect starProp;
    uint8_t quit;
};

struct starInfo {
    int starC1;
    int starC2;
    int starOffset;
};

void calculateStarPos(SDL_Rect* output, int thetaStar, int starC1, int starC2, int starOffset) {
    output->y = (thetaStar-starC1)*(thetaStar-starC1)/30+(310-(starC1*starC1/30));
    output->x = starOffset - (thetaStar*starC2);
}

void calculateFlash(SDL_Texture* texImg, int theta) {
    int out = ((theta-23)*(theta-23)/2);
    if (out > 255)
        out = 255;
    if (out < 0)
        out = 0;
   SDL_SetTextureColorMod(texImg, out, out, out);
}

void loop(void *arg) {
    uint64_t currTick = SDL_GetTicks();
    SDL_Rect tmpBakPos = GETCONTEXT->bakPos;

    static uint8_t starsSpawned = 0;
    static uint8_t starState = 0;
    static int theta = 0;
    static int thetaStar = 1000; //lazy hack to have the stars invisible at the start
    static uint64_t timerTick = 0;
    static uint64_t timerTickText = 0;
    static uint64_t timerStar = 0;
    static struct starInfo si[4];


    if(currTick > timerTick) {
        GETCONTEXT->bakPos.x = GETCONTEXT->bakPos.x + 1;
        GETCONTEXT->bakPos.y = GETCONTEXT->bakPos.y + 1;
        timerTick = currTick + 3;
    }
    if(currTick > timerTickText) {
        theta += 1;
        thetaStar += 1;
        //(abs(cos(theta))*10);
        timerTickText = currTick + 10;
    }
    //textProp.y = textMidPoint + abs(cos(theta)*1000);
    double inside = theta - 92.443;
    double textypos = ((inside*inside*inside)/2000) + 205;
    
    if (GETCONTEXT->bakPos.x >= 800) {
        GETCONTEXT->bakPos.x = 0;
    }
    if (GETCONTEXT->bakPos.y >= 600) {
        GETCONTEXT->bakPos.y = 0;
    }

    if (theta >= 70 && !starsSpawned) {
        GETCONTEXT->starProp.y = 310;
        GETCONTEXT->starProp.x = 390;
        int i;
        for (i = 0; i <= 3; i++) {
            si[i].starC1 = rand()%50 + 20;
            si[i].starC2 = rand()%4 - 2;
            si[i].starOffset = rand()%150 + 100 + (i*100);
        }
        //SDL_SetTextureColorMod(GETCONTEXT->texImg, 0, 0, 0);
        thetaStar = 0;
        starsSpawned = 1;

    }
    //GETCONTEXT->starProp.y = (thetaStar-starC1)*(thetaStar-starC1)/30+280;
    //GETCONTEXT->starProp.x = starOffset - (thetaStar*starC2);

    
    if (theta > 184) {
        //SDL_SetTextureColorMod(GETCONTEXT->texImg, 255, 255, 255);
        starsSpawned = 0;
        theta=0;
    }

    GETCONTEXT->textProp.y = textypos;
    //render first
    SDL_RenderCopy(GETCONTEXT->renderer, GETCONTEXT->backImg, NULL, &(GETCONTEXT->bakPos));
    //render second
    tmpBakPos = GETCONTEXT->bakPos;
    tmpBakPos.x = tmpBakPos.x - 800;
    SDL_RenderCopy(GETCONTEXT->renderer, GETCONTEXT->backImg, NULL, &tmpBakPos);
    //render third
    tmpBakPos.y = tmpBakPos.y - 600;
    SDL_RenderCopy(GETCONTEXT->renderer, GETCONTEXT->backImg, NULL, &tmpBakPos);
    //render fourth
    tmpBakPos.x = tmpBakPos.x + 800;
    SDL_RenderCopy(GETCONTEXT->renderer, GETCONTEXT->backImg, NULL, &tmpBakPos);

    //render text
    calculateFlash(GETCONTEXT->texImg, thetaStar);
    SDL_RenderCopy(GETCONTEXT->renderer, GETCONTEXT->texImg, NULL, &(GETCONTEXT->textProp));

    //render star
    SDL_Rect starSelect;
    if(currTick > timerStar) {
        starState = !starState;
        timerStar = currTick + 300;
    }
    if (starState) {
        starSelect.x = 0;
    } else {
        starSelect.x = 20;
    }
    starSelect.y = 0;
    starSelect.w = GETCONTEXT->starProp.w /5;
    starSelect.h = GETCONTEXT->starProp.h / 5;
    int i;
    for (i = 0; i <= 3; i++) {
        calculateStarPos(&(GETCONTEXT->starProp), thetaStar, si[i].starC1, si[i].starC2, si[i].starOffset);
        SDL_RenderCopy(GETCONTEXT->renderer, GETCONTEXT->starImg, &starSelect, &(GETCONTEXT->starProp));
    }
    //SDL_RenderCopy(GETCONTEXT->renderer, GETCONTEXT->starImg, &starSelect, &(GETCONTEXT->starProp));

    SDL_RenderPresent(GETCONTEXT->renderer);

    SDL_Event ev;
    while( SDL_PollEvent( &ev ) != 0 ) {
        if ( ev.type == SDL_QUIT ) {
            GETCONTEXT->quit = SDL_TRUE;
            #ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
            #endif
        }
    }
    SDL_Delay(1);
}

int main(int argc, char *argv[]) {
    if ( SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init Error:, %s\n", SDL_GetError());
        return 1;
    }

    struct loopContext context;

    SDL_Window* window = NULL;
    //SDL_Renderer* renderer = NULL;
    context.renderer = NULL;
    if ( SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY, &window, &(context.renderer)) ) {
        printf("SDL Create Window Error: %s\n", SDL_GetError());
        return 1;
    }
    context.backImg = loadImage("techdemo.bmp", context.renderer, NULL);

    //SDL_Rect textProp;
    context.texImg = loadImage("techdemo_text.bmp", context.renderer, &(context.textProp));
    SDL_SetTextureColorMod(context.texImg, 0, 0, 0);
    context.textProp.w = context.textProp.w*5;
    context.textProp.h = context.textProp.h*5;
    context.textProp.x = (SCREEN_WIDTH - context.textProp.w)/2;
    int textMidPoint = (SCREEN_HEIGHT - context.textProp.h)/2;
    //textProp.y = textMidPoint;
    context.textProp.y = 0;

    context.starImg = loadImage("fendemo_star.bmp", context.renderer, &(context.starProp));
    context.starProp.w = context.starProp.w * 5 / 2;
    context.starProp.h = context.starProp.h * 5;

    
    context.quit = SDL_FALSE;

    context.bakPos.x = 0;
    context.bakPos.y = 0;
    context.bakPos.w = 800;
    context.bakPos.h = 600;

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(loop, &context, -1, 1);
    #else
    while (context.quit == SDL_FALSE) {
        loop(&context);
    }
    #endif
    SDL_Quit();

    return 0;

}

SDL_Texture* loadImage(const char* fName, SDL_Renderer* renderer, SDL_Rect* imgprops) {
    SDL_Surface* img;
    img = SDL_LoadBMP( fName );
    if(!img) {
        printf("Error loading image: %s  SDL Error: %s\n", fName, SDL_GetError());
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, img);
    if(imgprops) {
        imgprops->h = img->h;
        imgprops->w = img->w;
    }
    SDL_FreeSurface(img);
    return tex;
}