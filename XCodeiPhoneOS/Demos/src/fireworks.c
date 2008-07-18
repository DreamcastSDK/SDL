/*
 *	fireworks.c
 *	written by Holmes Futrell
 *	use however you want
 */

#include "SDL.h"
#include "math.h"
#include "common.h"
#include <math.h>
#include <time.h>
#include <OpenGLES/ES1/gl.h>

#define MILLESECONDS_PER_FRAME 16	/* about 60 frames per second */
#define ACCEL 0.0001f
#define WIND_RESISTANCE 0.00005f
#define MAX_PARTICLES 2000
#define VEL_BEFORE_EXPLODE 0.0f

SDL_TextureID flashTextureID;


enum particleType {
	emitter = 0,
	trail,
	dust
};

struct glformat {
	int SDL_GL_RED_SIZE;
	int SDL_GL_GREEN_SIZE;
	int SDL_GL_BLUE_SIZE;
	int SDL_GL_ALPHA_SIZE;
	int SDL_GL_BUFFER_SIZE;
	int SDL_GL_DOUBLEBUFFER;
	int SDL_GL_DEPTH_SIZE;
	int SDL_GL_STENCIL_SIZE;
	int SDL_GL_ACCUM_RED_SIZE;
	int SDL_GL_ACCUM_GREEN_SIZE;
	int SDL_GL_ACCUM_BLUE_SIZE;
	int SDL_GL_ACCUM_ALPHA_SIZE;
	int SDL_GL_STEREO;
	int SDL_GL_MULTISAMPLEBUFFERS;
	int SDL_GL_MULTISAMPLESAMPLES;
	int SDL_GL_ACCELERATED_VISUAL;
	int SDL_GL_RETAINED_BACKING;
};

struct particle {

	GLfloat x;
	GLfloat y;
	GLubyte color[4];
	GLfloat size;
	GLfloat xvel;
	GLfloat yvel;
	int isActive;
	enum particleType type;
	int framesSinceEmission;
} particles[MAX_PARTICLES];

void spawnParticleFromEmitter(struct particle *emitter);
void explodeEmitter(struct particle *emitter);

static int num_active_particles;

static void getError(const char *prefix)
{
    const char *error;
	
	GLenum result = glGetError();
	if (result == GL_NO_ERROR)
		return;
	
    switch (result) {
		case GL_NO_ERROR:
			error = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			error = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "GL_INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			error = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			error = "GL_STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			error = "GL_OUT_OF_MEMORY";
			break;
		default:
			error = "UNKNOWN";
			break;
    }
    printf("%s: %s\n", prefix, error);
}

