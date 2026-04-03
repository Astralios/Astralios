#include "smlib.h"

typedef struct model_t
{
    const vec3_t *vcs;
    const int *fs[3];
} model_t;

void render_model(model_t model)
{

}

vec2_t to_screen(vec2_t p)
{
    return (vec2_t){
        .x = (p.x + 1) / 2 * fb_driver0->fb->width,
        .y = (-p.y + 1) / 2 * fb_driver0->fb->height,
    };
}


void draw_line(vec2_t p1, vec2_t p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    int steps = (int)(abs(dx) > abs(dy) ? abs(dx) : abs(dy));

    if (steps == 0)
    {
        fb_driver_set_pixel(fb_driver0, p1.x, p1.y, RED);
        return;
    }
    
    float x_inc = dx / steps;
    float y_inc = dy / steps;

    float x = p1.x;
    float y = p1.y;

    for (int i = 0; i <= steps; i++)
    {
        fb_driver_set_pixel(fb_driver0, (size_t)x, (size_t)y, RED);
        x += x_inc;
        y += y_inc;
    }
}

void clear()
{
    fb_driver_fill_entirely(fb_driver0, BLUE);
}

    const int FPS = 60;
    float angle = 0;

    while (1) {
        const float dt = 1.0 / FPS;
        angle += dt * HALF_PI;
        clear();

        for (size_t i = 0; i < arrlen(fs); ++i)
        {
            const int *f = fs[i];
            for (size_t j = 0; j < arrlen(*fs); ++j)
            {
                vec3_t p1 = vcs[f[j]];
                vec3_t p2 = vcs[(f[(j + 1) % 3])];
                vec2_t rp1 = to_screen(
                        vec3_project_to_vec2(
                            vec3_translate_z(
                                vec3_rot_xz(p1, angle), 
                                3)
                            )
                        );
                vec2_t rp2 = to_screen(
                        vec3_project_to_vec2(
                            vec3_translate_z(
                                vec3_rot_xz(p2, angle), 
                                3)
                            )
                        );
                draw_line(rp1, rp2);
            }
        }

        fb_driver_sync(fb_driver0, 0, to_fb_idx(fb_driver0->fb, fb_driver0->fb->width - 1, fb_driver0->fb->height - 1));

    }
int main()
{

    return 0;
}
