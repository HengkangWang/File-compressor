// huffman.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct node // node
{
	unsigned char ch; //character
	long long wgt; // weight
	node* pa; // parent node
	node* lc; // left child node
	node* rc; // right child node
	char code[256]; // code
}node;

typedef struct nodee
{
	char code[256];
}nodee;

int search(char* temp, nodee* root) // search code
{
	for (int i = 0; i < 256;++i)
	{
		if (strcmp(temp, (root+i)->code) == 0 && strlen(temp) > 0)
			return i;
	}
	return -1;
}

void transfer(unsigned char c, char* temp) // 10 to 2
{
	unsigned char ch = c,t;
	strcpy(temp, "");
	for (int i = 7; i >= 0; --i)
	{
		t = (unsigned char)pow(2, i);
		if (ch / t == 1)
		{
			ch -= t;
			strcat(temp, "1");
		}
		else if(ch / t == 0)
		{
			strcat(temp, "0");
		}
	}
}

void createCode(node* root) // create huffman codes
{
	if (root == nullptr)
		return;
	if (root->pa != nullptr)
	{
		if (root == root->pa->lc)
		{
			strcpy(root->code , root->pa->code);
			strcat(root->code , "0");
		}
		else if (root == root->pa->rc)
		{
			strcpy(root->code , root->pa->code);
			strcat(root->code , "1");
		}
	}
	createCode(root->lc);
	createCode(root->rc);
}
void findTwo(node* ch, int* s1, int* s2) // find two smallest nodes
{
	int temp = -1, p = -1;
	for (int i = 0; i < 511; ++i)
	{
		if (((ch + i)->wgt < temp || temp == -1) && (ch + i)->pa == nullptr && (ch + i)->wgt != 0)
		{
			temp = (ch + i)->wgt;
			p = i;
		}
	}
	*s1 = p;
	temp = p = -1;
	for (int i = 0; i < 511; ++i)
	{
		if (((ch + i)->wgt < temp || temp == -1) && (ch + i)->pa == nullptr && i != *s1 && (ch + i)->wgt != 0)
		{
			temp = (ch + i)->wgt;
			p = i;
		}
	}
	*s2 = p;
}

void huffmanTree(node *ch , int* i) // create a huffman tree
{
	int s1, s2;
	while(1)
	{
		findTwo(ch, &s1, &s2);
		if (s1 == -1 || s2 == -1)
			break;
		(ch + 256 + *i)->lc = ch + s1;
		(ch + 256 + *i)->rc = ch + s2;
		(ch + 256 + *i)->wgt = (ch + s1)->wgt + (ch + s2)->wgt;
		(ch + s1)->pa = ch + 256 + *i;
		(ch + s2)->pa = ch + 256 + *i;
		++(*i);
	}
	--(*i);
}