void render(void) {
		
	/* draw the background */
	glClear(GL_COLOR_BUFFER_BIT);
	
	struct particle *slot = particles;
	struct particle *curr = particles;
	int i;
	for (i=0; i<num_active_particles; i++) {
		if (curr->isActive) {
			
			if (curr->y > SCREEN_HEIGHT) curr->isActive = 0;
			if (curr->y < 0) curr->isActive = 0;
			if (curr->x > SCREEN_WIDTH) curr->isActive = 0;
			if (curr->x < 0) curr->isActive = 0;

			curr->yvel += ACCEL * MILLESECONDS_PER_FRAME;
			curr->xvel += 0.0f;
			curr->y += curr->yvel * MILLESECONDS_PER_FRAME;
			curr->x += curr->xvel * MILLESECONDS_PER_FRAME;
			
			if (curr->type == emitter) {
				spawnParticleFromEmitter(curr);
				curr->framesSinceEmission = 0;
				if (curr->yvel > -VEL_BEFORE_EXPLODE) {
					explodeEmitter(curr);
				}
				curr->framesSinceEmission++;
			}
			else {
				
				float speed = sqrt(curr->xvel*curr->xvel + curr->yvel*curr->yvel);
				
				if (WIND_RESISTANCE * MILLESECONDS_PER_FRAME < speed) {
					float normx = curr->xvel / speed;
					float normy = curr->yvel / speed;
					curr->xvel -= normx * WIND_RESISTANCE * MILLESECONDS_PER_FRAME;
					curr->yvel -= normy * WIND_RESISTANCE * MILLESECONDS_PER_FRAME;
				}
				else {
					curr->xvel = 0;
					curr->yvel = 0;
				}
				
				if (curr->color[3] <= MILLESECONDS_PER_FRAME * 0.0005f * 255) {
					curr->isActive = 0;

				}
				else {
					curr->color[3] -= MILLESECONDS_PER_FRAME * 0.0005f * 255;
				}
				
				if (curr->type == dust)
					curr->size -= MILLESECONDS_PER_FRAME * 0.010f;
				
			}
			
			*(slot++) = *curr;
		}
		curr++;
	}
	num_active_particles = slot - particles;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, 1);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(struct particle), particles + 0);
	getError("vertices");
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct particle), particles[0].color);
	getError("colors");

	glEnableClientState(GL_POINT_SIZE_ARRAY_OES);
	getError("enable client state");
	glPointSizePointerOES(GL_FLOAT, sizeof(struct particle), &(particles[0].size));	
	getError("point size");

	glEnable(GL_POINT_SPRITE_OES);
	glDrawArrays(GL_POINTS, 0, num_active_particles);	
	getError("glDrawArrays");

		
	/* update screen */
	SDL_RenderPresent();
	
}

void printOpenGLAttributes(struct glformat *format) {
	printf("\tSDL_GL_RED_SIZE = %d\n", format->SDL_GL_RED_SIZE);
	printf("\tSDL_GL_GREEN_SIZE = %d\n", format->SDL_GL_GREEN_SIZE);
	printf("\tSDL_GL_BLUE_SIZE = %d\n", format->SDL_GL_BLUE_SIZE);
	printf("\tSDL_GL_ALPHA_SIZE = %d\n", format->SDL_GL_ALPHA_SIZE);
	printf("\tSDL_GL_BUFFER_SIZE = %d\n", format->SDL_GL_BUFFER_SIZE);
	printf("\tSDL_GL_DOUBLEBUFFER = %d\n", format->SDL_GL_DOUBLEBUFFER);
	printf("\tSDL_GL_DEPTH_SIZE = %d\n", format->SDL_GL_DEPTH_SIZE);
	printf("\tSDL_GL_STENCIL_SIZE = %d\n", format->SDL_GL_STENCIL_SIZE);
	printf("\tSDL_GL_ACCUM_RED_SIZE = %d\n", format->SDL_GL_ACCUM_RED_SIZE);
	printf("\tSDL_GL_ACCUM_GREEN_SIZE = %d\n", format->SDL_GL_ACCUM_GREEN_SIZE);
	printf("\tSDL_GL_ACCUM_BLUE_SIZE = %d\n", format->SDL_GL_ACCUM_BLUE_SIZE);
	printf("\tSDL_GL_ACCUM_ALPHA_SIZE = %d\n", format->SDL_GL_ACCUM_ALPHA_SIZE);
	printf("\tSDL_GL_STEREO = %d\n", format->SDL_GL_STEREO);
	printf("\tSDL_GL_MULTISAMPLEBUFFERS = %d\n", format->SDL_GL_MULTISAMPLEBUFFERS);
	printf("\tSDL_GL_MULTISAMPLESAMPLES = %d\n", format->SDL_GL_MULTISAMPLESAMPLES);
	printf("\tSDL_GL_ACCELERATED_VISUAL = %d\n", format->SDL_GL_ACCELERATED_VISUAL);
	printf("\tSDL_GL_RETAINED_BACKING = %d\n", format->SDL_GL_RETAINED_BACKING);	
}

