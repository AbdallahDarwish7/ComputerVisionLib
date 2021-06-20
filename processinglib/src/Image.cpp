//
// Created by abdallah on 3/31/21.
//

#include "processinglib/Image.h"

#define STB_IMAGE_IMPLEMENTATION

#include "processinglib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "processinglib/stb_image_write.h"
#include "iostream"
Image::Image() {};

Image::Image(int width, int height, int channel) : width{width}, height{height}, channels{channel}, data{nullptr} {
    data = new float[height * width * channels];
    init(0.0);
}

Image::Image(std::string filename, int channels) {
    int channelsInImage = 0;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channelsInImage, channels);
    if (!data) {
        fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n",
                filename.c_str(), stbi_failure_reason());
        exit(0);
    }
    if(channelsInImage < channels){
        this->channels = channelsInImage;
    } else {
        this->channels = channels;
    }
    this->data = new float[height * width * this->channels];
    init(data);
    free(data);
}

Image::Image(const Image &image) : Image{image.width, image.height, image.channels} {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < channels; ++k) {
                (*this)(i, j, k) = image(i, j, k);
            }
        }
    }
}


Image::Image(const Image *image) : Image{image->width, image->height, image->channels} {
    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            for (int k = 0; k < image->channels; ++k) {
                (*this)(i, j, k) = (*image)(i, j, k);
            }
        }
    }
}


void Image::init(float value) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < channels; ++k) {
                (*this)(i, j, k) = value;
            }
        }
    }
}


Image Image::toScale() {
    Image scaleImg(width, height, channels);
    float *max = new float[channels];
    for (int k = 0; k < channels; ++k) {
        max[k] = 0;
    }
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < channels; ++k) {
                scaleImg(i, j, k) = std::abs((*this)(i, j, k));
                if (scaleImg(i, j, k) > max[k]) {
                    max[k] = scaleImg(i, j, k);
                }

            }
        }
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < channels; ++k) {
                scaleImg(i, j, k) = roundf((scaleImg(i, j, k) / max[k]) * 255);
            }
        }
    }

    delete[] max;
    return scaleImg;
}

Image Image::toGrayscale() {
    Image grayImg = Image(width, height, 1);
    if (channels > 1) {
        float avg = 0;
        float weights[] = {0.299, 0.587, 0.114};
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                avg = 0;
                for (int k = 0; k < channels; ++k) {
                    avg += weights[k] * (*this)(i, j, k);
                }
                grayImg(i, j, 0) = (unsigned char) roundf(avg);
            }
        }
    } else {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                grayImg(i, j) = (*this)(i, j);
            }
        }
    }
    return grayImg;
}

int Image::size() {
    return channels * width * height;
}
std::vector<float> Image::RGB2XYZ(float R, float G, float B) {

    std::vector<float> RGB {( R / 255.f), ( G / 255.f ), ( B / 255.f )};
    for(int loop = 0; loop < 3; loop++){
        if ( RGB[loop] > 0.04045 ) {
            RGB[loop] = pow(((RGB[loop] + 0.055) / 1.055) , 2.4);
        }else {
            RGB[loop] = RGB[loop] / 12.92;
        }
        RGB[loop] = RGB[loop] * 100;
    }

    float X = RGB[0] * 0.4124 + RGB[1] * 0.3576 + RGB[2] * 0.1805;
    float Y = RGB[0] * 0.2126 + RGB[1] * 0.7152 + RGB[2] * 0.0722;
    float Z = RGB[0] * 0.0193 + RGB[1] * 0.1192 + RGB[2] * 0.9505;
    std::vector<float> XYZ {X, Y, Z };
    return XYZ;
}
std::vector<float> Image::XYZ2RGB(float X, float Y, float Z) {

    std::vector<float> XYZ { X/100.F, Y/100.F, Z/100};
    std::vector<float> RGB = std::vector<float>(3);
    RGB[0] = XYZ[0] *  3.2406 + XYZ[1] * -1.5372 + XYZ[2] * -0.4986;
    RGB[1] = XYZ[0] * -0.9689 + XYZ[1] *  1.8758 + XYZ[2] *  0.0415;
    RGB[2] = XYZ[0] *  0.0557 + XYZ[1] * -0.2040 + XYZ[2] *  1.0570;

    for(int loop = 0; loop < 3; loop++){
        if ( RGB[loop] > 0.0031308 ) {
            RGB[loop] = 1.055 * pow(RGB[loop] , (1 / 2.4)) - 0.055;
        }else {
            RGB[loop] = 12.92 * RGB[loop];
        }
        RGB[loop] = RGB[loop] * 255;
    }
    return RGB;
}

