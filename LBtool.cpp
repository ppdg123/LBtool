#include "lbt.h"


int main(int argc ,char * argv[])
{
	//FILE * fp ; 
	//char cmd[STR_SIZE],filename[STR_SIZE];
	char ch;
	loadFilenames();
	if(argc==2)
	{
		cur_file_ptr = atoi(argv[1]);
		if(cur_file_ptr<0||cur_file_ptr>=file_set_cnt)
		{
			fprintf(stderr,"Invalid input id\n");
			exit(-1);
		}
	}
	loadConf();
	loadDataFile(file_set[cur_file_ptr]);
	initUI();
	im = cvLoadImage(file_set[cur_file_ptr],1);
	showImage();
	while(1)
	{
		ch = cvWaitKey(0);
		//printf("-- %d\n",ch);
		kCallBack(ch);
	}
	cvDestroyAllWindows();
	return 0;
}
