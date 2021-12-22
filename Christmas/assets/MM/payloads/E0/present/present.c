#include <z64ovl/mm/u10.h>
#include <z64ovl/oot/helpers.h>

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} presentRGBA;

typedef struct
{
    uint32_t presentModel;
    uint32_t treeModel;
    uint32_t topperModel;
    uint8_t pickupable;
    uint8_t displayMode;
    uint8_t snow;
    z64_actor_t *env1;
    z64_actor_t *env2;
    uint32_t treeId;
} present_t;

typedef struct
{
    z64_actor_t actor;
    present_t present;
} entity_t;

static uint32_t getuint32_t(entity_t *en, z64_global_t *global, uint32_t offset)
{
#ifdef OOT_U_1_0
    uint32_t *p = AADDR(AVAL(0x80700040, uint32_t, 0x0), offset);
#endif
#ifdef MM_U_1_0
    uint32_t *p = AADDR(AVAL(0x80F00040, uint32_t, 0x0), offset);
#endif
    return *p;
}

static uint32_t setuint32_t(entity_t *en, z64_global_t *global, uint32_t offset, uint32_t value)
{
#ifdef OOT_U_1_0
    uint32_t *p = AADDR(AVAL(0x80700040, uint32_t, 0x0), offset);
#endif
#ifdef MM_U_1_0
    uint32_t *p = AADDR(AVAL(0x80F00040, uint32_t, 0x0), offset);
#endif
    *p = value;
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

static void init(entity_t *en, z64_global_t *global)
{
    en->actor.flags = 0x0E000075;
    en->present.presentModel = getuint32_t(en, global, 0x4);
    en->present.treeModel = getuint32_t(en, global, 0x8);
    en->present.topperModel = getuint32_t(en, global, 0xC);
    en->present.pickupable = getuint32_t(en, global, 0x10);
    en->present.displayMode = getuint32_t(en, global, 0x14);
    en->present.snow = getuint32_t(en, global, 0x1C);
    en->present.treeId = getuint32_t(en, global, 0x28);

    if (en->present.snow > 0)
    {
#ifdef OOT_U_1_0
        if (!isZobjLoaded(&global->obj_ctxt, 0x0002))
        {
            en->present.snow = 0;
        }
#endif
    }
    z_actor_set_scale(&en->actor, 0.01f);
}

static void play(entity_t *en, z64_global_t *global)
{
    if (en->actor.dist_from_link_xz < 50 && en->present.pickupable > 0)
    {
        setuint32_t(en, global, 0x24, en->present.treeId);
        en->present.pickupable = 0;
    }
    if (en->present.snow > 0)
    {
        if (en->present.env1 > 0)
        {
            z64_player_t *player = zh_get_player(global);
            en->present.env1->pos.x = player->actor.pos.x;
            en->present.env1->pos.y = player->actor.pos.y;
            en->present.env1->pos.z = player->actor.pos.z;
        }
        if (en->present.env2 > 0)
        {
            z64_player_t *player = zh_get_player(global);
            en->present.env2->pos.x = player->actor.pos.x;
            en->present.env2->pos.y = player->actor.pos.y;
            en->present.env2->pos.z = player->actor.pos.z;
        }
        if (en->present.snow == 50)
        {
#ifdef OOT_U_1_0
            en->present.env1 = z_actor_spawn(&global->actor_ctxt, global, 0x0165, en->actor.pos.x, en->actor.pos.y, en->actor.pos.z, en->actor.rot.x, en->actor.rot.y, en->actor.rot.z, 0x7402);
            en->present.env1->room_index = 0xFF;
            en->present.env2 = z_actor_spawn(&global->actor_ctxt, global, 0x0097, en->actor.pos.x, en->actor.pos.y, en->actor.pos.z, en->actor.rot.x, en->actor.rot.y, en->actor.rot.z, 0x0003);
            en->present.env2->room_index = 0xFF;
#endif
            en->present.snow++;
        }
        else if (en->present.snow < 50)
        {
            en->present.snow++;
        }
    }
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