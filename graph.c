#include <SDL2/SDL.h>
#include <math.h>
int screenWidth = 0;
int screenHeight = 0;
typedef struct Boid
{
    double x;
    double y;
    double vx;
    double vy;
} Boid;

void updateBoids(Boid *boids, int numBoids, double scaleFactor)
{
    for (int i = 0; i < numBoids; i++)
    {
        for (int j = i + 1; j < numBoids; j++)
        {
            double dx = boids[i].x - boids[j].x;
            double dy = boids[i].y - boids[j].y;
            double distance = sqrt(dx * dx + dy * dy);
            if (distance < 10)
            { // 20 is the repulsion distance
                double angle = atan2(dy, dx);
                double speed_i = sqrt(boids[i].vx * boids[i].vx + boids[i].vy * boids[i].vy);
                double speed_j = sqrt(boids[j].vx * boids[j].vx + boids[j].vy * boids[j].vy);
                boids[i].vx += 0.1 * cos(angle);
                boids[i].vy += 0.1 * sin(angle);
                boids[j].vx -= 0.1 * cos(angle);
                boids[j].vy -= 0.1 * sin(angle);
                double new_speed_i = sqrt(boids[i].vx * boids[i].vx + boids[i].vy * boids[i].vy);
                double new_speed_j = sqrt(boids[j].vx * boids[j].vx + boids[j].vy * boids[j].vy);
                boids[i].vx = boids[i].vx * (speed_i / new_speed_i);
                boids[i].vy = boids[i].vy * (speed_i / new_speed_i);
                boids[j].vx = boids[j].vx * (speed_j / new_speed_j);
                boids[j].vy = boids[j].vy * (speed_j / new_speed_j);
            }
        }
    }
    for (int i = 0; i < numBoids; i++)
    {
        boids[i].x += boids[i].vx;
        boids[i].y += boids[i].vy;
        //COLLISION WITH WALL TELEPORTS
        double	overshot;
		if (boids[i].x > screenWidth/ scaleFactor)
		{
            overshot = boids[i].x - screenWidth/ scaleFactor;
			boids[i].x = overshot;
			boids[i].y = screenHeight/ scaleFactor - boids[i].y;
        }
        if (boids[i].x < 0)
        {
            overshot = 0 - boids[i].x;
            boids[i].x = screenWidth/ scaleFactor - overshot;
            boids[i].y = screenHeight/ scaleFactor - boids[i].y;
        }
        if (boids[i].y > screenHeight/ scaleFactor)
        {
            overshot = boids[i].y - screenHeight/ scaleFactor;
            boids[i].y = overshot;
            boids[i].x = screenWidth/ scaleFactor - boids[i].x;
        }
        if (boids[i].y < 0)
        {
            overshot = 0 - boids[i].y;
            boids[i].y = screenHeight/ scaleFactor - overshot;
            boids[i].x = screenWidth/ scaleFactor - boids[i].x;
        }

    }
}

void addRandomnessToMovement(Boid *boids, int numBoids, double maxRandomVelocity)
{
    for (int i = 0; i < numBoids; i++)
    {
        double randomVelocityX = ((double)rand() / (double)RAND_MAX) * maxRandomVelocity * 2 - maxRandomVelocity;
        double randomVelocityY = ((double)rand() / (double)RAND_MAX) * maxRandomVelocity * 2 - maxRandomVelocity;
        boids[i].vx += randomVelocityX;
        boids[i].vy += randomVelocityY;
    }
}

void rotateTowardsMouseWithSpeed(Boid *boids, int numBoids, double mouseX, double mouseY, double maxSpeed)
{
    for (int i = 0; i < numBoids; i++)
    {
        double angle = atan2(mouseY - boids[i].y, mouseX - boids[i].x);
        // Calculate the distance between the boid and the mouse
        double distance = sqrt(pow(boids[i].x - mouseX, 2) + pow(boids[i].y - mouseY, 2));
        // Set the velocity vector using the angle, distance, and maxSpeed
        boids[i].vx = (distance / maxSpeed) * cos(angle);
        boids[i].vy = (distance / maxSpeed) * sin(angle);
    }
}

float map(float value, float min1, float max1, float min2, float max2)
{
    return (value - min1) / (max1 - min1) * (max2 - min2) + min2;
}

