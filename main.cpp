#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "processing.h"
using namespace std;

Header loadFile(string filename)
{
    // read in file
    ifstream file(filename, ios_base::binary);

    Header headerObject;
    file.read(&headerObject.idLength, 1);
    file.read(&headerObject.colorMapType, 1);
    file.read(&headerObject.dataTypeCode, 1);

    file.read((char*)&headerObject.colorMapOrigin, 2);
    file.read((char*)&headerObject.colorMapLength, 2);
    file.read(&headerObject.colorMapDepth, 1);

    file.read((char*)&headerObject.xOrigin, 2);
    file.read((char*)&headerObject.yOrigin, 2);

    file.read((char*)&headerObject.width, 2);
    file.read((char*)&headerObject.height, 2);

    file.read(&headerObject.bitsPerPixel, 1);
    file.read(&headerObject.imageDescriptor, 1);

    for(unsigned int i = 0; i < headerObject.width * headerObject.height; i++)
    {
        Pixel pixel;

        file.read((char*)&pixel.blue, 1);
        file.read((char*)&pixel.green, 1);
        file.read((char*)&pixel.red, 1);

        headerObject.image.push_back(pixel);

    }

    file.close();

    return headerObject;
}

void print(Header headerObject)
{
    // file header output
    cout << "ID Length: " << (int)headerObject.idLength << endl;
    cout << "Color Map Type: " << (int)headerObject.colorMapType << endl;
    cout << "Date Type Code: " << (int)headerObject.dataTypeCode << endl;

    cout << "Color Map Origin: " << headerObject.colorMapOrigin << endl;
    cout << "Color Map Length: " << headerObject.colorMapLength << endl;
    cout << "Color Map Depth: " << (int)headerObject.colorMapDepth << endl;

    cout << "X Origin: " << headerObject.xOrigin << endl;
    cout << "Y Origin: " << headerObject.yOrigin << endl;

    cout << "Width: " << headerObject.width << endl;
    cout << "Height: " << headerObject.height << endl;

    cout << "Bits Per Pixel: " << (int)headerObject.bitsPerPixel << endl;
    cout << "Image Description: " << (int)headerObject.imageDescriptor << endl;
}

void exportFile(string filename, Header headerObject)
{
    // read in file
    ofstream file(filename, ios::binary | ios::out);

    file.write(&headerObject.idLength, 1);
    file.write(&headerObject.colorMapType, 1);
    file.write(&headerObject.dataTypeCode, 1);

    file.write((char*)&headerObject.colorMapOrigin, 2);
    file.write((char*)&headerObject.colorMapLength, 2);
    file.write(&headerObject.colorMapDepth, 1);

    file.write((char*)&headerObject.xOrigin, 2);
    file.write((char*)&headerObject.yOrigin, 2);

    file.write((char*)&headerObject.width, 2);
    file.write((char*)&headerObject.height, 2);

    file.write(&headerObject.bitsPerPixel, 1);
    file.write(&headerObject.imageDescriptor, 1);

    for(unsigned int i = 0; i < headerObject.image.size(); i++)
    {
        Pixel pixel;
        file.write((char*)&headerObject.image.at(i).blue, 1);
        file.write((char*)&headerObject.image.at(i).green, 1);
        file.write((char*)&headerObject.image.at(i).red, 1);
    }

    file.close();
}

// dividing the original by the maximum
vector<NormalizedPixel> normalizePixels(vector<Pixel> pixels)
{
    const float max = 255;
    vector<NormalizedPixel> pixelVector;

    for(int i = 0; i < pixels.size(); i++)
    {
        NormalizedPixel pixel;

        pixel.blue = pixels.at(i).blue / max;
        pixel.green = pixels.at(i).green / max;
        pixel.red = pixels.at(i).red / max;

        pixelVector.push_back(pixel);
    }

    return pixelVector;
}