void compressFile() // compress a file
{
	int j = 0, z = 0, zz;
	char name[50],namee[50], temp[500] = "",code[500]="";
	unsigned char c, ii, ch = 0;
	printf("\n\n          Please input the original file name:");
	rewind(stdin);
	gets_s(name,50);
	FILE* fp = fopen(name, "rb");
	if (fp == nullptr)
	{
		printf("\n          The file does not exit!");
		return;
	}
	printf("\n\n          Please input the decompressed file name:");
	rewind(stdin);
	gets_s(namee, 50);
	FILE* fpp = fopen(namee, "wb");
	node* root = new node[256 * 2 - 1];
	for (int i = 0; i < 511; ++i) // initiate nodes
	{
		if (i < 256)
			(root + i)->ch = i;
		(root + i)->wgt = 0;
		(root + i)->pa = nullptr;
		(root + i)->lc = nullptr;
		(root + i)->rc = nullptr;
		strcpy((root + i)->code, "");
	}
	while (1)
	{
		fread(&c, 1, 1, fp);
		if(!feof(fp))
			++((root + c)->wgt);
		else
			break;
	}
	huffmanTree(root, &j);
	createCode(root + 256 + j);
	fwrite(&c, 1, 1, fpp);
	for (int i = 0; i < 256; ++i)
	{
		c = strlen((root + i)->code);
		fwrite(&c,1,1,fpp);
	}
	for (int i = 0; i < 256; ++i)
	{
		fwrite((root + i)->code, 1, strlen((root + i)->code), fpp);
	}
	fp = fopen(name, "rb");
	while (1)
	{
		fread(&c, 1, 1, fp);
		if (feof(fp))
		{
			break;
		}
		strcat(temp, (root + c)->code);
		z = strlen(temp);
		while (z >= 8)
		{
			for (int jj = 0; jj < 8; ++jj)
			{
				ch += (unsigned char)pow(2, 7 - jj) * (temp[jj] - 48);
			}
			fwrite(&ch, 1, 1, fpp);
			ch = 0;
			if (z == 8)
				strcpy(temp, "");
			else
			{
				for (zz = 8; zz < z;++zz)
				{
					code[zz - 8] = temp[zz];
				}
				code[zz - 8] = 0;
				strcpy(temp, code);
			}
			z -= 8;
		}
	}
	if (strlen(temp) > 0)
	{
		for (ii = 0; ii < strlen(temp); ++ii)
		{
			ch += (unsigned char)pow(2, 7 - ii) * (temp[ii] - 48);
		}
		fwrite(&ch, 1, 1, fpp);
	}
	fseek(fpp, 0, 0);
	fwrite(&ii, 1, 1, fpp);
	fclose(fp);
	fclose(fpp);
	free(root);
	printf("\nThe compression has been completed!\n");
}
void decompressFile() // decompress a file
{
	char name[50],namee[50],length[257],buff[500] = "",temp[9]="";
	unsigned char c,cc,la,ttt;
	int pb = 0, pt = 0,tt;
	bool end = false;
	nodee* root = new nodee[256];
	printf("\n\n          Please input the decompressed file name:");
	rewind(stdin);
	gets_s(name, 50);
	FILE* fp = fopen(name, "rb");
	if (fp == nullptr)
	{
		printf("\n          The file does not exit!");
		return;
	}
	fread(&la, 1, 1, fp);
	for (int i = 0; i < 256; ++i)
	{
		fread(length + i, 1, 1, fp);
	}
	for (int i = 0,j; i < 256; ++i)
	{
		for (j = 0; j < length[i]; ++j)
		{
			fread((root+i)->code+j, 1, 1, fp);
		}
		*((root + i)->code + j) = '\0';
	}
	printf("\n\n          Please input the output file name:");
	rewind(stdin);
	gets_s(namee, 50);
	FILE* fpp = fopen(namee, "wb");
	while (1)
	{
		fread(&c, 1, 1, fp);
		fread(&cc, 1, 1, fp);
		if (feof(fp))
		{
			end = true;
		}
		fseek(fp, -1, 1);
		transfer(c, temp);
		if (end == true)
			*(temp + la) = '\0';
		while (1)
		{
			buff[pb++] = temp[pt++];
			buff[pb] = '\0';
			tt = search(buff, root);
			if (tt > -1)
			{
				ttt = (unsigned char)tt;
				fwrite(&ttt, 1, 1, fpp);
				pb = 0;
				strcat(buff, "");
			}
			if (pt == strlen(temp))
			{
				strcat(temp, "");
				pt = 0;
				break;
			}
		}
		if (end == true)
			break;
	}
	fclose(fp);
	fclose(fpp);
	free(root);
	printf("\nThe decompression has been completed!\n");
}
void compareFile() // compare two files
{
	char name[50], namee[50];
	unsigned char c, cc;
	printf("\n\n          Please input one file name you want to compare with:");
	rewind(stdin);
	gets_s(name, 50);
	FILE* fp = fopen(name, "rb");
	if (fp == nullptr)
	{
		printf("\n          The file does not exit!");
		return;
	}
	printf("\n\n          Please input another file name you want to compare with:");
	rewind(stdin);
	gets_s(namee, 50);
	FILE* fpp = fopen(namee, "rb");
	if (fpp == nullptr)
	{
		printf("\n          The file does not exit!");
		return;
	}
	while (1)
	{
		if (feof(fp) && feof(fpp))
		{
			printf("\nThe two files are the same!\n");
				break;
		}
		else if (feof(fp) && !feof(fpp))
		{
			printf("\nThe two files are not the same!\n");
			break;
		}
		else if (!feof(fp) && feof(fpp))
		{
			printf("\nThe two files are not the same!\n");
			break;
		}
		fread(&c, 1, 1, fp);
		fread(&cc, 1, 1, fpp);
		if (c != cc)
		{
			printf("\nThe two files are not the same!\n");
			break;
		}
	}
}
void myExit() // exit
{
	exit(0);
}

void init() // initiate the window
{
	int choice = -1;
	printf("\n\n\n                      Welcome to Use the Huffman Compression!");
	printf("\n\n\n\n");
	printf("      1 ~ Compress a file!\n");
	printf("      2 ~ Decompress a file!\n");
	printf("      3 ~ Compare two files!\n");
	printf("      4 ~ Exit!\n\n\n");
	printf("          Please input your choice(1~4):");
	rewind(stdin);
	scanf_s("%d", &choice,4);
	switch (choice)
	{
	case 1:
		compressFile();
		break;
	case 2:
		decompressFile();
		break;
	case 3:
		compareFile();
		break;
	case 4:
		myExit();
		break;
	default:
		printf("\n          Your input is incorrect!\n");
		exit(0);
		break;
	}
	system("PAUSE");
}

int main()
{
	init();
    return 0;
}