void setOpenGLAttributes(struct glformat *format) {
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, format->SDL_GL_RED_SIZE);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, format->SDL_GL_GREEN_SIZE);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, format->SDL_GL_BLUE_SIZE);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, format->SDL_GL_ALPHA_SIZE);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, format->SDL_GL_BUFFER_SIZE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, format->SDL_GL_DOUBLEBUFFER);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, format->SDL_GL_DEPTH_SIZE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, format->SDL_GL_STENCIL_SIZE);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, format->SDL_GL_ACCUM_RED_SIZE);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, format->SDL_GL_ACCUM_GREEN_SIZE);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, format->SDL_GL_ACCUM_BLUE_SIZE);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, format->SDL_GL_ACCUM_ALPHA_SIZE);
	SDL_GL_SetAttribute(SDL_GL_STEREO, format->SDL_GL_STEREO);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, format->SDL_GL_MULTISAMPLEBUFFERS);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, format->SDL_GL_MULTISAMPLESAMPLES);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, format->SDL_GL_ACCELERATED_VISUAL);
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, format->SDL_GL_RETAINED_BACKING);
}

void getOpenGLAttributes(struct glformat *format) {
		
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &(format->SDL_GL_RED_SIZE));
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &(format->SDL_GL_GREEN_SIZE));
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &(format->SDL_GL_BLUE_SIZE));
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &(format->SDL_GL_ALPHA_SIZE));
	SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &(format->SDL_GL_BUFFER_SIZE));
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &(format->SDL_GL_DOUBLEBUFFER));
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &(format->SDL_GL_DEPTH_SIZE));
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &(format->SDL_GL_STENCIL_SIZE));
	SDL_GL_GetAttribute(SDL_GL_ACCUM_RED_SIZE, &(format->SDL_GL_ACCUM_RED_SIZE));
	SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE, &(format->SDL_GL_ACCUM_GREEN_SIZE));
	SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE, &(format->SDL_GL_ACCUM_BLUE_SIZE));
	SDL_GL_GetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, &(format->SDL_GL_ACCUM_ALPHA_SIZE));
	SDL_GL_GetAttribute(SDL_GL_STEREO, &(format->SDL_GL_STEREO));
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &(format->SDL_GL_MULTISAMPLEBUFFERS));
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &(format->SDL_GL_MULTISAMPLESAMPLES));
	SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &(format->SDL_GL_ACCELERATED_VISUAL));
	SDL_GL_GetAttribute(SDL_GL_RETAINED_BACKING, &(format->SDL_GL_RETAINED_BACKING));
}

void explodeEmitter(struct particle *emitter) {

	emitter->isActive = 0;
	
	int i;
	for (i=0; i<200; i++) {
		
		if (num_active_particles >= MAX_PARTICLES) return;
		
		float theta = randomFloat(0, 2.0f * 3.141592);
		float max = 3.0f;
		float speed = randomFloat(0.00, powf(0.17, max));
		speed = powf(speed, 1.0f / max);
		
		struct particle *p = &particles[num_active_particles];
		p->xvel = speed * cos(theta);
		p->yvel = speed * sin(theta);
		p->x = emitter->x + emitter->xvel;
		p->y = emitter->y + emitter->yvel;
		p->isActive = 1;
		p->type = dust;
		p->color[0] = emitter->color[0];
		p->color[1] = emitter->color[1];
		p->color[2] = emitter->color[2];
		p->color[3] = 255;

		p->size = 15;
		
		num_active_particles++;
		
	}
	
}

void spawnParticleFromEmitter(struct particle *emitter) {
	
	if (num_active_particles >= MAX_PARTICLES) return;
	
	struct particle *p = &particles[num_active_particles];
	p->x = emitter->x + randomFloat(-3.0, 3.0);
	p->y = emitter->y + emitter->size / 2.0f;
	p->xvel = emitter->xvel + randomFloat(-0.005, 0.005);
	p->yvel = emitter->yvel + 0.1;
	p->color[0] = (0.8f + randomFloat(-0.1, 0.0)) * 255;
	p->color[1] = (0.4f + randomFloat(-0.1, 0.1)) * 255;
	p->color[2] = (0.0f + randomFloat(0.0, 0.2)) * 255;
	p->color[3] = (0.7f) * 255;
	p->size = 10;
	p->type = trail;
	p->isActive = 1;
	num_active_particles++;
	
}
	