// multiplying the value by the maximum
vector<Pixel> revertPixel(vector<NormalizedPixel> pixels)
{
    const float max = 255;
    vector<Pixel> pixelVector;

    for(int i = 0; i < pixels.size(); i++)
    {
        Pixel pixel;

        pixel.blue = pixels.at(i).blue * max + 0.5f;
        pixel.green = pixels.at(i).green * max + 0.5f;
        pixel.red = pixels.at(i).red * max + 0.5f;

        pixelVector.push_back(pixel);
    }

    return pixelVector;
}

Header multiply(Header topLayer, Header bottomLayer)
{
    vector<NormalizedPixel> topPixels = normalizePixels(topLayer.image);
    vector<NormalizedPixel> bottomPixels = normalizePixels(bottomLayer.image);
    vector<NormalizedPixel> blendPixels;

    for(int i = 0; i < topPixels.size(); i++)
    {
        NormalizedPixel blend;
        blend.blue = topPixels.at(i).blue * bottomPixels.at(i).blue;
        blend.green = topPixels.at(i).green * bottomPixels.at(i).green;
        blend.red = topPixels.at(i).red * bottomPixels.at(i).red;

        blendPixels.push_back(blend);
    }

    Header finalImage = topLayer;
    finalImage.image = revertPixel(blendPixels);
    return finalImage;
}

void compareImages(Header blend, Header example, int testNumber)
{
    cout << "Test " << testNumber << "...... ";
    bool success = true;

    if(blend.idLength != example.idLength)
    {
        success = false;
    }

    else if(blend.colorMapType != example.colorMapType)
    {
        success = false;
    }

    else if(blend.dataTypeCode != example.dataTypeCode)
    {
        success = false;
    }

    else if(blend.colorMapOrigin != example.colorMapOrigin)
    {
        success = false;
    }

    else if(blend.colorMapLength != example.colorMapLength)
    {
        success = false;
    }

    else if(blend.colorMapDepth != example.colorMapDepth)
    {
        success = false;
    }

    else if(blend.xOrigin != example.xOrigin)
    {
        success = false;
    }

    else if(blend.yOrigin != example.yOrigin)
    {
        success = false;
    }

    else if(blend.height != example.height)
    {
        success = false;
    }

    else if(blend.width != example.width)
    {
        success = false;
    }

    else if(blend.bitsPerPixel != example.bitsPerPixel)
    {
        success = false;
    }

    else if(blend.imageDescriptor != example.imageDescriptor)
    {
        success = false;
    }

    for(int i = 0; i < blend.image.size(); i++)
    {
        if(blend.image.at(i).blue != example.image.at(i).blue)
        {
            success = false;
        }

        else if(blend.image.at(i).green != example.image.at(i).green)
        {
            success = false;
        }

        else if(blend.image.at(i).red != example.image.at(i).red)
        {
            success = false;
        }
    }
    if (success) {
        cout << "Passed!" << endl;
    }
    else {
        cout << "Failed!" << endl;
    }
}

Header subtract(Header topLayer, Header bottomLayer)
{
    vector<Pixel> blendPixels;

    for(int i = 0; i < topLayer.image.size(); i++)
    {
        Pixel blend;
        int temp;

        temp = topLayer.image.at(i).blue - bottomLayer.image.at(i).blue;
        if(temp < 0)
        {
            temp = 0;
        }

        blend.blue = temp;

        temp = topLayer.image.at(i).green - bottomLayer.image.at(i).green;
        if(temp < 0)
        {
            temp = 0;
        }

        blend.green = temp;

        temp = topLayer.image.at(i).red - bottomLayer.image.at(i).red;
        if(temp < 0)
        {
            temp = 0;
        }

        blend.red = temp;

        blendPixels.push_back(blend);
    }

    Header finalImage = topLayer;
    finalImage.image = blendPixels;
    return finalImage;
}

