#ifndef Screenshot_h
#define Screenshot_h

class Screenshot {
private:
    bool stereo;

public:
    public Screenshot():stereo(false) {}

    int takeScreenshot(void) {

        int width = screenWidth;
        int height = screenHeight;

        int i,j;
        FILE *fptr;
        static int counter = 0; /* This supports animation sequences */
        char fname[32];
        unsigned char *image = (unsigned char*) malloc(3 * width * height * sizeof(unsigned char));
        /* Allocate our buffer for the image */
        if (image == NULL) {
            fprintf(stderr,"Failed to allocate memory for image\n");
            return 0;
        }

        glPixelStorei(GL_PACK_ALIGNMENT,1);

        /* Open the file */
        if (stereo)
        sprintf(fname,"L_%04d.jpg",counter);
        else
        sprintf(fname,"C_%04d.jpg",counter);
        if ((fptr = fopen(fname,"w")) == NULL) {
            fprintf(stderr,"Failed to open file for window dump\n");
            return 0;
        }

        /* Copy the image into our buffer */
        glReadBuffer(GL_BACK_LEFT);
        glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

        /* Write the raw file */
        /* fprintf(fptr,"P6\n%d %d\n255\n",width,height); for ppm */
        for (j=height-1;j>=0;j--) {
            for (i=0;i<width;i++) {
                fputc(image[3*j*width+3*i+0],fptr);
                fputc(image[3*j*width+3*i+1],fptr);
                fputc(image[3*j*width+3*i+2],fptr);
            }
        }
        fclose(fptr);

        if (stereo) {
            /* Open the file */
            sprintf(fname,"R_%04d.jpg",counter);
            if ((fptr = fopen(fname,"w")) == NULL) {
                fprintf(stderr,"Failed to open file for window dump\n");
                return 0;
            }

            /* Copy the image into our buffer */
            glReadBuffer(GL_BACK_RIGHT);
            glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

            /* Write the raw file */
            /* fprintf(fptr,"P6\n%d %d\n255\n",width,height); for ppm */
            for (j=height-1;j>=0;j--) {
                for (i=0;i<width;i++) {
                    fputc(image[3*j*width+3*i+0],fptr);
                    fputc(image[3*j*width+3*i+1],fptr);
                    fputc(image[3*j*width+3*i+2],fptr);
                }
            }
            fclose(fptr);
        }
        
        /* Clean up */
        counter++;
        free(image);
        return 1;
    }

};

#endif /* Screenshot_h */
