#ifndef __MAPPING__
#define __MAPPING__

#define MY_FUNCT_GET(xx) xx(int *, char**);
#define MY_FUNCT_SET(xx) xx(int *, char *);

typedef int (*Myadd_get)(int *command, char **val);
typedef int (*Myadd_set)(int *command, char *val);
typedef int (*MydelObj)(int * command, char *val);
typedef int (*MyaddObj)(int * command, char *val);
typedef int (*MycommitObj)(int * command, char *val);

typedef enum {
    eObject,
    eInstance
}eObjectype;

typedef struct my_param {
    char * name;
    Myadd_get getter;
    Myadd_set setter;
    //Instace data list
    //cwmp param type
    unsigned int custominform;
}cwmpParam;

typedef struct my_object {
    char *name;
    MydelObj delObj;
    MyaddObj addobj;
    MycommitObj commitobj;
    struct my_object *objlist;
    cwmpParam *paramlist;
    eObjectype otype:8;

}cwmpObject;

#endif