#ifndef MENU_NAVIGATION_MANAGER_H
#define MENU_NAVIGATION_MANAGER_H

typedef struct MenuNode {
  short int x, y;
  struct MenuNode *up, *down, *left, *right;
  void (*select)(void);
} MenuNode;

MenuNode *get_current(void);
void menu_set_active(MenuNode *root);
void menu_nav_up(void);
void menu_nav_down(void);
void menu_nav_left(void);
void menu_nav_right(void);
void menu_select(void);

#endif
