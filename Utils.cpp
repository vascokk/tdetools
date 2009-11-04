/*
 * Utils.cpp
 *
 *  Created on: 2009-06-30
 *      Author: jfrank8
 */

#include <stdlib.h>
#include <cstring>
#include <ctype.h>
#include <iostream>

#include <ANN/ANN.h>
#include <Tisean/tsa.h>
#include <opencv/cxcore.h>
#include "Utils.h"

using namespace std;

void get_embedding(Settings* settings, double* &data, ulong &length) {
    ulong i;
    uint j,k;
    uint alldim, maxemb, emb, rundel, delsum;
    uint *inddelay;
    uint *formatlist;
	double** series;

    check_alloc(formatlist=(uint*)malloc(sizeof(int)*settings->indim));
    for (i=0;i<settings->indim;i++) {
        formatlist[i]=settings->embdim/settings->indim;
    }
    alldim=0;
    for (i=0;i<settings->indim;i++)
        alldim += formatlist[i];
    check_alloc(inddelay=(uint*)malloc(sizeof(int)*alldim));

    rundel=0;
    for (i=0;i<settings->indim;i++) {
        delsum=0;
        inddelay[rundel++]=delsum;
        for (j=1;j<formatlist[i];j++) {
            delsum += settings->delay;
            inddelay[rundel++]=delsum;
        }
    }
    maxemb=0;
    for (i=0;i<alldim;i++)
        maxemb=(maxemb<inddelay[i])?inddelay[i]:maxemb;

    if (settings->column == NULL) {
        series=get_multi_series(settings->infile,&settings->length,settings->exclude,&settings->indim,(char*)"",settings->dimset,settings->verbosity);
    } else {
        series=get_multi_series(settings->infile,&settings->length,settings->exclude,&settings->indim,settings->column,settings->dimset,settings->verbosity);
    }
    cerr << "Length: " << settings->length << endl << "Embed Dim: " << settings->embdim << endl;

    check_alloc(data = (double*)calloc((settings->length-maxemb)*settings->embdim,sizeof(double)));
    uint step = settings->embdim;
    for (i=maxemb;i<settings->length;i++) {
        rundel=0;
        for (j=0;j<settings->indim;j++) {
            emb=formatlist[j];
            for (k=0;k<emb;k++)
                data[(i-maxemb)*step+(emb-k-1)] = series[j][i-inddelay[rundel++]];
        }
    }
    length = settings->length - maxemb;
}

void get_ann_points(ANNpointArray &dataPts, double* series, unsigned long  rows, unsigned long  cols)
{
	ulong k = 0;
    dataPts = annAllocPts(rows, cols);
    for (ulong i = 0; i < rows; i++) {
        for (ulong j = 0; j < cols; j++) {
        	dataPts[i][j] = series[k++];
        }
    }
}

void print_matrix(CvMat* matrix) {
	int i,j;
	for (i = 0; i < matrix->rows; i++) {
		cout << CV_MAT_ELEM(*matrix, double, i, 0);
		for (j = 1; j < matrix->cols; j++) {
			cout << " " << CV_MAT_ELEM(*matrix, double, i, j);
		}
		cout << endl;
	}
}


