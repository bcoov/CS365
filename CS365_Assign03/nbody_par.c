// N-body simulation (parallel version)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <forms.h>
#include "mtqueue.h"

// ----------------------------------------------------------------------

// Constants: note that the physical constants have been chosen
// somewhat arbitrarily

// See for random number generator
#define SEED 123L

// Gravitational constant
#define G 6.67384

// Softening factor (to limit the force when bodies are close)
#define SOFTENING 100000.0

// Number of particles to simulate
#define NUM 1024

// Width and height of simulation playing field
#define WIDTH 10000.0
#define HEIGHT 10000.0

#define NUM_THREADS 4
pthread_t * workers;

// Struct type representing a single particle
typedef struct {
	double x, y;
	double dx, dy;
	double mass;
	int color;
} Particle;

// Struct type representing a range of particles
typedef struct {
	int start;
	int end;
} Range;

// Struct type representing the overall simulation
typedef struct {
	Particle *particles;
	int num_particles;

	//Range * part_range;
	MTQueue * work_q;
	MTQueue * done_q;
} NBody;

// ----------------------------------------------------------------------

//
// Initialize given particle with a random position and velocity.
//
void particle_init_rand(Particle *p)
{
	double startWidth = WIDTH/4;
	double startHeight = HEIGHT/4;
	p->x = (drand48() * startWidth) - (startWidth / 2);
	p->y = (drand48() * startHeight) - (startHeight / 2);
	p->dx = (drand48() * 50.0) - 25.0;
	p->dy = (drand48() * 50.0) - 25.0;
	p->mass = 128000.0;
	p->color = 10 + (mrand48() % 240);
}

//
// Compute the distance between two particles.
//
double particle_dist(Particle *p1, Particle *p2)
{
	double xdist = p1->x - p2->x;
	double ydist = p1->y - p2->y;
	double dist = sqrt(xdist*xdist + ydist*ydist);
	return dist;
}

//
// Compute the attraction between two particles based on their
// distance from each other and their masses.
//
double particle_force(Particle *p1, Particle *p2)
{
	double dist = particle_dist(p1, p2);
	return (G*p1->mass*p2->mass) / ((dist*dist) + SOFTENING);
}

//
// Compute the effect on p1's velocity based on its attraction to p2.
void particle_compute_attraction(Particle *p1, Particle *p2)
{
	double dist = particle_dist(p1, p2);
	double force = particle_force(p1, p2);
	//printf("dist=%lf, force=%lf\n", dist, force);
	double forcex = force * ((p2->x - p1->x) / dist);
	double forcey = force * ((p2->y - p1->y) / dist);
	p1->dx += (forcex / p1->mass);
	p1->dy += (forcey / p1->mass);
}

//
// Update a particle's position based on its velocity.
//
void particle_update_position(Particle *p)
{
	// The .01 scaling factor here is to make the simulation "smoother",
	// by decreasing the distance the particles move per time step.
	p->x += (p->dx * .01);
	p->y += (p->dy * .01);
}

//
// Compute the particle attraction for a range of particles.
// (Parallel version)
//
void * particle_range_comp(void * t_arg)
{
	NBody * sim = t_arg;

	while (1) {
		Range * sim_range = mtqueue_dequeue(sim->work_q);

		for (int i = sim_range->start; i < sim_range->end; i++) {
			for (int j = 0; j < sim->num_particles; j++) {// should be all
				if (i != j) {
					particle_compute_attraction(&sim->particles[i],
												&sim->particles[j]);
				}
			}
		}
		mtqueue_enqueue(sim->done_q, sim_range);
	}
}

// ----------------------------------------------------------------------

void nbody_init(NBody *sim)
{
	printf("Initializing\n");
	sim->particles = malloc(NUM * sizeof(Particle));
	sim->num_particles = NUM;

	for (int i = 0; i < NUM; i++) {
		particle_init_rand(&sim->particles[i]);
	}

	sim->work_q = mtqueue_create();
	printf("Created work queue %p\n", sim->work_q);
	sim->done_q = mtqueue_create();
	printf("Created done queue %p\n", sim->done_q);

	printf("Work queue head=%p, tail=%p\n", sim->work_q->head, sim->work_q->tail);

	workers = malloc(NUM_THREADS * sizeof(pthread_t));

	printf("Creating threads\n");
	for (int i = 0; i < NUM_THREADS; ++i) {
		pthread_create(&workers[i], NULL, particle_range_comp, sim);
	}
	printf("Init done\n");
}

