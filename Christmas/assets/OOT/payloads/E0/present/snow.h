#ifndef SNOW_H
#define SNOW_H

#include <z64ovl/oot/u10.h>
#include <z64ovl/oot/helpers.h>
#include <z64ovl/zh/vec3f.h>
#include <z64ovl/zh/vec3s.h>

const float delta_time = 0.05f;

enum PARTICLE_SYSTEM_MODE
{
    PARTICLE_SYSTEM_MODE_LINEAR = 0,
    PARTICLE_SYSTEM_MODE_SNOW
};

enum PARTICLE_SYSTEM_SPAWNER_SHAPE
{
    PARTICLE_SYSTEM_SPAWNER_SHAPE_PLANE,
    PARTICLE_SYSTEM_SPAWNER_SHAPE_CIRCLE
};

enum MONONOKE_STATE
{
    MONONOKE_STATE_DEAD = 0,
    MONONOKE_STATE_ALIVE
};

typedef union
{
    /* 0x00 */ uint32_t rgba;
    struct
    {
        /* 0x00 */ uint8_t r;
        /* 0x01 */ uint8_t g;
        /* 0x02 */ uint8_t b;
        /* 0x03 */ uint8_t a;
    };
    /* 0x04 */
} rgba_t;

typedef struct mononoke_s
{
    /* 0x00 */ vec3f_t position;       // This is intended to be in local coords
    /* 0x0C */ vec3f_t position_final; // This is intended to be in local coords
    /* 0x18 */ vec3f_t velocity;
    /* 0x24 */ float scale;
    /* 0x28 */ float life;
    /* 0x2C */ float rotation;
    /* 0x30 */ rgba_t color;
    /* 0x34 */ uint32_t state;
    /* 0x38 */
} mononoke_t; // I dunno man, it's like a tree spirit from Princess Mononoke

typedef mononoke_t gnome_t;

typedef union mononoke_data_s
{
    struct linear;
    struct
    {
        /* 0x00 */ uint32_t particle_index;
    } snow;
    /* 0x04 */
} mononoke_data_t;

void mononoke_construct(mononoke_t *particle, float scale, float life, float rotation, rgba_t color, vec3f_t *position, vec3f_t *velocity)
{
    particle->scale = scale;
    particle->life = life;
    particle->rotation = rotation;
    particle->color.rgba = color.rgba;
    particle->position.x = position->x;
    particle->position.y = position->y;
    particle->position.z = position->z;
    particle->position_final.x = position->x;
    particle->position_final.y = position->y;
    particle->position_final.z = position->z;
    particle->velocity.x = velocity->x;
    particle->velocity.y = velocity->y;
    particle->velocity.z = velocity->z;
    particle->state = MONONOKE_STATE_ALIVE;
}

#define SNOW_TWIRL_SCALE (10.0f)
void mononoke_step(gnome_t *particle, float time, uint32_t mode, mononoke_data_t *modedata)
{
    vec3f_t p1, v1;
    float t0, t1, t2, t3; // temp variables

    // If a particle should die, kill it
    // Useful if we need to know to free or recycle a particle
    if (particle->life <= 0)
    {
        particle->state = MONONOKE_STATE_DEAD;
        particle->life = 0.0f;
        return;
    }

    v1 = vec3f_mul_f(particle->velocity, delta_time);
    p1 = vec3f_add(particle->position, v1);

    particle->position = p1;

    switch (mode)
    {
    case (PARTICLE_SYSTEM_MODE_LINEAR):
        break;
    case (PARTICLE_SYSTEM_MODE_SNOW):
        // simple circular-twirling effect
        t0 = time + (float)modedata->snow.particle_index; // instead of using the particle index, we could potentially store something like the invoke time for a better effect?
        t1 = z_sinf(t0) * SNOW_TWIRL_SCALE;
        t2 = z_cosf(t0) * SNOW_TWIRL_SCALE;

        // TODO: something cool xor fancy

        particle->position.y -= 2.0f;

        p1.x += t1;
        p1.y -= 2.0f;
        p1.z += t2;

        particle->rotation += DTOR(5);

        break;
    }

    particle->position_final.x = p1.x;
    particle->position_final.y = p1.y;
    particle->position_final.z = p1.z;
    particle->life -= delta_time;
}

