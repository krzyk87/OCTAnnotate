#include "calculate.h"
#include "functions.h"

#include <QDebug>
#include <QColor>

Calculate::Calculate(QObject *parent) :
    QObject(parent)
{

}

void Calculate::setObjectData(Scan *sData)
{
    this->scan = sData;
}

void Calculate::layersSegmentation()
{
    QImage image;

    Ort::Env env;
    Ort::RunOptions runOptions;
    Ort::Session session(nullptr);

    emit processingData(0, "Segmenting OCT data...");

//    constexpr int64_t batch_size = 1;
    constexpr int64_t numChannels = 1;
    constexpr int64_t height = 640;
    constexpr int64_t width = 384;
    constexpr int64_t numInputElements = numChannels * height * width;
    auto modelPath = L"..\\models\\model.onnx";

    session = Ort::Session(env, modelPath, Ort::SessionOptions{ nullptr });

    const std::vector<int64_t> inputShape = { 1, numChannels, height, width };
    const std::vector<int64_t> outputShape = { 1, 4, height, width };

    std::vector<float> input (numInputElements);
    std::vector<float> results (4*numInputElements);

    // TEST
    int bscansNumber = scan->getBscansNumber();
    for (int i = 0; i < bscansNumber; i++)
    {
        image = scan->getImage(i);
        image = image.convertToFormat(QImage::Format_Grayscale8);

        cv::Mat mat(image.height(), image.width(), CV_8UC1, image.bits(), image.bytesPerLine());

        const std::vector<float> imageVec = loadImageTest(mat, 384, 640);
        std::copy(imageVec.begin(), imageVec.end(), input.begin());
        qDebug() << "loaded image " << QString::number(i);

        auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
        auto inputTensor = Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), inputShape.data(), inputShape.size());
        auto outputTensor = Ort::Value::CreateTensor<float>(memory_info, results.data(), results.size(), outputShape.data(), outputShape.size());

        Ort::AllocatorWithDefaultOptions ort_alloc;
        Ort::AllocatedStringPtr inputName = session.GetInputNameAllocated(0, ort_alloc);
        Ort::AllocatedStringPtr outputName = session.GetOutputNameAllocated(0, ort_alloc);
        const std::array<const char*, 1> inputNames = { inputName.get() };
        const std::array<const char*, 1> outputNames = { outputName.get() };
        inputName.release();
        outputName.release();

        session.Run(runOptions, inputNames.data(), &inputTensor, 1, outputNames.data(), &outputTensor, 1);
        cv::Mat m = composeImage(results, numInputElements);

        cv::Mat layer_1, layer_2, layer_3;
        cv::threshold(m, layer_1, 50, 255, cv::THRESH_BINARY);
        cv::threshold(m, layer_2, 125, 255, cv::THRESH_BINARY);
        cv::threshold(m, layer_3, 200, 255, cv::THRESH_BINARY);
        cv::Mat edges_1, edges_2, edges_3;
        cv::Canny(layer_1, edges_1, 20, 100);
        cv::Canny(layer_2, edges_2, 20, 100);
        cv::Canny(layer_3, edges_3, 20, 100);

        QVector<double> layer_z;
        QVector<double> layer_x;
        LayerName layer = PCV;
        findPoints(edges_1, layer_z, layer_x, layer, i);
        layer_x.clear();
        layer_z.clear();
        layer = ILM;
        findPoints(edges_2, layer_z, layer_x, layer, i);
        layer_x.clear();
        layer_z.clear();
        layer = OB_RPE;
        findPoints(edges_3, layer_z, layer_x, layer, i);
        layer_x.clear();
        layer_z.clear();

        emit processingData((double)(i*100 / bscansNumber), "");
    }

    emit processingData(100, "Automatic segmentation completed.");

    /*
    cv::cvtColor(edges_1, edges_1, cv::COLOR_GRAY2BGR);
    cv::cvtColor(edges_2, edges_2, cv::COLOR_GRAY2BGR);
    cv::cvtColor(edges_3, edges_3, cv::COLOR_GRAY2BGR);
    cv::Mat red_1 = edges_1.mul(cv::Scalar(255, 0, 0), 1);
    cv::Mat red_2 = edges_2.mul(cv::Scalar(0, 255, 0), 1);
    cv::Mat red_3 = edges_3.mul(cv::Scalar(0, 0, 255), 1);
    cv::Mat result;
    cv::Mat m_color;
    cv::cvtColor(m, m_color, cv::COLOR_GRAY2RGB);
    m_color.copyTo(result);
    red_1.copyTo(result, edges_1);
    red_2.copyTo(result, edges_2);
    red_3.copyTo(result, edges_3);
    cv::imshow("No Copy 2", result);
    */

    emit segmentationFinished();
}

