#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Application.h"

// SDL_RunApp이 호출할 메인 콜백. 
int SDLCALL appCallback(int argc, char* argv[]) {
    Application application;

    if(application.init() != 0){
        SDL_Log("Application initialize failed: %s", SDL_GetError());
        return -1;
    }
    
    application.run(); /* main loop */

    application.quit();
    
    return 0;
}

int main(int argc, char* argv[]) {
    std::cout << "main func is called. " << std::endl;
    const int result = SDL_RunApp(argc, argv, appCallback, nullptr);

    if (result != 0 ) {
        SDL_Log("Application exited with errors : %s", SDL_GetError());
    }
    else {
        std::cout << "Application exited successfully!" << std::endl;
    }

    return result;
}