void mononoke_draw(mononoke_t* particle, z64_global_t* global, vec3f_t* world, float scale)
{
    //z64_disp_buf_t* opa = &ZQDL(global, poly_opa);
    z64_disp_buf_t *xlu = &ZQDL(global, poly_xlu);
    vec3f_t p0;

    gDPPipeSync(xlu->p++);
    gDPSetPrimColor(xlu->p++, 0, 0, particle->color.r, particle->color.g, particle->color.b, particle->color.a);
    gDPSetEnvColor(xlu->p++, particle->color.r, particle->color.g, particle->color.b, particle->color.a);
    p0 =vec3f_new_f(
        particle->position_final.x + world->x,
        particle->position_final.y + world->y,
        particle->position_final.z + world->z
    );
    //vec3f_add(particle->position_final, *world);

    z_matrix_scale_3f(1.0f, 1.0f, 1.0f, 0);
    z_matrix_translate_3f(p0.x, p0.y, p0.z, 1);
    z_matrix_scale_3f(particle->scale * scale, particle->scale * scale, particle->scale * scale, 1);
    draw_dlist_xlu(global, 0x80700080);
}

typedef union {
    struct {
        /* 0x00 */ vec3f_t p0;
        /* 0x0C */ vec3f_t p1;
    } plane;

    struct {
        /* 0x00 */ float radius;
    } circle;

    /* 0x18 */
} particle_system_shape_t;

typedef union {
    struct {
        /* 0x00 */ float start;
        /* 0x04 */ float end;
    } f;

    struct {
        /* 0x00 */ uint32_t start;
        /* 0x04 */ uint32_t end;
    } i;

    /* 0x08 */
} startend_t;

// We should discuss what kind of variance we want to add, etc.
typedef union {
    struct {
        /* 0x00 */ float speed;
        /* 0x04 */ startend_t scale;
        /* 0x0C */ startend_t color;
    } linear;

    struct fancy; // add fancy options or something

    /* 0x14 */ float life;
    /* 0x18 */
} particle_system_config_t;

// TODO: Write particle push/pop from particle list on node side, since that is where the heap will be handled
typedef struct
{
    /* 0x00 */ uint32_t mode;
    /* 0x04 */ uint32_t count;
    /* 0x08 */ uint32_t shape;
    /* 0x0C */ mononoke_t** particles;
    /* 0x10 */ vec3f_t position;
    /* 0x1C */ float scale;
    /* 0x20 */ float pitch;
    /* 0x24 */ float yaw;
    /* 0x28 */ particle_system_shape_t shape_info;
    /* 0x40 */ particle_system_config_t config;
    /* 0x54 */
} particle_system_t;

void particle_system_construct(particle_system_t* system, uint32_t mode, vec3f_t* position, float pitch, float yaw, uint32_t shape)
{
    system->mode = mode;
    system->count = 0;
    system->particles = 0;
    system->position = *position;
    system->pitch = pitch;
    system->yaw = yaw;
    system->shape = shape;
    // note that shape_info and config should be configured and applied on their own; these are just defaults
    system->shape_info.plane.p0 = vec3f_zero;
    system->shape_info.plane.p1 = vec3f_zero;
    system->config.linear.color.i.start = 0xFFFFFFFF;
    system->config.linear.color.i.end = 0xFFFFFFFF;
    system->config.linear.scale.f.start = 1.0f;
    system->config.linear.scale.f.end = 1.0f;
    system->config.linear.speed = 5.0f;
    system->config.life = 1000.0f;
}

