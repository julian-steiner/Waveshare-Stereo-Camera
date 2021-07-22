#ifndef IMAGE
#define IMAGE

class Image
{
public:
    void show(const std::string& windowname);
    void saveToFile(const std::string& folder);
    void fromFile(const std::string& folder, const std::string& filename);
};

#endif