#ifndef BUTTON_H

#define BUTTON_H

typedef struct {
    char text[16];
    int active;
} Button;

void button_render(Button *button, int idx, int tot);
void button_set_active(Button *button, int val);
int button_pressed(Button *button, int idx, int tot);

#endif /* BUTTON_H */