void particle_system_step(particle_system_t *system, float time)
{
    uint32_t index;
    float t0;
    vec3f_t p0, v0, v1;
    mononoke_data_t modedata;

    if (system->count == 0 || system->particles == 0) return;

    for (index = 0; index < system->count; index++)
    {
        if (system->particles[index] == 0) continue;

        if (system->particles[index]->state)
        {
            switch (system->mode)
            {
            case (PARTICLE_SYSTEM_MODE_LINEAR):
                break;
            case (PARTICLE_SYSTEM_MODE_SNOW):
                modedata.snow.particle_index = index;
                break;
            }

            mononoke_step(system->particles[index], time, &system->mode, &modedata);
        }
        else {
            // Create or ressurect a particle
            v1 = vec3f_new_f(
                z_cosf(system->pitch) * z_cosf(system->yaw),
                -z_sinf(system->pitch),
                z_cosf(system->pitch) * z_sinf(system->yaw)
            );

            v0 = vec3f_mul_f(v1, system->config.linear.speed);

            switch(system->shape) {
                case PARTICLE_SYSTEM_SPAWNER_SHAPE_PLANE:
                    v1 = vec3f_new_f(
                        (system->shape_info.plane.p0.x + system->shape_info.plane.p1.x) * 0.5f,
                        (system->shape_info.plane.p0.y + system->shape_info.plane.p1.y) * 0.5f,
                        (system->shape_info.plane.p0.z + system->shape_info.plane.p1.z) * 0.5f
                    );

                    t0 = system->shape_info.plane.p1.x - system->shape_info.plane.p0.x;
                    p0.x = ((z_math_rand_zero_one() - 0.5f) * 2.0f) * t0;

                    t0 = system->shape_info.plane.p1.y - system->shape_info.plane.p0.y;
                    p0.y = ((z_math_rand_zero_one() - 0.5f) * 2.0f) * t0;

                    t0 = system->shape_info.plane.p1.z - system->shape_info.plane.p0.z;
                    p0.z = ((z_math_rand_zero_one() - 0.5f) * 2.0f) * t0;

                    break;
                case PARTICLE_SYSTEM_SPAWNER_SHAPE_CIRCLE:
                    p0 = vec3f_new_f(
                        ((z_math_rand_zero_one() - 0.5f) * 2.0f) * system->shape_info.circle.radius,
                        ((z_math_rand_zero_one() - 0.5f) * 2.0f) * system->shape_info.circle.radius,
                        ((z_math_rand_zero_one() - 0.5f) * 2.0f) * system->shape_info.circle.radius
                    );
                    break;
            }

            system->particles[index]->position = p0;
            system->particles[index]->position_final = p0;
            system->particles[index]->velocity = v0;
            system->particles[index]->scale = system->config.linear.scale.f.start;
            system->particles[index]->color.rgba = system->config.linear.color.i.start;
            system->particles[index]->life = system->config.life;
            system->particles[index]->rotation = 0;
            system->particles[index]->state = MONONOKE_STATE_ALIVE;
        }
    }
}

void particle_system_draw(particle_system_t* system, float time)
{
    uint32_t index;

    if (system->count == 0 || system->particles == 0) return;

    for (index = 0; index < system->count; index++)
    {
        if (system->particles[index] == 0) continue;
        if (system->particles[index]->state) mononoke_draw(system->particles[index], GLOBAL_CONTEXT, &system->position, system->scale);
    }
}

/*
 *      Construct the particle system
 *          Allocate the particle table (which is merely a pointer list), which should always take the size (count * 4), and set system.particles to that
 *      To spawn a particle
 *          Add to count
 *              Realloc system->particles, resizing it to (count * 4)
 *          Alloc a mononoke_t and insert a pointer to that onto the new slot on system->particles
 *
 *
*/

/* 💬 Drahsid is typing... 💬
    system->particles = malloc(system->count * sizeof(int));    // whenever you first need a particle, you malloc where the size alloc'd is (count * 4)
                                                                // after the first time, you would realloc where the alloc'd size is (count * 4)

    for (int index = 0; index < system->count; index++) {
        system->particles[index] = &(new mononoke_t())  // construct it and set system->particles[index] to that pointer
                                                        // in this case, a particle, mononoke_t has 0x38 size
        // so it could be like this
        system->particles[index] = malloc(sizeof(mononoke_t))
        // then construct the particle
    }
    It's not rocket science. 🧑‍🚀
    🎅🎁🎄
*/

#endif