cv::Mat Calculate::composeImage(std::vector<float> &results, int64_t numInputElements)
{
    std::vector<uchar> thresholded_results(numInputElements);
    for (int i = 0; i < results.size() / 4; i++)
    {
        std::vector<float> temp{
        results[i],
        results[i + (results.size() / 4)],
        results[i + (2 * results.size() / 4)],
        results[i + (3 * results.size() / 4)]
        };
        int maxElementIndex = std::max_element(temp.begin(), temp.end()) - temp.begin();

        switch (maxElementIndex)
        {
        case 0:
            thresholded_results[i] = 0;
            break;
        case 1:
            thresholded_results[i] = 85;
            break;
        case 2:
            thresholded_results[i] = 170;
            break;
        case 3:
            thresholded_results[i] = 255;
            break;
        default:
            break;
        }
    }
    cv::Mat m = cv::Mat(640, 384, CV_8UC1);
    std::vector<uchar> doubleVec(thresholded_results.begin(), thresholded_results.end());
    std::memcpy(m.data, doubleVec.data(), doubleVec.size() * sizeof(uchar));
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(m, m, cv::MORPH_CLOSE, kernel);
    return m;
}

void Calculate::findPoints(cv::Mat edges, QVector<double> &z_data, QVector<double> &x_data, LayerName layer, int imageNumber)
{
    int rows = edges.rows;
    int cols = edges.cols;
    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            if (edges.at<uchar>(j, i) == 255)
            {
                z_data.push_back(rows - j);
                x_data.push_back(i);
                scan->setPoint(false, layer, imageNumber, i, j);
            }
        }
    }
}

void Calculate::imageEnhancement(QImage *img, float contrast, int brightness){

    // contrast enhancement
    int max = 0;    // TODO: wykorzystac OpenCV
    for (int i=0; i<img->width(); i++){
        for (int j=0; j<img->height(); j++){
            int value = QColor::fromRgb(img->pixel(i,j)).red();
            if (value > max)
                max = value;
        }
    }
    if (max == 0)
        max = 1;
    for (int i=0; i<img->width(); i++){
        for (int j=0; j<img->height(); j++){
            int value = QColor::fromRgb(img->pixel(i,j)).red();
            value = value * 255 / max * contrast + brightness;
            value = qBound(0,value,255);
            img->setColor(value,qRgb(value,value,value));
            img->setPixel(i,j,value);
        }
    }
}

QImage Calculate::calculateFundus(QList< QList< QList<int> > > octData)
{
    int scansNumber = octData.count();
    int scanHeight = octData[0].count();
    int scanWidth = octData[0][0].count();

    int maxSum = 1;
    QRgb rgbValue;
    QList< QList<int> > fundusTemp;
    fundusTemp.reserve(scansNumber);
    QList<int> fundusColumn;
    int sum, fundusValue;

    QImage fundus(scanWidth,scansNumber,QImage::Format_Indexed8);

    for (int y=0; y < scansNumber; y++){
        fundusColumn.clear();
        fundusColumn.reserve(scanWidth);

        for (int x=0; x < scanWidth; x++){

            sum = 0;
            for (int z=0; z < scanHeight; z++){
                sum += octData[y][z][x];
            }
            if (sum > maxSum){
                maxSum = sum;
            }
            fundusColumn.append(sum);
        }
        fundusTemp.append(fundusColumn);
    }
    for (int y=0; y <scansNumber; y++){
        for (int x=0; x < scanWidth; x++){
            fundusValue = fundusTemp[y][x]*255 / (maxSum);
            fundusValue = qBound(0,(int)fundusValue,255);
            rgbValue = qRgb(fundusValue,fundusValue,fundusValue);
            fundus.setColor(fundusValue,rgbValue);
            fundus.setPixel(x,y,fundusValue);
        }
    }
    return fundus;
}

