#include "menu_navigation_manager.h"
#include <stddef.h>

static MenuNode *current = NULL;

MenuNode *get_current(void) { return current; }
void menu_set_active(MenuNode *root) { current = root; }
void menu_nav_up(void) {
  if (current && current->up) {
    current = current->up;
  }
}
void menu_nav_down(void) {
  if (current && current->down) {
    current = current->down;
  }
}
void menu_nav_left(void) {
  if (current && current->left) {
    current = current->left;
  }
}
void menu_nav_right(void) {
  if (current && current->right) {
    current = current->right;
  }
}
void menu_select(void) {
  if (current && current->select) {
    current->select();
  }
}
