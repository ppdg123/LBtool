#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_SIZE 200
#define IMG_DIR "./img"
#define MAX_LB_FILE 50000
#define MAX_POINT 1000
#define TMP_LIST_FILE "/tmp/list.txt"
#define CONF_FILE "LBtool.conf"
#define TOOL_NAME "LBtool"
#define LINE_SET_FILE "LBline.conf"
#define ATTR_SET_FILE "Attr.conf"
#define MOD_SCAN 1
#define MAX_ATTR 20
#define MAX_ATTR_VAL 20

#define TYPE_REC 1
#define TYPE_PNT 2
#define TYPE_SP 3
#define TYPE_ATTR 4

#define PADDING_X 20
#define PADDING_Y 20

#define POINT_RADIUS 5
#define LINE_THICK 3
extern IplImage * im;
extern char * file_set[MAX_LB_FILE ];
extern int file_set_cnt;
extern int cur_file_ptr;

void mCallBack(int event,int x,int y, int flags, void * userdata);
void loadFilenames();
void kCallBack(char ch);
void initUI();
void loadConf();
void showImage();
void loadDataFile(char * img_file);
void nextPoint();
void previousPoint();
int findPointByName(char * name);
int findAttrValByName(int idx,char * name);