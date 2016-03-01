#include "lbt.h"

IplImage * im,*spim,*showim=NULL;
CvMat * spmat;

//all target images
char * file_set[MAX_LB_FILE ];
int file_set_cnt;
int cur_file_ptr;

int lb_type;

char * point_set[MAX_POINT];
int point_val[MAX_POINT][2];
int point_set_cnt;
int cur_point_ptr=0;

int sp_val[MAX_POINT];

int line_set[MAX_POINT][2];
int line_set_cnt = 0;

char attr_val_name[MAX_ATTR][MAX_ATTR_VAL][STR_SIZE];
int attr_val_cnt[MAX_ATTR];
int attr_val[MAX_ATTR];

void clearCur()
{
	if(im) cvReleaseImage(&im);
	if(showim) cvReleaseImage(&showim);
	if(spmat) cvReleaseMat(&spmat);
	if(spim) cvReleaseImage(&spim);
}
void initSpValue()
{
	int i;
	for(i=0;i<MAX_POINT;i++) sp_val[i] = -1;
}

void initSQRValue()
{
      int i ; 
     for(i=0;i<point_set_cnt;i++)
     {
         point_val[i][0] = -1;
	 point_val[i][1] = -1;
     } 
     cur_point_ptr = 0;
}
//Load all target images, saved in file_set.
void loadFilenames()
{
	file_set_cnt = 0;
	FILE * fp ;
	char cmd[STR_SIZE],fn[STR_SIZE];
	sprintf(cmd,"ls %s/*.jpg > %s",IMG_DIR,TMP_LIST_FILE);
	system(cmd);
	fp = fopen(TMP_LIST_FILE,"r");
	if(!fp)
	{
		fprintf(stderr,"Temp file not existed.\n");
		exit(-1);
	}
	while(EOF != fscanf(fp,"%s",fn))
	{
		file_set[file_set_cnt] = (char *)malloc(sizeof(char)*STR_SIZE);
		strcpy(file_set[file_set_cnt],fn);
		file_set_cnt ++;
	}
	cur_file_ptr = 0;
	fclose(fp);
}

void loadConf()
{
	/*
	   config file:
	   TYPE
	   POINT1
	   POINT2
	   ...
	 */
	FILE * fp;
	char cmd[STR_SIZE];
	int nval,idx,i,cnt;
	lb_type = -1;
	fp = fopen(CONF_FILE,"r");
	if(!fp)
	{
		fprintf(stderr,"config file not existed\n");
		exit(-1);
	}
	fscanf(fp,"%s",cmd);

	if(0==strcmp(cmd,"rec")) lb_type = TYPE_REC;
	if(0==strcmp(cmd,"point")) lb_type = TYPE_PNT;
	if(0==strcmp(cmd,"sp")) lb_type=TYPE_SP;
	if(0==strcmp(cmd,"attr")) lb_type=TYPE_ATTR;
	if(0==strcmp(cmd,"sqr")) lb_type = TYPE_SQR;
	
	if(lb_type<0)
	{
		fprintf(stderr,"wrong config file\n");
		exit(-1);
	}
	point_set_cnt = 0;
	while(EOF != fscanf(fp,"%s",cmd))
	{
		point_set[point_set_cnt] = (char *)malloc(sizeof(char)*STR_SIZE);
		strcpy(point_set[point_set_cnt],cmd);
		point_set_cnt ++;
	}
	if(lb_type == TYPE_REC&&point_set_cnt%2==1)
	{
		fprintf(stderr,"wrong config file : point number\n");
		exit(-1);
	}
	if(lb_type == TYPE_SQR&&point_set_cnt != 2)
	{
		fprintf(stderr,"2 points required(left , right)\n");
		exit(-1);
	}
	fclose(fp);
	if(lb_type==TYPE_PNT)
	{
		/*LBline.conf
		 POINTNAME1 POINTNAME2
		 POINTNAME3 POINTNAME4
		 ...

		 In TYPE_PNT, these lines are only draw on the showimg, but not for saving.
		 */
		fp = fopen(LINE_SET_FILE,"r");
		if(!fp){
			fprintf(stderr,"Line config not set\n");
		}
		else
		{
			int l1,l2;
			char name1[STR_SIZE],name2[STR_SIZE];
			while(EOF != fscanf(fp,"%s%s",name1,name2))
			{
				l1 = findPointByName(name1);
				l2 = findPointByName(name2);
				if(l1>=0&&l2>=0)
				{
					line_set[line_set_cnt][0] = l1;
					line_set[line_set_cnt][1] = l2;
					line_set_cnt++;
				}
				else
				{
					fprintf(stderr,"Error in line config\n");
				}
			}
			fclose(fp);
		}
	}
	if(lb_type==TYPE_ATTR)
	{
		memset(attr_val_cnt,0,sizeof(attr_val_cnt));
		point_val[0][0] = PADDING_X;
		point_val[0][1] = PADDING_Y;
		for(i=1;i<point_set_cnt;i++)
		{
			point_val[i][0] = PADDING_X;
			point_val[i][1] = PADDING_Y+point_val[i-1][1];
		}
		fp = fopen(ATTR_SET_FILE,"r");
		if(!fp)
		{
			fprintf(stderr,"Attr config file not found\n");
			exit(-1);
		}
		else
		{
			cnt=0;
			while(EOF != fscanf(fp,"%s%d",cmd,&nval))
			{
				idx = findPointByName(cmd);
				for(i=0;i<nval;i++)
				{
					fscanf(fp,"%s",cmd);
					if (idx >=0)
					{
						strcpy(attr_val_name[idx][i],cmd);
					}
				}
				if(idx>=0&&attr_val_cnt[idx]==0)
				{
					cnt ++;
					attr_val_cnt[idx] = nval;
				}
			}
			fclose(fp);
			if(cnt != point_set_cnt)
			{
				fprintf(stderr,"Errors in Attr config file\n");
				exit(-1);
			}
		}
	}
}

