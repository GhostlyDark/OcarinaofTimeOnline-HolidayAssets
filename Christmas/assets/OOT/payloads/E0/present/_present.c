#include <z64ovl/oot/u10.h>
#include <z64ovl/oot/helpers.h>
#include "snow.h"

typedef struct
{
    uint32_t presentModel;
    uint32_t treeModel;
    uint32_t topperModel;
    uint8_t pickupable;
    uint8_t displayMode;
    uint8_t snow;
    particle_system_t system;
    mononoke_t* p_particles_test[1];
    mononoke_t particles_test[1];
} present_t;

typedef struct
{
    z64_actor_t actor;
    present_t present;
} entity_t;

static uint32_t getuint32_t(entity_t *en, z64_global_t *global, uint32_t offset)
{
#ifdef OOT_U_1_0
    uint32_t *p = AADDR(AVAL(&en->actor, uint32_t, 0x14), offset);
#endif
#ifdef MM_U_1_0
    uint32_t *p = AADDR(AVAL(0x80810040, uint32_t, 0x0), offset);
#endif
    return *p;
}

static uint32_t isZobjLoaded(z64_obj_ctxt_t *obj_ctxt, int32_t id)
{
    int32_t index = z_scene_object_get_index(obj_ctxt, id);
    if (index < 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

static void startSnow(entity_t *en, z64_global_t *global)
{
#ifdef OOT_U_1_0
    AVAL(0x801D8FB5, uint8_t, 0x0) = 0x40;
#endif
}

static void init(entity_t *en, z64_global_t *global)
{
    en->actor.flags = 0x0E000075;

    en->present.presentModel = getuint32_t(en, global, 0x4);
    en->present.treeModel = getuint32_t(en, global, 0x8);
    en->present.topperModel = getuint32_t(en, global, 0xC);
    en->present.pickupable = getuint32_t(en, global, 0x10);
    en->present.displayMode = getuint32_t(en, global, 0x14);
    en->present.snow = getuint32_t(en, global, 0x1C);

    if (en->present.snow > 0)
    {
        particle_system_construct(&en->present.system, PARTICLE_SYSTEM_MODE_LINEAR, &vec3f_zero, DTOR(-90), 0, PARTICLE_SYSTEM_SPAWNER_SHAPE_CIRCLE);
    }
    z_actor_set_scale(&en->actor, 0.01f);
    en->present.system.count = 8;
    for (int i = 0; i < 1; i++) {
        rgba_t rgba;
        rgba.rgba = 0x80808080;
        z64_player_t* player = zh_get_player(global);
        mononoke_construct(&en->present.particles_test[i], 1.0f, 1000000.0f, 0.0f, rgba, &player->actor.pos, &vec3f_zero);
        en->present.p_particles_test[i] = &en->present.particles_test[i];
    }

    en->present.system.particles = &en->present.p_particles_test[0];
    en->present.system.scale = 0.3f;

    //en->present.system->particles = AVAL(&en->actor, uint32_t, 0x20);
}

static void play(entity_t *en, z64_global_t *global)
{
    if (en->actor.dist_from_link_xz < 50 && en->present.pickupable > 0)
    {
        uint32_t *p = AADDR(AVAL(&en->actor, uint32_t, 0x14), 0x14);
        *p = 0xDEADBEEF;
        en->present.pickupable = 0;
    }

#define NO_FOLLOW_LINK 1
#if NO_FOLLOW_LINK
    en->present.system.position = zh_get_player(global)->actor.pos;
//en->present.system.position.y += 125.0f;
    particle_system_step(&en->present.system, ((float)global->gameplay_frames) * 0.05f);
#else
    en->present.system.particles[0]->position_final = zh_get_player(global)->actor.pos;
    for (int i = en->present.system.count - 1; i > 0; i--) {
        en->present.system.particles[i]->position_final = en->present.system.particles[i - 1]->position_final;
    }
#endif
}

static void draw(entity_t *en, z64_global_t *global)
{

    if (en->present.displayMode == 1)
    {
        z_cheap_proc_draw_opa(global, en->present.treeModel);
        matrix_push();
        z_matrix_rotate_3s(0, en->actor.rot_toward_link_y, 0, 1);
        z_cheap_proc_draw_opa(global, en->present.topperModel);
        matrix_pop();
    }
    if (en->present.pickupable > 0)
    {
        z_cheap_proc_draw_opa(global, en->present.presentModel);
    }
    particle_system_draw(&en->present.system, (float)(1.0f * (global->gameplay_frames % 60)));
}

static void destroy(entity_t *en, z64_global_t *global)
{
}

/* .data */
const z64_actor_init_t init_vars = {
    .number = 0x05,
    .padding = 0x00,
    .type = 0x4,
    .room = 0xFF,
    .flags = 0x00000001,
    .object = 0x01,
    .instance_size = sizeof(entity_t),
    .init = init,
    .dest = destroy,
    .main = play,
    .draw = draw};