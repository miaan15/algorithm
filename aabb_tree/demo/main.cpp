#define AABBTREE_IMPLEMENTATION
#include "../aabbtree.hpp"

#include "raylib.h"
#include <vector>

using namespace mia;

void debug_boxes_helper_recursive(std::vector<std::pair<AABB2f, usize>> *boxes,
                                  _private::Node *cur_node, usize depth) {
    boxes->emplace_back(std::make_pair(cur_node->bound, depth));

    if (!cur_node->childs[0] || !cur_node->childs[1])
        return;
    debug_boxes_helper_recursive(boxes, cur_node->childs[0], depth + 1);
    debug_boxes_helper_recursive(boxes, cur_node->childs[1], depth + 1);
}

void handle_aabb_hit_bounds(AABB2f *aabb, Vec2f *velocity, float screen_width,
                            float screen_height) {
    float speed = vector_length(*velocity);
    bool hit_edge = false;

    if (aabb->min.x < 0) {
        aabb->max.x += -aabb->min.x;
        aabb->min.x = 0;
        hit_edge = true;
    } else if (aabb->max.x > screen_width) {
        aabb->min.x -= (aabb->max.x - screen_width);
        aabb->max.x = screen_width;
        hit_edge = true;
    }

    if (aabb->min.y < 0) {
        aabb->max.y += -aabb->min.y;
        aabb->min.y = 0;
        hit_edge = true;
    } else if (aabb->max.y > screen_height) {
        aabb->min.y -= (aabb->max.y - screen_height);
        aabb->max.y = screen_height;
        hit_edge = true;
    }

    if (hit_edge) {
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        velocity->x = cos(angle) * speed;
        velocity->y = sin(angle) * speed;
    }
}

int main(void) {
    AABBTree tree{};
    tree.margin = 2.0;

    constexpr usize cnt = 20000;
    AABB2f aabbs[cnt];
    Vec2f aabb_vecs[cnt];

    InitWindow(1280, 720, "aabbtree demo");
    SetTargetFPS(60);

    for (auto i = 0U; i < cnt; i++) {
        float minX = GetRandomValue(0 + 3, 1280 - 3 - 3);
        float minY = GetRandomValue(0 + 3, 720 - 3 - 3);
        float maxX = minX + GetRandomValue(1, 3);
        float maxY = minY + GetRandomValue(1, 3);

        aabbs[i].min = {minX, minY};
        aabbs[i].max = {maxX, maxY};

        aabbtree::insert(&tree, &aabbs[i]);
    }

    for (auto i = 0U; i < cnt; i++) {
        float speed = GetRandomValue(0, 30);
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        aabb_vecs[i].x = cos(angle) * speed;
        aabb_vecs[i].y = sin(angle) * speed;
    }

    // Color debug_box_color[7] = {PURPLE, PINK, RED, ORANGE, YELLOW, LIME, GREEN};

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        for (auto i = 0U; i < cnt; i++) {
            Vec2f displacement = aabb_vecs[i] * dt;
            aabbs[i].min += displacement;
            aabbs[i].max += displacement;

            handle_aabb_hit_bounds(&aabbs[i], &aabb_vecs[i], 1280, 720);
        }

        aabbtree::update(&tree);
        auto pair_list = aabbtree::get_collided_pairs(&tree);

        // std::vector<std::pair<AABB2f, usize>> debug_boxes;
        // debug_boxes_helper_recursive(&debug_boxes, tree.root, 0);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // for (const auto &[aabb, depth] : debug_boxes) {
        //     Color border_color = debug_box_color[depth % 7];
        //     Color fill_color = border_color;
        //     fill_color.a = 10;
        //
        //     f32 offset_scale = 0;
        //     Rectangle rect = {
        //         aabb.min.x - depth * offset_scale,
        //         aabb.min.y - depth * offset_scale,
        //         aabb.max.x - aabb.min.x + depth * offset_scale * 2,
        //         aabb.max.y - aabb.min.y + depth * offset_scale * 2};
        //
        //     DrawRectangle(rect.x, rect.y, rect.width, rect.height,
        //     fill_color); DrawRectangleLinesEx(rect, 1.0, border_color);
        // }

        for (auto i = 0U; i < cnt; i++) {
            DrawRectangle(aabbs[i].min.x, aabbs[i].min.y,
                          aabbs[i].max.x - aabbs[i].min.x,
                          aabbs[i].max.y - aabbs[i].min.y, BLUE);
            DrawRectangleLines(aabbs[i].min.x, aabbs[i].min.y,
                               aabbs[i].max.x - aabbs[i].min.x,
                               aabbs[i].max.y - aabbs[i].min.y, BLACK);
        }

        for (cauto &pair : pair_list) {
            Color color = RED;
            color.a = 200;
            DrawRectangle(pair.first->min.x, pair.first->min.y,
                          pair.first->max.x - pair.first->min.x,
                          pair.first->max.y - pair.first->min.y, color);
            DrawRectangle(pair.second->min.x, pair.second->min.y,
                          pair.second->max.x - pair.second->min.x,
                          pair.second->max.y - pair.second->min.y, color);
        }
        arrlist_free(&pair_list);

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