void nbody_destroy(NBody *sim)
{
	free(sim->particles);
	free(sim);
}

void nbody_tick(NBody *sim)
{
	printf("Tick\n");

	// Simulate the force on each particle due to the gravitational attraction
	// to all of other particles, and update each particle's velocity accordingly.
	// for (int i = 0; i < sim->num_particles; i++) {
	// 	for (int j = 0; j < sim->num_particles; j++) {
	// 		if (i != j) {
	// 			particle_compute_attraction(&sim->particles[i], &sim->particles[j]);
	// 		}
	// 	}
	// }

	for (int i = 0; i < NUM_THREADS; ++i) {
		int chunk_size = NUM / NUM_THREADS;
		Range * range = malloc(sizeof(Range));
		range->start = 0 + (i * chunk_size);
		range->end = range->start + chunk_size;

		printf("Enqueueing #%d\n", i);
		mtqueue_enqueue(sim->work_q, range);
	}

	// Wait until work is finished
	// Dequeue includes wait (for loop here)

	for (int i = 0; i < NUM_THREADS; ++i) {
		printf("Dequeueing #%d\n", i);
		Range *range = mtqueue_dequeue(sim->done_q);
		// could free range
	}

	// Based on each particle's velocity, update its position.
	printf("Updating\n");
	for (int i = 0; i < sim->num_particles; i++) {
		particle_update_position(&sim->particles[i]);
	}
}

// ----------------------------------------------------------------------

#define TIMEOUT_MS 40

typedef struct {
	NBody *sim;
	FL_FORM *form;
	FL_OBJECT *canvas;
	GC canvasGC;
	int timeout_id;
} UI;

int canvas_expose(FL_OBJECT *ob, Window win, int w, int h, XEvent *ev, void *d)
{
	//printf("Expose!\n");

	UI *ui = d;

	// Drawing!

	XSetForeground(fl_get_display(), ui->canvasGC, fl_get_flcolor(FL_BLACK));
	XFillRectangle(fl_get_display(), win, ui->canvasGC, 0, 0, w, h);
	
	//XFillRectangle(fl_get_display(), win, ui->canvasGC, 10, 10, w-20, h-20);
	NBody *sim = ui->sim;
	for (int i = 0; i < sim->num_particles; i++) {
		int px = (int) (((sim->particles[i].x + (WIDTH/2)) / WIDTH) * 500);
		int py = (int) (((sim->particles[i].y + (HEIGHT/2)) / HEIGHT) * 500);
		//printf("px=%i, py=%i\n", px, py);
		int xcolor = fl_get_flcolor(FL_DODGERBLUE);
		//int xcolor = fl_get_flcolor(sim->particles[i].color);
		XSetForeground(fl_get_display(), ui->canvasGC, xcolor);
		XFillRectangle(fl_get_display(), win, ui->canvasGC, px, py, 1, 1);
	}
	
	return 0;
}

void animation_callback(int x, void *data)
{
	UI *ui = data;

//	printf("Tick!\n");

	nbody_tick(ui->sim);

	// Force redraw
	Window w = fl_get_real_object_window(ui->canvas);
	XClearArea(fl_get_display(), w, 0, 0, 0, 0, True);

	// Re-install timeout
	ui->timeout_id = fl_add_timeout(TIMEOUT_MS, animation_callback, ui);
}

int main(int argc, char **argv)
{
	srand48(SEED);

	// Initialize simulation
	NBody *sim = malloc(sizeof(NBody));
	nbody_init(sim);

	// GUI
	UI *ui = malloc(sizeof(UI));
	ui->sim = sim;

	fl_initialize(&argc, argv, 0, 0, 0);

	ui->form = fl_bgn_form(FL_UP_BOX, 500, 500);
	//fl_set_form_dblbuffer(ui->form, 1);
	ui->canvas = fl_add_canvas(FL_NORMAL_CANVAS, 10, 10, 480, 480, "");
	fl_end_form();

	ui->canvasGC = XCreateGC(fl_get_display(),fl_state[fl_vmode].trailblazer,0,0);

	fl_add_canvas_handler(ui->canvas, Expose, canvas_expose, ui);
	ui->timeout_id = fl_add_timeout(TIMEOUT_MS, animation_callback, ui);

	fl_show_form(ui->form, FL_PLACE_MOUSE, FL_FULLBORDER, "N-body simulator");

	fl_do_forms();
	fl_hide_form(ui->form);
	fl_finish();

	nbody_destroy(sim);
	free(ui);

	return 0;
}