void initPointValue()
{
	int i;
	for(i=0;i<point_set_cnt;i++)
	{
		point_val[i][0] = -1;
		point_val[i][1] = -1;
	}
}
void initAttrValue()
{
	int i;
	for(i=0;i<point_set_cnt;i++)
	{
		attr_val[i] = -1;
	}
}
int findPointByName(char * name)
{
	int i;
	for(i=0;i<point_set_cnt;i++)
	{
		if(0==strcmp(point_set[i],name))
		{
			return i;
		}
	}
	return -1;
}

int findAttrValByName(int idx,char * name)
{
	int i;
	for(i=0;i<attr_val_cnt[idx];i++)
	{
		if(0==strcmp(attr_val_name[idx][i],name))
		{
			return i;
		}
	}
	return -1;
}

void loadDataFile(char * img_file)
{
	char cmd[STR_SIZE],attr[STR_SIZE];
	int x,y,idx,idxv;
	int i,j;
	int b,g,r;
	sprintf(cmd,"%s.dat",img_file);
	FILE * fp;
	fp = fopen(cmd,"r");

	if(lb_type==TYPE_PNT||lb_type==TYPE_REC)
	{
		initPointValue();
		if(!fp)
		{
			return;
		}
		while(EOF != fscanf(fp,"%s%d%d",cmd,&x,&y))
		{
			idx = findPointByName(cmd);
			if(idx<0) continue;
			point_val[idx][0]	 = x;
			point_val[idx][1] = y;
		}
	}
	if(lb_type==TYPE_ATTR)
	{
		initAttrValue();
		if(!fp)
		{
			return;
		}
		while(EOF != fscanf(fp,"%s%s",cmd,attr))
		{
			idx = findPointByName(cmd);
			idxv = findAttrValByName(idx,attr);
			attr_val[idx] = idxv;
		}
	}
	if(lb_type==TYPE_SP)
	{
		initSpValue();
		if(fp)
		{
			while(EOF != fscanf(fp,"%d%s",&x,cmd)) //x:superpixel index , y:label
			{
				y = findPointByName(cmd);
				sp_val[x-1] = y;
			}
		}
		sprintf(cmd,"%s.png",img_file);
		spim =  cvLoadImage(cmd,1);
		if(!spim)
		{
			sprintf(cmd,"the png file of %s not existed\n",img_file);
			fprintf(stderr,"%s",cmd);
			exit(-1);
		}
		if(spmat)
		{
			cvReleaseMat(&spmat);
		}
		spmat = cvCreateMat(spim->height,spim->width,CV_32FC1);
		for(i=0;i<spim->height;i++)
		{
			for(j=0;j<spim->width;j++)
			{
				b = (int)CV_IMAGE_ELEM(spim,uchar,i,3*j);
				g = (int)CV_IMAGE_ELEM(spim,uchar,i,3*j+1);
				r = (int)CV_IMAGE_ELEM(spim,uchar,i,3*j+2);
				cvmSet(spmat,i,j,r+256*g+256*256*b);
			}
		}
	}
	if(lb_type==TYPE_SQR)
	{
	     initSQRValue();
	}
	if(fp) fclose(fp);
}

