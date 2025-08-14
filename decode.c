#include <stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>
// function wrt to decoding 

//function to read and evaluate args
Status read_validate_de_args(int argc,char*argv[],DecodeInfo*decInfo){
     if(argc>5){
          return e_failure;
     }
      if(strcmp(strstr(argv[2],"."),".bmp")==0){
        decInfo->img_name= argv[2];
      }
      else{
        printf("ERROR: Stago image %s format should be .bmp\n", argv[2]);
        return e_failure;
      }
      if(argc==4){
          strcpy(decInfo->decodedFilename,argv[3]);
      }
     
     return e_success;
}



//function to open files for decode

Status openFilesTodecode(DecodeInfo*decInfo){
     decInfo->fptr_stago_img=fopen(decInfo->img_name,"rb");
     if(decInfo->fptr_stago_img==NULL){
          perror("fopen");
    	     fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->img_name);

    	     return e_failure;
     }
     else{
          printf("INFO: Opened %s \n",decInfo->img_name);
          return e_success;
     }

     
}
Status openDecodefile(DecodeInfo*DecInfo){
DecInfo->fptr_text_file=fopen(DecInfo->decodedFilename,"w");
if(DecInfo->fptr_text_file==NULL){
     perror("fopen");
    	     fprintf(stderr, "ERROR: Unable to open file %s\n", DecInfo->decodedFilename);
    	     return e_failure;
}
else{
          return e_success;
}


}
//function to decode magic string 
Status decode_magic_string(FILE*stagoimg,DecodeInfo*decInfo){
     char imageBuffer[16];
     fseek(stagoimg,54,SEEK_SET);
    
     fread(imageBuffer,sizeof(char),16,stagoimg);

     if(decode_text_data(imageBuffer,16,decInfo->magicstring)==e_success){
       
        return e_success ; 
     }
     else{
          return e_failure;
     }
}

//decode text data
Status decode_text_data(char imageBuffer[], int size, char magicstring[]) {
    int i = 0;
    unsigned char num = 0;
    int j = 0;

    while (i < size) {
        num = 0; // reset for each character
        for (int bit = 0; bit < 8 && i < size; i++, bit++) {
            int mask = (imageBuffer[i] & 1) << bit;
            num |= mask;
        }
        magicstring[j++] = (char)num;
    }

    magicstring[j] = '\0';

    return e_success;
}

//decode secert file extension
Status decode_op_file_extn(FILE *stagoimg, DecodeInfo *decInfo) {
    char imageBuffer[32];

    fseek(stagoimg, 70, SEEK_SET);
    size_t bytesRead = fread(imageBuffer, sizeof(char), 32, stagoimg);
    if (bytesRead != 32) {
        return e_failure;
    }

    if (decode_text_data(imageBuffer, 32, decInfo->secrertFileExt) == e_success) {
        // printf("Extension of secret file: %s\n", decInfo->secrertFileExt);
        return e_success;
    }
    return e_failure;
}

//decode secert file size 
Status decode_int_data(char imageBuffer[], int size, int *fileSize) {
    unsigned int num = 0; 
    for (int j = 0; j < size; j++) {
        int bit = imageBuffer[j] & 1; 
        num |= (bit << j);           
    }
    *fileSize = num; 
    return e_success;
}

//decode secert file size
Status decode_op_file_size(FILE *stagoimg, DecodeInfo *decInfo) {
    char imageBuffer[sizeof(int) * 8];
    fseek(stagoimg, 134, SEEK_SET);
    fread(imageBuffer, sizeof(char), sizeof(int) * 8, stagoimg);

    if (decode_int_data(imageBuffer, sizeof(int)* 8, &(decInfo->secretFileSize)) == e_success) {
        return e_success;
    }
    return e_failure;
}

// Decode secret  data 
Status decode_secret_data(FILE *stegoimg, DecodeInfo *decInfo) {

    char imagebuffer[((decInfo->secretFileSize)*8)];
    fseek(stegoimg,166,SEEK_SET);

    int size=fread(imagebuffer,1,(decInfo->secretFileSize)*8,stegoimg);

     if(decode_text_data(imagebuffer,(decInfo->secretFileSize)*8,decInfo->secertFilData)==e_success){
        decInfo->fptr_text_file=fopen(decInfo->decodedFilename,"w+");
        writeSecDataTofile(decInfo->secertFilData,decInfo->fptr_text_file,&decInfo->secretFileSize);
        return e_success ; 
     }
     else{
          return e_failure;
     }
}

Status writeSecDataTofile(char imagebuffer[],FILE*decodefile,int*size)
{
fwrite(imagebuffer,1,*size,decodefile);
return e_success;

}