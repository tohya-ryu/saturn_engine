#include "saturn_engine/_lib.h"

static saten_scene_info *self;

static saten_fptr_void each_scene_init;
static saten_fptr_bool each_scene_update;
static saten_fptr_void each_scene_draw;
static saten_fptr_void each_scene_quit;

static void *scene_init_data;

// public
saten_scene_info saten_scene_create(saten_scene_info info,
        saten_fptr_void init, saten_fptr_bool update, saten_fptr_void draw,
        saten_fptr_void quit, char *loadscriptfp)
{
    if (info.alive)
        return info;
    saten_scene scene;
    memset(&scene, 0, sizeof(saten_scene));
    int i = SATEN_DARR_SIZE(saten_darr_scene);
    scene.init = init;
    scene.update = update;
    scene.draw = draw;
    scene.quit = quit;
    scene.init_flag = false;
    scene.quit_flag = false;
    scene.info.id = i;
    scene.info.uid = info.uid;
    scene.info.alive = true;
    scene.loadscriptfp = loadscriptfp;
    SATEN_DARR_PUSH(saten_darr_scene, scene);
    info.id = i;
    info.alive = true;
    return info;
}

// public
/*
 * When switching scenes (e.g. title (menu) -> game)
 * Call this before creating the new scene (game)
*/
void saten_scene_quit(saten_scene_info scene)
{
    int i = scene.id;
    for (int j = SATEN_DARR_SIZE(saten_darr_scene); i < j; i++)
        saten_darr_scene[i].quit_flag = true;
}

/* Access functions to control scenes */
// public
void saten_scene_init_done(saten_scene_info scene)
{
    int id = scene.id;
    saten_darr_scene[id].init_flag = true;
}

// public
// Returns the scene currently being processed
saten_scene_info* saten_scene_self(void)
{
    return self; 
}

// public
void saten_scene_self_set(saten_scene_info *scene)
{
    self = scene;
}

// public
void saten_scene_load_done(saten_scene_info scene)
{
    int id = scene.id;
    saten_darr_scene[id].load_flag = true;
}

// public
// Returns the scene at the top of the stack
saten_scene_info saten_scene_get_current(void)
{
    return saten_darr_scene[(SATEN_DARR_SIZE(saten_darr_scene)) - 1].info;
}

// public
saten_scene_info saten_scene_get_previous(void)
{
    return saten_darr_scene[(SATEN_DARR_SIZE(saten_darr_scene)) - 2].info; 
}

// public
saten_scene_info saten_scene_set_start(saten_scene_info scene)
{
    // returns previous starting scene
    saten_scene_info old = saten_scene_start;
    saten_scene_start = scene;
    return old;
}

// public
bool saten_scene_initialized(saten_scene_info scene)
{
    if (saten_scene_exists(scene)) {
        return saten_darr_scene[scene.id].init_flag;
    }
    return false;
}

bool saten_scene_loaded(saten_scene_info scene)
{
    if (saten_scene_exists(scene)) {
        return saten_darr_scene[scene.id].load_flag;
    }
    return false;
}

bool saten_scene_is_quitting(saten_scene_info scene)
{
    if (saten_scene_exists(scene)) {
        return saten_darr_scene[scene.id].quit_flag;
    }
    return false;
}

uint64_t saten_scene_frame(saten_scene_info scene)
{
    if (saten_scene_exists(scene)) {
        return saten_darr_scene[scene.id].framecnt;
    }
    return false;
}

// private
bool saten_scene_exists(saten_scene_info scene)
{
    return scene.alive;
}

saten_scene_info saten_scene_destroy(saten_scene_info scene)
{
    if (!saten_scene_exists(scene))
        return scene;

    int n = SATEN_DARR_SIZE(saten_darr_scene);
    n--; // new size and index of top scene
    if (scene.id != n) {
        saten_errhandler(45);
        return scene;
    }
    // free resources
    saten_resource_free(saten_darr_scene[scene.id].res);

    SATEN_DARR_RESIZE(saten_darr_scene, n);

    scene.alive = false;

    if (saten_load_mtx) {
        SDL_DestroyMutex(saten_load_mtx);
        saten_load_mtx = NULL;
    }

    return scene;
}

// private
void saten_scene_each_init(void)
{
    if (each_scene_init)
        each_scene_init();
}

// private
void saten_scene_each_update(bool r)
{
    if (each_scene_update)
        each_scene_update(r);
}

// private
void saten_scene_each_draw(void)
{
    if (each_scene_draw)
        each_scene_draw();
}

// private
void saten_scene_each_quit(void)
{
    if (each_scene_quit)
        each_scene_quit();
}

// public
void saten_scene_each_set(saten_fptr_void init, saten_fptr_bool update,
        saten_fptr_void draw, saten_fptr_void quit)
{
    each_scene_init = init;
    each_scene_update = update;
    each_scene_draw = draw;
    each_scene_quit = quit;
}

void* saten_scene_init_datar(void)
{
    return scene_init_data;
}

void saten_scene_init_data_clear(void)
{
    if (scene_init_data)
        free(scene_init_data);
}