void saveDataFile(char * img_file)
{
	char cmd[STR_SIZE];
	int i;
	sprintf(cmd,"%s.dat",img_file);
	FILE * fp;
	fp = fopen(cmd,"w");
	if(lb_type==TYPE_REC||lb_type==TYPE_PNT)
	{
		for(i=0;i<point_set_cnt;i++)
		{
			fprintf(fp,"%s %d %d\n",point_set[i],point_val[i][0],point_val[i][1]);
		}
	}
	if(lb_type==TYPE_SP)
	{
		for(i=0;i<MAX_POINT;i++)
		{
			if(sp_val[i]>=0)
			{
				fprintf(fp,"%d %s\n",i+1,point_set[sp_val[i]]);
			}
		}
	}
	if(lb_type==TYPE_ATTR)
	{
		for(i=0;i<point_set_cnt;i++)
		{
			fprintf(fp,"%s %s\n",point_set[i],attr_val_name[i][attr_val[i]]);
		}
	}
	fclose(fp);
}

void saveSQR()
{
	int S;
	if(point_val[0][0]<0||point_val[1][0]<0||cur_point_ptr!=0) return;
	S = abs(point_val[0][0]-point_val[1][0]);

	cvSetImageROI(im,cvRect(point_val[0][0],point_val[0][1],S,S));
	char sfn[STR_SIZE];
	sprintf(sfn,"./res/%d_%d.jpg",cur_file_ptr,rand()%10000);
	cvSaveImage(sfn,im);
	cvResetImageROI(im);
}

void initUI()
{
	cvNamedWindow(TOOL_NAME,CV_WINDOW_AUTOSIZE);
	cvSetMouseCallback(TOOL_NAME,mCallBack,NULL);	
	//cvShowImage(TOOL_NAME,im);
}

IplImage * showPoints(IplImage * img)
{
	int i,x1,y1,x2,y2;
	CvFont font; 
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 0.5,0.5,0,1);

	for(i=0;i<line_set_cnt;i++)
	{
		x1 = point_val[line_set[i][0]][0];
		y1 = point_val[line_set[i][0]][1];
		x2 = point_val[line_set[i][1]][0];
		y2 = point_val[line_set[i][1]][1];
		
		if(x1>=0&&x2>=0)
		{
			cvLine(showim,cvPoint(x1,y1),cvPoint(x2,y2),CV_RGB(100,100,100),POINT_RADIUS);
		}
	}

	for(i=0;i<point_set_cnt;i++)
	{
		if(point_val[i][0]>=0)
		{
			cvCircle(img,cvPoint(point_val[i][0],point_val[i][1]),POINT_RADIUS,cvScalar(0,0,255),-1);
			cvPutText(img,point_set[i],cvPoint(point_val[i][0]+POINT_RADIUS,point_val[i][1]),&font,CV_RGB(100,200,255));
		}
	}

	return img;
}

IplImage * showRecs(IplImage * img)
{
	int i;
	for(i=1;i<point_set_cnt;i+=2)
	{
		if(point_val[i][0]>=0&&point_val[i-1][0]>=0)
		{
			cvRectangle(img,cvPoint(point_val[i-1][0],point_val[i-1][1]),cvPoint(point_val[i][0],point_val[i][1]),cvScalar(0,0,255),LINE_THICK);
		}
	}
	img = showPoints(img);
	return img;
}

IplImage * showSQR(IplImage * img)
{
	int i,x1,y1,x2,y2,S;
	x1 = point_val[0][0];
	y1 = point_val[0][1];
	x2 = point_val[1][0];
	y2 = point_val[1][1];
	
	if(cur_point_ptr != 0||x1<0||x2<0)
	{
	    return img;
	}
	
	S = abs(x2-x1);
	cvRectangle(img,cvPoint(x1,y1),cvPoint(x1+S,y1+S),cvScalar(0,0,255),LINE_THICK);

	return img;
}

IplImage * showSp(IplImage * img)
{
	int i,j;
	for(i=0;i<img->height;i++)
	{
		for(j=0;j<img->width;j++)
		{
			if(sp_val[(int)cvmGet(spmat,i,j)-1]==cur_point_ptr)
			{
				CV_IMAGE_ELEM(img,uchar,i,j*3+1) = 150;
				CV_IMAGE_ELEM(img,uchar,i,j*3+2) = 250;
			}
		}
	}
	return img;
}

IplImage * showAttr(IplImage * img)
{
	char cmd[STR_SIZE];
	int i;
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 0.6,0.6,0,2);
	
	for(i=0;i<point_set_cnt;i++)
	{
		cvCircle(img,cvPoint(point_val[i][0],point_val[i][1]),POINT_RADIUS,cvScalar(0,0,255),-1);
		if(attr_val[i]<0)
		{
			sprintf(cmd,"%s : not labeled",point_set[i]);	
		}
		else
		{
			sprintf(cmd,"%s : %s",point_set[i],attr_val_name[i][attr_val[i]]);
		}
		cvPutText(img,cmd,cvPoint(point_val[i][0]+PADDING_X,point_val[i][1]),&font,CV_RGB(90,0,255));
	}
	return img;
}