Header screen(Header topLayer, Header bottomLayer)
{
    vector<NormalizedPixel> topPixels = normalizePixels(topLayer.image);
    vector<NormalizedPixel> bottomPixels = normalizePixels(bottomLayer.image);
    vector<NormalizedPixel> blendPixels;

    for(int i = 0; i < topPixels.size(); i++)
    {
        NormalizedPixel blend;
        blend.blue = 1 - ((1 - topPixels.at(i).blue) * (1 - bottomPixels.at(i).blue));
        blend.green = 1 - ((1 - topPixels.at(i).green) * (1 - bottomPixels.at(i).green));
        blend.red =  1 - ((1 - topPixels.at(i).red) * (1 - bottomPixels.at(i).red));

        blendPixels.push_back(blend);
    }

    Header finalImage = topLayer;
    finalImage.image = revertPixel(blendPixels);
    return finalImage;
}

Header overlay(Header topLayer, Header bottomLayer)
{
    vector<NormalizedPixel> topPixels = normalizePixels(topLayer.image);
    vector<NormalizedPixel> bottomPixels = normalizePixels(bottomLayer.image);
    vector<NormalizedPixel> blendPixels;

    for(int i = 0; i < bottomPixels.size(); i++)
    {
        float tonalTotal = 0.0;
        float tonal = 0.0;

        tonalTotal += bottomPixels.at(i).blue;
        tonalTotal += bottomPixels.at(i).green;
        tonalTotal += bottomPixels.at(i).red;

        tonal = tonalTotal / 3.0;

        if(tonal <= 0.5f)
        {
                NormalizedPixel blend;
                blend.blue = 2 * topPixels.at(i).blue * bottomPixels.at(i).blue;
                blend.green = 2 * topPixels.at(i).green * bottomPixels.at(i).green;
                blend.red = 2 * topPixels.at(i).red * bottomPixels.at(i).red;

                blendPixels.push_back(blend);
        }

        else if(tonal > 0.5f)
        {
                NormalizedPixel blend;
                blend.blue = 1 - (2 * ((1 - topPixels.at(i).blue) * (1 - bottomPixels.at(i).blue)));
                blend.green = 1 - (2 * ((1 - topPixels.at(i).green) * (1 - bottomPixels.at(i).green)));
                blend.red =  1 - (2 * ((1 - topPixels.at(i).red) * (1 - bottomPixels.at(i).red)));

                blendPixels.push_back(blend);
        }
    }

    Header finalImage = topLayer;
    finalImage.image = revertPixel(blendPixels);
    return finalImage;
}

