#include<stdio.h>
#ifndef DECODE_H
#define DECODE_H
#include "types.h" 

typedef struct _DecodeInfo
{
char magicstring[17];
FILE *fptr_stago_img;  
FILE *fptr_text_file;
char*img_name;
char*decodetextName;
char secrertFileExt[32];
unsigned int secretFileSize;
char secertFilData[1024];
char decodedFilename[100];

}DecodeInfo;

//function to read and validate args
Status read_validate_de_args(int argc,char*argv[],DecodeInfo*decInfo);


//functions to open files for decode
Status openFilesTodecode(DecodeInfo*decInfo);
Status openDecodefile(DecodeInfo*DecInfo);
//funtion to decode magic string
Status decode_magic_string(FILE*stagoimg,DecodeInfo*decInfo);

//decode the text data
Status decode_text_data(char imageBuffer[],int size,char dataImg[]);

//decode secert file extn
Status decode_op_file_extn(FILE*stagoimg,DecodeInfo*decInfo);


//function to write data

 //write data to file
 Status writeDatatoFIle(FILE*fptr,char image[]);

 //decode output file size

 Status decode_op_file_size(FILE*stagoimg,DecodeInfo*decInfo);
//decode the int data
Status decode_int_data(char [], int , int *);

//decode secert file datas


Status decode_secret_data(FILE*stagoimg,DecodeInfo*decInfo);


//create secert file size
Status writeSecDataTofile(char [],FILE*,int*);
#endif