void showImage()
{
	if(showim)
	{
		cvReleaseImage(&showim);
	}
	showim = cvCloneImage(im);
	switch(lb_type)
	{
		case TYPE_REC :
			showim = showRecs(showim);
			break;
		case TYPE_PNT:
			showim = showPoints(showim);
			break;
		case TYPE_SP:
			showim = showSp(showim);
			break;
		case TYPE_ATTR:
			showim = showAttr(showim);
			break;
		case TYPE_SQR:
			showim = showSQR(showim);
			break;
	}

	if(lb_type==TYPE_REC||lb_type==TYPE_PNT||lb_type==TYPE_ATTR)
	{
		int x = point_val[cur_point_ptr][0];
		int y = point_val[cur_point_ptr][1];
		if(x>=0&&y>=0)
		{
			cvCircle(showim,cvPoint(x,y),POINT_RADIUS+1,cvScalar(0,255,255),-1);
		}
	}
	cvShowImage(TOOL_NAME,showim);
}

void curImage(){printf("Now : %s id=%d\n",file_set[cur_file_ptr],cur_file_ptr);}
void curPoint(){printf("Point : %s\n",point_set[cur_point_ptr]);}
void exitLBT()
{
	int i;
	for(i=0;i<file_set_cnt;i++)
	{
		free(file_set[i]);
	}
	exit(0);
}
void mCallBack(int event,int x,int y, int flags, void * userdata)
{
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		printf("x:%d y:%d\n",x,y);
		if(lb_type==TYPE_REC||lb_type==TYPE_PNT||lb_type==TYPE_SQR)
		{
			point_val[cur_point_ptr][0] = x;
			point_val[cur_point_ptr][1] = y;
			if(cur_point_ptr==1&&lb_type==TYPE_SQR)
			{
				cur_point_ptr = -1;    
			}
			nextPoint();
		}
		if(lb_type==TYPE_SP)
		{
			sp_val[(int)cvmGet(spmat,y,x)-1] = cur_point_ptr;
		}
		showImage();
	}
	if(event == CV_EVENT_RBUTTONDOWN)
	{
		if(lb_type==TYPE_SP)
		{
			sp_val[(int)cvmGet(spmat,y,x)-1] = -1;
		}
		showImage();
	}
}

void loadNext()
{
	if((cur_file_ptr+1)==file_set_cnt)
	{
		printf("Reach the last.\n");
		return;
	}
	clearCur();
	cur_file_ptr ++;
	im = cvLoadImage(file_set[cur_file_ptr],1);
	loadDataFile(file_set[cur_file_ptr]);
	cur_point_ptr =0 ;
	showImage();
	curImage();
}
void loadPrevious()
{
	if((cur_file_ptr)==0)
	{
		printf("Reach the front.\n");
		return;
	}
	clearCur();
	cur_file_ptr --;
	im = cvLoadImage(file_set[cur_file_ptr],1);
	loadDataFile(file_set[cur_file_ptr]);
	cur_point_ptr = 0;
	showImage();	
	curImage();
}

void nextPoint()
{
	if(cur_point_ptr==(point_set_cnt-1))
	{
		printf("the last point\n");
		return;
	}
	cur_point_ptr++;
	curPoint();
}

void previousPoint()
{
	if(cur_point_ptr==0)
	{
		printf("the first point\n");
		return;
	}
	cur_point_ptr--;
	curPoint();
}

void previousAttrVal()
{
	attr_val[cur_point_ptr] -- ;
	if(attr_val[cur_point_ptr]<0)
	{
		attr_val[cur_point_ptr] = attr_val_cnt[cur_point_ptr]-1;
	}
}

void nextAttrVal()
{
	attr_val[cur_point_ptr] ++;
	if(attr_val[cur_point_ptr]==attr_val_cnt[cur_point_ptr])
	{
		attr_val[cur_point_ptr] = 0;
	}
}

void kCallBack(char ch)
{
	switch(ch)
	{
		case ' ':
			saveDataFile(file_set[cur_file_ptr]);
			loadNext();
			break;
		case '\n':
			saveDataFile(file_set[cur_file_ptr]);
			loadPrevious();
			break;
		case 'p':
			loadPrevious();
			break;
		case 'q':
			exitLBT();
			break;
		case 'n':
			loadNext();
			break;
		case '=':
			nextPoint();
			showImage();
			break;
		case '-':
			previousPoint();
			showImage();
			break;
		case '[':
			previousAttrVal();
			showImage();
			break;
		case ']':
			nextAttrVal();
			showImage();
			break;
		case 'c':
			if(lb_type==TYPE_SQR)
			{
		        	saveSQR();
			}
	}
}