int main()
{
    // task 1
    Header objectHeader1 = loadFile("input/layer1.tga");
    Header objectHeader2 = loadFile("input/pattern1.tga");

    Header task1 = multiply(objectHeader1, objectHeader2);

    exportFile("output/part1.tga", task1);
    compareImages(task1, loadFile("examples/EXAMPLE_part1.tga"), 1);

    // task 2
    objectHeader1 = loadFile("input/layer2.tga");
    objectHeader2 = loadFile("input/car.tga");

    Header task2 = subtract(objectHeader2, objectHeader1);

    exportFile("output/part2.tga", task2);
    compareImages(task2, loadFile("examples/EXAMPLE_part2.tga"), 2);

    // task 3
    objectHeader1 = loadFile("input/layer1.tga");
    objectHeader2 = loadFile("input/pattern2.tga");

    Header task3a = multiply(objectHeader1, objectHeader2);

    Header objectHeader3 = loadFile("input/text.tga");
    Header task3b = screen(objectHeader3, task3a);

    exportFile("output/part3.tga", task3b);
    compareImages(task3b, loadFile("examples/EXAMPLE_part3.tga"), 3);

    // task 4
    objectHeader1 = loadFile("input/layer2.tga");
    objectHeader2 = loadFile("input/circles.tga");

    Header task4a = multiply(objectHeader1, objectHeader2);

    objectHeader3 = loadFile("input/pattern2.tga");
    Header task4b = subtract(task4a, objectHeader3);

    exportFile("output/part4.tga", task4b);
    compareImages(task4b, loadFile("examples/EXAMPLE_part4.tga"), 4);

    // task 5
    objectHeader1 = loadFile("input/layer1.tga");
    objectHeader2 = loadFile("input/pattern1.tga");

    Header task5 = overlay(objectHeader1, objectHeader2);

    exportFile("output/part5.tga", task5);
    compareImages(task5, loadFile("examples/EXAMPLE_part5.tga"), 5);

    // task 6
    objectHeader1 = loadFile("input/car.tga");

    vector<Pixel> greenAdd;

    for(int i = 0; i < objectHeader1.image.size(); i++)
    {
        Pixel pixelAdd;
        int temp;

        temp = 200 + objectHeader1.image.at(i).green;

        if(temp > 255)
        {
            temp = 255;
        }

        objectHeader1.image.at(i).green = temp;

    }

    Header task6 = objectHeader1;

    exportFile("output/part6.tga", task6);
    compareImages(task6, loadFile("examples/EXAMPLE_part6.tga"), 6);

    // task 7
    objectHeader1 = loadFile("input/car.tga");

    vector<Pixel> add;

    for(int i = 0; i < objectHeader1.image.size(); i++)
    {
        Pixel pixelAdd;
        int temp;

        temp = 4 * objectHeader1.image.at(i).red;

        if(temp > 255)
        {
            temp = 255;
        }

        objectHeader1.image.at(i).red = temp;

        temp = 0 * objectHeader1.image.at(i).blue;

        if(temp > 255)
        {
            temp = 255;
        }

        objectHeader1.image.at(i).blue = temp;

    }

    Header task7 = objectHeader1;

    exportFile("output/part7.tga", task7);
    compareImages(task7, loadFile("examples/EXAMPLE_part7.tga"), 7);

    // task 8
    objectHeader1 = loadFile("input/car.tga");
    Header task8r = objectHeader1;
    Header task8g = objectHeader1;
    Header task8b = objectHeader1;

    for(int i = 0; i < objectHeader1.image.size(); i++)
    {
        task8r.image.at(i).green = objectHeader1.image.at(i).red;
        task8r.image.at(i).blue = objectHeader1.image.at(i).red;

        task8g.image.at(i).red = objectHeader1.image.at(i).green;
        task8g.image.at(i).blue = objectHeader1.image.at(i).green;

        task8b.image.at(i).red = objectHeader1.image.at(i).blue;
        task8b.image.at(i).green = objectHeader1.image.at(i).blue;
    }

    exportFile("output/part8_r.tga", task8r);
    exportFile("output/part8_g.tga", task8g);
    exportFile("output/part8_b.tga", task8b);
    compareImages(task8r, loadFile("examples/EXAMPLE_part8_r.tga"), 8);
    compareImages(task8g, loadFile("examples/EXAMPLE_part8_g.tga"), 8);
    compareImages(task8b, loadFile("examples/EXAMPLE_part8_b.tga"), 8);

    // task 9
    objectHeader1 = loadFile("input/layer_red.tga");
    objectHeader2 = loadFile("input/layer_green.tga");
    objectHeader3 = loadFile("input/layer_blue.tga");

    vector<Pixel> combine;

    for(int i = 0; i < objectHeader1.image.size(); i++)
    {
        Pixel pixel;

        pixel.red = objectHeader1.image.at(i).red;
        pixel.green = objectHeader2.image.at(i).green;
        pixel.blue = objectHeader3.image.at(i).blue;

        combine.push_back(pixel);
    }

    Header task9 = objectHeader1;
    task9.image = combine;

    exportFile("output/part9.tga", task9);
    compareImages(task9, loadFile("examples/EXAMPLE_part9.tga"), 9);

    // task 10
    objectHeader1 = loadFile("input/text2.tga");

    int first = 0;
    int last = (objectHeader1.width * objectHeader1.height) - 1;

    for(int i = 0; i < objectHeader1.width * objectHeader1.height / 2; i++)
    {
        Pixel pixel;

        pixel = objectHeader1.image.at(first);
        objectHeader1.image.at(first) = objectHeader1.image.at(last);
        objectHeader1.image.at(last) = pixel;

        first++;
        last--;
    }

    Header task10 = objectHeader1;

    exportFile("output/part10.tga", task10);
    compareImages(task10, loadFile("examples/EXAMPLE_part10.tga"), 10);
}
