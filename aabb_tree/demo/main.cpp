#define AABBTREE_IMPLEMENTATION
#include "../aabb_tree.hpp"

#include "raylib.h"
#include <vector>

using namespace mia;

void debug_boxes_helper_recursive(std::vector<std::pair<AABB2f, usize>> *boxes,
                                  AABBTree_Node *cur_node, usize depth) {
    boxes->emplace_back(std::make_pair(cur_node->bound, depth));

    if (!cur_node->childs[0] || !cur_node->childs[1])
        return;
    debug_boxes_helper_recursive(boxes, cur_node->childs[0], depth + 1);
    debug_boxes_helper_recursive(boxes, cur_node->childs[1], depth + 1);
}

int main(void) {
    AABBTree tree{};
    AABB2f aabbs[10];

    InitWindow(1280, 720, "aabbtree demo");

    // Randomly generate aabbs in field 1280 x 720
    for (auto i = 0; i < 10; i++) {
        float minX = GetRandomValue(0 + 100, 1280 - 100 - 100);
        float minY = GetRandomValue(0 + 100, 720 - 100 - 100);
        float maxX = minX + GetRandomValue(20, 100);
        float maxY = minY + GetRandomValue(20, 100);

        aabbs[i].min = {minX, minY};
        aabbs[i].max = {maxX, maxY};

        aabbtree_insert(&tree, &aabbs[i]);
    }

    std::vector<std::pair<AABB2f, usize>> debug_boxes;
    debug_boxes_helper_recursive(&debug_boxes, tree.root, 0);
    Color debug_box_color[7] = {PURPLE, PINK, RED, ORANGE, YELLOW, LIME, GREEN};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Render base aabbs as filled rectangle with color = BLUE
        for (auto i = 0; i < 10; i++) {
            DrawRectangle(aabbs[i].min.x, aabbs[i].min.y,
                          aabbs[i].max.x - aabbs[i].min.x,
                          aabbs[i].max.y - aabbs[i].min.y, BLUE);
        }

        // Render debug_boxes as border rectangle with color = depth % 5 of
        // debug_box_color; make the filled inside transparent 20% if possible
        for (const auto &[aabb, depth] : debug_boxes) {
            Color border_color = debug_box_color[depth % 7];
            Color fill_color = border_color;
            fill_color.a = 30;

            f32 offset_scale = 1.0;
            Rectangle rect = {
                aabb.min.x - depth * offset_scale,
                aabb.min.y - depth * offset_scale,
                aabb.max.x - aabb.min.x + depth * offset_scale * 2,
                aabb.max.y - aabb.min.y + depth * offset_scale * 2};

            // Draw filled rectangle with transparency
            DrawRectangle(rect.x, rect.y, rect.width, rect.height, fill_color);

            // Draw border
            DrawRectangleLines(rect.x, rect.y, rect.width, rect.height,
                               border_color);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
