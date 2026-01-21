extern "C" {
#include "aabbtree.h"
}

#include <cglm/mat2x3.h>
#include <cglm/struct.h>
#include <cglm/types.h>
#include <cglm/vec3.h>
#include <raylib.h>
#include <utility>
#include <vector>

#include "def.h"

constexpr u32 window_width = 1600;
constexpr u32 window_height = 900;

constexpr size_t box_cnt = 500;
constexpr f32 box_min_sz = 10;
constexpr f32 box_max_sz = 30;
constexpr f32 box_min_speed = 10;
constexpr f32 box_max_speed = 100;

constexpr Color box_color = BLUE;
constexpr Color bounds_color = LIME;
constexpr Color collided_color = RED;

void handle_box_moving(std::pair<AABBs *, vec3s> &box, f32 dt) {
    vec3 delta;
    glm_vec3_scale(box.second.raw, dt, delta);
    glm_vec3_add(box.first->raw[0], delta, box.first->raw[0]);
    glm_vec3_add(box.first->raw[1], delta, box.first->raw[1]);

    bool hit_horizontal = box.first->raw[0][0] <= 0 || box.first->raw[1][0] >= window_width;
    bool hit_vertical = box.first->raw[0][1] <= 0 || box.first->raw[1][1] >= window_height;

    if (hit_horizontal || hit_vertical) {
        f32 speed = sqrtf(box.second.raw[0] * box.second.raw[0] + box.second.raw[1] * box.second.raw[1]);

        f32 angle = GetRandomValue(0, 360) * DEG2RAD;

        box.second.raw[0] = cos(angle) * speed;
        box.second.raw[1] = sin(angle) * speed;

        if (hit_horizontal) {
            if (box.first->raw[0][0] <= 0) {
                box.second.raw[0] = fabsf(box.second.raw[0]);
            } else {
                box.second.raw[0] = -fabsf(box.second.raw[0]);
            }
        }
        if (hit_vertical) {
            if (box.first->raw[0][1] <= 0) {
                box.second.raw[1] = fabsf(box.second.raw[1]);
            } else {
                box.second.raw[1] = -fabsf(box.second.raw[1]);
            }
        }
    }
}

void get_box_bounds_helper(std::vector<AABBs> &bounds_list, _AABBTree_Node *cur) {
    bounds_list.push_back(glms_mat2x3_make(cur->bounds[0]));

    if (cur->childs[0] != nullptr) get_box_bounds_helper(bounds_list, cur->childs[0]);
    if (cur->childs[1] != nullptr) get_box_bounds_helper(bounds_list, cur->childs[1]);
}

int main() {
    AABBTree tree{};
    tree.margin = 10;

    InitWindow(window_width, window_height, "aabbtree demo");

    std::vector<std::pair<AABBs *, vec3s>> box_data_list{};
    for (size_t i = 0; i < box_cnt; i++) {
        f32 minX = GetRandomValue(0 + 1, window_width - 1 - box_max_sz);
        f32 minY = GetRandomValue(0 + 1, window_height - 1 - box_max_sz);
        f32 maxX = minX + GetRandomValue(box_min_sz, box_max_sz);
        f32 maxY = minY + GetRandomValue(box_min_sz, box_max_sz);

        AABB aabb = {{minX, minY, 0}, {maxX, maxY, 1}};
        auto *data = aabbtree_insert(&tree, aabb);

        f32 speed = GetRandomValue(box_min_speed, box_max_speed);
        f32 angle = GetRandomValue(0, 360) * DEG2RAD;
        vec3s velocity;
        velocity.raw[0] = cos(angle) * speed;
        velocity.raw[1] = sin(angle) * speed;

        box_data_list.push_back({data, velocity});
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        for (auto &p : box_data_list) {
            handle_box_moving(p, dt);
        }

        aabbtree_update(&tree);

        auto collided_list = aabbtree_get_collided_pairs(&tree);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw bounds
        std::vector<AABBs> bounds_list;
        get_box_bounds_helper(bounds_list, tree.root);
        for (auto &b : bounds_list) {
            DrawRectangleLines(b.raw[0][0], b.raw[0][1], b.raw[1][0] - b.raw[0][0], b.raw[1][1] - b.raw[0][1], bounds_color);
        }

        // Draw boxes
        for (auto &p : box_data_list) {
            auto box = p.first;

            DrawRectangle(box->raw[0][0], box->raw[0][1], box->raw[1][0] - box->raw[0][0], box->raw[1][1] - box->raw[0][1], box_color);
            DrawRectangleLines(box->raw[0][0], box->raw[0][1], box->raw[1][0] - box->raw[0][0], box->raw[1][1] - box->raw[0][1], BLACK);
        }

        // Draw collided boxes
        for (size_t i = 0; i < collided_list.count; i++) {
            auto &p = collided_list.buffer[i];

            DrawRectangle(p.raw[0]->raw[0][0], p.raw[0]->raw[0][1], p.raw[0]->raw[1][0] - p.raw[0]->raw[0][0],
                          p.raw[0]->raw[1][1] - p.raw[0]->raw[0][1], collided_color);
            DrawRectangleLines(p.raw[0]->raw[0][0], p.raw[0]->raw[0][1], p.raw[0]->raw[1][0] - p.raw[0]->raw[0][0],
                               p.raw[0]->raw[1][1] - p.raw[0]->raw[0][1], BLACK);

            DrawRectangle(p.raw[1]->raw[0][0], p.raw[1]->raw[0][1], p.raw[1]->raw[1][0] - p.raw[1]->raw[0][0],
                          p.raw[1]->raw[1][1] - p.raw[1]->raw[0][1], collided_color);
            DrawRectangleLines(p.raw[1]->raw[0][0], p.raw[1]->raw[0][1], p.raw[1]->raw[1][0] - p.raw[1]->raw[0][0],
                               p.raw[1]->raw[1][1] - p.raw[1]->raw[0][1], BLACK);
        }
        ARRLIST_FREE(&collided_list);

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
