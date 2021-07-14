#ifndef CONFIGFILEHANDLER
#define CONFIGFILEHANDLER

namespace cfg
{
    struct CalibrationConfig
    {

    public:
        int numberOfFrames;
        int boardWidth;
        int boardHeight;
        int squareSize;
        std::string outputFilename;
        std::string filepathToFrames;

    public:
        CalibrationConfig() : numberOfFrames(20), boardWidth(8), boardHeight(6), squareSize(20), outputFilename("CameraConfigurationData.xml"), filepathToFrames("frames/") {}
        explicit CalibrationConfig(int) : numberOfFrames(20), boardWidth(8), boardHeight(6), squareSize(20), outputFilename("CameraConfigurationData.xml"), filepathToFrames("frames/"){}

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
        out << "boardWidth = " << data.boardWidth << ", ";
        out << "boardHeight = " << data.boardHeight << ", ";
        out << "squareSize = " << data.squareSize << ", ";
        out << "outputFileName = " << data.outputFilename << ", ";
        out << "filepathToFrames = " << data.filepathToFrames << " }";
        return out;
    }

    static void createDefaultConfig()
    {
        cv::FileStorage storage("CalibrationConfig.xml", cv::FileStorage::WRITE);
    
        CalibrationConfig config(1);

        storage << "Config" << config;

        storage.release();
    }

    static CalibrationConfig readConfig()
    {
        cv::FileStorage storage("CalibrationConfig.xml", cv::FileStorage::READ);

        if (!storage.isOpened())
        {
            std::cerr << "Failed to open the config file! FAIL" << std::endl;
        }

        CalibrationConfig config;

        storage["Config"] >> config;

        return config;
    }
}
#endif