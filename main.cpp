#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <cstdlib>

#define WINDOW_WIDTH 1100
#define WINDOW_HEIGHT 800
#define SQUARE_SIDE 20

using namespace std;

bool manualInput;

struct Kvadrat {
    int x;
    int y;
    int h;
    int w;
    Kvadrat *next = nullptr;
};

void getSquare(Kvadrat *sqr) {
    sqr->h = SQUARE_SIDE;
    sqr->w = SQUARE_SIDE;

    do {
        cout << "Vnesi koordinato x: ";
        cin >> sqr->x;

        cout << "Vnesi koordinato y: ";
        cin >> sqr->y;
    } while ((sqr->x < 0 || sqr->x) > WINDOW_WIDTH && (sqr->y < 0 || sqr->y > WINDOW_HEIGHT));
}

void randomSquare(Kvadrat *sqr) {
    sqr->h = SQUARE_SIDE;
    sqr->w = SQUARE_SIDE;

    sqr->x = rand()% 1079;
    sqr->y = rand()% 779;

    cout << "Input successful!\n";
}

bool askInputType() {
    bool switchActive = true;
    while (switchActive) {
        cout << "Would you like:\n [0] Manual Input \n [1] Automatic input\n Enter option: ";
        int input;
        cin >> input;
        switch(input) {
            case 0:
                return true;
                break;
            case 1: 
                return false;
                break;
            default:
                cout << "I'm sorry but " << input << " is not an option.\n\n";
        }
    }
    return true;
}

void outputChain(Kvadrat *head) {
    int i = 0;

    while (head->next) {
        cout << i++ << ". " << head->x << head->y << flush;
        head = head->next;
    }
}

void cleanChain(Kvadrat *head) {
    while (head->next) {
        Kvadrat *temp = head;
        head = head->next;
        delete temp;
    }
}

Kvadrat *unorderedChain(unsigned count) {
    Kvadrat *mov = new Kvadrat;
    Kvadrat *rtn = mov;

    while (count--) {
        manualInput ? getSquare(mov) : randomSquare(mov);
        mov->next = new Kvadrat;
        mov = mov->next;
    }
    return rtn;
}

Kvadrat *orderedChain(int ctr, bool (*cmp)(float a, float b)) {
    Kvadrat *mov = new Kvadrat;
    Kvadrat *rtn = mov;
    while (ctr--) {
        manualInput ? getSquare(mov) : randomSquare(mov);
        Kvadrat *head = rtn;
        while (head != mov) {
            if (cmp((head->x + head->y) / 2.f, (mov->x + mov->y) / 2.f)) {
                int xtmp = head->x;
                int ytmp = head->y;

                head->x = mov->x;
                head->y = mov->y;

                mov->x = xtmp;
                mov->y = ytmp;
            }
            head = head->next;
        }
        mov->next = new Kvadrat;
        mov = mov->next;
    }
    return rtn;
}

void drawChain(SDL_Renderer *renderer, Kvadrat *sqr, int R, int G, int B) {
    while (sqr->next) {
        SDL_SetRenderDrawColor(renderer, R, G, B, 255);
        SDL_Rect rect;

        rect.x = sqr->x;
        rect.y = sqr->y;
        rect.h = sqr->h;
        rect.w = sqr->w;

        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

        sqr = sqr->next;
    }
}

bool onRect(int x, int y, Kvadrat *head) {
    return (x >= head->x && x <= head->x + head->w) && (y >= head->y && y <= head->y + head->h);
}

void delChain(int x, int y, Kvadrat **head) {
    Kvadrat *prev, *temp = *head;

    if (head && onRect(x, y, *head)) {
        *head = temp->next;
        delete temp;
        return;
    }

    while (temp && !onRect(x, y, temp)) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) return;

    prev->next = temp->next;

    delete temp;
}

int main(int argc, char *argv[]) {
    
    manualInput = askInputType();

    cout << "~~~ Neurejen ~~~\n";

    Kvadrat *unorderedHead = unorderedChain(10u);
    cout << "~~~ Urejen ~~~\n";

    Kvadrat *orderedHead = orderedChain(10u, [](float a, float b) { return a > b; });

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow(
        "RPA - ATA - Naloga",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0u
    );
    
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, 0u);

    bool programRunning = 1;
    while (programRunning) {
        SDL_Event e;

        while (SDL_PollEvent(&e) > 0) {
            switch (e.type) {
                case SDL_QUIT:
                    programRunning = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    int xmouse, ymouse;
                    SDL_GetMouseState(&xmouse, &ymouse);
                    delChain(xmouse, ymouse, &unorderedHead);
                    SDL_RenderClear(renderer);
                    break;
            }

            drawChain(renderer, unorderedHead, 255, 0, 0);
            drawChain(renderer, orderedHead, 0, 255, 0);
        }
    }

    cout << "~~~ NEUREJENI ~~~\n";
    outputChain(unorderedHead);

    cout << "~~~ UREJENI ~~~\n";
    outputChain(orderedHead);

    cleanChain(unorderedHead);
    cleanChain(orderedHead);
    return 0;
}