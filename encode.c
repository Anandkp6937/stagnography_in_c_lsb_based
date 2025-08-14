#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
/* Function Definitions */
void printStruct(EncodeInfo*encodeinfo);
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("INFO: width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("INFO: height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files...\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    else{
        printf("INFO: Opened beautiful.bmp \n");
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    else{
        printf("INFO: Opened %s \n",encInfo->secret_fname);
    } 

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    else{
          printf("INFO: Opened %s \n",encInfo->stego_image_fname);
          printf("INFO: DONE\n");
    }

    // No failure return e_success
    return e_success;
}

//operation type
  OperationType check_operation_type(char *argv[]){
    if(strcmp(argv[1],"-e")==0){
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0){
        return e_decode;
    }
    else{
        return e_unsupported;

    }
    }

//read and validate

Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo){
    if(argc==4 || argc==5){

      if(strcmp(strstr(argv[2],"."),".bmp")==0){
        encInfo->src_image_fname = argv[2];
      }
      else{
        printf("ERROR: Source image %s format should be .bmp\n", argv[2]);
        return e_failure;
      }
      
      if(strcmp(strstr(argv[3],"."),".txt")==0){
         encInfo->secret_fname = argv[3];
      }
      else{
         printf("ERROR: Secret file need to be .txt format,Not %s \n", argv[3]);
        return e_failure;
      }

        if(argc==5){
             encInfo->stego_image_fname=argv[4];
        }
        else{
            printf("INFO: Output file not mentioned,Creating stago.bmp as default.\n");
            encInfo->stego_image_fname="stago.bmp";
        }
    
        return e_success;
    }

    else{
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n");
	    printf("./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
        return e_failure;
    }
}


// fetch secret text size

Status fetchSecretSize(EncodeInfo *encInfo){
    printf("INFO: Checking for %s size\n",encInfo->secret_fname);

    int count=0;
    FILE*fptr=fopen(encInfo->secret_fname,"r");
    
    while(getc(fptr)!=EOF){
        count++;
    }
    if(count==0){
        return e_failure;
    }
    else{
    encInfo->size_secret_file=count;
    }
    return e_success;
}
//capacity check for secert text in .bmp image
Status check_capacity(EncodeInfo *encInfo){

    if(encInfo->image_capacity>(112*(encInfo->size_secret_file)*8)){
        return e_success;
    }
    return e_failure;
}


//copy image header to the file
Status copy_bmp_header(FILE*fptr_src_image, FILE *fptr_dest_image,EncodeInfo *encInfo){
    char hData[54];
    fptr_src_image=fopen(encInfo->src_image_fname,"rb");
    fptr_dest_image=fopen(encInfo->stego_image_fname,"wb");

    if(fptr_dest_image==NULL || fptr_dest_image==NULL){
        printf("ERROR:unable to open files..");
        return e_failure;
    }
    else{
    fread(hData,sizeof(char),54,fptr_src_image);
    fwrite(hData,sizeof(char),54,fptr_dest_image);
    return e_success;
    
    }
}
//encode magic string 
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){

    encInfo->fptr_src_image=fopen(encInfo->src_image_fname,"rb");
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"rb+");
     fseek(encInfo->fptr_stego_image,54,SEEK_SET);
     fseek(encInfo->fptr_src_image,54,SEEK_SET);
   Status res=encode_data_to_image((char*)magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    if(res==e_success){
        return e_success;
    }
    else{
        return e_failure;
    }
}

//encoding logic code

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image){
    char imagedata[size*8];
    char Magicdata[size];
    int i=0;
   
    if(fread(imagedata,1,(size*8),fptr_src_image)==(size*8)){
        memcpy(Magicdata,data,size);

    for (int k = 0; k < size; k++) {
        int charData = Magicdata[k];
        for (int j = 0; j < 8; i++, j++) {
            int bit = charData & 1;                 
            imagedata[i] = (imagedata[i] & 0xFE) | bit; 
            charData >>= 1;                         
        }
    }
   
    fwrite(imagedata,1,(size*8),fptr_stego_image);

    return e_success;
}
else{
    return e_failure;
}
}
//encode secretfile extension

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){
 encInfo->fptr_src_image=fopen(encInfo->src_image_fname,"rb");
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"rb+");
    fseek(encInfo->fptr_stego_image,70,SEEK_SET);
    fseek(encInfo->fptr_src_image,70,SEEK_SET);
   Status res=encode_data_to_image((char*)file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    if(res==e_success){
        return e_success;
    }
    else{
        return e_failure;
    }
}
//encode secert text size
Status encode_secret_file_size_ext(long file_size, EncodeInfo *encInfo){
   
    char imagedata[sizeof(int)*8];
    encInfo->fptr_src_image=fopen(encInfo->src_image_fname,"rb");
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"rb+");
     fseek(encInfo->fptr_stego_image,102,SEEK_SET);
       fseek(encInfo->fptr_src_image,102,SEEK_SET);
    fread(imagedata,1,(sizeof(int)*8),encInfo->fptr_src_image);
    Status res= encode_byte_to_lsb(file_size, imagedata);
    if(res==e_success){
          fwrite(imagedata,1,(sizeof(int)*8),encInfo->fptr_stego_image);
    return e_success;
    }
    else{
        return e_failure;
    }

}
//encdode the int types code
Status encode_byte_to_lsb(int data, char *image_buffer){
 
        for (int j = 0; j <(sizeof(int)*8); j++) {
            int bit = data & 1;                 
            image_buffer[j] = (image_buffer[j] & 0xFE) | bit; 
            data >>= 1;                         
        }
    return e_success;

}
//secert file size encoding
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo) {
    char imagedata[sizeof(int) * 8];
    fseek(encInfo->fptr_src_image, 134, SEEK_SET);
    fseek(encInfo->fptr_stego_image, 134, SEEK_SET);

    fread(imagedata, 1, sizeof(int) * 8, encInfo->fptr_src_image);
    if (encode_byte_to_lsb(file_size, imagedata) == e_success) {
        fwrite(imagedata, 1, sizeof(int) * 8, encInfo->fptr_stego_image);
        return e_success;
    }
    return e_failure;
}

// encode secert file data
Status encode_secret_file_data(EncodeInfo *encInfo){
    int num=encInfo->size_secret_file;
    char secertfiledata[num];
      encInfo->fptr_src_image=fopen(encInfo->src_image_fname,"rb");
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"rb+");
   fseek(encInfo->fptr_stego_image,166,SEEK_SET);
   fseek(encInfo->fptr_src_image,166,SEEK_SET);
   fread(secertfiledata,sizeof(char),num,encInfo->fptr_secret);
   Status res=encode_data_to_image((char*)secertfiledata,num,encInfo->fptr_src_image,encInfo->fptr_stego_image);
    if(res==e_success){
       
        return e_success;
    }
    else{
        return e_failure;
    }
}


//copy reamining data

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0) {
        if (fwrite(buffer, 1, bytes_read, fptr_dest) != bytes_read) {
            return e_failure; 
        }
    }

    if (ferror(fptr_src)) {
        return e_failure; // Read error
    }

    return e_success;
}




















