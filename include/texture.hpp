#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <fstream>
#include <vector>
#include <sstream>
#include "color.hpp"

using namespace std;

// Returns tokens after splitting input string with delimiter
vector<string> split(const string &input, const string &delimiter) {
    string inputCopy = input;
    vector<string> ans;
    int pos = 0;
    string token;
    while ((pos = inputCopy.find(delimiter)) != string::npos) {
        token = inputCopy.substr(0, pos);
        ans.emplace_back(token);
        inputCopy.erase(0, pos + delimiter.length());
    }
    ans.emplace_back(inputCopy);
    return ans;
}

class Texture {
    string filename;
    int width;
    int height;
    int pixelMax;

    // this is to easily print a given object to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Texture &);

public:
    vector<vector<Color>> pixels;

    Texture() : filename(""), width(0), height(0), pixelMax(0) {};

    Texture(const string &filename) : filename(filename), width(0), height(0), pixelMax(0) {}

    bool isValid() const {
        return width > 0 && height > 0;
    }

    bool parse() {
        // Opening texture file
        ifstream texturePPM(filename.c_str());
        if (texturePPM.fail()) {
            // Error handling: if file could not be opened
            cerr << "Texture file named \"" << filename
                 << "\" could not be opened. Maybe it doesn't exist or has insufficient permissions." << endl;
            return false;
        }
        cout << "Parsing texture file: " << filename << endl;
        string line;
        getline(texturePPM, line);
        istringstream heading(line);
        string temp;
        heading >> temp >> width >> height >> pixelMax;
        if (width <= 0 || height <= 0) {
            cerr << "Texture file named \"" << filename << "\" is invalid. Zero dimension image." << endl;
            return false;
        }
        // Initialize pixel array for output image
        for (int i = 0; i < width; i++) {
            vector<Color> col;
            col.resize(height);
            pixels.push_back(col);
        }
        vector<float> numbers;
        while (getline(texturePPM, line)) {
            string keyword;
            vector<string> lineSplit = split(line, " ");
            for (auto &token: lineSplit) {
                if (token.empty()) { continue; }
                try {
                    float floatToken = stof(token);
                    numbers.push_back(floatToken / pixelMax);
                } catch (exception &e) {
                    continue;
                }
            }
        }
        // Insufficient color information
        if (numbers.size() % 3 != 0) {
            cerr << "Texture file named \"" << filename << "\" is invalid. Insufficient color information." << endl;
            return false;
        }
        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                pixels[i][j] = Color(numbers[j * width * 3 + i * 3], numbers[j * width * 3 + i * 3 + 1],
                                     numbers[j * width * 3 + i * 3 + 2]);
            }
        }
        return true;
    }

    Color colorAt(const TextureCoordinates &textureCoordinates) const {
        int i = round(textureCoordinates.u * (width - 1));
        int j = round(textureCoordinates.v * (height - 1));
        return pixels[i][j];
    }
};


std::ostream &operator<<(std::ostream &out, const Texture &i) {
    out << "Texture:" << "\t" << i.width << "\t" << i.height << "\t" << i.pixelMax;
    return out;
}

#endif