void spawnEmitterParticle(int x, int y) {

	if (num_active_particles >= MAX_PARTICLES) return;
	
	struct particle *p = &particles[num_active_particles];
	p->x = x;
	p->y = SCREEN_HEIGHT;
	p->xvel = 0;
	p->yvel = -sqrt(2*ACCEL*(SCREEN_HEIGHT-y) + VEL_BEFORE_EXPLODE * VEL_BEFORE_EXPLODE);
	p->color[0] = 1.0 * 255;
	p->color[1] = 0.4 * 255;
	p->color[2] = 0.4 * 255;
	p->color[3] = 1.0f * 255;
	p->size = 10;
	p->type = emitter;
	p->framesSinceEmission = 0;
	p->isActive = 1;
	num_active_particles++;
}

void initializeParticles(void) {
	
	num_active_particles = 0;
	
}

/*
 loads the brush texture
 */
void initializeTexture() {
	SDL_Surface *bmp_surface;
	bmp_surface = SDL_LoadBMP("stroke.bmp");
	if (bmp_surface == NULL) {
		fatalError("could not load stroke.bmp");
	}
	flashTextureID = SDL_CreateTextureFromSurface(SDL_PIXELFORMAT_ABGR8888, bmp_surface);
	SDL_FreeSurface(bmp_surface);
	if (flashTextureID == 0) {
		fatalError("could not create brush texture");
	}
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

int main(int argc, char *argv[]) {
	
	SDL_WindowID windowID;	/* ID of main window */
	Uint32 startFrame;		/* time frame began to process */
	Uint32 endFrame;		/* time frame ended processing */
	Uint32 delay;			/* time to pause waiting to draw next frame */
	int done;				/* should we clean up and exit? */
	
	struct glformat requested, obtained;
	
	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fatalError("Could not initialize SDL");
	}
	
	srand(time(NULL));
	
	SDL_GL_LoadLibrary(NULL);
		
	SDL_memset(&requested, 0, sizeof(requested));
	requested.SDL_GL_RED_SIZE = 5;
	requested.SDL_GL_GREEN_SIZE = 6; 
	requested.SDL_GL_BLUE_SIZE = 5;
	requested.SDL_GL_ALPHA_SIZE = 0;
	requested.SDL_GL_DEPTH_SIZE = 0;
	requested.SDL_GL_RETAINED_BACKING = 0;
	requested.SDL_GL_ACCELERATED_VISUAL = 1;	
	
	setOpenGLAttributes(&requested);
	
	/* create main window and renderer */
	windowID = SDL_CreateWindow(NULL, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,\
								SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_BORDERLESS);
	SDL_CreateRenderer(windowID, 0, 0);		
	
	printf("Requested:\n");
	printOpenGLAttributes(&requested);
	
	printf("obtained:\n");
	getOpenGLAttributes(&obtained);
	printOpenGLAttributes(&obtained);	
	
	initializeTexture();

	done = 0;
	/* enter main loop */
	while(!done) {
		startFrame = SDL_GetTicks();
		SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
				done = 1;
            }
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				printf("mouse down\n");
				int which = event.button.which;
				int x, y;
				SDL_SelectMouse(which);
				SDL_GetMouseState(&x, &y);
				spawnEmitterParticle(x, y);
			}
        }
		render();
		endFrame = SDL_GetTicks();
		
		/* figure out how much time we have left, and then sleep */
		delay = MILLESECONDS_PER_FRAME - (endFrame - startFrame);
		if (delay > MILLESECONDS_PER_FRAME) {
			delay = MILLESECONDS_PER_FRAME;
		}
		if (delay > 0) {
			SDL_Delay(delay);
		}
			
		//SDL_Delay(delay);
	}
	
	/* delete textures */
	
	/* shutdown SDL */
	SDL_Quit();
	
	return 0;
	
}