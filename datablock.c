#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"datablock.h"

DataBlock* DataBlock_new(int size){
	DataBlock* db;
	db = (DataBlock*)malloc(sizeof(size_t)+sizeof(char)*size);
	db->size=size;
	return db;
}

void DataBlock_del(DataBlock* db){
	free(db);
}

DataBlock* DataBlock_concat(DataBlock* A,DataBlock* B){
	DataBlock* C=DataBlock_new(A->size+B->size);
	memcpy(C->data,A->data,A->size);
	memcpy(C->data+A->size,B->data,B->size);
	return C;
}