void HSVtoRGB(int *r, int *g, int *b, float h, float s, float v)
{
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;

    if (h < 60)
    {
        *r = (c + m) * 255;
        *g = (x + m) * 255;
        *b = m * 255;
    }
    else if (h < 120)
    {
        *r = (x + m) * 255;
        *g = (c + m) * 255;
        *b = m * 255;
    }
    else if (h < 180)
    {
        *r = m * 255;
        *g = (c + m) * 255;
        *b = (x + m) * 255;
    }
    else if (h < 240)
    {
        *r = m * 255;
        *g = (x + m) * 255;
        *b = (c + m) * 255;
    }
    else if (h < 300)
    {
        *r = (x + m) * 255;
        *g = m * 255;
        *b = (c + m) * 255;
    }
    else
    {
        *r = (c + m) * 255;
        *g = m * 255;
        *b = (x + m) * 255;
    }
}

void setBoidColorBySpeedo(Boid *boid, int minSpeed, int maxSpeed, SDL_Renderer *renderer, int time, int not)
{
    // Calculate the speed magnitude
    float speed = sqrt(boid->vx * boid->vx + boid->vy * boid->vy);

    // Map the speed to a hue value between 0 and 360 degrees
    float hue = map(speed, minSpeed, maxSpeed, 0, 360);

    // Set the saturation and value to a constant value
    float saturation = 1.0f;
    float value = 1.0f;

    // Convert the HSV color to RGB
    int r, g, b;
    HSVtoRGB(&r, &g, &b, (int)(hue + time) % 360, saturation, value);

    // Set the renderer color to the RGB value
    if (not)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    else
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

void moveTowardsFlockCenter(Boid *boids, int numBoids, double maxSpeed)
{
    double centerX = 0;
    double centerY = 0;
    for (int i = 0; i < numBoids; i++)
    {
        centerX += boids[i].x;
        centerY += boids[i].y;
    }
    centerX /= numBoids;
    centerY /= numBoids;
    for (int i = 0; i < numBoids; i++)
    {
        double angle = atan2(centerY - boids[i].y, centerX - boids[i].x);
        double vx = maxSpeed * cos(angle);
        double vy = maxSpeed * sin(angle);
        double speed = sqrt(boids[i].vx * boids[i].vx + boids[i].vy * boids[i].vy);
        if (speed + maxSpeed < speed)
        {
            boids[i].vx += vx * 2;
            boids[i].vy += vy * 2;
        }
    }
}

void drawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    int diameter = (radius * 2);

    int x_c = (radius - 1);
    int y_c = 0;
    int tx = 1;
    int ty = 1;
    int error = (tx - diameter);

    while (x_c >= y_c)
    {
        for (int i = x - x_c; i <= x + x_c; i++)
        {
            SDL_RenderDrawPoint(renderer, i, y - y_c);
            SDL_RenderDrawPoint(renderer, i, y + y_c);
        }

        for (int i = x - y_c; i <= x + y_c; i++)
        {
            SDL_RenderDrawPoint(renderer, i, y - x_c);
            SDL_RenderDrawPoint(renderer, i, y + x_c);
        }

        if (error <= 0)
        {
            ++y_c;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x_c;
            tx += 2;
            error += (tx - diameter);
        }
    }
}



