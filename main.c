#define _DEFAULT_SOURCE
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <raylib.h>

#define TODOCAP 1024 
enum state{
    TODO =0,
    DOING,
    DONE
};

typedef struct{
    char name[256];
    char description[1024];
    int state;
    int pos[2];
} todo;

typedef struct{
    todo *todos;
    int size;
} todolist;


void list_init(todolist *list){
    list->todos = malloc(sizeof(todo) * TODOCAP);
    list->size = 0;
}

void list_add(todolist *list, todo item){
    if(list->size == TODOCAP){
        printf("List is full\n");
        return;
    }
    list->todos[list->size] = item;
    list->size++;
}

todo *mktodo(char *name, char *description, int state){
    todo *item = malloc(sizeof(todo));
    strcpy(item->name, name);
    strcpy(item->description, description);
    item->state = state;
    return item;
}

char *join_path(char *path, char *name){
    char *result = malloc(strlen(path) + strlen(name) + 2);
    strcpy(result, path);
    strcat(result, "/");
    strcat(result, name);
    return result;
}
int state = 0;
void todoListDir(char *path, todolist *list)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return;

    char path_read[1024];
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            switch(entry->d_name[0]){
                case 't':
                    state = TODO;
                    break;
                case 'd':
                    state = DOING;
                    break;
                case 'D':
                    state = DONE;
                    break;
            }
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char *tmp = join_path(path, entry->d_name);
            strcpy(path_read, tmp);
            free(tmp);
            todoListDir(path_read, list);
        }else{
            char buff[1024];
            memset(buff, 0, sizeof(buff));
            char *tmp = join_path(path, entry->d_name);
            FILE *fp = fopen(tmp, "r");
            free(tmp);
            if(fp == NULL){
                printf("Error opening file: %s\n", strerror(errno));
                return;
            }
            fread(buff, 1, sizeof(buff), fp);
            fclose(fp);

            todo *item = mktodo(entry->d_name,buff, state);
            list_add(list, *item);

        }
    }
    closedir(dir);
}

void list_dump(todolist *list){
    for(int i = 0; i < list->size; i++){
        printf("%s\n", list->todos[i].name);
        printf("%s\n", list->todos[i].description);
        printf("%d\n", list->todos[i].state);
    }
}


int main1(void)
{
    todolist list;
    list_init(&list);
    todoListDir("./test", &list);
    list_dump(&list);
    return 0;
}


int main(void){
    // todo gui 
    todolist list;
    list_init(&list);
    todoListDir("./test", &list);
    //list_dump(&list);
    int screenWidth = 800;
    int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "TODO");
    SetTargetFPS(60);
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);
        
        // draw list as bottem the bottem name is the name of the todo 
        // and draw it in three columns depending on the state
        // and make a border around it
        
        int width = screenWidth / 3;
        int height = screenHeight / 3;
        int padding = 10;
        int x = 0;
        int y = 0;
        for(int i = 0; i < list.size; i++){
            switch(list.todos[i].state){
                case TODO:
                    DrawRectangle(x, y, width, height, RED);
                    break;
                case DOING:
                    DrawRectangle(x, y, width, height, BLUE);
                    break;
                case DONE:
                    DrawRectangle(x, y, width, height, GREEN);
                    break;
            }
            DrawText(list.todos[i].name, x + padding, y + padding, 20, WHITE);
            list.todos[i].pos[0] = x;
            list.todos[i].pos[1] = y;
            y += height + padding;
            if(y > screenHeight){
                y = 0;
                x += width + padding;
            }
        }
    
        
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            // check if the mouse is in the rect
            for(int i = 0; i < list.size; i++){
                if(mouse_x > list.todos[i].pos[0] && mouse_x < list.todos[i].pos[0] + width){
                    if(mouse_y > list.todos[i].pos[1] && mouse_y < list.todos[i].pos[1] + height){
                        printf("%s\n", list.todos[i].description);

                    }
                }
            }
        }
        EndDrawing();
    }

    

    return 0; 
    
}
