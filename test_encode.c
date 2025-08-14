#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>

int main(int argc,char*argv[])
{
    EncodeInfo encInfo;
    uint img_size;
    DecodeInfo decInfo;
    // Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";
   
    //check operation type
   if(check_operation_type(argv)==e_encode){
    printf("INFO: Selected Encoding....\n");


    //read and save to struct and display progress.
    if(read_and_validate_encode_args(argc,argv,&encInfo)==e_failure){
        printf("##Invalid arguments...\n");
    }
  
    // Test open_files
    if (open_files(&encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return 1;
    }


    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    encInfo.image_capacity=img_size;
    printf("INFO: Image size = %u\n", img_size);
    printf("INFO: DONE\n");
    printf("## Encoding process started.....\n");
    //fetching secret text file size
     if(fetchSecretSize(&encInfo)==e_success){
        printf("INFO: Done . Not empty \n");
     }
     else{
        printf("INFO: It dosent have any data \n");
        return 0;
     }
    //capcaity checking for name.bmp file
    printf("INFO: Cheking for %s capacity to handle %s\n",encInfo.src_image_fname,encInfo.secret_fname);
    if(check_capacity(&encInfo)==e_success){
        printf("INFO: Done found ok \n");
    }
    else{
        printf("ERROR: No enough space..\n");
        return 0;
    }
    printf("INFO: Copying image header \n");
    if(copy_bmp_header(encInfo.fptr_src_image,encInfo.fptr_stego_image,&encInfo)==e_success){
        printf("INFO: DONE \n");
    }
    else{
        return 0;
    }
    //encode magic string 
    printf("INFO: Encoding magic string Signature \n");
    if(encode_magic_string("*#",&encInfo)==e_success){
        printf("INFO: DONE \n");
    }
    else{
        return 0;
    }
    //encoding secret file ext
    printf("INFO: Encoding secret file extension \n");
    char *filexten=strstr(encInfo.secret_fname,".");
    if(encode_secret_file_extn((char*)filexten,&encInfo)==e_success){
        printf("INFO: Done \n");
    }
    else{
        return 0;
    }
    //encoding secret file size

    printf("INFO: Encoding secret file extension size \n");
    if(encode_secret_file_size_ext(strlen(filexten),&encInfo)==e_success){
        printf("INFO: DONE \n");
    }
    else{
        return 0;
    }
    //encoding secert file size size
    printf("INFO: Encoding secret file  size \n");
    if(encode_secret_file_size(encInfo.size_secret_file,&encInfo)==e_success){
        printf("INFO: DONE \n");
    }
    else{
        return 0;
    }

    //encoding secert data
    printf("INFO: Encoding secert file data \n");
    if(encode_secret_file_data(&encInfo)==e_success){
        printf("INFO: DONE \n");
    }
    else{
        return 0;
    }
    // cpy remaining data 
    printf("INFO: Copying left over data \n");
    encInfo.fptr_src_image=fopen(encInfo.src_image_fname,"rb");
    encInfo.fptr_stego_image=fopen(encInfo.stego_image_fname,"rb+");

    if(copy_remaining_img_data(encInfo.fptr_src_image,encInfo.fptr_stego_image)==e_success){
        printf("INFO: DONE \n");
    }
    else{
        return 0;
    }
   
   
   
   
   
   }




   else if(check_operation_type(argv)==e_decode){
    printf("INFO: Selcted Decoding...\n");
    printf("INFO: ## Decoding process started ## \n");

    //read and evaluate
    if(read_validate_de_args(argc,argv,&decInfo)==e_success){
    if(openFilesTodecode(&decInfo)==e_success){
        printf("INFO: Decoding magic string signature \n");
    }
    }

    decInfo.fptr_stago_img=fopen(decInfo.img_name,"rb");
    //decode the text data
    if(decode_magic_string(decInfo.fptr_stago_img,&decInfo)==e_success){
        printf("INFO: DONE \n");
    }
    else{
        return 0;
    }

    //output file name if not given;
   if(argc==3){

     printf("INFO: Decoding output file extension \n");
    if(decode_op_file_extn(decInfo.fptr_stago_img,&decInfo)==e_success){
        printf("INFO: DONE \n");
    }
    else{
        return 0;
    }
    printf("INFO: Output file not mentioned.Creating Decode%s as default \n",decInfo.secrertFileExt);
    snprintf(decInfo.decodedFilename,sizeof(decInfo.decodedFilename),"Decode%s", decInfo.secrertFileExt);
    decInfo.fptr_text_file=fopen(decInfo.decodedFilename,"w");

   }
else{
 decInfo.fptr_text_file=fopen(decInfo.decodedFilename,"w");
}

if(openDecodefile(&decInfo)==e_success){
 printf("INFO: Opened %s \n",decInfo.decodedFilename);
 printf("INFO: DONE. Opened all required files \n");
}
else{
    return 0;
}

printf("INFO: Decoding %s File size \n",decInfo.decodedFilename);
if(decode_op_file_size(decInfo.fptr_stago_img,&decInfo)==e_success){
    printf("INFO: DONE \n");
}
else{
    return 0;
}

//decode secert data
printf("INFO: Decoding secert data \n");
if(decode_secret_data(decInfo.fptr_stago_img,&decInfo)==e_success){
    printf("INFO: DONE \n");
    printf("INFO: ## Decoding Done Sucessfully ## \n");
}
else{
    return 0;
}
   }

   else{
    printf("Invalid option\nUsage:\nFor Encoding:./a.out -e beautiful.bmp secert.txt [stegano.bmp]\n For Decoding ./a.out -d stagano.bmp[data.txt]\n");
    return 0;
   }

 

}