int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    SDL_Init(SDL_INIT_VIDEO);

    // get the current display mode
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    // create the window and renderer
    SDL_Window *window = SDL_CreateWindow("Example",0,0,DM.w,DM.h,0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
      SDL_Surface *screenSurface = SDL_CreateRGBSurface(0, DM.w, DM.h, 32, 0, 0, 0, 0); // create a surface to hold the contents of the screen
    screenHeight = DM.h;
    screenWidth = DM.w;
    
    const int numBoidsmax = 1500	;
    int numBoids = 1;
    Boid boids[numBoidsmax];
    double mouse_attraction = 20;
    int s_mouseX, s_mouseY;
    double mouseX, mouseY;
    int time = 0;
    int not = 0;
    int circle = 0;
    int grid = 0;
    const int defaultScaleFactor = 6;
    double scaleFactor = defaultScaleFactor;
    double oldScaleFactor = defaultScaleFactor;
    int random_speed = 0;
    for (int i = 0; i < numBoidsmax; i++)
    {
        boids[i].x = rand() % screenWidth / scaleFactor;
        boids[i].y = rand() % screenHeight / scaleFactor;
        boids[i].vx = 0;
        boids[i].vy = 0;
    }
    SDL_FillRect(screenSurface, NULL, 0);
    while (1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                goto exit;
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                if (event.wheel.y > 0)
                { // mouse wheel moved up
                    oldScaleFactor = scaleFactor;
                    scaleFactor+= 0.1;
                    if (scaleFactor > 200)
                    {
                        scaleFactor = 199;
                    }
                    for (int i = 0; i < numBoidsmax; i++)
                    {
                        double newX = map(boids[i].x, 0, screenWidth / oldScaleFactor, 0, screenWidth / scaleFactor);
                        double newY = map(boids[i].y, 0, screenHeight / oldScaleFactor, 0, screenHeight / scaleFactor);
                        boids[i].x = newX;
                        boids[i].y = newY;
                    }
                }
                else if (event.wheel.y < 0)
                { // mouse wheel moved down
                    oldScaleFactor = scaleFactor;
                    scaleFactor-= 0.1;
                    if (scaleFactor < 1)
                    {
                        scaleFactor = 1;
                    }
                    for (int i = 0; i < numBoids; i++)
                    {
                        double newX = map(boids[i].x, 0, screenWidth / oldScaleFactor, 0, screenWidth / scaleFactor);
                        double newY = map(boids[i].y, 0, screenHeight / oldScaleFactor, 0, screenHeight / scaleFactor);
                        boids[i].x = newX;
                        boids[i].y = newY;
                    }
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    goto exit;
                }
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    for (int i = 0; i < numBoidsmax; i++)
                    {
                        boids[i].x = rand() % screenWidth / scaleFactor;
                        boids[i].y = rand() % screenHeight / scaleFactor;
                        boids[i].vx = 0;
                        boids[i].vy = 0;
                    }
                }
                else if (event.key.keysym.sym == SDLK_UP)
                {
                    numBoids += numBoids / 2 + 1;
                    if (numBoids > numBoidsmax)
                    {
                        numBoids = numBoidsmax;
                    }
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    numBoids -= numBoids / 2 + 1;
                    if (numBoids < 1)
                    {
                        numBoids = 1;
                    }
                }
                else if (event.key.keysym.sym == SDLK_LEFT)
                {
                    { // mouse wheel moved down
                        random_speed--;
                        if (random_speed < 1)
                        {
                            random_speed = 0;
                        }
                    }
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    random_speed++;
                    if (random_speed > 8)
                    {
                        random_speed = 10;
                    }
                }
                else if (event.key.keysym.sym == SDLK_r)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderClear(renderer);
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                }
                else if (event.key.keysym.sym == SDLK_n)
                {
                    not = !not;
                }
                else if (event.key.keysym.sym == SDLK_g)
                {
                    grid = !grid;
                }
                else if (event.key.keysym.sym == SDLK_m)
                {
                    mouse_attraction += 5;
                    if (mouse_attraction > 5000)
                    {
                        mouse_attraction = 5000;
                    }
                }
                else if (event.key.keysym.sym == SDLK_l)
                {
                    mouse_attraction -= 5;
                    if (mouse_attraction < 1)
                    {
                        mouse_attraction = 1;
                    }
                }
                else if (event.key.keysym.sym == SDLK_c)
                {
                    circle = !circle;
                }
            }
        }
        SDL_GetMouseState(&s_mouseX, &s_mouseY);
        mouseX = s_mouseX / scaleFactor;
        mouseY = s_mouseY / scaleFactor;
         for (int i = 0; i < numBoids; i++)
        {
            // setBoidColorBySpeed(&boids[i], renderer);
            double coordx = boids[i].x;
            double coordy = boids[i].y;
            if (grid)
            {
                //boids[i].x = (int)boids[i].x;
                //boids[i].y = (int)boids[i].y;
                coordx = round(boids[i].x);
                coordy = round(boids[i].y);
            }
            
            setBoidColorBySpeedo(&boids[i], 0, 10, renderer, time, not);
            if (circle)
                drawCircle(renderer, coordx * scaleFactor + scaleFactor / 2, coordy * scaleFactor+ scaleFactor / 2, scaleFactor / 2);
            else
{            SDL_Rect rect = {coordx * scaleFactor, coordy * scaleFactor, scaleFactor, scaleFactor}; // adjust the size of the rectangle as needed
            SDL_RenderFillRect(renderer, &rect);}
        }
        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenSurface->pixels, screenSurface->pitch); // copy the contents of the renderer to the surface
        rotateTowardsMouseWithSpeed(boids, numBoids, mouseX, mouseY, mouse_attraction);
        addRandomnessToMovement(boids, numBoids, random_speed);
        updateBoids(boids, numBoids, scaleFactor);
        SDL_Texture* screenTexture = SDL_CreateTextureFromSurface(renderer, screenSurface); // create a texture from the surface
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL); // render the texture to the screen
        SDL_DestroyTexture(screenTexture); // destroy the texture
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
         time += 5;
    }
exit:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
