#ifndef CONFIGFILEHANDLER
#define CONFIGFILEHANDLER

namespace cfg
{
    struct CalibrationConfig
    {

    public:
        int numberOfFrames;
        std::string outputFilename;
        std::string filepathToFrames;

    public:
        CalibrationConfig() : numberOfFrames(10), outputFilename("CameraConfigurationData.xml"), filepathToFrames("frames/") {}
        explicit CalibrationConfig(int) : numberOfFrames(10), outputFilename("CameraConfigurationData.xml"), filepathToFrames("frames/"){}

        void write(cv::FileStorage& fs) const;

        void read(const cv::FileNode& node);

    };

    static void write(cv::FileStorage& fs, const std::string&, const CalibrationConfig& data)
    { 
        data.write(fs);
    }

    static void read(const cv::FileNode& node, CalibrationConfig& data, const CalibrationConfig& defaultValue)
    {
        if(node.empty()) data = defaultValue;
        else data.read(node);
    }
    
    static std::ostream& operator<<(std::ostream& out, const CalibrationConfig& data)
    {
        out << "{ numberOfFrames = " << data.numberOfFrames << ", ";
        out << "outputFileName = " << data.outputFilename << ", ";
        out << "filepathToFrames = " << data.filepathToFrames << " }";
        return out;
    }

    class ConfigHandler
    {
    public:
        void createDefaultConfig();
        CalibrationConfig readConfig();
    };
}

#endif