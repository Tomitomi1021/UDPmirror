typedef struct{
	size_t size;
	char data[];
} DataBlock;

DataBlock* DataBlock_new(int size);
void DataBlock_del(DataBlock* db);
DataBlock* DataBlock_concat(DataBlock* A,DataBlock* B);