std::vector<float> Image::XYZ2LUV(float X, float Y, float Z) {
    float var_U = ( 4 * X ) / ( X + ( 15 * Y ) + ( 3 * Z ) );
    float var_V = ( 9 * Y ) / ( X + ( 15 * Y ) + ( 3 * Z ) );

    float var_Y = Y / 100.f;
    if ( var_Y > 0.008856 ) {
        var_Y = pow(var_Y , 0.3333334);
    }else {
        var_Y = (7.787 * var_Y) + (16 / 116.f);
    }
    float XR = 95.047;
    float YR = 100;
    float ZR = 108.883;
    float ref_U = ( 4 * XR ) / (float ) ( XR + ( 15 * YR ) + ( 3 * ZR ) );
    float ref_V = ( 9 * YR ) / (float )( XR + ( 15 * YR ) + ( 3 * ZR ) );

    float L = ( 116 * var_Y ) - 16;
    float U = 13 * L * ( var_U - ref_U );
    float V = 13 * L * ( var_V - ref_V );

    std::vector<float> LUV {L, U, V};
    return LUV;
}

std::vector<float> Image::LUV2XYZ(float L, float U, float V) {
    float var_Y = ( L+ 16 ) / 116.F;
    if ( pow(var_Y, 3)  > 0.008856 ) {
        var_Y = pow(var_Y , 3);
    } else{
        var_Y = ( (var_Y - 16) / 116.F ) / 7.787;
    }

    float XR = 95.047;
    float YR = 100;
    float ZR = 108.883;
    float ref_U = ( 4 * XR ) / ( XR + ( 15 * YR ) + ( 3 * ZR ) );
    float ref_V = ( 9 * YR ) / ( XR + ( 15 * YR ) + ( 3 * ZR ) );

    float var_U = (U / ( 13.F * L)) + ref_U;
    float var_V = (V / ( 13.F * L)) + ref_V;

    float Y = var_Y * 100;
    float X =  - ( 9 * Y * var_U ) / (float)( (( var_U - 4 ) * var_V) - (var_U * var_V) );
    float Z = ( 9 * Y - ( 15 * var_V * Y ) - ( var_V * X ) ) / (float)( 3 * var_V );
    std::vector<float> XYZ {X, Y, Z};
    return XYZ;
}
Image Image::RGB2LUV() {

    Image LUVImage = this->toScale();

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            std::vector<float> XYZ = this->RGB2XYZ(LUVImage(i, j, 0), LUVImage(i, j, 1), LUVImage(i, j, 2));
            std::vector<float> LUV = this->XYZ2LUV(XYZ[0], XYZ[1], XYZ[2]);
            for(int k =0; k < 3; k++){
                LUVImage(i, j, k) = LUV[k];
            }

        }
    }
    return LUVImage;
}
Image Image::LUV2RGB() {

    Image RGBImgae(this);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::vector<float> XYZ = this->LUV2XYZ(RGBImgae(i, j, 0), RGBImgae(i, j, 1), RGBImgae(i, j, 2));
            std::vector<float> RGB = this->XYZ2RGB(XYZ[0], XYZ[1], XYZ[2]);
            for(int k =0; k < channels; k++){
                RGBImgae(i, j, k) = RGB[k];
            }
        }
    }

    return RGBImgae;
}

void Image::saveJPG(std::string filename) {
    char buff[256];
    sprintf(buff, "%s.jpg", filename.c_str());
    unsigned char *data_out = new unsigned char[width * height * channels];
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < channels; ++k) {
                data_out[k + j * channels + i * width] = (int) roundf((*this)(i, j, k));
            }
        }
    }
//    int success = stbi_write_png(buff, width, _width, channels, data_out, width*channels);
    int success = stbi_write_jpg(buff, width, height, channels, data_out, 100);
    if (!success) fprintf(stderr, "Failed to write image %s\n", buff);
    free(data_out);
}


float &Image::operator[](int index) const {
    return data[index];
}


float &Image::operator()(int i, int j) const {
    if (channels > 1)
        throw "Image is three channels";
    return data[i * width + j];
}


float &Image::operator()(int i, int j, int k) const {
    if (channels == 1 && k > 0)
        throw "Image is one channel";
    return data[k * height * width + i * width + j];
}

Image::~Image() {
    delete[] data;
}

std::vector<float> Image::ImageAsVector(void){
    std::vector<float> ImgVector;
    for (int i = 0; i < (this->width * this->height * this->channels); ++i) {
       ImgVector.push_back(this->data[i]);
    }
    return ImgVector;
}